#pragma once
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <string>
#include <initializer_list>
#define PROGMEM
#define INPUT_PULLUP 2
#define OUTPUT 3
#define LOW 0
#define HIGH 1
class __FlashStringHelper;
class String: public std::string { public: using std::string::string; };
inline void yield() {}
static uint32_t clockNow=0;
inline uint32_t millis(){return clockNow;}
inline uint32_t micros(){return clockNow*1000;}
inline void delay(int n){clockNow+=n;}
static int pinLevels[49];
inline void pinMode(int pin,int){pinLevels[pin]=1;}
inline int digitalRead(int pin){return pinLevels[pin];}
inline void digitalWrite(int pin,int level){pinLevels[pin]=level;}
inline uint16_t piezoHz=0;
inline bool piezoPWM=false,piezoAttachFailure=false;
inline bool ledcAttach(uint8_t pin,uint32_t,uint8_t){if(piezoAttachFailure)return false;piezoPWM=true;pinLevels[pin]=LOW;return true;}
inline bool ledcWrite(uint8_t pin,uint32_t duty){if(!duty){piezoHz=0;pinLevels[pin]=LOW;}return piezoPWM;}
inline uint32_t ledcWriteTone(uint8_t,uint32_t hz){piezoHz=hz;return piezoPWM?hz:0;}
inline bool ledcDetach(uint8_t){piezoPWM=false;piezoHz=0;return true;}
struct SerialType { void begin(int){} void println(const char*){} template<class... T> void printf(const char*,T...){} };
static SerialType Serial;
inline float radians(float degrees){return degrees*0.017453292519943295f;}
