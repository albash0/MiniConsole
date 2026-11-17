#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#ifdef MINICONSOLE_TFT114
#include "ConsoleTFT114.h"
#include "UITheme.h"
#include "SnakeMotion.h"
#else
#include <Adafruit_SSD1306.h>
#endif
#include <Preferences.h>
#include <esp_system.h>
#include "SnakeGame.h"
#include "BlocksGame.h"
#include "BlockBattle.h"
#include "BattleRadio.h"
#include "PartnersLink.h"
#include "BreakoutGame.h"
#include "InvadersGame.h"
#include "SokobanGame.h"
#include "SuspendSave.h"
#include "PartyGames.h"
#include "ConsoleSound.h"
#include <atomic>
#include <DoomPiezo.h>
#ifndef MINICONSOLE_HOST_TEST
#include <DoomPort.h>
#endif

constexpr int OLED_SDA = 9, OLED_SCL = 8;
enum Key : uint8_t { A, B, UP, RIGHT, LEFT, DOWN, KEY_COUNT };
#ifdef MINICONSOLE_TFT114
constexpr uint8_t BUTTON_PINS[KEY_COUNT] = {2, 7, 10, 11, 12, 13};
#else
constexpr uint8_t BUTTON_PINS[KEY_COUNT] = {7, 2, 10, 11, 12, 13};
#endif
constexpr const char *KEY_NAMES[KEY_COUNT] = {"A", "B", "UP", "RIGHT", "LEFT", "DOWN"};
struct Button {
  bool raw = false, held = false, pressed = false, repeated = false;
  uint32_t changedAt = 0, heldAt = 0, repeatAt = 0;
};
Button buttons[KEY_COUNT];
#ifdef MINICONSOLE_TFT114
ConsoleTFT114 oled;
#else
// Use 400 kHz throughout. Full-frame transfers take about 24 ms over I2C.
Adafruit_SSD1306 oled(128, 64, &Wire, -1, 400000, 400000);
#endif
Preferences prefs;
SnakeGame game;
BlocksGame blocks;
BreakoutGame breakout;
InvadersGame invaders;
SokobanGame sokoban;
MotorwaysGame motorways;
enum GameId : uint8_t { SNAKE, BLOCKS, BREAKOUT, INVADERS, SOKOBAN, DOOM, MOTORWAYS, PARTNERS, GAME_COUNT };
constexpr const char *GAME_NAMES[] = {"SNAKE", "TETRIS", "BREAKOUT", "INVADERS", "SOKOBAN", "DOOM", "MICRO MOTORWAYS", "POCKET PARTNERS"};
constexpr const char *GAME_GENRES[] = {"ARCADE", "PUZZLE", "ARCADE", "SHOOTER", "PUZZLE", "FPS", "STRATEGY", "CO-OP PUZZLE"};
enum Screen : uint8_t {
  HOME, SETTINGS, CONTROLS, BUTTON_TEST, PLAYING, PAUSED, GAME_OVER,
  BLOCKS_READY, BLOCKS_PLAYING, BLOCKS_PAUSED, BLOCKS_OVER,
  SYSTEM_MENU, NEW_READY, NEW_PLAYING, NEW_PAUSED, NEW_OVER, CONTINUE_PROMPT, DOOM_RUNNING,
  DRINK_COUNTER, PARTY_LIBRARY, PARTY_PLAYING, APPEARANCE, COLOR_EDITOR, BATTLE_LOBBY, PARTNERS_ROOM
};
Screen screen = HOME;
uint8_t menuItem = 0, settingItem = 0, speed = 1, controlsPage = 0;
bool wrapEdges = false, flipped = false, displayReady = false, won = false, newBest = false;
uint16_t best = 0;
uint32_t blocksBest = 0;
uint32_t breakoutBest = 0, invadersBest = 0, motorBest = 0;
uint32_t motorTick=0,motorNoticeAt=0;
uint8_t motorPauseItem=0,motorRewardItem=0;
bool motorTools=false,motorBPending=false,motorPaletteSaveFailed=false;
uint32_t motorBAt=0,motorDirectionAt=0;
bool motorDirectionPending=false;uint8_t motorDirectionMask=0;
MotorwaysGame::Result motorNotice=MotorwaysGame::OK;
constexpr const char *MOTOR_TOOLS[]={"Roads","Erase","Express"};
constexpr const char *MOTOR_ERRORS[]={"Road confirmed","Blocked route","Need more roads","Need a bridge","Need express link","Road busy","Route too long"};
uint8_t systemItem = 0, sokoLevel = 0, sokoSolved = 0;
uint32_t arcadeTick = 0, sokoMoveAt = 0, selectionAt = 0;
bool storageReady = false;
#ifdef MINICONSOLE_TFT114
UITheme::Palette menuTheme=UITheme::preset(0), pendingTheme=UITheme::preset(0), pickerBefore=UITheme::preset(0);
uint8_t appearanceItem=0, colorRole=0, colorChannel=0;
bool appearanceSaveFailed=false;
SnakeMotion snakeMotion;
#endif
uint8_t settingsTop=0;
// This extra home page is deliberately absent from the normal console UI.
constexpr uint32_t OUT_HOLD_MS = 1200, POUR_MS = 1100, MAX_DRINKS = 99999;
constexpr uint32_t OUT_MODE_FLAG = uint32_t(1) << 31;
constexpr uint8_t MAX_DRINK_GOAL = 99;
constexpr uint32_t CONFETTI_MS = 2200;
bool goingOut = false, menuAPending = false, counterBPending = false;
bool counterInputArmed = false, pouring = false, counterSaveFailed = false;
uint32_t drinkCount = 0, pourStartedAt = 0;
uint8_t drinkGoal = 0; // Zero disables the goal until the user chooses one.
bool confetti = false, goalSaveFailed = false;
uint32_t confettiStartedAt = 0;
enum PartyId : uint8_t { REACTION, HIGH_LOW, PASS_BOMB, PARTY_COUNT };
constexpr const char *PARTY_NAMES[] = {"REACTION", "HIGH/LOW", "PASS BOMB"};
uint8_t partyItem = 0;
bool partyInputArmed = false;
PartyGames::ReactionDuel reaction;
PartyGames::HighLow highLow;
PartyGames::PassBomb passBomb;
SuspendState checkpoint;
uint8_t checkpointBytes[SuspendSave::CAPACITY];
bool checkpointAvailable = false, pauseSaved = false, suppressFire = false;
uint8_t gameSpeed = 1;
uint32_t snakePausedAt = 0;
bool doomAssetsMissing = false, doomInputArmed = false;
bool doomResumeRequested = false;
volatile bool doomDumpRequested = false;

constexpr uint8_t PIEZO_PIN = 4; // Verified passive piezo through the 220-ohm R1.
using Sound = ConsoleSound::Effect;
bool piezoAttached = false, piezoFailed = false;
void writePiezo(uint16_t hz) {
  if (!hz) { if (piezoAttached) ledcWrite(PIEZO_PIN, 0); return; }
  if (piezoFailed) return;
  if (!piezoAttached) {
    piezoAttached = ledcAttach(PIEZO_PIN, 2000, 10);
    if (!piezoAttached) { piezoFailed = true; Serial.println("Piezo PWM unavailable"); return; }
  }
  if (!ledcWriteTone(PIEZO_PIN, hz)) {
    ledcWrite(PIEZO_PIN, 0); piezoFailed = true; Serial.println("Piezo tone failed");
  }
}
ConsoleSound::Player sound(writePiezo);
bool soundMuted = false, soundSaveFailed = false;
bool musicEnabled = true, musicSaveFailed = false;
bool matchGameTheme = true, gameThemeSaveFailed = false;
std::atomic<uint8_t> pendingDoomSound{0};
void playSound(Sound effect, uint32_t now) { sound.play(effect, now); }
void silenceSound() { sound.setMusic(false,millis());sound.stop(); pendingDoomSound.store(0); }
extern "C" void miniconsole_doom_sound(DoomPiezoEvent event) {
  static constexpr Sound cues[]={Sound::None,Sound::Navigate,Sound::Confirm,Sound::Back,
    Sound::Shot,Sound::Explosion,Sound::Pickup,Sound::Hurt,Sound::Door,Sound::Error,Sound::GameOver};
  if(unsigned(event)>=sizeof(cues)/sizeof(cues[0]))return;
  Sound cue=cues[event];if(cue==Sound::None)return;
  // Doom's game task only posts an event. The Arduino loop alone owns PWM and
  // the sequencer; a burst cannot allocate memory or create an audio backlog.
  uint8_t old=pendingDoomSound.load();
  while(ConsoleSound::priority(cue)>=ConsoleSound::priority(Sound(old))) {
    if(pendingDoomSound.compare_exchange_weak(old,uint8_t(cue)))break;
  }
}
bool doomSoundPaused=false;
void serviceDoomSound(uint32_t now,bool paused) {
  if(paused!=doomSoundPaused){silenceSound();playSound(paused?Sound::Pause:Sound::Resume,now);doomSoundPaused=paused;}
  Sound cue=Sound(pendingDoomSound.exchange(0));
  if(!paused)playSound(cue,now);
}

bool doomHasSave() {
#ifdef MINICONSOLE_HOST_TEST
  return false;
#else
  return DoomPort_saved();
#endif
}
uint32_t lastStep = 0, lastFrame = 0, lastStatus = 0, lastDisplayTry = 0, testEnteredAt = 0;
uint32_t lastFall = 0, groundSince = 0, frameCount = 0, transferMicros = 0;
uint32_t blocksPausedAt = 0;
uint32_t clearNoticeAt = 0, clearNoticePoints = 0;
uint8_t clearNoticeLines = 0;
bool clearAnimating = false;
uint32_t clearAnimationAt = 0;
uint8_t lockResets = 0;
bool groundActive = false;
BlockBattle::Match battle;
bool battleMode=false,battleChoosing=true,battleRoleHost=true,battleRadioError=false,battleFrozen=false;
uint32_t battleSentAt=0,battleFreezeAt=0,battleGarbageRng=1,battleLeavingAt=0;
bool battleLeaving=false;
PocketPartners::Session partners;
bool partnersChoosing=true,partnersHost=true,partnersError=false;
uint8_t partnersLevel=0,partnersUnlocked=0;
uint32_t partnersSentAt=0,partnersClosingAt=0;
bool partnersClosing=false;

constexpr const char *SPEED_NAMES[] = {"EASY", "NORMAL", "FAST"};
constexpr uint16_t BASE_INTERVALS[] = {220, 150, 100};

uint16_t snakeInterval() {
  int interval=BASE_INTERVALS[gameSpeed]-int(game.score()/5)*8;
  return interval<65?65:interval;
}

bool portraitScreen() { return (screen >= BLOCKS_READY && screen <= BLOCKS_OVER)||screen==BATTLE_LOBBY; }

uint8_t settingsCount() {
  return (goingOut?6:5)
#ifdef MINICONSOLE_TFT114
    +2
#endif
    ;
}
int8_t gameThemeSettingItem() {
#ifdef MINICONSOLE_TFT114
  return settingsCount()-3;
#else
  return -1; // Color-background preference is only shown on the TFT.
#endif
}
uint8_t musicSettingItem() { return settingsCount()-2; }
uint8_t muteSettingItem() { return settingsCount()-1; }

bool saveSoundMuted(bool muted, uint32_t now) {
  // Apply mute immediately, then verify its single persistent key. A failed
  // write leaves the requested runtime state and a visible retry indicator.
  soundMuted=muted; sound.setMuted(muted); pendingDoomSound.store(0);
  soundSaveFailed=!storageReady || prefs.putBool("muted",muted)!=sizeof(bool) ||
    prefs.getBool("muted",!muted)!=muted;
  if(!muted)playSound(soundSaveFailed?Sound::Error:Sound::Confirm,now);
  return !soundSaveFailed;
}

bool saveMusicEnabled(bool enabled, uint32_t now) {
  musicEnabled=enabled;
  sound.setMusic(enabled&&screen==BLOCKS_PLAYING,now);sound.update(now);
  musicSaveFailed=!storageReady || prefs.putBool("musicOn",enabled)!=sizeof(bool) ||
    prefs.getBool("musicOn",!enabled)!=enabled;
  playSound(musicSaveFailed?Sound::Error:Sound::Confirm,now);
  return !musicSaveFailed;
}

bool saveMatchGameTheme(bool enabled, uint32_t now) {
  matchGameTheme=enabled;
  gameThemeSaveFailed=!storageReady || prefs.putBool("gameTheme",enabled)!=sizeof(bool) ||
    prefs.getBool("gameTheme",!enabled)!=enabled;
  playSound(gameThemeSaveFailed?Sound::Error:Sound::Confirm,now);
  return !gameThemeSaveFailed;
}

#ifdef MINICONSOLE_TFT114
void loadTheme() {
  menuTheme=UITheme::preset(0);
  uint8_t bytes[UITheme::RECORD_SIZE];
  if(storageReady&&prefs.getBytesLength("theme")==sizeof(bytes)&&prefs.getBytes("theme",bytes,sizeof(bytes))==sizeof(bytes))
    UITheme::decode(bytes,sizeof(bytes),menuTheme);
  pendingTheme=pickerBefore=menuTheme;appearanceSaveFailed=false;settingsTop=0;
}
bool applyTheme() {
  appearanceSaveFailed=false;
  if(UITheme::equal(menuTheme,pendingTheme))return true;
  uint8_t bytes[UITheme::RECORD_SIZE],verify[UITheme::RECORD_SIZE];UITheme::Palette checked;
  UITheme::encode(pendingTheme,bytes);
  if(!storageReady||prefs.putBytes("theme",bytes,sizeof(bytes))!=sizeof(bytes)||
     prefs.getBytes("theme",verify,sizeof(verify))!=sizeof(verify)||
     !UITheme::decode(verify,sizeof(verify),checked)||!UITheme::equal(checked,pendingTheme)) {
    appearanceSaveFailed=true;return false;
  }
  menuTheme=pendingTheme;Serial.printf("Appearance saved: %s\n",UITheme::name(menuTheme.preset));return true;
}
#endif

void applyRotation() {
  if (displayReady) oled.setRotation(((portraitScreen() ? 3 : 0) + (flipped ? 2 : 0)) & 3);
}

void changeScreen(Screen nextScreen) {
  if (nextScreen != screen) {
    Screen previous=screen;
    silenceSound();
    Sound cue=Sound::Confirm;
    if(nextScreen==PAUSED||nextScreen==BLOCKS_PAUSED||nextScreen==NEW_PAUSED)cue=Sound::Pause;
    else if(nextScreen==GAME_OVER)cue=won?Sound::Win:Sound::GameOver;
    else if(nextScreen==BLOCKS_OVER)cue=battleMode&&battle.outcome==BlockBattle::WIN?Sound::Win:Sound::GameOver;
    else if(nextScreen==NEW_OVER)cue=menuItem==SOKOBAN?Sound::Win:Sound::GameOver;
    else if(nextScreen==PLAYING||nextScreen==BLOCKS_PLAYING||nextScreen==NEW_PLAYING||nextScreen==DOOM_RUNNING)
      cue=(previous==PAUSED||previous==BLOCKS_PAUSED||previous==NEW_PAUSED||previous==CONTINUE_PROMPT)?Sound::Resume:Sound::Start;
    else if(nextScreen==HOME||buttons[B].pressed)cue=Sound::Back;
    playSound(cue,millis());
    menuAPending = counterBPending = false;
    counterInputArmed = false;
    partyInputArmed = false;
    pouring = confetti = false;
  }
  screen = nextScreen;
  applyRotation();
}

bool saveGoingOut(bool enabled, uint32_t count) {
  // One NVS value commits the mode and count together; never write per frame.
  uint32_t value = (enabled ? OUT_MODE_FLAG : 0) | count;
  if (!storageReady || prefs.putUInt("goingOut", value) != sizeof(value) ||
      prefs.getUInt("goingOut", ~value) != value) {
    counterSaveFailed = true;
    Serial.println("Going Out save failed");
    return false;
  }
  goingOut = enabled; drinkCount = count; counterSaveFailed = false;
  Serial.printf("Going Out: %s drinks=%lu\n", enabled ? "ON" : "OFF", (unsigned long)count);
  return true;
}

