#pragma once

// ─── Wi-Fi ────────────────────────────────────────────────────
#define WIFI_SSID        "YOUR_WIFI_SSID"
#define WIFI_PASSWORD    "YOUR_WIFI_PASSWORD"
#define WIFI_TIMEOUT_MS  15000

// ─── Сеть ─────────────────────────────────────────────────────
#define MDNS_NAME        "speaker"        // http://speaker.local
#define WEB_PORT         80

// ─── OTA ──────────────────────────────────────────────────────
#define OTA_PASSWORD     "speaker123"
#define OTA_PORT         3232

// ─── Аудио ────────────────────────────────────────────────────
#define VOLUME_DEFAULT   10               // 0–21
#define VOLUME_MAX       21
#define VOLUME_MIN       0

// ─── OLED ─────────────────────────────────────────────────────
#define OLED_ADDRESS     0x3C
#define OLED_WIDTH       128
#define OLED_HEIGHT      64
#define OLED_RESET       -1

// ─── Радиостанции по умолчанию ────────────────────────────────
// Хранятся в NVS, этот список используется при первом запуске
#define DEFAULT_STATIONS_JSON R"([
  {"name":"Jazz FM",       "url":"http://stream.rcs.revma.com/an1ugyygzk8uv"},
  {"name":"Radio Jazz",    "url":"http://radio.silvertip.be:8080/;listen.mp3"},
  {"name":"NRJ France",    "url":"http://icecast.nrj.fr/nrj-1-44-128"},
  {"name":"Lounge FM",     "url":"http://eu1.reliastream.com:7048/stream"},
  {"name":"Deep House",    "url":"http://stream.laut.fm/deephouse"},
  {"name":"ChillOut Zone", "url":"http://stream.laut.fm/chillout"}
])"
