#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Audio.h>

#include "config.h"
#include "display.h"
#include "stations.h"
#include "web_ui.h"

// ─── Глобальные объекты ───────────────────────────────────────
Audio           audio;
DisplayManager  display;
StationManager  stationMgr;
AsyncWebServer  server(WEB_PORT);
AsyncWebSocket  ws("/ws");

// ─── Состояние ────────────────────────────────────────────────
struct AppState {
    bool    playing      = false;
    int     volume       = VOLUME_DEFAULT;
    int     currentIndex = -1;
    String  stationName  = "";
    String  trackTitle   = "";
};
AppState appState;

// ─── Прототипы ────────────────────────────────────────────────
void playStation(int index);
void stopAudio();
void broadcastState();
String buildStateJson();

// ─── Audio callbacks (ESP32-audioI2S) ─────────────────────────
void audio_info(const char* info) {
    Serial.printf("[audio] %s\n", info);
}

void audio_showstation(const char* info) {
    appState.stationName = String(info);
    Serial.printf("[station] %s\n", info);
    display.showPlaying(appState.stationName, appState.trackTitle,
                        appState.volume, appState.playing);
    broadcastState();
}

void audio_showstreamtitle(const char* info) {
    appState.trackTitle = String(info);
    Serial.printf("[track] %s\n", info);
    display.showPlaying(appState.stationName, appState.trackTitle,
                        appState.volume, appState.playing);
    broadcastState();
}

void audio_eof_stream(const char* info) {
    Serial.printf("[eof] %s\n", info);
    appState.playing = false;
    broadcastState();
}

// ─── WebSocket ────────────────────────────────────────────────
void onWsEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
               AwsEventType type, void* arg, uint8_t* data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        client->text(buildStateJson());
    }
}

// ─── Маршруты HTTP API ────────────────────────────────────────
void setupRoutes() {

    // Главная страница
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "text/html", WEB_UI);
    });

    // Статус системы + текущее состояние
    server.on("/status", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["playing"]      = appState.playing;
        doc["volume"]       = appState.volume;
        doc["currentIndex"] = appState.currentIndex;
        doc["station"]      = appState.stationName;
        doc["track"]        = appState.trackTitle;
        doc["ip"]           = WiFi.localIP().toString();
        doc["rssi"]         = WiFi.RSSI();
        doc["freeHeap"]     = ESP.getFreeHeap();
        doc["uptime"]       = (unsigned long)(millis() / 1000);
        String out;
        serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // Список станций
    server.on("/stations", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "application/json", stationMgr.toJson());
    });

    // Воспроизвести станцию по индексу
    server.on("/play", HTTP_GET, [](AsyncWebServerRequest* req) {
        if (req->hasParam("index")) {
            int idx = req->getParam("index")->value().toInt();
            playStation(idx);
        } else if (appState.currentIndex >= 0) {
            playStation(appState.currentIndex);
        } else {
            playStation(0);
        }
        req->send(200, "application/json", "{\"ok\":true}");
    });

    // Стоп
    server.on("/stop", HTTP_GET, [](AsyncWebServerRequest* req) {
        stopAudio();
        req->send(200, "application/json", "{\"ok\":true}");
    });

    // Следующая
    server.on("/next", HTTP_GET, [](AsyncWebServerRequest* req) {
        Station* s = stationMgr.next();
        if (s) playStation(stationMgr.currentIndex);
        req->send(200, "application/json", "{\"ok\":true}");
    });

    // Предыдущая
    server.on("/prev", HTTP_GET, [](AsyncWebServerRequest* req) {
        Station* s = stationMgr.prev();
        if (s) playStation(stationMgr.currentIndex);
        req->send(200, "application/json", "{\"ok\":true}");
    });

    // Громкость
    server.on("/volume", HTTP_GET, [](AsyncWebServerRequest* req) {
        if (req->hasParam("v")) {
            int v = constrain(req->getParam("v")->value().toInt(), VOLUME_MIN, VOLUME_MAX);
            appState.volume = v;
            audio.setVolume(v);
            display.showPlaying(appState.stationName, appState.trackTitle, v, appState.playing);
            broadcastState();
        }
        req->send(200, "application/json", "{\"ok\":true}");
    });

    // Добавить станцию
    AsyncCallbackJsonWebHandler* addHandler = new AsyncCallbackJsonWebHandler(
        "/stations/add",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject body = json.as<JsonObject>();
            String name = body["name"].as<String>();
            String url  = body["url"].as<String>();
            if (name.isEmpty() || url.isEmpty()) {
                req->send(400, "application/json", "{\"ok\":false,\"error\":\"name and url required\"}");
                return;
            }
            if (stationMgr.addStation(name, url)) {
                req->send(200, "application/json", "{\"ok\":true}");
            } else {
                req->send(400, "application/json", "{\"ok\":false,\"error\":\"max stations reached\"}");
            }
        }
    );
    server.addHandler(addHandler);

    // Удалить станцию
    server.on("/stations/remove", HTTP_DELETE, [](AsyncWebServerRequest* req) {
        if (!req->hasParam("index")) {
            req->send(400, "application/json", "{\"ok\":false}");
            return;
        }
        int idx = req->getParam("index")->value().toInt();
        stationMgr.removeStation(idx);
        req->send(200, "application/json", "{\"ok\":true}");
    });
}

