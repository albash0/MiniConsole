#include "../PartnersLink.h"
#include <cassert>
#include <cstdio>
#include "partners_solutions.h"
using namespace PocketPartners;
void ticks(Game &g,int n,Input a={},Input b={}){Input in[]={a,b};bool jump[]={false,false};while(n--)g.step(in,jump);}
void jump(Game &g,int i){Input in[2]{};bool j[2]{};j[i]=true;g.step(in,j);}
int main(){
 for(int i=0;i<LEVELS;++i){Game g;g.load(i);int key=0,exit=0;for(auto &row:g.tiles)for(auto t:row){key+=t==KEY;exit+=t==EXIT;}assert(key==1&&exit==1);ticks(g,20);assert(g.stage==PLAY);for(auto &p:g.players)assert(p.grounded);}
 // Every room is beatable with real two-player input, including both exits.
 for(int level=0;level<LEVELS;++level){Game v;v.load(level);for(const char *p=PARTNERS_SOLUTIONS[level];*p;++p){int action=*p<='9'?*p-'0':*p-'a'+10;int who=action/6,a=action%6;Input in[2]{};in[who].held=(a%3==0?MOVE_LEFT:a%3==2?MOVE_RIGHT:0)|PUSH;bool j[2]{};j[who]=a>=3;for(int n=0;n<4;++n){v.step(in,j);j[who]=false;}}assert(v.stage==(level==LEVELS-1?COMPLETE:CLEARED));}
 Game g;g.load(0);int y=g.players[0].y;jump(g,0);int top=g.players[0].y;for(int i=0;i<45;++i){ticks(g,1);if(g.players[0].y<top)top=g.players[0].y;}assert(y-top>26*FP&&y-top<32*FP&&g.players[0].grounded);
 // Land on a partner and be carried; no penetration, even at a wall.
 g.load(0);g.players[1].x=80*FP;g.players[0].x=80*FP;g.players[0].y=50*FP;ticks(g,35);assert(g.players[0].support==1&&g.players[0].y+PH*FP==g.players[1].y);
 int x=g.players[0].x;ticks(g,10,{},Input{MOVE_RIGHT});assert(g.players[0].x>x&&g.players[0].y+PH*FP==g.players[1].y);jump(g,0);assert(g.players[0].vy<0);
 // Crates only move while interacting. A crate can keep a plate down.
 g.load(3);g.players[0].x=76*FP;g.players[1].x=24*FP;ticks(g,25,Input{MOVE_RIGHT});assert(g.crates[0].x==84*FP);ticks(g,20,Input{uint8_t(MOVE_RIGHT|PUSH)});assert(g.crates[0].x>110*FP&&g.gate);ticks(g,100);assert(g.gate);
 // Both weights required in room six, gate cannot close through a player.
 g.load(5);g.crates[0].x=72*FP;g.switches();assert(!g.gate);g.crates[1].x=120*FP;g.switches();assert(g.gate);g.crates[0].x=60*FP;g.crates[1].x=108*FP;g.players[0].x=168*FP;ticks(g,70);assert(g.gate);
 // A fall restarts both; collecting key is shared; one player alone cannot win.
 g.load(4);g.players[0].x=120*FP;ticks(g,15);assert(g.stage==FAILED&&g.deaths==1);g.load(0,true);assert(g.deaths==1);
 g.players[0].x=120*FP;ticks(g,1);assert(g.key);g.players[0].x=208*FP;ticks(g,1);assert(g.stage==PLAY);g.players[1].x=220*FP;ticks(g,1);assert(g.stage==CLEARED);
 // Host-authoritative link, input counters survive loss/duplicates, pause/retry/next.
 Session h,j;h.open(true,123,77,0,0);j.open(false,456,66,0,0);uint8_t hm[]={1,2,3,4,5,6},jm[]={2,3,4,5,6,7};
 auto deliver=[&](Session &a,Session &b,uint8_t *mac,uint32_t t){uint8_t data[PACKET_SIZE];encode(a.packet(t),data);Packet p;assert(decode(data,sizeof(data),p));return b.receive(mac,p,t);};
 assert(deliver(h,j,hm,10));assert(deliver(j,h,jm,20));assert(deliver(h,j,hm,30));h.link.ready=j.link.ready=true;deliver(j,h,jm,40);h.update(40);deliver(h,j,hm,50);deliver(j,h,jm,3030);h.update(3041);deliver(h,j,hm,3042);j.update(3042);assert(h.started&&j.started);
 j.inputs[1].held=MOVE_RIGHT;j.inputs[1].jump++;deliver(j,h,jm,3050);h.update(3080);deliver(h,j,hm,3081);assert(h.game.players[1].vy<0&&j.game.players[1].y==h.game.players[1].y);
 uint32_t frame=h.game.ticks;j.inputs[1].pause++;deliver(j,h,jm,3100);h.update(3120);assert(h.paused());deliver(h,j,hm,3121);assert(j.paused());h.update(3160);assert(h.game.ticks==frame);
 j.inputs[1].retry++;deliver(j,h,jm,3190);h.update(3200);assert(!h.paused()&&!h.game.ticks);h.update(3700);assert(h.game.ticks==0); // missing peer freezes simulation early
 auto p=h.packet(3800);uint8_t data[PACKET_SIZE];encode(p,data);Packet decoded;assert(decode(data,sizeof(data),decoded));for(int i=0;i<PACKET_SIZE;++i){data[i]^=1;assert(!decode(data,sizeof(data),decoded));data[i]^=1;}
 h.update(12000);assert(h.disconnected(12000));
 puts("Pocket Partners: ten rooms, jump/stack/carry, crates/plates/gates, shared key/exit/death, packet bounds/CRC, pairing, authoritative physics, reliable input, pause/retry and link loss passed.");
}
