# 🔊 Smart Speaker на ESP32

Умная колонка на ESP32-C3 Supermini с интернет-радио, Web UI и OLED дисплеем.

## Компоненты

| Компонент | Описание |
|---|---|
| ESP32-C3 Supermini | Основной контроллер |
| TPA3116D2 (XH-M189) | Стерео усилитель 2×50W |
| PCM5102 DAC (с джеком 3.5мм) | I2S ЦАП, 32-бит/384кГц |
| SSD1306 OLED 0.96" (128×64) | Дисплей статуса |
| 2× динамик 2" | Акустика |
| БП 12V 5A | Основное питание |
| DC-DC понижайка 12V→5V 3A | Питание ESP32 |

## Схема подключения

### Питание
```
12V БП (+) ──→ TPA3116 VIN
12V БП (+) ──→ DC-DC IN+
12V БП (−) ──→ DC-DC IN−  ──→ общая GND
DC-DC OUT+ (5V) ──→ ESP32 5V/VIN
ESP32 3.3V ──→ PCM5102 VCC
ESP32 3.3V ──→ OLED VCC
```

### I²S (ESP32 → PCM5102)
```
ESP32 GPIO4  ──→ PCM5102 BCK   (тактирование)
ESP32 GPIO5  ──→ PCM5102 LCK   (L/R)
ESP32 GPIO6  ──→ PCM5102 DIN   (данные)
GND          ──→ PCM5102 FMT   (формат I²S)
GND          ──→ PCM5102 SCK   (без мастер-клока)
3.3V         ──→ PCM5102 XMT   (снять mute!)
```

### I²C (ESP32 → OLED)
```
ESP32 GPIO8  ──→ OLED SDA
ESP32 GPIO9  ──→ OLED SCL
```

### Аудио (PCM5102 → TPA3116)
```
PCM5102 LOUT ──→ TPA3116 L IN+
PCM5102 ROUT ──→ TPA3116 R IN+
GND          ──→ TPA3116 L IN−
GND          ──→ TPA3116 R IN−
```

### Динамики (TPA3116 BTL выход)
```
TPA3116 LOUT+ ──→ Динамик L (+)
TPA3116 LOUT− ──→ Динамик L (−)
TPA3116 ROUT+ ──→ Динамик R (+)
TPA3116 ROUT− ──→ Динамик R (−)
```
> ⚠️ Динамики к GND не подключать — мостовой усилитель!

## Быстрый старт

### 1. Настройка
Открой `src/config.h` и измени:
```cpp
#define WIFI_SSID     "ИМЯ_СЕТИ"
#define WIFI_PASSWORD "ПАРОЛЬ"
#define OTA_PASSWORD  "speaker123"   // пароль для OTA обновлений
```

### 2. Сборка и загрузка
```bash
# PlatformIO CLI
pio run --target upload

# Или через VS Code: Ctrl+Alt+U
```

### 3. Доступ к Web UI
После загрузки открой браузер:
```
http://speaker.local
```
Или по IP адресу (отображается на OLED при старте).

## Функции

- 📻 **Интернет-радио** — MP3, AAC, OGG, FLAC потоки
- 🎛️ **Web UI** — управление с телефона/ПК
- 📡 **WebSocket** — обновления в реальном времени
- 📟 **OLED дисплей** — станция, трек, громкость
- ➕ **Управление станциями** — добавить/удалить через UI
- 💾 **NVS хранилище** — список станций сохраняется
- 🔄 **OTA обновления** — по воздуху через Arduino IDE или PlatformIO
- 🌐 **mDNS** — доступ по имени `speaker.local`

## API

| Endpoint | Метод | Описание |
|---|---|---|
| `/` | GET | Web UI |
| `/status` | GET | JSON состояние системы |
| `/play?index=N` | GET | Воспроизвести станцию N |
| `/stop` | GET | Остановить |
| `/next` | GET | Следующая станция |
| `/prev` | GET | Предыдущая станция |
| `/volume?v=N` | GET | Установить громкость (0–21) |
| `/stations` | GET | Список станций JSON |
| `/stations/add` | POST | Добавить станцию `{"name":"..","url":".."}` |
| `/stations/remove?index=N` | DELETE | Удалить станцию |

## OTA обновление

```bash
# PlatformIO
pio run --target upload --upload-port speaker.local

# Arduino IDE
# Инструменты → Порт → speaker.local
# Загрузить (пароль: speaker123)
```

## Зависимости (platformio.ini)

- `schreibfaul1/ESP32-audioI2S`
- `esphome/ESPAsyncWebServer-esphome`
- `mathieucarbou/AsyncTCP`
- `adafruit/Adafruit SSD1306`
- `bblanchon/ArduinoJson`

## Структура репозитория

```
smart-speaker/
├── platformio.ini
├── README.md
└── src/
    ├── main.cpp        — основная прошивка
    ├── config.h        — конфигурация (Wi-Fi, пины, параметры)
    ├── display.h       — менеджер OLED дисплея
    ├── stations.h      — менеджер станций (NVS)
    └── web_ui.h        — встроенный Web UI (HTML/CSS/JS)
```

## Лицензия

MIT