bool saveDrinkGoal(uint8_t goal) {
  if (goal > MAX_DRINK_GOAL || !storageReady || prefs.putUChar("drinkGoal", goal) != sizeof(goal) ||
      prefs.getUChar("drinkGoal", 255) != goal) {
    goalSaveFailed = true;
    Serial.println("Drink goal save failed");
    return false;
  }
  drinkGoal = goal; goalSaveFailed = false;
  Serial.printf("Drink goal: %u\n", drinkGoal);
  return true;
}

void handleDrinkCounter(uint32_t now) {
  // Consume the entry hold, including a button held through power-on.
  if (!counterInputArmed) {
    if (!buttons[A].held && !buttons[B].held && !buttons[A].raw && !buttons[B].raw)
      counterInputArmed = true;
    return;
  }
  if (buttons[LEFT].pressed || buttons[UP].pressed || buttons[RIGHT].pressed || buttons[DOWN].pressed) {
    selectionAt = now;
    if (buttons[LEFT].pressed || buttons[UP].pressed) { menuItem = GAME_COUNT - 1; changeScreen(HOME); }
    else { partyItem = 0; changeScreen(PARTY_LIBRARY); }
    return;
  }
  if (buttons[A].pressed) menuAPending = true;
  if (buttons[B].pressed) counterBPending = true;
  if (buttons[B].held && !counterBPending) { menuAPending = false; return; }
  // B takes priority over a simultaneous A press: resetting must not add one.
  if (counterBPending) {
    menuAPending = false;
    if (buttons[B].held && now - buttons[B].heldAt >= OUT_HOLD_MS) {
      counterBPending = false;
      if (saveGoingOut(true, 0)) pouring = confetti = false;
    } else if (!buttons[B].held) {
      counterBPending = false; changeScreen(SYSTEM_MENU);
    }
    return;
  }
  if (menuAPending && buttons[A].held && now - buttons[A].heldAt >= OUT_HOLD_MS) {
    menuAPending = false;
    if (saveGoingOut(false, drinkCount)) changeScreen(HOME);
  } else if (menuAPending && !buttons[A].held) {
    menuAPending = false;
    if (drinkCount < MAX_DRINKS && saveGoingOut(true, drinkCount + 1)) {
      pourStartedAt = now; pouring = true;
      if (drinkGoal && drinkCount == drinkGoal) {
        confettiStartedAt = now; confetti = true;
      }
    }
  }
}

void pollButtons(uint32_t now) {
#if defined(MINICONSOLE_USB_BENCH) && !defined(MINICONSOLE_HOST_TEST)
  static uint32_t injectedUntil[KEY_COUNT];
  if (screen != DOOM_RUNNING) {
    const char *keys = "jkwdas";
    while (Serial.available()) {
      char c = Serial.read(); const char *key = strchr(keys, c);
      if (c == '!') doomDumpRequested = true;
#ifdef MINICONSOLE_TFT114
      if (c == '?') Serial.printf("UI_STATE screen=%u setting=%u item=%u role=%u channel=%u theme=%u preview=%u bg=%04X panel=%04X accent=%04X text=%04X\n",
        unsigned(screen),unsigned(settingItem),unsigned(appearanceItem),unsigned(colorRole),unsigned(colorChannel),unsigned(menuTheme.preset),unsigned(pendingTheme.preset),
        pendingTheme.color[0],pendingTheme.color[1],pendingTheme.color[2],pendingTheme.color[3]);
#endif
      if(c=='?'&&menuItem==MOTORWAYS)Serial.printf("MOTORWAYS ticks=%lu score=%lu homes=%u destinations=%u stock=%u bridges=%u cars=%u editing=%u valid=%u alive=%u\n",
        (unsigned long)motorways.ticks,(unsigned long)motorways.score,motorways.homeCount,motorways.destCount,motorways.stock,motorways.bridges,
        unsigned([]{unsigned n=0;for(const auto &car:motorways.cars)n+=car.active;return n;}()),motorways.editing,motorways.valid(),motorways.alive);
      if(c=='?'&&screen==PARTNERS_ROOM)Serial.printf("PARTNERS host=%u phase=%u paired=%u ready=%u peerReady=%u level=%u stage=%u tick=%lu paused=%u key=%u deaths=%u x0=%d y0=%d x1=%d y1=%d\n",partners.link.host,partners.link.phase,partners.link.paired,partners.link.ready,partners.link.peerReady,partners.game.level,partners.game.stage,(unsigned long)partners.game.ticks,partners.paused(),partners.game.key,partners.game.deaths,partners.game.players[0].x,partners.game.players[0].y,partners.game.players[1].x,partners.game.players[1].y);
      if(c=='v'&&battleMode&&battle.phase==BlockBattle::RUNNING)battle.clear(4); // Bench-only attack transport probe.
      if(c=='?'&&battleMode)Serial.printf("BATTLE phase=%u host=%u paired=%u ready=%u peerReady=%u session=%lu seed=%lu pending=%u sent=%lu outcome=%u\n",battle.phase,battle.host,battle.paired,battle.ready,battle.peerReady,(unsigned long)battle.session,(unsigned long)battle.seed,battle.pending,(unsigned long)battle.sent,battle.outcome);
      if (key && *key) injectedUntil[key - keys] = now + 140;
    }
  }
#endif
  for (int i = 0; i < KEY_COUNT; ++i) {
    Button &button = buttons[i];
    button.pressed = false;
    button.repeated = false;
    bool reading = digitalRead(BUTTON_PINS[i]) == LOW;
#if defined(MINICONSOLE_USB_BENCH) && !defined(MINICONSOLE_HOST_TEST)
    if (screen != DOOM_RUNNING && int32_t(injectedUntil[i] - now) > 0) reading = true;
#endif
    if (reading != button.raw) { button.raw = reading; button.changedAt = now; }
    if (reading != button.held && now - button.changedAt >= 18) {
      button.held = reading;
      if (reading) {
        button.pressed = true;
        button.heldAt = now;
        button.repeatAt = now + 220;
        Serial.printf("Button %s (GPIO%u)\n", KEY_NAMES[i], BUTTON_PINS[i]);
      }
    }
    if (button.held && int32_t(now - button.repeatAt) >= 0) {
      button.repeated = true;
      button.repeatAt = now + 50;
    }
  }
}

void saveSettings() {
  if (!storageReady) return;
  prefs.putUChar("speed", speed);
  prefs.putBool("wrap", wrapEdges);
  prefs.putBool("flip", flipped);
}

uint32_t boundedAge(uint32_t now, uint32_t then, uint32_t limit) {
  uint32_t age = now - then;
  return age > limit ? limit : age;
}

uint32_t clearAnimationDuration();

bool loadCheckpoint() {
  checkpointAvailable = false;
  if (!storageReady) return false;
  size_t size = prefs.getBytesLength("suspend");
  if (!size) return false;
  if (size > SuspendSave::CAPACITY || prefs.getBytes("suspend", checkpointBytes, size) != size ||
      !SuspendSave::decode(checkpointBytes, size, checkpoint)) {
    Serial.println("Checkpoint invalid or incompatible; ignored");
    return false;
  }
  checkpointAvailable = true;
  Serial.printf("Checkpoint ready: game=%s bytes=%u\n", GAME_NAMES[checkpoint.gameId], unsigned(size));
  return true;
}

void clearCheckpoint() {
  pauseSaved = false;
  // A different game keeps the previous checkpoint until it is itself paused.
  if (checkpointAvailable && checkpoint.gameId != menuItem) return;
  if (storageReady && prefs.getBytesLength("suspend") && !prefs.remove("suspend")) Serial.println("Checkpoint removal failed");
  checkpointAvailable = false;
}

bool saveCheckpoint(uint32_t now) {
  if(menuItem==PARTNERS||(battleMode&&menuItem==BLOCKS))return false;
  pauseSaved = false;
  if (!storageReady) return false;
  checkpoint.gameId = menuItem;
  checkpoint.difficulty = gameSpeed;
  checkpoint.stepAge = checkpoint.fallAge = checkpoint.groundAge = checkpoint.animationAge = 0;
  checkpoint.noticeAge = 1200; checkpoint.noticePoints = 0;
  checkpoint.noticeLines = checkpoint.lockResets = 0;
  checkpoint.clearAnimating = checkpoint.groundActive = false;
  if (menuItem == SNAKE) { checkpoint.snake = game; checkpoint.stepAge = boundedAge(now, lastStep, 10000); }
  else if (menuItem == BLOCKS) {
    checkpoint.blocks = blocks;
    checkpoint.fallAge = boundedAge(now, lastFall, 10000);
    checkpoint.groundAge = groundActive ? boundedAge(now, groundSince, 10000) : 0;
    checkpoint.noticeAge = boundedAge(now, clearNoticeAt, 1200);
    checkpoint.animationAge = clearAnimating ? boundedAge(now, clearAnimationAt, clearAnimationDuration()) : 0;
    checkpoint.noticePoints = clearNoticePoints; checkpoint.noticeLines = clearNoticeLines;
    checkpoint.lockResets = lockResets; checkpoint.clearAnimating = clearAnimating; checkpoint.groundActive = groundActive;
  } else if (menuItem == BREAKOUT) { checkpoint.breakout = breakout; checkpoint.stepAge = boundedAge(now, arcadeTick, 10000); }
  else if (menuItem == INVADERS) { checkpoint.invaders = invaders; checkpoint.stepAge = boundedAge(now, arcadeTick, 10000); }
  else if (menuItem == SOKOBAN) checkpoint.sokoban = sokoban;
  else if (menuItem == MOTORWAYS) { checkpoint.motorways=motorways;checkpoint.motorways.cancel();checkpoint.stepAge=boundedAge(now,motorTick,49); }
  else return false;
  size_t size = SuspendSave::encode(checkpoint, checkpointBytes);
  if (!size || prefs.putBytes("suspend", checkpointBytes, size) != size) {
    loadCheckpoint(); Serial.println("Checkpoint save failed"); return false;
  }
  // Preferences commits before returning. Read back and verify before showing SAVED.
  pauseSaved = loadCheckpoint() && checkpoint.gameId == menuItem;
  if (pauseSaved && prefs.getBool("doomResume", false)) prefs.putBool("doomResume", false);
  Serial.printf("Pause checkpoint: %s\n", pauseSaved ? "SAVED" : "FAILED");
  return pauseSaved;
}

bool resumeCheckpoint(uint32_t now) {
  if (!loadCheckpoint()) { changeScreen(HOME); return false; }
  menuItem = checkpoint.gameId; gameSpeed = checkpoint.difficulty;
  won = newBest = false; pauseSaved = true; suppressFire = true;
  if (menuItem == SNAKE) {
    game = checkpoint.snake; lastStep = now - checkpoint.stepAge; changeScreen(PLAYING);
#ifdef MINICONSOLE_TFT114
    snakeMotion.reset();
#endif
  } else if (menuItem == BLOCKS) {
    blocks = checkpoint.blocks;
    lastFall = now - checkpoint.fallAge; groundSince = now - checkpoint.groundAge;
    clearNoticeAt = now - checkpoint.noticeAge; clearAnimationAt = now - checkpoint.animationAge;
    clearNoticePoints = checkpoint.noticePoints; clearNoticeLines = checkpoint.noticeLines;
    lockResets = checkpoint.lockResets; clearAnimating = checkpoint.clearAnimating; groundActive = checkpoint.groundActive;
    changeScreen(BLOCKS_PLAYING);
  } else {
    if (menuItem == BREAKOUT) breakout = checkpoint.breakout;
    if (menuItem == INVADERS) invaders = checkpoint.invaders;
    if (menuItem == SOKOBAN) { sokoban = checkpoint.sokoban; sokoLevel = sokoban.level; }
    if(menuItem==MOTORWAYS){motorways=checkpoint.motorways;motorways.systemTheme=prefs.getBool("motorTheme",motorways.systemTheme);motorTools=motorBPending=motorDirectionPending=false;motorways.cancel();motorTick=now-checkpoint.stepAge;motorPauseItem=motorRewardItem=0;motorNoticeAt=now-2000;}
    arcadeTick = now - checkpoint.stepAge; sokoMoveAt = now; changeScreen(NEW_PLAYING);
  }
  Serial.printf("Checkpoint resumed: %s\n", GAME_NAMES[menuItem]);
  return true;
}

void startGame(uint32_t now) {
  clearCheckpoint(); gameSpeed = speed;
  game.reset(wrapEdges);
  game.placeFood(esp_random());
#ifdef MINICONSOLE_TFT114
  snakeMotion.reset();
#endif
  won = newBest = false;
  changeScreen(PLAYING);
  lastStep = now;
  Serial.println("Snake started");
}

void finishGame(bool victory) {
  clearCheckpoint();
  won = victory;
  changeScreen(GAME_OVER);
  if (game.score() > best) {
    best = game.score();
    newBest = true;
    if (storageReady) prefs.putUShort("best", best);
  }
  Serial.printf("Snake ended: score=%u best=%u won=%u\n", game.score(), best, won);
}

void resetBlocksTimers(uint32_t now) {
  lastFall = now;
  groundSince = now;
  groundActive = false;
  lockResets = 0;
}

void openBattleLobby(uint32_t now){
  BattleRadio::stop();battleMode=true;battleChoosing=true;battleRadioError=false;battleLeaving=false;battleFrozen=false;
  battle=BlockBattle::Match{};clearAnimating=false;changeScreen(BATTLE_LOBBY);silenceSound();
}
void leaveBattle(uint32_t now){battleLeaving=true;battleLeavingAt=now;if(battle.phase!=BlockBattle::ENDED)battle.topOut(now);}
void handleBattleLobby(uint32_t now){
  if(buttons[B].pressed){BattleRadio::stop();battleMode=false;changeScreen(BLOCKS_READY);return;}
  if(battleChoosing){
    if(buttons[UP].pressed||buttons[DOWN].pressed||buttons[LEFT].pressed||buttons[RIGHT].pressed)battleRoleHost=!battleRoleHost;
    if(buttons[A].pressed){battleRadioError=!BattleRadio::begin();if(!battleRadioError){battle.open(battleRoleHost,esp_random(),esp_random(),now,speed);battleChoosing=false;battleSentAt=now-200;}}
  }else if(buttons[A].pressed&&battle.paired&&battle.phase==BlockBattle::LOBBY){battle.ready=!battle.ready;playSound(Sound::Confirm,now);}
}
void serviceBattle(uint32_t now){
  if(!battleMode)return;
  if(battleLeaving&&now-battleLeavingAt>=600){BattleRadio::stop();battleMode=false;battleLeaving=false;changeScreen(HOME);return;}
  if(battleChoosing)return;
  BattleRadio::Message message;
  while(BattleRadio::poll(message)){BlockBattle::Packet p;if(BlockBattle::decode(message.data,message.length,p))battle.receive(message.mac,p,now);}
  battle.tick(now);
  if(battle.startEvent){
    battle.startEvent=false;blocks.reset(battle.seed);gameSpeed=battle.speed;clearAnimating=false;clearNoticeLines=0;newBest=false;
    battleGarbageRng=battle.seed^battle.nonce;if(!battleGarbageRng)battleGarbageRng=1;resetBlocksTimers(now);battleFrozen=false;changeScreen(BLOCKS_PLAYING);sound.restartMusic(now);
    Serial.printf("Battle started seed=%lu session=%lu\n",(unsigned long)battle.seed,(unsigned long)battle.session);
  }
  battle.height=BlockBattle::stackHeight(blocks);
  if(battle.phase==BlockBattle::ENDED&&screen!=BLOCKS_OVER&&screen!=BATTLE_LOBBY){changeScreen(BLOCKS_OVER);playSound(battle.outcome==BlockBattle::WIN?Sound::Win:Sound::GameOver,now);}
  if(battle.phase==BlockBattle::ENDED&&screen==BATTLE_LOBBY)changeScreen(BLOCKS_OVER);
  bool frozen=battle.frozen(now);
  if(frozen&&!battleFrozen){battleFreezeAt=now;battleFrozen=true;}
  if(!frozen&&battleFrozen){uint32_t age=now-battleFreezeAt;lastFall+=age;groundSince+=age;clearAnimationAt+=age;clearNoticeAt+=age;battleFrozen=false;}
  if(now-battleSentAt>=100){battleSentAt=now;auto p=battle.packet(now);BattleRadio::send(battle.paired?battle.peer:nullptr,p);}
}

