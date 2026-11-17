#include "../SnakeMotion.h"
#include <cassert>
#include <cstdio>
#include <initializer_list>

int norm(int v,int size){return (v%size+size)%size;}
void endpoints(const SnakeGame &g,const SnakeMotion &m,const SnakeGame &old){
  for(int i=0;i<g.length;++i){auto a=m.point(g,i,0),b=m.point(g,i,256);
    auto from=old.body[i<old.length?i:old.length-1];
    assert(a.x==from.x*256&&a.y==from.y*256);
    assert(norm(b.x,SnakeGame::COLS*256)==g.body[i].x*256&&norm(b.y,SnakeGame::ROWS*256)==g.body[i].y*256);
    auto clamped=m.point(g,i,500);assert(clamped.x==b.x&&clamped.y==b.y);
  }
}
int main(){
  SnakeGame g;g.reset(false);SnakeMotion m;auto a=m.point(g,0,0);assert(a.x==15*256&&a.y==6*256);
  SnakeGame old=g;m.beforeStep(g);assert(g.step()==SnakeGame::MOVED);endpoints(g,m,old);
  a=m.point(g,0,128);assert(a.x==15*256+128&&a.y==6*256);
  g.queueTurn(SnakeGame::UP);old=g;m.beforeStep(g);assert(g.step()==SnakeGame::MOVED);endpoints(g,m,old);
  a=m.point(g,0,128);assert(a.x==16*256&&a.y==6*256-128);
  g.reset(false);g.food={16,6};old=g;m.beforeStep(g);auto result=g.step();assert(result==SnakeGame::ATE);m.afterStep(result);endpoints(g,m,old);
  assert(g.placeFood(42));assert(SnakeGame::equal(m.food(g,0),old.food)&&SnakeGame::equal(m.food(g,256),g.food));
  auto tail=m.point(g,g.length-1,128);assert(tail.x==old.body[3].x*256&&tail.y==old.body[3].y*256);
  for(auto direction:{SnakeGame::LEFT,SnakeGame::RIGHT,SnakeGame::UP,SnakeGame::DOWN}){
    g.reset(true);g.direction=direction;g.food={10,4};
    for(int i=0;i<4;++i){if(direction==SnakeGame::LEFT)g.body[i]={int8_t(i),6};
      if(direction==SnakeGame::RIGHT)g.body[i]={int8_t(29-i),6};
      if(direction==SnakeGame::UP)g.body[i]={15,int8_t(i)};
      if(direction==SnakeGame::DOWN)g.body[i]={15,int8_t(11-i)};}
    old=g;m.beforeStep(g);assert(g.step()==SnakeGame::MOVED);endpoints(g,m,old);
    a=m.point(g,0,0);auto half=m.point(g,0,128),end=m.point(g,0,256);
    assert((half.x-a.x)*(half.x-a.x)+(half.y-a.y)*(half.y-a.y)==128*128);
    assert((end.x-a.x)*(end.x-a.x)+(end.y-a.y)*(end.y-a.y)==256*256);
  }
  m.reset();a=m.point(g,0,128);assert(a.x==g.body[0].x*256&&a.y==g.body[0].y*256);
  puts("Snake motion passed: sub-cell movement, turns, growth/tail history, all four wrap edges, phase clamping and resume reset.");
}
