# patch_audio.py — автопатч ESP32-audioI2S для Arduino ESP32 core 2.x
# Запускается автоматически через extra_scripts = pre:patch_audio.py
# SCons-скрипт: Import("env") — это не обычный Python, а PlatformIO/SCons API

Import("env")
import os

base = os.path.join(".pio", "libdeps", env["PIOENV"], "ESP32-audioI2S", "src")

patches = [
    # 1. <span> требует C++20, недоступен в старом тулчейне
    (
        os.path.join(base, "psram_unique_ptr.hpp"),
        "#include <span>",
        "#if __cplusplus >= 202002L\n#include <span>\n#endif"
    ),
    # 2. NetworkClient.h появился только в Arduino ESP32 core 3.x
    (
        os.path.join(base, "Audio.h"),
        "#include <NetworkClient.h>",
        "#include <WiFiClient.h>\ntypedef WiFiClient NetworkClient;"
    ),
    # 3. NetworkClientSecure.h — аналогично
    (
        os.path.join(base, "Audio.h"),
        "#include <NetworkClientSecure.h>",
        "#include <WiFiClientSecure.h>\ntypedef WiFiClientSecure NetworkClientSecure;"
    ),
]

for path, old, new in patches:
    if not os.path.isfile(path):
        continue
    with open(path, "r") as f:
        src = f.read()
    # Применяем только если старый текст есть и новый ещё не применён
    if old in src and new.split("\n")[0] not in src:
        with open(path, "w") as f:
            f.write(src.replace(old, new, 1))
        print(f"[patch_audio] {os.path.basename(path)}: патч применён")
    elif old not in src:
        print(f"[patch_audio] {os.path.basename(path)}: уже пропатчен или не нужно")
