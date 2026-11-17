#pragma once
#include <stdint.h>
#include <string.h>
#include "BlocksGame.h"

// Portable state-snapshot protocol. Cumulative attack totals survive loss,
// duplication and reordering; no individual attack datagram is fire-and-forget.
namespace BlockBattle {
constexpr int PACKET_SIZE=48;
enum Phase:uint8_t { SEEK,LOBBY,COUNTDOWN,RUNNING,ENDED };
enum Outcome:uint8_t { NONE,WIN,LOSS,DRAW,LINK_LOST };
struct Packet {
  uint32_t session=0,nonce=0,peerNonce=0,seed=0,sequence=0,attacks=0,elapsed=0;
  uint16_t remaining=0;uint8_t phase=SEEK,height=0,flags=0,speed=1;
};
constexpr uint8_t HOST=1,READY=2,PAUSED=4,DEAD=8;
inline uint32_t crc(const uint8_t *p,int n){uint32_t h=2166136261u;for(int i=0;i<n;++i)h=(h^p[i])*16777619u;return h;}
inline void put32(uint8_t *p,uint32_t v){for(int i=0;i<4;++i)p[i]=v>>(8*i);}
inline uint32_t get32(const uint8_t *p){uint32_t v=0;for(int i=0;i<4;++i)v|=uint32_t(p[i])<<(8*i);return v;}
inline void encode(const Packet &p,uint8_t *b){
 memset(b,0,PACKET_SIZE);memcpy(b,"MCBB",4);b[4]=1;b[5]=p.phase;b[6]=p.flags;b[7]=p.height;
 const uint32_t fields[]={p.session,p.nonce,p.peerNonce,p.seed,p.sequence,p.attacks,p.elapsed};for(int i=0;i<7;++i)put32(b+8+i*4,fields[i]);
 b[36]=p.remaining;b[37]=p.remaining>>8;b[38]=p.speed;put32(b+44,crc(b,44));
}
inline bool decode(const uint8_t *b,int n,Packet &p){
 if(n!=PACKET_SIZE||memcmp(b,"MCBB",4)||b[4]!=1||get32(b+44)!=crc(b,44)||b[5]>ENDED||b[6]>15||b[7]>20||b[38]>2)return false;
 for(int i=39;i<44;++i)if(b[i])return false;
 p.phase=b[5];p.flags=b[6];p.height=b[7];uint32_t *fields[]={&p.session,&p.nonce,&p.peerNonce,&p.seed,&p.sequence,&p.attacks,&p.elapsed};
 for(int i=0;i<7;++i)*fields[i]=get32(b+8+i*4);p.remaining=uint16_t(b[36])|uint16_t(b[37])<<8;p.speed=b[38];return p.nonce&&p.remaining<=3000;
}
struct Match {
 bool host=false,paired=false,ready=false,peerReady=false,paused=false,peerPaused=false,dead=false,peerDead=false,startEvent=false;
 Phase phase=SEEK;Outcome outcome=NONE;
 uint8_t peer[6]{},height=0,peerHeight=0,speed=1;
 uint32_t session=0,nonce=1,peerNonce=0,seed=1,sequence=0,peerSequence=0,sent=0,seen=0,lastHeard=0,deadline=0,started=0,deadAt=0;
 uint16_t pending=0;
 void open(bool isHost,uint32_t token,uint32_t bagSeed,uint32_t now,uint8_t difficulty=1){
  *this=Match{};host=isHost;nonce=token?token:1;session=host?nonce:0;seed=bagSeed?bagSeed:1;speed=difficulty;phase=host?LOBBY:SEEK;lastHeard=now;
 }
 bool samePeer(const uint8_t *mac)const{return paired&&!memcmp(peer,mac,6);}
 bool stale(uint32_t now)const{return paired&&now-lastHeard>1500;}
 bool frozen(uint32_t now)const{return phase!=RUNNING||dead||paused||peerPaused||stale(now);}
 bool receive(const uint8_t *mac,const Packet &p,uint32_t now){
  if(p.nonce==nonce)return false;
  if(!paired){
   if(host){if((p.flags&HOST)||p.session!=session||p.peerNonce!=nonce||p.phase!=LOBBY)return false;}
   else {if(!(p.flags&HOST)||p.phase!=LOBBY||!p.session||!p.seed||p.peerNonce)return false;session=p.session;seed=p.seed;speed=p.speed;phase=LOBBY;}
   memcpy(peer,mac,6);peerNonce=p.nonce;paired=true;peerSequence=p.sequence;lastHeard=now;
  }else{
   if(!samePeer(mac)||p.session!=session||p.nonce!=peerNonce||p.peerNonce!=nonce||bool(p.flags&HOST)==host)return false;
   if(int32_t(p.sequence-peerSequence)<=0)return false;peerSequence=p.sequence;lastHeard=now;
  }
  peerReady=p.flags&READY;peerPaused=p.flags&PAUSED;peerDead=p.flags&DEAD;peerHeight=p.height;
  if(!host&&p.phase==COUNTDOWN&&phase==LOBBY){phase=COUNTDOWN;deadline=now+p.remaining;}
  if(!host&&p.phase==RUNNING&&(phase==LOBBY||phase==COUNTDOWN)){phase=RUNNING;started=now;startEvent=true;}
  if(phase==RUNNING||(phase==ENDED&&outcome!=LINK_LOST)){
   if(p.attacks>=seen){uint32_t delta=p.attacks-seen;if(delta>200)return false;pending=uint16_t(pending+delta>200?200:pending+delta);seen=p.attacks;}
   if(peerDead){outcome=dead?DRAW:WIN;phase=ENDED;}
  }
  return true;
 }
 void tick(uint32_t now){
  if(phase!=ENDED&&paired&&now-lastHeard>8000){outcome=LINK_LOST;phase=ENDED;return;}
  if(host&&phase==LOBBY&&paired&&ready&&peerReady){phase=COUNTDOWN;deadline=now+3000;}
  if(host&&phase==COUNTDOWN&&int32_t(now-deadline)>=0){
   if(now-lastHeard<1500){phase=RUNNING;started=now;startEvent=true;}
   else deadline=now+1000;
  }
  if(dead&&phase==RUNNING&&now-deadAt>=700){outcome=peerDead?DRAW:LOSS;phase=ENDED;}
 }
 Packet packet(uint32_t now){Packet p;p.session=session;p.nonce=nonce;p.peerNonce=peerNonce;p.seed=seed;p.sequence=++sequence;p.attacks=sent;p.phase=phase;p.height=height;p.speed=speed;
  p.flags=(host?HOST:0)|(ready?READY:0)|(paused?PAUSED:0)|(dead?DEAD:0);p.remaining=phase==COUNTDOWN&&int32_t(deadline-now)>0?uint16_t(deadline-now):0;p.elapsed=phase==RUNNING?now-started:0;return p;}
 void topOut(uint32_t now){if(!dead){dead=true;deadAt=now;}paused=false;}
 uint8_t clear(uint8_t lines){static constexpr uint8_t attack[]={0,0,1,2,4};uint8_t n=attack[lines<=4?lines:0];uint16_t cancel=n<pending?n:pending;pending-=cancel;n-=cancel;sent+=n;return n;}
};
inline uint8_t stackHeight(const BlocksGame &g){for(int y=0;y<20;++y)for(int x=0;x<10;++x)if(g.board[y][x])return 20-y;return 0;}
// Garbage RNG is independent of the shared piece bag.
inline bool garbage(BlocksGame &g,uint16_t lines,uint32_t &rng){
 if(!g.alive||!lines)return g.alive;
 if(lines>20){g.alive=false;return false;}
 for(int y=0;y<lines;++y)for(int x=0;x<10;++x)if(g.board[y][x]){g.alive=false;return false;}
 for(int y=0;y<20-lines;++y)memcpy(g.board[y],g.board[y+lines],10);
 for(int y=20-lines;y<20;++y){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;int hole=rng%10;for(int x=0;x<10;++x)g.board[y][x]=x==hole?0:8;}
 if(!g.fits(g.x,g.y,g.rotation))g.alive=false;return g.alive;
}
}
