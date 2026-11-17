#pragma once
#include <stdint.h>

// Portable game rules, independent of the display and button hardware.
struct SnakeGame {
  static constexpr int COLS = 30, ROWS = 12, CAPACITY = COLS * ROWS;
  struct Cell { int8_t x, y; };
  enum Direction : uint8_t { UP, RIGHT, DOWN, LEFT };
  enum Result : uint8_t { MOVED, ATE, CRASHED, WON };
  Cell body[CAPACITY];
  Cell food;
  uint16_t length = 4;
  Direction direction = RIGHT;
  Direction turns[2];
  uint8_t turnCount = 0;
  bool wrap = false;
  bool alive = true;

  static bool equal(Cell a, Cell b) { return a.x == b.x && a.y == b.y; }
  void reset(bool wrapEdges) {
    length = 4;
    direction = RIGHT;
    turnCount = 0;
    alive = true;
    wrap = wrapEdges;
    for (int i = 0; i < length; ++i) body[i] = {int8_t(15 - i), 6};
    food = {22, 6};
  }
  bool queueTurn(Direction next) {
    Direction previous = turnCount ? turns[turnCount - 1] : direction;
    if (next == previous || ((int(next) + 2) % 4) == previous || turnCount == 2) return false;
    turns[turnCount++] = next;
    return true;
  }
  bool occupied(Cell cell) const {
    for (int i = 0; i < length; ++i) if (equal(body[i], cell)) return true;
    return false;
  }
  bool placeFood(uint32_t randomValue) {
    if (length == CAPACITY) return false;
    uint16_t target = randomValue % (CAPACITY - length);
    for (int y = 0; y < ROWS; ++y) for (int x = 0; x < COLS; ++x) {
      Cell candidate = {int8_t(x), int8_t(y)};
      if (!occupied(candidate) && target-- == 0) { food = candidate; return true; }
    }
    return false;
  }
  Result step() {
    if (!alive) return CRASHED;
    if (turnCount) {
      direction = turns[0];
      if (turnCount == 2) turns[0] = turns[1];
      --turnCount;
    }
    Cell head = body[0];
    if (direction == UP) --head.y;
    if (direction == RIGHT) ++head.x;
    if (direction == DOWN) ++head.y;
    if (direction == LEFT) --head.x;
    if (wrap) {
      head.x = (head.x + COLS) % COLS;
      head.y = (head.y + ROWS) % ROWS;
    } else if (head.x < 0 || head.x >= COLS || head.y < 0 || head.y >= ROWS) {
      alive = false;
      return CRASHED;
    }
    bool eating = equal(head, food);
    // Moving into the tail is legal when the tail moves away this tick.
    int collisionLength = length - (eating ? 0 : 1);
    for (int i = 0; i < collisionLength; ++i) if (equal(body[i], head)) {
      alive = false;
      return CRASHED;
    }
    if (eating && length < CAPACITY) ++length;
    for (int i = length - 1; i > 0; --i) body[i] = body[i - 1];
    body[0] = head;
    if (length == CAPACITY) { alive = false; return WON; }
    return eating ? ATE : MOVED;
  }
  uint16_t score() const { return length - 4; }
};
