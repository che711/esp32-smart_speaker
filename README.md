# Smart Speaker — ESP32 DevKit

## Структура проекта

```
smart-speaker/
├── platformio.ini
├── partitions.csv
├── scripts/
│   ├── fix_cxx.py       ← форсирует C++20 через SCons CXXFLAGS (не build_flags!)
│   └── patch_audio.py   ← патчи IDF 5.3 совместимости
└── src/
    ├── main.cpp
    ├── config.h          ← ← ← СЮДА вписать Wi-Fi
    ├── display.h
    ├── stations.h
    ├── fs_manager.h
    └── web_ui.h
```

## Быстрый старт

**1. Вписать Wi-Fi в `src/config.h`:**
```cpp
#define WIFI_SSID     "ИМЯ_СЕТИ"
#define WIFI_PASSWORD "ПАРОЛЬ"
```

**2. Собрать и прошить:**
```bash
rm -rf .pio
pio run --target upload
```

**3. Открыть браузер:**
```
http://speaker.local
# или по IP адресу с OLED дисплея
```

## Схема подключения (ESP32 DevKit → PCM5102 DAC)

```
GPIO27 → BCK    GPIO21 → OLED SDA
GPIO26 → LCK    GPIO22 → OLED SCL
GPIO25 → DIN    3.3V   → OLED VCC / PCM5102 VCC / PCM5102 XMT
GND    → FMT    GND    → OLED GND / PCM5102 GND
GND    → SCK
```

## Почему два скрипта в scripts/

**`fix_cxx.py`** — ESP32-audioI2S 3.x написан на C++20 (`requires`, `std::span`,
`operator<=>`). PlatformIO использует Arduino ESP32 core с `-std=gnu++17` по умолчанию,
и `build_flags` не может его перекрыть надёжно (платформа добавляет свои флаги позже).
Единственный рабочий способ — прямая замена `CXXFLAGS` через SCons API,
что делает `fix_cxx.py`. Важно: он модифицирует только `CXXFLAGS` (C++ файлы),
не трогая `CCFLAGS` (C файлы), поэтому `.c` файлы не ломаются.

**`patch_audio.py`** — ESP32-audioI2S 3.x использует API появившееся в IDF 5.4
(`allow_pd`, `dsps_biquad_sf32`) и Arduino ESP32 core 3.x (`NetworkClient.h`).
Скрипт патчит эти несовместимости перед сборкой.

## Пины

| Пин | Функция |
|-----|---------|
| GPIO27 | I2S BCLK → PCM5102 BCK |
| GPIO26 | I2S LRC → PCM5102 LCK |
| GPIO25 | I2S DOUT → PCM5102 DIN |
| GPIO21 | I2C SDA → OLED |
| GPIO22 | I2C SCL → OLED |

## OTA обновление

```bash
pio run --target upload --upload-port speaker.local
# пароль: speaker123
```
