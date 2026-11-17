#include "../../libraries/DiablitoOLED/src/DoomWeaponInput.h"
#include <cassert>
#include <cstdio>
#include <initializer_list>

int main() {
  constexpr uint32_t A=1,B=2,U=4,R=8,L=16,D=32,MENU=64,PREV=128,NEXT=256;
  DoomWeaponInput s={};
  auto input=[&](uint32_t keys,uint32_t t,int game=1){return DoomWeaponInput_update(&s,keys,t,game);};
  // Reproduce using a door while turning in either direction. No weapon change
  // and no lost Use action, even if both buttons stay held beyond the threshold.
  for(auto turn:{L,R}) {
    s={};assert(input(turn,0)==turn);
    assert(input(turn|B,10)==(turn|B));
    assert(input(turn|B,1000)==(turn|B));
    assert(input(turn,1010)==turn);assert(input(0,1020)==0);
    assert(input(turn|B,1100)==(turn|B));assert(input(turn|B,2000)==(turn|B));
  }
  s={};assert(input(B,0)==B);assert(input(B|R,399)==(B|R));
  assert(input(B|R,800)==(B|R)); // An early turn cannot become a late shortcut.
  assert(input(B,810)==B);assert(input(B|R,820)==(B|NEXT));
  assert(input(B|R,900)==(B|NEXT)); // Held direction remains one key-down.
  assert(input(B,910)==B);assert(input(B|L,920)==(B|PREV));
  assert(input(L,930)==L); // Releasing B restores ordinary turning.
  s={};assert(input(B,100)==B);assert(input(B|L|R,600)==(B|L|R));
  // Menus, pause input, fire and movement keep their original meanings.
  assert(input(B|L,610,0)==(B|L));assert(input(B|L,2000,1)==(B|L));
  assert(input(A|B|U,2100)==(A|B|U));assert(input(A|D|MENU,2110)==(A|D|MENU));
  // Hold duration remains correct across the 32-bit millis wrap.
  s={};assert(input(B,UINT32_MAX-199)==B);assert(input(B,200)==B);
  assert(input(B|L,201)==(B|PREV));
  puts("Doom input passed: Use while turning, deliberate weapon holds/edges, menus, pause, release, simultaneous directions and clock wrap.");
}
