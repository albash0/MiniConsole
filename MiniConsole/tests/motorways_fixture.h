#pragma once
#include "../MotorwaysGame.h"
// Fixed legacy geometry for tests that exercise specific cells. Production
// reset() is randomized and gets independent layout/budget coverage.
inline void fixedStart(MotorwaysGame &g,uint32_t seed=1,uint8_t difficulty=1){
  g=MotorwaysGame{};g.rng=seed?seed:1;g.difficulty=difficulty;g.stock=32;g.homeCount=g.destCount=2;
  g.homes[0]={g.cell(2,2),0,0};g.homes[1]={g.cell(2,7),1,0};
  g.destinations[0].anchor=g.cell(6,1);g.destinations[0].color=0;
  g.destinations[1].anchor=g.cell(6,6);g.destinations[1].color=1;
  for(int i=0;i<2;++i){g.connect(g.homes[i].cell,g.homes[i].cell+1);g.connect(g.port(i),g.port(i)-1);}
  g.cursor=g.homes[0].cell;
}
