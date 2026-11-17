#include "../MiniConsole.ino"
#include <cassert>
#include <cstdio>
uint32_t now=0;
void tick(uint32_t dt=1){now+=dt;clockNow=now;pollButtons(now);handleInput(now);sound.update(now);}
void down(Key key){pinLevels[BUTTON_PINS[key]]=LOW;tick();tick(18);}
void up(Key key){pinLevels[BUTTON_PINS[key]]=HIGH;tick();tick(18);}
void tap(Key key){down(key);up(key);}
void reboot(){screen=HOME;checkpointAvailable=false;now=clockNow=0;for(auto &b:buttons)b=Button{};prefs=Preferences{};setup();}
void openMusic(){changeScreen(SETTINGS);settingItem=0;tap(UP);assert(settingItem==muteSettingItem());tap(UP);assert(settingItem==musicSettingItem());render(now);}
int main(){
 Preferences::flash.clear();reboot();assert(musicEnabled&&!soundMuted);
 menuItem=BLOCKS;startBlocks(0);loop();now=clockNow=500;loop();assert(sound.musicPlaying());
 buttons[A].pressed=true;handleInput(now);buttons[A]=Button{};assert(screen==BLOCKS_PAUSED&&pauseSaved);
 auto checkpoint=Preferences::flash.at("miniconsole/suspend");openMusic();uint32_t writes=Preferences::blobWrites;
 tap(A);assert(!musicEnabled&&!musicSaveFailed&&!soundMuted&&!sound.musicPlaying());
 assert(Preferences::blobWrites==writes+1&&Preferences::flash.at("miniconsole/suspend")==checkpoint);
 writes=Preferences::blobWrites;for(int i=0;i<30;++i){tick(20);render(now);}assert(Preferences::blobWrites==writes);
 reboot();assert(!musicEnabled&&!soundMuted&&checkpointAvailable);assert(resumeCheckpoint(0));
 now=clockNow=1000;loop();assert(!sound.musicPlaying()&&piezoHz==0);
 silenceSound();playSound(Sound::Eat,now);assert(piezoHz>0&&!sound.musicPlaying());
 // Music can be stopped immediately while effects remain active.
 now=clockNow=1200;assert(saveMusicEnabled(true,now));assert(sound.musicPlaying());
 now=clockNow=1500;sound.update(now);assert(piezoHz>0);assert(saveMusicEnabled(false,now));assert(!sound.musicPlaying()&&piezoHz>0);
 now=clockNow=2000;sound.update(now);assert(piezoHz==0&&!soundMuted);
 playSound(Sound::Tetris,now);assert(piezoHz>0&&!sound.musicPlaying());
 // Global mute still overrides both, without replacing the Music preference.
 assert(saveMusicEnabled(true,now));assert(saveSoundMuted(true,now));now=clockNow=2300;loop();assert(sound.musicPlaying()&&piezoHz==0);
 assert(saveMusicEnabled(false,now));reboot();assert(!musicEnabled&&soundMuted);
 assert(saveSoundMuted(false,now));assert(!musicEnabled&&!soundMuted);
 // Failed Music writes leave the requested runtime state and A retries it.
 openMusic();Preferences::failNextWrite=true;tap(A);
 assert(musicEnabled&&musicSaveFailed&&!prefs.getBool("musicOn",true)&&!soundMuted);
 tap(A);assert(musicEnabled&&!musicSaveFailed&&prefs.getBool("musicOn",false));
 reboot();assert(musicEnabled&&!soundMuted);
 // A hidden goal row never changes which two settings control audio.
 assert(saveGoingOut(true,0));openMusic();tap(LEFT);assert(!musicEnabled);
 tap(DOWN);assert(settingItem==muteSettingItem());tap(A);assert(soundMuted&&!musicEnabled);
 assert(saveGoingOut(false,0));reboot();assert(!musicEnabled&&soundMuted);
 puts("Music setting: GPIO navigation/scrolling, saved toggle/reboot/retry, independent SFX, immediate stop, global mute, hidden rows and checkpoint preservation passed.");
}