void startBlocks(uint32_t now) {
  if(battleMode){BattleRadio::stop();battleMode=false;}
  clearCheckpoint(); gameSpeed = speed;
  blocks.reset(esp_random());
  newBest = false;
  clearNoticeLines = 0;
  clearAnimating = false;
  resetBlocksTimers(now);
  changeScreen(BLOCKS_PLAYING);
  sound.restartMusic(now);
  Serial.println("Tetris started (portrait, D-pad at bottom)");
}

void saveBlocksBest() {
  if (blocks.score > blocksBest) {
    blocksBest = blocks.score;
    newBest = true;
    if (storageReady) prefs.putUInt("blocksBestGB", blocksBest);
  }
}

void finishBlocksIfNeeded() {
  if (!blocks.alive) {
    if(battleMode){battle.topOut(millis());return;}
    saveBlocksBest();
    clearCheckpoint();
    changeScreen(BLOCKS_OVER);
    Serial.printf("Blocks ended: score=%lu lines=%u best=%lu\n",
                  (unsigned long)blocks.score, blocks.lines, (unsigned long)blocksBest);
  }
}

void afterBlocksLock(uint32_t now) {
  resetBlocksTimers(now);
  if(battleMode)battle.clear(blocks.lastCleared);
  if (blocks.lastCleared) {
    clearNoticeAt = now;
    clearNoticePoints = blocks.lastClearPoints;
    clearNoticeLines = blocks.lastCleared;
    clearAnimationAt = now;
    clearAnimating = true;
    Serial.printf("Blocks clear: lines=%u points=%lu total=%lu\n",
                  clearNoticeLines, (unsigned long)clearNoticePoints, (unsigned long)blocks.score);
  }
  if (blocks.lastCleared) {
    static constexpr Sound clears[]={Sound::None,Sound::Clear1,Sound::Clear2,Sound::Clear3,Sound::Tetris};
    playSound(clears[blocks.lastCleared],now);
  } else if(sound.current()!=Sound::Drop)playSound(Sound::Lock,now);
  if(!clearAnimating&&battleMode&&blocks.alive&&battle.pending){uint16_t n=battle.pending;battle.pending=0;BlockBattle::garbage(blocks,n,battleGarbageRng);}
  if (!clearAnimating) finishBlocksIfNeeded();
}

uint32_t clearAnimationDuration() { return clearNoticeLines == 4 ? 650 : 420; }

void resetLockAfterMove(uint32_t now) {
  if (groundActive && lockResets < 15) { groundSince = now; ++lockResets; }
}

void updateBlocks(uint32_t now) {
  if(battleMode&&!blocks.alive)return;
  if (clearAnimating) {
    if (now - clearAnimationAt >= clearAnimationDuration()) {
      clearAnimating = false;
      resetBlocksTimers(now);
      // Keep the awarded points visible for another 600 ms after the wipe.
      clearNoticeAt = now - 600;
      if(battleMode&&blocks.alive&&battle.pending){uint16_t n=battle.pending;battle.pending=0;BlockBattle::garbage(blocks,n,battleGarbageRng);}
      finishBlocksIfNeeded();
    }
    return;
  }
  int interval = 800 - int(gameSpeed) * 200 - int(blocks.level()) * 55;
  if (interval < 80) interval = 80;
  // After turning the console clockwise: physical Right is portrait Down.
  bool soft = buttons[RIGHT].held;
  if (soft) interval = 40;
  if (now - lastFall >= uint32_t(interval)) {
    lastFall = now;
    blocks.fall(soft);
  }
  if (!blocks.grounded()) groundActive = false;
  else {
    if (!groundActive) { groundActive = true; groundSince = now; }
    if (now - groundSince >= 400) {
      blocks.lock();
      afterBlocksLock(now);
    }
  }
}

void saveArcadeBest() {
  uint32_t value = menuItem == MOTORWAYS ? motorways.score : menuItem == BREAKOUT ? breakout.score : invaders.score;
  uint32_t &record = menuItem == MOTORWAYS ? motorBest : menuItem == BREAKOUT ? breakoutBest : invadersBest;
  if (value > record) {
    record = value; newBest = true;
    if (storageReady) prefs.putUInt(menuItem == MOTORWAYS ? "motorBest" : menuItem == BREAKOUT ? "breakBest" : "invBest", record);
  }
}

#include "PartnersController.h"

void startSelectedGame(uint32_t now) {
  if(menuItem==PARTNERS){openPartners(now);return;}
  newBest = false;
  if (menuItem == DOOM) {
    BattleRadio::stop();battleMode=false;
#ifdef MINICONSOLE_HOST_TEST
    changeScreen(DOOM_RUNNING);
#else
    doomAssetsMissing = !DoomPort_available();
    if (doomAssetsMissing) { changeScreen(NEW_READY); return; }
    doomInputArmed = false;
    changeScreen(DOOM_RUNNING);
    oled.clearDisplay(); oled.setTextColor(WHITE); oled.setTextSize(1);
    oled.setCursor(40, 26); oled.print("DOOM"); oled.display();
    if (!DoomPort_start(doomResumeRequested)) { doomAssetsMissing = true; changeScreen(NEW_READY); }
#endif
    return;
  }
  if (menuItem == SNAKE) { startGame(now); return; }
  clearCheckpoint(); gameSpeed = speed; suppressFire = false;
  if (menuItem == BREAKOUT) breakout.reset();
  if (menuItem == INVADERS) invaders.reset(esp_random());
  if (menuItem == SOKOBAN) sokoban.reset(sokoLevel);
  if (menuItem == MOTORWAYS) { motorways.reset(esp_random(),speed);motorways.systemTheme=prefs.getBool("motorTheme",false);motorTools=motorBPending=motorDirectionPending=false;motorTick=now;motorPauseItem=motorRewardItem=0;motorNoticeAt=now-2000; }
  arcadeTick = sokoMoveAt = now;
  changeScreen(NEW_PLAYING);
}

void updateArcade(uint32_t now) {
  // Fixed simulation ticks keep speed independent of OLED transfer time.
  if (now - arcadeTick > 128) arcadeTick = now - 128;
  while (screen == NEW_PLAYING && now - arcadeTick >= 16) {
    arcadeTick += 16;
    int direction = int(buttons[RIGHT].held) - int(buttons[LEFT].held);
    bool alive = true;
    if (menuItem == BREAKOUT) {
      uint8_t lives=breakout.lives;uint16_t level=breakout.level;uint32_t score=breakout.score;
      bool serving=breakout.serving;float vx=breakout.vx,vy=breakout.vy;
      breakout.step(direction, buttons[A].held && !suppressFire); alive = breakout.alive;
      if(breakout.lives<lives)playSound(Sound::Hurt,now);
      else if(breakout.level!=level)playSound(Sound::Win,now);
      else if(breakout.score!=score)playSound(Sound::Brick,now);
      else if(serving&&!breakout.serving)playSound(Sound::Shot,now);
      else if(!serving&&(vx*breakout.vx<0||vy*breakout.vy<0))playSound(Sound::Bounce,now);
    } else if (menuItem == INVADERS) {
      uint8_t lives=invaders.lives;uint16_t wave=invaders.wave,fire=invaders.fireClock;
      uint32_t score=invaders.score;int x=invaders.formationX,y=invaders.formationY;
      invaders.step(direction, buttons[A].held && !suppressFire); alive = invaders.alive;
      if(invaders.lives<lives)playSound(Sound::Hurt,now);
      else if(invaders.wave!=wave)playSound(Sound::Win,now);
      else if(invaders.score!=score)playSound(Sound::Hit,now);
      else if(invaders.fireClock==14&&fire<=1)playSound(Sound::Shot,now);
      else if(invaders.formationX!=x||invaders.formationY!=y)playSound(Sound::Move,now);
    }
    if (!alive) { saveArcadeBest(); clearCheckpoint(); changeScreen(NEW_OVER); }
  }
}

uint32_t bombSoundAt=0;
void reactionSound(PartyGames::ReactionDuel::Phase before,uint32_t now) {
  if(reaction.phase==before)return;
  if(reaction.phase==PartyGames::ReactionDuel::GO)playSound(Sound::Ready,now);
  else if(reaction.phase==PartyGames::ReactionDuel::RESULT)
    playSound(reaction.falseStart?Sound::Error:reaction.winner==PartyGames::ReactionDuel::NONE?Sound::Back:Sound::Win,now);
}
void updateParty(uint32_t now) {
  if (partyItem == REACTION) {auto before=reaction.phase;reaction.update(now);reactionSound(before,now);}
  if (partyItem == HIGH_LOW) {
    auto before=highLow.phase;highLow.update(now);
    if(before!=highLow.phase&&highLow.phase==PartyGames::HighLow::RESULT)
      playSound(highLow.outcome==PartyGames::HighLow::CORRECT?Sound::Target:highLow.outcome==PartyGames::HighLow::WRONG?Sound::Error:Sound::Back,now);
  }
  if (partyItem == PASS_BOMB) {
    auto before=passBomb.phase;passBomb.update(now);
    if(before!=passBomb.phase&&passBomb.phase==PartyGames::PassBomb::EXPLODED)playSound(Sound::Explosion,now);
    if(passBomb.phase==PartyGames::PassBomb::RUNNING) {
      uint32_t elapsed=now-passBomb.startedAt;
      uint32_t interval=700-uint32_t(uint64_t(elapsed)*550/passBomb.fuseMs);
      if(now-bombSoundAt>=interval){bombSoundAt=now;playSound(Sound::Tick,now);}
    }
  }
}

void handlePartyInput(uint32_t now) {
  if (!goingOut) { changeScreen(HOME); return; }
  if (buttons[LEFT].pressed) { changeScreen(PARTY_LIBRARY); return; }
  if (!partyInputArmed) {
    if (!buttons[A].raw && !buttons[B].raw && !buttons[A].held && !buttons[B].held) partyInputArmed = true;
    return;
  }
  // Resolve deadlines before processing a new press, even on a delayed frame.
  bool wasBombRunning = partyItem == PASS_BOMB && passBomb.phase == PartyGames::PassBomb::RUNNING;
  bool wasReactionRunning = partyItem == REACTION && (reaction.phase == PartyGames::ReactionDuel::WAIT || reaction.phase == PartyGames::ReactionDuel::GO);
  updateParty(now);
  if ((wasBombRunning && passBomb.phase == PartyGames::PassBomb::EXPLODED) ||
      (wasReactionRunning && reaction.phase == PartyGames::ReactionDuel::RESULT)) return;
  bool a = buttons[A].pressed, b = buttons[B].pressed;
  if (partyItem == REACTION) {
    if (reaction.phase == PartyGames::ReactionDuel::READY) {
      if (a) {reaction.begin(now, esp_random());playSound(Sound::Confirm,now);}
    } else if (reaction.phase == PartyGames::ReactionDuel::RESULT) {
      // Both reaction buttons remain harmless after the round, even when the
      // losing player presses later or either player keeps their button held.
      if (buttons[UP].pressed) {reaction.begin(now, esp_random());playSound(Sound::Confirm,now);}
    } else {auto before=reaction.phase;reaction.update(now, a, b);reactionSound(before,now);}
  } else if (partyItem == HIGH_LOW) {
    if (highLow.phase == PartyGames::HighLow::READY) { if (a) {highLow.begin(esp_random());playSound(Sound::Deal,now);} }
    else if (highLow.phase == PartyGames::HighLow::GUESS) {
      if (a != b && highLow.guess(a, now)) playSound(Sound::Deal,now);
    } else if (highLow.phase == PartyGames::HighLow::RESULT && a) {
      if (!highLow.next()) highLow.begin(esp_random());
      playSound(Sound::Deal,now);
    }
  } else {
    if (passBomb.phase != PartyGames::PassBomb::RUNNING) { if (a) {passBomb.begin(now, esp_random());bombSoundAt=now;playSound(Sound::Confirm,now);} }
    else if (a) {passBomb.pass(now);playSound(Sound::Tick,now);}
  }
}

void updateMotorways(uint32_t now) {
  if(motorways.editing||motorways.rewardPending||motorTools||motorBPending){motorTick=now;return;}
  if(now-motorTick>200)motorTick=now-200;
  while(screen==NEW_PLAYING&&now-motorTick>=50){motorTick+=50;uint8_t event=motorways.step();
    if(event&MotorwaysGame::FAILURE){saveArcadeBest();clearCheckpoint();changeScreen(NEW_OVER);return;}
    if(event&MotorwaysGame::WEEK){motorRewardItem=0;playSound(Sound::Win,now);saveCheckpoint(now);motorTick=now;return;}
    if(event&MotorwaysGame::WARNING)playSound(Sound::Error,now);
    else if(event&(MotorwaysGame::GROWTH|MotorwaysGame::REDEVELOP|MotorwaysGame::OPENED))playSound(Sound::Pickup,now);
    else if(event&MotorwaysGame::DELIVERY)playSound(Sound::Target,now);
  }
}
void motorResult(MotorwaysGame::Result result,uint32_t now) {
  motorNotice=result;motorNoticeAt=now;playSound(result==MotorwaysGame::OK?Sound::Confirm:Sound::Error,now);
}
void moveMotorwaysMask(uint8_t mask,uint32_t now){
  int dx=bool(mask&2)-bool(mask&8),dy=bool(mask&4)-bool(mask&1),d=-1;
  if(dx&&dy)d=dy<0?(dx>0?4:7):(dx>0?5:6);
  else if(dx)d=dx>0?1:3;else if(dy)d=dy>0?2:0;
  if(d>=0){auto result=motorways.move(d);if(result!=MotorwaysGame::OK)motorResult(result,now);else playSound(Sound::Move,now);}
}
void handleMotorwaysInput(uint32_t now) {
  if(screen==NEW_PAUSED){
    if(buttons[B].pressed){saveArcadeBest();changeScreen(HOME);return;}
    if(buttons[UP].pressed)motorPauseItem=(motorPauseItem+4)%5;
    if(buttons[DOWN].pressed)motorPauseItem=(motorPauseItem+1)%5;
    bool cycle=motorPauseItem==1&&(buttons[LEFT].pressed||buttons[RIGHT].pressed||buttons[A].pressed);
    if(cycle){motorways.tool=MotorwaysGame::Tool((motorways.tool+(buttons[LEFT].pressed?2:1))%3);saveCheckpoint(now);playSound(Sound::Navigate,now);return;}
    if(motorPauseItem==2&&(buttons[LEFT].pressed||buttons[RIGHT].pressed||buttons[A].pressed)){
      motorways.systemTheme=!motorways.systemTheme;
      motorPaletteSaveFailed=!storageReady||prefs.putBool("motorTheme",motorways.systemTheme)!=1||prefs.getBool("motorTheme",!motorways.systemTheme)!=motorways.systemTheme;
      saveCheckpoint(now);playSound(motorPaletteSaveFailed?Sound::Error:Sound::Navigate,now);return;}
    if(buttons[A].pressed){
      if(motorPauseItem==0){if(!pauseSaved)saveCheckpoint(now);motorTick=now;changeScreen(NEW_PLAYING);}
      if(motorPauseItem==3){saveArcadeBest();startSelectedGame(now);}
      if(motorPauseItem==4){saveArcadeBest();changeScreen(HOME);}
    }return;
  }
  if(buttons[B].pressed){motorDirectionPending=false;motorBPending=true;motorBAt=now;motorTick=now;}
  if(motorBPending){
    motorTick=now;
    if(buttons[B].held&&now-motorBAt>=600){
      motorBPending=false;motorTools=false;motorways.cancel();motorPauseItem=0;
      changeScreen(NEW_PAUSED);saveCheckpoint(now);return;
    }
    if(!buttons[B].held&&!buttons[B].pressed){
      motorBPending=false;
      if(motorways.editing)motorways.cancel();
      motorTools=!motorTools;playSound(Sound::Navigate,now);
    }
    return;
  }
  if(motorTools){
    motorTick=now;
    if(buttons[UP].pressed||buttons[UP].repeated)motorways.tool=MotorwaysGame::Tool((motorways.tool+2)%3);
    if(buttons[DOWN].pressed||buttons[DOWN].repeated)motorways.tool=MotorwaysGame::Tool((motorways.tool+1)%3);
    if(buttons[A].pressed){motorTools=false;playSound(Sound::Confirm,now);}
    return;
  }
  if(motorways.rewardPending){
    if(buttons[UP].pressed)motorRewardItem=(motorRewardItem+2)%3;
    if(buttons[DOWN].pressed)motorRewardItem=(motorRewardItem+1)%3;
    if(buttons[A].pressed){motorways.reward(motorRewardItem);motorTick=now;saveCheckpoint(now);playSound(Sound::Pickup,now);}return;
  }
  if(buttons[A].pressed&&motorDirectionPending){
    uint8_t held=0;const Key directions[]={UP,RIGHT,DOWN,LEFT};for(int d=0;d<4;++d)if(buttons[directions[d]].held)held|=1<<d;
    moveMotorwaysMask(held?held:motorDirectionMask,now);motorDirectionPending=false;
  }
  if(buttons[A].pressed){motorResult(motorways.editing?motorways.commit():motorways.begin(),now);motorTick=now;return;}
  const Key keys[]={UP,RIGHT,DOWN,LEFT};
  uint8_t held=0,edges=0;
  for(int d=0;d<4;++d){if(buttons[keys[d]].held)held|=1<<d;if(buttons[keys[d]].pressed||buttons[keys[d]].repeated)edges|=1<<d;}
  if(edges){if(!motorDirectionPending){motorDirectionAt=now;motorDirectionMask=0;}motorDirectionPending=true;motorDirectionMask|=edges;}
  if(motorDirectionPending&&now-motorDirectionAt>=35){
    uint8_t mask=held?held:motorDirectionMask;motorDirectionPending=false;
    moveMotorwaysMask(mask,now);
  }
}

