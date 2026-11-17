# MiniConsole build and installation

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

For a first installation, Doom also needs its separate compressed shareware game data. The source repository does not bundle copyrighted game assets. `--fetch-assets` downloads the upstream asset at a pinned commit and checks its length and SHA-256 **before** writing it. The asset retains its upstream shareware terms. Review [third-party notices](../THIRD_PARTY.md).

```sh
# Replace PORT with the serial port reported by arduino-cli board list.
python3 MiniConsole/tools/build_doom.py --display tft --port PORT --assets --fetch-assets
# Later updates only need the application:
python3 MiniConsole/tools/build_doom.py --display tft --port PORT
```

You can provide the exact supported asset using `--asset-file PATH` instead of downloading it. Board settings: ESP32S3 Dev Module, 4 MB DIO flash, USB hardware CDC, **QSPI PSRAM enabled**. The custom partition table preserves NVS and assigns separate Doom-data and native-save partitions. Back up your device before changing its partition layout.


[Back to project overview](../README.md).
