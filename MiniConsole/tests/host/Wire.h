#pragma once
#include <cstdint>
struct TwoWire {void begin(int,int){} void setClock(int){} void setTimeOut(int){} void beginTransmission(int){} int endTransmission(){return 0;}};
static TwoWire Wire;
