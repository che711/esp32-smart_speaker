# patch_audio.py — полный патч ESP32-audioI2S для pioarduino IDF 5.3
# Исправляет все известные несовместимости без смены версии библиотеки

Import("env")
import os, re

base = os.path.join(".pio", "libdeps", env["PIOENV"], "ESP32-audioI2S", "src")

def patch_file(path, replacements):
    if not os.path.isfile(path):
        print(f"[patch] SKIP (not found): {path}")
        return
    with open(path, "r") as f:
        src = f.read()
    changed = False
    for old, new in replacements:
        if old in src and new not in src:
            src = src.replace(old, new, 1)
            changed = True
    if changed:
        with open(path, "w") as f:
            f.write(src)
        print(f"[patch] OK: {os.path.basename(path)}")
    else:
        print(f"[patch] already patched: {os.path.basename(path)}")

# ── 1. psram_unique_ptr.hpp ────────────────────────────────────
patch_file(os.path.join(base, "psram_unique_ptr.hpp"), [
    # C++20 <span> — не доступен в C++17
    (
        "#include <span>",
        "#if __cplusplus >= 202002L\n#include <span>\n#endif"
    ),
    # C++20 <cstdint> включается неправильно в C-файлах
    (
        "#include <cstdint>",
        "#ifdef __cplusplus\n#include <cstdint>\n#else\n#include <stdint.h>\n#endif"
    ),
])

# ── 2. Audio.h ─────────────────────────────────────────────────
patch_file(os.path.join(base, "Audio.h"), [
    # NetworkClient.h появился только в Arduino ESP32 core 3.x
    (
        "#include <NetworkClient.h>",
        "#include <WiFiClient.h>\ntypedef WiFiClient NetworkClient;"
    ),
    # NetworkClientSecure.h аналогично
    (
        "#include <NetworkClientSecure.h>",
        "#include <WiFiClientSecure.h>\ntypedef WiFiClientSecure NetworkClientSecure;"
    ),
])

# ── 3. Audio.cpp ───────────────────────────────────────────────
patch_file(os.path.join(base, "Audio.cpp"), [
    # allow_pd добавлен в i2s_chan_config_t только в IDF 5.4+
    (
        "m_i2s_chan_cfg.allow_pd = false;",
        "// m_i2s_chan_cfg.allow_pd = false;  // IDF 5.4+ only, patched"
    ),
    # dsps_biquad_sf32 — SIMD версия, недоступна в IDF 5.3 libs
    # заменяем на стандартную dsps_biquad_f32
    (
        "dsps_biquad_sf32(",
        "dsps_biquad_f32("
    ),
])

print("[patch] all done")
