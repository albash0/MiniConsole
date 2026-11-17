#pragma once

// Native TFT playfields. Game rules, board coordinates and save data are shared
// with the OLED profile; only the presentation uses these larger coordinates.
namespace TFTGames {
using TFTUI::c;
using TFTUI::text;
using TFTUI::small;
using TFTUI::center;
uint16_t BG=0x0841,FIELD=0x10C4,FRAME=0x31A8,GRID=0x10C4,INK=0xEF7D;
constexpr int SNAKE_CELL=7,SNAKE_X=15,SNAKE_Y=39,SNAKE_W=210,SNAKE_H=84;
constexpr uint16_t HEAD=UITheme::rgb(115,231,161),TAIL=UITheme::rgb(30,112,74),FOOD=UITheme::rgb(248,118,116),OUTLINE=UITheme::rgb(17,65,47);
constexpr int BLOCK_CELL=10,BLOCK_X=17,BLOCK_Y=30,BLOCK_W=100,BLOCK_H=200;
constexpr uint16_t PIECE_COLORS[]={0x3EFF,0xFFE5,0xBAFF,0x67E9,0xF9C6,0x6B7F,0xFD45};
constexpr uint16_t SOKO_WALL=UITheme::rgb(105,132,156),SOKO_MORTAR=UITheme::rgb(61,83,105),
  SOKO_BOX=UITheme::rgb(224,158,77),SOKO_FILLED=UITheme::rgb(88,198,127),SOKO_TARGET=UITheme::rgb(48,150,204),
  SOKO_SKIN=UITheme::rgb(255,205,154),SOKO_SHIRT=UITheme::rgb(80,154,242),SOKO_OUTLINE=UITheme::rgb(34,48,63);
void useGamePalette() {
  TFTUI::usePalette(matchGameTheme?menuTheme:UITheme::preset(0));
  // The surrounding UI and empty playfield follow the system theme. Sprites,
  // food, pieces, crates, targets and walls keep their gameplay colors.
  BG=TFTUI::BG;FIELD=TFTUI::PANEL;FRAME=TFTUI::BORDER;INK=TFTUI::TEXT;
  GRID=UITheme::blend(FIELD,INK,24);
  if(!matchGameTheme){FIELD=UITheme::rgb(6,14,20);FRAME=UITheme::rgb(55,76,95);GRID=UITheme::rgb(15,26,34);}
}

int floor256(int32_t v){return v>=0?v/256:-int((-v+255)/256);}
SnakeMotion::Point snakePoint(int segment,uint16_t phase) {
  auto p=snakeMotion.point(game,segment,phase);
  return {floor256(p.x*SNAKE_CELL)+SNAKE_CELL/2,floor256(p.y*SNAKE_CELL)+SNAKE_CELL/2};
}
template<class Draw> void snakeCopies(int minX,int minY,int maxX,int maxY,Draw draw) {
  int lo=game.wrap?-1:0,hi=game.wrap?1:0;
  for(int oy=lo;oy<=hi;++oy)for(int ox=lo;ox<=hi;++ox){int dx=ox*SNAKE_W,dy=oy*SNAKE_H;
    if(maxX+dx<0||minX+dx>=SNAKE_W||maxY+dy<0||minY+dy>=SNAKE_H)continue;
    draw(SNAKE_X+dx,SNAKE_Y+dy);
  }
}
void capsule(int x1,int y1,int x2,int y2,int radius,uint16_t color) {
  int left=x1<x2?x1:x2,top=y1<y2?y1:y2;
  int w=(x1>x2?x1-x2:x2-x1)+radius*2+1,h=(y1>y2?y1-y2:y2-y1)+radius*2+1;
  snakeCopies(left-radius,top-radius,left+w-radius-1,top+h-radius-1,[&](int dx,int dy){
    c().fillRoundRect(left+dx-radius,top+dy-radius,w,h,radius,color);
  });
}
void snake(uint32_t now) {
  useGamePalette();c().setRotation(flipped?2:0);c().fillScreen(BG);
  c().fillRect(SNAKE_X,SNAKE_Y,SNAKE_W,SNAKE_H,FIELD);
  for(int y=0;y<SnakeGame::ROWS;++y)for(int x=0;x<SnakeGame::COLS;++x)
    c().drawPixel(SNAKE_X+x*SNAKE_CELL+3,SNAKE_Y+y*SNAKE_CELL+3,GRID);
  uint32_t age=now-lastStep;uint16_t interval=snakeInterval();
  uint16_t phase=age>=interval?256:uint16_t(age*256/interval);
  for(int outline=1;outline>=0;--outline)for(int i=game.length-1;i>0;--i){
    auto head=snakePoint(i-1,phase),tail=snakePoint(i,phase);
    int ax=game.body[i].x*SNAKE_CELL+3,ay=game.body[i].y*SNAKE_CELL+3;
    if(game.wrap){head.x=ax+SnakeMotion::shortest(head.x-ax,SNAKE_W);head.y=ay+SnakeMotion::shortest(head.y-ay,SNAKE_H);
      tail.x=ax+SnakeMotion::shortest(tail.x-ax,SNAKE_W);tail.y=ay+SnakeMotion::shortest(tail.y-ay,SNAKE_H);}
    int radius=(i==game.length-1?1:2)+outline;
    uint16_t color=outline?OUTLINE:UITheme::blend(HEAD,TAIL,i*200/(game.length-1));
    // Bend at the grid corner instead of cutting diagonally through a turn.
    capsule(head.x,head.y,ax,ay,radius,color);capsule(ax,ay,tail.x,tail.y,radius,color);
  }
  auto fruit=snakeMotion.food(game,phase);
  int fx=SNAKE_X+fruit.x*SNAKE_CELL+3,fy=SNAKE_Y+fruit.y*SNAKE_CELL+3;
  int pulse=(now%1000)<500?now%500:500-now%500;
  c().drawCircle(fx,fy,5+pulse/250,UITheme::blend(FIELD,FOOD,25+pulse/12));
  c().fillCircle(fx,fy,3,FOOD);c().drawPixel(fx-1,fy-1,0xFFF8);
  c().drawLine(fx,fy-3,fx+2,fy-5,HEAD);
  auto head=snakePoint(0,phase);
  snakeCopies(head.x-4,head.y-4,head.x+4,head.y+4,[&](int dx,int dy){
    int x=head.x+dx,y=head.y+dy;
    c().fillRoundRect(x-4,y-4,9,9,4,OUTLINE);
    c().fillRoundRect(x-3,y-3,7,7,3,HEAD);
    int vx=game.direction==SnakeGame::RIGHT?1:game.direction==SnakeGame::LEFT?-1:0;
    int vy=game.direction==SnakeGame::DOWN?1:game.direction==SnakeGame::UP?-1:0;
    for(int side=-1;side<=1;side+=2){int ex=x+vx*2-vy*side*2,ey=y+vy*2+vx*side*2;
      c().fillCircle(ex,ey,1,0xFFFF);c().drawPixel(ex+vx,ey+vy,0x0020);}
  });
  // Clip wrapped body/food effects to the arena, then draw its frame and HUD.
  c().fillRect(0,0,240,SNAKE_Y,BG);c().fillRect(0,SNAKE_Y+SNAKE_H,240,135-SNAKE_Y-SNAKE_H,BG);
  c().fillRect(0,SNAKE_Y,SNAKE_X,SNAKE_H,BG);c().fillRect(SNAKE_X+SNAKE_W,SNAKE_Y,240-SNAKE_X-SNAKE_W,SNAKE_H,BG);
  c().drawRoundRect(SNAKE_X-2,SNAKE_Y-2,SNAKE_W+4,SNAKE_H+4,4,FRAME);
  text(13,19,"SNAKE",TFTUI::ACCENT,&FreeSansBold9pt7b);char value[24];
  uint16_t score=game.score();if(snakeMotion.moving&&snakeMotion.ate&&phase<256&&score)--score;
  snprintf(value,sizeof(value),"%u",score);text(226-TFTUI::textWidth(value,&FreeSansBold12pt7b),23,value,INK,&FreeSansBold12pt7b);
  snprintf(value,sizeof(value),"BEST %u",best);small(14,25,value);
  small(15,128,SPEED_NAMES[gameSpeed]);small(164,128,"A/B: PAUSE");
}

void tile(int x,int y,int size,uint8_t type,bool ghost=false) {
  uint16_t color=type<7?PIECE_COLORS[type]:UITheme::rgb(130,144,153);
  if(ghost){c().drawRoundRect(x+2,y+2,size-4,size-4,1,UITheme::blend(FIELD,color,140));return;}
  uint16_t light=UITheme::blend(color,0xFFFF,115),dark=UITheme::blend(color,0x0000,120);
  c().fillRoundRect(x+1,y+1,size-1,size-1,1,dark);
  c().fillRect(x+2,y+2,size-3,size-3,color);
  c().drawFastHLine(x+2,y+1,size-3,light);c().drawFastVLine(x+1,y+2,size-3,light);
  if(size<9)return;
  switch(type){
    case BlocksGame::I:c().drawFastHLine(x+3,y+3,4,light);break;
    case BlocksGame::O:c().drawRect(x+3,y+3,4,4,dark);break;
    case BlocksGame::T:for(int r=3;r<8;r+=2)for(int col=3;col<8;col+=2)c().drawPixel(x+col,y+r,dark);break;
    case BlocksGame::S:c().drawFastHLine(x+3,y+4,5,dark);c().drawFastHLine(x+3,y+6,5,dark);break;
    case BlocksGame::Z:c().drawFastVLine(x+4,y+3,5,dark);c().drawFastVLine(x+6,y+3,5,dark);break;
    case BlocksGame::J:c().drawLine(x+3,y+7,x+7,y+3,dark);c().drawLine(x+3,y+4,x+4,y+3,dark);break;
    case BlocksGame::L:c().drawRect(x+3,y+3,5,5,dark);c().drawFastHLine(x+4,y+5,3,dark);break;
  }
}
void blockCell(int col,int row,uint8_t type,bool ghost=false) {
  if(col<0||col>=10||row<0||row>=20)return;
  tile(BLOCK_X+col*BLOCK_CELL,BLOCK_Y+row*BLOCK_CELL,BLOCK_CELL,type,ghost);
}
void clearing(uint32_t now) {
  uint32_t elapsed=now-clearAnimationAt,duration=clearAnimationDuration(),flash=clearNoticeLines==4?300:180,collapseAt=duration-100;
  bool collapsing=elapsed>=collapseAt;
  uint32_t progress=collapsing?elapsed-collapseAt:0;if(progress>100)progress=100;
  for(int row=0;row<20;++row){bool cleared=blocks.clearedRows&(uint32_t(1)<<row);
    if(collapsing&&cleared)continue;
    int drop=0;if(collapsing)for(int r=row+1;r<20;++r)if(blocks.clearedRows&(uint32_t(1)<<r))++drop;
    int y=BLOCK_Y+row*10+drop*10*progress/100;
    for(int col=0;col<10;++col)if(blocks.clearBoard[row][col])tile(BLOCK_X+col*10,y,10,blocks.clearBoard[row][col]-1);
    if(!cleared)continue;
    uint16_t glow=clearNoticeLines==4?0xFFE5:0xBFFF;
    if(elapsed<flash){int strength=(elapsed%100)<50?elapsed%50:50-elapsed%50;
      c().fillRect(BLOCK_X,y+1,BLOCK_W,9,UITheme::blend(glow,BG,100+strength*3));
    }else if(elapsed<collapseAt){int half=50*(elapsed-flash)/(collapseAt-flash);
      c().fillRect(BLOCK_X+50-half,y,half*2,10,FIELD);
      for(int side=-1;side<=1;side+=2)for(int p=0;p<3;++p){int x=BLOCK_X+50+side*(half+p*3),py=y+2+(p*3+int(elapsed/30))%7;
        if(x>=BLOCK_X&&x<BLOCK_X+BLOCK_W)c().drawPixel(x,py,glow);}
    }
  }
}
void blocksFrame(uint32_t now) {
  useGamePalette();c().setRotation((3+(flipped?2:0))&3);c().fillScreen(BG);
  c().fillRect(BLOCK_X,BLOCK_Y,BLOCK_W,BLOCK_H,FIELD);
  for(int col=1;col<10;++col)c().drawFastVLine(BLOCK_X+col*10,BLOCK_Y,BLOCK_H,GRID);
  for(int row=1;row<20;++row)c().drawFastHLine(BLOCK_X,BLOCK_Y+row*10,BLOCK_W,GRID);
  if(clearAnimating)clearing(now);
  else {
    for(int row=0;row<20;++row)for(int col=0;col<10;++col)if(::blocks.board[row][col])blockCell(col,row,::blocks.board[row][col]-1);
    if(::blocks.alive){int ghost=::blocks.ghostY();
      for(int row=0;row<4;++row)for(int col=0;col<4;++col)if(BlocksGame::cell(::blocks.piece,::blocks.rotation,col,row)){
        if(ghost!=::blocks.y)blockCell(::blocks.x+col,ghost+row,::blocks.piece,true);
        blockCell(::blocks.x+col,::blocks.y+row,::blocks.piece);
      }
    }
  }
  c().drawRoundRect(BLOCK_X-2,BLOCK_Y-2,BLOCK_W+4,BLOCK_H+4,3,FRAME);
  char value[32];snprintf(value,sizeof(value),"%lu",(unsigned long)(::blocks.score>999999?999999 : ::blocks.score));text(7,15,value,INK,&FreeSansBold9pt7b);
  snprintf(value,sizeof(value),"LV%u  L%u",::blocks.level()>99?99 : ::blocks.level(),::blocks.lines>999?999 : ::blocks.lines);small(7,20,value);
  small(104,0,"NEXT");uint8_t next=clearAnimating?::blocks.piece : ::blocks.next;
  for(int row=0;row<4;++row)for(int col=0;col<4;++col)if(BlocksGame::cell(next,0,col,row))tile(105+col*6,10+row*6,6,next);
  if(clearNoticeLines&&now-clearNoticeAt<1200){const char *names[]={"","SINGLE","DOUBLE","TRIPLE","TETRIS!"};
    snprintf(value,sizeof(value),"%s +%lu",names[clearNoticeLines],(unsigned long)clearNoticePoints);
    small((135-int(strlen(value))*6)/2,232,value,clearNoticeLines==4?0xFFE5:0xBFFF);
  }else small(22,232,"UP:ROTATE B:DROP");
}
constexpr int SOKO_CELL=15,SOKO_X=60,SOKO_Y=27;
void sokobanFrame() {
  useGamePalette();c().setRotation(flipped?2:0);c().fillScreen(BG);
  text(8,18,"SOKOBAN",TFTUI::ACCENT,&FreeSansBold9pt7b);char value[32];
  snprintf(value,sizeof(value),"M:%u P:%u",sokoban.moves>9999?9999:sokoban.moves,sokoban.pushes>9999?9999:sokoban.pushes);
  small(232-int(strlen(value))*6,10,value,INK);
  snprintf(value,sizeof(value),"LEVEL %u",sokoban.level+1);small(8,25,value);
  c().fillRoundRect(SOKO_X-2,SOKO_Y-2,124,94,3,FRAME);
  c().fillRect(SOKO_X,SOKO_Y,120,90,FIELD);
  for(int cell=0;cell<48;++cell) {
    int x=SOKO_X+(cell%8)*15,y=SOKO_Y+(cell/8)*15;uint64_t bit=SokobanGame::cellMask(cell);
    if(sokoban.walls&bit) {
      c().fillRoundRect(x+1,y+1,14,14,2,SOKO_WALL);c().drawRoundRect(x+1,y+1,14,14,2,SOKO_MORTAR);
      c().drawFastHLine(x+2,y+7,12,SOKO_MORTAR);c().drawFastVLine(x+7,y+2,5,SOKO_MORTAR);c().drawFastVLine(x+5,y+8,5,SOKO_MORTAR);
    } else {
      if(sokoban.goals&bit){c().drawCircle(x+7,y+7,4,SOKO_TARGET);c().fillCircle(x+7,y+7,1,SOKO_TARGET);}
      if(sokoban.boxes&bit) {
        c().fillRoundRect(x+2,y+2,12,12,2,(sokoban.goals&bit)?SOKO_FILLED:SOKO_BOX);c().drawRoundRect(x+2,y+2,12,12,2,SOKO_OUTLINE);
        if(sokoban.goals&bit){c().drawLine(x+4,y+7,x+6,y+10,SOKO_OUTLINE);c().drawLine(x+6,y+10,x+11,y+5,SOKO_OUTLINE);}
        else {c().drawLine(x+4,y+4,x+11,y+11,SOKO_OUTLINE);c().drawLine(x+4,y+11,x+11,y+4,SOKO_OUTLINE);}
      }
      if(cell==sokoban.player) {
        c().fillCircle(x+7,y+5,4,SOKO_SKIN);c().fillRoundRect(x+3,y+8,9,6,2,SOKO_SHIRT);
        c().drawRoundRect(x+3,y+8,9,6,2,SOKO_OUTLINE);c().drawPixel(x+8,y+4,SOKO_OUTLINE);
      }
    }
  }
  small(51,125,"A: UNDO    B: PAUSE",INK);
}
bool render(uint32_t now) {
  if(screen==PLAYING){snake(now);return true;}
  if(screen==BLOCKS_PLAYING){blocksFrame(now);return true;}
  if(screen==NEW_PLAYING&&menuItem==SOKOBAN){sokobanFrame();return true;}
  return false;
}
} // namespace TFTGames
