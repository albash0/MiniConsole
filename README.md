# MiniConsole

A pocket ESP32-S3 game console with six buttons, an OLED or color TFT screen, and a growing library of games. Made by [albash0](https://github.com/albash0).

## About the project

MiniConsole started with Snake and portrait Tetris on a 128×64 OLED. It grew into a handheld with arcade games, puzzles, an adapted Doom engine, an original traffic-planning game, and wireless play between two consoles. This repository contains the firmware, the complete local DIABLITO adaptation, host tests, custom carrier PCBs, and printable plain-back case parts.

### Timeline

Dates below come from dated validation notes and source/CAD records; early dates identify the earliest retained files, rather than a reconstructed Git history.

| Date | Milestone |
| --- | --- |
| 2026-10-12 | Earliest retained Snake and Tetris game sources. |
| 2026-10-13 | Breakout, Invaders and ten-level Sokoban; pause/resume checkpoints; DIABLITO-based Doom integration on the OLED console. |
| 2026-10-14 | Custom OLED and 1.14-inch TFT carrier-board design records. |
| 2026-10-16 | Going Out mode: persistent drink counter, configurable goal, and three party games. |
| 2026-10-29 | Color TFT firmware validated on the assembled console, including Doom. |
| 2026-10-30 | Native TFT game presentation and appearance work. |
| 2026-11-05 | Shorter case top selected after a physical test print. |
| 2026-11-09 | Piezo sound, Tetris music, themes, independent music/background controls, and keyed case refinements. |
| 2026-11-10–11 | Micro Motorways: traffic simulation, roads, bridges, express links, endless growth, diagonal roads and junction balancing. |
| 2026-11-12 | Wireless Tetris Block Battle and ten-room Pocket Partners co-op, tested on two TFT consoles. |
| 2026-11-17 | Public source preparation, portable build instructions and privacy review. |

## Games and features

- **Snake:** animated body, configurable walls/speed and persistent high score.
- **Tetris:** portrait play, textured pieces, next-piece preview, ghost, line-clear animations and classic scoring.
- **Breakout** and **Invaders:** arcade games with successive levels/waves.
- **Sokoban:** ten original puzzles with undo and saved unlocks.
- **Doom:** the real shareware engine, adapted from DIABLITO with six-button controls and separate native saves.
- **Micro Motorways:** an original traffic-planning game with bridges, express links, persistent cities and endless destination replacement.
- **Block Battle:** two-console Tetris with matching bags, garbage attacks and rematches.
- **Pocket Partners:** ten original cooperative platform-puzzle rooms, shared keys, crates and pressure plates.
- **Going Out:** drink counter/goal, Reaction Duel, High or Low and Pass the Bomb.

Settings, themes, sound, music, high scores and checkpoints survive restarts. Wireless modes use ESP-NOW without a router. [Controls and hardware](docs/USER_GUIDE.md) explain the details.

## Build and install

Requires Python 3, Arduino CLI, ESP32 Arduino core **3.3.8**, and the display dependencies. From this repository's root:

```sh
arduino-cli core update-index --additional-urls https://espressif.github.io/arduino-esp32/package_esp32_index.json
arduino-cli core install esp32:esp32@3.3.8 --additional-urls https://espressif.github.io/arduino-esp32/package_esp32_index.json
arduino-cli lib install "Adafruit SSD1306" "Adafruit ST7735 and ST7789 Library"
python3 -m pip install esptool
python3 MiniConsole/tools/build_doom.py --display oled
python3 MiniConsole/tools/build_doom.py --display tft
```

The repository vendors Adafruit GFX/BusIO for the host tests and the **full adapted DIABLITO library** under `libraries/DiablitoOLED`; the builder explicitly uses these local libraries. `ARDUINO_CLI` and `ESPTOOL` can override executable locations. The Arduino IDE's bundled CLI is detected on macOS when a CLI is not on PATH.

For a first installation, Doom also needs its separate compressed shareware game data. The source repository does not bundle copyrighted game assets. `--fetch-assets` downloads the upstream asset at a pinned commit and checks its length and SHA-256 **before** writing it. The asset retains its upstream shareware terms. Review [third-party notices](THIRD_PARTY.md).

```sh
# Replace PORT with the serial port reported by arduino-cli board list.
python3 MiniConsole/tools/build_doom.py --display tft --port PORT --assets --fetch-assets
# Later updates only need the application:
python3 MiniConsole/tools/build_doom.py --display tft --port PORT
```

You can provide the exact supported asset using `--asset-file PATH` instead of downloading it. Board settings: ESP32S3 Dev Module, 4 MB DIO flash, USB hardware CDC, **QSPI PSRAM enabled**. The custom partition table preserves NVS and assigns separate Doom-data and native-save partitions. Back up your device before changing its partition layout.

## Hardware

| Item | Files |
| --- | --- |
| OLED carrier V1.7 | [KiCad project, Gerbers and BOM](MiniConsole/PCB/) |
| 1.14-inch ST7789 TFT carrier V1.3 | [KiCad project, Gerbers and BOM](MiniConsole/PCB-TFT114/) |
| Keyed TFT case with plain rear choices | [Individual STEP/STL parts and assembly notes](hardware/case/) |

The boards use an ESP32-S3 SuperMini, six grounded buttons with internal pull-ups, and an optional GPIO4 piezo through 220 Ω. The TFT screen is 135×240, displayed in landscape. Read the [battery wiring correction](MiniConsole/PCB-TFT114/wiring/README.md) before assembling that carrier. Native KiCad files and unpacked fabrication outputs are supplied; hardware illustrations are nominal references.

## Test

A C++17 compiler with AddressSanitizer and UndefinedBehaviorSanitizer is required. Host tests exercise game rules, saves, actual Adafruit GFX rendering, menus, music, radio packet codecs and co-op room solutions. They stub the Doom engine; a host pass does not validate Doom runtime or radio range on hardware.

```sh
sh MiniConsole/tests/run_tests.sh
DISPLAY_PROFILE=tft sh MiniConsole/tests/run_tests.sh
```

## License and credits

Original software, documentation and design files are released under **GPL-2.0-or-later**, except where a retained file notice states otherwise. See [LICENSE](LICENSE) and [THIRD_PARTY.md](THIRD_PARTY.md). DIABLITO, RP2040 Doom, Chocolate Doom and id Software retain their respective credits and licenses. Bundled Adafruit and KiCad library material retains its own license. Doom game data is licensed separately from source code.
