#pragma once
#include "../SuspendSave.h"

inline void makeSuspendFixture(SuspendState &s, uint8_t id) {
  s.gameId = id; s.difficulty = 2;
  s.stepAge = 7; s.fallAge = 180; s.groundAge = 0; s.noticeAge = 1200;
  s.animationAge = s.noticePoints = 0; s.noticeLines = s.lockResets = 0;
  s.clearAnimating = s.groundActive = false;
  if (id == 0) {
    s.snake.reset(true); s.snake.food = {15,5};
    s.snake.queueTurn(SnakeGame::UP); s.snake.step(); s.snake.placeFood(72);
    s.snake.queueTurn(SnakeGame::LEFT); s.snake.queueTurn(SnakeGame::UP);
  } else if (id == 1) {
    s.blocks.reset(42); s.blocks.piece = BlocksGame::T; s.blocks.y = 5; s.blocks.x = 4;
    s.blocks.rotate(); s.blocks.board[19][0] = BlocksGame::O + 1;
    s.blocks.score = 321; s.blocks.lines = 9;
  } else if (id == 2) {
    s.breakout.reset(); s.breakout.serve();
    for (int i = 0; i < 40; ++i) s.breakout.step(i < 20 ? -1 : 1, false);
  } else if (id == 3) {
    s.invaders.reset(42); s.invaders.player = 87;
    for (int i = 0; i < 70; ++i) s.invaders.step(0, true);
  } else {
    s.sokoban.reset(0); s.sokoban.move(0,1);
    // Wrap the history ring, then undo into it before suspending.
    for (int i = 0; i < 200; ++i) s.sokoban.move(i % 2 ? -1 : 1, 0);
    s.sokoban.undo(); s.sokoban.undo();
  }
}
