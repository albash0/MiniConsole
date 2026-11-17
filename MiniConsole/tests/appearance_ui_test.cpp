#include "../MiniConsole.ino"
#include <cassert>
#include <vector>

uint32_t now=0;
const char *outputDir=nullptr;
void tick(uint32_t elapsed=1){now+=elapsed;clockNow=now;pollButtons(now);handleInput(now);}
void down(Key k){pinLevels[BUTTON_PINS[k]]=LOW;tick();tick(18);}
void up(Key k){pinLevels[BUTTON_PINS[k]]=HIGH;tick();tick(18);}
void tap(Key k){down(k);up(k);}
void reboot(){prefs=Preferences{};screen=HOME;checkpointAvailable=false;now=clockNow=0;for(auto &b:buttons)b=Button{};setup();}
void openAppearance(){changeScreen(SETTINGS);settingItem=goingOut?4:3;tap(A);assert(screen==APPEARANCE);}
void dump(const char *name){
  render(now);if(!outputDir)return;char path[512];snprintf(path,sizeof(path),"%s/%s.ppm",outputDir,name);
  FILE *f=fopen(path,"wb");assert(f);fprintf(f,"P6\n240 135\n255\n");
  for(int i=0;i<240*135;++i){uint16_t p=oled.physicalBuffer()[i];uint8_t rgb[]={uint8_t((p>>11)*255/31),uint8_t(((p>>5)&63)*255/63),uint8_t((p&31)*255/31)};fwrite(rgb,1,3,f);}fclose(f);
}
int main(int argc,char **argv){
  if(argc>1)outputDir=argv[1];Preferences::flash.clear();reboot();assert(UITheme::equal(menuTheme,UITheme::preset(0)));
  menuItem=SNAKE;startGame(now);assert(saveCheckpoint(now));auto gameSave=Preferences::flash.at("miniconsole/suspend");
  render(1000);std::vector<uint16_t> snakeFrame(oled.physicalBuffer(),oled.physicalBuffer()+240*135);
  openAppearance();uint32_t writes=Preferences::blobWrites;
  for(int i=0;i<UITheme::PRESET_COUNT;++i){char name[30];snprintf(name,sizeof(name),"theme_preview_%u",i);dump(name);assert(menuTheme.preset==0);tap(RIGHT);}
  assert(Preferences::blobWrites==writes&&pendingTheme.preset==0);
  tap(LEFT);assert(pendingTheme.preset==UITheme::PRESET_COUNT-1);tap(B);assert(screen==SETTINGS&&menuTheme.preset==0);
  openAppearance();tap(RIGHT);assert(pendingTheme.preset==1);tap(DOWN);tap(DOWN);tap(DOWN); // Accent.
  tap(A);assert(screen==COLOR_EDITOR&&colorRole==UITheme::ACCENT);
  auto before=pendingTheme;tap(LEFT);assert(pendingTheme.preset==UITheme::CUSTOM);dump("theme_rgb_editor");
  tap(B);assert(screen==APPEARANCE&&UITheme::equal(before,pendingTheme));
  tap(A);tap(DOWN);uint8_t green=UITheme::channel(pendingTheme.color[colorRole],1);down(RIGHT);tick(500);up(RIGHT);
  assert(UITheme::channel(pendingTheme.color[colorRole],1)==green+2);
  tap(A);assert(screen==APPEARANCE&&pendingTheme.preset==UITheme::CUSTOM);
  assert(Preferences::blobWrites==writes);auto custom=pendingTheme;
  appearanceItem=6;Preferences::failNextWrite=true;tap(A);
  assert(screen==APPEARANCE&&appearanceSaveFailed&&menuTheme.preset==0);dump("theme_save_failed");
  tap(A);assert(screen==SETTINGS&&!appearanceSaveFailed&&UITheme::equal(menuTheme,custom));
  assert(Preferences::blobWrites==writes+1);assert(Preferences::flash.at("miniconsole/suspend")==gameSave);
  reboot();assert(UITheme::equal(menuTheme,custom)&&checkpointAvailable);
  writes=Preferences::blobWrites;openAppearance();appearanceItem=6;tap(A);assert(Preferences::blobWrites==writes);
  // Reset is staged too; cancelling it preserves the saved custom palette.
  openAppearance();appearanceItem=5;tap(A);assert(pendingTheme.preset==0);tap(B);reboot();assert(UITheme::equal(menuTheme,custom));
  // The final Mute row scrolls into view; existing Appearance/hidden goal
  // positions remain intact and the goal stays hidden in normal mode.
  assert(saveGoingOut(true,4));changeScreen(SETTINGS);settingItem=0;tap(UP);assert(settingItem==muteSettingItem());dump("theme_settings_scroll");assert(settingsTop==settingsCount()-4);
  do{tap(UP);}while(settingItem!=3);assert(settingItem==3);tap(RIGHT);assert(drinkGoal==1);tap(DOWN);tap(A);assert(screen==APPEARANCE);
  pendingTheme={{0,0,0,0},UITheme::CUSTOM};dump("theme_black_editor");assert(oled.physicalBuffer()[0]==ConsoleTFT114::BACKGROUND);
  appearanceItem=4;tap(A);dump("theme_black_rgb");assert(oled.physicalBuffer()[0]==ConsoleTFT114::BACKGROUND);tap(B);tap(B);
  assert(saveGoingOut(false,4));assert(settingsCount()==7);
  // Corrupt theme data cannot affect game saves, counters, or the boot palette.
  uint8_t record[UITheme::RECORD_SIZE];UITheme::encode(custom,record);
  for(size_t i=0;i<sizeof(record);++i){record[i]^=1;prefs.putBytes("theme",record,sizeof(record));loadTheme();assert(menuTheme.preset==0);record[i]^=1;}
  assert(Preferences::flash.at("miniconsole/suspend")==gameSave&&drinkCount==4);
  // Every preset is checked on menu, settings, portrait pause, and Doom pause.
  for(uint8_t i=0;i<UITheme::PRESET_COUNT;++i){menuTheme=UITheme::preset(i);menuItem=SNAKE;changeScreen(HOME);char name[40];snprintf(name,sizeof(name),"theme_home_%u",i);dump(name);
    changeScreen(SETTINGS);settingItem=3;snprintf(name,sizeof(name),"theme_settings_%u",i);dump(name);
    menuItem=BLOCKS;changeScreen(BLOCKS_PAUSED);pauseSaved=true;snprintf(name,sizeof(name),"theme_portrait_%u",i);dump(name);
    for(bool flip:{false,true}){flipped=flip;render(now);assert(oled.nativeCanvas().getPixel(0,0)==menuTheme.color[UITheme::BACKGROUND]);}flipped=false;
    menuItem=SNAKE;changeScreen(PLAYING);render(1000);assert(oled.nativeCanvas().getPixel(0,0)==menuTheme.color[UITheme::BACKGROUND]);
    if(i==0)assert(std::vector<uint16_t>(oled.physicalBuffer(),oled.physicalBuffer()+240*135)==snakeFrame);
  }
  std::vector<uint16_t> doom(240*135,0x1234);oled.composeDoom(doom.data(),false);menuTheme=UITheme::preset(0);TFTUI::doomHUD(true,100,50,0,false,"");
  auto normal=std::vector<uint16_t>(oled.physicalBuffer(),oled.physicalBuffer()+240*135);
  oled.composeDoom(doom.data(),false);menuTheme=UITheme::preset(5);TFTUI::doomHUD(true,100,50,0,false,"");assert(normal==std::vector<uint16_t>(oled.physicalBuffer(),oled.physicalBuffer()+240*135));
  TFTUI::doomHUD(true,100,50,0,true,"Saved");assert(oled.nativeCanvas().getPixel(20,100)==menuTheme.color[UITheme::PANEL]);
  openAppearance();appearanceItem=5;tap(A);appearanceItem=6;tap(A);reboot();
  assert(UITheme::equal(menuTheme,UITheme::preset(0))&&Preferences::flash.at("miniconsole/suspend")==gameSave);
  puts("Appearance UI passed: GPIO navigation, preview/cancel, RGB hold/undo, verified save/reboot/failure, defaults, hidden-row scrolling, corrupt data, rotated pauses and themed Snake, unchanged Doom colors and preserved saves.");
}
