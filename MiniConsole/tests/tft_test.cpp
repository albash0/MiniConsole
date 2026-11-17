#include "../MiniConsole.ino"
#include <cassert>
#include <vector>

const char *outputDir = nullptr;
void dump(const char *name, uint32_t now = 1000) {
  render(now);
  if (!outputDir) return;
  char path[512]; snprintf(path, sizeof(path), "%s/%s.ppm", outputDir, name);
  FILE *f = fopen(path, "wb"); assert(f); fprintf(f, "P6\n240 135\n255\n");
  const uint16_t *pixels = oled.physicalBuffer();
  for (int i = 0; i < 240 * 135; ++i) {
    uint16_t c = pixels[i];
    uint8_t rgb[3] = {uint8_t(((c >> 11) & 31) * 255 / 31), uint8_t(((c >> 5) & 63) * 255 / 63), uint8_t((c & 31) * 255 / 31)};
    fwrite(rgb, 1, 3, f);
  }
  fclose(f);
}
int main(int argc, char **argv) {
  if (argc > 1) outputDir = argv[1];
  setup(); assert(displayReady && oled.CS == -1 && oled.SCK == 8 && oled.MOSI == 9 && oled.RESET == 1 && oled.DC == 6 && oled.BACKLIGHT == 43);
  for (uint8_t rotation = 0; rotation < 4; ++rotation) {
    oled.setRotation(rotation); oled.setInk(); oled.clearDisplay();
    oled.drawRect(-2, -2, oled.width() + 3, oled.height() + 3, WHITE);
    oled.fillRoundRect(4, 5, 25, 16, 3, WHITE); oled.fillRect(8, 8, 5, 5, BLACK);
    oled.drawCircle(22, 30, 12, WHITE); oled.drawFastHLine(-4, 4, 25, WHITE); oled.drawFastVLine(4, -4, 25, WHITE);
    oled.setCursor(4, 45); oled.setTextColor(WHITE); oled.setTextSize(1); oled.print("TEST 123");
    for (int y = 0; y < 64; ++y) for (int x = 0; x < 128; ++x) {
      bool bit = oled.getBuffer()[y * 16 + x / 8] & (0x80 >> (x & 7));
      assert(oled.colorBuffer()[y * 128 + x] == (bit ? ConsoleTFT114::INK : ConsoleTFT114::BACKGROUND));
    }
    oled.display();
    for (int y = 0; y < 135; ++y) for (int x = 0; x < 240; ++x) {
      uint16_t expected = y < 7 || y >= 127 ? ConsoleTFT114::BACKGROUND : oled.colorBuffer()[((y - 7) * 64 / 120) * 128 + x * 128 / 240];
      assert(oled.physicalBuffer()[y * 240 + x] == expected);
    }
  }
  // RGB565 Doom frames retain their colors and flip once, without the old OLED
  // dithering. HUD/pause overlays cover only their own logical areas.
  std::vector<uint16_t> native(240 * 135);
  for (int i = 0; i < 240 * 135; ++i) native[i] = uint16_t(i + 2);
  oled.setRotation(0); oled.clearDisplay();
  oled.displayDoom(native.data(), false, false, false);
  for (int i = 0; i < 240 * 135; ++i) assert(oled.physicalBuffer()[i] == native[i]);
  oled.displayDoom(native.data(), true, false, false);
  for (int i = 0; i < 240 * 135; ++i) assert(oled.physicalBuffer()[i] == native[240 * 135 - 1 - i]);
  oled.displayDoom(native.data(), false, false);
  assert(oled.physicalBuffer()[10 * 240 + 10] == native[10 * 240 + 10]);
  assert(oled.physicalBuffer()[134 * 240 + 10] == ConsoleTFT114::BACKGROUND);
  oled.displayDoom(native.data(), true, false);
  assert(oled.physicalBuffer()[10] == ConsoleTFT114::BACKGROUND);
  assert(oled.physicalBuffer()[134 * 240 + 10] == native[229]);
  oled.displayDoom(native.data(), false, true, false);
  assert(oled.physicalBuffer()[60 * 240 + 60] == ConsoleTFT114::BACKGROUND);

  flipped = false; changeScreen(HOME); applyRotation();
  for (menuItem = 0; menuItem < GAME_COUNT; ++menuItem) { char name[24]; snprintf(name, sizeof(name), "tft_card_%u", menuItem); dump(name); }
  menuItem = SNAKE; startGame(0); dump("tft_snake");
  menuItem = BLOCKS; startBlocks(0); dump("tft_blocks"); assert(oled.width() == 64 && oled.height() == 128);
  menuItem = BREAKOUT; startSelectedGame(0); dump("tft_breakout");
  menuItem = INVADERS; startSelectedGame(0); dump("tft_invaders");
  menuItem = SOKOBAN; startSelectedGame(0); dump("tft_sokoban");
  changeScreen(SYSTEM_MENU); dump("tft_system");
  changeScreen(SETTINGS); dump("tft_settings");
  settingItem=muteSettingItem();dump("tft_mute_settings");
  settingItem=musicSettingItem();dump("tft_music_settings");
  settingItem=gameThemeSettingItem();dump("tft_game_theme_settings");
  changeScreen(BUTTON_TEST); dump("tft_buttons");
  for(controlsPage=0;controlsPage<=GAME_COUNT;++controlsPage){changeScreen(CONTROLS);char name[32];snprintf(name,sizeof(name),"tft_controls_%u",controlsPage);dump(name);}
  menuItem=BLOCKS;changeScreen(BLOCKS_READY);dump("tft_blocks_ready");
  changeScreen(BLOCKS_PAUSED);pauseSaved=true;dump("tft_blocks_pause");
  changeScreen(BLOCKS_OVER);dump("tft_blocks_over");
  menuItem=SOKOBAN;sokoLevel=4;changeScreen(NEW_READY);dump("tft_sokoban_ready");
  menuItem=BREAKOUT;changeScreen(NEW_PAUSED);dump("tft_pause");
  assert(saveGoingOut(true, 4) && saveDrinkGoal(4)); changeScreen(DRINK_COUNTER);
  pouring = confetti = true; pourStartedAt = confettiStartedAt = 0; dump("tft_goal", 1400);
  for (partyItem = 0; partyItem < PARTY_COUNT; ++partyItem) { changeScreen(PARTY_LIBRARY); char name[24]; snprintf(name, sizeof(name), "tft_party_%u", partyItem); dump(name); }
  changeScreen(SETTINGS);settingItem=3;dump("tft_goal_settings");
  partyItem=REACTION;changeScreen(PARTY_PLAYING);reaction.begin(0,42);reaction.update(reaction.waitMs);dump("tft_duel_go");
  reaction.update(reaction.waitMs+220,false,true);dump("tft_duel_result");
  partyItem=HIGH_LOW;highLow.begin(42);dump("tft_highlow_play");
  // The native board remains framed below the compact portrait header.
  menuItem=BLOCKS;startBlocks(0);render(1000);
  assert(oled.nativeCanvas().getPixel(TFTGames::BLOCK_X+3,TFTGames::BLOCK_Y-2)==TFTGames::FRAME);
  puts("TFT: pin map, CS=-1, four rotations, buffered primitives/colors, full-panel scaling, native-color Doom and HUD/flip, and all UI/game frames passed.");
}
