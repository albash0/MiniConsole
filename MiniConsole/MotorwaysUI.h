#pragma once
namespace MotorwaysUI {
#ifdef MINICONSOLE_TFT114
using TFTUI::c;using TFTUI::text;using TFTUI::small;using TFTUI::center;
constexpr int X=20,Y=24,S=10;
constexpr uint16_t COLORS[]={UITheme::rgb(226,86,76),UITheme::rgb(65,143,179),UITheme::rgb(240,194,69)};
constexpr uint16_t LAND=UITheme::rgb(247,242,225),ROAD=UITheme::rgb(153,153,146),ROAD_EDGE=UITheme::rgb(115,119,113),
  WATER=UITheme::rgb(151,206,207),WAVE=UITheme::rgb(193,226,221),EXPRESS=UITheme::rgb(161,111,162);
UITheme::Palette classicPalette(){
  return {{LAND,UITheme::rgb(235,228,207),UITheme::rgb(43,117,123),UITheme::rgb(49,66,67)},UITheme::CUSTOM};
}
void useGamePalette(){
  if(motorways.systemTheme){TFTGames::useGamePalette();return;}
  TFTUI::usePalette(classicPalette());
  TFTGames::BG=LAND;TFTGames::FIELD=LAND;TFTGames::FRAME=UITheme::rgb(218,213,193);
  TFTGames::GRID=UITheme::rgb(229,225,208);TFTGames::INK=TFTUI::TEXT;
}
int px(uint8_t p){return X+MotorwaysGame::x(p)*S+S/2;}
int py(uint8_t p){return Y+MotorwaysGame::y(p)*S+S/2;}
void stroke(int ax,int ay,int bx,int by,int r,uint16_t color){
  if(ax==bx)c().fillRect(ax-r,ay<by?ay:by,2*r+1,abs(by-ay)+1,color);
  else if(ay==by)c().fillRect(ax<bx?ax:bx,ay-r,abs(bx-ax)+1,2*r+1,color);
  else {for(int offset=-r;offset<=r;++offset){c().drawLine(ax+offset,ay,bx+offset,by,color);c().drawLine(ax,ay+offset,bx,by+offset,color);}}
  c().fillCircle(ax,ay,r,color);c().fillCircle(bx,by,r,color);
}
void symbol(int color,int x,int y,uint16_t ink,int size=2){
  if(color==0)c().fillCircle(x,y,size,ink);
  else if(color==1)c().fillTriangle(x,y-size,x-size,y+size,x+size,y+size,ink);
  else c().fillRect(x-size,y-size,size*2+1,size*2+1,ink);
}
void road(int a,int b,uint16_t color=ROAD){
  stroke(px(a),py(a),px(b),py(b),3,ROAD_EDGE);stroke(px(a),py(a),px(b),py(b),2,color);
}
void world(uint32_t now){
  auto &g=motorways;useGamePalette();c().setRotation(flipped?2:0);c().fillScreen(TFTGames::BG);
  c().fillRoundRect(X-2,Y-2,204,104,3,TFTGames::FRAME);c().fillRect(X,Y,200,100,TFTGames::FIELD);
  for(int y=0;y<10;++y)for(int x=0;x<20;++x)c().drawPixel(X+x*S+5,Y+y*S+5,TFTGames::GRID);
  c().fillRect(X+100,Y,10,100,WATER);
  for(int y=Y+3;y<Y+100;y+=9)c().drawFastHLine(X+101+int((now/300+y)%3),y,4,WAVE);
  for(int p=0;p<g.N;++p){for(int d:{1,2,4,5})if(g.roads[p]&(1<<d))road(p,g.neighbor(p,d));
    if(g.water(p)&&g.roads[p]){int x=px(p),y=py(p);
      if(g.roads[p]&10){c().drawFastHLine(x-5,y-4,10,0xD65A);c().drawFastHLine(x-5,y+4,10,0xD65A);}
      else {int slope=(g.roads[p]&80)?-1:1;for(int offset:{-4,4})c().drawLine(x-5,y-5*slope+offset,x+5,y+5*slope+offset,0xD65A);}}}
  for(int p=0;p<g.N;++p)if(g.crossAt(p)){c().fillCircle(px(p)+S/2,py(p)+S/2,3,ROAD_EDGE);c().fillCircle(px(p)+S/2,py(p)+S/2,2,ROAD);}
  for(int i=0;i<g.linkCount;++i){auto &l=g.links[i];c().drawLine(px(l.a),py(l.a),px(l.b),py(l.b),EXPRESS);c().drawCircle(px(l.a),py(l.a),3,EXPRESS);c().drawCircle(px(l.b),py(l.b),3,EXPRESS);}
  for(int i=0;i<g.homeCount;++i){auto &h=g.homes[i];int x=px(h.cell),y=py(h.cell);uint16_t color=COLORS[h.color];
    c().fillRoundRect(x-4,y-2,9,7,1,color);c().fillTriangle(x-4,y-2,x,y-5,x+4,y-2,color);symbol(h.color,x,y+1,0x1084,1);}
  for(int i=0;i<g.destCount;++i){auto &d=g.destinations[i];int x=X+g.x(d.anchor)*S,y=Y+g.y(d.anchor)*S;uint16_t color=COLORS[d.color];
    if(i==g.closing)color=UITheme::blend(color,TFTGames::FIELD,160);
    c().fillRoundRect(x+1,y+1,18,18,4,0x1084);c().fillRoundRect(x+2,y+2,16,16,3,d.flash?UITheme::blend(color,0xFFFF,d.flash*4):color);
    symbol(d.color,x+10,y+7,0x1084,2);c().fillRect(x+3,y+17,5,3,ROAD);
    for(int n=0;n<d.demand;++n)c().fillCircle(x+4+(n%5)*3,y+12+(n/5)*3,1,0x1084);
    if(i==g.closing){c().drawLine(x+4,y+4,x+15,y+15,TFTUI::TEXT);c().drawLine(x+15,y+4,x+4,y+15,TFTUI::TEXT);}
    else if(g.grace[i])c().drawRoundRect(x-1,y-1,22,22,4,TFTUI::ACCENT);
    if(d.overload){int segments=d.overload*20/g.overloadLimit();uint16_t warning=((now/200)&1)?0xF986:0xFE47;
      // A clockwise square timer stays legible at this scale.
      c().drawRoundRect(x-1,y-1,22,22,5,ROAD_EDGE);
      for(int n=0;n<=segments;++n){int side=n/5,pos=(n%5)*4;if(side==0)c().drawPixel(x+pos,y-2,warning);if(side==1)c().drawPixel(x+20,y+pos,warning);if(side==2)c().drawPixel(x+20-pos,y+20,warning);if(side==3)c().drawPixel(x-2,y+20-pos,warning);}}
  }
  if(g.closing!=g.NONE){int x=X+g.x(g.replacement)*S,y=Y+g.y(g.replacement)*S;uint16_t color=COLORS[g.replacementColor];
    for(int p=0;p<20;p+=4){c().drawFastHLine(x+p,y,2,color);c().drawFastHLine(x+p,y+19,2,color);c().drawFastVLine(x,y+p,2,color);c().drawFastVLine(x+19,y+p,2,color);}
    if(g.replacement!=g.destinations[g.closing].anchor){symbol(g.replacementColor,x+10,y+8,color,3);c().drawRect(x+2,y+15,6,5,color);}
  }
  for(const auto &car:g.cars)if(car.active){if(car.from==car.to&&g.building(car.from))continue;
    int progress=car.progress;
    if(screen==NEW_PLAYING&&!g.editing&&!g.rewardPending&&!motorTools&&!motorBPending&&car.from!=car.to){uint32_t age=now-motorTick;if(age>49)age=49;progress+=age*(g.linked(car.from,car.to)?64:(g.direction(car.from,car.to)>=4||g.crossingFor(car.from,car.to)!=g.NONE)?23:32)/50;if(progress>255)progress=255;}
    int ax=px(car.from),ay=py(car.from),bx=px(car.to),by=py(car.to);
    int x=ax+(bx-ax)*progress/256,y=ay+(by-ay)*progress/256;
    uint8_t crossing=g.crossingFor(car.from,car.to);
    if(crossing!=g.NONE){int mx=px(crossing)+S/2,my=py(crossing)+S/2;
      if(progress<128){x=ax+(mx-ax)*progress/128;y=ay+(my-ay)*progress/128;}
      else{x=mx+(bx-mx)*(progress-128)/128;y=my+(by-my)*(progress-128)/128;}}

    if(abs(bx-ax)>abs(by-ay))y+=(bx>ax?1:-1);else if(by!=ay)x+=(by>ay?-1:1);
    uint16_t color=COLORS[g.homes[car.home].color];if(car.returning)color=UITheme::blend(color,0,80);
    c().fillRoundRect(x-2,y-2,5,5,1,0x1084);c().fillRect(x-1,y-1,3,3,color);c().drawPixel(x,y-1,0xFFFF);
    if(car.from==car.to)c().drawPixel(x+1,y+1,COLORS[0]);
  }
  if(g.editing){uint16_t cost;uint8_t bridges;bool valid=g.tool==g.EXPRESS?g.express&&g.linkCount<g.LINKS:g.planLength==1||g.cost(cost,bridges)==g.OK;
    uint16_t color=valid?TFTUI::GREEN:COLORS[0];
    if(g.tool==g.EXPRESS)c().drawLine(px(g.plan[0]),py(g.plan[0]),px(g.cursor),py(g.cursor),EXPRESS);
    else for(int i=1;i<g.planLength;++i)stroke(px(g.plan[i-1]),py(g.plan[i-1]),px(g.plan[i]),py(g.plan[i]),1,color);
  }
  int cx=px(g.cursor),cy=py(g.cursor);uint16_t cursor=g.tool==g.ERASE?0xF986:TFTUI::TEXT;
  c().drawRoundRect(cx-5,cy-5,11,11,2,cursor);c().drawPixel(cx,cy,cursor);
  char line[40];
  if(g.score>=1000000000)snprintf(line,sizeof(line),"%lu.%luB",(unsigned long)(g.score/1000000000),(unsigned long)(g.score/100000000%10));
  else if(g.score>=1000000)snprintf(line,sizeof(line),"%lu.%luM",(unsigned long)(g.score/1000000),(unsigned long)(g.score/100000%10));
  else snprintf(line,sizeof(line),"%lu",(unsigned long)g.score);
  text(8,16,line,TFTUI::TEXT,&FreeSansBold9pt7b);
  if(g.week()>=10000)snprintf(line,sizeof(line),"W%luk R%u",(unsigned long)(g.week()/1000),g.stock);
  else snprintf(line,sizeof(line),"W%lu  R%u",(unsigned long)g.week(),g.stock);
  small(82,2,line,TFTUI::TEXT);
  snprintf(line,sizeof(line),"B%u   X%u",g.bridges,g.express);small(82,13,line);
  int rush=g.rushColor();small(184,rush<0?8:1,MOTOR_TOOLS[g.tool],TFTUI::ACCENT);
  if(rush>=0){c().drawCircle(185,15,3,TFTUI::TEXT);c().fillCircle(185,15,2,COLORS[rush]);small(191,12,"RUSH",TFTUI::TEXT);}
  const char *hint=g.editing?"D-PAD: DRAW  A: OK  B: CANCEL":g.tool==g.ERASE?"A: ERASE  B:TOOLS HOLD B:PAUSE":"A: BUILD  B:TOOLS  HOLD B:PAUSE";
  if(!g.editing){
    if(g.closing!=g.NONE)hint="SITE CLOSING - NEW SITE OUTLINED";
    else {uint16_t grace=0;for(int i=0;i<g.destCount;++i)if(g.grace[i]>grace)grace=g.grace[i];
      if(grace){snprintf(line,sizeof(line),"NEW SITE: %us TO CONNECT",(grace+19)/20);hint=line;}}
  }
  if(now-motorNoticeAt<1400)hint=MOTOR_ERRORS[motorNotice];
  if(g.editing&&g.tool==g.ROAD&&g.planLength>1){uint16_t roads;uint8_t bridges;g.cost(roads,bridges);snprintf(line,sizeof(line),"COST %u ROADS %u BRIDGES  A: OK",roads,bridges);hint=line;}
  small((240-int(strlen(hint))*6)/2,127,hint,TFTUI::TEXT);
}
void pause(){
  TFTUI::usePalette(menuTheme);c().setRotation(flipped?2:0);c().fillScreen(TFTUI::BG);TFTUI::heading("PAUSED");
  small(146,12,motorPaletteSaveFailed?"Palette error":pauseSaved?"Saved":"Save failed",pauseSaved&&!motorPaletteSaveFailed?TFTUI::GREEN:TFTUI::AMBER);
  const char *labels[]={"Resume","Tool","Palette","Restart","Home"};
  for(int i=0;i<5;++i){int y=43+i*18;bool selected=motorPauseItem==i;
    if(selected)c().fillRoundRect(7,y-13,226,17,4,TFTUI::PANEL);
    text(18,y,labels[i],selected?TFTUI::TEXT:TFTUI::MUTED);
    const char *value=i==1?MOTOR_TOOLS[motorways.tool]:i==2?(motorways.systemTheme?"System":"Classic"):nullptr;
    if(value)text(224-TFTUI::textWidth(value),y,value,selected?TFTUI::ACCENT:TFTUI::TEXT);
  }
  TFTUI::footer(motorPauseItem==1||motorPauseItem==2?"LEFT / RIGHT: CHANGE  A: CYCLE":"A: SELECT      B: HOME");
}
void reward(){
  useGamePalette();c().fillScreen(TFTUI::BG);c().fillRoundRect(8,5,224,125,8,TFTUI::BG);c().drawRoundRect(8,5,224,125,8,TFTUI::ACCENT);
  char title[24];snprintf(title,sizeof(title),"WEEK %lu",(unsigned long)motorways.week());center(25,title,TFTUI::ACCENT,&FreeSansBold12pt7b);
  const char *labels[]={"Road supply","Bridge kit","Express link"};char values[3][18];
  snprintf(values[0],sizeof(values[0]),"+%u roads",motorways.roadReward());
  snprintf(values[1],sizeof(values[1]),"+1 / %u roads",motorways.kitRoadReward(true));
  if(motorways.express+motorways.linkCount>=3)snprintf(values[2],sizeof(values[2]),"+%u roads",motorways.roadReward());
  else snprintf(values[2],sizeof(values[2]),"+1 / %u roads",motorways.kitRoadReward(false));
  for(int i=0;i<3;++i){int y=48+i*25;if(motorRewardItem==i)c().fillRoundRect(14,y-15,212,22,4,TFTUI::PANEL);small(20,y-8,labels[i],motorRewardItem==i?TFTUI::ACCENT:TFTUI::TEXT);small(144,y-8,values[i],TFTUI::TEXT);}
  small(27,115,"UP/DOWN: CHOOSE   A: CLAIM",TFTUI::TEXT);
}
void tools(){
  // Overlay the frozen map; each icon occupies its own left-side tile.
  for(int i=0;i<3;++i){int x=2,y=29+i*29;bool selected=motorways.tool==i;
    c().fillRoundRect(x,y,27,26,4,TFTUI::BG);c().drawRoundRect(x,y,27,26,4,selected?TFTUI::ACCENT:TFTUI::BORDER);
    if(selected)c().fillRoundRect(x+2,y+2,23,22,3,TFTUI::PANEL);
    uint16_t ink=selected?TFTUI::ACCENT:TFTUI::MUTED;
    if(i==0){c().fillRoundRect(x+10,y+4,7,18,2,ink);for(int j=0;j<3;++j)c().drawFastVLine(x+13,y+6+j*5,2,TFTUI::BG);}
    else if(i==1){c().fillTriangle(x+5,y+16,x+14,y+5,x+23,y+14,ink);c().fillTriangle(x+5,y+16,x+23,y+14,x+14,y+23,ink);c().drawLine(x+10,y+12,x+19,y+19,TFTUI::BG);}
    else {c().drawLine(x+6,y+20,x+21,y+6,ink);c().drawLine(x+6,y+19,x+21,y+5,ink);c().fillCircle(x+6,y+20,3,ink);c().fillCircle(x+21,y+6,3,ink);}
  }
  c().fillRect(0,126,240,9,TFTUI::BG);small(35,127,"UP/DOWN  A:OK  HOLD B:PAUSE",TFTUI::TEXT);
}
void draw(uint32_t now){world(now);}
bool render(uint32_t now){
  if(menuItem!=MOTORWAYS||(screen!=NEW_PLAYING&&screen!=NEW_PAUSED))return false;
  if(screen==NEW_PAUSED)pause();else {world(now);if(motorways.rewardPending)reward();if(motorTools)tools();}return true;
}
#else
void draw(uint32_t now){
  auto &g=motorways;char line[24];
  if(screen==NEW_PAUSED){heading(pauseSaved?"PAUSED - SAVED":"PAUSED - SAVE FAILED");
    const char *labels[]={"RESUME","TOOL","PALETTE","RESTART","HOME"};
    for(int i=0;i<5;++i){snprintf(line,sizeof(line),"%s %s",labels[i],i==1?MOTOR_TOOLS[g.tool]:i==2?(g.systemTheme?"System":"Classic"):"");drawRow(14+i*9,line,motorPauseItem==i);}return;}
  if(g.rewardPending){heading("WEEKLY SUPPLY");char labels[3][24];snprintf(labels[0],sizeof(labels[0]),"%u ROADS",g.roadReward());
    snprintf(labels[1],sizeof(labels[1]),"BRIDGE + %u ROADS",g.kitRoadReward(true));
    if(g.express+g.linkCount>=3)snprintf(labels[2],sizeof(labels[2]),"%u ROADS",g.roadReward());else snprintf(labels[2],sizeof(labels[2]),"EXPRESS + %u ROADS",g.kitRoadReward(false));
    for(int i=0;i<3;++i)drawRow(17+i*12,labels[i],motorRewardItem==i);centerText(57,"U/D:CHOOSE A:CLAIM");return;}
  snprintf(line,sizeof(line),"%lu R%u B%u X%u",(unsigned long)g.score,g.stock,g.bridges,g.express);textAt(0,0,line);
  constexpr int X=24,Y=15,S=4;
  for(int y=0;y<40;y+=3)oled.drawPixel(X+42,Y+y,WHITE);
  auto px=[](uint8_t p){return X+MotorwaysGame::x(p)*S+2;};auto py=[](uint8_t p){return Y+MotorwaysGame::y(p)*S+2;};
  for(int p=0;p<g.N;++p)for(int d:{1,2,4,5})if(g.roads[p]&(1<<d))oled.drawLine(px(p),py(p),px(g.neighbor(p,d)),py(g.neighbor(p,d)),WHITE);
  for(int i=0;i<g.linkCount;++i)oled.drawLine(px(g.links[i].a),py(g.links[i].a),px(g.links[i].b),py(g.links[i].b),WHITE);
  for(int i=0;i<g.homeCount;++i){auto &h=g.homes[i];oled.drawRect(px(h.cell)-1,py(h.cell)-1,3,3,WHITE);oled.drawPixel(px(h.cell),py(h.cell),h.color==1?BLACK:WHITE);}
  for(int i=0;i<g.destCount;++i){auto &d=g.destinations[i];int x=px(d.anchor)-2,y=py(d.anchor)-2;oled.drawRect(x,y,8,8,WHITE);oled.setCursor(x+1,y);oled.write(uint8_t('1'+d.color));if(d.overload&&((now/200)&1))oled.drawRect(x-1,y-1,10,10,WHITE);}
  if(g.closing!=g.NONE){int x=px(g.replacement)-2,y=py(g.replacement)-2;oled.drawRect(x,y,8,8,WHITE);oled.drawLine(x,y,x+7,y+7,WHITE);}
  for(auto &car:g.cars)if(car.active){int ax=px(car.from),ay=py(car.from),bx=px(car.to),by=py(car.to),progress=car.progress;
    int x=ax+(bx-ax)*progress/256,y=ay+(by-ay)*progress/256;uint8_t cross=g.crossingFor(car.from,car.to);
    if(cross!=g.NONE){int mx=px(cross)+S/2,my=py(cross)+S/2;if(progress<128){x=ax+(mx-ax)*progress/128;y=ay+(my-ay)*progress/128;}else{x=mx+(bx-mx)*(progress-128)/128;y=my+(by-my)*(progress-128)/128;}}
    oled.drawPixel(x,y,WHITE);}

  if(g.editing)for(int i=1;i<g.planLength;++i)oled.drawLine(px(g.plan[i-1]),py(g.plan[i-1]),px(g.plan[i]),py(g.plan[i]),WHITE);
  oled.drawRect(px(g.cursor)-2,py(g.cursor)-2,5,5,WHITE);
  if(motorTools){for(int i=0;i<3;++i){int y=14+i*13;oled.fillRect(0,y,15,12,BLACK);oled.drawRect(0,y,15,12,WHITE);if(g.tool==i)oled.fillRect(1,y+1,13,10,WHITE);oled.setTextColor(g.tool==i?BLACK:WHITE);textAt(5,y+2,i==0?"R":i==1?"X":"E");oled.setTextColor(WHITE);}}
  centerText(57,now-motorNoticeAt<1400?MOTOR_ERRORS[motorNotice]:g.editing?"A:OK B:CANCEL":g.closing!=g.NONE?"NEW SITE OUTLINED":"A:BUILD B:TOOLS");
}
#endif
}
