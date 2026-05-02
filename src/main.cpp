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
#include "fs_manager.h"
#include "web_ui.h"

// ─── Объекты ──────────────────────────────────────────────────
Audio          audio;
Display        disp;
Stations       stations;
FsManager      fileFs;        // НЕ "fs" — конфликт с namespace fs (Arduino core 3.x)
AsyncWebServer server(WEB_PORT);
AsyncWebSocket ws("/ws");

// ─── Состояние ────────────────────────────────────────────────
enum Source { SRC_RADIO, SRC_FILE };

struct State {
    bool    playing   = false;
    Source  source    = SRC_RADIO;
    int     vol       = VOLUME_DEFAULT;
    int     stIdx     = -1;
    int     fileIdx   = -1;
    String  title     = "";
    String  sub       = "";
} st;

unsigned long lastWifiCheck = 0;
String uploadFileName = "";

// ─── Прототипы ────────────────────────────────────────────────
void playRadio(int idx);
void playFile(int idx);
void stopAudio();
void broadcastState();
String stateJson();
void updateDisplay();

// ─── Audio callbacks ──────────────────────────────────────────
void audio_showstation(const char* info) {
    if (strlen(info) > 0) {
        st.title = String(info);
        updateDisplay();
        broadcastState();
    }
}

void audio_showstreamtitle(const char* info) {
    st.sub = String(info);
    updateDisplay();
    broadcastState();
    Serial.printf("[icy] %s\n", info);
}

void audio_eof_stream(const char* info) {
    Serial.printf("[eof stream] %s\n", info);
    if (st.source == SRC_RADIO && st.playing) {
        delay(3000);
        Station* s = stations.get(st.stIdx);
        if (s) audio.connecttohost(s->url.c_str());
    }
}

void audio_eof_mp3(const char* info) {
    Serial.printf("[eof mp3] %s\n", info);
    st.playing = false;
    updateDisplay();
    broadcastState();
}

void audio_info(const char* info) {
    // Serial.printf("[audio] %s\n", info);
}

// ─── WebSocket ────────────────────────────────────────────────
void onWsEvent(AsyncWebSocket* srv, AsyncWebSocketClient* client,
               AwsEventType type, void* arg, uint8_t* data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        client->text(stateJson());
        Serial.printf("[ws] client #%u connected\n", client->id());
    }
}

