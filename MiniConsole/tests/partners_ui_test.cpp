#include "../MiniConsole.ino"
#include <cassert>
const char *dir=nullptr;
void dump(const char *name,uint32_t now){render(now);
#ifdef MINICONSOLE_TFT114
 if(dir){char path[512];snprintf(path,sizeof(path),"%s/%s.ppm",dir,name);FILE *f=fopen(path,"wb");assert(f);fprintf(f,"P6\n240 135\n255\n");auto p=oled.physicalBuffer();for(int i=0;i<240*135;++i){uint16_t v=p[i];uint8_t rgb[]={uint8_t((v>>11)*255/31),uint8_t(((v>>5)&63)*255/63),uint8_t((v&31)*255/31)};fwrite(rgb,1,3,f);}fclose(f);}
#endif
}
void press(Key k,uint32_t t){for(auto &b:buttons)b=Button{};buttons[k].pressed=true;clockNow=t;handleInput(t);}
int main(int argc,char **argv){if(argc>1)dir=argv[1];Preferences::flash.clear();setup();menuItem=SNAKE;startGame(100);press(A,200);assert(pauseSaved);auto solo=Preferences::flash.at("miniconsole/suspend");
 menuItem=PARTNERS;changeScreen(HOME);dump("partners-card",500);press(A,1000);assert(screen==PARTNERS_ROOM&&partnersChoosing);dump("partners-choose",1100);
 press(A,1200);assert(BattleRadio::running&&partners.link.host&&!partnersChoosing);
 PocketPartners::Session join;join.open(false,44,123,1200,0);uint8_t hm[]={1,2,3,4,5,6},jm[]={2,3,4,5,6,7};assert(join.receive(hm,partners.packet(1210),1220));assert(partners.receive(jm,join.packet(1230),1240));
 press(A,1300);join.link.ready=true;partners.receive(jm,join.packet(1310),1320);servicePartners(1320);dump("partners-countdown",1400);
 partners.receive(jm,join.packet(4300),4300);servicePartners(4350);assert(partners.started&&screen==PARTNERS_ROOM);dump("partners-play",4400);assert(Preferences::flash.at("miniconsole/suspend")==solo&&!saveCheckpoint(4400));
 press(UP,4410);partners.receive(jm,join.packet(4420),4420);servicePartners(4450);assert(partners.paused());dump("partners-pause",4500);
 press(A,4510);partners.receive(jm,join.packet(4520),4520);servicePartners(4550);assert(!partners.paused());
 partners.game.key=true;partners.game.players[0].x=208*16;partners.game.players[1].x=220*16;partners.receive(jm,join.packet(4560),4560);servicePartners(4600);assert(partners.game.stage==PocketPartners::CLEARED&&partnersUnlocked==1&&prefs.getUChar("pocketLevel",0)==1);dump("partners-clear",4650);
 press(A,4660);partners.receive(jm,join.packet(4670),4670);servicePartners(4700);assert(partners.game.level==1&&partners.game.stage==PocketPartners::PLAY);dump("partners-room2",4750);
#ifdef MINICONSOLE_TFT114
 render(4800);uint16_t pixels[240*135];memcpy(pixels,oled.physicalBuffer(),sizeof(pixels));flipped=true;render(4800);for(int i=0;i<240*135;++i)assert(pixels[i]==oled.physicalBuffer()[240*135-1-i]);flipped=false;
#endif
 press(UP,4810);partners.receive(jm,join.packet(4820),4820);servicePartners(4850);assert(partners.paused());press(B,4860);partners.receive(jm,join.packet(4870),4870);servicePartners(4900);assert(partnersClosing);servicePartners(5550);assert(screen==HOME&&!BattleRadio::running);assert(Preferences::flash.at("miniconsole/suspend")==solo);
 partnersUnlocked=0;setup();assert(partnersUnlocked==1);
 puts("Pocket Partners UI: library, lobby/countdown, shared pause/resume/exit, level clear/unlock, native render/flip and preserved solo saves passed.");}
