#include "../MiniConsole.ino"
#include "motorways_fixture.h"
#include <cassert>
const char *outputDir=nullptr;
void press(Key key,uint32_t now){for(auto &b:buttons)b=Button{};buttons[key].pressed=true;handleInput(now);
  if(key==UP||key==RIGHT||key==DOWN||key==LEFT){buttons[key].pressed=false;handleInput(now+40);}
  if(key==B&&menuItem==MOTORWAYS&&screen==NEW_PLAYING){bool drawing=motorways.editing;buttons[B].pressed=false;buttons[B].held=!drawing;handleInput(now+(drawing?1:600));buttons[B].held=false;handleInput(now+601);if(drawing&&motorTools){buttons[A].pressed=true;handleInput(now+602);buttons[A].pressed=false;}}}
void dump(const char *name,uint32_t now){
  render(now);if(!outputDir)return;char path[512];
#ifdef MINICONSOLE_TFT114
  snprintf(path,sizeof(path),"%s/motorways-%s.ppm",outputDir,name);FILE *f=fopen(path,"wb");assert(f);fprintf(f,"P6\n240 135\n255\n");
  for(int i=0;i<240*135;++i){uint16_t c=oled.physicalBuffer()[i];uint8_t pixel[]={uint8_t((c>>11)*255/31),uint8_t(((c>>5)&63)*255/63),uint8_t((c&31)*255/31)};fwrite(pixel,1,3,f);}fclose(f);
#else
  snprintf(path,sizeof(path),"%s/motorways-%s.pbm",outputDir,name);FILE *f=fopen(path,"w");assert(f);fprintf(f,"P1\n128 64\n");
  for(int y=0;y<64;++y)for(int x=0;x<128;++x)fprintf(f,"%d ",oled.getPixel(x,y));fclose(f);
#endif
}
int main(int argc,char **argv){
  if(argc>1)outputDir=argv[1];Preferences::flash.clear();setup();
  static_assert(DOOM==5&&MOTORWAYS==6,"Never renumber persisted game IDs");
  menuItem=MOTORWAYS;changeScreen(HOME);dump("library",1000);press(A,1010);assert(screen==NEW_READY);dump("ready",1020);
  press(A,1100);assert(screen==NEW_PLAYING&&motorways.valid());dump("start",1100);
  fixedStart(motorways,42); // Controlled geometry for exact button/road-cost checks.
  auto stock=motorways.stock;press(A,1120);assert(motorways.editing);press(RIGHT,1140);press(RIGHT,1160);dump("planning",1160);
  auto ticks=motorways.ticks;updateMotorways(5000);assert(motorways.ticks==ticks&&motorways.stock==stock);
  press(B,5010);assert(!motorways.editing&&screen==NEW_PLAYING);press(A,5020);press(RIGHT,5030);press(RIGHT,5040);press(A,5050);assert(motorways.stock==stock-1);
  // Complete both starter routes with the physical cursor/confirmation controls.
  motorways.cursor=motorways.homes[0].cell;press(A,5060);for(int i=0;i<4;++i)press(RIGHT,5070+i*20);press(A,5160);
  motorways.cursor=motorways.homes[1].cell;press(A,5170);for(int i=0;i<4;++i)press(RIGHT,5180+i*20);press(A,5270);
  for(uint32_t now=5320;now<=15320;now+=50)updateMotorways(now);assert(motorways.score>0&&motorways.valid());dump("traffic",15320);
  press(B,15330);assert(screen==NEW_PAUSED&&pauseSaved&&checkpoint.gameId==MOTORWAYS);dump("pause",15330);
  auto traffic=motorways;press(DOWN,15340);press(RIGHT,15350);assert(motorways.tool==MotorwaysGame::ERASE&&pauseSaved);dump("erase-tool",15350);
  press(DOWN,15360);press(A,15370);assert(motorways.systemTheme&&pauseSaved);dump("system-palette-setting",15370);
  // Tool changes are durable; a power cycle resumes the same committed roads/cars.
  auto bytes=Preferences::flash.at("miniconsole/suspend");motorways=MotorwaysGame{};checkpoint= SuspendState{};screen=HOME;checkpointAvailable=false;
  assert(loadCheckpoint()&&checkpoint.gameId==MOTORWAYS);assert(resumeCheckpoint(100));assert(motorways.score==traffic.score&&motorways.tool==MotorwaysGame::ERASE&&motorways.systemTheme);
  assert(!memcmp(motorways.roads,traffic.roads,motorways.N)&&!motorways.editing);assert(screen==NEW_PLAYING&&motorways.valid());
  // Weekly choice is frozen and survives its checkpoint, then grants supplies once.
  motorways.rewardPending=true;motorways.ticks=1200;motorTick=100;motorRewardItem=0;dump("weekly",100);press(B,110);assert(pauseSaved);
  assert(resumeCheckpoint(200)&&motorways.rewardPending);auto roads=motorways.stock;press(A,210);assert(!motorways.rewardPending&&motorways.stock==roads+motorways.roadReward());
  assert(loadCheckpoint()&&!checkpoint.motorways.rewardPending);auto writes=Preferences::blobWrites;updateMotorways(211);assert(Preferences::blobWrites==writes);
#ifdef MINICONSOLE_TFT114
  // Classic is the per-game default; the optional System palette follows the existing setting.
  for(auto &destination:motorways.destinations)destination.flash=0;
  motorways.systemTheme=false;matchGameTheme=true;menuTheme=UITheme::preset(4);dump("classic-default",219);
  assert(oled.nativeCanvas().getPixel(0,80)==MotorwaysUI::LAND);
  motorways.systemTheme=true;
  matchGameTheme=true;menuTheme=UITheme::preset(1);dump("pastel",220);
  assert(oled.nativeCanvas().getPixel(0,80)==menuTheme.color[UITheme::BACKGROUND]);
  assert(oled.nativeCanvas().getPixel(MotorwaysUI::X+63,MotorwaysUI::Y+14)==MotorwaysUI::COLORS[0]);
  motorways.destinations[0].flash=14;dump("delivery-pulse",221);
  assert(oled.nativeCanvas().getPixel(MotorwaysUI::X+63,MotorwaysUI::Y+14)==UITheme::blend(MotorwaysUI::COLORS[0],0xFFFF,56));
  motorways.destinations[0].flash=0;
  matchGameTheme=false;dump("system-off",230);assert(oled.nativeCanvas().getPixel(0,80)==UITheme::preset(0).color[UITheme::BACKGROUND]);
  motorways.systemTheme=false;dump("classic",231);assert(oled.nativeCanvas().getPixel(0,80)==MotorwaysUI::LAND);
  flipped=true;applyRotation();dump("flipped",240);assert(oled.nativeCanvas().getRotation()==2);flipped=false;applyRotation();
#endif
  motorways.homeCount=3;motorways.homes[2]={MotorwaysGame::cell(15,7),2,0};
  motorways.destCount=3;motorways.destinations[2].anchor=MotorwaysGame::cell(15,1);motorways.destinations[2].color=2;
  assert(motorways.startRenewal());dump("redevelopment",260);
  assert(motorways.valid());motorways.closeClock=MotorwaysGame::CLOSE_LIMIT-1;assert(motorways.updateRenewal());dump("new-site",280);
  assert(motorways.valid());
  // Warning animation and the game-over/high-score flow share normal mute/SFX.
  motorways.destinations[1].demand=8;motorways.destinations[1].overload=motorways.overloadLimit()-2;dump("warning",500);
  motorways.score=321;motorTick=500;updateMotorways(550);updateMotorways(600);assert(screen==NEW_OVER&&motorBest==321&&!checkpointAvailable);dump("over",600);
  press(A,610);assert(screen==NEW_PLAYING&&motorways.score==0);press(B,620);press(B,630);assert(screen==HOME&&checkpointAvailable);
  for(int seed=1;seed<=6;++seed){menuItem=MOTORWAYS;changeScreen(NEW_PLAYING);motorways.reset(seed);assert(motorways.valid());
    char name[32];snprintf(name,sizeof(name),"random-start-%d",seed);dump(name,1000);}
  motorways.ticks=1000;dump("rush-hour",1000);assert(motorways.rushColor()>=0);
  puts("Micro Motorways UI: controls, frozen road previews, pause/tools, checkpoint resume, weekly rewards, theme surfaces, failure and restart passed.");
}
