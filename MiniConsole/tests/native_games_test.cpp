#include "../MiniConsole.ino"
#include <cassert>
#include <vector>

const char *outputDir=nullptr;
void dump(const char *name,uint32_t now){
  render(now);if(!outputDir)return;char path[512];snprintf(path,sizeof(path),"%s/%s.ppm",outputDir,name);
  FILE *f=fopen(path,"wb");assert(f);fprintf(f,"P6\n240 135\n255\n");
  for(int i=0;i<240*135;++i){uint16_t p=oled.physicalBuffer()[i];uint8_t rgb[]={uint8_t((p>>11)*255/31),uint8_t(((p>>5)&63)*255/63),uint8_t((p&31)*255/31)};fwrite(rgb,1,3,f);}fclose(f);
}
void clearButtons(){for(auto &b:buttons)b=Button{};}
void press(Key key,uint32_t now){clearButtons();buttons[key].pressed=true;handleInput(now);clearButtons();}
int main(int argc,char **argv){
  if(argc>1)outputDir=argv[1];Preferences::flash.clear();setup();menuItem=SNAKE;startGame(1000);game.food={22,4};
  snakeMotion.beforeStep(game);assert(game.step()==SnakeGame::MOVED);lastStep=1000;
  dump("native_snake_move_0",1000);auto frame=std::vector<uint16_t>(oled.physicalBuffer(),oled.physicalBuffer()+240*135);
  dump("native_snake_move_half",1075);assert(frame!=std::vector<uint16_t>(oled.physicalBuffer(),oled.physicalBuffer()+240*135));
  game.queueTurn(SnakeGame::UP);snakeMotion.beforeStep(game);game.step();lastStep=1150;dump("native_snake_turn",1225);
  SnakeGame before=game;render(1270);assert(memcmp(&before,&game,sizeof(game))==0);
  auto p=snakeMotion.point(game,0,128);press(A,1225);assert(screen==PAUSED);press(A,3225);assert(screen==PLAYING&&lastStep==3150);
  auto resumed=snakeMotion.point(game,0,uint16_t((3225-lastStep)*256/snakeInterval()));assert(p.x==resumed.x&&p.y==resumed.y);
  assert(saveCheckpoint(3225));game.reset(false);assert(resumeCheckpoint(4000)&&!snakeMotion.moving);assert(memcmp(game.body,before.body,game.length*sizeof(game.body[0]))==0);
  // A wrapped chain must appear at the two edges, never across the middle.
  game.reset(true);game.direction=SnakeGame::RIGHT;game.food={10,2};for(int i=0;i<4;++i)game.body[i]={int8_t(29-i),6};
  snakeMotion.beforeStep(game);game.step();lastStep=5000;dump("native_snake_wrap",5075);
  assert(oled.nativeCanvas().getPixel(TFTGames::SNAKE_X+12*7+3,TFTGames::SNAKE_Y+6*7+3)==TFTGames::GRID);
  snakeMotion.reset();dump("native_snake_wrap_resumed",5100);
  assert(oled.nativeCanvas().getPixel(TFTGames::SNAKE_X+12*7+3,TFTGames::SNAKE_Y+6*7+3)==TFTGames::GRID);
  game.reset(false);SnakeGame::Cell curve[]={{19,5},{19,6},{19,7},{19,8},{18,8},{17,8},{16,8},{15,8},{15,7},{15,6},{15,5},{15,4},{14,4},{13,4},{12,4},{11,4},{11,5},{11,6},{10,6},{9,6},{8,6}};
  game.length=sizeof(curve)/sizeof(curve[0]);memcpy(game.body,curve,sizeof(curve));game.direction=SnakeGame::UP;game.food={23,3};snakeMotion.reset();dump("native_snake_curve",5200);
  game.length=SnakeGame::CAPACITY;
  for(int i=0;i<game.length;++i){int row=i/30;game.body[i]={int8_t(row%2?29-i%30:i%30),int8_t(row)};}
  snakeMotion.reset();dump("native_snake_full",5200);
  // Frame orientation is exactly one 180-degree flip, including animations.
  flipped=false;render(5200);frame.assign(oled.physicalBuffer(),oled.physicalBuffer()+240*135);
  flipped=true;render(5200);for(int i=0;i<240*135;++i)assert(oled.physicalBuffer()[i]==frame[240*135-1-i]);flipped=false;
  game.reset(false);game.food={16,6};snakeMotion.beforeStep(game);auto ate=game.step();snakeMotion.afterStep(ate);assert(ate==SnakeGame::ATE);game.placeFood(42);lastStep=5500;
  dump("native_snake_eating",5500);assert(oled.nativeCanvas().getPixel(TFTGames::SNAKE_X+16*7+3,TFTGames::SNAKE_Y+6*7+3)==TFTGames::FOOD);

  menuItem=BLOCKS;startBlocks(6000);
  for(int row=14;row<20;++row)for(int col=0;col<10;++col)if((row+col)%4)blocks.board[row][col]=(row+col)%7+1;
  blocks.piece=BlocksGame::T;blocks.rotation=0;blocks.x=3;blocks.y=4;blocks.next=BlocksGame::I;dump("native_blocks_stack",6000);
  BlocksGame blocksBefore=blocks;render(6030);assert(memcmp(&blocksBefore,&blocks,sizeof(blocks))==0);
  // Texture centers distinguish all seven shapes beyond the old 4x4 pixels.
  for(int i=0;i<7;++i){blocks.board[19][i]=i+1;}dump("native_blocks_textures",6100);
  assert(oled.nativeCanvas().getPixel(TFTGames::BLOCK_X+3,TFTGames::BLOCK_Y-2)==TFTGames::FRAME);
  for(int i=0;i<7;++i)assert(oled.nativeCanvas().getPixel(TFTGames::BLOCK_X+i*10+2,TFTGames::BLOCK_Y+19*10+2)==TFTGames::PIECE_COLORS[i]);
  startBlocks(7000);for(int row=16;row<20;++row)for(int col=0;col<10;++col)blocks.board[row][col]=col==4?0:1;
  blocks.board[15][0]=7;blocks.piece=BlocksGame::I;blocks.rotation=1;blocks.x=2;blocks.y=16;press(B,7100);
  assert(clearAnimating&&blocks.score==1200&&blocks.lines==4);blocksBefore=blocks;
  dump("native_blocks_clear_flash",7170);dump("native_blocks_clear_wipe",7510);dump("native_blocks_clear_collapse",7700);
  render(7650);assert(oled.nativeCanvas().getPixel(TFTGames::BLOCK_X+25,TFTGames::BLOCK_Y+185)==TFTGames::FIELD);
  assert(clearAnimating&&memcmp(&blocksBefore,&blocks,sizeof(blocks))==0);
  press(A,7300);assert(screen==BLOCKS_PAUSED);press(A,9300);assert(clearAnimationAt==9100);
  dump("native_blocks_clear_resumed",9510);updateBlocks(9750);assert(!clearAnimating&&blocks.board[19][0]==7);
  dump("native_blocks_after_tetris",9750);assert(saveCheckpoint(9750));
  auto saved=Preferences::flash.at("miniconsole/suspend");assert(resumeCheckpoint(10000)&&Preferences::flash.at("miniconsole/suspend")==saved);
  flipped=false;render(10100);frame.assign(oled.physicalBuffer(),oled.physicalBuffer()+240*135);
  flipped=true;render(10100);for(int i=0;i<240*135;++i)assert(oled.physicalBuffer()[i]==frame[240*135-1-i]);flipped=false;
  puts("Native games passed: actual-frame Snake interpolation/wrap/flip, pause/resume/save, full-length rendering, native textured Blocks/ghost/clear phases, scoring and unchanged rule state.");
}