void handleInput(uint32_t now) {
  if(screen==PARTNERS_ROOM){handlePartnersInput(now);return;}
  if(screen==BATTLE_LOBBY){handleBattleLobby(now);return;}
  if(menuItem==MOTORWAYS&&(screen==NEW_PLAYING||screen==NEW_PAUSED)){handleMotorwaysInput(now);return;}
  bool menuScreen=screen==HOME||screen==SYSTEM_MENU||screen==SETTINGS||screen==CONTROLS||
    screen==PARTY_LIBRARY||screen==NEW_READY||screen==APPEARANCE||screen==COLOR_EDITOR;
  if(menuScreen) {
    if(buttons[B].pressed)playSound(Sound::Back,now);
    else if(buttons[A].pressed&&screen!=HOME&&screen!=PARTY_LIBRARY)playSound(Sound::Confirm,now);
    else for(Key key:{UP,RIGHT,LEFT,DOWN})if(buttons[key].pressed||
      (screen==COLOR_EDITOR&&buttons[key].repeated)){playSound(Sound::Navigate,now);break;}
  }
  if (screen == DRINK_COUNTER) {
    handleDrinkCounter(now);
  } else if (screen == PARTY_PLAYING) {
    handlePartyInput(now);
  } else if (screen == PARTY_LIBRARY) {
    if (!goingOut) { changeScreen(HOME); return; }
    if (buttons[LEFT].pressed || buttons[UP].pressed) {
      menuAPending = false; selectionAt = now;
      if (partyItem == 0) { changeScreen(DRINK_COUNTER); return; }
      --partyItem;
    }
    if (buttons[RIGHT].pressed || buttons[DOWN].pressed) {
      menuAPending = false; selectionAt = now;
      if (partyItem == PARTY_COUNT - 1) { menuItem = 0; changeScreen(HOME); return; }
      ++partyItem;
    }
    if (buttons[B].pressed) { changeScreen(SYSTEM_MENU); return; }
    if (buttons[A].pressed) menuAPending = true;
    if (menuAPending && buttons[A].held && now - buttons[A].heldAt >= OUT_HOLD_MS) {
      changeScreen(DRINK_COUNTER); return;
    }
    if (menuAPending && !buttons[A].held) {
      menuAPending = false;
      if (partyItem == REACTION) reaction = PartyGames::ReactionDuel{};
      if (partyItem == HIGH_LOW) highLow = PartyGames::HighLow{};
      if (partyItem == PASS_BOMB) passBomb = PartyGames::PassBomb{};
      changeScreen(PARTY_PLAYING);
    }
  } else if (screen == HOME) {
    if (buttons[LEFT].pressed || buttons[UP].pressed) {
      if (goingOut && menuItem == 0) { partyItem = PARTY_COUNT - 1; changeScreen(PARTY_LIBRARY); return; }
      menuItem = (menuItem + GAME_COUNT - 1) % GAME_COUNT; selectionAt = now; menuAPending = false;
    }
    if (buttons[RIGHT].pressed || buttons[DOWN].pressed) {
      if (goingOut && menuItem == GAME_COUNT - 1) { changeScreen(DRINK_COUNTER); return; }
      menuItem = (menuItem + 1) % GAME_COUNT; selectionAt = now; menuAPending = false;
    }
    if (buttons[B].pressed) { changeScreen(SYSTEM_MENU); return; }
    // Library A activates on release so its long hold can stay a hidden shortcut.
    if (buttons[A].pressed) menuAPending = true;
    if (menuAPending && buttons[A].held && now - buttons[A].heldAt >= OUT_HOLD_MS) {
      menuAPending = false;
      if (goingOut || saveGoingOut(true, drinkCount)) changeScreen(DRINK_COUNTER);
      return;
    }
    if (menuAPending && !buttons[A].held) {
      menuAPending = false;
      if ((menuItem == DOOM && doomHasSave()) || (checkpointAvailable && checkpoint.gameId == menuItem)) changeScreen(CONTINUE_PROMPT);
      else if(menuItem==PARTNERS)openPartners(now);
      else if (menuItem == BLOCKS) changeScreen(BLOCKS_READY);
      else changeScreen(NEW_READY);
    }
  } else if (screen == CONTINUE_PROMPT) {
    if (buttons[B].pressed) changeScreen(HOME);
    else if (buttons[A].pressed) {
      if (menuItem == DOOM) { doomResumeRequested = true; startSelectedGame(now); }
      else resumeCheckpoint(now);
    }
    else if (buttons[LEFT].pressed) {
      if (menuItem == DOOM) {
#ifndef MINICONSOLE_HOST_TEST
        DoomPort_clearSaved();
#endif
        doomResumeRequested = false;
      } else clearCheckpoint();
      changeScreen(menuItem == BLOCKS ? BLOCKS_READY : NEW_READY);
    }
  } else if (screen == SYSTEM_MENU) {
    if (buttons[B].pressed) { changeScreen(HOME); return; }
    if (buttons[UP].pressed) systemItem = (systemItem + 2) % 3;
    if (buttons[DOWN].pressed) systemItem = (systemItem + 1) % 3;
    if (buttons[A].pressed) {
      if (systemItem == 0) changeScreen(SETTINGS);
      if (systemItem == 1) { controlsPage = menuItem; changeScreen(CONTROLS); }
      if (systemItem == 2) { changeScreen(BUTTON_TEST); testEnteredAt = now; }
    }
  } else if (screen == SETTINGS) {
    if (buttons[B].pressed) { changeScreen(SYSTEM_MENU); return; }
    uint8_t count = settingsCount();
    if (settingItem >= count) settingItem = 0;
    if (buttons[UP].pressed) settingItem = (settingItem + count - 1) % count;
    if (buttons[DOWN].pressed) settingItem = (settingItem + 1) % count;
#ifdef MINICONSOLE_TFT114
    if(settingItem==(goingOut?4:3)) {
      if(buttons[A].pressed){pendingTheme=menuTheme;appearanceItem=0;appearanceSaveFailed=false;changeScreen(APPEARANCE);}
      return;
    }
#endif
    int change = buttons[LEFT].pressed ? -1 : (buttons[RIGHT].pressed || buttons[A].pressed ? 1 : 0);
    if (change) {
      if(settingItem==gameThemeSettingItem()) {
        saveMatchGameTheme(gameThemeSaveFailed&&buttons[A].pressed?matchGameTheme:!matchGameTheme,now);return;
      }
      if(settingItem==musicSettingItem()) {
        saveMusicEnabled(musicSaveFailed&&buttons[A].pressed?musicEnabled:!musicEnabled,now);return;
      }
      if(settingItem==muteSettingItem()) {
        saveSoundMuted(soundSaveFailed&&buttons[A].pressed?soundMuted:!soundMuted,now);return;
      }
      if (settingItem == 3 && goingOut) {
        int goal = int(drinkGoal) + change;
        if (goal >= 0 && goal <= MAX_DRINK_GOAL) saveDrinkGoal(uint8_t(goal));
        return;
      }
      if (settingItem == 0) speed = (speed + change + 3) % 3;
      if (settingItem == 1) wrapEdges = !wrapEdges;
      if (settingItem == 2) { flipped = !flipped; applyRotation(); }
      saveSettings();
    }
#ifdef MINICONSOLE_TFT114
  } else if(screen==APPEARANCE) {
    if(buttons[B].pressed){pendingTheme=menuTheme;appearanceSaveFailed=false;changeScreen(SETTINGS);return;}
    if(buttons[UP].pressed)appearanceItem=(appearanceItem+6)%7;
    if(buttons[DOWN].pressed)appearanceItem=(appearanceItem+1)%7;
    int delta=buttons[LEFT].pressed?-1:buttons[RIGHT].pressed?1:0;
    if(appearanceItem==0&&(delta||buttons[A].pressed)) {
      int i=pendingTheme.preset==UITheme::CUSTOM?(delta<0?0:-1):pendingTheme.preset;
      pendingTheme=UITheme::preset(uint8_t((i+(delta?delta:1)+UITheme::PRESET_COUNT)%UITheme::PRESET_COUNT));
      appearanceSaveFailed=false;
    } else if(buttons[A].pressed) {
      if(appearanceItem<=4){pickerBefore=pendingTheme;colorRole=appearanceItem-1;colorChannel=0;changeScreen(COLOR_EDITOR);}
      else if(appearanceItem==5){pendingTheme=UITheme::preset(0);appearanceSaveFailed=false;}
      else if(applyTheme())changeScreen(SETTINGS);
    }
  } else if(screen==COLOR_EDITOR) {
    if(buttons[B].pressed){pendingTheme=pickerBefore;changeScreen(APPEARANCE);return;}
    if(buttons[A].pressed){changeScreen(APPEARANCE);return;}
    if(buttons[UP].pressed)colorChannel=(colorChannel+2)%3;
    if(buttons[DOWN].pressed)colorChannel=(colorChannel+1)%3;
    int delta=(buttons[LEFT].pressed||buttons[LEFT].repeated)?-1:(buttons[RIGHT].pressed||buttons[RIGHT].repeated)?1:0;
    if(delta){uint16_t &c=pendingTheme.color[colorRole];uint16_t next=UITheme::adjust(c,colorChannel,delta);
      if(c!=next){c=next;pendingTheme.preset=UITheme::CUSTOM;appearanceSaveFailed=false;}}
#endif
  } else if (screen == CONTROLS) {
    if (buttons[LEFT].pressed) controlsPage = (controlsPage + GAME_COUNT) % (GAME_COUNT+1);
    if (buttons[RIGHT].pressed) controlsPage = (controlsPage + 1) % (GAME_COUNT+1);
    if (buttons[B].pressed || buttons[A].pressed) changeScreen(SYSTEM_MENU);
  } else if (screen == BUTTON_TEST) {
    if (buttons[B].held && now - buttons[B].heldAt >= 800 && now - testEnteredAt >= 800) changeScreen(SYSTEM_MENU);
  } else if (screen == PLAYING) {
    if (buttons[A].pressed || buttons[B].pressed) { snakePausedAt = now; changeScreen(PAUSED); saveCheckpoint(now); return; }
    // Two queued turns preserve quick corner inputs between movement ticks.
    if (buttons[UP].pressed) game.queueTurn(SnakeGame::UP);
    if (buttons[RIGHT].pressed) game.queueTurn(SnakeGame::RIGHT);
    if (buttons[DOWN].pressed) game.queueTurn(SnakeGame::DOWN);
    if (buttons[LEFT].pressed) game.queueTurn(SnakeGame::LEFT);
  } else if (screen == PAUSED) {
    if (buttons[B].pressed) {
      if (game.score() > best) { best = game.score(); if (storageReady) prefs.putUShort("best", best); }
      changeScreen(HOME);
    }
    else if (buttons[A].pressed) { changeScreen(PLAYING); lastStep += now - snakePausedAt; }
  } else if (screen == GAME_OVER) {
    if (buttons[B].pressed) changeScreen(HOME);
    else if (buttons[A].pressed) startGame(now);
  } else if (screen == NEW_READY) {
    if (buttons[B].pressed) { changeScreen(HOME); return; }
    if (menuItem == SOKOBAN) {
      uint8_t limit = sokoSolved < SokobanGame::LEVELS ? sokoSolved : SokobanGame::LEVELS - 1;
      if (buttons[LEFT].pressed) sokoLevel = sokoLevel == 0 ? limit : sokoLevel - 1;
      if (buttons[RIGHT].pressed) sokoLevel = sokoLevel == limit ? 0 : sokoLevel + 1;
    }
    if (buttons[A].pressed) { doomResumeRequested = false; startSelectedGame(now); }
  } else if (screen == NEW_PLAYING) {
    if (buttons[B].pressed) { changeScreen(NEW_PAUSED); saveCheckpoint(now); return; }
    if (menuItem == SOKOBAN) {
      if (buttons[A].pressed) { playSound(sokoban.undo()?Sound::Undo:Sound::Error,now); sokoMoveAt = now; return; }
      int dx = 0, dy = 0;
      for (Key key : {UP, RIGHT, LEFT, DOWN}) {
        if (buttons[key].pressed || (buttons[key].repeated && now - sokoMoveAt >= 120)) {
          if (key == UP) dy = -1;
          if (key == DOWN) dy = 1;
          if (key == LEFT) dx = -1;
          if (key == RIGHT) dx = 1;
          break;
        }
      }
      if (dx || dy) {
        uint16_t pushes=sokoban.pushes;uint8_t targets=__builtin_popcountll(sokoban.boxes&sokoban.goals);
        bool moved=sokoban.move(dx, dy); sokoMoveAt = now;
        playSound(!moved?Sound::Error:__builtin_popcountll(sokoban.boxes&sokoban.goals)>targets?Sound::Target:
          sokoban.pushes!=pushes?Sound::Push:Sound::Move,now);
        if (sokoban.solved()) {
          if (sokoLevel + 1 > sokoSolved) {
            sokoSolved = sokoLevel + 1;
            if (storageReady) prefs.putUChar("sokoSolved", sokoSolved);
          }
          clearCheckpoint(); changeScreen(NEW_OVER);
        }
      }
    }
  } else if (screen == NEW_PAUSED) {
    if (buttons[B].pressed) { if (menuItem != SOKOBAN) saveArcadeBest(); changeScreen(HOME); }
    else if (buttons[A].pressed) { arcadeTick = sokoMoveAt = now; changeScreen(NEW_PLAYING); }
    else if (buttons[LEFT].pressed) { if (menuItem != SOKOBAN) saveArcadeBest(); startSelectedGame(now); }
  } else if (screen == NEW_OVER) {
    if (buttons[B].pressed) changeScreen(HOME);
    else if (buttons[A].pressed) {
      if (menuItem == SOKOBAN) {
        if (sokoLevel + 1 < SokobanGame::LEVELS) { ++sokoLevel; startSelectedGame(now); }
        else { sokoLevel = 0; changeScreen(NEW_READY); }
      } else startSelectedGame(now);
    }
  } else if (screen == BLOCKS_READY || screen == BLOCKS_OVER) {
    if(screen==BLOCKS_READY&&buttons[A].pressed){openBattleLobby(now);return;}
    if(battleMode){if(buttons[B].pressed)leaveBattle(now);else if(battle.phase==BlockBattle::ENDED&&(buttons[LEFT].pressed||buttons[A].pressed))openBattleLobby(now);return;}
    if (buttons[B].pressed) changeScreen(HOME);
    else if (buttons[LEFT].pressed) startBlocks(now);
  } else if (screen == BLOCKS_PAUSED) {
    if (buttons[B].pressed) {if(battleMode){battle.topOut(now);changeScreen(BLOCKS_OVER);return;} clearAnimating = false; saveBlocksBest(); changeScreen(HOME); }
    else if (buttons[A].pressed) {
      // Freeze both gravity and the current piece's lock delay while paused.
      if(battleMode){battle.paused=false;changeScreen(BLOCKS_PLAYING);return;}
      changeScreen(BLOCKS_PLAYING);
      lastFall += now - blocksPausedAt;
      groundSince += now - blocksPausedAt;
      clearAnimationAt += now - blocksPausedAt;
      clearNoticeAt += now - blocksPausedAt;
    }
  } else if (screen == BLOCKS_PLAYING) {
    if (buttons[A].pressed) { blocksPausedAt = now;if(battleMode){battle.paused=true;if(!battleFrozen){battleFrozen=true;battleFreezeAt=now;}}changeScreen(BLOCKS_PAUSED);if(!battleMode)saveCheckpoint(now);return; }
    if(battleMode&&battle.frozen(now))return;
    // Keep new-piece input out of the clear animation; A can still pause it.
    if (clearAnimating) return;
    // Physical Down -> portrait Left. Physical Up -> portrait Right.
    bool left = buttons[DOWN].pressed || buttons[DOWN].repeated;
    bool right = buttons[UP].pressed || buttons[UP].repeated;
    if (left != right && blocks.move(left ? -1 : 1)) {resetLockAfterMove(now);playSound(Sound::Move,now);}
    // Physical Left becomes portrait Up: start/retry and rotate pieces.
    if (buttons[LEFT].pressed && blocks.rotate()) {resetLockAfterMove(now);playSound(Sound::Rotate,now);}
    if (buttons[B].pressed) {
      blocks.hardDrop();
      playSound(Sound::Drop,now);
      afterBlocksLock(now);
    } else if (buttons[RIGHT].pressed) {
      if(blocks.fall(true))playSound(Sound::Move,now);
      lastFall = now;
    }
  }
}

