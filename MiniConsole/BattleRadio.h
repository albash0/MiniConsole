#pragma once
#include "BlockBattle.h"
#ifndef MINICONSOLE_HOST_TEST
#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_netif.h>
#include <esp_mac.h>
#include <freertos/FreeRTOS.h>
#include <freertos/portmacro.h>
#endif
// Callbacks only copy validated-size bytes to a bounded mailbox. All game,
// peer-management, display and sound work happens on the Arduino loop.
namespace BattleRadio {
constexpr int MAX_PACKET=128;
struct Message{uint8_t mac[6],data[MAX_PACKET];uint8_t length=0;};
inline bool running=false;
#ifndef MINICONSOLE_HOST_TEST
inline portMUX_TYPE mux=portMUX_INITIALIZER_UNLOCKED;
inline uint32_t received=0;
inline Message queue[8];inline uint8_t head=0,count=0;
inline const uint8_t broadcast[6]={255,255,255,255,255,255};
inline void receive(const esp_now_recv_info_t *info,const uint8_t *data,int length){
 if(!info||!data||(length!=BlockBattle::PACKET_SIZE&&length!=MAX_PACKET))return;
 portENTER_CRITICAL(&mux);++received;if(running&&count<8){auto &m=queue[(head+count)%8];memcpy(m.mac,info->src_addr,6);memcpy(m.data,data,length);m.length=length;++count;}portEXIT_CRITICAL(&mux);
}
inline bool peer(const uint8_t *mac){if(esp_now_is_peer_exist(mac))return true;esp_now_peer_info_t p{};memcpy(p.peer_addr,mac,6);p.channel=6;p.ifidx=WIFI_IF_STA;p.encrypt=false;return esp_now_add_peer(&p)==ESP_OK;}
inline void stop(){if(!running)return;esp_now_unregister_recv_cb();esp_now_deinit();esp_wifi_stop();esp_wifi_deinit();portENTER_CRITICAL(&mux);running=false;head=count=0;portEXIT_CRITICAL(&mux);}
inline bool begin(){
 if(running)return true;
 esp_err_t e=esp_netif_init();if(e!=ESP_OK&&e!=ESP_ERR_INVALID_STATE)return false;
 e=esp_event_loop_create_default();if(e!=ESP_OK&&e!=ESP_ERR_INVALID_STATE)return false;
 wifi_init_config_t cfg=WIFI_INIT_CONFIG_DEFAULT();cfg.static_rx_buf_num=4;cfg.dynamic_rx_buf_num=8;
 if(esp_wifi_init(&cfg)!=ESP_OK)return false;
 if(esp_wifi_set_storage(WIFI_STORAGE_RAM)!=ESP_OK||esp_wifi_set_mode(WIFI_MODE_STA)!=ESP_OK||esp_wifi_start()!=ESP_OK){esp_wifi_deinit();return false;}
 esp_wifi_set_ps(WIFI_PS_NONE);
 // Moderate output avoids overdriving the tiny SuperMini antenna at close range.
 esp_wifi_set_max_tx_power(44); // quarter-dBm units: 11 dBm

 if(esp_wifi_set_channel(6,WIFI_SECOND_CHAN_NONE)!=ESP_OK||esp_now_init()!=ESP_OK){esp_wifi_stop();esp_wifi_deinit();return false;}
 running=true;
 if(!peer(broadcast)||esp_now_register_recv_cb(receive)!=ESP_OK){stop();return false;}
 uint8_t mac[6];esp_read_mac(mac,ESP_MAC_WIFI_STA);Serial.printf("Battle radio ready ch6 MAC=%02X:%02X:%02X:%02X:%02X:%02X heap=%u\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],ESP.getFreeHeap());return true;
}
inline bool poll(Message &m){portENTER_CRITICAL(&mux);bool ok=count;if(ok){m=queue[head];head=(head+1)%8;--count;}portEXIT_CRITICAL(&mux);return ok;}
inline bool sendRaw(const uint8_t *mac,const uint8_t *data,int length){
 if(!running||length<1||length>MAX_PACKET)return false;const uint8_t *dest=mac?mac:broadcast;
 if(!peer(dest))return false;return esp_now_send(dest,data,length)==ESP_OK;
}
inline bool send(const uint8_t *mac,const BlockBattle::Packet &p){uint8_t data[BlockBattle::PACKET_SIZE];BlockBattle::encode(p,data);return sendRaw(mac,data,sizeof(data));}
#else
inline bool begin(){running=true;return true;}
inline void stop(){running=false;}
inline bool poll(Message &){return false;}
inline bool send(const uint8_t *,const BlockBattle::Packet &){return running;}
inline bool sendRaw(const uint8_t *,const uint8_t *,int){return running;}
#endif
}
