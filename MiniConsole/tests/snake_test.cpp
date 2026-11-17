#include "../SnakeGame.h"
#include <cassert>
#include <cstdio>

int main() {
  SnakeGame g;
  g.reset(false);
  assert(!g.queueTurn(SnakeGame::LEFT)); // No instant reversal.
  assert(g.queueTurn(SnakeGame::UP));
  assert(!g.queueTurn(SnakeGame::DOWN));
  assert(g.queueTurn(SnakeGame::LEFT));
  assert(!g.queueTurn(SnakeGame::DOWN)); // Queue is bounded.
  assert(g.step() == SnakeGame::MOVED);
  assert(g.body[0].x == 15 && g.body[0].y == 5);
  assert(g.step() == SnakeGame::MOVED);
  assert(g.body[0].x == 14 && g.body[0].y == 5);

  g.reset(false);
  g.food = {16, 6};
  assert(g.step() == SnakeGame::ATE && g.length == 5 && g.score() == 1);
  for (unsigned seed = 0; seed < 720; ++seed) {
    assert(g.placeFood(seed));
    assert(!g.occupied(g.food));
  }
  g.reset(false);
  g.body[0] = {29, 6};
  assert(g.step() == SnakeGame::CRASHED && !g.alive);
  g.reset(true);
  g.body[0] = {29, 6};
  assert(g.step() == SnakeGame::MOVED && g.body[0].x == 0);
  g.reset(true);
  g.body[0] = {15, 0};
  g.queueTurn(SnakeGame::UP);
  assert(g.step() == SnakeGame::MOVED && g.body[0].y == 11);

  // A loop allows entering the old tail as it vacates its cell.
  g.reset(false);
  g.body[0] = {5, 5}; g.body[1] = {5, 6};
  g.body[2] = {6, 6}; g.body[3] = {6, 5};
  g.food = {20, 10};
  assert(g.step() == SnakeGame::MOVED);
  g.reset(false);
  g.body[0] = {5, 5}; g.body[1] = {6, 5};
  assert(g.step() == SnakeGame::CRASHED); // Non-tail body collision.

  // Fill the last free cell: win without writing outside the body array.
  g.reset(false);
  g.length = SnakeGame::CAPACITY - 1;
  g.body[0] = {28, 11};
  int index = 1;
  for (int y = 0; y < SnakeGame::ROWS; ++y) for (int x = 0; x < SnakeGame::COLS; ++x) {
    if (y == 11 && (x == 28 || x == 29)) continue;
    g.body[index++] = {int8_t(x), int8_t(y)};
  }
  assert(index == g.length);
  g.food = {29, 11};
  assert(g.step() == SnakeGame::WON && g.length == SnakeGame::CAPACITY);
  assert(!g.placeFood(0));
  std::puts("Snake rules passed: turns, growth, food, walls, wrap, collisions, full-grid win.");
}
