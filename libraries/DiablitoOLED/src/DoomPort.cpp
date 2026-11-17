#include <Arduino.h>
#include "port_config.h"
#include "DoomPort.h"
#include "esp_partition.h"
#include "esp_heap_caps.h"
#include "esp_rom_crc.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
extern "C" {
void D_DoomMain(void);
void I_Init(void);
void W_Memory_SetBase(const uint8_t *base);
extern char _bss_end;
extern volatile bool miniconsole_doom_pause;
extern volatile uint8_t miniconsole_doom_save_pending;
}
static const uint8_t *wadBase;
static esp_partition_mmap_handle_t wadMap;
// Ordinary RTC_DATA is initialized again on a software reset. NOINIT retains
// this one-shot launcher marker; reset reason prevents using it on power-up.
RTC_NOINIT_ATTR static uint32_t returnMarker;
static SemaphoreHandle_t logLock;
static bool resumeOnStart;
static volatile bool pauseStored;
extern "C" int miniconsole_doom_vprintf(const char *fmt, va_list args) {
  char line[512]; int n=vsnprintf(line,sizeof(line),fmt,args);
  if(logLock)xSemaphoreTake(logLock,portMAX_DELAY);
  if(n>0)Serial.write(reinterpret_cast<const uint8_t *>(line),n<int(sizeof(line))?n:sizeof(line)-1);
  if(logLock)xSemaphoreGive(logLock);
  return n;
}
extern "C" int miniconsole_doom_printf(const char *fmt, ...) {
  va_list args;va_start(args,fmt);int n=miniconsole_doom_vprintf(fmt,args);va_end(args);return n;
}
extern "C" int miniconsole_doom_puts(const char *text) { return miniconsole_doom_printf("%s\n",text); }
extern "C" int miniconsole_doom_putchar(int c) { return miniconsole_doom_printf("%c",c); }
void DoomPort_dumpFrame(const uint8_t *pixels) {
  static char text[2060];
  static const char digits[]="0123456789ABCDEF";
  memcpy(text,"OLED_FRAME ",11);
  for(int i=0;i<1024;++i){text[11+i*2]=digits[pixels[i]>>4];text[12+i*2]=digits[pixels[i]&15];}
  text[2059]='\n';
  if(logLock)xSemaphoreTake(logLock,portMAX_DELAY);
  Serial.write(reinterpret_cast<const uint8_t *>(text),sizeof(text)); Serial.flush();
  if(logLock)xSemaphoreGive(logLock);
}
void DoomPort_dumpColorFrame(const uint16_t *pixels) {
  if (!pixels) return;
  if(logLock)xSemaphoreTake(logLock,portMAX_DELAY);
  Serial.println("TFT_BEGIN 240 135"); Serial.flush();
  for (int y=0;y<135;++y) {
    char line[980]; int n=snprintf(line,sizeof(line),"TFT_ROW %d ",y);
    for (int x=0;x<240;++x) n+=snprintf(line+n,sizeof(line)-n,"%04X",pixels[y*240+x]);
    Serial.println(line); Serial.flush();
    vTaskDelay(1);
  }
  Serial.println("TFT_END"); Serial.flush();
  if(logLock)xSemaphoreGive(logLock);
}
bool DoomPort_returned(void) { bool returned=esp_reset_reason()==ESP_RST_SW && returnMarker==0x444F4F4D; returnMarker=0; return returned; }
bool DoomPort_available(void) {
  if (wadBase) return true;
  const esp_partition_t *part=esp_partition_find_first(ESP_PARTITION_TYPE_DATA,static_cast<esp_partition_subtype_t>(0x40),"wad");
  if (!part || part->size<2073676) return false;
  const void *mapped;
  if (esp_partition_mmap(part,0,part->size,ESP_PARTITION_MMAP_DATA,&mapped,&wadMap)!=ESP_OK) return false;
  // An erased asset partition must not be handed to the compressed-lump decoder.
  const uint32_t *first=static_cast<const uint32_t *>(mapped);
  if (memcmp(mapped,"IWHD",4) || first[3]!=2073676) { esp_partition_munmap(wadMap); return false; }
  wadBase=static_cast<const uint8_t *>(mapped); return true;
}
bool DoomPort_saved(void) {
  const esp_partition_t *part=esp_partition_find_first(ESP_PARTITION_TYPE_DATA,static_cast<esp_partition_subtype_t>(0x41),"saves");
  uint32_t h[3];
  if(!part || esp_partition_read(part,7*0x4000,h,sizeof(h))!=ESP_OK || h[0]!=0x47535644 || !h[1] || h[1]>0x4000-sizeof(h))return false;
  uint8_t bytes[512]; uint32_t crc=0;
  for(uint32_t at=0;at<h[1];at+=sizeof(bytes)) {
    size_t count=h[1]-at<sizeof(bytes)?h[1]-at:sizeof(bytes);
    if(esp_partition_read(part,7*0x4000+sizeof(h)+at,bytes,count)!=ESP_OK)return false;
    crc=esp_rom_crc32_le(crc,bytes,count);
  }
  return crc==h[2];
}
void DoomPort_clearSaved(void) {
  const esp_partition_t *part=esp_partition_find_first(ESP_PARTITION_TYPE_DATA,static_cast<esp_partition_subtype_t>(0x41),"saves");
  if(part)esp_partition_erase_range(part,7*0x4000,0x4000);
  pauseStored=false;miniconsole_doom_checkpoint(false);
}
bool DoomPort_paused(void) { return miniconsole_doom_pause; }
bool DoomPort_pauseSaved(void) { return pauseStored; }
bool DoomPort_saving(void) { return miniconsole_doom_save_pending; }
extern "C" bool miniconsole_doom_should_resume(void) { bool r=resumeOnStart;resumeOnStart=false;return r; }
extern "C" void miniconsole_doom_saved(bool ok) { pauseStored=ok;miniconsole_doom_checkpoint(ok); }
extern "C" void miniconsole_doom_begin_save(void) { pauseStored=false; }
static void runDoom(void *) {
  miniconsole_doom_printf("Doom: bss end=%p, PSRAM=%u bytes, heap=%u\n",&_bss_end,ESP.getPsramSize(),ESP.getFreeHeap());
  if (reinterpret_cast<uintptr_t>(&_bss_end)>0x3FCD0000) {
    miniconsole_doom_message("Doom SRAM limit"); vTaskDelay(pdMS_TO_TICKS(2500)); miniconsole_doom_exit();
  }
  W_Memory_SetBase(wadBase); I_Init(); D_DoomMain();
  miniconsole_doom_exit();
}
bool DoomPort_start(bool resume) {
  if (!DoomPort_available()) return false;
  resumeOnStart=resume && DoomPort_saved();pauseStored=false;
  if(!logLock)logLock=xSemaphoreCreateMutex();
  Serial.setTxBufferSize(4096);
  return xTaskCreatePinnedToCore(runDoom,"doom-game",16384,nullptr,1,nullptr,1)==pdPASS;
}
void miniconsole_doom_exit(void) { returnMarker=0x444F4F4D; ESP.restart(); for (;;) vTaskDelay(1000); }
