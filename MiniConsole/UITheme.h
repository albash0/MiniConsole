#pragma once
#include <stdint.h>
#include <stddef.h>

namespace UITheme {
enum Role : uint8_t { BACKGROUND, PANEL, ACCENT, TEXT, ROLE_COUNT };
// Custom has a permanent ID, allowing more presets without renaming saved
// custom palettes. Version 1 used ID 6 for Custom; decode migrates that ID.
constexpr uint8_t PRESET_COUNT=11, CUSTOM=255;
constexpr size_t RECORD_SIZE=16;
constexpr uint16_t rgb(uint8_t r,uint8_t g,uint8_t b) {
  return uint16_t((uint16_t(r>>3)<<11)|(uint16_t(g>>2)<<5)|(b>>3));
}
struct Palette {
  uint16_t color[ROLE_COUNT];
  uint8_t preset;
};
inline Palette preset(uint8_t i) {
  static constexpr uint16_t colors[PRESET_COUNT][ROLE_COUNT]={
    {0x0841,0x10C4,0x4E7F,0xEF7D}, // Current cyan.
    {0x0841,0x10C4,0xFD45,0xEF7D},
    {0x0841,0x10C4,0xBAFF,0xEF7D},
    {0x0841,0x10C4,0x67E9,0xEF7D},
    {0x0000,0x1082,0xFFFF,0xFFFF},
    {0xEF7D,0xFFFF,0x0351,0x0841},
    {rgb(248,232,237),rgb(255,245,247),rgb(182,108,134),rgb(73,53,64)}, // Rose.
    {rgb(238,233,248),rgb(250,247,255),rgb(139,117,174),rgb(59,52,80)}, // Lavender.
    {rgb(229,241,232),rgb(245,250,245),rgb(108,146,123),rgb(48,70,56)}, // Sage.
    {rgb(229,240,250),rgb(245,250,255),rgb(107,145,179),rgb(48,69,90)}, // Sky.
    {rgb(249,234,221),rgb(255,248,241),rgb(191,136,105),rgb(83,62,51)}  // Peach.
  };
  if(i>=PRESET_COUNT)i=0;
  Palette p={{colors[i][0],colors[i][1],colors[i][2],colors[i][3]},i};return p;
}
inline const char *name(uint8_t i) {
  static const char *const names[]={"Cyan","Amber","Violet","Mint","Mono","Light","Rose","Lavender","Sage","Sky","Peach"};
  return i==CUSTOM?"Custom":names[i<PRESET_COUNT?i:0];
}
inline bool equal(const Palette &a,const Palette &b) {
  if(a.preset!=b.preset)return false;
  for(uint8_t i=0;i<ROLE_COUNT;++i)if(a.color[i]!=b.color[i])return false;
  return true;
}
inline uint16_t blend(uint16_t a,uint16_t b,uint8_t amount) {
  uint32_t r=(((a>>11)&31)*(255-amount)+((b>>11)&31)*amount)/255;
  uint32_t g=(((a>>5)&63)*(255-amount)+((b>>5)&63)*amount)/255;
  uint32_t blue=((a&31)*(255-amount)+(b&31)*amount)/255;
  return uint16_t((r<<11)|(g<<5)|blue);
}
inline uint8_t lightness(uint16_t c) {
  return uint8_t((((c>>11)&31)*255/31*54+((c>>5)&63)*255/63*183+(c&31)*255/31*19)/256);
}
inline uint16_t muted(const Palette &p) {
  if(p.color[BACKGROUND]==0x0841&&p.color[TEXT]==0xEF7D)return 0x9515;
  return blend(p.color[BACKGROUND],p.color[TEXT],165);
}
inline uint16_t border(const Palette &p) {
  if(p.color[PANEL]==0x10C4&&p.color[TEXT]==0xEF7D)return 0x31A8;
  return blend(p.color[PANEL],p.color[TEXT],80);
}
inline bool lowContrast(const Palette &p) {
  int diff=int(lightness(p.color[TEXT]))-int(lightness(p.color[BACKGROUND]));
  int card=int(lightness(p.color[TEXT]))-int(lightness(p.color[PANEL]));
  return (diff>-65&&diff<65)||(card>-65&&card<65);
}
inline uint8_t channel(uint16_t c,uint8_t i) { return i==0?(c>>11)&31:i==1?(c>>5)&63:c&31; }
inline uint8_t channelMax(uint8_t i) { return i==1?63:31; }
inline uint16_t adjust(uint16_t c,uint8_t i,int delta) {
  int value=int(channel(c,i))+delta;
  if(value<0)value=0;if(value>channelMax(i))value=channelMax(i);
  uint16_t mask=i==0?0xF800:i==1?0x07E0:0x001F;
  return uint16_t((c&~mask)|(uint16_t(value)<<(i==0?11:i==1?5:0)));
}
inline uint32_t checksum(const uint8_t *bytes,size_t size) {
  uint32_t hash=2166136261u;
  for(size_t i=0;i<size;++i){hash^=bytes[i];hash*=16777619u;}return hash;
}
inline void encode(const Palette &p,uint8_t *bytes) {
  bytes[0]='T';bytes[1]='H';bytes[2]=2;bytes[3]=p.preset;
  for(uint8_t i=0;i<ROLE_COUNT;++i){bytes[4+i*2]=uint8_t(p.color[i]);bytes[5+i*2]=uint8_t(p.color[i]>>8);}
  uint32_t hash=checksum(bytes,12);
  for(uint8_t i=0;i<4;++i)bytes[12+i]=uint8_t(hash>>(i*8));
}
inline bool decode(const uint8_t *bytes,size_t size,Palette &p) {
  if(size!=RECORD_SIZE||bytes[0]!='T'||bytes[1]!='H'||(bytes[2]!=1&&bytes[2]!=2))return false;
  uint8_t id=bytes[3];
  if(bytes[2]==1){if(id>6)return false;if(id==6)id=CUSTOM;}
  else if(id>=PRESET_COUNT&&id!=CUSTOM)return false;
  uint32_t stored=0;for(uint8_t i=0;i<4;++i)stored|=uint32_t(bytes[12+i])<<(i*8);
  if(stored!=checksum(bytes,12))return false;
  Palette next={{},id};
  for(uint8_t i=0;i<ROLE_COUNT;++i)next.color[i]=uint16_t(bytes[4+i*2])|(uint16_t(bytes[5+i*2])<<8);
  p=next;return true;
}
} // namespace UITheme
