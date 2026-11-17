#include "../MotorwaysGame.h"
#include "motorways_fixture.h"
#include "../SuspendSave.h"
#include <cassert>
#include <cstdio>
#include <vector>
using G=MotorwaysGame;
void route(G &g,uint8_t start,int direction,int count){g.cursor=start;assert(g.begin()==g.OK);for(int i=0;i<count;++i)assert(g.move(direction)==g.OK);assert(g.commit()==g.OK);}
void connectStart(G &g){route(g,g.homes[0].cell,1,4);route(g,g.homes[1].cell,1,4);}
int main(){
  G g;fixedStart(g,42);assert(g.valid()&&g.stock==32);auto original=g;
  // Preview freezes the world and never spends supplies; backtrack and cancel.
  assert(g.begin()==g.OK);assert(g.move(1)==g.OK);assert(g.move(1)==g.OK);assert(g.move(3)==g.OK&&g.planLength==2);
  for(int i=0;i<100;++i)assert(g.step()==0);assert(g.ticks==0&&g.stock==32);g.cancel();assert(!memcmp(g.roads,original.roads,g.N));
  connectStart(g);assert(g.stock==28);assert(g.nextHop(g.homes[0].cell,g.port(0))==G::cell(3,2));
  for(int i=0;i<500;++i){g.step();assert(g.valid());}assert(g.score>5);
  int active=0;for(auto &c:g.cars)active+=c.active;assert(active>0&&active<=4);
  // Erasing an occupied segment would strand a car, so it is rejected.
  for(auto &c:g.cars)if(c.active){assert(g.erase(c.from)==g.BUSY);break;}
  fixedStart(g,1);g.connect(G::cell(1,2),G::cell(2,2));assert(g.nextHop(G::cell(1,2),G::cell(3,2))==g.NONE);
  // Bridges cost one token per water tile, refunded only once on removal.
  fixedStart(g,2);auto supplies=g.stock;route(g,G::cell(9,4),1,2);assert(g.bridges==0&&g.stock==supplies-2&&g.valid());
  g.cursor=G::cell(10,4);g.begin();assert(g.move(2)==g.BLOCKED);g.cancel();
  assert(g.erase(G::cell(10,4))==g.OK&&g.bridges==1&&g.stock==supplies&&g.valid());
  g.bridges=0;g.cursor=G::cell(9,4);g.begin();g.move(1);g.move(1);auto roads=g;
  assert(g.commit()==g.NO_BRIDGE&&g.stock==supplies&&!memcmp(g.roads,roads.roads,g.N));g.cancel();
  fixedStart(g,3);g.stock=0;g.cursor=G::cell(3,2);g.begin();g.move(1);assert(g.commit()==g.NO_ROADS&&g.stock==0);g.cancel();
  // Express routes have explicit endpoints; their visual crossings aren't junctions.
  fixedStart(g,4);route(g,G::cell(8,4),1,1);route(g,G::cell(11,4),1,1);g.express=1;g.tool=g.EXPRESS;
  g.cursor=G::cell(9,4);assert(g.begin()==g.OK);g.cursor=G::cell(11,4);assert(g.commit()==g.OK&&g.linkCount==1&&g.express==0);
  assert(g.nextHop(G::cell(8,4),G::cell(12,4))==G::cell(9,4));assert(g.nextHop(G::cell(9,4),G::cell(12,4))==G::cell(11,4));
  assert(g.erase(G::cell(9,4))==g.OK&&g.linkCount==0&&g.express==1&&g.valid());
  // A neglected destination warns, then ends the run; no sudden instant loss.
  fixedStart(g,5);bool warned=false;for(int i=0;i<2000&&g.alive;++i){auto e=g.step();warned|=e&g.WARNING;}assert(warned&&!g.alive);
  // Weekly upgrade freezes demand until claimed and all three rewards work.
  fixedStart(g,6);connectStart(g);for(int i=0;i<1200;++i)g.step();assert(g.alive&&g.rewardPending&&g.week()==2);
  auto ticks=g.ticks;g.step();assert(g.ticks==ticks);auto stock=g.stock;g.reward(0);assert(g.stock==stock+g.roadReward()&&!g.rewardPending);
  g.rewardPending=true;g.reward(1);assert(g.bridges==2);g.rewardPending=true;g.reward(2);assert(g.express==1&&g.valid());
  // Grow varied cities across seeds, checking every graph/building/car invariant.
  for(int seed=1;seed<=30;++seed){fixedStart(g,seed,seed%3);connectStart(g);
    for(int i=0;i<4000;++i){for(int d=0;d<g.destCount;++d){g.destinations[d].demand=d==g.closing?0:1;g.destinations[d].clock=0;g.destinations[d].overload=0;}
      if(g.rewardPending)g.reward(seed%3);g.step();assert(g.valid());}
    assert(g.homeCount>2&&g.destCount>=3);
  }
  // Save round-trips preserve traffic, supply, growth RNG and pending upgrades.
  fixedStart(g,77);connectStart(g);for(int i=0;i<50;++i)g.step();int moving=0;for(auto &c:g.cars)moving+=c.active;assert(moving>0);g.rewardPending=true;
  SuspendState state;state.gameId=6;state.motorways=g;uint8_t bytes[2048],again[2048];size_t size=SuspendSave::encode(state,bytes);assert(size>400&&size<2048);
  SuspendState restored;assert(SuspendSave::decode(bytes,size,restored));assert(SuspendSave::encode(restored,again)==size&&!memcmp(bytes,again,size));
  g.rewardPending=false;restored.motorways.rewardPending=false;for(int i=0;i<150;++i){assert(g.step()==restored.motorways.step());assert(g.score==restored.motorways.score);}
  for(size_t n=0;n<size;++n)assert(!SuspendSave::decode(bytes,n,restored));
  // Even payloads with a valid CRC must survive strict semantic bounds checks.
  for(int i=0;i<1500;++i){std::vector<uint8_t> damaged(bytes,bytes+size);size_t p=44+g.random()%(size-44);damaged[p]^=uint8_t(1<<(g.random()%8));
    uint32_t crc=SuspendSave::checksum(damaged.data(),size);for(int b=0;b<4;++b)damaged[12+b]=crc>>(b*8);
    if(SuspendSave::decode(damaged.data(),size,restored))assert(restored.motorways.valid());}
  // The largest count-controlled payload remains comfortably inside NVS capacity.
  G full;full.homeCount=full.HOMES;full.destCount=full.DESTS;full.linkCount=full.LINKS;
  for(auto &car:full.cars)car.active=true;
  SuspendSave::Writer maximum{again,sizeof(again)};SuspendSave::writeMotorways(maximum,full);
  assert(maximum.good&&maximum.pos+44<SuspendSave::CAPACITY);
  printf("Micro Motorways: routing, traffic, supplies, bridges, express links, growth, failure, weekly rewards and %zu-byte saves passed.\n",size);
}
