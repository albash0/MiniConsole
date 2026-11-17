#include "../MiniConsole.ino"
#include <cassert>
#include <cstdio>
uint32_t now=0;
void tick(uint32_t dt=1){now+=dt;clockNow=now;pollButtons(now);handleInput(now);}
void down(Key key){pinLevels[BUTTON_PINS[key]]=LOW;tick();tick(18);}
void up(Key key){pinLevels[BUTTON_PINS[key]]=HIGH;tick();tick(18);}
void tap(Key key){down(key);up(key);}
void reboot(){screen=HOME;checkpointAvailable=false;now=clockNow=0;for(auto &b:buttons)b=Button{};prefs=Preferences{};setup();}
void openTheme(){changeScreen(SETTINGS);settingItem=0;tap(UP);tap(UP);tap(UP);assert(settingItem==gameThemeSettingItem());render(now);}
int main(){
 Preferences::flash.clear();reboot();assert(matchGameTheme);
 menuItem=SNAKE;startGame(now);assert(saveCheckpoint(now));auto save=Preferences::flash.at("miniconsole/suspend");
 pendingTheme=UITheme::preset(6);assert(applyTheme());auto theme=Preferences::flash.at("miniconsole/theme");
 openTheme();uint32_t writes=Preferences::blobWrites;tap(A);assert(!matchGameTheme&&!gameThemeSaveFailed);
 assert(Preferences::blobWrites==writes+1);assert(musicEnabled&&!soundMuted);
 assert(Preferences::flash.at("miniconsole/suspend")==save&&Preferences::flash.at("miniconsole/theme")==theme);
 reboot();assert(!matchGameTheme&&menuTheme.preset==6&&checkpointAvailable&&musicEnabled&&!soundMuted);
 assert(resumeCheckpoint(now));render(now);assert(oled.nativeCanvas().getPixel(0,0)==ConsoleTFT114::BACKGROUND);
 openTheme();Preferences::failNextWrite=true;tap(A);assert(matchGameTheme&&gameThemeSaveFailed&&!prefs.getBool("gameTheme",true));
 tap(A);assert(matchGameTheme&&!gameThemeSaveFailed&&prefs.getBool("gameTheme",false));
 reboot();assert(matchGameTheme&&menuTheme.preset==6&&checkpointAvailable);
 assert(resumeCheckpoint(now));render(now);assert(oled.nativeCanvas().getPixel(0,0)==menuTheme.color[UITheme::BACKGROUND]);
 // Hidden goal and Appearance rows retain their existing positions.
 assert(saveGoingOut(true,0));openTheme();tap(LEFT);assert(!matchGameTheme);tap(UP);assert(settingItem==4);tap(A);assert(screen==APPEARANCE);
 assert(saveGoingOut(false,0));reboot();assert(!matchGameTheme&&musicEnabled&&!soundMuted);
 puts("Game-theme setting: GPIO navigation, scrolling, classic/themed backgrounds, verified save/reboot/retry, independent audio, hidden rows and preserved palette/checkpoint passed.");
}