// ─── HTTP маршруты ────────────────────────────────────────────
void setupRoutes() {

    server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "text/html", WEB_UI);
    });

    server.on("/status", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["type"]    = "state";
        doc["playing"] = st.playing;
        doc["source"]  = st.source == SRC_FILE ? "file" : "radio";
        doc["vol"]     = st.vol;
        doc["stIdx"]   = st.stIdx;
        doc["fileIdx"] = st.fileIdx;
        doc["title"]   = st.title;
        doc["sub"]     = st.sub;
        doc["ip"]      = WiFi.localIP().toString();
        doc["rssi"]    = WiFi.RSSI();
        doc["ram"]     = ESP.getFreeHeap();
        doc["uptime"]  = millis() / 1000;
        doc["fsUsed"]  = fileFs.usedKb();
        doc["fsTotal"] = fileFs.totalKb();
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    server.on("/play", HTTP_GET, [](AsyncWebServerRequest* req) {
        String src = req->hasParam("src") ? req->getParam("src")->value() : "radio";
        int    i   = req->hasParam("i")   ? req->getParam("i")->value().toInt() : 0;
        if (src == "file") playFile(i);
        else               playRadio(i);
        req->send(200, "application/json", "{\"ok\":true}");
    });

    server.on("/stop", HTTP_GET, [](AsyncWebServerRequest* req) {
        stopAudio();
        req->send(200, "application/json", "{\"ok\":true}");
    });

    server.on("/next", HTTP_GET, [](AsyncWebServerRequest* req) {
        if (st.source == SRC_FILE) {
            auto files = fileFs.list();
            if (!files.empty()) playFile((st.fileIdx + 1) % (int)files.size());
        } else {
            Station* s = stations.next();
            if (s) playRadio(stations.current);
        }
        req->send(200, "application/json", "{\"ok\":true}");
    });

    server.on("/prev", HTTP_GET, [](AsyncWebServerRequest* req) {
        if (st.source == SRC_FILE) {
            auto files = fileFs.list();
            if (!files.empty()) {
                int prev = (st.fileIdx - 1 + (int)files.size()) % (int)files.size();
                playFile(prev);
            }
        } else {
            Station* s = stations.prev();
            if (s) playRadio(stations.current);
        }
        req->send(200, "application/json", "{\"ok\":true}");
    });

    server.on("/vol", HTTP_GET, [](AsyncWebServerRequest* req) {
        if (req->hasParam("v")) {
            int v = constrain(req->getParam("v")->value().toInt(), VOLUME_MIN, VOLUME_MAX);
            st.vol = v;
            audio.setVolume(v);
            updateDisplay();
            broadcastState();
        }
        req->send(200, "application/json", "{\"ok\":true}");
    });

    server.on("/stations", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "application/json", stations.toJson());
    });

    // POST /stations/add  body: {"name":"...","url":"..."}
    // onBody вместо AsyncCallbackJsonWebHandler (не требует доп. зависимостей)
    server.on("/stations/add", HTTP_POST,
        [](AsyncWebServerRequest* req) { /* ответ отправляется в onBody */ },
        nullptr,
        [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t index, size_t total) {
            String body;
            body.reserve(len);
            for (size_t i = 0; i < len; i++) body += (char)data[i];
            JsonDocument doc;
            if (deserializeJson(doc, body)) {
                req->send(400, "application/json", "{\"ok\":false,\"err\":\"bad json\"}");
                return;
            }
            String name = doc["name"].as<String>();
            String url  = doc["url"].as<String>();
            if (name.isEmpty() || url.isEmpty()) {
                req->send(400, "application/json", "{\"ok\":false,\"err\":\"empty fields\"}");
                return;
            }
            req->send(200, "application/json",
                stations.add(name, url) ? "{\"ok\":true}" : "{\"ok\":false,\"err\":\"max reached\"}");
        }
    );

    server.on("/stations/del", HTTP_DELETE, [](AsyncWebServerRequest* req) {
        if (!req->hasParam("i")) { req->send(400, "application/json", "{\"ok\":false}"); return; }
        stations.remove(req->getParam("i")->value().toInt());
        req->send(200, "application/json", "{\"ok\":true}");
    });

    server.on("/files", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "application/json", fileFs.listJson());
    });

    server.on("/files/del", HTTP_DELETE, [](AsyncWebServerRequest* req) {
        if (!req->hasParam("name")) { req->send(400, "application/json", "{\"ok\":false}"); return; }
        bool ok = fileFs.remove(req->getParam("name")->value());
        req->send(200, "application/json", ok ? "{\"ok\":true}" : "{\"ok\":false}");
    });

    // Загрузка файла (multipart POST)
    server.on("/upload", HTTP_POST,
        [](AsyncWebServerRequest* req) {
            req->send(200, "application/json", "{\"ok\":true}");
            Serial.printf("[upload] done: %s\n", uploadFileName.c_str());
        },
        [](AsyncWebServerRequest* req, const String& filename,
           size_t index, uint8_t* data, size_t len, bool final) {
            static File uploadFile;
            static size_t totalReceived = 0;
            if (index == 0) {
                uploadFileName = fileFs.sanitize(filename);
                String path = String(FS_MP3_DIR) + "/" + uploadFileName;
                if (fileFs.freeKb() < 64) {
                    Serial.println("[upload] Not enough space!");
                    return;
                }
                uploadFile = LittleFS.open(path, "w");
                totalReceived = 0;
                disp.uploading(uploadFileName.c_str(), 0);
                Serial.printf("[upload] start: %s\n", uploadFileName.c_str());
            }
            if (uploadFile) {
                uploadFile.write(data, len);
                totalReceived += len;
                if (totalReceived % (32 * 1024) < len) {
                    size_t totalSize = req->contentLength();
                    int pct = totalSize > 0 ? (int)(totalReceived * 100 / totalSize) : 0;
                    disp.uploading(uploadFileName.c_str(), pct);
                }
            }
            if (final) {
                if (uploadFile) uploadFile.close();
                disp.message("Upload done!", uploadFileName.c_str());
                delay(1500);
                updateDisplay();
                Serial.printf("[upload] complete: %s (%d bytes)\n",
                    uploadFileName.c_str(), (int)totalReceived);
            }
        }
    );
}

