#include "../MiniConsole.ino"
#include <cassert>
#include <cstdio>
uint32_t now=0;
void tick(uint32_t dt=1){now+=dt;clockNow=now;pollButtons(now);handleInput(now);sound.update(now);}
void down(Key k){pinLevels[BUTTON_PINS[k]]=LOW;tick();tick(18);}
void up(Key k){pinLevels[BUTTON_PINS[k]]=HIGH;tick();tick(18);}
void tap(Key k){down(k);up(k);}
void settle(){now+=1200;clockNow=now;sound.update(now);}
void reboot(){screen=HOME;checkpointAvailable=false;now=clockNow=0;for(auto &b:buttons)b=Button{};prefs=Preferences{};setup();}
void cleanGame(){settle();silenceSound();for(auto &b:buttons)b=Button{};}
int main(){
 Preferences::flash.clear();reboot();assert(!soundMuted&&!sound.playing()&&piezoHz==0&&pinLevels[PIEZO_PIN]==LOW);
 // The real GPIO path navigates to a scrollable Mute row on each display profile.
 tap(B);tap(A);assert(screen==SETTINGS);settingItem=0;tap(UP);assert(settingItem==muteSettingItem());render(now);
 assert(settingsTop==settingsCount()-4);
 auto writes=Preferences::blobWrites;tap(A);assert(soundMuted&&sound.muted()&&!piezoHz&&!sound.playing());
 assert(Preferences::blobWrites==writes+1);reboot();assert(soundMuted&&sound.muted()&&!piezoHz);
 tap(RIGHT);tap(B);tap(A);settingItem=muteSettingItem();tap(A);assert(!soundMuted&&piezoHz>0);
 menuItem=SNAKE;startGame(now);assert(saveCheckpoint(now));auto save=Preferences::flash.at("miniconsole/suspend");
 // Muting cancels a sounding PWM note immediately; idle service never writes flash.
 playSound(Sound::GameOver,now);assert(piezoHz);assert(saveSoundMuted(true,now));assert(!piezoHz);
 writes=Preferences::blobWrites;for(int i=0;i<100;++i){clockNow=++now;sound.update(now);render(now);}
 assert(Preferences::blobWrites==writes&&Preferences::flash.at("miniconsole/suspend")==save);
 // Write failure is visible, runtime mute remains in force, and A retries it.
 assert(saveSoundMuted(false,now));changeScreen(SETTINGS);settingItem=muteSettingItem();
 Preferences::failNextWrite=true;tap(A);assert(soundMuted&&soundSaveFailed&&!piezoHz);
 assert(!prefs.getBool("muted",true));tap(A);assert(soundMuted&&!soundSaveFailed&&prefs.getBool("muted",false));
 assert(saveSoundMuted(false,now));
 // Existing Appearance/hidden goal positions are unchanged; Mute is always last.
 assert(saveGoingOut(true,0));changeScreen(SETTINGS);settingItem=muteSettingItem();render(now);
 assert(settingsTop==settingsCount()-4);settingItem=3;tap(RIGHT);assert(drinkGoal==1);
#ifdef MINICONSOLE_TFT114
 settingItem=4;tap(A);assert(screen==APPEARANCE);
#endif
 assert(saveGoingOut(false,0));
 // Snake emits a food cue only on a logical eat, then replaces it on collision.
 menuItem=SNAKE;startGame(now);cleanGame();game.food={int8_t(game.body[0].x+1),game.body[0].y};
 game.direction=SnakeGame::RIGHT;lastStep=now-snakeInterval();clockNow=now;loop();
 assert(game.score()==1&&sound.current()==Sound::Eat);
 finishGame(false);assert(sound.current()==Sound::GameOver);changeScreen(HOME);assert(sound.current()==Sound::Back);
 // Blocks rotate/drop, distinct Tetris clear, and pause must not leave gameplay tones active.
 menuItem=BLOCKS;startBlocks(now);cleanGame();blocks.piece=BlocksGame::T;blocks.y=4;tap(LEFT);assert(sound.current()==Sound::Rotate);
 cleanGame();for(int row=16;row<20;++row)for(int col=0;col<10;++col)blocks.board[row][col]=col==4?0:1;
 blocks.piece=BlocksGame::I;blocks.rotation=1;blocks.x=2;blocks.y=16;tap(B);
 assert(blocks.lastCleared==4&&sound.current()==Sound::Tetris);tap(A);assert(screen==BLOCKS_PAUSED&&sound.current()==Sound::Pause);
 // Breakout cues are tied to actual events rather than held buttons or redraws.
 menuItem=BREAKOUT;startSelectedGame(now);cleanGame();breakout.serving=false;breakout.x=1;breakout.y=40;breakout.vx=-1;breakout.vy=0;
 arcadeTick=now;now+=16;clockNow=now;updateArcade(now);assert(sound.current()==Sound::Bounce);
 cleanGame();breakout.x=20;breakout.y=66;breakout.vy=1;arcadeTick=now;now+=16;clockNow=now;updateArcade(now);assert(sound.current()==Sound::Hurt);
 menuItem=INVADERS;startSelectedGame(now);cleanGame();buttons[A].held=true;arcadeTick=now;now+=16;clockNow=now;updateArcade(now);assert(sound.current()==Sound::Shot);
 cleanGame();invaders.bombs[0]={invaders.player,57,true};arcadeTick=now;now+=16;clockNow=now;updateArcade(now);assert(sound.current()==Sound::Hurt);
 menuItem=SOKOBAN;sokoLevel=0;startSelectedGame(now);cleanGame();tap(RIGHT);assert(sound.current()==Sound::Push);
 cleanGame();tap(A);assert(sound.current()==Sound::Undo);cleanGame();tap(UP);cleanGame();tap(UP);assert(sound.current()==Sound::Error);
 // Party deadlines and result locks retain their behavior while producing cues.
 goingOut=true;partyItem=REACTION;changeScreen(PARTY_PLAYING);cleanGame();reaction.begin(now,42);now+=reaction.waitMs;clockNow=now;updateParty(now);assert(sound.current()==Sound::Ready);
 reaction.update(now+200,true,false);reactionSound(PartyGames::ReactionDuel::GO,now+200);assert(sound.current()==Sound::Win);
 partyItem=PASS_BOMB;passBomb.begin(now,42);cleanGame();now=passBomb.startedAt+passBomb.fuseMs;clockNow=now;updateParty(now);assert(sound.current()==Sound::Explosion);
 // Doom events cross a bounded atomic mailbox; pause discards world sounds.
 cleanGame();miniconsole_doom_sound(DOOM_PIEZO_HURT);miniconsole_doom_sound(DOOM_PIEZO_MENU);
 serviceDoomSound(now,false);assert(sound.current()==Sound::Hurt);serviceDoomSound(++now,true);assert(sound.current()==Sound::Pause);
 miniconsole_doom_sound(DOOM_PIEZO_SHOT);serviceDoomSound(++now,true);assert(pendingDoomSound.load()==0&&sound.current()==Sound::Pause);
 serviceDoomSound(++now,false);assert(sound.current()==Sound::Resume);
 saveSoundMuted(true,now);miniconsole_doom_sound(DOOM_PIEZO_DEATH);serviceDoomSound(++now,false);assert(!piezoHz&&!sound.playing());
 // PWM exhaustion fails quietly without changing game/menu state or touching other GPIOs.
 reboot();assert(saveSoundMuted(false,now));if(piezoAttached)ledcDetach(PIEZO_PIN);piezoAttached=false;piezoFailed=false;silenceSound();piezoAttachFailure=true;
 playSound(Sound::Confirm,now+1000);assert(piezoFailed&&!piezoHz);piezoAttachFailure=false;
 puts("Sound UI: mute scrolling/save/reboot/retry, immediate PWM silence, save preservation, arcade/party events, Doom mailbox/pause and hardware-failure handling passed.");
}
