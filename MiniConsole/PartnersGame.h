#pragma once
#include <stdint.h>
#include <string.h>

// Shared, fixed-step world. Positions are sixteenths of a pixel, 33 ms per tick.
namespace PocketPartners {
constexpr int W=20,H=9,TILE=12,FP=16,PW=8,PH=10,LEVELS=10;
enum Tile:uint8_t { AIR,WALL,SPIKE,KEY,EXIT,PLATE,GATE };
enum Stage:uint8_t { PLAY,FAILED,CLEARED,COMPLETE,CLOSED };
enum Control:uint8_t { MOVE_LEFT=1,MOVE_RIGHT=2,PUSH=4 };
struct Body {int16_t x=0,y=0,vx=0,vy=0;bool grounded=false;int8_t support=-1;};
struct Input {uint8_t held=0,jump=0,pause=0,retry=0,quit=0;};
struct Game {
 Body players[2],crates[2];uint8_t tiles[H][W]{};
 uint8_t level=0,crateCount=0;Stage stage=PLAY;bool key=false,gate=false;
 uint8_t coyote[2]{},jumpBuffer[2]{};
 uint16_t deaths=0,gateGrace=0;uint32_t ticks=0;
 static const char *name(int i){static const char *n[]={"Meet your partner","A little boost","Switch relay","Heavy lifting","Leap together","Two weights","Up and over","The high road","Mind the gap","Better together"};return n[i%LEVELS];}
 static const char *hint(int i){static const char *h[]={"Find the key. Both reach the door.","Stand on your partner for a boost.","Hold either plate for your partner.","Hold B and walk to push the crate.","Release A between jumps.","Put a crate on each pressure plate.","Climb the steps. Share the key.","A partner makes a good step.","Leave the crate on the plate.","Get the high key, then meet at exit."};return h[i%LEVELS];}
 void tile(int x,int y,Tile t){if(x>=0&&x<W&&y>=0&&y<H)tiles[y][x]=t;}
 void platform(int x,int y,int n){for(int i=0;i<n;++i)tile(x+i,y,WALL);}
 void barrier(int x){tile(x,6,GATE);tile(x,7,GATE);}
 void crate(int x){Body &b=crates[crateCount++];b=Body{};b.x=x*TILE*FP;b.y=(96-TILE)*FP;}
 void load(uint8_t n,bool keepDeaths=false){
  uint16_t d=keepDeaths?deaths:0;*this=Game{};deaths=d;level=n<LEVELS?n:0;
  platform(0,8,W);for(int y=0;y<8;++y){tile(0,y,WALL);tile(19,y,WALL);}
  players[0].x=24*FP;players[1].x=40*FP;for(auto &p:players){p.y=86*FP;p.grounded=true;}
  tile(18,7,EXIT);tile(10,7,KEY);
  switch(level){
   case 1:tile(10,7,AIR);platform(8,5,4);tile(10,4,KEY);break;
   case 2:tile(6,7,PLATE);tile(15,7,PLATE);barrier(13);break;
   case 3:tile(10,7,PLATE);tile(16,7,KEY);barrier(14);crate(7);break;
   case 4:for(int x=9;x<=11;++x)tile(x,8,SPIKE);tile(10,7,AIR);tile(16,7,KEY);break;
   case 5:tile(6,7,PLATE);tile(10,7,PLATE);tile(16,7,KEY);barrier(14);crate(5);crate(9);break;
   case 6:tile(10,7,AIR);platform(6,6,3);platform(10,5,3);platform(13,3,3);tile(14,2,KEY);break;
   case 7:tile(10,7,AIR);platform(6,6,3);platform(10,4,4);tile(12,3,KEY);break;
   case 8:tile(10,7,AIR);tile(6,7,PLATE);crate(5);barrier(9);for(int x=12;x<=13;++x)tile(x,8,SPIKE);tile(16,7,KEY);break;
   case 9:tile(10,7,AIR);platform(6,6,3);platform(9,3,4);tile(10,2,KEY);tile(4,7,PLATE);tile(17,7,PLATE);barrier(15);crate(5);break;
  }
 }
 static bool overlap(int x,int y,int w,int h,int a,int b,int aw,int ah){return x<a+aw&&x+w>a&&y<b+ah&&y+h>b;}
 bool solid(int x,int y)const{if(x<0||x>=W||y<0)return true;if(y>=H)return false;return tiles[y][x]==WALL||(tiles[y][x]==GATE&&!gate);}
 bool terrain(int x,int y,int w,int h)const{
  for(int r=y/(TILE*FP);r<=(y+h-1)/(TILE*FP);++r)for(int c=x/(TILE*FP);c<=(x+w-1)/(TILE*FP);++c)if(solid(c,r))return true;
  return x<TILE*FP||x+w>19*TILE*FP||y<0;
 }
 bool crateFits(int id,int x,int y)const{
  if(terrain(x,y,TILE*FP,TILE*FP))return false;
  for(int i=0;i<crateCount;++i)if(i!=id&&overlap(x,y,TILE*FP,TILE*FP,crates[i].x,crates[i].y,TILE*FP,TILE*FP))return false;
  for(const auto &p:players)if(overlap(x,y,TILE*FP,TILE*FP,p.x,p.y,PW*FP,PH*FP))return false;
  return true;
 }
 bool blocked(int id,int x,int y,bool push,int dx){
  if(terrain(x,y,PW*FP,PH*FP))return true;
  for(int i=0;i<crateCount;++i)if(overlap(x,y,PW*FP,PH*FP,crates[i].x,crates[i].y,TILE*FP,TILE*FP)){
   if(!push||!dx||!crateFits(i,crates[i].x+dx,crates[i].y))return true;
   crates[i].x+=dx;
  }
  const auto &p=players[1-id];return overlap(x,y,PW*FP,PH*FP,p.x,p.y,PW*FP,PH*FP);
 }
 static int clamp(int v,int lo,int hi){return v<lo?lo:v>hi?hi:v;}
 void horizontal(int id,int dx,bool push){
  Body &p=players[id];int start=p.x;
  while(dx){int step=clamp(dx,-FP,FP);if(blocked(id,p.x+step,p.y,push,step))break;p.x+=step;dx-=step;}
  // A rider is carried unless a ceiling or wall prevents it.
  Body &r=players[1-id];if(r.support==id){int d=p.x-start;if(!terrain(r.x+d,r.y,PW*FP,PH*FP)){bool ok=true;for(int i=0;i<crateCount;++i)if(overlap(r.x+d,r.y,PW*FP,PH*FP,crates[i].x,crates[i].y,TILE*FP,TILE*FP))ok=false;if(ok)r.x+=d;}}
 }
 void vertical(int id){
  Body &p=players[id];p.grounded=false;p.support=-1;int dy=p.vy;
  while(dy){int step=clamp(dy,-FP,FP);if(blocked(id,p.x,p.y+step,false,0)){if(step>0){p.grounded=true;const auto &q=players[1-id];if(p.y+PH*FP<=q.y&&p.y+PH*FP+FP>=q.y)p.support=1-id;}p.vy=0;break;}p.y+=step;dy-=step;}
 }
 bool touches(const Body &p,int x,int y)const{return overlap(p.x,p.y,PW*FP,PH*FP,x*TILE*FP,y*TILE*FP,TILE*FP,TILE*FP);}
 bool pressure(int x,int y)const{
  int a=x*TILE*FP,b=(y+1)*TILE*FP;
  for(const auto &p:players)if(p.x+PW*FP>a&&p.x<a+TILE*FP&&p.y+PH*FP>=b-FP&&p.y+PH*FP<=b+FP)return true;
  for(int i=0;i<crateCount;++i){const auto &c=crates[i];if(c.x+TILE*FP>a&&c.x<a+TILE*FP&&c.y+TILE*FP>=b-FP&&c.y+TILE*FP<=b+FP)return true;}
  return false;
 }
 void switches(){
  int total=0,down=0;for(int y=0;y<H;++y)for(int x=0;x<W;++x)if(tiles[y][x]==PLATE){++total;down+=pressure(x,y);}
  bool active=total&&(level==5?down==total:down>0);if(active)gateGrace=45;else if(gateGrace)--gateGrace;
  bool occupied=false;for(int y=0;y<H;++y)for(int x=0;x<W;++x)if(tiles[y][x]==GATE){for(const auto &p:players)occupied|=touches(p,x,y);for(int i=0;i<crateCount;++i)occupied|=overlap(crates[i].x,crates[i].y,TILE*FP,TILE*FP,x*TILE*FP,y*TILE*FP,TILE*FP,TILE*FP);}
  gate=gateGrace||occupied; // Never close a gate through a player or crate.
 }
 void step(const Input in[2],const bool jump[2]){
  if(stage!=PLAY)return;++ticks;switches();
  for(int i=0;i<2;++i){auto &p=players[i];if(p.grounded)coyote[i]=4;else if(coyote[i])--coyote[i];if(jump[i])jumpBuffer[i]=4;else if(jumpBuffer[i])--jumpBuffer[i];if(jumpBuffer[i]&&coyote[i]){p.vy=-69;p.grounded=false;p.support=-1;coyote[i]=jumpBuffer[i]=0;}p.vx=((in[i].held&MOVE_RIGHT)?32:0)-((in[i].held&MOVE_LEFT)?32:0);horizontal(i,p.vx,in[i].held&PUSH);}
  // Bottom player moves first so a rider cannot be pushed inside them.
  int first=players[0].y>=players[1].y?0:1;
  for(int n=0;n<2;++n){int i=n?1-first:first;players[i].vy=clamp(players[i].vy+5,-80,80);vertical(i);}
  for(int i=0;i<crateCount;++i){auto &c=crates[i];c.vy=clamp(c.vy+5,0,80);int dy=c.vy;while(dy){int s=clamp(dy,0,FP);if(!crateFits(i,c.x,c.y+s)){c.vy=0;break;}c.y+=s;dy-=s;}}
  for(int i=0;i<crateCount;++i)if(crates[i].y>112*FP)stage=FAILED;
  switches();bool atExit[2]={false,false};
  for(int i=0;i<2;++i){const auto &p=players[i];if(p.y>112*FP)stage=FAILED;
   for(int y=0;y<H;++y)for(int x=0;x<W;++x){if(tiles[y][x]==KEY&&touches(p,x,y))key=true;if(tiles[y][x]==SPIKE&&touches(p,x,y))stage=FAILED;if(tiles[y][x]==EXIT&&touches(p,x,y))atExit[i]=true;}}
  // Both can occupy the doorway vertically (stacked) or side by side nearby.
  for(int i=0;i<2;++i)if(overlap(players[i].x,players[i].y,PW*FP,PH*FP,207*FP,79*FP,21*FP,17*FP))atExit[i]=true;
  if(stage==FAILED){++deaths;return;}if(key&&atExit[0]&&atExit[1])stage=level==LEVELS-1?COMPLETE:CLEARED;
 }
};
}
