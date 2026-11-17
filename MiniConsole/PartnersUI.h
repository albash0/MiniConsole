#pragma once
namespace PartnersUI {
#ifdef MINICONSOLE_TFT114
using TFTUI::c;using TFTUI::small;using TFTUI::center;using namespace PocketPartners;
constexpr uint16_t CORAL=0xFAAC,CYAN=0x4E7F,GOLD=0xFE48;
void person(int x,int y,int id,bool you,uint32_t t,bool moving){
 uint16_t color=id?CYAN:CORAL;
 c().fillRoundRect(x,y,8,9,2,color);c().fillRect(x+1,y+8,2,2,color);c().fillRect(x+5,y+8,2,2,color);
 c().drawPixel(x+2,y+3,0x0841);c().drawPixel(x+5,y+3,0x0841);
 if(moving)c().drawPixel(x+((t/5)&1?1:6),y+9,TFTUI::BG);
 if(you)c().fillTriangle(x+2,y-4,x+6,y-4,x+4,y-2,color);
}
void overlay(const char *title,const char *line,const char *bottom){
 c().fillRoundRect(15,35,210,69,8,TFTUI::BG);c().drawRoundRect(15,35,210,69,8,TFTUI::ACCENT);
 center(56,title,TFTUI::ACCENT,&FreeSansBold12pt7b);center(77,line,TFTUI::TEXT,nullptr);center(93,bottom,TFTUI::MUTED,nullptr);
}
void lobby(uint32_t now){
 TFTUI::heading("POCKET PARTNERS");
 if(partnersChoosing){
  c().fillRoundRect(9,35,107,29,6,partnersHost?TFTUI::PANEL:TFTUI::BG);c().fillRoundRect(124,35,107,29,6,!partnersHost?TFTUI::PANEL:TFTUI::BG);
  TFTUI::text(29,55,"Host",partnersHost?TFTUI::ACCENT:TFTUI::MUTED);TFTUI::text(149,55,"Join",!partnersHost?TFTUI::ACCENT:TFTUI::MUTED);
  char b[44];snprintf(b,sizeof(b),"Room %u: %s",partnersLevel+1,Game::name(partnersLevel));center(83,b,TFTUI::TEXT,nullptr);
  center(104,partnersError?"Radio failed. A to retry.":"L/R: role    UP/DOWN: room",partnersError?TFTUI::AMBER:TFTUI::MUTED,nullptr);
  TFTUI::footer("A: OPEN     B: LIBRARY");return;
 }
 person(48,40,0,partners.link.host,now,false);person(184,40,1,!partners.link.host,now,false);
 char b[32];snprintf(b,sizeof(b),"ROOM %04u",unsigned(partners.link.session%10000));center(53,partners.link.session?b:"Searching...",TFTUI::ACCENT,&FreeSansBold12pt7b);
 if(partners.link.phase==BlockBattle::COUNTDOWN){uint32_t left=int32_t(partners.link.deadline-now)>0?partners.link.deadline-now:0;snprintf(b,sizeof(b),"Starting in %lu",(unsigned long)((left+999)/1000));center(83,b,TFTUI::GREEN);center(106,"A: jump   B: push   UP: pause",TFTUI::MUTED,nullptr);}
 else {center(79,partners.link.paired?"Partner connected":"Waiting for partner",TFTUI::TEXT);snprintf(b,sizeof(b),"YOU: %s    THEM: %s",partners.link.ready?"READY":"WAIT",partners.link.peerReady?"READY":"WAIT");center(103,b,TFTUI::MUTED,nullptr);}
 TFTUI::footer("A: READY     B: BACK");
}
bool render(uint32_t now){
 if(screen!=PARTNERS_ROOM)return false;
 TFTUI::usePalette(matchGameTheme?menuTheme:UITheme::preset(0));c().setRotation((flipped?2:0)&3);c().fillScreen(TFTUI::BG);
 if(partnersChoosing||!partners.started){lobby(now);if(partners.disconnected(now))overlay("LINK LOST","Move closer and try again","B: library");return true;}
 const Game &g=partners.game;const Game &prev=partners.previous;char text[44];
 snprintf(text,sizeof(text),"%02u/10  %s",g.level+1,Game::name(g.level));small(5,3,text,TFTUI::TEXT);
 snprintf(text,sizeof(text),"YOU: %s   %s",partners.local()?"CYAN":"CORAL",g.key?"KEY FOUND":"FIND THE KEY");small(5,14,text,g.key?TFTUI::GREEN:TFTUI::MUTED);
 for(int y=0;y<H;++y)for(int x=0;x<W;++x){int px=x*TILE,py=23+y*TILE;uint8_t t=g.tiles[y][x];
  if(t==WALL){c().fillRect(px,py,12,12,TFTUI::PANEL);c().drawFastHLine(px,py,12,TFTUI::BORDER);c().drawPixel(px+3,py+7,TFTUI::BORDER);c().drawPixel(px+9,py+4,TFTUI::BORDER);}
  else if(t==SPIKE){for(int i=0;i<3;++i)c().fillTriangle(px+i*4,py+10,px+i*4+2,py+3,px+i*4+4,py+10,CORAL);}
  else if(t==KEY&&!g.key){int bob=(now/240)&1;c().drawCircle(px+4,py+5-bob,3,GOLD);c().drawFastHLine(px+6,py+5-bob,6,GOLD);c().drawFastVLine(px+10,py+5-bob,4,GOLD);}
  else if(t==EXIT){c().fillRoundRect(207,102,21,17,4,TFTUI::PANEL);c().drawRoundRect(207,102,21,17,4,g.key?TFTUI::GREEN:TFTUI::BORDER);c().drawCircle(218,110,2,g.key?TFTUI::GREEN:TFTUI::MUTED);}
  else if(t==PLATE){bool on=g.pressure(x,y);c().fillRoundRect(px,py+(on?11:9),12,on?1:3,1,on?TFTUI::GREEN:GOLD);}
  else if(t==GATE){if(g.gate){c().drawPixel(px+3,py,TFTUI::MUTED);c().drawPixel(px+8,py+6,TFTUI::MUTED);}else{for(int i=2;i<12;i+=4)c().fillRect(px+i,py,2,12,GOLD);c().drawFastHLine(px,py,12,GOLD);}}
 }
 uint32_t mix=now-partners.interpolationAt;if(mix>33)mix=33;
 for(int i=0;i<g.crateCount;++i){int x=(prev.crates[i].x+(g.crates[i].x-prev.crates[i].x)*int(mix)/33)/FP,y=23+(prev.crates[i].y+(g.crates[i].y-prev.crates[i].y)*int(mix)/33)/FP;c().fillRoundRect(x,y,12,12,2,GOLD);c().drawRect(x+2,y+2,8,8,0x9B20);c().drawLine(x+3,y+3,x+8,y+8,0x9B20);c().drawLine(x+8,y+3,x+3,y+8,0x9B20);}
 for(int i=0;i<2;++i){int x=(prev.players[i].x+(g.players[i].x-prev.players[i].x)*int(mix)/33)/FP,y=23+(prev.players[i].y+(g.players[i].y-prev.players[i].y)*int(mix)/33)/FP;person(x,y,i,i==partners.local(),g.ticks,g.players[i].vx!=0);}
 if(partners.disconnected(now))overlay("LINK LOST","Partner disconnected","B: library");
 else if(partnersClosing)overlay("SEE YOU!","Returning to library","");
 else if(partners.stalled(now))overlay("RECONNECTING","Both games are frozen","Keep consoles nearby");
 else if(partners.paused())overlay("PAUSED","A: resume    DOWN: retry","B: leave together");
 else if(g.stage==FAILED)overlay("TRY TOGETHER","A: restart this room","UP: pause");
 else if(g.stage==CLEARED||g.stage==COMPLETE){for(int i=0;i<22;++i)c().fillRect((i*37+now/43)%230+5,(i*19+now/23)%100+25,2,3,i&1?CYAN:CORAL);overlay(g.stage==COMPLETE?"GREAT PARTNERS!":"ROOM CLEAR!",g.stage==COMPLETE?"All ten rooms complete":"You both made it!",g.stage==COMPLETE?"A: play again  UP: pause":"A: next room  UP: pause");}
 else if(g.ticks<100){c().fillRoundRect(10,29,220,14,3,TFTUI::BG);center(32,Game::hint(g.level),TFTUI::MUTED,nullptr);}
 return true;
}
#else
void draw(uint32_t){centerText(8,"POCKET PARTNERS");centerText(30,"Use TFT consoles");centerText(52,"B: BACK");}
#endif
}
