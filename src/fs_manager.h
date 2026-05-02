#pragma once
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <vector>
#include "config.h"

struct Mp3File { String name, path; size_t size; };

class FsManager {
public:
    bool begin() {
        if (!LittleFS.begin(true)) {
            Serial.println("[fs] LittleFS mount failed");
            return false;
        }
        // Создать папку если нет
        if (!LittleFS.exists(FS_MP3_DIR)) LittleFS.mkdir(FS_MP3_DIR);
        Serial.printf("[fs] Mounted. Used: %d / %d KB\n",
            (int)(LittleFS.usedBytes() / 1024),
            (int)(LittleFS.totalBytes() / 1024));
        return true;
    }

    // ── Список файлов ──────────────────────────────────────────
    std::vector<Mp3File> list() {
        std::vector<Mp3File> files;
        File dir = LittleFS.open(FS_MP3_DIR);
        if (!dir || !dir.isDirectory()) return files;
        File f = dir.openNextFile();
        while (f) {
            if (!f.isDirectory()) {
                String name = f.name();
                String lower = name; lower.toLowerCase();
                if (lower.endsWith(".mp3") || lower.endsWith(".aac") ||
                    lower.endsWith(".wav") || lower.endsWith(".flac")) {
                    files.push_back({
                        name,
                        String(FS_MP3_DIR) + "/" + name,
                        f.size()
                    });
                }
            }
            f = dir.openNextFile();
        }
        return files;
    }

    String listJson() {
        auto files = list();
        JsonDocument doc;
        JsonArray arr = doc.to<JsonArray>();
        for (int i = 0; i < (int)files.size(); i++) {
            JsonObject o = arr.add<JsonObject>();
            o["i"]    = i;
            o["name"] = files[i].name;
            o["path"] = files[i].path;
            o["size"] = files[i].size;
            o["kb"]   = files[i].size / 1024;
        }
        String s; serializeJson(doc, s); return s;
    }

    bool remove(const String& name) {
        String path = String(FS_MP3_DIR) + "/" + name;
        return LittleFS.remove(path);
    }

    // Путь по индексу
    String pathByIndex(int idx) {
        auto files = list();
        if (idx < 0 || idx >= (int)files.size()) return "";
        return files[idx].path;
    }

    String nameByIndex(int idx) {
        auto files = list();
        if (idx < 0 || idx >= (int)files.size()) return "";
        return files[idx].name;
    }

    // Свободное место в KB
    size_t freeKb() { return (LittleFS.totalBytes() - LittleFS.usedBytes()) / 1024; }
    size_t usedKb() { return LittleFS.usedBytes() / 1024; }
    size_t totalKb() { return LittleFS.totalBytes() / 1024; }

    // Для прямой записи из upload handler
    File openForWrite(const String& name) {
        String path = String(FS_MP3_DIR) + "/" + sanitize(name);
        return LittleFS.open(path, "w");
    }

    String sanitize(const String& name) {
        String s = name;
        s.replace("..", "");
        s.replace("/", "_");
        s.replace("\\", "_");
        return s;
    }
};
