#include "port_config.h"
#include "pico.h"
#include "doomtype.h"
#include "doomkeys.h"
#include "d_event.h"
#include "i_input.h"
#include "m_controls.h"
#include "doom/doomstat.h"
#include "doom/m_menu.h"
#include "doom/g_game.h"
#include "doom/info.h"
#include "DoomPort.h"
#include "DoomWeaponInput.h"
#include "esp_timer.h"
float mouse_acceleration = 2.0;
int mouse_threshold = 10, novert = 0;
static uint32_t previous;
static int64_t bothSince;
static bool chordLatched;
static DoomWeaponInput weaponInput;
volatile bool miniconsole_doom_pause;
extern void miniconsole_doom_pause_save(void);
enum { BTN_A=1, BTN_B=2, BTN_UP=4, BTN_RIGHT=8, BTN_LEFT=16, BTN_DOWN=32, BTN_MENU=64 };
static const uint8_t keys[9][3] = {
  {KEY_RCTRL,KEY_ENTER,'y'}, {' ',KEY_BACKSPACE,'n'}, {KEY_UPARROW,0,0},
  {KEY_RIGHTARROW,0,0}, {KEY_LEFTARROW,0,0}, {KEY_DOWNARROW,0,0}, {KEY_ESCAPE,0,0}, {'[',0,0}, {']',0,0}
};
static void post(int type,int key) {
  event_t e={.type=type,.data1=key,.data2=key<128?key:0,.data3=key<128?key:0}; D_PostEvent(&e);
}
void I_InputInit(void) { key_prevweapon='['; key_nextweapon=']'; }
void I_BindInputVariables(void) {}
void I_StartTextInput(int x1,int y1,int x2,int y2) { (void)x1;(void)y1;(void)x2;(void)y2; }
void I_StopTextInput(void) {}
bool DoomPort_hud(DoomHUD *hud) {
  if(gamestate!=GS_LEVEL || menuactive)return false;
  player_t *p=&players[consoleplayer];
  hud->health=p->health;hud->armor=p->armorpoints;hud->ammo=-1;
  if(p->readyweapon<NUMWEAPONS){int kind=weaponinfo[p->readyweapon].ammo;if(kind<NUMAMMO)hud->ammo=p->ammo[kind];}
  return true;
}
void I_GetEvent(void) {
  uint32_t buttons = miniconsole_doom_buttons();
  int64_t now = esp_timer_get_time();
  if ((buttons & 3) == 3) {
    if (!bothSince) bothSince=now;
    if (!chordLatched && now-bothSince>=300000) { buttons |= BTN_MENU; chordLatched=true; }
    if (chordLatched && now-bothSince>=2000000) miniconsole_doom_exit();
    buttons &= ~3u;
  } else { bothSince=0; if (!(buttons&3)) chordLatched=false; if (chordLatched) buttons &= ~3u; }
  if ((buttons & BTN_MENU) && !(previous & BTN_MENU) && gamestate==GS_LEVEL && usergame && !demoplayback && !menuactive && !miniconsole_doom_pause) {
    printf("Doom pause requested: health=%d ammo=%d mask=%u\n",players[consoleplayer].health,players[consoleplayer].ammo[0],buttons);
    miniconsole_doom_pause_save();
    M_StartControlPanel();miniconsole_doom_pause=true;buttons=0;
  }
  if(miniconsole_doom_pause) {
    uint32_t down=buttons & ~previous;
    if(down & BTN_B)miniconsole_doom_exit();
    if(down & BTN_A){printf("Doom resume\n");M_ClearMenus();miniconsole_doom_pause=false;chordLatched=true;buttons=0;}
    if(down & BTN_UP){printf("Doom native menu\n");miniconsole_doom_pause=false;buttons=0;}
    if(miniconsole_doom_pause)buttons=0;
  }
  buttons=DoomWeaponInput_update(&weaponInput,buttons,(uint32_t)(now/1000),
    gamestate==GS_LEVEL && usergame && !demoplayback && !menuactive && !miniconsole_doom_pause);
#ifdef MINICONSOLE_DOOR_DIAGNOSTICS
  if ((buttons & BTN_B) && !(previous & BTN_B))
    printf("Door input: mask=%u usekey=%d firekey=%d health=%d state=%d menu=%d demo=%d\n",
      buttons,(int)key_use,(int)key_fire,players[consoleplayer].health,
      (int)players[consoleplayer].playerstate,(int)menuactive,(int)demoplayback);
#endif
  uint32_t changed = buttons ^ previous;
  for (int i=0;i<9;++i) if (changed & (1u<<i)) {
    for (int j=0;j<3 && keys[i][j];++j) post(buttons & (1u<<i) ? ev_keydown : ev_keyup, keys[i][j]);
  }
  previous=buttons;
}
