#pragma once
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>

// Native-pixel UI; the arcade playfields deliberately retain their pixel art.
namespace TFTUI {
uint16_t BG=0x0841, PANEL=0x10C4, BORDER=0x31A8, TEXT=0xEF7D,
                   MUTED=0x9515, ACCENT=0x4E7F, GREEN=0x67E9, AMBER=0xFD45;
void usePalette(const UITheme::Palette &p) {
  BG=p.color[UITheme::BACKGROUND];PANEL=p.color[UITheme::PANEL];ACCENT=p.color[UITheme::ACCENT];TEXT=p.color[UITheme::TEXT];
  MUTED=UITheme::muted(p);BORDER=UITheme::border(p);
  bool light=UITheme::lightness(BG)>150;GREEN=light?0x0340:0x67E9;AMBER=light?0x9B20:0xFD45;
}
inline TFTNativeCanvas &c() { return oled.nativeCanvas(); }
void text(int x,int baseline,const char *s,uint16_t color=TEXT,const GFXfont *font=&FreeSans9pt7b) {
  c().setFont(font);c().setTextSize(1);c().setTextWrap(false);c().setTextColor(color);c().setCursor(x,baseline);c().print(s);
}
int textWidth(const char *s,const GFXfont *font=&FreeSans9pt7b) {
  int16_t x,y;uint16_t w,h;c().setFont(font);c().setTextSize(1);c().getTextBounds(s,0,0,&x,&y,&w,&h);return w;
}
void center(int baseline,const char *s,uint16_t color=TEXT,const GFXfont *font=&FreeSans9pt7b) {
  if(textWidth(s,font)>c().width()-12&&font==&FreeSansBold18pt7b)font=&FreeSansBold12pt7b;
  if(textWidth(s,font)>c().width()-12&&font==&FreeSansBold12pt7b)font=&FreeSansBold9pt7b;
  int16_t x,y;uint16_t w,h;c().setFont(font);c().setTextSize(1);c().getTextBounds(s,0,0,&x,&y,&w,&h);
  text((c().width()-w)/2-x,baseline,s,color,font);
}
void small(int x,int y,const char *s,uint16_t color=MUTED) { text(x,y,s,color,nullptr); }
void footer(const char *s) {
  c().fillRect(0,c().height()-15,c().width(),15,BG);
  small((c().width()-int(strlen(s))*6)/2,c().height()-11,s,TEXT);
}
void heading(const char *title) {
  text(9,20,title,ACCENT,&FreeSansBold9pt7b);c().drawFastHLine(9,26,c().width()-18,BORDER);
}
uint16_t gameColor(int id) {
  static constexpr uint16_t colors[]={0x5F26,0xBAFF,0xFD45,0x67E9,0xFFE5,0xF9C6,0x3EFF,0xFFE5,0x6B7F,0xFD45,0xFAAC};
  return colors[id];
}
void icon(int id,int x,int y) {
  uint16_t color=gameColor(id);
  if(id==SNAKE) {
    const int dx[]={0,0,1,2,2,3,4},dy[]={3,2,2,2,1,1,1};
    for(int i=0;i<7;++i)c().fillRoundRect(x+dx[i]*8,y+dy[i]*8,9,9,2,color);
    c().fillCircle(x+38,y+10,1,BG);c().drawCircle(x+35,y+33,3,AMBER);
  } else if(id==BLOCKS) {
    for(int r=0;r<4;++r)for(int col=0;col<4;++col)if(BlocksGame::cell(BlocksGame::T,0,col,r))c().fillRoundRect(x+col*9,y+r*9,8,8,1,color);
    for(int col=0;col<5;++col)c().fillRect(x+col*9,y+35,8,6,gameColor(col));
  } else if(id==BREAKOUT) {
    for(int r=0;r<3;++r)for(int col=0;col<4;++col)c().fillRoundRect(x+col*11,y+r*7,9,5,1,gameColor(r));
    c().fillRoundRect(x+9,y+36,27,4,2,TEXT);c().fillCircle(x+27,y+28,2,ACCENT);
  } else if(id==INVADERS) {
    c().fillRoundRect(x+5,y+7,33,18,5,color);c().fillRect(x+13,y+2,5,7,color);c().fillRect(x+27,y+2,5,7,color);
    c().fillRect(x+12,y+12,5,5,BG);c().fillRect(x+27,y+12,5,5,BG);
    c().fillRect(x+8,y+25,5,7,color);c().fillRect(x+30,y+25,5,7,color);c().fillTriangle(x+12,y+42,x+31,y+42,x+22,y+34,TEXT);
  } else if(id==SOKOBAN) {
    c().drawRoundRect(x+2,y+4,29,29,3,color);c().drawLine(x+6,y+8,x+26,y+28,color);c().drawLine(x+26,y+8,x+6,y+28,color);
    c().drawCircle(x+39,y+34,5,GREEN);
  } else if(id==DOOM) {
    c().drawRoundRect(x+5,y+1,34,40,7,color);c().fillRoundRect(x+9,y+5,26,11,3,color);
    c().fillRect(x+11,y+20,7,3,TEXT);c().fillRect(x+27,y+20,7,3,TEXT);c().drawRect(x+15,y+28,15,9,color);
  } else if(id==MOTORWAYS) {
    c().fillRoundRect(x+2,y+2,7,38,3,color);c().fillRoundRect(x+2,y+33,38,7,3,color);
    c().fillRoundRect(x+33,y+3,7,37,3,color);c().fillCircle(x+5,y+4,5,0xF9C6);c().drawRoundRect(x+29,y,15,13,3,TEXT);
  } else if(id==PARTNERS) {
    c().fillRoundRect(x+2,y+17,20,23,4,0xFAAC);c().fillRoundRect(x+25,y+1,20,23,4,0x4E7F);
    c().fillCircle(x+8,y+24,2,BG);c().fillCircle(x+16,y+24,2,BG);c().fillCircle(x+31,y+8,2,BG);c().fillCircle(x+39,y+8,2,BG);
  } else if(id==GAME_COUNT+REACTION) {
    c().fillTriangle(x+27,y,x+10,y+23,x+24,y+23,color);c().fillTriangle(x+17,y+39,x+35,y+16,x+22,y+16,color);
    c().drawCircle(x+6,y+34,7,TEXT);c().drawCircle(x+39,y+34,7,TEXT);small(x+4,y+31,"A",TEXT);small(x+37,y+31,"B",TEXT);
  } else if(id==GAME_COUNT+HIGH_LOW) {
    c().drawRoundRect(x+1,y+8,24,33,3,color);c().fillRoundRect(x+17,y,25,33,3,PANEL);c().drawRoundRect(x+17,y,25,33,3,TEXT);
    text(x+5,y+24,"K",color);text(x+22,y+17,"A",TEXT);
  } else {
    c().fillCircle(x+20,y+25,16,color);c().fillCircle(x+15,y+19,3,PANEL);
    c().drawLine(x+29,y+12,x+37,y+5,color);c().drawLine(x+37,y+5,x+43,y+8,color);
    c().drawLine(x+40,y+2,x+46,y+12,TEXT);c().drawLine(x+40,y+12,x+46,y+2,TEXT);
  }
}
void library(bool party) {
  int id=party?GAME_COUNT+partyItem:menuItem;
  heading(party?"GOING OUT":"POCKET ARCADE");
  char line[32];int page=party?partyItem+2:menuItem+1+(goingOut?PARTY_COUNT+1:0);
  snprintf(line,sizeof(line),"%d/%u",page,libraryPages());small(230-int(strlen(line))*6,10,line);
  c().fillRoundRect(8,34,224,70,10,PANEL);c().drawRoundRect(8,34,224,70,10,BORDER);
  icon(id,22,47);
  const char *title=party?PARTY_NAMES[partyItem]:GAME_NAMES[menuItem];
  const GFXfont *font=textWidth(title,&FreeSansBold12pt7b)>139?&FreeSansBold9pt7b:&FreeSansBold12pt7b;
  if(!party&&(menuItem==MOTORWAYS||menuItem==PARTNERS)){text(85,49,menuItem==PARTNERS?"POCKET":"MICRO",TEXT,&FreeSansBold9pt7b);text(85,69,menuItem==PARTNERS?"PARTNERS":"MOTORWAYS",TEXT,&FreeSansBold9pt7b);}
  else text(85,60,title,TEXT,font);
  bool saved=!party&&(menuItem==DOOM?doomHasSave():checkpointAvailable&&checkpoint.gameId==menuItem);
  text(85,(!party&&(menuItem==MOTORWAYS||menuItem==PARTNERS))?86:81,saved?"Saved game":party?(partyItem==REACTION?"Two players":partyItem==HIGH_LOW?"Ace high":"Random fuse"):GAME_GENRES[menuItem],saved?GREEN:UITheme::lightness(BG)>150?ACCENT:gameColor(id));
  if(party)snprintf(line,sizeof(line),"LEFT / RIGHT TO BROWSE");
  else if(menuItem==DOOM)snprintf(line,sizeof(line),"9 SHAREWARE MAPS");
  else if(menuItem==PARTNERS)snprintf(line,sizeof(line),"TWO CONSOLES / 10 ROOMS");
  else if(menuItem==SOKOBAN)snprintf(line,sizeof(line),"%u / 10 COMPLETE",sokoSolved);
  else snprintf(line,sizeof(line),"BEST %lu",(unsigned long)(menuItem==SNAKE?best:menuItem==BLOCKS?blocksBest:menuItem==BREAKOUT?breakoutBest:menuItem==MOTORWAYS?motorBest:invadersBest));
  small(85,(!party&&(menuItem==MOTORWAYS||menuItem==PARTNERS))?95:91,line);
  int total=libraryPages(),start=(240-(total-1)*13)/2;
  for(int i=0;i<total;++i)c().fillCircle(start+i*13,112,i==page-1?3:1,i==page-1?ACCENT:BORDER);
  footer(saved?"A: CONTINUE     B: SYSTEM":"A: PLAY       B: SYSTEM");
}
void row(int y,const char *label,const char *value,bool selected) {
  if(selected)c().fillRoundRect(7,y-16,226,22,5,PANEL);
  if(selected)c().fillRoundRect(7,y-13,3,15,1,ACCENT);
  text(18,y,label,selected?TEXT:MUTED);
  if(value)text(224-textWidth(value),y,value,selected?ACCENT:TEXT);
}
void systemMenu() {
  heading("SYSTEM");const char *items[]={"Settings","How to play","Button test"};
  for(int i=0;i<3;++i)row(48+i*27,items[i],nullptr,systemItem==i);
  footer("A: OPEN        B: LIBRARY");
}
void settings() {
  heading(soundSaveFailed||musicSaveFailed||gameThemeSaveFailed||(goingOut&&goalSaveFailed)?"SAVE FAILED":"SETTINGS");
  uint8_t count=settingsCount();if(settingItem>=count)settingItem=0;
  if(settingsTop>settingItem)settingsTop=settingItem;
  if(settingItem>=settingsTop+4)settingsTop=settingItem-3;
  if(settingsTop>count-4)settingsTop=count-4;
  for(uint8_t i=settingsTop;i<count&&i<settingsTop+4;++i) {
    const char *label;const char *value;char goal[8];snprintf(goal,sizeof(goal),"%u",drinkGoal);
    if(i==0){label="Speed";value=SPEED_NAMES[speed];}
    else if(i==1){label="Walls";value=wrapEdges?"Wrap":"Solid";}
    else if(i==2){label="Flip screen";value=flipped?"On":"Off";}
    else if(i==gameThemeSettingItem()){label="Match system theme";value=matchGameTheme?"On":"Off";}
    else if(i==musicSettingItem()){label="Music";value=musicEnabled?"On":"Off";}
    else if(i==muteSettingItem()){label="Mute";value=soundMuted?"On":"Off";}
    else if(i==3&&goingOut){label="Drink goal";value=drinkGoal?goal:"Off";}
    else {label="Appearance";value=UITheme::name(menuTheme.preset);}
    row(44+(i-settingsTop)*22,label,value,settingItem==i);
  }
  if(settingsTop)c().fillTriangle(235,32,232,36,238,36,ACCENT);
  if(settingsTop+4<count)c().fillTriangle(235,115,232,111,238,111,ACCENT);
  footer(settingItem==(goingOut?4:3)?"A: APPEARANCE    B: BACK":"LEFT / RIGHT: CHANGE   B: BACK");
}

// The editor uses a fixed, readable palette. Only its preview card uses the
// staged colors, so even black-on-black choices can always be undone.
void themePreview() {
  const auto &p=pendingTheme;
  c().fillRoundRect(137,32,96,76,6,p.color[UITheme::BACKGROUND]);
  c().drawRoundRect(137,32,96,76,6,BORDER);
  small(145,38,UITheme::name(p.preset),p.color[UITheme::TEXT]);
  c().fillRoundRect(144,51,82,54,5,p.color[UITheme::PANEL]);
  c().drawRoundRect(144,51,82,54,5,UITheme::border(p));
  text(150,73,"Arcade",p.color[UITheme::TEXT]);
  c().fillRoundRect(150,82,69,4,2,p.color[UITheme::ACCENT]);
  small(150,93,"A: PLAY",UITheme::muted(p));
}
void appearance() {
  usePalette(UITheme::preset(0));c().fillScreen(BG);heading(appearanceSaveFailed?"SAVE FAILED":"APPEARANCE");
  static const char *const labels[]={"Preset","Background","Panels","Accent","Text","Default","Apply"};
  uint8_t top=appearanceItem>4?appearanceItem-4:0;
  for(uint8_t i=top;i<7&&i<top+5;++i){int y=43+(i-top)*18;
    if(i==appearanceItem){c().fillRoundRect(7,y-14,123,18,4,PANEL);c().fillRect(7,y-11,2,12,ACCENT);}
    text(13,y,labels[i],i==appearanceItem?TEXT:MUTED);
    if(i>=1&&i<=4){c().fillRect(118,y-9,8,8,pendingTheme.color[i-1]);c().drawRect(117,y-10,10,10,BORDER);}
  }
  if(top)c().fillTriangle(132,31,130,35,134,35,ACCENT);
  if(top+5<7)c().fillTriangle(132,116,130,112,134,112,ACCENT);
  themePreview();
  small(144,111,UITheme::lowContrast(pendingTheme)?"Low contrast":UITheme::equal(pendingTheme,menuTheme)?"Saved":"Unsaved",UITheme::lowContrast(pendingTheme)?AMBER:MUTED);
  footer(appearanceItem==0?"L/R: PRESET  A: NEXT  B: CANCEL":appearanceItem==6?"A: APPLY / SAVE   B: CANCEL":appearanceItem==5?"A: RESTORE DEFAULT  B: CANCEL":"A: EDIT        B: CANCEL");
}
void colorEditor() {
  usePalette(UITheme::preset(0));c().fillScreen(BG);
  static const char *const names[]={"BACKGROUND","PANELS","ACCENT","TEXT"};heading(names[colorRole]);
  const char *channels[]={"R","G","B"};uint16_t color=pendingTheme.color[colorRole];
  for(uint8_t i=0;i<3;++i){int y=47+i*27;uint8_t v=UITheme::channel(color,i),max=UITheme::channelMax(i);
    if(i==colorChannel)c().fillRoundRect(7,y-17,123,24,5,PANEL);
    text(14,y,channels[i],i==colorChannel?ACCENT:MUTED,&FreeSansBold9pt7b);
    c().fillRoundRect(36,y-9,57,5,2,BORDER);if(v)c().fillRect(36,y-9,int(v)*57/max,5,i==0?0xF9C6:i==1?0x67E9:0x4E7F);
    char value[4];snprintf(value,sizeof(value),"%u",unsigned(v)*255/max);small(126-int(strlen(value))*6,y-7,value,TEXT);
  }
  themePreview();footer("L/R: ADJUST   A: KEEP   B: UNDO");
}
void controls() {
  heading(controlsPage==GAME_COUNT?"TETRIS SCORING":GAME_NAMES[controlsPage]);
  const char *lines[4];
  if(controlsPage==GAME_COUNT){lines[0]="1 line: 40    2: 100";lines[1]="3 lines: 300    4: 1200";lines[2]="Multiply by level + 1";lines[3]="Next level every 10 lines";}
  else if(controlsPage==BLOCKS){lines[0]="Turn right; D-pad below";lines[1]="Up: rotate   B: hard drop";lines[2]="L/R: move   Down: faster";lines[3]="A: pause or resume";}
  else if(controlsPage==SNAKE){lines[0]="D-pad steers the snake";lines[1]="Eat food and avoid walls";lines[2]="A or B: pause and save";lines[3]="Paused: A resume, B home";}
  else if(controlsPage==DOOM){lines[0]="Up/Down: walk; L/R: turn";lines[1]="A: fire / confirm; B: use";lines[2]="Hold B, then L/R: weapon";lines[3]="Hold A+B: pause / home";}
  else if(controlsPage==MOTORWAYS){lines[0]="A: begin / confirm road";lines[1]="D-pad: move cursor / draw";lines[2]="B: tools; hold B: pause";lines[3]="Match homes to destinations";}
  else if(controlsPage==PARTNERS){lines[0]="L/R: walk    A: jump";lines[1]="Hold B + walk: push crate";lines[2]="Up: pause both consoles";lines[3]="Get key; both reach exit";}
  else if(controlsPage==SOKOBAN){lines[0]="D-pad: walk / push boxes";lines[1]="Put each box on a target";lines[2]="A: undo   B: pause / save";lines[3]="Paused Left: restart level";}
  else {lines[0]="Left/Right: move";lines[1]=controlsPage==BREAKOUT?"A: serve the ball":"Hold A: fire";lines[2]="B: pause and save";lines[3]="Paused: A resume, B home";}
  for(int i=0;i<4;++i)text(8,46+i*20,lines[i]);
  footer("LEFT / RIGHT: PAGE    B: BACK");
}
void buttonTest() {
  heading("BUTTON TEST");
  for(int i=0;i<KEY_COUNT;++i){int x=8+(i%2)*116,y=34+(i/2)*26;c().fillRoundRect(x,y,108,22,5,buttons[i].held?GREEN:PANEL);
    char label[20];snprintf(label,sizeof(label),"%s  %u",KEY_NAMES[i],BUTTON_PINS[i]);text(x+8,y+16,label,buttons[i].held?BG:TEXT);}
  footer("HOLD B: BACK");
}
void continuePrompt() {
  heading("CONTINUE?");icon(menuItem,24,44);text(91,56,menuItem==MOTORWAYS?"MOTORWAYS":GAME_NAMES[menuItem],TEXT,menuItem==MOTORWAYS?&FreeSansBold9pt7b:&FreeSansBold12pt7b);
  text(91,82,"Saved game",GREEN);footer("A: RESUME  LEFT: NEW  B: BACK");
}
void ready() {
  if(screen==BLOCKS_READY){center(27,"TETRIS",ACCENT,&FreeSansBold12pt7b);center(54,"Turn right 90");center(75,"D-pad below");
    center(111,"Up: rotate");center(134,"B: hard drop");center(157,"L/R: move");center(180,"Down: faster");center(203,"A: battle lobby");footer("UP: START   B: HOME");return;}
  heading(GAME_NAMES[menuItem]);
  if(menuItem==SOKOBAN){char level[12];snprintf(level,sizeof(level),"LEVEL %u",sokoLevel+1);small(230-int(strlen(level))*6,10,level);}
  const char *lines[3];
  if(menuItem==SNAKE){lines[0]="D-pad to steer";lines[1]="Eat food. Stay alive.";lines[2]="A or B pauses and saves";}
  else if(menuItem==BREAKOUT){lines[0]="Left / Right: paddle";lines[1]="A serves. B pauses.";lines[2]="3 lives. Clear each rack.";}
  else if(menuItem==INVADERS){lines[0]="Left / Right: move";lines[1]="Hold A to fire";lines[2]="B pauses and saves";}
  else if(menuItem==MOTORWAYS){lines[0]="Match colored buildings";lines[1]="A: draw / confirm roads";lines[2]="B: tools; hold B: pause";}
  else if(menuItem==PARTNERS){lines[0]="Host on one; join on the other";lines[1]="A: jump. Hold B: push crates";lines[2]="UP: pause. Both need the exit";}
  else if(menuItem==DOOM){lines[0]="D-pad: walk and turn";lines[1]="A: fire    B: use";lines[2]=doomAssetsMissing?"Doom data missing":"Hold A+B: pause / save";}
  else {lines[0]="Push boxes onto targets";lines[1]="A: undo    B: pause";lines[2]="Left / Right: choose level";}
  for(int i=0;i<3;++i)text(13,48+i*23,lines[i]);footer("A: START       B: BACK");
}
void pauseOrOver() {
  bool paused=screen==PAUSED||screen==BLOCKS_PAUSED||screen==NEW_PAUSED;
  bool puzzle=menuItem==SOKOBAN;char value[40];
  const char *title=paused?"PAUSED":puzzle?(sokoLevel==9?"ALL CLEAR!":"LEVEL CLEAR!"):menuItem==SNAKE&&won?"YOU WIN!":"GAME OVER";
  center(25,title,ACCENT,&FreeSansBold12pt7b);
  bool portrait=portraitScreen();int top=portrait?72:54;
  if(paused)center(top,pauseSaved?"Saved":"Save failed",pauseSaved?GREEN:AMBER);
  else center(top,GAME_NAMES[menuItem],MUTED);
  uint32_t score=menuItem==SNAKE?game.score():menuItem==BLOCKS?blocks.score:menuItem==BREAKOUT?breakout.score:menuItem==MOTORWAYS?motorways.score:invaders.score;
  if(puzzle)snprintf(value,sizeof(value),"%u moves",sokoban.moves);else snprintf(value,sizeof(value),"%lu",(unsigned long)score);
  center(portrait?116:91,value,TEXT,&FreeSansBold18pt7b);
  if(portrait){center(153,paused?"A: resume":"Up: retry");center(183,"B: home");footer(paused?(pauseSaved?"SAVED TO FLASH":"SAVE FAILED"):"UP: RETRY   B: HOME");}
  else footer(paused?(menuItem==SNAKE?"A: RESUME       B: HOME":"A: GO  LEFT: RESTART  B: HOME"):puzzle?(sokoLevel==9?"A: LEVELS      B: HOME":"A: NEXT        B: HOME"):"A: RETRY       B: HOME");
}
void counter(uint32_t now) {
  uint32_t age=now-pourStartedAt,celebration=now-confettiStartedAt;
  if(pouring&&age>=POUR_MS)pouring=false;if(confetti&&celebration>=CONFETTI_MS)confetti=false;
  if(pouring){int height=age<650?int(age*94/650):int((POUR_MS-age)*94/450);
    for(int x=0;x<240;++x){int phase=(x+int(age/10))%40;int wave=(phase<20?phase:40-phase)/4-2;int y=135-height+wave;if(y<135)c().drawFastVLine(x,y,135-y,0x2454);}
    if(age<650){c().fillTriangle(0,28,9,28,17,53,ACCENT);c().fillRect(14,51,4,70,ACCENT);}}
  if(confetti)for(int i=0;i<40;++i){int phase=(int(celebration)+i*83)%1000,x=(i*67+17)%240,y=phase*160/1000-16;c().drawLine(x,y,x+3,y+3,gameColor(i%(GAME_COUNT+PARTY_COUNT)));}
  c().fillRect(0,0,240,30,BG);heading(counterSaveFailed?"SAVE FAILED":confetti?"GOAL REACHED":"GOING OUT");small(206,10,"1/10");
  c().fillRoundRect(28,40,57,64,8,PANEL);c().drawLine(38,48,44,94,TEXT);c().drawLine(76,48,70,94,TEXT);c().drawFastHLine(44,94,27,TEXT);c().drawFastHLine(38,48,39,TEXT);
  for(int y=76;y<91;++y)c().drawFastHLine(45,y,25,AMBER);
  c().fillRoundRect(104,36,125,74,9,PANEL);char value[20];snprintf(value,sizeof(value),"%lu",(unsigned long)drinkCount);
  const GFXfont *font=drinkCount<10000?&FreeSansBold18pt7b:&FreeSansBold12pt7b;text(166-textWidth(value,font)/2,79,value,TEXT,font);
  if(drinkGoal)snprintf(value,sizeof(value),"Goal: %u",drinkGoal);else snprintf(value,sizeof(value),"Drinks");text(166-textWidth(value)/2,101,value,MUTED);
  if(menuAPending||counterBPending){Key key=counterBPending?B:A;uint32_t held=now-buttons[key].heldAt;if(held>OUT_HOLD_MS)held=OUT_HOLD_MS;c().fillRect(0,115,held*240/OUT_HOLD_MS,2,ACCENT);}
  footer("A: +1    HOLD B: RESET");
}
void playingCard(int x,uint8_t card,bool front,int width=48) {
  int left=x+(48-width)/2;c().fillRoundRect(left,35,width,62,width<6?0:5,PANEL);c().drawRoundRect(left,35,width,62,width<6?0:5,BORDER);
  if(width<38)return;
  if(!front){for(int y=41;y<92;y+=6)c().drawFastHLine(x+6,y,36,0x2949);text(x+15,75,"?",ACCENT,&FreeSansBold18pt7b);return;}
  uint8_t rank=PartyGames::HighLow::rank(card);char label[3];if(rank>=11){label[0]="JQKA"[rank-11];label[1]=0;}else snprintf(label,sizeof(label),"%u",rank);
  uint16_t color=PartyGames::HighLow::suit(card)<2?0xF9C6:TEXT;text(x+6,62,label,color,&FreeSansBold12pt7b);
  int sx=x+33,sy=81;uint8_t suit=PartyGames::HighLow::suit(card);
  if(suit==0){c().fillCircle(sx-3,sy-2,3,color);c().fillCircle(sx+3,sy-2,3,color);c().fillTriangle(sx-6,sy,sx+6,sy,sx,sy+6,color);}
  else if(suit==1){c().fillTriangle(sx,sy-6,sx-5,sy,sx+5,sy,color);c().fillTriangle(sx,sy+6,sx-5,sy,sx+5,sy,color);}
  else {c().fillTriangle(sx,sy-6,sx-6,sy+3,sx+6,sy+3,color);c().fillRect(sx-1,sy,3,9,color);if(suit==2){c().fillCircle(sx-4,sy,3,color);c().fillCircle(sx+4,sy,3,color);}}
}
void party(uint32_t now) {
  char line[40];
  if(partyItem==REACTION){heading("REACTION DUEL");
    if(reaction.phase==PartyGames::ReactionDuel::READY){text(13,50,"A versus B: two players");text(13,76,"Wait for GO to press");text(13,101,"Early press loses");footer("A: READY      LEFT: BACK");return;}
    if(reaction.phase==PartyGames::ReactionDuel::GO){c().fillRoundRect(8,33,224,57,9,GREEN);center(76,"GO!",BG,&FreeSansBold18pt7b);}
    else if(reaction.phase==PartyGames::ReactionDuel::WAIT)center(74,"Wait...",AMBER,&FreeSansBold18pt7b);
    else {const char *winner=reaction.winner==PartyGames::ReactionDuel::PLAYER_A?"A wins":reaction.winner==PartyGames::ReactionDuel::PLAYER_B?"B wins":reaction.winner==PartyGames::ReactionDuel::TIE?"Tie":"No press";center(65,winner,TEXT,&FreeSansBold18pt7b);
      if(reaction.falseStart)snprintf(line,sizeof(line),"Too early");else if(reaction.winner==PartyGames::ReactionDuel::NONE)snprintf(line,sizeof(line),"Try again");else snprintf(line,sizeof(line),"%lu ms",(unsigned long)reaction.reactionMs);center(86,line,MUTED);}
    snprintf(line,sizeof(line),"A: %u               B: %u",reaction.scoreA,reaction.scoreB);center(109,line);
    footer(reaction.phase==PartyGames::ReactionDuel::RESULT?"UP: NEXT      LEFT: BACK":"A versus B    LEFT: BACK");
  } else if(partyItem==HIGH_LOW){heading("HIGH OR LOW");
    if(highLow.phase==PartyGames::HighLow::READY){text(13,50,"A: higher    B: lower");text(13,76,"Ace high. Ties push.");text(13,101,"Build your streak");footer("A: DEAL       LEFT: BACK");return;}
    bool front=highLow.phase==PartyGames::HighLow::RESULT;int width=48;
    if(highLow.phase==PartyGames::HighLow::FLIP){uint32_t age=now-highLow.flippedAt;if(age>280)age=280;front=age>=140;width=2+int(age<140?140-age:age-140)*46/140;}
    playingCard(42,highLow.current,true);playingCard(150,highLow.revealed,front,width);
    if(highLow.phase==PartyGames::HighLow::RESULT)snprintf(line,sizeof(line),highLow.outcome==PartyGames::HighLow::PUSH?"Same rank: push":highLow.outcome==PartyGames::HighLow::WRONG?"Miss: streak reset":"Correct!");
    else snprintf(line,sizeof(line),"Streak %u / Best %u",highLow.streak,highLow.best);center(113,line,MUTED);
    footer(highLow.phase==PartyGames::HighLow::RESULT?"A: NEXT       LEFT: BACK":"A: HIGHER  B: LOWER  LEFT: BACK");
  } else {heading("PASS THE BOMB");
    if(passBomb.phase==PartyGames::PassBomb::READY){text(13,50,"Pass round the room");text(13,76,"A marks each pass");text(13,101,"Holder at BOOM loses");footer("A: START      LEFT: BACK");return;}
    if(passBomb.phase==PartyGames::PassBomb::EXPLODED){center(79,"BOOM!",AMBER,&FreeSansBold18pt7b);center(109,"Holder loses",MUTED);footer("A: AGAIN      LEFT: BACK");}
    else {uint32_t age=now-passBomb.startedAt;if(age>=passBomb.fuseMs)age=passBomb.fuseMs-1;int shake=(now/(450-age*380/passBomb.fuseMs))&1;
      c().fillCircle(118+shake,68,26,AMBER);c().fillCircle(108+shake,58,5,PANEL);c().drawLine(135,48,149,36,AMBER);int tip=150+(passBomb.fuseMs-age)*36/passBomb.fuseMs;c().drawFastHLine(149,36,tip-148,AMBER);c().drawLine(tip-3,31,tip+3,41,TEXT);c().drawLine(tip-3,41,tip+3,31,TEXT);
      snprintf(line,sizeof(line),"Passes: %u",passBomb.passes);center(113,line);footer("A: PASS       LEFT: BACK");}
  }
}
bool render(uint32_t now) {
  if(screen==PLAYING||screen==BLOCKS_PLAYING||screen==NEW_PLAYING||screen==DOOM_RUNNING)return false;
  usePalette(menuTheme);
  c().setRotation(((portraitScreen()?3:0)+(flipped?2:0))&3);c().fillScreen(BG);
  switch(screen){case HOME:library(false);break;case PARTY_LIBRARY:library(true);break;case SYSTEM_MENU:systemMenu();break;
    case SETTINGS:settings();break;case CONTROLS:controls();break;case BUTTON_TEST:buttonTest();break;case CONTINUE_PROMPT:continuePrompt();break;
    case APPEARANCE:appearance();break;case COLOR_EDITOR:colorEditor();break;
    case NEW_READY:case BLOCKS_READY:ready();break;case DRINK_COUNTER:counter(now);break;case PARTY_PLAYING:party(now);break;
    default:pauseOrOver();break;}
  return true;
}
void gameHUD() {
  usePalette(UITheme::preset(0));
  if(menuItem==INVADERS)BG=0;
  c().setRotation(((portraitScreen()?3:0)+(flipped?2:0))&3);char line[40];
  c().fillRect(0,0,c().width(),28,BG);
  if(screen==BLOCKS_PLAYING){snprintf(line,sizeof(line),"%lu",(unsigned long)(blocks.score>999999?999999:blocks.score));text(6,17,line,TEXT,&FreeSansBold9pt7b);
    snprintf(line,sizeof(line),"L%u LV%u",blocks.lines>999?999:blocks.lines,blocks.level()>99?99:blocks.level());small(6,24,line);
    small(102,0,"NEXT");uint8_t piece=clearAnimating?blocks.piece:blocks.next;
    for(int r=0;r<4;++r)for(int x=0;x<4;++x)if(BlocksGame::cell(piece,0,x,r))c().fillRect(101+x*6,10+r*6,5,5,gameColor(piece%6));
  } else if(menuItem==SOKOBAN){snprintf(line,sizeof(line),"LEVEL %u",sokoLevel+1);text(8,20,line,ACCENT,&FreeSansBold9pt7b);snprintf(line,sizeof(line),"M:%u  P:%u",sokoban.moves,sokoban.pushes);text(232-textWidth(line),20,line);}
  else {text(8,14,GAME_NAMES[menuItem],ACCENT,&FreeSansBold9pt7b);uint32_t score=menuItem==SNAKE?game.score():menuItem==BREAKOUT?breakout.score:menuItem==MOTORWAYS?motorways.score:invaders.score;
    snprintf(line,sizeof(line),"%lu",(unsigned long)score);text(232-textWidth(line,&FreeSansBold9pt7b),14,line,TEXT,&FreeSansBold9pt7b);
    if(menuItem==SNAKE)snprintf(line,sizeof(line),"BEST %u",best);
    else if(menuItem==BREAKOUT)snprintf(line,sizeof(line),"LEVEL %u   LIVES %u",breakout.level,breakout.lives);
    else snprintf(line,sizeof(line),"WAVE %u   LIVES %u",invaders.wave,invaders.lives);
    small(8,19,line);}
  c().drawFastHLine(0,27,c().width(),BORDER);
}
void doomHUD(bool hasHud,int health,int ammo,int armor,bool paused,const char *status) {
  usePalette(UITheme::preset(0));
  c().setRotation(flipped?2:0);char line[32];
  if(hasHud){c().fillRect(0,115,240,20,BG);snprintf(line,sizeof(line),"HP %d",health);text(5,131,line,GREEN);if(ammo<0)snprintf(line,sizeof(line),"AM --");else snprintf(line,sizeof(line),"AM %d",ammo);text(85,131,line,AMBER);snprintf(line,sizeof(line),"AR %d",armor);text(166,131,line,ACCENT);}
  if(paused){usePalette(menuTheme);c().fillRoundRect(14,17,212,100,10,PANEL);c().drawRoundRect(14,17,212,100,10,BORDER);center(43,"Doom paused",TEXT,&FreeSansBold12pt7b);center(68,status,GREEN);center(91,"A: resume   B: home");small(65,104,"UP: DOOM MENU",MUTED);}
}
} // namespace TFTUI
