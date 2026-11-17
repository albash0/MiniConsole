#pragma once
// PCB-TFT114 V1.3 wiring; 3.3 V MSP1141/ST7789 module.
// SCL/SDA are SPI SCK/MOSI, not I2C signals.
namespace MiniConsoleTFT114 {
constexpr int SCK = 8;
constexpr int MOSI = 9;
constexpr int RESET = 1;
constexpr int DC = 6;
constexpr int CS = -1;        // J1 CS tied to GND; sole SPI device, always selected.
constexpr int IIC_SCL = 44;
constexpr int IIC_SDA = 5;    // Wire.begin(IIC_SDA, IIC_SCL), independent of TFT SPI.
constexpr int BACKLIGHT = 43;  // BLK controls the module's on-board transistor.
constexpr int PIEZO = 4;      // Via 220-ohm R1; sound hardware is optional.
constexpr int A = 7, B = 2;
constexpr int UP = 10, RIGHT = 11, LEFT = 12, DOWN = 13;
constexpr int PANEL_WIDTH = 135, PANEL_HEIGHT = 240;
// Actual module verified with SPI_MODE3 and landscape base rotation 3.
// Build the shared MiniConsole sketch with tools/build_doom.py --display tft.
}
