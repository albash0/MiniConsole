#include "../MotorwaysGame.h"
#include "../SuspendSave.h"
#include "motorways_fixture.h"
#include <cassert>
#include <cstdio>
#include <set>
#include <vector>
using G=MotorwaysGame;
void buildOpening(G &g){
  int row=-1;uint16_t cost=g.openingCost(&row),before=g.stock;assert(row>=0&&cost<=g.stock-4);
  for(int i=0;i<2;++i){uint8_t parent[G::N],path[G::N];int count=0;
    assert(g.constructionPath(g.homes[i].cell,g.port(i),row,parent));
    for(uint8_t p=g.port(i);p!=g.homes[i].cell;p=parent[p])path[count++]=p;
    g.cursor=g.homes[i].cell;assert(g.begin()==g.OK);
    for(int j=count-1;j>=0;--j)assert(g.move(g.direction(g.cursor,path[j]))==g.OK);
    assert(g.commit()==g.OK&&g.valid());}
  assert(before-g.stock<=cost&&g.stock>=4);
}
void crc(std::vector<uint8_t> &data){
  uint32_t n=SuspendSave::checksum(data.data(),data.size());for(int i=0;i<4;++i)data[12+i]=n>>(8*i);
}
int main(){
  std::set<uint64_t> layouts,perDifficulty[3];unsigned colors=0,crossings=0,totalCost=0;
  for(int difficulty=0;difficulty<3;++difficulty)for(int seed=1;seed<=500;++seed){
    G g;g.reset(seed,difficulty);assert(g.valid()&&g.homeCount==2&&g.destCount==2&&g.stock==g.initialStock());
    G same;same.reset(seed,difficulty);assert(g.rng==same.rng&&!memcmp(g.homes,same.homes,sizeof(g.homes))&&!memcmp(g.destinations,same.destinations,sizeof(g.destinations)));
    assert(g.homes[0].color!=g.homes[1].color&&g.homes[0].color==g.destinations[0].color&&g.homes[1].color==g.destinations[1].color);
    uint64_t hash=1469598103934665603ull;bool crosses=false;
    for(int i=0;i<2;++i){assert(g.manhattan(g.homes[i].cell,g.port(i))>=4);colors|=1u<<g.homes[i].color;
      crosses|=(g.x(g.homes[i].cell)<10)!=(g.x(g.port(i))<10);
      for(auto n:{g.homes[i].cell,g.destinations[i].anchor,g.homes[i].color}){hash^=n;hash*=1099511628211ull;}}
    if(difficulty)assert(crosses);crossings+=crosses;
    for(auto road:g.roads)assert(!road);
    layouts.insert(hash);perDifficulty[difficulty].insert(hash);totalCost+=g.openingCost();buildOpening(g);
    assert(g.nextHop(g.homes[0].cell,g.port(0))!=g.NONE&&g.nextHop(g.homes[1].cell,g.port(1))!=g.NONE);
    // Check initial traffic really runs on the generated roads, without a free
    // third neighborhood or special resource assumptions.
    for(int i=0;i<160;++i){g.step();assert(g.alive&&g.valid());}
  }
  for(auto &set:perDifficulty)assert(set.size()>480);
  assert(layouts.size()>600&&colors==7&&crossings>=1000);
  // The first new destination fills the missing color after randomized starts.
  for(int seed=1;seed<=100;++seed){G g;g.reset(seed);assert(g.growDestination());unsigned mask=0;for(int i=0;i<3;++i)mask|=1u<<g.destinations[i].color;assert(mask==7);}
  G g;fixedStart(g);uint16_t initial=g.demandRate(0);g.ticks=11200;uint16_t peak=g.demandRate(0);g.ticks=10800;uint16_t quiet=g.demandRate(0);
  assert(peak<quiet&&g.junctionDelay()>6&&g.openingGrace()==200&&g.roadReward()==16&&g.overloadLimit()==120);
  assert(initial<120); // The old opening requested only one trip every 6 seconds.
  auto roads=g.stock;g.rewardPending=true;g.reward(0);assert(g.stock==roads+16);
  // A distant house must add capacity, rather than slow its entire color group.
  fixedStart(g);auto rate=g.demandRate(0);g.homeCount=3;g.homes[2]={g.cell(19,9),0,0};assert(g.demandRate(0)<=rate);
  // Existing v1 saves preserve the fraction of their longer warning timer.
  fixedStart(g);g.destinations[0].demand=7;g.destinations[0].overload=80;
  SuspendState state;state.gameId=6;state.motorways=g;uint8_t bytes[2048];size_t size=SuspendSave::encode(state,bytes);assert(size);
  std::vector<uint8_t> old(bytes,bytes+size);old[size-28]=1;crc(old);SuspendState restored;
  assert(SuspendSave::decode(old.data(),old.size(),restored)&&restored.motorways.destinations[0].overload==60);
  assert(restored.motorways.homes[0].cell==g.homes[0].cell&&restored.motorways.stock==g.stock);
  printf("Micro Motorways balance: 1500 seeded starts (%zu unique), %u crossings, average opening cost %.1f; playable budgets, color coverage, traffic, stronger rushes/junctions, scarce supplies and legacy warning migration passed.\n",layouts.size(),crossings,double(totalCost)/1500);
}