// ─── Вспомогательные функции ──────────────────────────────────
void playStation(int index) {
    if (index < 0 || index >= (int)stationMgr.stations.size()) return;
    stationMgr.currentIndex = index;
    appState.currentIndex   = index;
    appState.stationName    = stationMgr.stations[index].name;
    appState.trackTitle     = "";
    appState.playing        = true;

    audio.stopSong();
    audio.connecttohost(stationMgr.stations[index].url.c_str());
    audio.setVolume(appState.volume);

    display.showPlaying(appState.stationName, "", appState.volume, true);
    broadcastState();
    Serial.printf("[play] %s -> %s\n",
        appState.stationName.c_str(),
        stationMgr.stations[index].url.c_str());
}

void stopAudio() {
    audio.stopSong();
    appState.playing = false;
    display.showPlaying(appState.stationName, appState.trackTitle, appState.volume, false);
    broadcastState();
}

String buildStateJson() {
    JsonDocument doc;
    doc["type"]         = "state";
    doc["playing"]      = appState.playing;
    doc["volume"]       = appState.volume;
    doc["currentIndex"] = appState.currentIndex;
    doc["station"]      = appState.stationName;
    doc["track"]        = appState.trackTitle;
    String out;
    serializeJson(doc, out);
    return out;
}

void broadcastState() {
    String msg = buildStateJson();
    ws.textAll(msg);
}

// ─── Wi-Fi ────────────────────────────────────────────────────
bool connectWifi() {
    display.showConnecting(WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - start > WIFI_TIMEOUT_MS) return false;
        delay(200);
        Serial.print(".");
    }
    return true;
}

// ─── OTA ──────────────────────────────────────────────────────
void setupOTA() {
    ArduinoOTA.setPort(OTA_PORT);
    ArduinoOTA.setPassword(OTA_PASSWORD);
    ArduinoOTA.onStart([]() {
        audio.stopSong();
        display.showMessage("OTA Update", "Starting...");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%u%%", progress * 100 / total);
        display.showMessage("OTA Update", buf);
    });
    ArduinoOTA.onEnd([]() {
        display.showMessage("OTA Done", "Rebooting...");
    });
    ArduinoOTA.onError([](ota_error_t err) {
        display.showError("OTA Error");
    });
    ArduinoOTA.begin();
}

// ─── Setup ────────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    Serial.println("\n[boot] Smart Speaker starting...");

    // OLED
    if (!display.begin()) {
        Serial.println("[warn] OLED not found");
    }

    // Станции
    stationMgr.begin();

    // Wi-Fi
    if (!connectWifi()) {
        display.showError("WiFi failed");
        Serial.println("[error] WiFi connection failed!");
        // Продолжаем без сети (можно добавить AP режим)
        return;
    }
    Serial.printf("[wifi] Connected: %s\n", WiFi.localIP().toString().c_str());

    // mDNS
    if (MDNS.begin(MDNS_NAME)) {
        MDNS.addService("http", "tcp", WEB_PORT);
        Serial.printf("[mdns] http://%s.local\n", MDNS_NAME);
    }

    // OTA
    setupOTA();

    // I2S Audio
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(appState.volume);
    Serial.printf("[audio] I2S BCLK=%d LRC=%d DOUT=%d\n", I2S_BCLK, I2S_LRC, I2S_DOUT);

    // WebSocket
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    // HTTP маршруты
    setupRoutes();
    server.begin();
    Serial.println("[web] Server started");

    // Показать IP на дисплее
    char mdnsStr[32];
    snprintf(mdnsStr, sizeof(mdnsStr), "http://%s", MDNS_NAME);
    display.showIP(WiFi.localIP().toString().c_str(), mdnsStr);

    Serial.println("[boot] Ready!");
}

// ─── Loop ─────────────────────────────────────────────────────
void loop() {
    audio.loop();
    ArduinoOTA.handle();
    ws.cleanupClients();
}
