#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <cmath>
#include "SnakeGame.h"
#include "BlocksGame.h"
#include "BreakoutGame.h"
#include "InvadersGame.h"
#include "SokobanGame.h"
#include "MotorwaysGame.h"

// One portable, versioned checkpoint. No pointers, struct padding or absolute
// millis() values go into flash. Only the selected game's state is encoded.
struct SuspendState {
  uint8_t gameId = 0, difficulty = 1;
  uint32_t stepAge = 0, fallAge = 0, groundAge = 0, noticeAge = 1200, animationAge = 0;
  uint32_t noticePoints = 0;
  uint8_t noticeLines = 0, lockResets = 0;
  bool clearAnimating = false, groundActive = false;
  SnakeGame snake;
  BlocksGame blocks;
  BreakoutGame breakout;
  InvadersGame invaders;
  SokobanGame sokoban;
  MotorwaysGame motorways;
};

namespace SuspendSave {
constexpr size_t CAPACITY = 2048, HEADER = 16;
constexpr uint32_t MAGIC = 0x50535553;
constexpr uint16_t VERSION = 1;

struct Writer {
  uint8_t *data; size_t capacity, pos = 0; bool good = true;
  void u8(uint8_t n) { if (pos < capacity) data[pos++] = n; else good = false; }
  void u16(uint16_t n) { u8(n); u8(n >> 8); }
  void u32(uint32_t n) { u16(n); u16(n >> 16); }
  void u64(uint64_t n) { u32(n); u32(n >> 32); }
  void real(float f) { uint32_t n; static_assert(sizeof(f) == 4, "32-bit float required"); memcpy(&n, &f, 4); u32(n); }
};
struct Reader {
  const uint8_t *data; size_t size, pos = 0; bool good = true;
  uint8_t u8() { if (pos < size) return data[pos++]; good = false; return 0; }
  bool flag() { uint8_t n = u8(); if (n > 1) good = false; return n == 1; }
  uint16_t u16() { uint16_t lo = u8(); return lo | uint16_t(u8()) << 8; }
  uint32_t u32() { uint32_t lo = u16(); return lo | uint32_t(u16()) << 16; }
  uint64_t u64() { uint64_t lo = u32(); return lo | uint64_t(u32()) << 32; }
  float real() { uint32_t n = u32(); float f; memcpy(&f, &n, 4); return f; }
};
inline uint32_t checksum(const uint8_t *data, size_t size) {
  uint32_t crc = 0xFFFFFFFF;
  for (size_t i = 0; i < size; ++i) {
    if (i >= 12 && i < 16) continue;
    crc ^= data[i];
    for (int b = 0; b < 8; ++b) crc = (crc >> 1) ^ (0xEDB88320u & (0u - (crc & 1)));
  }
  return ~crc;
}
inline bool snakeCell(SnakeGame::Cell c) { return c.x >= 0 && c.x < 30 && c.y >= 0 && c.y < 12; }
inline bool finite(float f, float lo, float hi) { return std::isfinite(f) && f >= lo && f <= hi; }
inline bool valid(const SuspendState &s) {
  if ((s.gameId >= 5 && s.gameId != 6) || s.difficulty > 2 || s.stepAge > 10000 || s.fallAge > 10000 || s.groundAge > 10000 ||
      s.noticeAge > 1200 || s.noticeLines > 4 || s.lockResets > 15 || s.animationAge > (s.noticeLines == 4 ? 650u : 420u)) return false;
  if (s.gameId == 6) return s.stepAge<50 && s.difficulty==s.motorways.difficulty && s.motorways.valid();
  if (s.gameId == 0) {
    const auto &g = s.snake;
    if (!g.alive || g.length < 4 || g.length >= g.CAPACITY || g.direction > 3 || g.turnCount > 2 || !snakeCell(g.food)) return false;
    for (int i = 0; i < g.turnCount; ++i) if (g.turns[i] > 3) return false;
    for (int i = 0; i < g.length; ++i) {
      if (!snakeCell(g.body[i]) || SnakeGame::equal(g.body[i], g.food)) return false;
      for (int j = 0; j < i; ++j) if (SnakeGame::equal(g.body[i], g.body[j])) return false;
    }
  } else if (s.gameId == 1) {
    const auto &g = s.blocks;
    if ((!g.alive && !s.clearAnimating) || g.piece >= 7 || g.next >= 7 || g.rotation >= 4 || g.bagIndex > 7 ||
        !g.randomState || g.x < -3 || g.x > 9 || g.y < -4 || g.y > 19 || g.lastCleared > 4 || (g.clearedRows >> 20)) return false;
    uint8_t seen = 0;
    for (uint8_t p : g.bag) { if (p >= 7 || (seen & (1u << p))) return false; seen |= 1u << p; }
    for (int r = 0; r < 20; ++r) for (int c = 0; c < 10; ++c) if (g.board[r][c] > 7 || g.clearBoard[r][c] > 7) return false;
    if (g.alive && !g.fits(g.x, g.y, g.rotation)) return false;
    if (s.clearAnimating && (!s.noticeLines || g.lastCleared != s.noticeLines || !g.clearedRows)) return false;
  } else if (s.gameId == 2) {
    const auto &g = s.breakout; int count = 0;
    for (const auto &row : g.bricks) for (bool b : row) count += b;
    if (!g.alive || !g.level || !g.lives || g.lives > 3 || !count || count != g.remaining ||
        !finite(g.x, 0, 127) || !finite(g.y, 10, 68) || !finite(g.vx, -10, 10) || !finite(g.vy, -10, 10) || !finite(g.paddle, 12, 116)) return false;
    if (g.flash && (g.hitRow < 0 || g.hitRow >= 4 || g.hitCol < 0 || g.hitCol >= 6)) return false;
  } else if (s.gameId == 3) {
    const auto &g = s.invaders; int count = 0;
    for (const auto &row : g.aliens) for (bool a : row) count += a;
    if (!g.alive || !g.wave || !g.rng || !g.lives || g.lives > 3 || count != g.remaining ||
        (!count && !g.waveWait) || g.waveWait > 75 || g.invincible > 75 || !finite(g.player, 5, 122) ||
        g.formationX < -80 || g.formationX > 126 || g.formationY < 14 || g.formationY > 55 || (g.direction != -1 && g.direction != 1)) return false;
    for (const auto &shot : g.shots) if (!finite(shot.x, -10, 140) || !finite(shot.y, -16, 80)) return false;
    for (const auto &bomb : g.bombs) if (!finite(bomb.x, -10, 140) || !finite(bomb.y, -16, 80)) return false;
  } else {
    const auto &g = s.sokoban;
    if (g.level >= 10 || g.historyHead >= 128 || g.historyCount > 128) return false;
    uint64_t walls = 0, goals = 0;
    const char *map = SokobanGame::layout(g.level);
    for (int i = 0; i < 48; ++i) {
      if (map[i] == '#') walls |= SokobanGame::cellMask(i);
      if (map[i] == '.' || map[i] == '*' || map[i] == '+') goals |= SokobanGame::cellMask(i);
    }
    if (g.walls != walls || g.goals != goals || g.solved()) return false;
    auto positionValid = [walls, goals](uint64_t boxes, uint8_t player) {
      return !(boxes >> 48) && !(boxes & walls) && player < 48 &&
             !((boxes | walls) & SokobanGame::cellMask(player)) && __builtin_popcountll(boxes) == __builtin_popcountll(goals);
    };
    if (!positionValid(g.boxes, g.player)) return false;
    for (int i = 0; i < g.historyCount; ++i) {
      const auto &h = g.history[(g.historyHead + 128 - g.historyCount + i) % 128];
      if (!positionValid(h.boxes, h.player)) return false;
    }
  }
  return true;
}

inline void writeMotorways(Writer &w, const MotorwaysGame &g) {
  w.u32(g.rng); w.u32(g.ticks); w.u32(g.score); w.u16(g.stock);
  w.u8(g.bridges);w.u8(g.express);w.u8(g.difficulty);w.u8(g.cursor);w.u8(g.tool);w.u8(g.alive);w.u8(g.rewardPending);
  for(auto n:g.roads)w.u8(n);for(auto n:g.junction)w.u8(n);
  w.u8(g.homeCount);w.u8(g.destCount);w.u8(g.linkCount);
  for(int i=0;i<g.homeCount;++i){auto &h=g.homes[i];w.u8(h.cell);w.u8(h.color);w.u8(h.cooldown);}
  for(int i=0;i<g.destCount;++i){auto &d=g.destinations[i];w.u8(d.anchor);w.u8(d.color);w.u8(d.demand);w.u8(d.flash);w.u16(d.clock);w.u16(d.overload);}
  for(int i=0;i<g.linkCount;++i){w.u8(g.links[i].a);w.u8(g.links[i].b);}
  for(auto &c:g.cars){w.u8(c.active);if(c.active){w.u8(c.from);w.u8(c.to);w.u8(c.home);w.u8(c.dest);w.u8(c.progress);w.u8(c.hold);w.u8(c.returning);}}
  // Optional tail preserves all original game IDs and legacy checkpoints.
  w.u8(3);w.u32(uint32_t(g.ticks>>32));w.u8(g.systemTheme);
  for(auto n:g.grace)w.u16(n);
  w.u8(g.closing);w.u8(g.replacement);w.u8(g.replacementColor);w.u8(g.nextRenew);w.u16(g.closeClock);w.u32(g.renewals);
}
inline bool readMotorways(Reader &r, MotorwaysGame &g) {
  g=MotorwaysGame{};
  g.rng=r.u32();g.ticks=r.u32();g.score=r.u32();g.stock=r.u16();
  g.bridges=r.u8();g.express=r.u8();g.difficulty=r.u8();g.cursor=r.u8();g.tool=MotorwaysGame::Tool(r.u8());g.alive=r.flag();g.rewardPending=r.flag();
  for(auto &n:g.roads)n=r.u8();for(auto &n:g.junction)n=r.u8();
  g.homeCount=r.u8();g.destCount=r.u8();g.linkCount=r.u8();
  if(g.homeCount>g.HOMES||g.destCount>g.DESTS||g.linkCount>g.LINKS)return false;
  for(int i=0;i<g.homeCount;++i){auto &h=g.homes[i];h.cell=r.u8();h.color=r.u8();h.cooldown=r.u8();}
  for(int i=0;i<g.destCount;++i){auto &d=g.destinations[i];d.anchor=r.u8();d.color=r.u8();d.demand=r.u8();d.flash=r.u8();d.clock=r.u16();d.overload=r.u16();}
  for(int i=0;i<g.linkCount;++i){g.links[i].a=r.u8();g.links[i].b=r.u8();}
  for(auto &c:g.cars){c.active=r.flag();if(c.active){c.from=r.u8();c.to=r.u8();c.home=r.u8();c.dest=r.u8();c.progress=r.u8();c.hold=r.u8();c.returning=r.flag();}}
  uint8_t motorVersion=0;
  if(r.pos<r.size){
    motorVersion=r.u8();if(motorVersion<1||motorVersion>3)return false;
    g.ticks|=uint64_t(r.u32())<<32;g.systemTheme=r.flag();
    for(auto &n:g.grace)n=r.u16();
    g.closing=r.u8();g.replacement=r.u8();g.replacementColor=r.u8();g.nextRenew=r.u8();g.closeClock=r.u16();g.renewals=r.u32();
  }
  if(motorVersion<2){
    // Preserve an existing warning's fraction when loading the older, longer
    // congestion timers. Geometry, roads, cars and score stay as saved.
    uint16_t oldLimit=g.difficulty==0?220:g.difficulty==2?120:160;
    for(int i=0;i<g.destCount;++i)g.destinations[i].overload=uint32_t(g.destinations[i].overload)*g.overloadLimit()/oldLimit;
  }
  return r.good;
}

inline size_t encode(const SuspendState &s, uint8_t *data, size_t capacity = CAPACITY) {
  if (!valid(s)) return 0;
  Writer w{data, capacity};
  w.u32(MAGIC); w.u16(VERSION); w.u8(s.gameId); w.u8(s.difficulty); w.u16(0); w.u16(0); w.u32(0);
  w.u32(s.stepAge); w.u32(s.fallAge); w.u32(s.groundAge); w.u32(s.noticeAge); w.u32(s.animationAge);
  w.u32(s.noticePoints); w.u8(s.noticeLines); w.u8(s.lockResets); w.u8(s.clearAnimating); w.u8(s.groundActive);
  if (s.gameId == 6) writeMotorways(w,s.motorways);
  else if (s.gameId == 0) {
    const auto &g = s.snake;
    w.u16(g.length); w.u8(g.direction); w.u8(g.turnCount); w.u8(g.wrap); w.u8(g.alive);
    w.u8(g.food.x); w.u8(g.food.y);
    for (int i = 0; i < 2; ++i) w.u8(i < g.turnCount ? g.turns[i] : 0);
    for (int i = 0; i < g.length; ++i) { w.u8(g.body[i].x); w.u8(g.body[i].y); }
  } else if (s.gameId == 1) {
    const auto &g = s.blocks;
    for (const auto &row : g.board) for (uint8_t c : row) w.u8(c);
    for (const auto &row : g.clearBoard) for (uint8_t c : row) w.u8(c);
    w.u32(g.clearedRows); w.u8(g.piece); w.u8(g.next); w.u8(g.rotation); w.u8(g.x); w.u8(g.y); w.u8(g.alive);
    w.u32(g.score); w.u16(g.lines); w.u8(g.lastCleared); w.u32(g.lastClearPoints);
    for (uint8_t p : g.bag) w.u8(p);
    w.u8(g.bagIndex); w.u32(g.randomState);
  } else if (s.gameId == 2) {
    const auto &g = s.breakout;
    for (const auto &row : g.bricks) for (bool b : row) w.u8(b);
    w.u8(g.alive); w.u8(g.serving); w.real(g.x); w.real(g.y); w.real(g.vx); w.real(g.vy); w.real(g.paddle);
    w.u32(g.score); w.u16(g.level); w.u8(g.lives); w.u8(g.remaining); w.u8(g.flash); w.u8(g.hitRow); w.u8(g.hitCol);
  } else if (s.gameId == 3) {
    const auto &g = s.invaders;
    for (const auto &row : g.aliens) for (bool a : row) w.u8(a);
    w.u8(g.alive);
    for (const auto &shot : g.shots) { w.real(shot.x); w.real(shot.y); w.u8(shot.active); }
    for (const auto &bomb : g.bombs) { w.real(bomb.x); w.real(bomb.y); w.u8(bomb.active); }
    for (const auto &shield : g.shields) for (uint8_t row : shield) w.u8(row);
    w.real(g.player); w.u16(g.formationX); w.u16(g.formationY); w.u8(g.direction);
    w.u32(g.score); w.u32(g.rng); w.u16(g.wave); w.u16(g.marchClock); w.u16(g.bombClock); w.u16(g.fireClock); w.u16(g.waveWait);
    w.u8(g.lives); w.u8(g.remaining); w.u8(g.invincible); w.u8(g.explosion); w.u16(g.explosionX); w.u16(g.explosionY);
  } else {
    const auto &g = s.sokoban;
    w.u64(g.walls); w.u64(g.goals); w.u64(g.boxes); w.u8(g.player); w.u8(g.level); w.u16(g.moves); w.u16(g.pushes);
    w.u8(g.historyHead); w.u8(g.historyCount);
    for (int i = 0; i < g.historyCount; ++i) {
      const auto &h = g.history[(g.historyHead + 128 - g.historyCount + i) % 128];
      w.u64(h.boxes); w.u16(h.moves); w.u16(h.pushes); w.u8(h.player);
    }
  }
  if (!w.good) return 0;
  size_t size = w.pos;
  data[8] = (size - HEADER) & 255; data[9] = (size - HEADER) >> 8;
  uint32_t crc = checksum(data, size); w.pos = 12; w.u32(crc);
  return size;
}

inline bool decode(const uint8_t *data, size_t size, SuspendState &s) {
  if (size < HEADER || size > CAPACITY) return false;
  Reader r{data, size};
  if (r.u32() != MAGIC || r.u16() != VERSION) return false;
  s.gameId = r.u8(); s.difficulty = r.u8();
  if ((s.gameId >= 5 && s.gameId != 6) || s.difficulty > 2 || r.u16() != size - HEADER || r.u16() != 0 || r.u32() != checksum(data, size)) return false;
  s.stepAge = r.u32(); s.fallAge = r.u32(); s.groundAge = r.u32(); s.noticeAge = r.u32(); s.animationAge = r.u32();
  s.noticePoints = r.u32(); s.noticeLines = r.u8(); s.lockResets = r.u8(); s.clearAnimating = r.flag(); s.groundActive = r.flag();
  if (s.gameId == 6) { if(!readMotorways(r,s.motorways))return false; }
  else if (s.gameId == 0) {
    auto &g = s.snake;
    g.length = r.u16(); if (g.length < 4 || g.length >= g.CAPACITY) return false;
    g.direction = SnakeGame::Direction(r.u8()); g.turnCount = r.u8(); g.wrap = r.flag(); g.alive = r.flag();
    g.food.x = r.u8(); g.food.y = r.u8();
    for (auto &t : g.turns) t = SnakeGame::Direction(r.u8());
    for (int i = 0; i < g.length; ++i) { g.body[i].x = r.u8(); g.body[i].y = r.u8(); }
  } else if (s.gameId == 1) {
    auto &g = s.blocks;
    for (auto &row : g.board) for (auto &c : row) c = r.u8();
    for (auto &row : g.clearBoard) for (auto &c : row) c = r.u8();
    g.clearedRows = r.u32(); g.piece = r.u8(); g.next = r.u8(); g.rotation = r.u8(); g.x = int8_t(r.u8()); g.y = int8_t(r.u8()); g.alive = r.flag();
    g.score = r.u32(); g.lines = r.u16(); g.lastCleared = r.u8(); g.lastClearPoints = r.u32();
    for (auto &p : g.bag) p = r.u8();
    g.bagIndex = r.u8(); g.randomState = r.u32();
  } else if (s.gameId == 2) {
    auto &g = s.breakout;
    for (auto &row : g.bricks) for (auto &b : row) b = r.flag();
    g.alive = r.flag(); g.serving = r.flag(); g.x = r.real(); g.y = r.real(); g.vx = r.real(); g.vy = r.real(); g.paddle = r.real();
    g.score = r.u32(); g.level = r.u16(); g.lives = r.u8(); g.remaining = r.u8(); g.flash = r.u8(); g.hitRow = int8_t(r.u8()); g.hitCol = int8_t(r.u8());
  } else if (s.gameId == 3) {
    auto &g = s.invaders;
    for (auto &row : g.aliens) for (auto &a : row) a = r.flag();
    g.alive = r.flag();
    for (auto &shot : g.shots) { shot.x = r.real(); shot.y = r.real(); shot.active = r.flag(); }
    for (auto &bomb : g.bombs) { bomb.x = r.real(); bomb.y = r.real(); bomb.active = r.flag(); }
    for (auto &shield : g.shields) for (auto &row : shield) row = r.u8();
    g.player = r.real(); g.formationX = int16_t(r.u16()); g.formationY = int16_t(r.u16()); g.direction = int8_t(r.u8());
    g.score = r.u32(); g.rng = r.u32(); g.wave = r.u16(); g.marchClock = r.u16(); g.bombClock = r.u16(); g.fireClock = r.u16(); g.waveWait = r.u16();
    g.lives = r.u8(); g.remaining = r.u8(); g.invincible = r.u8(); g.explosion = r.u8(); g.explosionX = int16_t(r.u16()); g.explosionY = int16_t(r.u16());
  } else {
    auto &g = s.sokoban;
    g.walls = r.u64(); g.goals = r.u64(); g.boxes = r.u64(); g.player = r.u8(); g.level = r.u8(); g.moves = r.u16(); g.pushes = r.u16();
    g.historyHead = r.u8(); g.historyCount = r.u8();
    if (g.historyHead >= 128 || g.historyCount > 128) return false;
    for (int i = 0; i < g.historyCount; ++i) {
      auto &h = g.history[(g.historyHead + 128 - g.historyCount + i) % 128];
      h.boxes = r.u64(); h.moves = r.u16(); h.pushes = r.u16(); h.player = r.u8();
    }
  }
  return r.good && r.pos == size && valid(s);
}
} // namespace SuspendSave
