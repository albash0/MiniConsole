#include "../MiniConsole.ino"
#include "motorways_fixture.h"
#include <cassert>
void clear(){for(auto &b:buttons)b=Button{};}
int main(){setup();menuItem=MOTORWAYS;startSelectedGame(1000);fixedStart(motorways);auto &g=motorways;
 for(int d=4;d<8;++d){fixedStart(g);g.cursor=g.cell(15,5);auto start=g.cursor;auto stock=g.stock;assert(g.begin()==g.OK&&g.move(d)==g.OK);auto end=g.cursor;assert(g.commit()==g.OK&&g.stock==stock-1&&g.valid());assert(g.nextHop(start,end)==end&&g.nextHop(end,start)==start);assert(g.erase(start)==g.OK&&g.stock==stock&&g.valid());}
 fixedStart(g);g.cursor=g.cell(14,4);g.begin();g.move(5);g.move(5);auto end=g.cursor;auto stock=g.stock;assert(g.commit()==g.OK&&g.stock==stock-2&&g.valid());assert(g.nextHop(g.cell(14,4),end)==g.cell(15,5));
 // Crossing diagonals create a routable center junction.
 g.cursor=g.cell(15,4);g.begin();assert(g.move(6)==g.OK&&g.commit()==g.OK&&g.valid());
 assert(g.roadConnected(g.cell(14,4),g.cell(15,4)));assert(g.nextHop(g.cell(14,4),g.cell(15,4))==g.cell(15,4));
 fixedStart(g);g.cursor=g.cell(14,4);g.begin();auto budgetBefore=g.stock;g.move(2);g.move(1);assert(g.planLength==2&&g.cursor==g.cell(15,5));assert(g.commit()==g.OK&&g.stock==budgetBefore-1);
 // Two cars approaching the X wait for the occupied center, then clear it.
 fixedStart(g);memset(g.roads,0,g.N);g.connect(g.cell(14,4),g.cell(15,5));g.connect(g.cell(15,4),g.cell(14,5));
 g.cars[0].active=true;g.cars[0].home=0;g.cars[0].dest=0;g.cars[0].from=g.cell(14,4);g.cars[0].to=g.cell(15,5);g.cars[0].progress=115;
 g.cars[1]=g.cars[0];g.cars[1].from=g.cell(15,4);g.cars[1].to=g.cell(14,5);g.cars[1].progress=92;
 assert(g.erase(g.cell(14,5))==g.BUSY);g.step();assert(g.cars[0].progress>115&&g.cars[1].progress==92);
 for(int i=0;i<4;++i)g.step();assert(g.cars[1].progress>92);
 // A turning car really travels across the center and reaches the other arm.
 fixedStart(g);memset(g.roads,0,g.N);g.homes[0].cell=g.cell(12,6);g.destinations[0].anchor=g.cell(17,4);
 g.connect(g.cell(12,6),g.cell(13,5));g.connect(g.cell(13,5),g.cell(14,4));g.connect(g.cell(14,5),g.cell(13,4));
 for(int x=14;x<17;++x)g.connect(g.cell(x,5),g.cell(x+1,5));
 assert(g.nextHop(g.cell(13,5),g.port(0))==g.cell(14,5));for(int i=0;i<160;++i){g.step();assert(g.valid());}assert(g.score>0);
 g.cars[0]=MotorwaysGame::Car{};g.cars[0].active=true;g.cars[0].from=g.cell(13,5);g.cars[0].to=g.cell(14,5);g.cars[0].progress=100;
 assert(g.valid()&&saveCheckpoint(1700));g=MotorwaysGame{};assert(resumeCheckpoint(1800)&&g.cars[0].progress==100&&g.crossingFor(g.cars[0].from,g.cars[0].to)!=g.NONE);
 fixedStart(g);
 // No corner cuts through buildings; diagonal bridges use one token per water tile.
 g.cursor=g.cell(5,1);g.begin();assert(g.move(5)==g.BLOCKED);g.cancel();
 g.cursor=g.cell(9,5);auto before=g.stock;g.begin();assert(g.move(4)==g.OK&&g.move(4)==g.OK&&g.commit()==g.OK);assert(g.bridges==0&&g.stock==before-2&&g.valid());
 g.cursor=g.cell(10,4);g.begin();g.move(1);assert(g.commit()==g.BLOCKED);g.cancel();
 assert(saveCheckpoint(1100));auto snapshot=g;g=MotorwaysGame{};assert(resumeCheckpoint(1200));assert(!memcmp(g.roads,snapshot.roads,g.N));
 // A cardinal button starts a short chord window; two held directions make one diagonal.
 fixedStart(g);g.cursor=g.cell(14,4);g.begin();clear();buttons[UP].pressed=buttons[UP].held=true;handleInput(1300);assert(g.cursor==g.cell(14,4));
 buttons[UP].pressed=false;buttons[RIGHT].pressed=buttons[RIGHT].held=true;handleInput(1318);buttons[RIGHT].pressed=false;handleInput(1340);assert(g.cursor==g.cell(15,3)&&g.planLength==2);g.commit();clear();
 render(1400);assert(g.valid());
 // Traffic follows diagonals and returns, using the same persisted road graph.
 fixedStart(g);g.homeCount=g.destCount=2;g.homes[0].cell=g.cell(12,7);g.destinations[0].anchor=g.cell(16,2);memset(g.roads,0,g.N);
 for(int i=0;i<4;++i)g.connect(g.cell(12+i,7-i),g.cell(13+i,6-i));
 g.grace[1]=g.OPEN_GRACE;for(int i=0;i<150;++i){g.step();assert(g.valid());}assert(g.score>0);
 puts("Diagonal roads: all four directions, one-road cost/refund, routing, chords, traffic, corners/crossings/bridges and save/resume passed.");
}
