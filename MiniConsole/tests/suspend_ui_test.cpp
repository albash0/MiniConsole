#include "../MiniConsole.ino"
#include "suspend_fixture.h"
#include <cassert>

const char *outputDir = nullptr;
void dumpSuspend(const char *name) {
  render(1000);
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

void press(Key key, uint32_t now) {
  for (auto &b : buttons) b.pressed = b.repeated = b.held = false;
  buttons[key].pressed = true; handleInput(now);
}
void reboot() {
  // Reconstruct all volatile game objects and clocks, retaining only mock flash.
  prefs = Preferences{}; checkpoint = SuspendState{};
  game = SnakeGame{}; blocks = BlocksGame{}; breakout = BreakoutGame{};
  invaders = InvadersGame{}; sokoban = SokobanGame{};
  screen = HOME; menuItem = 0; speed = gameSpeed = 1; sokoLevel = sokoSolved = 0;
  best = 0; blocksBest = breakoutBest = invadersBest = 0;
  flipped = wrapEdges = won = newBest = false;
  checkpointAvailable = pauseSaved = suppressFire = storageReady = false;
  lastStep = lastFall = groundSince = clearNoticeAt = clearAnimationAt = blocksPausedAt = snakePausedAt = arcadeTick = 0;
  clearNoticePoints = clearNoticeLines = lockResets = 0; clearAnimating = groundActive = false;
  for (auto &b : buttons) b = Button{};
  clockNow = 0;
  setup();
}
void checkCurrentState(const std::vector<uint8_t> &expected) {
  SuspendState current = checkpoint;
  if (menuItem == SNAKE) current.snake = game;
  if (menuItem == BLOCKS) current.blocks = blocks;
  if (menuItem == BREAKOUT) current.breakout = breakout;
  if (menuItem == INVADERS) current.invaders = invaders;
  if (menuItem == SOKOBAN) current.sokoban = sokoban;
  uint8_t bytes[2048]; size_t n = SuspendSave::encode(current, bytes);
  assert(n == expected.size() && !memcmp(bytes, expected.data(), n));
}
int main(int argc, char **argv) {
  if (argc > 1) outputDir = argv[1];
  for (int id = 0; id < 5; ++id) {
    Preferences::flash.clear(); reboot();
    speed = gameSpeed = 2; flipped = true; saveSettings();
    best = 23; prefs.putUShort("best", best);
    menuItem = id;
    SuspendState fixture{}; makeSuspendFixture(fixture, id);
    game = fixture.snake; blocks = fixture.blocks; breakout = fixture.breakout;
    invaders = fixture.invaders; sokoban = fixture.sokoban;
    lastStep = arcadeTick = 990; lastFall = 820;
    changeScreen(id == SNAKE ? PLAYING : id == BLOCKS ? BLOCKS_PLAYING : NEW_PLAYING);
    uint32_t writes = Preferences::blobWrites;
    press(id == SNAKE || id == BLOCKS ? A : B, 1000);
    assert(pauseSaved && checkpointAvailable && Preferences::blobWrites == writes + 1);
    char imageName[20]; snprintf(imageName, sizeof(imageName), "save_%u", id); dumpSuspend(imageName);
    auto expected = Preferences::flash.at("miniconsole/suspend");
    // Waiting while paused never rewrites flash or advances the game.
    for (int i = 0; i < 100; ++i) { render(1000 + i); if (id == 2 || id == 3) updateArcade(1000 + i); }
    assert(Preferences::blobWrites == writes + 1);
    reboot();
    assert(screen == CONTINUE_PROMPT && menuItem == id && flipped && best == 23);
    snprintf(imageName, sizeof(imageName), "resume_%u", id); dumpSuspend(imageName);
    press(A, 200); assert(gameSpeed == 2 && oled.getRotation() == (id == BLOCKS ? 1 : 2));
    checkCurrentState(expected);
    assert(screen == (id == SNAKE ? PLAYING : id == BLOCKS ? BLOCKS_PLAYING : NEW_PLAYING));
    if (id == SNAKE) assert(lastStep == 190 && game.turnCount == 2);
    if (id == BLOCKS) assert(lastFall == 20 && blocks.score == 321);
    if (id == SOKOBAN) { assert(sokoban.historyCount == 126); press(A, 210); assert(sokoban.historyCount == 125); }
    assert(Preferences::flash.at("miniconsole/suspend") == expected);
  }

  // A checkpoint during a four-line animation preserves its exact phase.
  Preferences::flash.clear(); reboot(); menuItem = BLOCKS; startBlocks(4000);
  for (int r = 16; r < 20; ++r) for (int c = 0; c < 10; ++c) blocks.board[r][c] = c == 4 ? 0 : 1;
  blocks.board[15][0] = 7; blocks.piece = BlocksGame::I; blocks.rotation = 1; blocks.x = 2; blocks.y = 16;
  press(B, 4100); press(A, 4300); assert(pauseSaved && clearAnimating);
  reboot(); press(A, 20); assert(clearAnimating && clearNoticeLines == 4);
  updateBlocks(469); assert(clearAnimating); updateBlocks(470); assert(!clearAnimating && blocks.board[19][0] == 7);

  // Grounded lock delay and reset count also survive the millis() reset.
  menuItem = BLOCKS; startBlocks(5000); blocks.y = blocks.ghostY();
  groundActive = true; groundSince = 4825; lockResets = 10;
  press(A, 5000); assert(pauseSaved); reboot(); press(A, 30);
  assert(groundActive && lockResets == 10);
  updateBlocks(254); assert(blocks.y >= 0); updateBlocks(255); assert(blocks.y == -1);

  // Leaving for the library keeps the save. Choosing NEW explicitly deletes it.
  menuItem = SNAKE; startGame(6000); press(A, 6050); press(B, 6100);
  assert(screen == HOME && checkpointAvailable);
  press(A, 6150); assert(screen == CONTINUE_PROMPT); press(LEFT, 6160);
  assert(screen == NEW_READY && !checkpointAvailable && !prefs.getBytesLength("suspend"));

  // A failed write does not claim SAVED or destroy the previous committed blob.
  startGame(6200); press(A, 6250);
  auto old = Preferences::flash.at("miniconsole/suspend");
  press(A, 6300); game.queueTurn(SnakeGame::UP); game.step();
  Preferences::failNextWrite = true; press(A, 6350);
  assert(!pauseSaved && checkpointAvailable && Preferences::flash.at("miniconsole/suspend") == old);
  dumpSuspend("save_failed");
  reboot(); assert(screen == CONTINUE_PROMPT); press(A, 40); assert(game.direction == SnakeGame::RIGHT);

  // Starting another game preserves the previous save until the next pause.
  menuItem = INVADERS; startSelectedGame(1000); assert(checkpointAvailable && checkpoint.gameId == SNAKE);
  press(B, 1050); assert(pauseSaved && checkpoint.gameId == INVADERS);
  invaders.alive = false; press(A, 1100); updateArcade(1116);
  assert(screen == NEW_OVER && !checkpointAvailable && !prefs.getBytesLength("suspend"));

  // Corruption/unknown versions never replace live state; boot safely opens HOME.
  menuItem = SNAKE; startGame(2000); press(A, 2050);
  Preferences::flash["miniconsole/suspend"][4] ^= 1;
  reboot(); assert(screen == HOME && !checkpointAvailable);
  puts("Five-game power-cycle resume, frozen timers/animation, saved settings, write failures and checkpoint lifecycle passed.");
}