// ─── Воспроизведение ──────────────────────────────────────────
void playRadio(int idx) {
    Station* s = stations.get(idx);
    if (!s) return;
    audio.stopSong();
    st.source = SRC_RADIO; st.stIdx = idx;
    st.playing = true; st.title = s->name; st.sub = "";
    audio.connecttohost(s->url.c_str());
    audio.setVolume(st.vol);
    updateDisplay(); broadcastState();
    Serial.printf("[radio] %s\n", s->name.c_str());
}

void playFile(int idx) {
    String path = fileFs.pathByIndex(idx);
    String name = fileFs.nameByIndex(idx);
    if (path.isEmpty()) return;
    audio.stopSong();
    st.source = SRC_FILE; st.fileIdx = idx;
    st.playing = true; st.title = name; st.sub = "";
    audio.connecttoFS(LittleFS, path.c_str());
    audio.setVolume(st.vol);
    updateDisplay(); broadcastState();
    Serial.printf("[file] %s\n", path.c_str());
}

void stopAudio() {
    audio.stopSong();
    st.playing = false;
    updateDisplay(); broadcastState();
}

void updateDisplay() {
    disp.playing(st.title, st.sub, st.vol, st.playing,
                 st.source == SRC_FILE ? D_FILE : D_RADIO);
}

String stateJson() {
    JsonDocument doc;
    doc["type"]    = "state";
    doc["playing"] = st.playing;
    doc["source"]  = st.source == SRC_FILE ? "file" : "radio";
    doc["vol"]     = st.vol;
    doc["stIdx"]   = st.stIdx;
    doc["fileIdx"] = st.fileIdx;
    doc["title"]   = st.title;
    doc["sub"]     = st.sub;
    String out; serializeJson(doc, out); return out;
}

void broadcastState() { ws.textAll(stateJson()); }

// ─── Wi-Fi ────────────────────────────────────────────────────
bool connectWifi() {
    disp.connecting(WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    unsigned long t = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - t > WIFI_TIMEOUT_MS) return false;
        delay(250); Serial.print(".");
    }
    Serial.printf("\n[wifi] %s\n", WiFi.localIP().toString().c_str());
    return true;
}

// ─── OTA ──────────────────────────────────────────────────────
void setupOTA() {
    ArduinoOTA.setPassword(OTA_PASSWORD);
    ArduinoOTA.onStart([]()    { audio.stopSong(); disp.message("OTA Update", "starting..."); });
    ArduinoOTA.onProgress([](unsigned int p, unsigned int t) { disp.uploading("OTA", p*100/t); });
    ArduinoOTA.onEnd([]()      { disp.message("OTA Done", "rebooting..."); });
    ArduinoOTA.begin();
}

// ─── Setup ────────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    Serial.println("\n[boot] Smart Speaker starting");

    if (!disp.begin())   Serial.println("[warn] OLED not found");
    fileFs.begin();
    stations.begin();

    if (!connectWifi()) {
        disp.wifiError();
        Serial.println("[error] WiFi failed!");
        return;
    }

    if (MDNS.begin(MDNS_NAME)) {
        MDNS.addService("http", "tcp", WEB_PORT);
        Serial.printf("[mdns] http://%s.local\n", MDNS_NAME);
    }

    setupOTA();
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);
    setupRoutes();
    server.begin();

    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(st.vol);

    disp.ready(WiFi.localIP().toString().c_str());
    Serial.printf("[boot] Ready! http://%s  or  http://speaker.local\n",
        WiFi.localIP().toString().c_str());
}

// ─── Loop ─────────────────────────────────────────────────────
void loop() {
    audio.loop();
    ArduinoOTA.handle();
    ws.cleanupClients();

    unsigned long now = millis();
    if (now - lastWifiCheck > WIFI_RETRY_MS) {
        lastWifiCheck = now;
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[wifi] lost, reconnecting...");
            WiFi.reconnect();
        }
    }
}
