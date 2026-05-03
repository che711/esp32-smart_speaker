# scripts/patch_audio.py
# Патчи совместимости ESP32-audioI2S 3.x с IDF 5.3 / Arduino ESP32 core 3.1

Import("env")
import os

base = os.path.join(".pio", "libdeps", env["PIOENV"], "ESP32-audioI2S", "src")

def patch(path, replacements):
    if not os.path.isfile(path):
        return
    with open(path, "r") as f:
        src = f.read()
    changed = False
    for old, new in replacements:
        if old in src and new.split("\n")[0] not in src:
            src = src.replace(old, new, 1)
            changed = True
    if changed:
        with open(path, "w") as f:
            f.write(src)
        print(f"[patch_audio] OK: {os.path.basename(path)}")

# Audio.h: NetworkClient/Secure только в Arduino core 3.x
patch(os.path.join(base, "Audio.h"), [
    (
        "#include <NetworkClient.h>",
        "#include <WiFiClient.h>\ntypedef WiFiClient NetworkClient;"
    ),
    (
        "#include <NetworkClientSecure.h>",
        "#include <WiFiClientSecure.h>\ntypedef WiFiClientSecure NetworkClientSecure;"
    ),
])

# Audio.cpp: allow_pd только в IDF 5.4+, dsps_biquad_sf32 недоступна в IDF 5.3
patch(os.path.join(base, "Audio.cpp"), [
    (
        "m_i2s_chan_cfg.allow_pd = false;",
        "// m_i2s_chan_cfg.allow_pd = false; // IDF 5.4+ only"
    ),
    (
        "dsps_biquad_sf32(",
        "dsps_biquad_f32("
    ),
])
