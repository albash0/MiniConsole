#pragma once
#include <stdint.h>
#include <string.h>

// 10x20 falling-block rules, with a shuffled seven-piece bag.
struct BlocksGame {
  static constexpr int COLS = 10, ROWS = 20;
  enum Piece : uint8_t { I, O, T, S, Z, J, L };
  enum LockResult : uint8_t { LOCKED, TOPPED_OUT };
  uint8_t board[ROWS][COLS] = {};
  // Preserve the locked stack before completed rows collapse, for animation.
  uint8_t clearBoard[ROWS][COLS] = {};
  uint32_t clearedRows = 0;
  uint8_t piece = I, next = O, rotation = 0;
  int8_t x = 3, y = -1;
  bool alive = true;
  uint32_t score = 0;
  uint16_t lines = 0;
  uint8_t lastCleared = 0;
  uint32_t lastClearPoints = 0;
  uint8_t bag[7], bagIndex = 7;
  uint32_t randomState = 1;

  static uint16_t shape(uint8_t type, uint8_t rot) {
    static constexpr uint16_t shapes[7][4] = {
      {0x00F0, 0x4444, 0x0F00, 0x2222}, // I
      {0x0066, 0x0066, 0x0066, 0x0066}, // O
      {0x0072, 0x0262, 0x0270, 0x0232}, // T
      {0x0036, 0x0462, 0x0360, 0x0231}, // S
      {0x0063, 0x0264, 0x0630, 0x0132}, // Z
      {0x0071, 0x0226, 0x0470, 0x0322}, // J
      {0x0074, 0x0622, 0x0170, 0x0223}  // L
    };
    return shapes[type][rot & 3];
  }
  static bool cell(uint8_t type, uint8_t rot, int col, int row) {
    return (shape(type, rot) & (uint16_t(1) << (row * 4 + col))) != 0;
  }
  uint32_t random() {
    randomState ^= randomState << 13;
    randomState ^= randomState >> 17;
    randomState ^= randomState << 5;
    return randomState;
  }
  uint8_t takePiece() {
    if (bagIndex == 7) {
      for (int i = 0; i < 7; ++i) bag[i] = i;
      for (int i = 6; i > 0; --i) {
        int j = random() % (i + 1);
        uint8_t temporary = bag[i]; bag[i] = bag[j]; bag[j] = temporary;
      }
      bagIndex = 0;
    }
    return bag[bagIndex++];
  }
  bool fits(int px, int py, uint8_t rot) const {
    for (int row = 0; row < 4; ++row) for (int col = 0; col < 4; ++col) {
      if (!cell(piece, rot, col, row)) continue;
      int bx = px + col, by = py + row;
      if (bx < 0 || bx >= COLS || by >= ROWS) return false;
      if (by >= 0 && board[by][bx]) return false;
    }
    return true;
  }
  void spawn() {
    piece = next;
    next = takePiece();
    x = 3; y = -1; rotation = 0;
    if (!fits(x, y, rotation)) alive = false;
  }
  void reset(uint32_t seed) {
    memset(board, 0, sizeof(board));
    randomState = seed ? seed : 1;
    bagIndex = 7;
    alive = true; score = 0; lines = 0; lastCleared = 0; lastClearPoints = 0;
    clearedRows = 0;
    next = takePiece();
    spawn();
  }
  bool move(int dx) {
    if (!alive || !fits(x + dx, y, rotation)) return false;
    x += dx;
    return true;
  }
  bool rotate() {
    if (!alive) return false;
    if (piece == O) return false;
    uint8_t candidate = (rotation + 1) & 3;
    // Small wall/floor kicks keep pieces usable beside edges and stacks.
    static constexpr int8_t kicks[][2] = {
      {0, 0}, {-1, 0}, {1, 0}, {-2, 0}, {2, 0}, {0, -1}, {0, -2}
    };
    for (const auto &kick : kicks) if (fits(x + kick[0], y + kick[1], candidate)) {
      x += kick[0]; y += kick[1]; rotation = candidate;
      return true;
    }
    return false;
  }
  bool grounded() const { return !fits(x, y + 1, rotation); }
  bool fall(bool soft = false) {
    if (!alive || grounded()) return false;
    ++y;
    if (soft) ++score;
    return true;
  }
  int ghostY() const {
    int landing = y;
    while (fits(x, landing + 1, rotation)) ++landing;
    return landing;
  }
  uint16_t level() const { return lines / 10; }
  uint8_t clearLines() {
    int destination = ROWS - 1;
    uint8_t cleared = 0;
    for (int source = ROWS - 1; source >= 0; --source) {
      bool full = true;
      for (int col = 0; col < COLS; ++col) if (!board[source][col]) full = false;
      if (full) ++cleared;
      else {
        if (destination != source) memcpy(board[destination], board[source], COLS);
        --destination;
      }
    }
    while (destination >= 0) memset(board[destination--], 0, COLS);
    return cleared;
  }
  LockResult lock() {
    lastCleared = 0;
    lastClearPoints = 0;
    clearedRows = 0;
    if (!alive) return TOPPED_OUT;
    // Check hidden cells before writing anything to the board.
    for (int row = 0; row < 4; ++row) for (int col = 0; col < 4; ++col) {
      if (cell(piece, rotation, col, row) && y + row < 0) {
        alive = false; return TOPPED_OUT;
      }
    }
    for (int row = 0; row < 4; ++row) for (int col = 0; col < 4; ++col) {
      if (cell(piece, rotation, col, row)) board[y + row][x + col] = piece + 1;
    }
    for (int row = 0; row < ROWS; ++row) {
      bool full = true;
      for (int col = 0; col < COLS; ++col) if (!board[row][col]) full = false;
      if (full) clearedRows |= uint32_t(1) << row;
    }
    if (clearedRows) memcpy(clearBoard, board, sizeof(board));
    lastCleared = clearLines();
    // Original Nintendo/Game Boy line-clear scoring. Four-line clears are
    // worth 30 singles, rewarding the risk of building a stack for an I piece.
    static constexpr uint16_t awards[] = {0, 40, 100, 300, 1200};
    lastClearPoints = awards[lastCleared] * uint32_t(level() + 1);
    score += lastClearPoints;
    lines += lastCleared;
    spawn();
    return alive ? LOCKED : TOPPED_OUT;
  }
  LockResult hardDrop() {
    if (!alive) return TOPPED_OUT;
    int landing = ghostY();
    score += (landing - y) * 2;
    y = landing;
    return lock();
  }
};
