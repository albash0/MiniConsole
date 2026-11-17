#pragma once
#include <stdint.h>
#include <cmath>

// Coordinates are OLED pixels. One simulation tick is 16 ms.
struct BreakoutGame {
  static constexpr int COLS = 6, ROWS = 4, PADDLE_Y = 59, PADDLE_W = 22;
  bool bricks[ROWS][COLS] = {}, alive = true, serving = true;
  float x = 64, y = 56, vx = 0.4f, vy = -0.7f, paddle = 64;
  uint32_t score = 0;
  uint16_t level = 1;
  uint8_t lives = 3, remaining = 24, flash = 0;
  int8_t hitRow = -1, hitCol = -1;

  static int brickX(int col) { return 5 + col * 20; }
  static int brickY(int row) { return 14 + row * 6; }
  void rack() {
    remaining = 0;
    for (int r = 0; r < ROWS; ++r) for (int c = 0; c < COLS; ++c) {
      // Alternate full racks and a staggered pattern.
      bricks[r][c] = (level % 2) || ((r + c) % 3 != 0);
      remaining += bricks[r][c];
    }
    serving = true; flash = 0;
  }
  void reset() {
    score = 0; level = 1; lives = 3; alive = true; paddle = 64;
    x = 64; y = 56; rack();
  }
  void serve() {
    if (!serving || !alive) return;
    serving = false;
    float speed = 0.85f + (level > 8 ? 8 : level) * 0.055f;
    vx = speed * 0.45f; vy = -speed * 0.89f;
  }
  void step(int direction, bool launch) {
    if (!alive) return;
    paddle += direction * 1.65f;
    if (paddle < 12) paddle = 12;
    if (paddle > 116) paddle = 116;
    if (flash) --flash;
    if (serving) { x = paddle; y = 56; if (launch) serve(); return; }
    float oldX = x, oldY = y;
    x += vx; y += vy;
    if (x < 2) { x = 2; vx = std::fabs(vx); }
    if (x > 125) { x = 125; vx = -std::fabs(vx); }
    if (y < 12) { y = 12; vy = std::fabs(vy); }
    if (vy > 0 && oldY + 1 < PADDLE_Y && y + 1 >= PADDLE_Y &&
        x + 1 >= paddle - PADDLE_W / 2 && x - 1 <= paddle + PADDLE_W / 2) {
      y = PADDLE_Y - 1.1f;
      float speed = std::sqrt(vx * vx + vy * vy);
      float offset = (x - paddle) / (PADDLE_W / 2 + 1);
      if (offset < -0.9f) offset = -0.9f;
      if (offset > 0.9f) offset = 0.9f;
      vx = speed * offset;
      if (std::fabs(vx) < 0.15f) vx = vx < 0 ? -0.15f : 0.15f;
      vy = -std::sqrt(speed * speed - vx * vx);
    }
    for (int r = 0; r < ROWS; ++r) for (int c = 0; c < COLS; ++c) {
      int bx = brickX(c), by = brickY(r);
      if (!bricks[r][c] || x + 1 < bx || x - 1 > bx + 17 || y + 1 < by || y - 1 > by + 3) continue;
      bricks[r][c] = false; --remaining; score += (ROWS - r) * 10;
      hitRow = r; hitCol = c; flash = 6;
      if (oldY + 1 < by || oldY - 1 > by + 3) vy = -vy;
      else { vx = -vx; x = oldX; }
      if (!remaining) { score += 100 * level; ++level; rack(); }
      return;
    }
    if (y > 65) {
      if (--lives == 0) alive = false;
      else serving = true;
    }
  }
};
