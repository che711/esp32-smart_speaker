#pragma once
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "config.h"

// Режим отображения
enum DispMode { D_BOOT, D_CONNECTING, D_RADIO, D_FILE, D_UPLOAD };

class Display {
public:
    Adafruit_SSD1306 oled{ OLED_W, OLED_H, &Wire, OLED_RESET };

    bool begin() {
        Wire.begin(I2C_SDA, I2C_SCL);
        if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) return false;
        oled.setTextColor(SSD1306_WHITE);
        boot(); return true;
    }

    // ── Системные экраны ───────────────────────────────────────
    void boot() {
        clear();
        center("Smart Speaker", 1, 14);
        center("booting...", 1, 30);
        show();
    }

    void connecting(const char* ssid) {
        clear();
        small("Connecting to:", 0, 0);
        small(ssid, 0, 12);
        small("Please wait...", 0, 28);
        show();
    }

    void ready(const char* ip) {
        clear();
        small("Ready!", 0, 0);
        oled.drawLine(0, 11, 127, 11, SSD1306_WHITE);
        small("IP:", 0, 15);
        small(ip, 20, 15);
        small("http://speaker.local", 0, 27);
        show();
    }

    void wifiError() {
        clear();
        center("WiFi Error", 1, 20);
        center("Check config.h", 1, 36);
        show();
    }

    // ── Основной экран (радио / файл) ──────────────────────────
    void playing(const String& line1, const String& line2,
                 int vol, bool isPlaying, DispMode mode) {
        clear();

        // Строка режима
        oled.setTextSize(1);
        oled.setCursor(0, 0);
        if (mode == D_FILE)  oled.print(isPlaying ? "\x10 FILE" : "|| FILE");
        else                 oled.print(isPlaying ? "\x10 RADIO" : "|| RADIO");
        oled.drawLine(0, 11, 127, 11, SSD1306_WHITE);

        // Название станции/файла
        small(truncate(line1, 21).c_str(), 0, 14);

        // Название трека / ICY метаданные
        if (line2.length() > 0) {
            String part1 = truncate(line2, 21);
            String part2 = line2.length() > 21 ? truncate(line2.substring(21), 21) : "";
            small(part1.c_str(), 0, 25);
            if (part2.length()) small(part2.c_str(), 0, 35);
        }

        // Полоска громкости
        oled.drawLine(0, 52, 127, 52, SSD1306_WHITE);
        oled.setCursor(0, 56);
        oled.print("VOL");
        oled.fillRect(22, 57, map(vol, 0, VOLUME_MAX, 0, 100), 5, SSD1306_WHITE);
        oled.drawRect(22, 57, 100, 5, SSD1306_WHITE);
        show();
    }

    // ── Загрузка файла ─────────────────────────────────────────
    void uploading(const char* name, int pct) {
        clear();
        small("Uploading:", 0, 0);
        oled.drawLine(0, 11, 127, 11, SSD1306_WHITE);
        small(truncate(String(name), 21).c_str(), 0, 14);

        // Прогресс-бар
        oled.drawRect(0, 32, 128, 12, SSD1306_WHITE);
        oled.fillRect(2, 34, map(pct, 0, 100, 0, 124), 8, SSD1306_WHITE);

        char buf[8]; snprintf(buf, sizeof(buf), "%d%%", pct);
        center(buf, 1, 50);
        show();
    }

    void message(const char* l1, const char* l2 = "") {
        clear();
        center(l1, 1, 20);
        if (strlen(l2)) center(l2, 1, 36);
        show();
    }

private:
    void clear() { oled.clearDisplay(); }
    void show()  { oled.display(); }

    void small(const char* txt, int x, int y) {
        oled.setTextSize(1);
        oled.setCursor(x, y);
        oled.print(txt);
    }

    void center(const char* txt, int size, int y) {
        oled.setTextSize(size);
        int w = strlen(txt) * 6 * size;
        oled.setCursor(max(0, (128 - w) / 2), y);
        oled.print(txt);
    }

    String truncate(const String& s, int maxLen) {
        if ((int)s.length() <= maxLen) return s;
        return s.substring(0, maxLen - 1) + "~";
    }
};
