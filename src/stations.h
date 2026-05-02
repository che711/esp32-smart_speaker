#pragma once
#include <ArduinoJson.h>
#include <Preferences.h>
#include <vector>
#include "config.h"

struct Station { String name, url; };

class Stations {
public:
    std::vector<Station> list;
    int current = 0;

    void begin() {
        load();
        if (list.empty()) { defaults(); save(); }
    }

    // ── CRUD ───────────────────────────────────────────────────
    bool add(const String& name, const String& url) {
        if (list.size() >= 24) return false;
        list.push_back({ name, url });
        save(); return true;
    }

    bool remove(int i) {
        if (i < 0 || i >= (int)list.size()) return false;
        list.erase(list.begin() + i);
        if (current >= (int)list.size()) current = 0;
        save(); return true;
    }

    // ── Навигация ──────────────────────────────────────────────
    Station* get(int i) {
        if (i < 0 || i >= (int)list.size()) return nullptr;
        current = i; return &list[i];
    }
    Station* next() { return get((current + 1) % list.size()); }
    Station* prev() { return get((current - 1 + list.size()) % list.size()); }
    Station* cur()  { return list.empty() ? nullptr : &list[current]; }

    // ── Сериализация ───────────────────────────────────────────
    String toJson() {
        JsonDocument doc;
        JsonArray arr = doc.to<JsonArray>();
        for (int i = 0; i < (int)list.size(); i++) {
            JsonObject o = arr.add<JsonObject>();
            o["i"] = i; o["name"] = list[i].name; o["url"] = list[i].url;
        }
        String s; serializeJson(doc, s); return s;
    }

private:
    void defaults() {
        list.clear();
        JsonDocument doc;
        deserializeJson(doc, DEFAULT_STATIONS_JSON);
        for (JsonObject o : doc.as<JsonArray>())
            list.push_back({ o["name"].as<String>(), o["url"].as<String>() });
    }

    void load() {
        Preferences p; p.begin("st", true);
        String json = p.getString("v", ""); p.end();
        if (json.isEmpty()) return;
        list.clear();
        JsonDocument doc;
        if (!deserializeJson(doc, json))
            for (JsonObject o : doc.as<JsonArray>())
                list.push_back({ o["name"].as<String>(), o["url"].as<String>() });
    }

    void save() {
        JsonDocument doc;
        JsonArray arr = doc.to<JsonArray>();
        for (auto& s : list) {
            JsonObject o = arr.add<JsonObject>();
            o["name"] = s.name; o["url"] = s.url;
        }
        String json; serializeJson(doc, json);
        Preferences p; p.begin("st", false);
        p.putString("v", json); p.end();
    }
};