void displayInk(uint16_t color = 0xEF7D) {
#ifdef MINICONSOLE_TFT114
  oled.setInk(color);
#else
  (void)color;
#endif
}

void textAt(int x, int y, const char *text, uint8_t size = 1) {
  oled.setTextSize(size);
  oled.setCursor(x, y);
  oled.print(text);
}

void centerText(int y, const char *text, uint8_t size = 1) {
  textAt((oled.width() - int(strlen(text)) * 6 * size) / 2, y, text, size);
}

void heading(const char *title) {
  displayInk(0x4E7F);
  textAt(0, 0, title);
  oled.drawFastHLine(0, 10, 128, WHITE);
  displayInk();
}

void drawRow(int y, const char *text, bool selected) {
  if (selected) oled.fillRoundRect(0, y - 1, 128, 9, 2, WHITE);
  oled.setTextColor(selected ? BLACK : WHITE);
  textAt(4, y, text);
  oled.setTextColor(WHITE);
}

void drawGame(uint32_t now) {
  char line[24];
  snprintf(line, sizeof(line), "S:%u", game.score());
  textAt(0, 1, line);
  snprintf(line, sizeof(line), "BEST:%u", best);
  textAt(72, 1, line);
  oled.drawRect(3, 14, 122, 50, WHITE);
  displayInk(0x5F26);
  for (int i = game.length - 1; i >= 0; --i) {
    int x = 4 + game.body[i].x * 4, y = 15 + game.body[i].y * 4;
    oled.fillRect(x, y, 4, 4, WHITE);
    if (i == 0) {
      int eyeX = x + (game.direction == SnakeGame::LEFT ? 0 : 2);
      int eyeY = y + (game.direction == SnakeGame::UP ? 0 : 2);
      oled.drawPixel(eyeX, eyeY, BLACK);
    }
  }
  int fx = 4 + game.food.x * 4, fy = 15 + game.food.y * 4;
  displayInk(0xFD45);
  if ((now / 250) % 2) oled.fillRect(fx + 1, fy + 1, 2, 2, WHITE);
  else oled.drawRect(fx, fy, 4, 4, WHITE);
  displayInk();
}

void drawBlockTexture(int x, int y, uint8_t type) {
  static constexpr uint16_t colors[] = {0x3EFF, 0xFFE5, 0xBAFF, 0x67E9, 0xF9C6, 0x6B7F, 0xFD45};
  displayInk(type<7?colors[type]:0x8410);
  // Seven distinct 4x4 monochrome patterns, sized for this tiny OLED.
  static constexpr uint16_t patterns[7] = {
    0xFFFF, // I: solid
    0xF99F, // O: hollow square
    0xA5A5, // T: checkerboard
    0xF0F0, // S: horizontal stripes
    0xAAAA, // Z: vertical stripes
    0x8C63, // J: diagonal hatching
    0xF66F  // L: inset bands
  };
  oled.fillRect(x, y, 4, 4, BLACK);
  uint16_t pattern = type<7?patterns[type]:0xA5A5;
  for (int row = 0; row < 4; ++row) for (int col = 0; col < 4; ++col) {
    if (pattern & (uint16_t(1) << (row * 4 + col))) oled.drawPixel(x + col, y + row, WHITE);
  }
  displayInk();
}

void drawBlockCell(int col, int row, uint8_t type, bool ghost = false) {
  if (col < 0 || col >= BlocksGame::COLS || row < 0 || row >= BlocksGame::ROWS) return;
  int x = 7 + col * 5, y = 17 + row * 5;
  if (ghost) oled.drawRect(x + 1, y + 1, 3, 3, WHITE);
  else drawBlockTexture(x, y, type);
}

void drawClearAnimation(uint32_t now) {
  uint32_t elapsed = (screen == BLOCKS_PAUSED ? blocksPausedAt : now) - clearAnimationAt;
  uint32_t duration = clearAnimationDuration();
  uint32_t flashTime = clearNoticeLines == 4 ? 300 : 180;
  uint32_t collapseAt = duration - 100;
  bool collapsing = elapsed >= collapseAt;
  uint32_t collapseProgress = collapsing ? elapsed - collapseAt : 0;
  if (collapseProgress > 100) collapseProgress = 100;
  for (int row = 0; row < BlocksGame::ROWS; ++row) {
    bool cleared = blocks.clearedRows & (uint32_t(1) << row);
    if (collapsing && cleared) continue;
    int fallingRows = 0;
    if (collapsing) for (int below = row + 1; below < BlocksGame::ROWS; ++below) {
      if (blocks.clearedRows & (uint32_t(1) << below)) ++fallingRows;
    }
    int pixelY = 17 + row * 5 + fallingRows * 5 * collapseProgress / 100;
    for (int col = 0; col < BlocksGame::COLS; ++col) {
      if (blocks.clearBoard[row][col]) drawBlockTexture(7 + col * 5, pixelY, blocks.clearBoard[row][col] - 1);
    }
    if (!cleared) continue;
    if (elapsed < flashTime) {
      oled.fillRect(7, 17 + row * 5, 50, 4, (elapsed / 60) % 2 ? BLACK : WHITE);
    } else if (!collapsing) {
      // Wipe outwards from the center, leaving other rows stationary.
      int halfWidth = 25 * (elapsed - flashTime) / (collapseAt - flashTime);
      oled.fillRect(32 - halfWidth, 17 + row * 5, halfWidth * 2, 4, BLACK);
    }
  }
}

void drawBlocks(uint32_t now) {
  char line[24];
  if ((now / 3000) % 2) snprintf(line, sizeof(line), "LV:%u", blocks.level() > 99 ? 99 : blocks.level());
  else snprintf(line, sizeof(line), "L:%u", blocks.lines > 999 ? 999 : blocks.lines);
  textAt(1, 0, line);
  textAt(36, 0, "N");
  uint8_t preview = clearAnimating ? blocks.piece : blocks.next;
  for (int row = 0; row < 4; ++row) for (int col = 0; col < 4; ++col) {
    if (BlocksGame::cell(preview, 0, col, row)) drawBlockTexture(47 + col * 4, row * 4, preview);
  }
  // Six displayed score digits fit the portrait header; full score is saved.
  snprintf(line, sizeof(line), "S:%lu", (unsigned long)(blocks.score > 999999 ? 999999 : blocks.score));
  textAt(1, 8, line);
  oled.drawRect(6, 16, 52, 102, WHITE);
  if (clearAnimating) drawClearAnimation(now);
  else for (int row = 0; row < BlocksGame::ROWS; ++row) for (int col = 0; col < BlocksGame::COLS; ++col) {
    if (blocks.board[row][col]) drawBlockCell(col, row, blocks.board[row][col] - 1);
  }
  if (blocks.alive && !clearAnimating) {
    int ghostY = blocks.ghostY();
    for (int row = 0; row < 4; ++row) for (int col = 0; col < 4; ++col) {
      if (BlocksGame::cell(blocks.piece, blocks.rotation, col, row)) {
        if (ghostY != blocks.y) drawBlockCell(blocks.x + col, ghostY + row, blocks.piece, true);
        drawBlockCell(blocks.x + col, blocks.y + row, blocks.piece);
      }
    }
  }
  if (screen == BLOCKS_PLAYING && clearNoticeLines && now - clearNoticeAt < 1200) {
    const char *names[] = {"", "SINGLE", "DOUBLE", "TRIPLE", "TETRIS!"};
    if (clearAnimating || now - clearNoticeAt < 600) centerText(120, names[clearNoticeLines]);
    else {
      snprintf(line, sizeof(line), "+%lu", (unsigned long)clearNoticePoints);
      centerText(120, line);
    }
  } else centerText(120, "UP:ROT B:D");
}

void drawBlocksScreen(uint32_t now) {
  if (screen == BLOCKS_READY) {
    centerText(0, "TETRIS");
    oled.drawFastHLine(0, 11, 64, WHITE);
    centerText(18, "TURN RIGHT");
    centerText(28, "90 DEGREES");
    centerText(42, "UP: ROTATE");
    centerText(52, "B: DROP");
    centerText(62, "D-PAD:");
    centerText(72, "L/R: MOVE");
    centerText(82, "DOWN: FAST");
    centerText(92, "A:BATTLE");
    centerText(110, "UP: START");
    centerText(120, "B: HOME");
    return;
  }
  drawBlocks(now);
  if (screen == BLOCKS_PAUSED || screen == BLOCKS_OVER) {
    oled.fillRoundRect(1, 37, 62, 66, 3, BLACK);
    oled.drawRoundRect(1, 37, 62, 66, 3, WHITE);
    centerText(42, screen == BLOCKS_PAUSED ? "PAUSED" : "GAME OVER");
    char line[16];
    snprintf(line, sizeof(line), "S:%lu", (unsigned long)(blocks.score > 999999 ? 999999 : blocks.score));
    centerText(54, line);
    if (screen == BLOCKS_PAUSED) centerText(66, pauseSaved ? "SAVED" : "SAVE FAIL");
    else if (newBest && screen == BLOCKS_OVER) centerText(66, "NEW BEST");
    else {
      snprintf(line, sizeof(line), "B:%lu", (unsigned long)(blocksBest > 999999 ? 999999 : blocksBest));
      centerText(66, line);
    }
    centerText(80, screen == BLOCKS_PAUSED ? "A:GO" : "UP:RETRY");
    centerText(92, "B:HOME");
  }
}

void drawAlien(int x, int y, int type, bool alternate, int scale = 1) {
  static constexpr uint16_t colors[] = {0xBAFF, 0x3EFF, 0x67E9};
  displayInk(colors[type]);
  static constexpr uint8_t sprites[3][2][5] = {
    {{0x18,0x3C,0x7E,0xDB,0x24},{0x18,0x3C,0x7E,0xDB,0x42}},
    {{0x24,0x7E,0xDB,0xFF,0xA5},{0x24,0x7E,0xDB,0xFF,0x42}},
    {{0x3C,0x7E,0xDB,0x7E,0x42},{0x3C,0x7E,0xDB,0x7E,0xA5}}
  };
  for (int r = 0; r < 5; ++r) for (int c = 0; c < 8; ++c) {
    if (sprites[type][alternate][r] & (0x80 >> c)) oled.fillRect(x + c * scale, y + r * scale, scale, scale, WHITE);
  }
  displayInk();
}

void drawCartridgeIcon(uint8_t id, int x, int y, uint32_t now) {
  static constexpr uint16_t colors[] = {0x5F26, 0xBAFF, 0xFD45, 0x67E9, 0xFFE5, 0xF9C6, 0x3EFF, 0xFD45};
  static_assert(sizeof(colors) / sizeof(colors[0]) == GAME_COUNT, "Each cartridge needs an icon color");
  displayInk(colors[id]);
  if (id == SNAKE) {
    const int cx[] = {0,0,0,1,2,2,3,4}, cy[] = {3,2,1,1,1,2,2,2};
    for (int i = 0; i < 8; ++i) oled.fillRect(x + cx[i] * 5, y + cy[i] * 5, 5, 5, WHITE);
    oled.drawPixel(x + 23, y + 11, BLACK);
    oled.drawRect(x + 21, y + 1, 4, 4, WHITE);
  } else if (id == BLOCKS) {
    for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c) {
      if (BlocksGame::cell(BlocksGame::T, 0, c, r)) drawBlockTexture(x + c * 5 + 3, y + r * 5 + 2, BlocksGame::T);
    }
    for (int c = 0; c < 5; ++c) drawBlockTexture(x + c * 5, y + 21, c);
    drawBlockTexture(x + 15, y + 16, BlocksGame::O);
    drawBlockTexture(x + 20, y + 16, BlocksGame::O);
  } else if (id == BREAKOUT) {
    for (int r = 0; r < 3; ++r) for (int c = 0; c < 3; ++c) oled.drawRect(x + c * 9, y + r * 5, 7, 4, WHITE);
    oled.fillRect(x + 7, y + 24, 14, 2, WHITE);
    oled.fillRect(x + 15, y + 18, 2, 2, WHITE);
  } else if (id == INVADERS) {
    drawAlien(x + 4, y + 3, 1, (now / 500) % 2, 2);
    oled.fillRect(x + 12, y + 18, 2, 4, WHITE);
    oled.fillTriangle(x + 8, y + 26, x + 18, y + 26, x + 13, y + 22, WHITE);
  } else if (id == SOKOBAN) {
    oled.drawRect(x + 2, y + 3, 13, 13, WHITE);
    oled.drawLine(x + 3, y + 4, x + 13, y + 14, WHITE);
    oled.drawLine(x + 13, y + 4, x + 3, y + 14, WHITE);
    oled.drawRect(x + 19, y + 7, 5, 5, WHITE);
    oled.fillCircle(x + 18, y + 21, 3, WHITE);
    oled.drawPixel(x + 19, y + 20, BLACK);
  } else if(id==MOTORWAYS) {
    oled.drawLine(x+3,y+3,x+3,y+22,WHITE);oled.drawLine(x+3,y+22,x+23,y+22,WHITE);
    oled.drawLine(x+23,y+22,x+23,y+5,WHITE);oled.fillCircle(x+3,y+3,3,WHITE);oled.drawRect(x+20,y+1,7,7,WHITE);
  } else {
    // Helmet, eyes and muzzle of the original Doom marine.
    oled.drawRoundRect(x + 3, y + 2, 22, 23, 4, WHITE);
    oled.fillRect(x + 6, y + 4, 16, 6, WHITE);
    oled.drawFastHLine(x + 5, y + 11, 18, WHITE);
    oled.fillRect(x + 7, y + 13, 4, 2, WHITE); oled.fillRect(x + 17, y + 13, 4, 2, WHITE);
    oled.drawRect(x + 9, y + 18, 10, 5, WHITE);
  }
  displayInk();
}

uint8_t libraryPages() { return GAME_COUNT + (goingOut ? PARTY_COUNT + 1 : 0); }

void drawPageNumber(uint8_t page) {
  char label[8]; snprintf(label, sizeof(label), "%u/%u", page + 1, libraryPages());
  textAt(128 - int(strlen(label)) * 6, 0, label);
}

