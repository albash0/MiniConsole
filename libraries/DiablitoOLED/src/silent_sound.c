#include "port_config.h"
#include "pico.h"
#include "doomtype.h"
#include "i_sound.h"
#include "i_picosound.h"
#include "DoomPiezo.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
isb_int8_t snd_pitchshift = -1;
uint8_t restart_song_state;
void I_InitSound(boolean prefix) { (void)prefix; }
void I_ShutdownSound(void) {}
int I_GetSfxLumpNum(sfxinfo_t *sfx) { (void)sfx; return -1; }
void I_UpdateSound(void) { vTaskDelay(1); }
void I_UpdateSoundParams(int channel,int volume,int separation) { (void)channel;(void)volume;(void)separation; }
// Keep other sketches using this library link-compatible without a piezo hook.
__attribute__((weak)) void miniconsole_doom_sound(DoomPiezoEvent event) { (void)event; }
int I_StartSound(sfxinfo_t *sfx,int channel,int volume,int separation,int pitch) {
  (void)channel;(void)separation;(void)pitch;
  if(sfx && volume>0)miniconsole_doom_sound(DoomPiezo_classify(sfx->name));
  return -1; // No sampled audio channel, sample cache or mixer is allocated.
}
void I_StopSound(int channel) { (void)channel; }
boolean I_SoundIsPlaying(int channel) { (void)channel; return false; }
void I_PrecacheSounds(sfxinfo_t *sounds,int count) { (void)sounds;(void)count; }
void I_InitMusic(void) {}
void I_ShutdownMusic(void) {}
void I_SetMusicVolume(int volume) { (void)volume; }
void I_PauseSong(void) {}
void I_ResumeSong(void) {}
void *I_RegisterSong(const void *data,int length) { (void)data;(void)length; return NULL; }
void I_UnRegisterSong(void *handle) { (void)handle; }
void I_PlaySong(void *handle,boolean looping) { (void)handle;(void)looping; }
void I_StopSong(void) {}
boolean I_MusicIsPlaying(void) { return false; }
void I_BindSoundVariables(void) {}
void I_SetOPLDriverVer(opl_driver_ver_t version) { (void)version; }
void I_PicoSoundSetMusicGenerator(void (*generator)(audio_buffer_t *)) { (void)generator; }
bool I_PicoSoundIsInitialized(void) { return false; }
void I_PicoSoundFade(bool in) { (void)in; }
bool I_PicoSoundFading(void) { return false; }
uint32_t audio_underrun_count(void) { return 0; }
int audio_queued_ms(void) { return 0; }
