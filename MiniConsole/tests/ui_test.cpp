#include "../MiniConsole.ino"
#include "sokoban_solutions.h"
#include <cassert>

const char *outputDir = nullptr;
void dump(const char *name, uint32_t now = 1000) {
  render(now);
  if (!outputDir) return;
  char path[512]; snprintf(path, sizeof(path), "%s/%s.pbm", outputDir, name);
  FILE *f = fopen(path, "w"); assert(f);
  fprintf(f, "P1\n%d %d\n", oled.width(), oled.height());
  for (int y = 0; y < oled.height(); ++y) {
    for (int x = 0; x < oled.width(); ++x) fprintf(f, "%d ", oled.getPixel(x, y));
    fprintf(f, "\n");
  }
  fclose(f);
}
void clearEvents() { for (auto &b : buttons) b.pressed = b.repeated = b.held = false; }
void press(Key key, uint32_t now) { clearEvents(); buttons[key].pressed = true; handleInput(now); }

int main(int argc, char **argv) {
  if (argc > 1) outputDir = argv[1];
  setup(); assert(displayReady);
#ifdef MINICONSOLE_TFT114
  assert(BUTTON_PINS[A] == 2 && BUTTON_PINS[B] == 7);
#else
  assert(BUTTON_PINS[A] == 7 && BUTTON_PINS[B] == 2);
#endif
  // Debounce an actual GPIO edge; press is emitted once, hold then repeats.
  pinLevels[BUTTON_PINS[A]] = LOW; pollButtons(0); assert(!buttons[A].pressed);
  pollButtons(17); assert(!buttons[A].pressed);
  pollButtons(18); assert(buttons[A].pressed);
  pollButtons(19); assert(!buttons[A].pressed && buttons[A].held);
  pollButtons(238); assert(buttons[A].repeated);
  clearEvents();
  for (int i = 0; i < GAME_COUNT; ++i) {
    menuItem = i; changeScreen(HOME);
    dump(GAME_NAMES[i]);
    press(A, 1000);
    if(i==PARTNERS){assert(screen==PARTNERS_ROOM&&partnersChoosing);dump("partners_lobby");press(B,1050);assert(screen==HOME);continue;}
    assert(screen == (i == BLOCKS ? BLOCKS_READY : NEW_READY));
    dump(i == BLOCKS ? "blocks_ready" : i == SNAKE ? "snake_ready" : i == BREAKOUT ? "breakout_ready" : i == INVADERS ? "invaders_ready" : i == DOOM ? "doom_ready" : "sokoban_ready");
    press(i == BLOCKS ? LEFT : A, 1100);
    assert(screen == (i == SNAKE ? PLAYING : i == BLOCKS ? BLOCKS_PLAYING : i == DOOM ? DOOM_RUNNING : NEW_PLAYING));
    assert(oled.width() == (i == BLOCKS ? 64 : 128));
  }
  changeScreen(HOME); menuItem = 0;
  press(LEFT, 2000); assert(menuItem == GAME_COUNT-1); press(RIGHT, 2010); assert(menuItem == 0);
  press(B, 2020); assert(screen == SYSTEM_MENU); dump("system");
  systemItem = 0; press(A, 2030); assert(screen == SETTINGS); dump("settings");
  settingItem = 2; press(A, 2040); assert(flipped && oled.getRotation() == 2);
  changeScreen(BLOCKS_READY); assert(oled.getRotation() == 1);
  changeScreen(NEW_READY); assert(oled.getRotation() == 2 && oled.width() == 128);
  flipped = false; applyRotation();
  changeScreen(CONTROLS);
  for (controlsPage = 0; controlsPage <= GAME_COUNT; ++controlsPage) { char name[20]; snprintf(name, sizeof(name), "controls_%u", controlsPage); dump(name); }
  controlsPage = GAME_COUNT; press(RIGHT, 2100); assert(controlsPage == 0);

  // Preserve the user's portrait controls and pauseable line-clear animation.
  menuItem = BLOCKS; startBlocks(4000); assert(oled.getRotation() == 3);
  blocks.piece = BlocksGame::T; blocks.x = 3; blocks.y = 4;
  int before = blocks.x; press(DOWN, 4100); assert(blocks.x == before - 1);
  press(UP, 4200); assert(blocks.x == before);
  press(LEFT, 4300); assert(blocks.rotation == 1);
  press(A, 4400); assert(screen == BLOCKS_PAUSED);
  press(A, 5400); assert(screen == BLOCKS_PLAYING && lastFall == 5000);
  startBlocks(6000);
  for (int row = 16; row < 20; ++row) for (int col = 0; col < 10; ++col) blocks.board[row][col] = col == 4 ? 0 : 1;
  blocks.board[15][0] = 7; blocks.piece = BlocksGame::I; blocks.rotation = 1; blocks.x = 2; blocks.y = 16;
  press(B, 6100); assert(clearAnimating && blocks.lines == 4 && blocks.score == 1200);
  press(A, 6200); assert(screen == BLOCKS_PAUSED);
  press(A, 8200); assert(clearAnimationAt == 8100);
  updateBlocks(8749); assert(clearAnimating); updateBlocks(8750); assert(!clearAnimating);
  assert(blocks.board[19][0] == 7); dump("blocks_play", 9000);

  menuItem = BREAKOUT; startSelectedGame(10000);
  clearEvents(); buttons[A].held = true; updateArcade(10016); assert(!breakout.serving);
  float x = breakout.x, y = breakout.y;
  press(B, 10020); assert(screen == NEW_PAUSED);
  updateArcade(16000); assert(breakout.x == x && breakout.y == y); dump("arcade_pause");
  press(A, 16000); clearEvents(); updateArcade(16001); assert(breakout.x == x && breakout.y == y);
  updateArcade(16016); assert(breakout.y < y);
  BreakoutGame expected = breakout;
  expected.step(0, false); expected.step(0, false);
  updateArcade(16048); assert(breakout.x == expected.x && breakout.y == expected.y);
  breakout.score = 250; press(B, 16100); press(LEFT, 16110); assert(breakoutBest == 250 && breakout.score == 0);
  breakout.x = 86; breakout.y = 44; breakout.serving = false; dump("breakout_play");
  menuItem = INVADERS; startSelectedGame(17000); invaders.player = 87; clearEvents();
  buttons[A].held = true; updateArcade(17240); assert(invaders.shots[0].active); dump("invaders_play");
  invaders.score = 400; invaders.alive = false; updateArcade(17256);
  assert(screen == NEW_OVER && invadersBest == 400); dump("arcade_over");

  menuItem = SOKOBAN; sokoLevel = sokoSolved = 0; startSelectedGame(18000); dump("sokoban_play");
  press(RIGHT, 18100); assert(sokoban.pushes == 1);
  press(A, 18200); assert(sokoban.pushes == 0);
  press(B, 18300); press(LEFT, 18400); assert(screen == NEW_PLAYING && sokoban.moves == 0);
  press(RIGHT, 18500); press(RIGHT, 18600); assert(screen == NEW_OVER && sokoSolved == 1); dump("sokoban_clear");
  press(A, 18700); assert(sokoLevel == 1 && screen == NEW_PLAYING);
  for (int level = 1; level < SokobanGame::LEVELS; ++level) {
    for (const char *c = SOKOBAN_SOLUTIONS[level]; *c; ++c) {
      press(*c == 'L' ? LEFT : *c == 'R' ? RIGHT : *c == 'U' ? UP : DOWN, 19000);
    }
    assert(screen == NEW_OVER && sokoSolved == level + 1);
    if (level < 9) press(A, 19500);
  }
  dump("sokoban_complete");
  press(A, 20000); assert(screen == NEW_READY && sokoLevel == 0);
  press(LEFT, 20100); assert(sokoLevel == 9);
  menuItem = SOKOBAN; changeScreen(HOME); dump("library_complete");
  puts("GPIO debounce, eight-cartridge navigation, rotation, pause, score/progress and framebuffer checks passed.");
}
