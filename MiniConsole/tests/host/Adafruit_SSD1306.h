#pragma once
#include <Adafruit_GFX.h>
#include <Wire.h>
#define WHITE 1
#define BLACK 0
#define SSD1306_SWITCHCAPVCC 2
struct Adafruit_SSD1306: GFXcanvas1 {
Adafruit_SSD1306(int w,int h,TwoWire*,int,int,int):GFXcanvas1(w,h){}
bool begin(int,int,bool,bool){return true;}
void clearDisplay(){fillScreen(0);}
void display(){}
};