void drawPageDots(uint8_t page) {
  int pages = libraryPages(), startX = (128 - pages * 9) / 2 + 1;
  for (int i = 0; i < pages; ++i) {
    if (page == i) oled.fillRect(startX + i * 9, 51, 5, 3, WHITE);
    else oled.drawPixel(startX + 2 + i * 9, 52, WHITE);
  }
}

void drawPartyIcon(uint8_t id, int x, int y, uint32_t now) {
  displayInk(id == REACTION ? 0xFFE5 : id == HIGH_LOW ? 0x6B7F : 0xFD45);
  if (id == REACTION) {
    oled.drawCircle(x + 6, y + 19, 6, WHITE); oled.drawCircle(x + 23, y + 19, 6, WHITE);
    textAt(x + 4, y + 16, "A"); textAt(x + 21, y + 16, "B");
    oled.fillTriangle(x + 17, y, x + 9, y + 11, x + 16, y + 11, WHITE);
    oled.fillTriangle(x + 12, y + 19, x + 21, y + 7, x + 14, y + 7, WHITE);
  } else if (id == HIGH_LOW) {
    oled.drawRoundRect(x, y + 3, 17, 23, 2, WHITE);
    oled.fillRoundRect(x + 11, y, 17, 23, 2, BLACK); oled.drawRoundRect(x + 11, y, 17, 23, 2, WHITE);
    textAt(x + 3, y + 7, "K"); textAt(x + 15, y + 4, "A");
    oled.fillTriangle(x + 15, y + 17, x + 23, y + 17, x + 19, y + 12, WHITE);
  } else {
    oled.fillCircle(x + 13, y + 16, 10, WHITE);
    oled.fillCircle(x + 10, y + 13, 2, BLACK);
    oled.drawLine(x + 17, y + 6, x + 21, y + 2, WHITE);
    oled.drawLine(x + 21, y + 2, x + 25, y + 4, WHITE);
    int pulse = int(now / 150) & 1;
    oled.drawLine(x + 25, y + 1, x + 25 + pulse * 2, y + 7, WHITE);
    oled.drawFastHLine(x + 22, y + 4, 7, WHITE);
  }
  displayInk();
}

void drawPartyLibrary(uint32_t now) {
  heading("GOING OUT"); drawPageNumber(partyItem + 1);
  oled.drawRoundRect(10, 13, 108, 36, 3, WHITE);
  if (now - selectionAt < 130) oled.drawRoundRect(12, 15, 104, 32, 2, WHITE);
  drawPartyIcon(partyItem, 16, 18, now);
  textAt(49, 18, PARTY_NAMES[partyItem]);
  textAt(49, 28, partyItem == REACTION ? "DUEL / 2P" : partyItem == HIGH_LOW ? "CARD GAME" : "PASS IT ON");
  textAt(49, 39, partyItem == REACTION ? "FAST WINS" : partyItem == HIGH_LOW ? "ACE HIGH" : "RANDOM FUSE");
  oled.fillTriangle(2, 30, 6, 26, 6, 34, WHITE); oled.fillTriangle(125, 30, 121, 26, 121, 34, WHITE);
  drawPageDots(partyItem + 1); centerText(57, "A:PLAY  B:SYSTEM");
}

void drawPartyCard(int x, uint8_t card, bool front, int width = 32) {
  int left = x + (32 - width) / 2;
  oled.drawRoundRect(left, 14, width, 30, width < 5 ? 0 : 2, WHITE);
  if (width < 28) return;
  if (!front) {
    for (int y = 18; y < 41; y += 4) for (int col = 4; col < 28; col += 4) oled.drawPixel(x + col, y, WHITE);
    textAt(x + 10, 23, "?", 2); return;
  }
  uint8_t rank = PartyGames::HighLow::rank(card);
  char label[3];
  if (rank >= 11) { label[0] = "JQKA"[rank - 11]; label[1] = 0; }
  else snprintf(label, sizeof(label), "%u", rank);
  textAt(x + 3, 17, label, 2);
  int sx = x + 22, sy = 36;
  switch (PartyGames::HighLow::suit(card)) {
    case 0: // Heart.
      oled.fillCircle(sx - 2, sy - 1, 2, WHITE); oled.fillCircle(sx + 2, sy - 1, 2, WHITE);
      oled.fillTriangle(sx - 4, sy, sx + 4, sy, sx, sy + 4, WHITE); break;
    case 1: // Diamond.
      oled.fillTriangle(sx, sy - 4, sx - 3, sy, sx + 3, sy, WHITE);
      oled.fillTriangle(sx, sy + 4, sx - 3, sy, sx + 3, sy, WHITE); break;
    case 2: // Club.
      oled.fillCircle(sx, sy - 2, 2, WHITE); oled.fillCircle(sx - 2, sy + 1, 2, WHITE); oled.fillCircle(sx + 2, sy + 1, 2, WHITE);
      oled.drawFastVLine(sx, sy + 1, 4, WHITE); break;
    default: // Spade.
      oled.fillTriangle(sx, sy - 4, sx - 4, sy + 1, sx + 4, sy + 1, WHITE);
      oled.drawFastVLine(sx, sy + 1, 4, WHITE); break;
  }
}

void drawPartyGame(uint32_t now) {
  char line[32];
  if (partyItem == REACTION) {
    heading("REACTION DUEL");
    if (reaction.phase == PartyGames::ReactionDuel::READY) {
      textAt(4, 16, "A vs B: two players"); textAt(4, 27, "WAIT for GO to press"); textAt(4, 38, "Early press loses.");
      centerText(57, "A:READY LEFT:BACK"); return;
    }
    if (reaction.phase == PartyGames::ReactionDuel::WAIT) centerText(20, "WAIT...", 2);
    else if (reaction.phase == PartyGames::ReactionDuel::GO) {
      oled.fillRect(0, 13, 128, 29, WHITE); oled.setTextColor(BLACK); centerText(17, "GO!", 3); oled.setTextColor(WHITE);
    } else {
      centerText(16, reaction.winner == PartyGames::ReactionDuel::PLAYER_A ? "A WINS" : reaction.winner == PartyGames::ReactionDuel::PLAYER_B ? "B WINS" : reaction.winner == PartyGames::ReactionDuel::TIE ? "TIE" : "NO PRESS", 2);
      if (reaction.falseStart) snprintf(line, sizeof(line), "TOO EARLY");
      else if (reaction.winner == PartyGames::ReactionDuel::NONE) snprintf(line, sizeof(line), "TRY AGAIN");
      else snprintf(line, sizeof(line), "%lu ms", (unsigned long)reaction.reactionMs);
      centerText(34, line);
    }
    snprintf(line, sizeof(line), "A:%02u       B:%02u", reaction.scoreA, reaction.scoreB); centerText(46, line);
    centerText(57, reaction.phase == PartyGames::ReactionDuel::RESULT ? "UP:NEXT LEFT:BACK" : "A vs B  LEFT:BACK");
  } else if (partyItem == HIGH_LOW) {
    heading("HIGH OR LOW");
    if (highLow.phase == PartyGames::HighLow::READY) {
      textAt(4, 16, "A:HIGHER B:LOWER"); textAt(4, 27, "ACE HIGH. TIES PUSH."); textAt(4, 38, "Build your streak.");
      centerText(57, "A:DEAL LEFT:BACK"); return;
    }
    snprintf(line, sizeof(line), "S:%u", highLow.streak); textAt(98, 0, line);
    bool front = highLow.phase == PartyGames::HighLow::RESULT;
    int width = 32;
    if (highLow.phase == PartyGames::HighLow::FLIP) {
      uint32_t age = now - highLow.flippedAt; if (age > 280) age = 280;
      front = age >= 140; width = 2 + int(age < 140 ? 140 - age : age - 140) * 30 / 140;
    }
    drawPartyCard(20, highLow.current, true); drawPartyCard(76, highLow.revealed, front, width);
    if (highLow.phase == PartyGames::HighLow::RESULT) {
      if (highLow.outcome == PartyGames::HighLow::PUSH) snprintf(line, sizeof(line), "SAME RANK - PUSH");
      else if (highLow.outcome == PartyGames::HighLow::WRONG) snprintf(line, sizeof(line), "MISS - STREAK RESET");
      else snprintf(line, sizeof(line), "RIGHT! STREAK %u", highLow.streak);
    } else if (highLow.phase == PartyGames::HighLow::FLIP) snprintf(line, sizeof(line), "TURNING...");
    else snprintf(line, sizeof(line), "%u LEFT  BEST %u", 52 - highLow.used, highLow.best);
    centerText(46, line);
    centerText(57, highLow.phase == PartyGames::HighLow::RESULT ? (highLow.used == 52 ? "A:DEAL LEFT:BACK" : "A:NEXT LEFT:BACK") : "A:HI B:LO LEFT:BACK");
  } else {
    heading("PASS THE BOMB");
    if (passBomb.phase == PartyGames::PassBomb::READY) {
      textAt(4, 16, "Pass round the room"); textAt(4, 27, "Tap A on each pass."); textAt(4, 38, "Holder at BOOM loses");
      centerText(57, "A:START LEFT:BACK"); return;
    }
    uint32_t elapsed = now - passBomb.startedAt;
    if (passBomb.phase == PartyGames::PassBomb::EXPLODED) {
      for (int i = 0; i < 8; ++i) {
        int x = 7 + i * 16, y = 17 + ((i + int(now / 120)) & 1) * 18;
        oled.fillTriangle(x, y, x + 5, y - 4, x + 9, y + 4, WHITE);
      }
      oled.fillRect(15, 21, 98, 23, BLACK); centerText(21, "BOOM!", 3);
      centerText(46, "HOLDER LOSES"); centerText(57, "A:AGAIN LEFT:BACK");
    } else {
      if (elapsed >= passBomb.fuseMs) elapsed = passBomb.fuseMs - 1;
      uint32_t beat = 450 - elapsed * 380 / passBomb.fuseMs;
      int shake = (now / beat) & 1;
      oled.fillCircle(62 + shake, 31, 13, WHITE); oled.fillCircle(58 + shake, 27, 3, BLACK);
      oled.drawLine(70 + shake, 21, 79, 15, WHITE);
      int tip = 80 + int((passBomb.fuseMs - elapsed) * 27 / passBomb.fuseMs);
      oled.drawFastHLine(79, 15, tip - 78, WHITE);
      oled.drawLine(tip - 2, 12, tip + 2, 18, WHITE); oled.drawLine(tip - 2, 18, tip + 2, 12, WHITE);
      snprintf(line, sizeof(line), "PASSES %u", passBomb.passes); centerText(46, line);
      centerText(57, "A:PASS LEFT:BACK");
    }
  }
}

void drawLibrary(uint32_t now) {
  heading("POCKET ARCADE");
  char line[18];
  drawPageNumber(menuItem + (goingOut ? PARTY_COUNT + 1 : 0));
  // A brief inset highlight makes changing cartridges feel responsive.
  oled.drawRoundRect(10, 13, 108, 36, 3, WHITE);
  if (now - selectionAt < 130) oled.drawRoundRect(12, 15, 104, 32, 2, WHITE);
  drawCartridgeIcon(menuItem, 16, 18, now);
  textAt(49, 18, menuItem==MOTORWAYS?"MOTORWAYS":GAME_NAMES[menuItem]);
  bool hasSave = menuItem == DOOM ? doomHasSave() : checkpointAvailable && checkpoint.gameId == menuItem;
  textAt(49, 28, hasSave ? "SAVED GAME" : GAME_GENRES[menuItem]);
  if (menuItem == DOOM) snprintf(line, sizeof(line), "9 LEVELS");
  else if (menuItem == SOKOBAN) snprintf(line, sizeof(line), "%u/10 CLEAR", sokoSolved);
  else {
    uint32_t record = menuItem == SNAKE ? best : menuItem == BLOCKS ? blocksBest : menuItem == BREAKOUT ? breakoutBest : menuItem == MOTORWAYS ? motorBest : invadersBest;
    snprintf(line, sizeof(line), "HI:%lu", (unsigned long)(record > 999999 ? 999999 : record));
  }
  textAt(49, 39, line);
  oled.fillTriangle(2, 30, 6, 26, 6, 34, WHITE);
  oled.fillTriangle(125, 30, 121, 26, 121, 34, WHITE);
  drawPageDots(menuItem + (goingOut ? PARTY_COUNT + 1 : 0));
  centerText(57, hasSave ? "A:CONTINUE B:SYSTEM" : "A:PLAY  B:SYSTEM");
}

void drawDrinkCounter(uint32_t now) {
  uint32_t elapsed = now - pourStartedAt;
  if (pouring && elapsed >= POUR_MS) pouring = false;
  if (pouring) {
    displayInk(0x2CDF);
    // A stream enters at the left, then rippling liquid floods across the OLED
    // and drains away. No delays: taps and reset still work during the pour.
    int height = elapsed < 650 ? int(elapsed * 45 / 650) : int((POUR_MS - elapsed) * 45 / 450);
    for (int x = 0; x < 128; ++x) {
      int phase = (x + int(elapsed / 18)) % 24;
      int wave = (phase < 12 ? phase : 24 - phase) / 3 - 2;
      int top = 64 - height + wave;
      if (top < 64) oled.drawFastVLine(x, top, 64 - top, WHITE);
    }
    if (elapsed < 650) {
      oled.fillTriangle(1, 12, 7, 12, 10, 27, WHITE);
      oled.fillRect(8, 25, 3, 39, WHITE);
      for (int i = 0; i < 4; ++i) {
        int x = 31 + (i * 27 + int(elapsed / 8)) % 92;
        int y = 60 - (i * 7 + int(elapsed / 35)) % (height > 4 ? height - 2 : 3);
        oled.drawCircle(x, y, 1, BLACK);
      }
    }
  }
  displayInk();
  uint32_t celebrationAge = now - confettiStartedAt;
  if (confetti && celebrationAge >= CONFETTI_MS) confetti = false;
  if (confetti) {
    // Deterministic particles leave the games' random state alone. Rendering and
    // input keep running, and a held button never replays the celebration.
    for (int i = 0; i < 30; ++i) {
      static constexpr uint16_t colors[] = {0xF9C6, 0xFFE5, 0x67E9, 0x3EFF, 0xBAFF, 0xFD45};
      displayInk(colors[i % 6]);
      int phase = int((celebrationAge + uint32_t(i) * 83) % 900);
      int sway = (phase / 60) % 8;
      if (sway > 4) sway = 8 - sway;
      int x = (i * 37 + 19) % 128 + sway - 2;
      int y = phase * 88 / 900 - 12;
      if (i % 3 == 0) oled.drawRect(x, y, 3, 3, WHITE);
      else if (i % 3 == 1) oled.drawLine(x, y, x + 2, y + 2, WHITE);
      else { oled.drawFastHLine(x, y + 1, 3, WHITE); oled.drawPixel(x + 1, y, WHITE); }
    }
  }
  displayInk();
  oled.fillRect(0, 0, 128, 11, BLACK);
  heading(counterSaveFailed ? "SAVE FAILED" : confetti ? "GOAL REACHED!" : "GOING OUT");
  drawPageNumber(0);
  // Glass and a dark number panel keep the count readable through the wave.
  oled.fillRoundRect(12, 17, 38, 34, 3, BLACK);
  oled.drawLine(19, 20, 22, 46, WHITE); oled.drawLine(43, 20, 40, 46, WHITE);
  oled.drawFastHLine(22, 46, 19, WHITE); oled.drawFastHLine(19, 20, 25, WHITE);
  displayInk(0xFD45);
  for (int y = 35; y < 44; ++y) oled.drawFastHLine(23, y, 17, WHITE);
  oled.drawPixel(28, 38, BLACK); oled.drawPixel(36, 41, BLACK);
  displayInk();
  char count[12]; snprintf(count, sizeof(count), "%lu", (unsigned long)drinkCount);
  uint8_t size = drinkCount < 1000 ? 3 : 2;
  int countX = 86 - int(strlen(count)) * 3 * size;
  oled.fillRoundRect(countX - 4, 16, int(strlen(count)) * 6 * size + 8, 26, 3, BLACK);
  oled.fillRect(65, 43, 42, 9, BLACK);
  textAt(countX, 18, count, size);
  if (drinkGoal) {
    char goal[12]; snprintf(goal, sizeof(goal), "GOAL %u", drinkGoal);
    textAt(65, 44, goal);
  } else textAt(68, 44, "DRINKS");
  oled.fillRect(0, 55, 128, 9, BLACK);
  centerText(57, "A:+1 HOLD B:RESET");
  oled.fillTriangle(2, 32, 6, 28, 6, 36, WHITE);
  oled.fillTriangle(125, 32, 121, 28, 121, 36, WHITE);
  // Hold feedback only appears inside the hidden mode.
  if (menuAPending || counterBPending) {
    Key key = counterBPending ? B : A;
    uint32_t held = now - buttons[key].heldAt;
    int progress = int((held > OUT_HOLD_MS ? OUT_HOLD_MS : held) * 128 / OUT_HOLD_MS);
    oled.drawFastHLine(0, 54, progress, WHITE);
  }
}

