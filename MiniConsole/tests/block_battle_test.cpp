#include "../BlockBattle.h"
#include <cassert>
#include <cstdio>
using namespace BlockBattle;
int main(){
 uint8_t hm[]={1,2,3,4,5,6},jm[]={2,3,4,5,6,7};Match h,j;h.open(true,123,999,0);j.open(false,456,42,0);
 uint8_t bytes[PACKET_SIZE];auto offer=h.packet(1);encode(offer,bytes);Packet decoded;assert(decode(bytes,sizeof(bytes),decoded));
 for(int i=0;i<PACKET_SIZE;++i){bytes[i]^=1;assert(!decode(bytes,sizeof(bytes),decoded));bytes[i]^=1;}
 assert(j.receive(hm,offer,10)&&j.paired&&j.seed==999);auto join=j.packet(20);assert(h.receive(jm,join,30));assert(j.receive(hm,h.packet(40),50));
 h.ready=j.ready=true;h.receive(jm,j.packet(60),70);h.tick(70);assert(h.phase==COUNTDOWN);j.receive(hm,h.packet(80),90);assert(j.phase==COUNTDOWN);
 h.receive(jm,j.packet(3050),3055);h.tick(3071);assert(h.phase==RUNNING&&h.startEvent);j.receive(hm,h.packet(3080),3090);assert(j.phase==RUNNING&&j.startEvent);
 // Repeated/lost snapshots cannot drop or duplicate attacks.
 assert(h.clear(4)==4);auto first=h.packet(3100);assert(j.receive(hm,first,3110)&&j.pending==4);assert(!j.receive(hm,first,3120)&&j.pending==4);
 h.clear(3);h.packet(3130);auto later=h.packet(3140);assert(j.receive(hm,later,3150)&&j.pending==6);assert(!j.receive(hm,first,3160)&&j.pending==6);
 assert(j.clear(4)==0&&j.pending==2);assert(j.clear(3)==0&&j.pending==0);assert(j.clear(2)==1);
 auto unrelated=later;unrelated.session++;unrelated.sequence++;assert(!j.receive(hm,unrelated,3161));
 j.paused=true;h.receive(jm,j.packet(3200),3210);assert(h.frozen(3210));j.paused=false;h.receive(jm,j.packet(3300),3310);assert(!h.frozen(3310));
 assert(h.stale(5000)&&h.frozen(5000));h.tick(12000);assert(h.outcome==LINK_LOST&&h.phase==ENDED);
 // Topout and simultaneous topout are explicit, retransmittable state.
 j.topOut(3400);j.tick(4100);assert(j.outcome==LOSS);auto p=j.packet(4110);Match winner;winner.open(true,123,999,0);winner.paired=true;memcpy(winner.peer,jm,6);winner.peerNonce=456;winner.phase=RUNNING;
 assert(winner.receive(jm,p,4120)&&winner.outcome==WIN);
 winner.tick(30000);assert(winner.outcome==WIN);j.tick(30000);assert(j.outcome==LOSS);
 BlocksGame a,b;a.reset(999);b.reset(999);uint32_t rng=12,bag=a.randomState;assert(garbage(a,3,rng));assert(a.randomState==bag&&a.next==b.next);
 for(int y=17;y<20;++y){int holes=0;for(int x=0;x<10;++x)holes+=a.board[y][x]==0;assert(holes==1);}
 a.board[0][0]=1;assert(!garbage(a,1,rng)&&!a.alive);
 puts("Block battle: pairing/seed, CRC/bounds, ready/countdown, packet loss/duplicates/reordering, attacks/cancellation, pause/disconnect, results and garbage passed.");
}
