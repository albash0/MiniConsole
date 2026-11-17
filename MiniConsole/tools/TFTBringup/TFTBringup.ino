#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

Adafruit_ST7789 panel(&SPI, -1, 6, 1);
const uint8_t pins[] = {7, 2, 10, 11, 12, 13};
const char *names[] = {"A", "B", "UP", "RIGHT", "LEFT", "DOWN"};
uint32_t last = 0;
void setup() {
  Serial.begin(115200);
  pinMode(43, OUTPUT); digitalWrite(43, LOW);
  for (auto pin : pins) pinMode(pin, INPUT_PULLUP);
  SPI.begin(8, -1, 9, -1);
  panel.init(135, 240, SPI_MODE3); panel.setSPISpeed(10000000); panel.setRotation(3);
  panel.fillScreen(ST77XX_BLACK); panel.setTextWrap(false);
  panel.setTextSize(2); panel.setTextColor(ST77XX_WHITE); panel.setCursor(12, 9); panel.print("MINICONSOLE TFT");
  panel.setTextSize(1); panel.setCursor(12, 32); panel.print("240x135  ST7789  CS=-1");
  uint16_t colors[] = {ST77XX_RED, ST77XX_GREEN, ST77XX_BLUE, ST77XX_YELLOW, ST77XX_CYAN, ST77XX_MAGENTA};
  for (int i = 0; i < 6; ++i) panel.fillRect(i * 40, 45, 40, 22, colors[i]);
  panel.drawRect(0, 0, 240, 135, ST77XX_WHITE);
  digitalWrite(43, HIGH);
  Serial.printf("TFT probe: SCK8 MOSI9 DC6 RST1 BL43 CS=-1; flash=%u PSRAM=%u\n", ESP.getFlashChipSize(), ESP.getPsramSize());
}
void loop() {
  static uint8_t previous = 255;
  uint8_t mask = 0;
  for (int i = 0; i < 6; ++i) if (!digitalRead(pins[i])) mask |= 1 << i;
  if (mask != previous) {
    previous = mask;
    panel.fillRect(2, 75, 236, 57, ST77XX_BLACK);
    panel.setTextSize(1); panel.setTextColor(ST77XX_WHITE);
    for (int i = 0; i < 6; ++i) {
      int x = 10 + (i % 3) * 78, y = 81 + (i / 3) * 24;
      panel.setCursor(x, y); panel.setTextColor(mask & (1 << i) ? ST77XX_GREEN : ST77XX_WHITE); panel.print(names[i]);
    }
    Serial.printf("TFT probe buttons=%02X\n", mask);
  }
  if (millis() - last >= 3000) { last = millis(); Serial.println("TFT probe running"); }
  delay(10);
}