void drawContinuePrompt(uint32_t now) {
  heading("CONTINUE?");
  drawCartridgeIcon(menuItem, 8, 18, now);
  textAt(44, 17, menuItem==MOTORWAYS?"MOTORWAYS":GAME_NAMES[menuItem]);
  textAt(44, 29, menuItem == BLOCKS ? "TURN RIGHT 90" : "SAVED GAME");
  textAt(44, 41, "LEFT:NEW");
  oled.drawFastHLine(0, 51, 128, WHITE);
  centerText(57, "A:RESUME B:LIBRARY");
}

void drawReady() {
  heading(GAME_NAMES[menuItem]);
  char line[24];
  if (menuItem == SNAKE) {
    textAt(4, 16, "D-PAD: STEER");
    textAt(4, 27, "EAT DOTS. STAY ALIVE");
    textAt(4, 38, "A/B: PAUSE");
  } else if (menuItem == BREAKOUT) {
    textAt(4, 16, "L/R: MOVE PADDLE");
    textAt(4, 27, "A: SERVE  B: PAUSE");
    textAt(4, 38, "3 LIVES. CLEAR RACKS");
  } else if (menuItem == INVADERS) {
    textAt(4, 16, "L/R: MOVE SHIP");
    textAt(4, 27, "HOLD A: FIRE");
    textAt(4, 38, "B: PAUSE  3 LIVES");
  } else if (menuItem == MOTORWAYS) {
    textAt(4,16,"A:BEGIN/CONFIRM ROAD");textAt(4,27,"D-PAD: DRAW / MOVE");textAt(4,38,"B:CANCEL / TOOLS");
  } else if (menuItem == DOOM) {
    textAt(4, 16, "U/D:WALK  L/R:TURN");
    textAt(4, 27, "A:FIRE  B:USE");
    textAt(4, 38, doomAssetsMissing ? "DOOM DATA MISSING" : "A+B:MENU HOLD:HOME");
  } else {
    snprintf(line, sizeof(line), "LEVEL %02u  L/R:CHOOSE", sokoLevel + 1);
    textAt(4, 16, line);
    textAt(4, 27, "PUSH BOXES ON DOTS");
    textAt(4, 38, "A:UNDO  B:PAUSE");
  }
  oled.drawFastHLine(8, 50, 112, WHITE);
  centerText(56, "A:START  B:BACK");
}

void drawArcadeHUD(uint32_t score, uint16_t stage, uint8_t lives, const char *stageName) {
  char line[20];
  snprintf(line, sizeof(line), "%lu", (unsigned long)(score > 999999 ? 999999 : score));
  textAt(1, 1, line);
  snprintf(line, sizeof(line), "%s%u", stageName, stage > 99 ? 99 : stage);
  textAt(58, 1, line);
  for (int i = 0; i < lives; ++i) {
    int x = 104 + i * 8;
    oled.fillRect(x, 2, 2, 2, WHITE); oled.fillRect(x + 3, 2, 2, 2, WHITE);
    oled.fillRect(x + 1, 4, 3, 2, WHITE); oled.drawPixel(x + 2, 6, WHITE);
  }
  oled.drawFastHLine(0, 10, 128, WHITE);
}

void drawBreakout() {
  drawArcadeHUD(breakout.score, breakout.level, breakout.lives, "L");
  oled.drawFastVLine(0, 11, 53, WHITE); oled.drawFastVLine(127, 11, 53, WHITE);
  for (int r = 0; r < BreakoutGame::ROWS; ++r) for (int c = 0; c < BreakoutGame::COLS; ++c) {
    static constexpr uint16_t colors[] = {0xF9C6, 0xFD45, 0xFFE5, 0x67E9};
    displayInk(colors[r % 4]);
    int x = BreakoutGame::brickX(c), y = BreakoutGame::brickY(r);
    if (breakout.bricks[r][c]) {
      if (r % 2) oled.drawRect(x, y, 18, 4, WHITE);
      else oled.fillRect(x, y, 18, 4, WHITE);
    } else if (breakout.flash && r == breakout.hitRow && c == breakout.hitCol) oled.drawRect(x, y - 1, 18, 6, WHITE);
  }
  displayInk();
  oled.fillRoundRect(int(breakout.paddle) - 11, 59, 22, 3, 1, WHITE);
  oled.fillRect(int(breakout.x), int(breakout.y), 2, 2, WHITE);
  if (breakout.serving && screen == NEW_PLAYING) centerText(44, "A:SERVE");
}

void drawInvaders(uint32_t now) {
  drawArcadeHUD(invaders.score, invaders.wave, invaders.lives, "W");
  if (!invaders.waveWait) for (int r = 0; r < InvadersGame::ROWS; ++r) for (int c = 0; c < InvadersGame::COLS; ++c) {
    if (invaders.aliens[r][c]) drawAlien(invaders.alienX(c), invaders.alienY(r), r, (now / 300) % 2);
  }
  for (int s = 0; s < 3; ++s) for (int r = 0; r < 4; ++r) for (int c = 0; c < 8; ++c) {
    if (invaders.shields[s][r] & (1u << c)) oled.drawPixel(InvadersGame::shieldX(s) + c, 48 + r, WHITE);
  }
  if (!invaders.invincible || (invaders.invincible / 6) % 2) {
    int x = int(invaders.player);
    oled.fillRect(x - 4, 59, 9, 3, WHITE); oled.fillRect(x - 2, 57, 5, 2, WHITE); oled.drawPixel(x, 56, WHITE);
  }
  for (const auto &s : invaders.shots) if (s.active) oled.drawFastVLine(int(s.x), int(s.y), 3, WHITE);
  for (const auto &b : invaders.bombs) if (b.active) {
    oled.drawPixel(int(b.x), int(b.y), WHITE); oled.drawPixel(int(b.x) + 1, int(b.y) + 1, WHITE); oled.drawPixel(int(b.x), int(b.y) + 2, WHITE);
  }
  if (invaders.explosion) {
    int x = invaders.explosionX + 3, y = invaders.explosionY + 2;
    oled.drawLine(x - 4, y - 3, x + 4, y + 3, WHITE); oled.drawLine(x - 4, y + 3, x + 4, y - 3, WHITE);
  }
  if (invaders.waveWait) {
    oled.fillRect(20, 29, 88, 15, BLACK); char line[20];
    snprintf(line, sizeof(line), "WAVE %u", invaders.wave); centerText(33, line);
  }
}

void drawSokoban() {
  char line[24];
  snprintf(line, sizeof(line), "L%02u", sokoLevel + 1); textAt(0, 1, line);
  snprintf(line, sizeof(line), "M:%u P:%u", sokoban.moves > 9999 ? 9999 : sokoban.moves, sokoban.pushes > 9999 ? 9999 : sokoban.pushes); textAt(29, 1, line);
  oled.drawFastHLine(0, 10, 128, WHITE);
  for (int cell = 0; cell < 48; ++cell) {
    int x = 32 + (cell % 8) * 8, y = 14 + (cell / 8) * 8;
    uint64_t bit = SokobanGame::cellMask(cell);
    if (sokoban.walls & bit) {
      displayInk(0x7C10);
      oled.drawRect(x, y, 8, 8, WHITE); oled.drawFastHLine(x, y + 3, 8, WHITE);
      oled.drawFastVLine(x + 3, y, 3, WHITE); oled.drawFastVLine(x + 5, y + 4, 3, WHITE);
    } else if (sokoban.boxes & bit) {
      displayInk((sokoban.goals & bit) ? 0x67E9 : 0xFD45);
      if (sokoban.goals & bit) {
        oled.fillRect(x + 1, y + 1, 6, 6, WHITE);
        oled.drawPixel(x + 2, y + 3, BLACK); oled.drawPixel(x + 3, y + 4, BLACK); oled.drawLine(x + 3, y + 4, x + 5, y + 2, BLACK);
      } else {
        oled.drawRect(x + 1, y + 1, 6, 6, WHITE); oled.drawLine(x + 2, y + 2, x + 5, y + 5, WHITE); oled.drawLine(x + 5, y + 2, x + 2, y + 5, WHITE);
      }
    } else if (sokoban.goals & bit) { displayInk(0x3EFF); oled.drawRect(x + 3, y + 3, 2, 2, WHITE); }
    if (cell == sokoban.player) {
      displayInk(0x4E7F);
      oled.fillCircle(x + 4, y + 3, 2, WHITE); oled.fillRect(x + 2, y + 5, 5, 2, WHITE); oled.drawPixel(x + 5, y + 2, BLACK);
    }
  }
  displayInk();
  textAt(0, 24, "A"); textAt(0, 34, "UNDO"); textAt(104, 24, "B"); textAt(98, 34, "PAUSE");
}

void drawNewGameScreen(uint32_t now) {
  if (screen == NEW_READY) { drawReady(); return; }
  if (menuItem == BREAKOUT) drawBreakout();
  if (menuItem == INVADERS) drawInvaders(now);
  if (menuItem == SOKOBAN) drawSokoban();
  if (screen != NEW_PAUSED && screen != NEW_OVER) return;
  oled.fillRoundRect(10, 15, 108, 48, 3, BLACK);
  oled.drawRoundRect(10, 15, 108, 48, 3, WHITE);
  bool paused = screen == NEW_PAUSED;
  const char *title = paused ? (pauseSaved ? "SAVED" : "SAVE FAILED") : menuItem == SOKOBAN ? (sokoLevel == 9 ? "ALL CLEAR!" : "LEVEL CLEAR!") : "GAME OVER";
  centerText(19, title);
  char line[24];
  if (menuItem == SOKOBAN) snprintf(line, sizeof(line), "M:%u  P:%u", sokoban.moves > 9999 ? 9999 : sokoban.moves, sokoban.pushes > 9999 ? 9999 : sokoban.pushes);
  else {
    uint32_t score = menuItem == BREAKOUT ? breakout.score : menuItem == MOTORWAYS ? motorways.score : invaders.score;
    snprintf(line, sizeof(line), "%lu%s", (unsigned long)(score > 999999 ? 999999 : score), newBest ? " NEW BEST" : " POINTS");
  }
  centerText(31, line);
  centerText(43, paused ? "A:GO  B:HOME" : menuItem == SOKOBAN ? (sokoLevel == 9 ? "A:LEVELS B:HOME" : "A:NEXT B:HOME") : "A:RETRY B:HOME");
  if (paused) centerText(54, "LEFT:RESTART");
}

#ifdef MINICONSOLE_TFT114
#include "TFTNativeUI.h"
#include "TFTGames.h"
#endif
#include "MotorwaysUI.h"
#include "BattleUI.h"
#include "PartnersUI.h"

void render(uint32_t now) {
#ifdef MINICONSOLE_TFT114
  oled.setBackground(screen==NEW_PLAYING&&menuItem==INVADERS?0:ConsoleTFT114::BACKGROUND);
  if (PartnersUI::render(now) || BattleUI::render(now) || MotorwaysUI::render(now) || TFTGames::render(now) || TFTUI::render(now)) {
    uint32_t transferStart=micros();oled.presentPhysical();
    transferMicros+=micros()-transferStart;++frameCount;return;
  }
#endif
  displayInk();
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setTextSize(1);
  if(screen==PARTNERS_ROOM){
#ifndef MINICONSOLE_TFT114
    PartnersUI::draw(now);
#endif
  }else if(battleMode&&(screen==BATTLE_LOBBY||screen==BLOCKS_PAUSED||screen==BLOCKS_OVER)){
#ifndef MINICONSOLE_TFT114
    BattleUI::draw(now);
#endif
  } else if (screen == HOME) {
    drawLibrary(now);
  } else if (screen == PARTY_LIBRARY) {
    drawPartyLibrary(now);
  } else if (screen == PARTY_PLAYING) {
    drawPartyGame(now);
  } else if (screen == DRINK_COUNTER) {
    drawDrinkCounter(now);
  } else if (screen == CONTINUE_PROMPT) {
    drawContinuePrompt(now);
  } else if (screen == SYSTEM_MENU) {
    heading("SYSTEM");
    const char *items[] = {"SETTINGS", "HOW TO PLAY", "BUTTON TEST"};
    for (int i = 0; i < 3; ++i) drawRow(18 + i * 12, items[i], systemItem == i);
    centerText(57, "A:OPEN  B:LIBRARY");
  } else if (screen == SETTINGS) {
    heading(soundSaveFailed||musicSaveFailed||gameThemeSaveFailed||(goingOut&&goalSaveFailed)?"SAVE FAILED":"SETTINGS");
    uint8_t count=settingsCount();if(settingItem>=count)settingItem=0;
    if(settingsTop>settingItem)settingsTop=settingItem;
    if(settingItem>=settingsTop+4)settingsTop=settingItem-3;
    if(settingsTop>count-4)settingsTop=count-4;
    for(uint8_t i=settingsTop;i<count&&i<settingsTop+4;++i) {
      char row[24];
      if(i==0)snprintf(row,sizeof(row),"Speed: %s",SPEED_NAMES[speed]);
      else if(i==1)snprintf(row,sizeof(row),"Walls: %s",wrapEdges?"WRAP":"SOLID");
      else if(i==2)snprintf(row,sizeof(row),"Flip screen: %s",flipped?"ON":"OFF");
      else if(i==musicSettingItem())snprintf(row,sizeof(row),"Music: %s",musicEnabled?"ON":"OFF");
      else if(i==muteSettingItem())snprintf(row,sizeof(row),"Mute: %s",soundMuted?"ON":"OFF");
      else if(drinkGoal)snprintf(row,sizeof(row),"Drink goal: %u",drinkGoal);
      else snprintf(row,sizeof(row),"Drink goal: OFF");
      drawRow(15+(i-settingsTop)*10,row,settingItem==i);
    }
    if(settingsTop)oled.fillTriangle(123,12,120,15,126,15,WHITE);
    if(settingsTop+4<count)oled.fillTriangle(123,54,120,51,126,51,WHITE);
    centerText(57, "L/R:CHANGE  B:BACK");
  } else if (screen == CONTROLS) {
    heading(controlsPage == GAME_COUNT ? "TETRIS SCORING" : GAME_NAMES[controlsPage]);
    if (controlsPage == GAME_COUNT) {
      textAt(0, 16, "1:40 2:100 3:300");
      textAt(0, 26, "4:TETRIS = 1200");
      textAt(0, 36, "Multiply by LEVEL+1");
      textAt(0, 46, "Level up: 10 lines");
    } else if (controlsPage == BLOCKS) {
      textAt(0, 16, "Turn console right 90");
      textAt(0, 26, "L/R:move DOWN:fast");
      textAt(0, 36, "UP:rotate B:drop");
      textAt(0, 46, "A:pause/go B:home");
    } else if (controlsPage == SNAKE) {
      textAt(0, 16, "D-PAD: steer");
      textAt(0, 26, "Eat dots. Don't crash");
      textAt(0, 36, "A/B: pause game");
      textAt(0, 46, "Pause: A:GO B:HOME");
    } else if (controlsPage == DOOM) {
      textAt(0, 16, "U/D:walk L/R:turn");
      textAt(0, 26, "A:fire/OK B:use/back");
      textAt(0, 36, "Hold B then L/R:gun");
      textAt(0, 46, "Hold A+B:pause/home");
    } else if (controlsPage == MOTORWAYS) {
      textAt(0,16,"A:begin/confirm road");textAt(0,26,"D-pad:move/draw");
      textAt(0,36,"B:tools HOLD B:pause");textAt(0,46,"Match house to shape");
    } else if (controlsPage == SOKOBAN) {
      textAt(0, 16, "D-PAD: walk/push");
      textAt(0, 26, "Put all boxes on dots");
      textAt(0, 36, "A:undo B:pause");
      textAt(0, 46, "Paused L:restart");
    } else {
      textAt(0, 16, controlsPage == BREAKOUT ? "L/R: move paddle" : "L/R: move ship");
      textAt(0, 26, controlsPage == BREAKOUT ? "A: serve ball" : "Hold A: shoot");
      textAt(0, 36, "B:pause A:resume");
      textAt(0, 46, "Paused L:restart");
    }
    centerText(57, "L/R:PAGE  B:BACK");
  } else if (screen == BUTTON_TEST) {
    heading("BUTTON TEST");
    for (int i = 0; i < KEY_COUNT; ++i) {
      int x = (i % 2) * 65, y = 16 + (i / 2) * 13;
      if (buttons[i].held) oled.fillRoundRect(x, y - 1, 62, 11, 2, WHITE);
      oled.setTextColor(buttons[i].held ? BLACK : WHITE);
      char row[14];
      snprintf(row, sizeof(row), "%s %u", KEY_NAMES[i], BUTTON_PINS[i]);
      textAt(x + 3, y, row);
    }
    oled.setTextColor(WHITE);
    centerText(57, "HOLD B:BACK");
  } else if (menuItem==MOTORWAYS&&(screen==NEW_PLAYING||screen==NEW_PAUSED)) {
    MotorwaysUI::draw(now);
  } else if (screen >= NEW_READY && screen <= NEW_OVER) {
    drawNewGameScreen(now);
  } else if (screen == DOOM_RUNNING) {
    centerText(26, "DOOM");
  } else if (portraitScreen()) {
    drawBlocksScreen(now);
  } else {
    drawGame(now);
    if (screen == PAUSED || screen == GAME_OVER) {
      oled.fillRoundRect(10, 19, 108, 40, 3, BLACK);
      oled.drawRoundRect(10, 19, 108, 40, 3, WHITE);
      centerText(23, screen == PAUSED ? (pauseSaved ? "SAVED" : "SAVE FAILED") : (won ? "YOU WIN!" : "GAME OVER"));
      char row[24];
      snprintf(row, sizeof(row), "S:%u%s", game.score(), newBest ? " NEW BEST" : "");
      centerText(35, row);
      centerText(48, screen == PAUSED ? "A:GO  B:HOME" : "A:RETRY B:HOME");
    }
  }
  uint32_t transferStart = micros();
#ifdef MINICONSOLE_TFT114
  oled.compose();TFTUI::gameHUD();oled.presentPhysical();
#else
  oled.display();
#endif
  transferMicros += micros() - transferStart;
  ++frameCount;
}

