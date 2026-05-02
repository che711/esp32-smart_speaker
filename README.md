# 🔊 Smart Speaker — Radio + Streaming + MP3 Upload

ESP32 DevKit (WROOM-32) колонка с интернет-радио, Web UI и загрузкой MP3 файлов по Wi-Fi.

## Возможности

| Функция | Описание |
|---|---|
| 📻 Интернет-радио | MP3/AAC потоки, ICY метаданные (artist – title) |
| 🎵 Воспроизведение файлов | MP3/AAC/WAV/FLAC загруженные на устройство |
| ⬆ Загрузка MP3 по Wi-Fi | Drag & drop или кнопка выбора в Web UI |
| 📟 OLED дисплей | Станция, трек, громкость, прогресс загрузки |
| 🌐 Web UI по IP | Открывается в браузере на любом устройстве в сети |
| 🌐 mDNS | http://speaker.local — без запоминания IP |
| 📡 WebSocket | Реальное время — дисплей и UI синхронизированы |
| 💾 NVS | Список станций сохраняется при перезагрузке |
| 🔄 OTA | Обновление прошивки по воздуху |
| 🔁 Wi-Fi watchdog | Авто-переподключение при потере сети |
| 🔁 Авто-реконнект | Перезапуск потока при обрыве радио |

## Компоненты

| Компонент | Описание |
|---|---|
| ESP32 DevKit V1 (WROOM-32) | Основной контроллер |
| TPA3116D2 (XH-M189) | Стерео усилитель 2×50W |
| PCM5102 DAC (с джеком 3.5мм) | I2S ЦАП 32-бит |
| SSD1306 OLED 0.96" 128×64 | Дисплей статуса |
| 2× динамик 3–4" | Акустика |
| БП 12V 5A | Основное питание |
| DC-DC 12V→5V 3A | Питание ESP32 |

## Схема подключения

### I²S (ESP32 → PCM5102 DAC)
```
GPIO27  → PCM5102 BCK    (тактирование)
GPIO26  → PCM5102 LCK    (L/R канал)
GPIO25  → PCM5102 DIN    (данные)
GND     → PCM5102 FMT    (формат I²S)
GND     → PCM5102 SCK    (без мастер-клока)
3.3V    → PCM5102 XMT    (снять mute — обязательно!)
3.3V    → PCM5102 VCC
GND     → PCM5102 GND
```

### Аудио линия (PCM5102 → TPA3116D2)
```
PCM5102 LOUT  → TPA3116 L IN+
PCM5102 ROUT  → TPA3116 R IN+
GND           → TPA3116 L IN−
GND           → TPA3116 R IN−
```

### I²C (ESP32 → OLED SSD1306)
```
GPIO21  → OLED SDA
GPIO22  → OLED SCL
3.3V    → OLED VCC
GND     → OLED GND
```

### Питание
```
12V БП  → TPA3116 VIN
12V БП  → DC-DC IN+
DC-DC OUT+ (5V) → ESP32 VIN    ← замерь мультиметром перед подключением!
Общая шина GND: ESP32 ←→ TPA3116 ←→ PCM5102 ←→ OLED ←→ DC-DC OUT−
```

### Динамики (BTL — к GND не подключать!)
```
TPA3116 LOUT+ → Динамик L (+)
TPA3116 LOUT− → Динамик L (−)
TPA3116 ROUT+ → Динамик R (+)
TPA3116 ROUT− → Динамик R (−)
```

## Быстрый старт

### 1. Настройка Wi-Fi
```cpp
// src/config.h
#define WIFI_SSID     "ИМЯ_СЕТИ"
#define WIFI_PASSWORD "ПАРОЛЬ"
```

### 2. Сборка и загрузка
```bash
# Используй правильный pio (не системный /usr/bin/pio)
~/.platformio/penv/bin/pio run --target upload
```

### 3. Открыть Web UI
IP адрес отображается на OLED при старте. Открой в браузере:
```
http://<IP_адрес>
# или
http://speaker.local
```

## Загрузка MP3

1. Открой вкладку **⬆ Загрузка** в Web UI
2. Перетащи MP3 в зону загрузки или нажми кнопку выбора
3. Прогресс виден и в браузере, и на OLED дисплее
4. Файл появится во вкладке **🎵 Файлы** — нажми для воспроизведения

**Ограничения LittleFS:**
- Макс. размер файла: 8 МБ
- Форматы: MP3, AAC, WAV, FLAC

## Структура проекта

```
smart-speaker/
├── platformio.ini      ← конфигурация сборки
├── partitions.csv      ← таблица разделов (app 1MB + LittleFS ~2MB)
├── patch_audio.py      ← автопатч ESP32-audioI2S (совместимость)
├── README.md
└── src/
    ├── main.cpp        ← основной код, HTTP API, audio loop
    ├── config.h        ← Wi-Fi, пины, параметры
    ├── display.h       ← OLED менеджер
    ├── stations.h      ← радиостанции (NVS хранилище)
    ├── fs_manager.h    ← LittleFS файловый менеджер
    └── web_ui.h        ← встроенный Web UI (HTML/CSS/JS)
```

## HTTP API

| Endpoint | Метод | Описание |
|---|---|---|
| `/` | GET | Web UI |
| `/status` | GET | JSON состояние системы |
| `/play?src=radio&i=N` | GET | Воспроизвести станцию N |
| `/play?src=file&i=N` | GET | Воспроизвести файл N |
| `/stop` | GET | Стоп |
| `/next` | GET | Следующая |
| `/prev` | GET | Предыдущая |
| `/vol?v=N` | GET | Громкость 0–21 |
| `/stations` | GET | Список станций JSON |
| `/stations/add` | POST | `{"name":"...","url":"..."}` |
| `/stations/del?i=N` | DELETE | Удалить станцию |
| `/files` | GET | Список загруженных файлов |
| `/files/del?name=...` | DELETE | Удалить файл |
| `/upload` | POST | Multipart загрузка файла |

## OTA обновление прошивки

```bash
~/.platformio/penv/bin/pio run --target upload --upload-port speaker.local
# Пароль: speaker123  (меняется в config.h → OTA_PASSWORD)
```

## Решение проблем сборки

Если `pio run` падает с ошибками библиотеки ESP32-audioI2S:
```bash
# 1. Убедись что используется правильный pio
which pio          # должно быть НЕ /usr/bin/pio
~/.platformio/penv/bin/pio --version   # должно быть 6.1.19+

# 2. Добавь в PATH (один раз)
echo 'export PATH="$HOME/.platformio/penv/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc

# 3. Пересобери с нуля
rm -rf .pio
pio run
```

`patch_audio.py` применяется автоматически перед каждой сборкой и исправляет
проблемы совместимости ESP32-audioI2S с Arduino ESP32 core 2.x.

## Лицензия

MIT
