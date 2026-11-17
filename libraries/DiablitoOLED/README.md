# DIABLITO for MiniConsole

This directory is the complete local Arduino adaptation of DIABLITO used by MiniConsole. It is committed directly so fresh checkouts contain the fork's source. See `UPSTREAM.txt` for the base commit and `COPYING` for GPL terms; individual source headers retain additional upstream licensing.

The original ESP32-C6 port is adapted for ESP32-S3 with QSPI PSRAM, SSD1306 OLED and ST7789 TFT output, six-button controls, launcher lifecycle, native save/load and simplified GPIO4 piezo sound events. Original sampled audio and OPL2 music are disabled in this adaptation. Doom's game data remains a separate download, documented in the repository README.

Host tests replace Doom with a stub. Firmware compilation and physical gameplay are separate checks.
