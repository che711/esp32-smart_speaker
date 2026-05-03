# scripts/fix_cxx.py
#
# Форсирует C++20 через SCons CXXFLAGS — единственный надёжный способ
# в PlatformIO переопределить стандарт языка для C++ файлов,
# не затрагивая при этом C файлы (CCFLAGS).
#
# Почему build_flags/-std=gnu++2a не работает:
#   PlatformIO добавляет build_flags ДО флагов платформы,
#   и -std=gnu++17 платформы перекрывает наш флаг.
#
# Почему build_unflags ломает C файлы:
#   build_unflags убирает -std= из ВСЕХ команд включая .c файлы,
#   после чего C файлы компилируются без стандарта и падают.
#
# Решение: прямая замена CXXFLAGS (только C++) через SCons API.

Import("env")

def force_cxx20(e):
    flags = e.get("CXXFLAGS", [])
    # Нормализуем в список
    if isinstance(flags, str):
        flags = flags.split()
    else:
        flags = list(flags)
    # Убираем любые -std= флаги
    flags = [f for f in flags if not f.startswith("-std=")]
    # Добавляем C++20
    flags.append("-std=gnu++2a")
    e.Replace(CXXFLAGS=flags)
    print(f"[fix_cxx] CXXFLAGS → -std=gnu++2a ({e['PIOENV']})")

# Применяем к основному окружению (библиотеки + src)
force_cxx20(env)

# Применяем к окружению исходников проекта если доступно
try:
    Import("projenv")
    force_cxx20(projenv)
except Exception:
    pass
