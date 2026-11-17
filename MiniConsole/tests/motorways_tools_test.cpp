#include "../MiniConsole.ino"
#include <cassert>
void clear(){for(auto &b:buttons)b=Button{};}
void tap(Key k,uint32_t t){clear();buttons[k].pressed=buttons[k].held=true;handleInput(t);clear();handleInput(t+30);}
void holdB(uint32_t t){clear();buttons[B].pressed=buttons[B].held=true;handleInput(t);buttons[B].pressed=false;handleInput(t+600);clear();handleInput(t+630);}
int main(){Preferences::flash.clear();setup();menuItem=MOTORWAYS;startSelectedGame(1000);
 tap(B,1100);assert(motorTools&&screen==NEW_PLAYING);auto ticks=motorways.ticks;updateMotorways(4000);assert(motorways.ticks==ticks);render(4000);
 auto tool=motorways.tool;tap(DOWN,4100);assert(motorways.tool==(tool+1)%3);tap(A,4200);assert(!motorTools);
 holdB(5000);assert(screen==NEW_PAUSED&&pauseSaved);motorPauseItem=2;tap(A,5700);assert(motorways.systemTheme&&prefs.getBool("motorTheme",false)&&!motorPaletteSaveFailed);
 startSelectedGame(6000);assert(motorways.systemTheme&&!motorTools&&!motorBPending);
 assert(saveCheckpoint(6050));motorways.systemTheme=false;assert(resumeCheckpoint(6100)&&motorways.systemTheme);
 holdB(7000);motorPauseItem=2;tap(A,7700);assert(!motorways.systemTheme&&!prefs.getBool("motorTheme",true));
 startSelectedGame(8000);assert(!motorways.systemTheme);motorways.begin();tap(B,8100);assert(!motorways.editing&&motorTools&&screen==NEW_PLAYING);
 tap(A,8150);tap(B,8200);assert(motorTools);tap(B,8300);assert(!motorTools&&screen==NEW_PLAYING);
 puts("Motorways tools: tap/hold distinction, frozen simulation, highlighted selection, preview cancel, flash palette and new-run/resume persistence passed.");}
