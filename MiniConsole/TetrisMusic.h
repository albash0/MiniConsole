#pragma once
#include <stdint.h>

namespace ConsoleSound {
// Single-voice arrangement of the traditional Korobeiniki melody. Pitches are
// raised for the small piezo; no game recording, sample data or mixer is used.
class TetrisMusic {
public:
  struct Step { uint16_t hz; uint8_t eighths; };
  static constexpr uint16_t EIGHTH_MS=208; // About 144 quarter notes/minute.
  static const Step *score(uint16_t &count) {
    static constexpr uint16_t A=1760,B=1976,C=2093,D=2349,E=2637,F=2794,G=3136,AH=3520,GS=1661;
    static constexpr Step notes[]={
      {E,2},{B,1},{C,1},{D,2},{C,1},{B,1},{A,2},{A,1},{C,1},{E,2},{D,1},{C,1},
      {B,3},{C,1},{D,2},{E,2},{C,2},{A,2},{A,2},{0,2},
      {D,3},{F,1},{AH,2},{G,1},{F,1},{E,3},{C,1},{E,2},{D,1},{C,1},
      {B,2},{B,1},{C,1},{D,2},{E,2},{C,2},{A,2},{A,2},{0,2},
      {E,4},{C,4},{D,4},{B,4},{C,4},{A,4},{GS,4},{B,2},{0,2},
      {E,4},{C,4},{D,4},{B,4},{C,2},{E,2},{AH,4},{GS,4},{0,4}
    };
    count=sizeof(notes)/sizeof(notes[0]);return notes;
  }
  static uint32_t lengthMs() {
    uint16_t count;const Step *notes=score(count);uint32_t length=0;
    for(uint16_t i=0;i<count;++i)length+=uint32_t(notes[i].eighths)*EIGHTH_MS;
    return length;
  }
  void restart(uint32_t now){position_=0;at_=now;}
  void setPlaying(bool active,uint32_t now){update(now);active_=active;}
  void pause(){active_=false;}
  void update(uint32_t now){if(active_)position_=uint32_t((uint64_t(position_)+uint32_t(now-at_))%lengthMs());at_=now;}
  bool playing()const{return active_;}
  uint32_t position()const{return position_;}
  uint16_t frequency()const {
    if(!active_)return 0;
    uint16_t count;const Step *notes=score(count);uint32_t age=position_;
    for(uint16_t i=0;i<count;++i){uint32_t duration=uint32_t(notes[i].eighths)*EIGHTH_MS;
      if(age<duration)return age<duration-25?notes[i].hz:0; // Articulate repeated notes.
      age-=duration;
    }
    return 0;
  }
private:
  uint32_t position_=0,at_=0;bool active_=false;
};
} // namespace ConsoleSound
