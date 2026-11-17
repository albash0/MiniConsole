#pragma once
#include "PartnersGame.h"
#include "BlockBattle.h"
namespace PocketPartners {
constexpr int PACKET_SIZE=128;
struct Packet {BlockBattle::Packet link;Input input;Game world;};
inline void put16(uint8_t *b,int16_t v){b[0]=v;b[1]=uint16_t(v)>>8;}
inline int16_t get16(const uint8_t *b){return int16_t(uint16_t(b[0])|uint16_t(b[1])<<8);}
inline void encode(const Packet &p,uint8_t *b){
 memset(b,0,PACKET_SIZE);BlockBattle::encode(p.link,b);memcpy(b+48,"PP01",4);b[52]=p.world.level;
 b[53]=p.input.held;b[54]=p.input.jump;b[55]=p.input.pause;b[56]=p.input.retry;b[57]=p.input.quit;
 const Game &g=p.world;b[60]=g.stage;b[61]=g.key;b[62]=g.gate;b[63]=g.crateCount;put16(b+64,g.deaths);put16(b+66,g.gateGrace);BlockBattle::put32(b+68,g.ticks);
 for(int i=0;i<4;++i){const Body &v=i<2?g.players[i]:g.crates[i-2];int o=72+i*8;put16(b+o,v.x);put16(b+o+2,v.y);put16(b+o+4,v.vx);put16(b+o+6,v.vy);}
 b[104]=(g.players[0].grounded?1:0)|(g.players[1].grounded?2:0);BlockBattle::put32(b+124,BlockBattle::crc(b,124));
}
inline bool decode(const uint8_t *b,int size,Packet &p){
 if(size!=PACKET_SIZE||memcmp(b+48,"PP01",4)||!BlockBattle::decode(b,48,p.link)||BlockBattle::get32(b+124)!=BlockBattle::crc(b,124))return false;
 if(b[52]>=LEVELS||b[53]>7||b[60]>CLOSED||b[61]>1||b[62]>1||b[63]>2||b[104]>3||get16(b+66)<0||get16(b+66)>45)return false;
 p.input={b[53],b[54],b[55],b[56],b[57]};p.world.load(b[52]);Game &g=p.world;
 g.stage=Stage(b[60]);g.key=b[61];g.gate=b[62];if(g.crateCount!=b[63])return false;g.deaths=uint16_t(get16(b+64));g.gateGrace=get16(b+66);g.ticks=BlockBattle::get32(b+68);
 for(int i=0;i<4;++i){Body &v=i<2?g.players[i]:g.crates[i-2];int o=72+i*8;v.x=get16(b+o);v.y=get16(b+o+2);v.vx=get16(b+o+4);v.vy=get16(b+o+6);if(v.x<0||v.x>240*FP||v.y<0||v.y>160*FP||v.vx<-100||v.vx>100||v.vy<-100||v.vy>100)return false;}
 g.players[0].grounded=b[104]&1;g.players[1].grounded=b[104]&2;return true;
}
struct Session {
 BlockBattle::Match link;Game game,previous;Input inputs[2],consumed[2];
 uint32_t frameAt=0,interpolationAt=0;bool started=false;
 void open(bool host,uint32_t nonce,uint32_t seed,uint32_t now,uint8_t level){*this=Session{};link.open(host,nonce,seed,now);game.load(level);previous=game;frameAt=interpolationAt=now;}
 int local()const{return link.host?0:1;}
 bool receive(const uint8_t *mac,const Packet &p,uint32_t now){
  if(!link.receive(mac,p.link,now))return false;
  if(link.host)inputs[1]=p.input;
  else {
   bool discontinuity=game.level!=p.world.level||game.deaths!=p.world.deaths||game.stage!=p.world.stage||!started;
   if(discontinuity||game.ticks!=p.world.ticks){previous=discontinuity?p.world:game;interpolationAt=now;}game=p.world;
  }return true;
 }
 bool disconnected(uint32_t now)const{return link.outcome==BlockBattle::LINK_LOST;}
 bool stalled(uint32_t now)const{return link.paired&&now-link.lastHeard>250;}
 bool paused()const{return link.host?link.paused:link.peerPaused;}
 Packet packet(uint32_t now){Packet p;p.link=link.packet(now);p.input=inputs[local()];p.world=game;return p;}
 void update(uint32_t now){
  link.tick(now);
  if(link.startEvent){link.startEvent=false;started=true;frameAt=interpolationAt=now;consumed[0]=inputs[0];consumed[1]=inputs[1];previous=game;}
  if(!started||!link.host||link.phase!=BlockBattle::RUNNING||disconnected(now))return;
  if(stalled(now)){frameAt=now;return;}
  if(now-frameAt<33)return;if(now-frameAt>100)frameAt=now-33;frameAt+=33;interpolationAt=frameAt;previous=game;
  bool jumps[2]={false,false},pauseRequest=false,retryRequest=false,quitRequest=false;
  for(int i=0;i<2;++i){jumps[i]=inputs[i].jump!=consumed[i].jump;pauseRequest|=inputs[i].pause!=consumed[i].pause;retryRequest|=inputs[i].retry!=consumed[i].retry;quitRequest|=inputs[i].quit!=consumed[i].quit;consumed[i]=inputs[i];}
  if(quitRequest){game.stage=CLOSED;return;}
  if(pauseRequest){link.paused=!link.paused;return;}
  if(link.paused){if(jumps[0]||jumps[1])link.paused=false;else if(retryRequest){game.load(game.level,true);previous=game;link.paused=false;}return;}
  if(game.stage==FAILED&&(jumps[0]||jumps[1])){game.load(game.level,true);previous=game;return;}
  if((game.stage==CLEARED||game.stage==COMPLETE)&&(jumps[0]||jumps[1])){game.load(game.stage==COMPLETE?0:game.level+1,true);previous=game;return;}
  game.step(inputs,jumps);
 }
};
}
