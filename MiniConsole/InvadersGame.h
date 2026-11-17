#pragma once
#include <stdint.h>

struct InvadersGame {
  static constexpr int COLS = 6, ROWS = 3;
  struct Shot { float x = 0, y = -10; bool active = false; };
  bool aliens[ROWS][COLS] = {}, alive = true;
  Shot shots[3], bombs[3];
  uint8_t shields[3][4] = {};
  float player = 64;
  int formationX = 13, formationY = 14, direction = 1;
  uint32_t score = 0, rng = 1;
  uint16_t wave = 1, marchClock = 0, bombClock = 0, fireClock = 0, waveWait = 0;
  uint8_t lives = 3, remaining = 18, invincible = 0, explosion = 0;
  int explosionX = 0, explosionY = 0;
  static int shieldX(int i) { return 20 + i * 40; }
  uint32_t random() { rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5; return rng; }
  int alienX(int c) const { return formationX + c * 16; }
  int alienY(int r) const { return formationY + r * 8; }
  void startWave() {
    formationX = 13; formationY = 14; direction = 1; remaining = 18;
    marchClock = bombClock = fireClock = 0;
    for (auto &row : aliens) for (auto &a : row) a = true;
    for (auto &s : shots) s.active = false;
    for (auto &b : bombs) b.active = false;
    for (auto &s : shields) { s[0] = 0x3C; s[1] = 0x7E; s[2] = 0xFF; s[3] = 0xC3; }
  }
  void reset(uint32_t seed) {
    rng = seed ? seed : 1; score = 0; wave = 1; lives = 3; player = 64;
    alive = true; invincible = explosion = waveWait = 0; startWave();
  }
  bool shieldHit(int x, int y) {
    if (y < 48 || y > 51) return false;
    for (int i = 0; i < 3; ++i) {
      int col = x - shieldX(i);
      if (col >= 0 && col < 8 && (shields[i][y - 48] & (1u << col))) {
        shields[i][y - 48] &= ~(1u << col);
        // A small crater makes damage readable at this resolution.
        if (col > 0) shields[i][y - 48] &= ~(1u << (col - 1));
        if (col < 7) shields[i][y - 48] &= ~(1u << (col + 1));
        return true;
      }
    }
    return false;
  }
  void step(int move, bool fire) {
    if (!alive) return;
    if (invincible) --invincible;
    if (explosion) --explosion;
    player += move * 1.35f;
    if (player < 5) player = 5;
    if (player > 122) player = 122;
    if (waveWait) { if (--waveWait == 0) startWave(); return; }
    if (fireClock) --fireClock;
    if (fire && !fireClock) for (auto &s : shots) if (!s.active) {
      s = {player, 56, true}; fireClock = 14; break;
    }
    for (auto &s : shots) if (s.active) {
      s.y -= 1.5f;
      // Test both pixels traversed so bullets cannot skip a shield row.
      if (s.y < 11 || shieldHit(int(s.x), int(s.y)) || shieldHit(int(s.x), int(s.y + 1))) { s.active = false; continue; }
      for (int r = 0; r < ROWS && s.active; ++r) for (int c = 0; c < COLS && s.active; ++c) {
        if (aliens[r][c] && s.x >= alienX(c) && s.x <= alienX(c) + 7 && s.y <= alienY(r) + 4 && s.y + 2 >= alienY(r)) {
          aliens[r][c] = false; s.active = false; --remaining; score += (ROWS - r) * 10;
          explosion = 6; explosionX = alienX(c); explosionY = alienY(r);
        }
      }
    }
    if (!remaining) {
      score += 100 * wave; ++wave; waveWait = 75;
      for (auto &b : bombs) b.active = false;
      for (auto &s : shots) s.active = false;
      return;
    }
    int marchEvery = 8 + remaining * 2 - (wave > 6 ? 6 : wave);
    if (++marchClock >= marchEvery) {
      marchClock = 0;
      int left = 128, right = 0, bottom = 0;
      for (int r = 0; r < ROWS; ++r) for (int c = 0; c < COLS; ++c) if (aliens[r][c]) {
        if (alienX(c) < left) left = alienX(c);
        if (alienX(c) + 7 > right) right = alienX(c) + 7;
        if (alienY(r) + 4 > bottom) bottom = alienY(r) + 4;
      }
      bool descend = (direction > 0 && right + 2 > 126) || (direction < 0 && left - 2 < 1);
      if (descend) {
        direction = -direction; formationY += 3;
      } else formationX += direction * 2;
      if (bottom + (descend ? 3 : 0) >= 56) { alive = false; return; }
    }
    int bombEvery = 68 - (wave > 10 ? 10 : wave) * 3;
    if (++bombClock >= bombEvery) {
      bombClock = 0;
      int start = random() % COLS;
      for (int n = 0; n < COLS; ++n) {
        int c = (start + n) % COLS, r = ROWS - 1;
        while (r >= 0 && !aliens[r][c]) --r;
        if (r < 0) continue;
        for (auto &b : bombs) if (!b.active) { b = {float(alienX(c) + 3), float(alienY(r) + 5), true}; break; }
        break;
      }
    }
    for (auto &b : bombs) if (b.active) {
      b.y += 0.65f;
      if (b.y > 64 || shieldHit(int(b.x), int(b.y))) { b.active = false; continue; }
      if (b.y >= 57 && b.y <= 62 && b.x >= player - 4 && b.x <= player + 4) {
        b.active = false;
        if (!invincible) {
          if (--lives == 0) { alive = false; return; }
          invincible = 75;
        }
      }
    }
  }
};
