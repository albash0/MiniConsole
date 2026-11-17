#pragma once
#include <sdkconfig.h>
#include <stdio.h>
#include <stdarg.h>
#ifdef __cplusplus
extern "C" {
#endif
int miniconsole_doom_printf(const char *fmt, ...);
int miniconsole_doom_vprintf(const char *fmt, va_list args);
int miniconsole_doom_puts(const char *text);
int miniconsole_doom_putchar(int c);
#ifdef __cplusplus
}
#endif
#define printf miniconsole_doom_printf
#define vprintf miniconsole_doom_vprintf
#define puts miniconsole_doom_puts
#define putchar miniconsole_doom_putchar
#define NDEBUG 1
#define PICO_ON_DEVICE 1
#define PICO_BUILD 1
#define PICO_RP2040 0
#define PICO_RP2350 0
#define LIB_PICO_STDLIB 0
#define USE_INTERP 0
#define USE_CORE1_FOR_FLATS 0
#define USE_CORE1_FOR_REGULAR 0
#define USE_ZONE_FOR_MALLOC 0
#define NO_USE_MOUSE 1
#define NO_USE_ENDDOOM 1
#define SUPPORT_OVERLAYS 1
#define SUPPORT_TEXT 0
#define TINY_WAD_ADDR 0
#define PICO_AUDIO_I2S_DATA_PIN 23
#define PICO_AUDIO_I2S_CLOCK_PIN_BASE 20
#define SHRINK_MOBJ 1
#define DOOM_ONLY 1
#define DOOM_SMALL 1
#define DOOM_CONST 1
#define SOUND_LOW_PASS 1
#define NUM_SOUND_CHANNELS 8
#define NO_USE_CHECKSUM 1
#define NO_USE_RELOAD 1
#define USE_SINGLE_IWAD 1
#define NO_USE_WIPE 1
#define NO_USE_JOYSTICK 1
#define NO_USE_DEH 1
#define NO_USE_MUSIC_PACKS 1
#define USE_FLAT_MAX_256 1
#define USE_MEMMAP_ONLY 1
#define USE_LIGHTMAP_INDEXES 1
#define USE_ERASE_FRAME 1
#define NO_DRAW_MID 1
#define NO_DRAW_TOP 1
#define NO_DRAW_BOTTOM 1
#define NO_DRAW_MASKED 1
#define NO_DRAW_SKY 1
#define NO_DRAW_SPRITES 1
#define NO_DRAW_PSPRITES 1
#define NO_VISPLANE_GUTS 1
#define NO_VISPLANE_CACHES 1
#define NO_DRAWSEGS 1
#define NO_VISSPRITES 1
#define NO_MASKED_FLOOR_CLIP 1
#define PD_DRAW_COLUMNS 1
#define PD_DRAW_MARKERS 1
#define PD_DRAW_PLANES 1
#define PD_SCALE_SORT 1
#define PD_CLIP_WALLS 1
#define PD_QUANTIZE 1
#define PD_SANITY 1
#define PD_COLUMNS 1
#define PICO_DOOM 1
#define NO_USE_DS_COLORMAP 1
#define NO_USE_DC_COLORMAP 1
#define USE_READONLY_MMAP 1
#define NO_USE_TIMIDITY 1
#define NO_USE_GUS 1
#define NO_USE_LIBSAMPLERATE 1
#define NO_USE_STATE_MISC 1
#define USE_RAW_MAPNODE 1
#define USE_RAW_MAPVERTEX 1
#define USE_RAW_MAPSEG 1
#define USE_RAW_MAPLINEDEF 1
#define USE_RAW_MAPTHING 1
#define USE_INDEX_LINEBUFFER 1
#define NO_USE_ZLIGHT 1
#define NO_Z_ZONE_ID 1
#define Z_MALOOC_EXTRA_DATA 1
#define USE_THINKER_POOL 1
#define NO_INTERCEPTS_OVERRUN 1
#define TEMP_IMMUTABLE_DISABLED 1
#define USE_CONST_SFX 1
#define USE_CONST_MUSIC 1
#define NO_DEMO_RECORDING 1
#define PICO_NO_TIMING_DEMO 1
#define NO_USE_EXIT 1
#define DOOM_TINY 1
#define EMU8950_SLOT_RENDER 1
#define NO_RDRAW 1
#define USE_EMU8950_OPL 1
#define USE_DIRECT_MIDI_LUMP 1
#define EMU8950_ASM 0
#define EMU8950_NO_WAVE_TABLE_MAP 1
#define EMU8950_NO_TLL 1
#define EMU8950_NO_FLOAT 1
#define EMU8950_NO_TIMER 1
#define EMU8950_NO_TEST_FLAG 1
#define EMU8950_SIMPLER_NOISE 1
#define EMU8950_SHORT_NOISE_UPDATE_CHECK 1
#define EMU8950_LINEAR_SKIP 1
#define EMU8950_LINEAR_END_OF_NOTE_OPTIMIZATION 1
#define EMU8950_NO_PERCUSSION_MODE 1
#define EMU8950_LINEAR 1
#define OPL_HALF_RATE 1
#define PICO_SOUND_SAMPLE_FREQ 24858
#define NO_USE_NET 1
#define USE_PICO_NET 0
#define NO_FILE_ACCESS 1
#define SAVE_COMPRESSED 1
#define LOAD_COMPRESSED 1
#define NO_USE_ARGS 1
#define NO_USE_SAVE_CONFIG 1
#define NO_USE_FLOAT 1
#define USE_VANILLA_KEYBOARD_MAPPING_ONLY 1
#define NO_USE_LOADING_DISK 1
#define USE_WHD 1
#define NO_Z_MALLOC_USER_PTR 1
#define FIXED_SCREENWIDTH 1
#define FLOOR_CEILING_CLIP_8BIT 1
#define USE_MUSX 1
#define MUSX_COMPRESSED 1
#define NO_SCREENSHOT 1
#define NO_USE_BOUND_CONFIG 1
#define USE_FPS 1
#define USE_MEMORY_WAD 1
#define EMU8950_NO_RATECONV 1
#define NO_ZONE_DEBUG 1
#define PICODOOM_RENDER_NEWHOPE 1
#define MERGE_DISTSCALE0_INTO_VIEWCOSSINANGLE 1

#define DOOM_ZONE_SIZE (64 * 1024)
