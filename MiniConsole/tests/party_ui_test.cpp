#include "../MiniConsole.ino"
#include <cassert>

uint32_t now = 0;
const char *outputDir = nullptr;
void tick(uint32_t elapsed = 1) {
  now += elapsed; clockNow = now; pollButtons(now); handleInput(now);
  if (screen == PARTY_PLAYING) updateParty(now);
}
void down(Key key) { pinLevels[BUTTON_PINS[key]] = LOW; tick(); tick(18); }
void up(Key key) { pinLevels[BUTTON_PINS[key]] = HIGH; tick(); tick(18); }
void tap(Key key) { down(key); up(key); }
void dump(const char *name, uint32_t at) {
  render(at);
  if (!outputDir) return;
  char path[512]; snprintf(path, sizeof(path), "%s/%s.pbm", outputDir, name);
  FILE *f = fopen(path, "w"); assert(f); fprintf(f, "P1\n128 64\n");
  for (int y = 0; y < 64; ++y) {
    for (int x = 0; x < 128; ++x) fprintf(f, "%u ", oled.getPixel(x, y));
    fputc('\n', f);
  }
  fclose(f);
}
void openParty(uint8_t id) {
  partyItem = id; changeScreen(PARTY_LIBRARY);
  tap(A); assert(screen == PARTY_PLAYING); tick();
  assert(partyInputArmed && oled.getRotation() == 0);
}
int main(int argc, char **argv) {
  if (argc > 1) outputDir = argv[1];
  Preferences::flash.clear(); setup();
  assert(!goingOut && libraryPages() == GAME_COUNT);
  for (int i = 0; i < 12; ++i) { tap(RIGHT); assert(screen == HOME); }
  // Keep a real game checkpoint and all drink state while trying party games.
  menuItem = SNAKE; startGame(now); tap(A); assert(screen == PAUSED && pauseSaved);
  assert(saveGoingOut(true, 12) && saveDrinkGoal(4));
  auto flashBefore = Preferences::flash;
  uint32_t writes = Preferences::blobWrites;
  changeScreen(DRINK_COUNTER); tick(); assert(libraryPages() == GAME_COUNT+PARTY_COUNT+1);
  for (int i = 1; i <= GAME_COUNT+PARTY_COUNT+1; ++i) {
    tap(RIGHT);
    if (i <= 3) assert(screen == PARTY_LIBRARY && partyItem == i - 1);
    else if (i < GAME_COUNT+PARTY_COUNT+1) assert(screen == HOME && menuItem == i - 4);
    else { assert(screen == DRINK_COUNTER); tick(); }
  }
  tap(LEFT); assert(screen == HOME && menuItem == GAME_COUNT-1);
  for (int i = 0; i < GAME_COUNT-1; ++i) tap(LEFT);
  assert(menuItem == SNAKE); tap(LEFT); assert(screen == PARTY_LIBRARY && partyItem == PASS_BOMB);
  tap(LEFT); tap(LEFT); assert(partyItem == REACTION); tap(LEFT); assert(screen == DRINK_COUNTER); tick();

  for (partyItem = 0; partyItem < PARTY_COUNT; ++partyItem) {
    changeScreen(PARTY_LIBRARY); char name[32]; snprintf(name, sizeof(name), "party_card_%u", partyItem); dump(name, now);
  }
  openParty(REACTION); dump("party_reaction_ready", now);
  tap(A); assert(reaction.phase == PartyGames::ReactionDuel::WAIT); dump("party_reaction_wait", now);
  down(B); assert(reaction.phase == PartyGames::ReactionDuel::RESULT && reaction.falseStart && reaction.scoreA == 1);
  tick(2000); assert(reaction.scoreA == 1); up(B); dump("party_reaction_early", now);
  tap(A); assert(reaction.phase == PartyGames::ReactionDuel::RESULT && reaction.scoreA == 1);
  tap(UP); tick(reaction.startedAt + reaction.waitMs - now); assert(reaction.phase == PartyGames::ReactionDuel::GO);
  dump("party_reaction_go", now); tick(200); down(B);
  assert(reaction.winner == PartyGames::ReactionDuel::PLAYER_B && !reaction.falseStart && reaction.reactionMs >= 200 && reaction.reactionMs < 250);
  uint32_t winningTime = reaction.reactionMs;
  // Reproduce the user's bug: B wins, then the slower A arrives while B is
  // still held. Neither that edge nor later A/B taps may restart the round.
  down(A); assert(reaction.phase == PartyGames::ReactionDuel::RESULT && reaction.winner == PartyGames::ReactionDuel::PLAYER_B);
  tick(2000); assert(reaction.reactionMs == winningTime && reaction.scoreB == 1);
  up(B); up(A); tap(A); tap(B);
  assert(reaction.phase == PartyGames::ReactionDuel::RESULT && reaction.reactionMs == winningTime && reaction.scoreA == 1 && reaction.scoreB == 1);
  dump("party_reaction_result", now);
  tap(UP); tick(reaction.startedAt + reaction.waitMs - now);
  pinLevels[BUTTON_PINS[A]] = pinLevels[BUTTON_PINS[B]] = LOW; tick(); tick(18);
  assert(reaction.winner == PartyGames::ReactionDuel::TIE);
  up(A); up(B); tap(A); tap(B); assert(reaction.phase == PartyGames::ReactionDuel::RESULT && reaction.winner == PartyGames::ReactionDuel::TIE);
  tap(UP); tick(reaction.startedAt + reaction.waitMs - now); down(A);
  assert(reaction.phase == PartyGames::ReactionDuel::RESULT && reaction.winner == PartyGames::ReactionDuel::PLAYER_A);
  down(B); assert(reaction.phase == PartyGames::ReactionDuel::RESULT && reaction.winner == PartyGames::ReactionDuel::PLAYER_A);
  up(A); up(B); tap(LEFT); assert(screen == PARTY_LIBRARY);

  openParty(HIGH_LOW); dump("party_cards_ready", now);
  tap(A); assert(highLow.phase == PartyGames::HighLow::GUESS && highLow.used == 1); dump("party_cards_guess", now);
  uint8_t oldRank = PartyGames::HighLow::rank(highLow.current), nextRank = PartyGames::HighLow::rank(highLow.deck[highLow.used]);
  tap(nextRank >= oldRank ? A : B); assert(highLow.phase == PartyGames::HighLow::FLIP);
  dump("party_cards_flip", highLow.flippedAt + 120);
  tick(highLow.flippedAt + 280 - now); assert(highLow.phase == PartyGames::HighLow::RESULT && highLow.outcome != PartyGames::HighLow::WRONG);
  dump("party_cards_result", now);
  tap(A); assert(highLow.phase == PartyGames::HighLow::GUESS && highLow.used == 2);
  pinLevels[BUTTON_PINS[A]] = pinLevels[BUTTON_PINS[B]] = LOW; tick(); tick(18);
  assert(highLow.phase == PartyGames::HighLow::GUESS && highLow.used == 2); up(A); up(B);
  tap(B); assert(highLow.phase == PartyGames::HighLow::FLIP);
  tap(LEFT); assert(screen == PARTY_LIBRARY);

  openParty(PASS_BOMB); dump("party_bomb_ready", now);
  tap(A); assert(passBomb.phase == PartyGames::PassBomb::RUNNING && !passBomb.passes);
  down(A); tick(1000); assert(passBomb.passes == 1); up(A); dump("party_bomb_running", now);
  tick(passBomb.startedAt + passBomb.fuseMs - 100 - now); dump("party_bomb_fuse", now);
  tick(81); // The next debounced press lands exactly on the fuse deadline.
  down(A); assert(passBomb.phase == PartyGames::PassBomb::EXPLODED && passBomb.passes == 1);
  up(A); dump("party_bomb_exploded", now);
  tap(A); assert(passBomb.phase == PartyGames::PassBomb::RUNNING && !passBomb.passes);
  tap(LEFT); assert(screen == PARTY_LIBRARY);
  tick(40000); tap(A); tick(); assert(passBomb.phase == PartyGames::PassBomb::READY);
  tap(LEFT); tap(B); assert(screen == SYSTEM_MENU);
  assert(Preferences::blobWrites == writes && Preferences::flash == flashBefore);
  assert(drinkCount == 12 && drinkGoal == 4 && checkpointAvailable);

  // Hidden screens cannot continue if the mode is disabled. Portrait settings
  // still apply only to Blocks; party games obey the existing screen flip.
  goingOut = false; changeScreen(PARTY_LIBRARY); tick(); assert(screen == HOME && libraryPages() == GAME_COUNT);
  changeScreen(PARTY_PLAYING); tick(); assert(screen == HOME);
  goingOut = true; flipped = true; changeScreen(PARTY_LIBRARY); assert(oled.getRotation() == 2);
  partyItem = REACTION; tap(A); tick(); assert(oled.getRotation() == 2);
  prefs = Preferences{}; screen = HOME; for (auto &b : buttons) b = Button{};
  setup(); assert(screen == DRINK_COUNTER && drinkCount == 12 && drinkGoal == 4 && checkpointAvailable);
  assert(reaction.phase == PartyGames::ReactionDuel::READY && highLow.phase == PartyGames::HighLow::READY && passBomb.phase == PartyGames::PassBomb::READY);
  puts("Party UI passed: hidden eleven-page navigation, GPIO A/B fairness, card flips, bomb deadline, retries/back, unchanged drinks/goals/game save and boot/rotation.");
}
