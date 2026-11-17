#pragma once
#include <Adafruit_GFX.h>
#ifndef MINICONSOLE_HOST_TEST
#include <SPI.h>
#include <Adafruit_ST7789.h>
#include <esp_heap_caps.h>
#endif

#ifndef WHITE
#define WHITE 1
#define BLACK 0
#endif

// Preserve the games' logical 128x64/64x128 coordinates and rotations while
// presenting RGB565 at 240x135. All drawing is buffered; no per-pixel SPI calls.
class TFTNativeCanvas : public GFXcanvas16 {
public:
  TFTNativeCanvas() : GFXcanvas16(240, 135, false) {}
  void attach(uint16_t *pixels) { buffer = pixels; }
};

class ConsoleTFT114 : public Adafruit_GFX {
public:
  static constexpr int SCK = 8, MOSI = 9, RESET = 1, DC = 6, CS = -1, BACKLIGHT = 43;
  static constexpr int PHYSICAL_W = 240, PHYSICAL_H = 135;
  static constexpr uint16_t INK = 0xEF7D, BACKGROUND = 0x0841;
  ConsoleTFT114() : Adafruit_GFX(128, 64), mono_(128, 64), canvas_(128, 64)
#ifndef MINICONSOLE_HOST_TEST
    , panel_(&SPI, CS, DC, RESET)
#endif
  {}
  ~ConsoleTFT114() { free(frame_); }
  bool begin() {
    if (!mono_.getBuffer() || !canvas_.getBuffer()) return false;
    if (!frame_) {
#ifndef MINICONSOLE_HOST_TEST
      frame_ = static_cast<uint16_t *>(heap_caps_malloc(PHYSICAL_W * PHYSICAL_H * 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
#endif
      if (!frame_) frame_ = static_cast<uint16_t *>(malloc(PHYSICAL_W * PHYSICAL_H * 2));
    }
    if (!frame_) return false;
    native_.attach(frame_);
#ifndef MINICONSOLE_HOST_TEST
    pinMode(BACKLIGHT, OUTPUT); digitalWrite(BACKLIGHT, LOW);
    SPI.begin(SCK, -1, MOSI, -1);
    // This module is permanently selected (CS grounded); mode 3 was verified
    // on the user's panel. Mode 0 lights its backlight but produces no image.
    panel_.init(135, 240, SPI_MODE3);
    panel_.setSPISpeed(40000000);
    // The TFT PCB mounts the header at the top, so this is its fixed landscape
    // base. Games and Flip screen rotate the buffered image relative to it.
    panel_.setRotation(3); panel_.fillScreen(BACKGROUND);
    digitalWrite(BACKLIGHT, HIGH);
#endif
    clearDisplay(); compose(); return true;
  }
  void setRotation(uint8_t value) override {
    Adafruit_GFX::setRotation(value); mono_.setRotation(value); canvas_.setRotation(value);
  }
  void setInk(uint16_t value = INK) { ink_ = value; }
  void setBackground(uint16_t value) { background_ = value; }
  void clearDisplay() { fillScreen(BLACK); }
  void drawPixel(int16_t x, int16_t y, uint16_t color) override {
    mono_.drawPixel(x, y, color != BLACK); canvas_.drawPixel(x, y, resolve(color));
  }
  void drawFastHLine(int16_t x, int16_t y, int16_t width, uint16_t color) override {
    mono_.drawFastHLine(x, y, width, color != BLACK); canvas_.drawFastHLine(x, y, width, resolve(color));
  }
  void drawFastVLine(int16_t x, int16_t y, int16_t height, uint16_t color) override {
    mono_.drawFastVLine(x, y, height, color != BLACK); canvas_.drawFastVLine(x, y, height, resolve(color));
  }
  void fillScreen(uint16_t color) override {
    mono_.fillScreen(color != BLACK); canvas_.fillScreen(resolve(color));
  }
  bool getPixel(int16_t x, int16_t y) const { return mono_.getPixel(x, y); }
  // Debug screenshots use the logical image. This buffer is GFX row-major,
  // deliberately not SSD1306 page layout; Doom uses its own RGB565 callback.
  uint8_t *getBuffer() { return mono_.getBuffer(); }
  uint16_t *colorBuffer() { return canvas_.getBuffer(); }
  uint16_t *physicalBuffer() { return frame_; }
  TFTNativeCanvas &nativeCanvas() { return native_; }
  void compose() {
    if (!frame_) return;
    const uint16_t *source = canvas_.getBuffer();
    for (int y = 0; y < PHYSICAL_H; ++y) {
      uint16_t *row = frame_ + y * PHYSICAL_W;
      if (y < 7 || y >= 127) {
        for (int x = 0; x < PHYSICAL_W; ++x) row[x] = background_;
      } else {
        const uint16_t *src = source + ((y - 7) * 64 / 120) * 128;
        for (int x = 0; x < PHYSICAL_W; ++x) row[x] = src[x * 128 / PHYSICAL_W];
      }
    }
  }
  void presentPhysical() {
#ifndef MINICONSOLE_HOST_TEST
    if (frame_) panel_.drawRGBBitmap(0, 0, frame_, PHYSICAL_W, PHYSICAL_H);
#endif
  }
  void display() { compose(); presentPhysical(); }
  void composeDoom(const uint16_t *source, bool flip) {
    if (!frame_) return;
    if (!flip) memcpy(frame_, source, PHYSICAL_W * PHYSICAL_H * 2);
    else for (int i=0;i<PHYSICAL_W*PHYSICAL_H;++i) frame_[i]=source[PHYSICAL_W*PHYSICAL_H-1-i];
  }
  // Copy a composed Doom frame, then overlay only the app's logical HUD/pause
  // pixels. Source colors are host-endian RGB565; orientation is applied once.
  void displayDoom(const uint16_t *source, bool flip, bool paused, bool hasHud = true) {
    if (!frame_) return;
    const uint16_t *overlay = canvas_.getBuffer();
    for (int y = 0; y < PHYSICAL_H; ++y) for (int x = 0; x < PHYSICAL_W; ++x) {
      int sx = flip ? PHYSICAL_W - 1 - x : x, sy = flip ? PHYSICAL_H - 1 - y : y;
      frame_[y * PHYSICAL_W + x] = source[sy * PHYSICAL_W + sx];
      int rawX = x * 128 / PHYSICAL_W, rawY = y * 64 / PHYSICAL_H;
      int logicalX = flip ? 127 - rawX : rawX, logicalY = flip ? 63 - rawY : rawY;
      if ((hasHud && logicalY >= 54) || (paused && logicalX >= 9 && logicalX < 119 && logicalY >= 13 && logicalY < 63))
        frame_[y * PHYSICAL_W + x] = overlay[rawY * 128 + rawX];
    }
    presentPhysical();
  }
private:
  uint16_t resolve(uint16_t color) const { return color == BLACK ? background_ : color == WHITE ? ink_ : color; }
  GFXcanvas1 mono_;
  GFXcanvas16 canvas_;
  TFTNativeCanvas native_;
  uint16_t *frame_ = nullptr;
  uint16_t ink_ = INK, background_ = BACKGROUND;
#ifndef MINICONSOLE_HOST_TEST
  Adafruit_ST7789 panel_;
#endif
};
