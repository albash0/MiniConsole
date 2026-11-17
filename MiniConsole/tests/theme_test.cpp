#include "../UITheme.h"
#include <cassert>
#include <cstring>
#include <cstdio>

int main() {
  using namespace UITheme;
  uint8_t bytes[RECORD_SIZE];
  for(uint8_t i=0;i<=PRESET_COUNT;++i) {
    Palette p=preset(i);if(i==PRESET_COUNT){p.preset=CUSTOM;for(auto &c:p.color)c^=0x3579;}
    encode(p,bytes);Palette restored=preset(0);assert(decode(bytes,sizeof(bytes),restored)&&equal(p,restored));
    for(size_t j=0;j<RECORD_SIZE;++j){bytes[j]^=1;Palette before=restored;assert(!decode(bytes,sizeof(bytes),restored)&&equal(before,restored));bytes[j]^=1;}
    for(size_t n=0;n<RECORD_SIZE;++n)assert(!decode(bytes,n,restored));
    assert(!decode(bytes,RECORD_SIZE+1,restored));
  }
  // Existing version-1 presets retain their IDs/colors. Its Custom ID 6 must
  // become permanent ID 255, never the new Rose preset at ID 6.
  for(uint8_t old=0;old<=6;++old){
    Palette original=preset(old<6?old:0);if(old==6){original.preset=CUSTOM;original.color[ACCENT]=0xF545;}
    encode(original,bytes);bytes[2]=1;bytes[3]=old;uint32_t hash=checksum(bytes,12);
    for(uint8_t j=0;j<4;++j)bytes[12+j]=uint8_t(hash>>(j*8));
    Palette restored=preset(0);assert(decode(bytes,sizeof(bytes),restored)&&equal(original,restored));
  }
  Palette rose=preset(6),restored;encode(rose,bytes);assert(decode(bytes,sizeof(bytes),restored)&&restored.preset==6);
  assert(std::strcmp(name(6),"Rose")==0&&std::strcmp(name(CUSTOM),"Custom")==0);
  Palette p=preset(0);p.preset=99;encode(p,bytes);assert(!decode(bytes,sizeof(bytes),p));
  for(uint8_t ch=0;ch<3;++ch)for(unsigned c=0;c<=0xFFFF;c+=31) {
    uint16_t up=adjust(c,ch,1),down=adjust(c,ch,-1);
    assert(channel(up,ch)<=channelMax(ch));
    assert(channel(up,ch)==(channel(c,ch)<channelMax(ch)?channel(c,ch)+1:channelMax(ch)));
    assert(channel(down,ch)==(channel(c,ch)>0?channel(c,ch)-1:0));
    for(uint8_t other=0;other<3;++other)if(ch!=other)assert(channel(up,other)==channel(c,other)&&channel(down,other)==channel(c,other));
  }
  assert(lightness(0)==0&&lightness(0xFFFF)==255);
  for(uint8_t i=0;i<PRESET_COUNT;++i)assert(!lowContrast(preset(i)));
  Palette black={{0,0,0,0},CUSTOM};assert(lowContrast(black));
  puts("Themes passed: all presets, legacy custom/preset migration, 16-byte records, corrupt/version/length rejection, RGB565 channel bounds and contrast.");
}
