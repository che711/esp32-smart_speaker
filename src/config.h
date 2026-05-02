#pragma once

// ─── Wi-Fi ────────────────────────────────────────────────────
#define WIFI_SSID        "network"
#define WIFI_PASSWORD    "password"
#define WIFI_TIMEOUT_MS  20000
#define WIFI_RETRY_MS    30000

// ─── Сеть ─────────────────────────────────────────────────────
#define MDNS_NAME        "speaker"      // http://speaker.local
#define WEB_PORT         80

// ─── OTA ──────────────────────────────────────────────────────
#define OTA_PASSWORD     "speaker123"

// ─── Аудио ────────────────────────────────────────────────────
#define VOLUME_DEFAULT   12             // 0–21
#define VOLUME_MAX       21
#define VOLUME_MIN       0

// ─── OLED ─────────────────────────────────────────────────────
#define OLED_ADDR        0x3C
#define OLED_W           128
#define OLED_H           64
#define OLED_RESET       -1

// ─── Файловая система ─────────────────────────────────────────
#define FS_MP3_DIR       "/mp3"         // папка для загруженных файлов
#define FS_MAX_FILE_MB   8              // максимальный размер одного файла

// ─── Станции по умолчанию ─────────────────────────────────────
#define DEFAULT_STATIONS_JSON R"([
  {"name":"Jazz FM",        "url":"http://stream.rcs.revma.com/an1ugyygzk8uv"},
  {"name":"NRJ France",     "url":"http://icecast.nrj.fr/nrj-1-44-128"},
  {"name":"Deep House",     "url":"http://stream.laut.fm/deephouse"},
  {"name":"Lounge FM",      "url":"http://eu1.reliastream.com:7048/stream"},
  {"name":"Chill Out",      "url":"http://stream.laut.fm/chillout"},
  {"name":"Radio Paradise", "url":"http://stream.radioparadise.com/mp3-128"},
  {"name":"Ambient",        "url":"http://stream.laut.fm/ambient"},
  {"name":"Smooth Jazz",    "url":"http://jazz.streamr.ru/jazz-64.mp3"}
])"
