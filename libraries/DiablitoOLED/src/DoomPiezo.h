#pragma once
#include <string.h>
// Event categories only: the launcher synthesizes short piezo tones, without
// loading digital SFX samples or adding Doom's original music/mixer buffers.
typedef enum {
  DOOM_PIEZO_NONE,DOOM_PIEZO_MENU,DOOM_PIEZO_CONFIRM,DOOM_PIEZO_BACK,
  DOOM_PIEZO_SHOT,DOOM_PIEZO_EXPLOSION,DOOM_PIEZO_PICKUP,DOOM_PIEZO_HURT,
  DOOM_PIEZO_DOOR,DOOM_PIEZO_ERROR,DOOM_PIEZO_DEATH
} DoomPiezoEvent;
static inline DoomPiezoEvent DoomPiezo_classify(const char *name) {
  if(!name)return DOOM_PIEZO_NONE;
  if(!strcmp(name,"stnmov"))return DOOM_PIEZO_MENU;
  if(!strcmp(name,"swtchn"))return DOOM_PIEZO_CONFIRM;
  if(!strcmp(name,"swtchx"))return DOOM_PIEZO_BACK;
  if(!strcmp(name,"pistol")||!strcmp(name,"shotgn")||!strcmp(name,"dshtgn")||!strcmp(name,"plasma")||!strcmp(name,"rlaunc")||!strcmp(name,"bfg")||!strcmp(name,"punch")||!strcmp(name,"sawhit"))return DOOM_PIEZO_SHOT;
  if(!strcmp(name,"barexp")||!strcmp(name,"rxplod"))return DOOM_PIEZO_EXPLOSION;
  if(!strcmp(name,"itemup")||!strcmp(name,"wpnup")||!strcmp(name,"getpow"))return DOOM_PIEZO_PICKUP;
  if(!strcmp(name,"plpain"))return DOOM_PIEZO_HURT;
  if(!strcmp(name,"doropn")||!strcmp(name,"dorcls")||!strcmp(name,"bdopn")||!strcmp(name,"bdcls"))return DOOM_PIEZO_DOOR;
  if(!strcmp(name,"oof")||!strcmp(name,"noway"))return DOOM_PIEZO_ERROR;
  if(!strcmp(name,"pldeth")||!strcmp(name,"pdiehi"))return DOOM_PIEZO_DEATH;
  return DOOM_PIEZO_NONE; // Ambient monster chatter stays quiet on a single piezo.
}
#ifdef __cplusplus
extern "C" {
#endif
void miniconsole_doom_sound(DoomPiezoEvent event);
#ifdef __cplusplus
}
#endif
