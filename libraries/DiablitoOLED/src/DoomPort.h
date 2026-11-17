#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
bool DoomPort_available(void);
bool DoomPort_start(bool resume);
bool DoomPort_returned(void);
bool DoomPort_saved(void);
bool DoomPort_paused(void);
bool DoomPort_pauseSaved(void);
bool DoomPort_saving(void);
void DoomPort_clearSaved(void);
void DoomPort_dumpFrame(const uint8_t *pixels);
void DoomPort_dumpColorFrame(const uint16_t *pixels);
typedef struct { int health, ammo, armor; } DoomHUD;
bool DoomPort_hud(DoomHUD *hud);
uint32_t miniconsole_doom_buttons(void);
void miniconsole_doom_present(const uint8_t *pixels);
void miniconsole_doom_present_color(const uint16_t *pixels);
void miniconsole_doom_message(const char *message);
void miniconsole_doom_exit(void);
void miniconsole_doom_checkpoint(bool saved);
#ifdef __cplusplus
}
#endif
