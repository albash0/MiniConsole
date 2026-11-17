// Temporary hardware test. No Preferences, filesystem or flash writes.
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

constexpr uint8_t PIEZO = 4, BUTTON_A = 2, BUTTON_B = 7;
constexpr uint16_t tones[] = {2000, 3000, 4000};
constexpr uint32_t ON_MS = 350, GAP_MS = 200;
Adafruit_ST7789 panel(&SPI, -1, 6, 1);
bool attached = false, playing = false, toneOn = false;
bool lastA = false, lastB = false, stableA = false, stableB = false;
uint8_t step = 0;
uint32_t phaseAt = 0, changedA = 0, changedB = 0, heartbeat = 0;

void status(const char *message, uint16_t color = ST77XX_WHITE) {
  panel.fillRect(10, 45, 220, 44, ST77XX_BLACK);
  panel.setTextSize(2); panel.setTextColor(color); panel.setCursor(12, 51); panel.print(message);
}

void silence() {
  if (attached) { ledcWrite(PIEZO, 0); ledcDetach(PIEZO); attached = false; }
  pinMode(PIEZO, OUTPUT); digitalWrite(PIEZO, LOW);
  playing = toneOn = false;
}

void startTone(uint32_t now) {
  uint32_t actual = ledcWriteTone(PIEZO, tones[step]);
  if (!actual) { silence(); status("PWM ERROR", ST77XX_RED); Serial.println("PIEZO_ERROR tone"); return; }
  phaseAt = now; toneOn = true;
  char label[20]; snprintf(label, sizeof(label), "%u Hz", unsigned(actual)); status(label, ST77XX_CYAN);
  Serial.printf("PIEZO_TONE GPIO4 requested=%u actual=%lu duty=%lu\n", unsigned(tones[step]), (unsigned long)actual, (unsigned long)ledcRead(PIEZO));
}

void startTest() {
  silence();
  attached = ledcAttach(PIEZO, 4000, 10);
  if (!attached) { status("PWM ERROR", ST77XX_RED); Serial.println("PIEZO_ERROR attach"); return; }
  playing = true; step = 0; Serial.println("PIEZO_BEGIN GPIO4 R1=220R three tones, 350ms each"); startTone(millis());
}

bool pressed(uint8_t pin, bool &last, bool &stable, uint32_t &changed, uint32_t now) {
  bool raw = digitalRead(pin) == LOW;
  if (raw != last) { last = raw; changed = now; }
  if (now - changed >= 20 && raw != stable) { stable = raw; return raw; }
  return false;
}

void setup() {
  silence(); pinMode(BUTTON_A, INPUT_PULLUP); pinMode(BUTTON_B, INPUT_PULLUP);
  Serial.begin(115200);
  pinMode(43, OUTPUT); digitalWrite(43, LOW); SPI.begin(8, -1, 9, -1);
  panel.init(135, 240, SPI_MODE3); panel.setSPISpeed(40000000); panel.setRotation(3);
  panel.fillScreen(ST77XX_BLACK); panel.setTextWrap(false); panel.setTextSize(2);
  panel.setTextColor(ST77XX_WHITE); panel.setCursor(12, 12); panel.print("PIEZO TEST");
  panel.setTextSize(1); panel.setCursor(12, 98); panel.print("A: play    B: stop");
  panel.setCursor(12, 116); panel.print("GPIO4 -> 220R -> piezo");
  status("READY", ST77XX_GREEN); digitalWrite(43, HIGH);
}

void loop() {
  uint32_t now = millis();
  while (Serial.available()) {
    char c = Serial.read();
    if (c == 't') startTest();
    else if (c == 's') { silence(); status("STOPPED"); Serial.println("PIEZO_STOP GPIO4 LOW"); }
    else if (c == '?') Serial.printf("PIEZO_READY pin=4 playing=%u; t=test s=stop; A=play B=stop\n", playing);
  }
  bool a = pressed(BUTTON_A,lastA,stableA,changedA,now);
  bool b = pressed(BUTTON_B,lastB,stableB,changedB,now);
  if (b) { silence(); status("STOPPED"); Serial.println("PIEZO_STOP GPIO4 LOW"); }
  else if (a && !playing) startTest();
  // Refresh after a serial-started test changes the phase clock.
  now = millis();
  if (playing && now - phaseAt >= (toneOn ? ON_MS : GAP_MS)) {
    if (toneOn) { ledcWrite(PIEZO, 0); toneOn = false; phaseAt = now; }
    else if (++step < sizeof(tones)/sizeof(tones[0])) startTone(now);
    else { silence(); status("DONE", ST77XX_GREEN); Serial.println("PIEZO_DONE GPIO4 LOW detached"); }
  }
  if (now - heartbeat >= 3000) { heartbeat = now; Serial.printf("PIEZO_READY pin=4 playing=%u uptime=%lus\n", playing, (unsigned long)(now/1000)); }
  delay(2);
}
