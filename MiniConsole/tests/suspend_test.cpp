#include "suspend_fixture.h"
#include <cassert>
#include <cstdio>
#include <vector>

void identical(const SuspendState &a, const SuspendState &b) {
  uint8_t first[2048], second[2048];
  size_t n = SuspendSave::encode(a, first), m = SuspendSave::encode(b, second);
  assert(n && n == m && !memcmp(first, second, n));
}
int main() {
  SuspendState source{}, restored{};
  uint8_t data[2048];
  for (int id = 0; id < 5; ++id) {
    makeSuspendFixture(source, id);
    size_t size = SuspendSave::encode(source, data); assert(size && size <= 2048);
    assert(SuspendSave::decode(data, size, restored)); identical(source, restored);
    // Resume follows the same trajectory, including RNG, queues and cooldowns.
    if (id == 0) { assert(source.snake.step() == restored.snake.step()); assert(source.snake.step() == restored.snake.step()); }
    if (id == 1) {
      source.blocks.hardDrop(); restored.blocks.hardDrop(); identical(source, restored);
      for (int i = 0; i < 12; ++i) assert(source.blocks.takePiece() == restored.blocks.takePiece());
    }
    if (id == 2) for (int i = 0; i < 100; ++i) { source.breakout.step(i % 3 - 1, false); restored.breakout.step(i % 3 - 1, false); }
    if (id == 3) for (int i = 0; i < 100; ++i) { source.invaders.step(i % 3 - 1, true); restored.invaders.step(i % 3 - 1, true); }
    if (id == 4) while (source.sokoban.historyCount) {
      assert(source.sokoban.undo() && restored.sokoban.undo());
      assert(source.sokoban.boxes == restored.sokoban.boxes && source.sokoban.player == restored.sokoban.player && source.sokoban.moves == restored.sokoban.moves);
    }
    if (id != 1) identical(source, restored);
    // Every single-byte corruption and truncated version must be rejected.
    makeSuspendFixture(source, id); size = SuspendSave::encode(source, data);
    for (size_t i = 0; i < size; ++i) { data[i] ^= 1; assert(!SuspendSave::decode(data, size, restored)); data[i] ^= 1; }
    for (size_t n = 0; n < size; ++n) assert(!SuspendSave::decode(data, n, restored));
    assert(!SuspendSave::encode(source, data, size - 1));
  }
  makeSuspendFixture(source, 0); source.snake.length = 65535; assert(!SuspendSave::encode(source, data));
  makeSuspendFixture(source, 1); source.blocks.piece = 7; assert(!SuspendSave::encode(source, data));
  makeSuspendFixture(source, 2); source.breakout.vx = NAN; assert(!SuspendSave::encode(source, data));
  makeSuspendFixture(source, 4); source.sokoban.historyCount = 129; assert(!SuspendSave::encode(source, data));
  uint32_t rng = 41;
  for (int i = 0; i < 4000; ++i) {
    size_t n = i % 2049;
    for (size_t j = 0; j < n; ++j) { rng = rng * 1664525u + 1013904223u; data[j] = rng >> 24; }
    assert(!SuspendSave::decode(data, n, restored));
  }
  puts("All five checkpoint round trips, continued trajectories, undo history, corrupt/truncated records and bounds passed.");
}
