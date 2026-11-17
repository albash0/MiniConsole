#pragma once
#include <stdint.h>
#include "TetrisMusic.h"

// Monophonic, allocation-free piezo effects. The loop services deadlines; PWM
// generates the waveform, so neither a note nor a melody blocks game input.
namespace ConsoleSound {
enum class Effect : uint8_t {
  None, Navigate, Confirm, Back, Start, Pause, Resume, Move, Rotate, Drop,
  Lock, Clear1, Clear2, Clear3, Tetris, Eat, Bounce, Brick, Shot, Hit,
  Hurt, GameOver, Win, Push, Target, Undo, Error, Ready, Deal, Tick,
  Explosion, Pickup, Door
};
struct Note { uint16_t hz, ms; };
struct Pattern { const Note *notes; uint8_t count; };
constexpr uint8_t priority(Effect e) {
  switch(e) {
    case Effect::None:return 0;
    case Effect::Navigate:case Effect::Move:case Effect::Tick:return 1;
    case Effect::Bounce:case Effect::Shot:case Effect::Rotate:case Effect::Undo:case Effect::Deal:return 2;
    case Effect::Confirm:case Effect::Back:case Effect::Drop:case Effect::Lock:case Effect::Brick:case Effect::Hit:case Effect::Push:case Effect::Door:return 3;
    case Effect::Eat:case Effect::Target:case Effect::Pickup:case Effect::Error:return 4;
    case Effect::Clear1:case Effect::Clear2:case Effect::Clear3:case Effect::Tetris:case Effect::Ready:return 5;
    case Effect::Start:case Effect::Pause:case Effect::Resume:case Effect::Hurt:return 6;
    default:return 7;
  }
}
inline Pattern pattern(Effect e) {
  static constexpr Note nav[]={{2400,18}}, confirm[]={{2600,35},{0,12},{3400,45}}, back[]={{2700,30},{2100,40}};
  static constexpr Note start[]={{2000,45},{0,15},{2600,45},{0,15},{3400,80}};
  static constexpr Note pause[]={{3000,40},{0,12},{2200,60}}, resume[]={{2200,35},{0,12},{3000,55}};
  static constexpr Note move[]={{1800,12}}, rotate[]={{2300,22},{3100,28}}, drop[]={{3300,18},{2700,18},{2000,40}};
  static constexpr Note lock[]={{1500,30}}, clear1[]={{2400,50},{3200,80}}, clear2[]={{2200,45},{2800,45},{3500,90}};
  static constexpr Note clear3[]={{2000,45},{2500,45},{3000,45},{3800,100}};
  static constexpr Note tetris[]={{2000,60},{0,20},{2500,60},{0,20},{3000,60},{0,20},{4000,150}};
  static constexpr Note eat[]={{2700,30},{3600,45}}, bounce[]={{2200,18}}, brick[]={{3000,25},{3800,30}};
  static constexpr Note shot[]={{4000,15},{3000,15},{2000,20}}, hit[]={{2000,30},{1400,45}};
  static constexpr Note hurt[]={{1800,55},{0,25},{1300,90}};
  static constexpr Note over[]={{2600,90},{0,25},{2200,90},{0,25},{1700,100},{0,25},{1100,180}};
  static constexpr Note win[]={{2200,70},{0,20},{2700,70},{0,20},{3300,70},{0,20},{4000,170}};
  static constexpr Note push[]={{1600,30},{2000,30}}, target[]={{2400,40},{3200,70}}, undo[]={{3000,25},{2300,30}};
  static constexpr Note error[]={{1100,45},{0,30},{1100,45}}, ready[]={{3500,120}}, deal[]={{2100,20},{2900,20}};
  static constexpr Note tick[]={{3000,15}}, explosion[]={{2600,25},{1900,30},{1400,45},{1000,70},{700,100}};
  static constexpr Note pickup[]={{2400,35},{3200,35},{4000,65}}, door[]={{1400,35},{1800,35},{2300,45}};
#define CONSOLE_PATTERN(x) return {x,uint8_t(sizeof(x)/sizeof(x[0]))}
  switch(e) {
    case Effect::Navigate:CONSOLE_PATTERN(nav);case Effect::Confirm:CONSOLE_PATTERN(confirm);case Effect::Back:CONSOLE_PATTERN(back);
    case Effect::Start:CONSOLE_PATTERN(start);case Effect::Pause:CONSOLE_PATTERN(pause);case Effect::Resume:CONSOLE_PATTERN(resume);
    case Effect::Move:CONSOLE_PATTERN(move);case Effect::Rotate:CONSOLE_PATTERN(rotate);case Effect::Drop:CONSOLE_PATTERN(drop);
    case Effect::Lock:CONSOLE_PATTERN(lock);case Effect::Clear1:CONSOLE_PATTERN(clear1);case Effect::Clear2:CONSOLE_PATTERN(clear2);
    case Effect::Clear3:CONSOLE_PATTERN(clear3);case Effect::Tetris:CONSOLE_PATTERN(tetris);case Effect::Eat:CONSOLE_PATTERN(eat);
    case Effect::Bounce:CONSOLE_PATTERN(bounce);case Effect::Brick:CONSOLE_PATTERN(brick);case Effect::Shot:CONSOLE_PATTERN(shot);
    case Effect::Hit:CONSOLE_PATTERN(hit);case Effect::Hurt:CONSOLE_PATTERN(hurt);case Effect::GameOver:CONSOLE_PATTERN(over);
    case Effect::Win:CONSOLE_PATTERN(win);case Effect::Push:CONSOLE_PATTERN(push);case Effect::Target:CONSOLE_PATTERN(target);
    case Effect::Undo:CONSOLE_PATTERN(undo);case Effect::Error:CONSOLE_PATTERN(error);case Effect::Ready:CONSOLE_PATTERN(ready);
    case Effect::Deal:CONSOLE_PATTERN(deal);case Effect::Tick:CONSOLE_PATTERN(tick);case Effect::Explosion:CONSOLE_PATTERN(explosion);
    case Effect::Pickup:CONSOLE_PATTERN(pickup);case Effect::Door:CONSOLE_PATTERN(door);default:return {nullptr,0};
  }
#undef CONSOLE_PATTERN
}
class Player {
public:
  using Output=void (*)(uint16_t);
  explicit Player(Output output):output_(output){}
  void begin(bool muted) { muted_=muted;last_=Effect::None;music_=TetrisMusic{};stop(); }
  void setMuted(bool muted) { muted_=muted;last_=Effect::None;if(muted){finishEffect();emit(0);} }
  bool muted()const{return muted_;}
  bool playing()const{return current_!=Effect::None;}
  Effect current()const{return current_;}
  void stop(){finishEffect();music_.pause();emit(0);}
  void restartMusic(uint32_t now){music_.restart(now);}
  void setMusic(bool active,uint32_t now){music_.setPlaying(active,now);}
  uint32_t musicPosition()const{return music_.position();}
  bool musicPlaying()const{return music_.playing();}
  bool play(Effect effect,uint32_t now) {
    if(muted_||effect==Effect::None)return false;
    update(now);
    if(playing()&&priority(effect)<priority(current_))return false;
    uint16_t cooldown=priority(effect)<=2?60:0;
    if(effect==last_&&now-lastAt_<cooldown)return false;
    sequence_=pattern(effect);if(!sequence_.count)return false;
    current_=last_=effect;lastAt_=now;index_=0;deadline_=now+sequence_.notes[0].ms;
    emit(sequence_.notes[0].hz);return true;
  }
  void update(uint32_t now) {
    music_.update(now);
    // Effects temporarily own the voice, including their rests. Music keeps its
    // beat underneath and returns at its current note when the effect ends.
    while(playing()&&int32_t(now-deadline_)>=0) {
      if(++index_>=sequence_.count){finishEffect();break;}
      deadline_+=sequence_.notes[index_].ms;
    }
    emit(muted_?0:playing()?sequence_.notes[index_].hz:music_.frequency());
  }
private:
  TetrisMusic music_;
  void finishEffect(){current_=Effect::None;sequence_={nullptr,0};index_=0;}
  Output output_;Pattern sequence_{nullptr,0};Effect current_=Effect::None,last_=Effect::None;
  uint8_t index_=0;uint32_t deadline_=0,lastAt_=0;uint16_t frequency_=0;bool muted_=false;
  void emit(uint16_t hz){if(hz!=frequency_){frequency_=hz;output_(hz);}}
};
} // namespace ConsoleSound
