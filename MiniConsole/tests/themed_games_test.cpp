#include "../MiniConsole.ino"
#include <cassert>
#include <set>
#include <vector>
const char *outputDir=nullptr;
void dump(const char *name,uint32_t now){
 render(now);if(!outputDir)return;char path[512];snprintf(path,sizeof(path),"%s/%s.ppm",outputDir,name);
 FILE *f=fopen(path,"wb");assert(f);fprintf(f,"P6\n240 135\n255\n");
 for(int i=0;i<240*135;++i){uint16_t p=oled.physicalBuffer()[i];uint8_t rgb[]={uint8_t((p>>11)*255/31),uint8_t(((p>>5)&63)*255/63),uint8_t((p&31)*255/31)};fwrite(rgb,1,3,f);}fclose(f);
}
void flipCheck(uint32_t now){
 flipped=false;render(now);auto frame=std::vector<uint16_t>(oled.physicalBuffer(),oled.physicalBuffer()+240*135);
 flipped=true;render(now);for(int i=0;i<240*135;++i)assert(oled.physicalBuffer()[i]==frame[240*135-1-i]);flipped=false;
}
int main(int argc,char **argv){
 if(argc>1)outputDir=argv[1];Preferences::flash.clear();setup();assert(!strcmp(GAME_NAMES[BLOCKS],"TETRIS"));
 changeScreen(SETTINGS);settingItem=muteSettingItem();dump("audio-background-settings",0);
 musicEnabled=false;settingItem=musicSettingItem();dump("music-off-settings",0);musicEnabled=true;
 matchGameTheme=false;settingItem=gameThemeSettingItem();dump("classic-background-settings",0);matchGameTheme=true;
 changeScreen(HOME);
 for(int theme=0;theme<=UITheme::PRESET_COUNT;++theme){
  menuTheme=theme<UITheme::PRESET_COUNT?UITheme::preset(theme):UITheme::Palette{{0x1823,0x3087,0xE754,0xF7FD},UITheme::CUSTOM};
  char name[48];uint16_t bg=menuTheme.color[UITheme::BACKGROUND],panel=menuTheme.color[UITheme::PANEL],accent=menuTheme.color[UITheme::ACCENT],ink=menuTheme.color[UITheme::TEXT];
  menuItem=SNAKE;startGame(1000);game.food={23,4};auto snake=game;snprintf(name,sizeof(name),"snake-theme-%02d",theme);dump(name,1000);
  assert(oled.nativeCanvas().getPixel(0,0)==bg);assert(oled.nativeCanvas().getPixel(TFTGames::SNAKE_X+1,TFTGames::SNAKE_Y+1)==panel);
  assert(TFTGames::HEAD==UITheme::rgb(115,231,161)&&TFTGames::FOOD==UITheme::rgb(248,118,116));
  assert(oled.nativeCanvas().getPixel(TFTGames::SNAKE_X+23*7+3,TFTGames::SNAKE_Y+4*7+3)==TFTGames::FOOD);assert(memcmp(&game,&snake,sizeof(game))==0);flipCheck(1000);
  menuItem=BLOCKS;startBlocks(1000);for(int i=0;i<7;++i)blocks.board[19][i]=i+1;auto board=blocks;
  snprintf(name,sizeof(name),"tetris-theme-%02d",theme);dump(name,1000);
  assert(oled.nativeCanvas().getPixel(0,0)==bg);assert(oled.nativeCanvas().getPixel(TFTGames::BLOCK_X+1,TFTGames::BLOCK_Y+1)==panel);
  for(int i=0;i<7;++i)assert(oled.nativeCanvas().getPixel(TFTGames::BLOCK_X+i*10+2,TFTGames::BLOCK_Y+192)==TFTGames::PIECE_COLORS[i]);assert(memcmp(&blocks,&board,sizeof(blocks))==0);flipCheck(1000);
  if(theme==4){std::set<std::vector<uint16_t>> textures;
   render(1000);for(int i=0;i<7;++i){std::vector<uint16_t> tile;for(int y=0;y<10;++y)for(int x=0;x<10;++x)tile.push_back(oled.nativeCanvas().getPixel(TFTGames::BLOCK_X+i*10+x,TFTGames::BLOCK_Y+190+y));textures.insert(tile);}assert(textures.size()==7);
  }
  menuItem=SOKOBAN;sokoLevel=0;startSelectedGame(1000);auto puzzle=sokoban;
  snprintf(name,sizeof(name),"sokoban-theme-%02d",theme);dump(name,1000);
  assert(oled.nativeCanvas().getPixel(0,0)==bg);assert(oled.nativeCanvas().getPixel(TFTGames::SOKO_X+16,TFTGames::SOKO_Y+16)==panel);
  assert(oled.nativeCanvas().getPixel(TFTGames::SOKO_X+5*15+7,TFTGames::SOKO_Y+2*15+7)==TFTGames::SOKO_TARGET);
  assert(memcmp(&puzzle,&sokoban,sizeof(sokoban))==0);flipCheck(1000);
  render(1000);
  assert(oled.nativeCanvas().getPixel(TFTGames::SOKO_X+15+3,TFTGames::SOKO_Y+3)==TFTGames::SOKO_WALL);
  assert(oled.nativeCanvas().getPixel(TFTGames::SOKO_X+2*15+7,TFTGames::SOKO_Y+2*15+5)==TFTGames::SOKO_SKIN);
  assert(oled.nativeCanvas().getPixel(TFTGames::SOKO_X+2*15+7,TFTGames::SOKO_Y+2*15+10)==TFTGames::SOKO_SHIRT);
  assert(oled.nativeCanvas().getPixel(TFTGames::SOKO_X+3*15+3,TFTGames::SOKO_Y+2*15+5)==TFTGames::SOKO_BOX);
  assert(oled.nativeCanvas().getPixel(TFTGames::SOKO_X+3*15+4,TFTGames::SOKO_Y+2*15+4)==TFTGames::SOKO_OUTLINE);
  sokoban.boxes|=sokoban.goals;render(1000);
  assert(oled.nativeCanvas().getPixel(TFTGames::SOKO_X+5*15+7,TFTGames::SOKO_Y+2*15+7)==TFTGames::SOKO_FILLED);
  sokoban=puzzle;
  menuItem=INVADERS;startSelectedGame(1000);snprintf(name,sizeof(name),"invaders-theme-%02d",theme);dump(name,1000);
  assert(oled.physicalBuffer()[0]==0&&oled.physicalBuffer()[134*240+239]==0&&oled.physicalBuffer()[90*240+1]==0);
  changeScreen(HOME);render(1000);assert(oled.nativeCanvas().getPixel(0,0)==bg);
 }
 // Disabling game-theme matching restores classic backgrounds without changing
 // the system palette, sprites, music preference, or Invaders' black backdrop.
 menuTheme=UITheme::preset(6);auto chosen=menuTheme;assert(saveMatchGameTheme(false,0));
 for(GameId id:{SNAKE,BLOCKS,SOKOBAN}){
  menuItem=id;if(id==SNAKE)startGame(0);else if(id==BLOCKS)startBlocks(0);else startSelectedGame(0);
  render(0);assert(oled.nativeCanvas().getPixel(0,0)==ConsoleTFT114::BACKGROUND);
  assert(TFTGames::FIELD==UITheme::rgb(6,14,20));assert(UITheme::equal(menuTheme,chosen));
 }
 menuItem=INVADERS;startSelectedGame(0);render(0);assert(oled.physicalBuffer()[0]==0);
 changeScreen(HOME);render(0);assert(oled.nativeCanvas().getPixel(0,0)==chosen.color[UITheme::BACKGROUND]);
 assert(saveMatchGameTheme(true,0));
 // Runtime loop: music starts only in Tetris, freezes on pause, survives sound
 // effects, and remains inaudible under the persistent global Mute setting.
 clockNow=0;menuItem=BLOCKS;startBlocks(0);loop();clockNow=500;loop();assert(sound.musicPlaying()&&sound.musicPosition()>=500);
 buttons[A].pressed=true;handleInput(500);assert(screen==BLOCKS_PAUSED&&!sound.musicPlaying());uint32_t position=sound.musicPosition();
 buttons[A]=Button{};clockNow=5000;loop();assert(sound.musicPosition()==position);
 buttons[A].pressed=true;handleInput(5000);buttons[A]=Button{};loop();assert(sound.musicPlaying()&&sound.musicPosition()==position);
 saveSoundMuted(true,5000);clockNow=5300;loop();assert(sound.musicPlaying()&&!piezoHz);
 changeScreen(HOME);clockNow=6000;loop();assert(!sound.musicPlaying()&&!piezoHz);
 puts("Game themes/music: all presets and custom colors, native Snake/Tetris/Sokoban, colorful sprites and seven piece textures, exact flips, unchanged rules, black Invaders, palette restoration, Tetris-only music/pause/mute passed.");
}
