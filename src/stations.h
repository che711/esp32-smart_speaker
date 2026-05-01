#pragma once
#include <ArduinoJson.h>
#include <Preferences.h>
#include "config.h"

struct Station {
    String name;
    String url;
};

class StationManager {
public:
    std::vector<Station> stations;
    int currentIndex = 0;

    void begin() {
        load();
        if (stations.empty()) {
            loadDefaults();
            save();
        }
    }

    void loadDefaults() {
        stations.clear();
        JsonDocument doc;
        deserializeJson(doc, DEFAULT_STATIONS_JSON);
        for (JsonObject obj : doc.as<JsonArray>()) {
            stations.push_back({ obj["name"].as<String>(), obj["url"].as<String>() });
        }
    }

    void load() {
        Preferences prefs;
        prefs.begin("stations", true);
        String json = prefs.getString("list", "");
        prefs.end();
        if (json.isEmpty()) return;

        stations.clear();
        JsonDocument doc;
        if (deserializeJson(doc, json) == DeserializationError::Ok) {
            for (JsonObject obj : doc.as<JsonArray>()) {
                stations.push_back({ obj["name"].as<String>(), obj["url"].as<String>() });
            }
        }
    }

    void save() {
        JsonDocument doc;
        JsonArray arr = doc.to<JsonArray>();
        for (auto& s : stations) {
            JsonObject obj = arr.add<JsonObject>();
            obj["name"] = s.name;
            obj["url"]  = s.url;
        }
        String json;
        serializeJson(doc, json);
        Preferences prefs;
        prefs.begin("stations", false);
        prefs.putString("list", json);
        prefs.end();
    }

    String toJson() {
        JsonDocument doc;
        JsonArray arr = doc.to<JsonArray>();
        for (int i = 0; i < (int)stations.size(); i++) {
            JsonObject obj = arr.add<JsonObject>();
            obj["index"] = i;
            obj["name"]  = stations[i].name;
            obj["url"]   = stations[i].url;
        }
        String out;
        serializeJson(doc, out);
        return out;
    }

    bool addStation(const String& name, const String& url) {
        if (stations.size() >= 20) return false;
        stations.push_back({ name, url });
        save();
        return true;
    }

    bool removeStation(int index) {
        if (index < 0 || index >= (int)stations.size()) return false;
        stations.erase(stations.begin() + index);
        if (currentIndex >= (int)stations.size()) currentIndex = 0;
        save();
        return true;
    }

    Station* current() {
        if (stations.empty()) return nullptr;
        return &stations[currentIndex];
    }

    Station* next() {
        if (stations.empty()) return nullptr;
        currentIndex = (currentIndex + 1) % stations.size();
        return current();
    }

    Station* prev() {
        if (stations.empty()) return nullptr;
        currentIndex = (currentIndex - 1 + stations.size()) % stations.size();
        return current();
    }
};
