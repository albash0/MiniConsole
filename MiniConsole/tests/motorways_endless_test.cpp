#include "../MotorwaysGame.h"
#include "motorways_fixture.h"
#include "../SuspendSave.h"
#include <cassert>
#include <cstdio>
#include <vector>
using G=MotorwaysGame;
void matureTown(G &g){
  g=G{};g.rng=123;g.homeCount=12;g.destCount=6;
  const uint8_t homes[]={G::cell(1,1),G::cell(1,3),G::cell(1,5),G::cell(1,7),G::cell(3,1),G::cell(3,3),G::cell(3,5),G::cell(3,7),G::cell(19,1),G::cell(19,3),G::cell(19,5),G::cell(19,7)};
  const uint8_t sites[]={G::cell(5,0),G::cell(5,4),G::cell(5,8),G::cell(15,0),G::cell(15,4),G::cell(15,8)};
  for(int i=0;i<12;++i)g.homes[i]={homes[i],uint8_t(i%3),0};
  for(int i=0;i<6;++i){g.destinations[i].anchor=sites[i];g.destinations[i].color=i%3;}
  // A dense shared road fixture exposes congestion at heavily reused junctions.
  // Rules and resource costs have separate tests.
  for(int p=0;p<g.N;++p)if(!g.blocked(p))for(int d:{1,2}){
    uint8_t q=g.neighbor(p,d);if(q==g.NONE||g.blocked(q)||(d==2&&(g.water(p)||g.water(q))))continue;g.connect(p,q);}
  g.bridges=0;g.stock=300;assert(g.valid());
}
void goodTown(G &g){
 g=G{};g.rng=123;g.homeCount=12;g.destCount=2;
 for(int side=0;side<2;++side){int off=side*10;g.destinations[side].anchor=g.cell(off+7,3);g.destinations[side].color=side;
  const int rows[]={0,2,4,6,8,9};for(int h=0;h<6;++h){int row=rows[h];g.homes[side*6+h]={g.cell(off+1,row),uint8_t(side),0};
   for(int x=1;x<4;++x)g.connect(g.cell(off+x,row),g.cell(off+x+1,row));}
  for(int y=0;y<4;++y)g.connect(g.cell(off+4,y),g.cell(off+4,y+1));
  for(int y=6;y<9;++y)g.connect(g.cell(off+4,y),g.cell(off+4,y+1));
  for(int x=4;x<7;++x){g.connect(g.cell(off+x,4),g.cell(off+x+1,4));g.connect(g.cell(off+x,6),g.cell(off+x+1,6));}
  for(int y=4;y<6;++y)g.connect(g.cell(off+7,y),g.cell(off+7,y+1));
 }
 for(int p=0;p<g.N;++p)if(g.siteAvailable(p))g.connect(p,p+1);
 for(int p=0;p<g.N;++p)assert(!g.siteAvailable(p));assert(g.valid());
}
void thirdNeighborhood(G &g){
  fixedStart(g,42);g.homeCount=g.destCount=3;
  g.homes[2]={G::cell(15,7),2,0};g.destinations[2].anchor=G::cell(15,1);g.destinations[2].color=2;
  for(int row:{2,7})for(int x=3;x<5;++x)g.connect(g.cell(x,row),g.cell(x+1,row));
  for(int y=2;y<7;++y)g.connect(g.cell(15,y),g.cell(15,y+1));
  assert(g.valid());
}
void crc(std::vector<uint8_t> &bytes){
  size_t size=bytes.size()-SuspendSave::HEADER;bytes[8]=size;bytes[9]=size>>8;
  uint32_t sum=SuspendSave::checksum(bytes.data(),bytes.size());for(int i=0;i<4;++i)bytes[12+i]=sum>>(8*i);
}
int main(){
  G g;thirdNeighborhood(g);uint8_t roads[g.N];memcpy(roads,g.roads,g.N);
  assert(g.startRenewal()&&g.closing==0&&g.replacement!=g.destinations[0].anchor&&g.valid());
  uint8_t target=g.replacement;uint8_t old=g.destinations[0].anchor;
  assert(!g.blocked(uint8_t(target+g.W))&&g.blocked(target)&&g.building(target));
  g.cursor=target;assert(g.begin()==g.OK&&g.cursor==target+g.W);
  int entry=-1;for(int d:{2,3}){uint8_t q=g.neighbor(g.cursor,d);if(q!=g.NONE&&!g.building(q)&&!g.water(q)){entry=d;break;}}
  assert(entry>=0&&g.move(entry)==g.OK&&g.commit()==g.OK&&g.valid());
  memcpy(roads,g.roads,g.N);
  auto stock=g.stock;for(int i=0;i<G::CLOSE_NOTICE-1;++i){g.step();assert(g.closing==0&&g.destinations[0].anchor==old&&g.valid());}
  assert(g.step()&g.OPENED);assert(g.closing==g.NONE&&g.destinations[0].anchor==target&&g.grace[0]==g.openingGrace()&&g.renewals==1);
  assert(g.stock==stock&&!memcmp(roads,g.roads,g.N));
  for(int i=0;i<g.openingGrace();++i){g.step();assert(g.destinations[0].demand==1&&g.destinations[0].overload==0&&g.valid());}
  assert(g.grace[0]==0);
  // A car disconnected from home cannot hold redevelopment open forever.
  thirdNeighborhood(g);g.connect(G::cell(2,4),G::cell(3,4));g.cars[0].active=true;
  g.cars[0].from=g.cars[0].to=G::cell(2,4);g.cars[0].returning=true;
  assert(g.startRenewal()&&g.valid());
  for(int i=0;i<g.CLOSE_LIMIT-1;++i){for(int d=0;d<g.destCount;++d)if(d!=g.closing)g.grace[d]=g.OPEN_GRACE;g.step();assert(g.closing==0&&g.cars[0].active&&g.valid());}
  assert(g.step()&g.OPENED);assert(!g.cars[0].active&&g.closing==g.NONE&&g.valid());
  // A full map reuses the outgoing plot, preserves every road, and still renews.
  matureTown(g);memcpy(roads,g.roads,g.N);uint8_t previousColor=g.destinations[0].color;assert(g.startRenewal());assert(g.replacement==g.destinations[0].anchor);
  for(int i=0;i<g.CLOSE_LIMIT&&g.closing!=g.NONE;++i)g.step();
  assert(g.closing==g.NONE&&g.renewals==1&&g.destinations[0].color!=previousColor&&!memcmp(roads,g.roads,g.N)&&g.valid());
  // A dense network with shared junctions must now jam, rather than coast forever.
  matureTown(g);unsigned jamWarnings=0;
  for(int i=0;i<6000&&g.alive;++i){if(g.rewardPending)g.reward(0);jamWarnings+=bool(g.step()&g.WARNING);}
  assert(!g.alive&&jamWarnings&&g.score>0);
  // Separate feeders to destinations can survive for hundreds of weeks, while
  // bursts still generate warnings. There is no unavoidable elapsed-time loss.
  for(int difficulty=0;difficulty<3;++difficulty){
    goodTown(g);g.difficulty=difficulty;unsigned warnings=0;
    for(int tick=0;tick<240000;++tick){if(g.rewardPending)g.reward(tick%3);warnings+=bool(g.step()&g.WARNING);
      if(!g.alive){printf("Failed stable town at difficulty %d tick %d score %u\n",difficulty,tick,g.score);fflush(stdout);}assert(g.alive);
      if(tick%100==0)assert(g.valid());}
    if(difficulty)assert(warnings>0);
    assert(g.week()==201&&g.renewals>=65&&g.score>1000&&g.homeCount==12&&g.destCount==2&&g.valid());
  }
  // Crossing the old 32-bit tick limit must not reset week, rewards, or growth.
  thirdNeighborhood(g);g.ticks=uint64_t(UINT32_MAX)-40;
  for(int i=0;i<100;++i){if(g.rewardPending)g.reward(0);g.step();assert(g.valid());}
  assert(g.ticks>UINT32_MAX&&g.week()>3000000);
  // Save a closure mid-countdown, including its reserved lot and classic/system choice.
  thirdNeighborhood(g);g.ticks=uint64_t(UINT32_MAX)+1234;g.systemTheme=true;g.startRenewal();
  for(int i=0;i<45;++i)g.step();
  SuspendState s;s.gameId=6;s.motorways=g;uint8_t data[2048],copy[2048];size_t n=SuspendSave::encode(s,data);assert(n);
  SuspendState restored;assert(SuspendSave::decode(data,n,restored)&&restored.motorways.systemTheme&&restored.motorways.ticks==g.ticks);
  assert(SuspendSave::encode(restored,copy)==n&&!memcmp(copy,data,n));
  for(int i=0;i<300;++i){if(g.rewardPending){g.reward(0);restored.motorways.reward(0);}assert(g.step()==restored.motorways.step());assert(g.valid()&&restored.motorways.valid());assert(g.rng==restored.motorways.rng&&g.closing==restored.motorways.closing);}
  // Original Micro Motorways payloads have no extension and still migrate.
  g.reset(4);s.motorways=g;n=SuspendSave::encode(s,data);
  std::vector<uint8_t> legacy(data,data+n-28);crc(legacy);
  assert(SuspendSave::decode(legacy.data(),legacy.size(),restored)&&!restored.motorways.systemTheme&&restored.motorways.closing==g.NONE&&restored.motorways.valid());
  // Unknown extension versions or malformed flags/lots never reach the live game.
  auto corrupt=std::vector<uint8_t>(data,data+n);corrupt[n-28]=99;crc(corrupt);assert(!SuspendSave::decode(corrupt.data(),corrupt.size(),restored));
  corrupt=std::vector<uint8_t>(data,data+n);corrupt[n-23]=2;crc(corrupt);assert(!SuspendSave::decode(corrupt.data(),corrupt.size(),restored));
  puts("Micro Motorways endless survival: 600 simulated weeks with dedicated feeders, congested-network failure, repeating renewals, full-map reuse, stalled-car recall, 64-bit time, classic default, and legacy/current saves passed.");
}
