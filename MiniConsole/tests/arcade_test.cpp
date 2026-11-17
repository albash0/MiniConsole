#include "../BreakoutGame.h"
#include "../InvadersGame.h"
#include "../SokobanGame.h"
#include "sokoban_solutions.h"
#include <cassert>
#include <cstdio>

int main() {
  BreakoutGame b;
  b.reset();
  for (int i = 0; i < 300; ++i) b.step(-1, false);
  assert(b.paddle == 12 && b.x == b.paddle && b.serving);
  b.step(0, true); assert(!b.serving && b.vy < 0);
  b.x = 1.5f; b.vx = -1; b.y = 45; b.step(0, false);
  assert(b.x == 2 && b.vx > 0);
  b.paddle = 64; b.x = 69; b.y = 57.5; b.vy = 1; b.vx = 0;
  b.step(0, false); assert(b.vy < 0 && b.vx > 0 && b.y < 59);
  b.x = 10; b.y = 19; b.vx = 0; b.vy = -1;
  b.step(0, false); assert(!b.bricks[0][0] && b.score == 40 && b.vy > 0);
  for (auto &row : b.bricks) for (auto &brick : row) brick = false;
  b.bricks[0][0] = true; b.remaining = 1; b.x = 10; b.y = 19; b.vy = -1;
  b.step(0, false); assert(b.level == 2 && b.score == 180 && b.remaining == 16 && b.serving);
  for (int life = 2; life >= 0; --life) {
    b.serving = false; b.y = 66; b.x = 2; b.vy = 1;
    b.step(0, false); assert(b.lives == life && b.alive == (life != 0));
  }

  InvadersGame g;
  g.reset(123);
  g.step(0, true); assert(g.shots[0].active);
  for (int i = 0; i < 10; ++i) g.step(0, true);
  assert(!g.shots[1].active); // Hold-fire respects its cooldown.
  g.reset(123); g.shots[0] = {16, 19, true}; g.step(0, false);
  assert(!g.aliens[0][0] && g.score == 30 && g.remaining == 17);
  assert(g.shieldHit(22, 48) && g.shields[0][0] != 0x3C);
  assert(!g.shieldHit(0, 48));
  g.reset(123); g.bombs[0] = {64, 57, true}; g.bombs[1] = {64, 57, true};
  g.step(0, false); assert(g.lives == 2 && g.invincible == 75 && g.alive);
  g.invincible = 0; g.lives = 1;
  for (auto &bomb : g.bombs) bomb = {64, 57, true};
  g.step(0, false); assert(g.lives == 0 && !g.alive); // No unsigned underflow.
  g.reset(123);
  for (auto &row : g.aliens) for (auto &alien : row) alien = false;
  g.aliens[0][0] = true; g.remaining = 1; g.shots[0] = {16, 19, true};
  g.step(0, false); assert(g.wave == 2 && g.waveWait == 75 && g.score == 130);
  for (int i = 0; i < 75; ++i) g.step(0, false);
  assert(g.waveWait == 0 && g.remaining == 18);
  g.formationY = 51; g.formationX = 36; g.marchClock = 100;
  g.step(0, false); assert(!g.alive); // Reaching the ship ends the run.

  SokobanGame s;
  for (int level = 0; level < SokobanGame::LEVELS; ++level) {
    s.reset(level);
    assert(__builtin_popcountll(s.boxes) == __builtin_popcountll(s.goals));
    assert(!(s.boxes & s.walls) && !(s.walls & s.goals));
    uint64_t initial = s.boxes; uint8_t player = s.player;
    for (const char *c = SOKOBAN_SOLUTIONS[level]; *c; ++c) {
      assert(s.move(*c == 'L' ? -1 : *c == 'R' ? 1 : 0, *c == 'U' ? -1 : *c == 'D' ? 1 : 0));
    }
    assert(s.solved());
    while (s.undo()) {}
    assert(s.boxes == initial && s.player == player && s.moves == 0 && s.pushes == 0);
  }
  s.reset(0); assert(!s.move(-1, -1));
  s.move(0,-1); assert(!s.move(0,-1)); // Wall blocks a walk.
  s.reset(0); s.move(1,0); assert(s.pushes == 1);
  assert(s.undo() && s.pushes == 0 && s.moves == 0);
  s.reset(0); s.move(0,1);
  for (int i = 0; i < 200; ++i) assert(s.move(i % 2 ? -1 : 1, 0));
  assert(s.historyCount == SokobanGame::HISTORY);
  for (int i = 0; i < SokobanGame::HISTORY; ++i) assert(s.undo());
  assert(!s.undo() && s.moves == 73);
  // Many deterministic runs exercise edge collisions and bullet/brick lifetimes.
  uint32_t rng = 11;
  for (int seed = 1; seed <= 40; ++seed) {
    b.reset(); g.reset(seed);
    for (int t = 0; t < 8000; ++t) {
      rng = rng * 1664525u + 1013904223u;
      int direction = int(rng % 3) - 1;
      b.step(direction, true); g.step(direction, true);
      assert(b.lives <= 3 && g.lives <= 3 && b.remaining <= 24 && g.remaining <= 18);
      assert(std::isfinite(b.x) && std::isfinite(b.y) && std::isfinite(b.vx) && std::isfinite(b.vy));
    }
  }
  puts("Arcade collisions/lives/waves, all Sokoban solutions and undo passed.");
}
