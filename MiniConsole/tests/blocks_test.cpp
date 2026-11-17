#include "../BlocksGame.h"
#include <cassert>
#include <cstdio>
#include <initializer_list>

int main() {
  for (int type = 0; type < 7; ++type) for (int rotation = 0; rotation < 4; ++rotation) {
    uint16_t mask = BlocksGame::shape(type, rotation);
    assert(__builtin_popcount(mask) == 4);
  }
  BlocksGame g;
  g.reset(42);
  // Restart the bag directly to check each group of seven is a permutation.
  g.bagIndex = 7;
  for (int group = 0; group < 100; ++group) {
    uint8_t seen = 0;
    for (int n = 0; n < 7; ++n) {
      uint8_t piece = g.takePiece();
      assert(!(seen & (1 << piece)));
      seen |= 1 << piece;
    }
    assert(seen == 0x7F);
  }

  g.reset(1);
  g.piece = BlocksGame::I; g.rotation = 1; g.x = -2; g.y = 5;
  assert(g.fits(g.x, g.y, g.rotation));
  assert(!g.move(-1));
  assert(g.rotate()); // Wall kick from the left edge into a horizontal I.
  assert(g.rotation == 2 && g.fits(g.x, g.y, g.rotation));
  g.piece = BlocksGame::T; g.rotation = 0; g.x = 3; g.y = 18;
  assert(g.grounded());
  assert(g.rotate()); // Floor kick.
  assert(g.fits(g.x, g.y, g.rotation));

  g.reset(1);
  g.piece = BlocksGame::O; g.rotation = 0; g.x = 3; g.y = -1;
  int landing = g.ghostY();
  assert(landing == 18);
  assert(g.fall(true) && g.score == 1);
  assert(g.hardDrop() == BlocksGame::LOCKED);
  assert(g.score == 37); // One soft-drop cell + 18 hard-drop cells * 2.
  assert(g.board[18][4] && g.board[19][5]);

  // Four-line clear with a vertical I, including shifting a partial row down.
  g.reset(2);
  for (int y = 16; y < 20; ++y) for (int x = 0; x < 10; ++x) g.board[y][x] = x == 4 ? 0 : 1;
  g.board[15][0] = 7;
  g.piece = BlocksGame::I; g.rotation = 1; g.x = 2; g.y = 16;
  assert(g.grounded());
  assert(g.lock() == BlocksGame::LOCKED);
  assert(g.lines == 4 && g.lastCleared == 4 && g.score == 1200 && g.lastClearPoints == 1200);
  assert(g.clearedRows == 0xF0000);
  assert(g.clearBoard[15][0] == 7 && g.clearBoard[19][4] == BlocksGame::I + 1);
  assert(g.board[19][0] == 7 && !g.board[18][0]);

  // Scoring uses the level before the clear that crosses the threshold.
  g.reset(3);
  g.lines = 9;
  for (int x = 0; x < 10; ++x) g.board[19][x] = (x == 4 || x == 5) ? 0 : 1;
  g.piece = BlocksGame::O; g.rotation = 0; g.x = 3; g.y = 18;
  assert(g.lock() == BlocksGame::LOCKED);
  assert(g.lines == 10 && g.level() == 1 && g.score == 40);

  // Classic scoring for all clear sizes, including the level multiplier.
  const unsigned awards[] = {0, 40, 100, 300, 1200};
  for (int level : {0, 3}) for (int count = 1; count <= 4; ++count) {
    g.reset(8);
    g.lines = level * 10;
    for (int row = 20 - count; row < 20; ++row) for (int col = 0; col < 10; ++col) {
      g.board[row][col] = col == 4 ? 0 : 1;
    }
    g.piece = BlocksGame::I; g.rotation = 1; g.x = 2; g.y = 16;
    assert(g.lock() == BlocksGame::LOCKED);
    assert(g.lastCleared == count);
    assert(g.score == awards[count] * (level + 1));
    assert(g.lastClearPoints == g.score);
  }

  // A piece blocked above the visible board must end the game without writes.
  g.reset(4);
  g.piece = BlocksGame::O; g.rotation = 0; g.x = 3; g.y = -1;
  g.board[1][4] = 7;
  assert(g.grounded());
  assert(g.lock() == BlocksGame::TOPPED_OUT && !g.alive);
  assert(g.lastCleared == 0 && g.lastClearPoints == 0);
  assert(g.clearedRows == 0);
  assert(!g.board[0][4] && g.board[1][4] == 7);

  // Deterministic stress: random legal actions across many games, under ASan.
  for (uint32_t seed = 1; seed <= 100; ++seed) {
    g.reset(seed);
    for (int piece = 0; piece < 100 && g.alive; ++piece) {
      for (int action = 0; action < 20; ++action) {
        switch (g.random() % 4) {
          case 0: g.move(-1); break;
          case 1: g.move(1); break;
          case 2: g.rotate(); break;
          case 3: g.fall(); break;
        }
        assert(g.fits(g.x, g.y, g.rotation));
      }
      g.hardDrop();
      for (int row = 0; row < 20; ++row) {
        int filled = 0;
        for (int col = 0; col < 10; ++col) {
          assert(g.board[row][col] <= 7);
          filled += g.board[row][col] != 0;
        }
        assert(filled < 10); // Every completed row must have been cleared.
      }
    }
  }
  std::puts("Blocks rules passed: shapes, bags, kicks, drops, clears, score, top-out and stress.");
}
