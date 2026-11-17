#include "../ConsoleSound.h"
#include "../../libraries/DiablitoOLED/src/DoomPiezo.h"
#include <cassert>
#include <cstdio>
#include <vector>
#include <limits>
using namespace ConsoleSound;
std::vector<uint16_t> output;
void tone(uint16_t hz){output.push_back(hz);}
int main(){
  Player p(tone);p.begin(false);
  assert(p.play(Effect::Confirm,1000)&&output.back()==2600);
  p.update(1034);assert(output.size()==1);
  p.update(1035);assert(output.back()==0);
  p.update(1047);assert(output.back()==3400);
  p.update(1092);assert(!p.playing()&&output.back()==0);
  // Quiet motion ticks cannot erase a line-clear fanfare; a loss can interrupt it.
  assert(p.play(Effect::Tetris,2000));assert(!p.play(Effect::Move,2020));assert(p.current()==Effect::Tetris);
  assert(p.play(Effect::GameOver,2030));assert(p.current()==Effect::GameOver);
  p.setMuted(true);assert(!p.playing()&&output.back()==0);
  size_t count=output.size();assert(!p.play(Effect::Win,3000));p.update(4000);assert(output.size()==count);
  p.setMuted(false);assert(p.play(Effect::Navigate,4000));p.update(4018);
  assert(!p.play(Effect::Navigate,4030));assert(p.play(Effect::Navigate,4060));
  // A blocked loop skips stale notes instead of playing a burst or sustaining forever.
  p.stop();assert(p.play(Effect::Win,5000));count=output.size();p.update(10000);
  assert(!p.playing()&&output.size()==count+1&&output.back()==0);
  uint32_t start=std::numeric_limits<uint32_t>::max()-20;
  assert(p.play(Effect::Confirm,start));p.update(start+35);assert(output.back()==0);
  p.update(start+47);assert(output.back()==3400);p.update(start+92);assert(!p.playing());
  // Every effect is bounded and returns to silence, including after clock rollover.
  for(int i=1;i<=int(Effect::Door);++i){
    p.begin(false);Effect e=Effect(i);auto seq=pattern(e);assert(seq.count>0&&seq.count<=7);
    uint32_t length=0;for(int n=0;n<seq.count;++n){assert(seq.notes[n].ms>0);length+=seq.notes[n].ms;}
    assert(length<=1000);assert(p.play(e,start));p.update(start+length);assert(!p.playing());
  }
  // Background melody loops, freezes on pause, respects mute, and yields its
  // single voice to effects without losing the underlying beat.
  p.begin(false);p.setMusic(true,1000);p.update(1000);assert(output.back()==2637);
  p.update(1416);assert(output.back()==1976);
  assert(p.play(Effect::Move,1500));assert(output.back()==1800);
  p.update(1512);assert(!p.playing()&&output.back()==1976&&p.musicPlaying());
  p.setMusic(false,1600);p.update(9000);assert(output.back()==0&&p.musicPosition()==600);
  p.setMusic(true,9000);p.update(9024);assert(output.back()==2093&&p.musicPosition()==624);
  p.setMuted(true);assert(output.back()==0);count=output.size();p.update(9500);assert(output.size()==count);
  p.setMuted(false);p.update(9501);assert(p.musicPlaying()&&p.musicPosition()==1101&&output.back()==2349);
  p.stop();p.update(12000);assert(!p.musicPlaying()&&output.back()==0);
  p.restartMusic(start);p.setMusic(true,start);p.update(start+TetrisMusic::lengthMs()+100);
  assert(p.musicPosition()==100&&output.back()==2637);
  p.setMusic(false,start+TetrisMusic::lengthMs()+150);uint32_t frozen=p.musicPosition();
  p.update(500000);assert(p.musicPosition()==frozen&&output.back()==0);
  assert(DoomPiezo_classify("pistol")==DOOM_PIEZO_SHOT);
  assert(DoomPiezo_classify("shotgn")==DOOM_PIEZO_SHOT);
  assert(DoomPiezo_classify("doropn")==DOOM_PIEZO_DOOR);
  assert(DoomPiezo_classify("itemup")==DOOM_PIEZO_PICKUP);
  assert(DoomPiezo_classify("plpain")==DOOM_PIEZO_HURT);
  assert(DoomPiezo_classify("pldeth")==DOOM_PIEZO_DEATH);
  assert(DoomPiezo_classify("stnmov")==DOOM_PIEZO_MENU);
  assert(DoomPiezo_classify("posit1")==DOOM_PIEZO_NONE);
  assert(DoomPiezo_classify(nullptr)==DOOM_PIEZO_NONE);
  puts("Sound sequencer: bounded notes, deadlines/rests, priorities, rate limiting, immediate mute, stall recovery, clock wrap music loop/pause/mute/SFX arbitration and Doom event mapping passed.");
}
