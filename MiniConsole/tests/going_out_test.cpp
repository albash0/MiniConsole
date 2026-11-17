#include "../MiniConsole.ino"
#include <cassert>
#include <limits>

uint32_t now = 0;
const char *outputDir = nullptr;
void tick(uint32_t elapsed = 1) {
  now += elapsed; clockNow = now; pollButtons(now); handleInput(now);
}
void down(Key key) { pinLevels[BUTTON_PINS[key]] = LOW; tick(); tick(18); }
void up(Key key) { pinLevels[BUTTON_PINS[key]] = HIGH; tick(); tick(18); }
void tap(Key key) { down(key); up(key); }
void firstArcadeCard() { for (int i = 0; i < PARTY_COUNT + 1; ++i) tap(RIGHT); }
void hold(Key key) { down(key); tick(OUT_HOLD_MS); }
void reboot() {
  prefs = Preferences{}; screen = HOME; menuItem = 0;
  checkpointAvailable = false; now = clockNow = 0;
  for (auto &b : buttons) b = Button{};
  setup();
}
void dump(const char *name, uint32_t at) {
  render(at);
  if (!outputDir) return;
  char path[512]; snprintf(path, sizeof(path), "%s/%s.pbm", outputDir, name);
  FILE *f = fopen(path, "w"); assert(f);
  fprintf(f, "P1\n128 64\n");
  for (int y = 0; y < 64; ++y) {
    for (int x = 0; x < 128; ++x) fprintf(f, "%u ", oled.getPixel(x, y));
    fputc('\n', f);
  }
  fclose(f);
}
int main(int argc, char **argv) {
  if (argc > 1) outputDir = argv[1];
  Preferences::flash.clear(); reboot();
  assert(!goingOut && !drinkCount && screen == HOME);
  // The normal UI has eight pages, including normal wraparound, and no extra card.
  tap(LEFT); assert(menuItem == GAME_COUNT-1 && screen == HOME);
  tap(RIGHT); assert(menuItem == SNAKE && screen == HOME);
  down(A); assert(screen == HOME); up(A); assert(screen == NEW_READY && !goingOut);
  // A can still be held to fire in games without enabling the hidden mode.
  menuItem = INVADERS; startSelectedGame(now);
  hold(A); tick(2000); assert(screen == NEW_PLAYING && !goingOut); up(A);
  changeScreen(HOME); menuItem = SNAKE;
  uint32_t writes = Preferences::blobWrites;
  down(A); tick(OUT_HOLD_MS - 1); assert(!goingOut && screen == HOME);
  tick(); assert(goingOut && screen == DRINK_COUNTER && drinkCount == 0);
  assert(Preferences::blobWrites == writes + 1);
  tick(5000); assert(!drinkCount && Preferences::blobWrites == writes + 1);
  up(A); tick(); assert(counterInputArmed && !drinkCount);
  dump("going_out_empty", now);
  down(A); assert(drinkCount == 0); up(A);
  assert(drinkCount == 1 && pouring);
  uint32_t started = pourStartedAt;
  dump("going_out_pour_early", started + 180);
  dump("going_out_pour_full", started + 620);
  dump("going_out_pour_drain", started + 900);
  // Another tap during the animation is counted and restarts it.
  tap(A); assert(drinkCount == 2 && pouring && pourStartedAt != started);
  dump("going_out_two", pourStartedAt + POUR_MS);
  assert(!pouring);
  auto saved = Preferences::flash.at("miniconsole/goingOut");
  writes = Preferences::blobWrites;
  for (int i = 0; i < 100; ++i) { tick(16); render(now); }
  assert(Preferences::blobWrites == writes);
  // Reset is a hold, fires once and never changes pages when released.
  down(B); tick(OUT_HOLD_MS - 1); assert(drinkCount == 2);
  tick(); assert(!drinkCount && !pouring && screen == DRINK_COUNTER);
  tick(5000); assert(Preferences::blobWrites == writes + 1);
  up(B); assert(screen == DRINK_COUNTER && !drinkCount);
  tap(A); tap(A); assert(drinkCount == 2);
  reboot(); assert(goingOut && drinkCount == 2 && screen == DRINK_COUNTER);
  // Holding A through boot is consumed until released, preventing a false exit.
  down(A); tick(2000); assert(goingOut && drinkCount == 2);
  up(A); tick();
  tap(RIGHT); assert(screen == PARTY_LIBRARY && partyItem == REACTION);
  tap(LEFT); assert(screen == DRINK_COUNTER); tick();
  tap(LEFT); assert(screen == HOME && menuItem == GAME_COUNT-1);
  tap(RIGHT); assert(screen == DRINK_COUNTER); tick();
  tap(B); assert(screen == SYSTEM_MENU && drinkCount == 2);
  tap(B); assert(screen == HOME);
  hold(A); assert(screen == DRINK_COUNTER && drinkCount == 2); up(A); tick();
  // A failed write neither invents a saved drink nor discards committed state.
  Preferences::failNextWrite = true; tap(A);
  assert(counterSaveFailed && drinkCount == 2 && Preferences::flash.at("miniconsole/goingOut") == saved);
  dump("going_out_save_failed", now);
  tap(A); assert(!counterSaveFailed && drinkCount == 3);
  // Existing game checkpoints survive mode changes and remain selectable.
  menuItem = SNAKE; startGame(now); tap(A); assert(screen == PAUSED && pauseSaved);
  auto checkpointBefore = Preferences::flash.at("miniconsole/suspend");
  reboot(); assert(screen == DRINK_COUNTER && drinkCount == 3 && checkpointAvailable);
  tick(); firstArcadeCard(); tap(A); assert(screen == CONTINUE_PROMPT);
  tap(A); assert(screen == PLAYING && game.length > 0);
  assert(Preferences::flash.at("miniconsole/suspend") == checkpointBefore);
  changeScreen(DRINK_COUNTER); tick();
  writes = Preferences::blobWrites;
  hold(A); assert(!goingOut && screen == HOME && drinkCount == 3);
  tick(5000); up(A); assert(screen == HOME && Preferences::blobWrites == writes + 1);
  reboot(); assert(!goingOut && screen == CONTINUE_PROMPT && drinkCount == 3);
  tap(B); menuItem = SNAKE; tap(LEFT); assert(screen == HOME && menuItem == GAME_COUNT-1);
  // Saturation and millis() wrap must not cause overflow or repeat a hold.
  assert(saveGoingOut(true, MAX_DRINKS)); changeScreen(DRINK_COUNTER); tick();
  writes = Preferences::blobWrites; tap(A); assert(drinkCount == MAX_DRINKS && Preferences::blobWrites == writes);
  dump("going_out_max", now);
  now = std::numeric_limits<uint32_t>::max() - 600;
  hold(B); assert(drinkCount == 0); up(B);
  assert(saveGoingOut(true, 1)); changeScreen(HOME);
  now = std::numeric_limits<uint32_t>::max() - 600;
  hold(A); assert(screen == DRINK_COUNTER && drinkCount == 1); up(A); tick();
  // A corrupt stored count fails closed into the ordinary console.
  prefs.remove("suspend"); prefs.putUInt("goingOut", OUT_MODE_FLAG | (MAX_DRINKS + 1));
  reboot(); assert(!goingOut && !drinkCount && screen == HOME);

  // The goal row is absent from ordinary settings (TFT also has Appearance).
  changeScreen(SETTINGS); settingItem = 0;
  tap(UP); assert(settingItem == settingsCount()-1); tap(DOWN); assert(settingItem == 0);
  dump("drink_goal_hidden", now);
  assert(!drinkGoal && saveGoingOut(true, 0)); changeScreen(DRINK_COUNTER); tick();
  tap(B); systemItem = 0; tap(A); assert(screen == SETTINGS);
  settingItem = 0;do{tap(UP);}while(settingItem!=3); // Scroll past Mute, Music and Appearance.
  assert(settingItem == 3);
  dump("drink_goal_off", now);
  writes = Preferences::blobWrites; tap(LEFT); assert(!drinkGoal && Preferences::blobWrites == writes);
  tap(RIGHT); assert(drinkGoal == 1); tap(A); assert(drinkGoal == 2);
  auto storedGoal = Preferences::flash.at("miniconsole/drinkGoal");
  dump("drink_goal_settings", now);
  tap(B); tap(B); hold(A); up(A); tick(); assert(screen == DRINK_COUNTER);
  tap(A); assert(drinkCount == 1 && !confetti);
  tap(A); assert(drinkCount == 2 && confetti && pouring);
  dump("drink_goal_confetti_early", confettiStartedAt + 350);
  dump("drink_goal_confetti", confettiStartedAt + 1400);
  assert(confetti);
  // Restart at the target preserves the goal and count without celebrating again.
  reboot(); tick(); assert(drinkGoal == 2 && drinkCount == 2 && !confetti);
  tap(A); assert(drinkCount == 3 && !confetti);
  hold(B); assert(!drinkCount && !confetti); up(B);
  tap(A); tap(A); assert(drinkCount == 2 && confetti);
  writes = Preferences::blobWrites;
  for (int i = 0; i < 30; ++i) render(confettiStartedAt + i * 60);
  assert(confetti && Preferences::blobWrites == writes);
  render(confettiStartedAt + CONFETTI_MS); assert(!confetti);
  render(confettiStartedAt + CONFETTI_MS + 100); assert(!confetti);
  // A failed count save must not claim that the target has been reached.
  hold(B); up(B); tap(A); Preferences::failNextWrite = true; tap(A);
  assert(drinkCount == 1 && !confetti && counterSaveFailed);
  tap(A); assert(drinkCount == 2 && confetti);
  hold(B); assert(!confetti && !pouring && !drinkCount); up(B);
  // Failed goal edits retain the last saved target and report the failure.
  tap(B); tap(A); settingItem = 3;
  Preferences::failNextWrite = true; tap(RIGHT);
  assert(drinkGoal == 2 && goalSaveFailed && Preferences::flash.at("miniconsole/drinkGoal") == storedGoal);
  dump("drink_goal_save_failed", now);
  tap(RIGHT); assert(drinkGoal == 3 && !goalSaveFailed);
  assert(saveDrinkGoal(MAX_DRINK_GOAL));
  writes = Preferences::blobWrites; tap(RIGHT); tap(A);
  assert(drinkGoal == MAX_DRINK_GOAL && Preferences::blobWrites == writes);
  tap(LEFT); assert(drinkGoal == MAX_DRINK_GOAL - 1);
  // OFF disables confetti; setting a target alone never starts it.
  assert(saveDrinkGoal(0)); changeScreen(DRINK_COUNTER); tick(); tap(A);
  assert(drinkCount == 1 && !confetti);
  assert(saveDrinkGoal(1)); assert(!confetti); tap(A); assert(drinkCount == 2 && !confetti);
  assert(saveGoingOut(false, drinkCount)); changeScreen(SETTINGS); settingItem = 0;
  tap(UP); assert(settingItem == settingsCount()-1 && drinkGoal == 1);
  dump("drink_goal_hidden_saved", now);
  reboot(); assert(!goingOut && drinkGoal == 1 && drinkCount == 2 && !confetti);
  // Confetti expires across the millis() wrap without blocking input.
  assert(saveGoingOut(true, 0)); changeScreen(DRINK_COUNTER); tick();
  now = std::numeric_limits<uint32_t>::max() - 200; tap(A); assert(confetti);
  render(confettiStartedAt + 1400); assert(confetti);
  render(confettiStartedAt + CONFETTI_MS); assert(!confetti);
  prefs.putUChar("drinkGoal", MAX_DRINK_GOAL + 1); reboot(); assert(drinkGoal == 0 && !confetti);
  puts("Going Out: GPIO holds/taps, hidden navigation/settings, pour/confetti, goal bounds, reset, NVS failures, reboot, game saves and clock wrap passed.");
}
