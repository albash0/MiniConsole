#pragma once
namespace BattleUI {
const char *result(){return battleLeaving?"LEAVING":battle.outcome==BlockBattle::NONE?"FINISHING...":battle.outcome==BlockBattle::WIN?"YOU WIN!":battle.outcome==BlockBattle::DRAW?"DRAW":battle.outcome==BlockBattle::LINK_LOST?"LINK LOST":"YOU LOSE";}
#ifdef MINICONSOLE_TFT114
using TFTUI::c;using TFTUI::small;using TFTUI::center;
void lobby(){
 TFTUI::usePalette(menuTheme);c().setRotation((3+(flipped?2:0))&3);c().fillScreen(TFTUI::BG);
 center(26,"BLOCK",TFTUI::ACCENT,&FreeSansBold12pt7b);center(52,"BATTLE",TFTUI::ACCENT,&FreeSansBold12pt7b);
 if(battleChoosing){center(88,"Two consoles");center(111,"No router needed",TFTUI::MUTED,nullptr);
   c().fillRoundRect(7,battleRoleHost?126:159,121,29,5,TFTUI::PANEL);
   center(147,"Host a battle",battleRoleHost?TFTUI::ACCENT:TFTUI::TEXT);
   center(180,"Join a battle",!battleRoleHost?TFTUI::ACCENT:TFTUI::TEXT);
   if(battleRadioError)center(205,"Radio failed: retry",TFTUI::AMBER,nullptr);
   TFTUI::footer("A:OPEN B:BACK");return;}
 char label[24];snprintf(label,sizeof(label),"ROOM %04u",unsigned(battle.session%10000));center(84,battle.session?label:"Searching...",TFTUI::ACCENT,&FreeSansBold9pt7b);
 center(112,battle.host?"Hosting":"Joining");
 if(battle.phase==BlockBattle::COUNTDOWN){center(159,"GET READY",TFTUI::GREEN,&FreeSansBold9pt7b);uint32_t left=int32_t(battle.deadline-millis())>0?battle.deadline-millis():0;snprintf(label,sizeof(label),"%lu",(unsigned long)((left+999)/1000));center(198,label,TFTUI::TEXT,&FreeSansBold18pt7b);}
 else {center(143,battle.paired?"Opponent found":"Waiting for player",TFTUI::MUTED,nullptr);center(168,battle.ready?"YOU: READY":"YOU: NOT READY",battle.ready?TFTUI::GREEN:TFTUI::MUTED,nullptr);center(188,battle.peerReady?"THEM: READY":"THEM: NOT READY",battle.peerReady?TFTUI::GREEN:TFTUI::MUTED,nullptr);}
 TFTUI::footer("A:READY B:BACK");
}
void hud(){
 char value[16];c().drawRect(3,31,7,200,TFTUI::BORDER);int n=battle.pending>20?20:battle.pending;
 if(n)c().fillRect(4,230-n*10,5,n*10,TFTUI::AMBER);
 small(120,66,"OP",TFTUI::MUTED);c().drawRect(124,78,7,104,TFTUI::BORDER);c().fillRect(125,181-battle.peerHeight*5,5,battle.peerHeight*5,TFTUI::ACCENT);
 snprintf(value,sizeof(value),"IN:%u",battle.pending);c().fillRect(0,231,135,9,TFTUI::BG);small(6,232,value,TFTUI::AMBER);small(60,232,"A:PAUSE",TFTUI::TEXT);
}
bool render(uint32_t now){
 if(!battleMode)return false;
 if(screen==BATTLE_LOBBY){lobby();return true;}
 if(screen==BLOCKS_PLAYING){TFTGames::blocksFrame(now);hud();if(battle.frozen(now)){
    c().fillRoundRect(5,90,125,62,6,TFTUI::BG);c().drawRoundRect(5,90,125,62,6,TFTUI::ACCENT);
    center(115,battle.dead?"TOP OUT":battle.stale(now)?"RECONNECTING":"THEY PAUSED",TFTUI::ACCENT,&FreeSansBold9pt7b);center(139,"Waiting...",TFTUI::MUTED,nullptr);}return true;}
 if(screen==BLOCKS_PAUSED||screen==BLOCKS_OVER){TFTUI::usePalette(menuTheme);c().setRotation((3+(flipped?2:0))&3);c().fillScreen(TFTUI::BG);
   center(54,screen==BLOCKS_PAUSED?"PAUSED":result(),TFTUI::ACCENT,&FreeSansBold12pt7b);
   char score[24];snprintf(score,sizeof(score),"%lu points",(unsigned long)blocks.score);center(104,score,TFTUI::TEXT,nullptr);
   center(150,screen==BLOCKS_PAUSED?"Both games frozen":"Up/A: rematch",TFTUI::MUTED,nullptr);
   center(181,screen==BLOCKS_PAUSED?"A: resume":"B: library",TFTUI::TEXT,nullptr);
   TFTUI::footer(screen==BLOCKS_PAUSED?"B:FORFEIT":"UP:REMATCH");return true;}
 return false;
}
#else
void draw(uint32_t now){
 if(screen==BATTLE_LOBBY){centerText(4,"BLOCK BATTLE");if(battleChoosing){centerText(30,battleRoleHost?"> HOST":"  HOST");centerText(46,battleRoleHost?"  JOIN":"> JOIN");centerText(78,battleRadioError?"RADIO FAILED":"A:OPEN B:BACK");}
 else {char b[24];snprintf(b,sizeof(b),"ROOM %04u",unsigned(battle.session%10000));centerText(27,b);centerText(46,battle.paired?"LINKED":"SEARCHING");centerText(65,battle.ready?"YOU READY":"A:READY");centerText(80,battle.peerReady?"THEM READY":"WAITING");}}else {centerText(30,screen==BLOCKS_PAUSED?"PAUSED":result());centerText(60,screen==BLOCKS_PAUSED?"A:GO B:QUIT":"UP:AGAIN B:HOME");}
}
#endif
}
