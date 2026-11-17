#pragma once
#include "SnakeGame.h"

// Display-only history: every previous segment is the following current cell.
// Only the departed tail needs remembering; saves and collision rules stay on
// the existing grid. Points are in 1/256-cell units, unwrapped at screen edges.
struct SnakeMotion {
  struct Point { int32_t x,y; };
  SnakeGame::Cell oldTail={0,0};
  SnakeGame::Cell oldFood={0,0};
  bool moving=false,ate=false;
  void reset(){moving=ate=false;}
  void beforeStep(const SnakeGame &game){oldTail=game.body[game.length-1];oldFood=game.food;moving=true;ate=false;}
  void afterStep(SnakeGame::Result result){ate=result==SnakeGame::ATE||result==SnakeGame::WON;}
  SnakeGame::Cell food(const SnakeGame &game,uint16_t phase)const{return moving&&ate&&phase<256?oldFood:game.food;}
  static int shortest(int delta,int size){if(delta>size/2)delta-=size;else if(delta<-size/2)delta+=size;return delta;}
  Point point(const SnakeGame &game,int segment,uint16_t phase)const {
    SnakeGame::Cell to=game.body[segment];
    if(!moving)return {int32_t(to.x)*256,int32_t(to.y)*256};
    if(phase>256)phase=256;
    SnakeGame::Cell from=segment+1<game.length?game.body[segment+1]:oldTail;
    int dx=to.x-from.x,dy=to.y-from.y;
    if(game.wrap){dx=shortest(dx,SnakeGame::COLS);dy=shortest(dy,SnakeGame::ROWS);}
    return {int32_t(from.x)*256+dx*int32_t(phase),int32_t(from.y)*256+dy*int32_t(phase)};
  }
};
