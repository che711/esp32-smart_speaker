#pragma once
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "config.h"

class DisplayManager {
public:
    Adafruit_SSD1306 oled;

    DisplayManager() : oled(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET) {}

    bool begin() {
        Wire.begin(I2C_SDA, I2C_SCL);
        if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
            return false;
        }
        oled.clearDisplay();
        oled.setTextColor(SSD1306_WHITE);
        showBoot();
        return true;
    }

    void showBoot() {
        oled.clearDisplay();
        oled.setTextSize(1);
        oled.setCursor(28, 20);
        oled.println("Smart Speaker");
        oled.setCursor(38, 34);
        oled.println("Loading...");
        oled.display();
    }

    void showConnecting(const char* ssid) {
        oled.clearDisplay();
        oled.setTextSize(1);
        oled.setCursor(0, 0);
        oled.println("Connecting WiFi...");
        oled.setCursor(0, 16);
        oled.print(ssid);
        oled.display();
    }

    void showIP(const char* ip, const char* mdns) {
        oled.clearDisplay();
        oled.setTextSize(1);
        oled.setCursor(0, 0);
        oled.println("WiFi connected!");
        oled.setCursor(0, 16);
        oled.print("IP: ");
        oled.println(ip);
        oled.setCursor(0, 30);
        oled.print(mdns);
        oled.println(".local");
        oled.display();
    }

    // Основной экран воспроизведения
    void showPlaying(const String& station, const String& track, int volume, bool playing) {
        oled.clearDisplay();

        // Статус воспроизведения
        oled.setTextSize(1);
        oled.setCursor(0, 0);
        oled.print(playing ? "\x10 " : "|| ");    // ▶ или ||

        // Название станции (обрезать если длинное)
        String st = station;
        if (st.length() > 16) st = st.substring(0, 15) + "~";
        oled.print(st);

        // Разделитель
        oled.drawLine(0, 12, 127, 12, SSD1306_WHITE);

        // Название трека (с переносом)
        oled.setTextSize(1);
        String tr = track;
        if (tr.length() > 40) tr = tr.substring(0, 39) + "...";
        oled.setCursor(0, 16);
        // Простой перенос по 21 символу
        if (tr.length() > 21) {
            oled.println(tr.substring(0, 21));
            oled.setCursor(0, 26);
            oled.println(tr.substring(21, 42));
        } else {
            oled.println(tr);
        }

        // Разделитель низа
        oled.drawLine(0, 51, 127, 51, SSD1306_WHITE);

        // Громкость — полоска
        oled.setCursor(0, 55);
        oled.print("VOL ");
        int barLen = map(volume, 0, VOLUME_MAX, 0, 90);
        oled.fillRect(26, 56, barLen, 6, SSD1306_WHITE);
        oled.drawRect(26, 56, 90, 6, SSD1306_WHITE);

        oled.display();
    }

    void showError(const char* msg) {
        oled.clearDisplay();
        oled.setTextSize(1);
        oled.setCursor(0, 0);
        oled.println("Error:");
        oled.setCursor(0, 16);
        oled.println(msg);
        oled.display();
    }

    void showMessage(const char* line1, const char* line2 = "") {
        oled.clearDisplay();
        oled.setTextSize(1);
        oled.setCursor(0, 20);
        oled.println(line1);
        if (strlen(line2) > 0) {
            oled.setCursor(0, 34);
            oled.println(line2);
        }
        oled.display();
    }
};