bool initializeDisplay() {
#ifdef MINICONSOLE_TFT114
  if (!oled.begin()) { Serial.println("TFT framebuffer allocation failed"); return false; }
  oled.setTextWrap(false);
  oled.setRotation(((portraitScreen() ? 3 : 0) + (flipped ? 2 : 0)) & 3);
  Serial.println("ST7789 135x240 initialized: SPI SCK8 MOSI9 DC6 RST1 BL43 CS=-1, 40MHz; IIC SDA5 SCL44");
  return true;
#else
  uint8_t address = 0;
  for (uint8_t candidate : {uint8_t(0x3C), uint8_t(0x3D)}) {
    Wire.beginTransmission(candidate);
    if (Wire.endTransmission() == 0) { address = candidate; break; }
  }
  if (!address) {
    Serial.printf("OLED not found: SDA=9 SCL=8; idle levels SDA=%d SCL=%d (normally 1/1)\n",
                  digitalRead(OLED_SDA), digitalRead(OLED_SCL));
    return false;
  }
  // Keep the explicitly assigned I2C pins; do not restart Wire on default pins.
  if (!oled.begin(SSD1306_SWITCHCAPVCC, address, false, false)) return false;
  oled.setTextWrap(false);
  oled.setRotation(((portraitScreen() ? 3 : 0) + (flipped ? 2 : 0)) & 3);
  Serial.printf("SSD1306 128x64 ready at 0x%02X; SDA=9 SCL=8\n", address);
  return true;
#endif
}

void setup() {
  Serial.begin(115200);
  if(piezoAttached)ledcDetach(PIEZO_PIN);
  piezoAttached=piezoFailed=false;pinMode(PIEZO_PIN,OUTPUT);digitalWrite(PIEZO_PIN,LOW);
  silenceSound();doomSoundPaused=false;
  for (int i = 0; i < KEY_COUNT; ++i) pinMode(BUTTON_PINS[i], INPUT_PULLUP);
  storageReady = prefs.begin("miniconsole", false);
  soundMuted=storageReady&&prefs.getBool("muted",false);soundSaveFailed=false;
  musicEnabled=!storageReady||prefs.getBool("musicOn",true);musicSaveFailed=false;
  matchGameTheme=!storageReady||prefs.getBool("gameTheme",true);gameThemeSaveFailed=false;
  sound.begin(soundMuted);settingsTop=0;
  goingOut = menuAPending = counterBPending = counterInputArmed = pouring = counterSaveFailed = false;
  confetti = goalSaveFailed = false;
  partyItem = 0; partyInputArmed = false;
  reaction = PartyGames::ReactionDuel{}; highLow = PartyGames::HighLow{}; passBomb = PartyGames::PassBomb{};
  drinkCount = drinkGoal = 0;
#ifdef MINICONSOLE_TFT114
  loadTheme();
  snakeMotion.reset();
#endif
  if (storageReady) {
    speed = prefs.getUChar("speed", 1);
    if (speed > 2) speed = 1;
    wrapEdges = prefs.getBool("wrap", false);
    flipped = prefs.getBool("flip", false);
    best = prefs.getUShort("best", 0);
    // Keep the old scoring record stored separately rather than deleting it.
    blocksBest = prefs.getUInt("blocksBestGB", 0);
    breakoutBest = prefs.getUInt("breakBest", 0);
    invadersBest = prefs.getUInt("invBest", 0);
    motorBest = prefs.getUInt("motorBest", 0);
    partnersUnlocked=prefs.getUChar("pocketLevel",0);if(partnersUnlocked>=PocketPartners::LEVELS)partnersUnlocked=0;
    sokoSolved = prefs.getUChar("sokoSolved", 0);
    if (sokoSolved > SokobanGame::LEVELS) sokoSolved = 0;
    sokoLevel = sokoSolved < SokobanGame::LEVELS ? sokoSolved : 0;
    if (loadCheckpoint()) { menuItem = checkpoint.gameId; screen = CONTINUE_PROMPT; }
    uint32_t out = prefs.getUInt("goingOut", 0);
    if ((out & ~OUT_MODE_FLAG) <= MAX_DRINKS) {
      goingOut = (out & OUT_MODE_FLAG) != 0; drinkCount = out & ~OUT_MODE_FLAG;
    }
    drinkGoal = prefs.getUChar("drinkGoal", 0);
    if (drinkGoal > MAX_DRINK_GOAL) drinkGoal = 0;
  }
#ifndef MINICONSOLE_HOST_TEST
  if (prefs.getBool("doomResume", false) && doomHasSave()) { screen = CONTINUE_PROMPT; menuItem = DOOM; }
  if (DoomPort_returned()) { screen = HOME; menuItem = DOOM; }
#endif
  if (goingOut) screen = DRINK_COUNTER;
#ifdef MINICONSOLE_TFT114
  Wire.begin(5, 44);
#else
  Wire.begin(OLED_SDA, OLED_SCL);
#endif
  Wire.setClock(400000);
  Wire.setTimeOut(25);
  displayReady = initializeDisplay();
#ifdef MINICONSOLE_TFT114
  applyRotation();
  Serial.println("MiniConsole v4 TFT / arcade + party games boot; GPIO4 piezo effects");
  Serial.printf("Appearance: %s bg=%04X panel=%04X accent=%04X text=%04X\n",UITheme::name(menuTheme.preset),
    menuTheme.color[0],menuTheme.color[1],menuTheme.color[2],menuTheme.color[3]);
#else
  Serial.println("MiniConsole OLED / arcade + party games boot: A=7 B=2 U=10 R=11 L=12 D=13; I2C=400kHz; GPIO4 piezo effects");
#endif
  Serial.printf("Sound: %s (mute saved between boots)\n",soundMuted?"MUTED":"ON");
  Serial.printf("Music: %s (independent of sound effects)\n",musicEnabled?"ON":"OFF");
#ifdef MINICONSOLE_TFT114
  Serial.printf("Match game backgrounds to system theme: %s\n",matchGameTheme?"ON":"OFF");
#endif
}

#ifndef MINICONSOLE_HOST_TEST
void dumpOLEDFrame() {
#ifdef MINICONSOLE_TFT114
  DoomPort_dumpColorFrame(oled.physicalBuffer());
#else
  const uint8_t *buffer = oled.getBuffer();
  static uint8_t capture[1024];
  if (flipped) for (int i = 0; i < 1024; ++i) {
    uint8_t b = buffer[1023 - i]; b = ((b & 0x55) << 1) | ((b >> 1) & 0x55);
    b = ((b & 0x33) << 2) | ((b >> 2) & 0x33); capture[i] = (b << 4) | (b >> 4);
  }
  else memcpy(capture, buffer, 1024);
  DoomPort_dumpFrame(capture);
#endif
}

extern "C" uint32_t miniconsole_doom_buttons(void) {
  pollButtons(millis());
  uint32_t mask = 0;
  for (int i = 0; i < KEY_COUNT; ++i) if (buttons[i].held) mask |= 1u << i;
  if (!doomInputArmed) { if (!mask) doomInputArmed = true; mask = 0; }
  // USB bench keys permit controller verification without extra hardware.
  static uint32_t until[7];
  const char *keys = "jkwdasq";
  while (Serial.available()) {
    char c = Serial.read(); const char *key = strchr(keys, c);
    if (c == '!') doomDumpRequested = true;
    if (key && *key) until[key - keys] = millis() + 140;
  }
  for (int i = 0; i < 7; ++i) if (int32_t(until[i] - millis()) > 0) mask |= 1u << i;
  return mask;
}

#ifdef MINICONSOLE_TFT114
extern "C" void miniconsole_doom_present_color(const uint16_t *pixels) {
  DoomHUD hud;
  bool hasHud = DoomPort_hud(&hud);
  oled.composeDoom(pixels,flipped);
  TFTUI::doomHUD(hasHud,hasHud?hud.health:0,hasHud?hud.ammo:0,hasHud?hud.armor:0,DoomPort_paused(),
    DoomPort_saving()?"Saving...":DoomPort_pauseSaved()?"Saved":"Save failed");
  oled.presentPhysical();
  if (doomDumpRequested) { doomDumpRequested = false; dumpOLEDFrame(); }
}
#else
extern "C" void miniconsole_doom_present(const uint8_t *pixels) {
  uint8_t *buffer = oled.getBuffer();
  if (!flipped) memcpy(buffer, pixels, 1024);
  else for (int i = 0; i < 1024; ++i) {
    uint8_t b = pixels[1023 - i];
    b = ((b & 0x55) << 1) | ((b >> 1) & 0x55);
    b = ((b & 0x33) << 2) | ((b >> 2) & 0x33);
    buffer[i] = (b << 4) | (b >> 4);
  }
  DoomHUD hud;
  if (DoomPort_hud(&hud)) {
    oled.fillRect(0, 54, 128, 10, BLACK); oled.setTextColor(WHITE); oled.setTextSize(1);
    char stats[24];
    if (hud.ammo < 0) snprintf(stats, sizeof(stats), "H:%d A:-- R:%d", hud.health, hud.armor);
    else snprintf(stats, sizeof(stats), "H:%d A:%d R:%d", hud.health, hud.ammo, hud.armor);
    centerText(56, stats);
  }
  if (DoomPort_paused()) {
    oled.fillRoundRect(9, 13, 110, 50, 3, BLACK); oled.drawRoundRect(9, 13, 110, 50, 3, WHITE);
    centerText(18, "DOOM PAUSED"); centerText(30, DoomPort_saving() ? "SAVING..." : DoomPort_pauseSaved() ? "SAVED" : "SAVE FAILED");
    centerText(42, "A:GO B:HOME"); centerText(54, "UP:DOOM MENU");
  }
  oled.display();
  if (doomDumpRequested) {
    doomDumpRequested = false;
    dumpOLEDFrame();
  }
}
#endif

extern "C" void miniconsole_doom_message(const char *message) {
  oled.clearDisplay(); oled.setRotation(flipped ? 2 : 0);
  oled.setTextColor(WHITE); centerText(26, message); oled.display();
}
extern "C" void miniconsole_doom_checkpoint(bool saved) {
  if (storageReady) prefs.putBool("doomResume", saved);
}
#endif

void loop() {
  uint32_t now = millis();
  sound.setMusic(musicEnabled&&screen==BLOCKS_PLAYING&&(!battleMode||!battle.frozen(millis())),now);
  sound.update(now);
  if (screen == DOOM_RUNNING) {
#ifndef MINICONSOLE_HOST_TEST
    serviceDoomSound(now,DoomPort_paused());
#endif
    delay(1);return;
  }
  pollButtons(now);
  if (!buttons[A].held) suppressFire = false;
  serviceBattle(now);
  servicePartners(now);
  handleInput(now);
  if (screen == PLAYING) {
    uint16_t interval=snakeInterval();
    if (now - lastStep >= uint32_t(interval)) {
      lastStep = now;
#ifdef MINICONSOLE_TFT114
      snakeMotion.beforeStep(game);
#endif
      SnakeGame::Result result = game.step();
#ifdef MINICONSOLE_TFT114
      snakeMotion.afterStep(result);
#endif
      if (result == SnakeGame::ATE) {game.placeFood(esp_random());playSound(Sound::Eat,now);}
      else if (result == SnakeGame::CRASHED || result == SnakeGame::WON) finishGame(result == SnakeGame::WON);
    }
  }
  if (screen == BLOCKS_PLAYING&&(!battleMode||!battle.frozen(now))) updateBlocks(now);
  if (screen == PARTY_PLAYING) updateParty(now);
  if (screen == NEW_PLAYING && menuItem == MOTORWAYS) updateMotorways(now);
  if (screen == NEW_PLAYING && (menuItem == BREAKOUT || menuItem == INVADERS)) updateArcade(now);
  if (!displayReady && now - lastDisplayTry >= 2000) {
    lastDisplayTry = now;
    displayReady = initializeDisplay();
  }
  // Request 60 fps; throughput is limited by the profile's buffered transfer.
  // The old 33 ms limit held the display to 30 fps even when the bus was faster.
  if (displayReady && now - lastFrame >= 16) { lastFrame = now; render(now); }
#if defined(MINICONSOLE_USB_BENCH) && !defined(MINICONSOLE_HOST_TEST)
  if (doomDumpRequested && displayReady) { doomDumpRequested = false; dumpOLEDFrame(); }
#endif
  if (now - lastStatus >= 5000) {
    uint32_t elapsed = now - lastStatus;
    uint32_t fpsTimes10 = frameCount * 10000 / elapsed;
    uint32_t averageTransfer = frameCount ? transferMicros / frameCount : 0;
    lastStatus = now;
#ifdef MINICONSOLE_TFT114
    Serial.printf("MiniConsole running: TFT=%s screen=%u fps=%lu.%lu transfer=%luus SPI=40MHz uptime=%lus\n",
#else
    Serial.printf("MiniConsole running: OLED=%s screen=%u fps=%lu.%lu transfer=%luus I2C=400kHz uptime=%lus\n",
#endif
                  displayReady ? "OK" : "MISSING", screen,
                  (unsigned long)(fpsTimes10 / 10), (unsigned long)(fpsTimes10 % 10),
                  (unsigned long)averageTransfer, (unsigned long)(now / 1000));
    frameCount = transferMicros = 0;
  }
  sound.setMusic(musicEnabled&&screen==BLOCKS_PLAYING&&(!battleMode||!battle.frozen(millis())),millis());
  sound.update(millis());
  delay(1);
}
