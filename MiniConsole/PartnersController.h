#pragma once
void openPartners(uint32_t now){
 BattleRadio::stop();battleMode=false;partners=PocketPartners::Session{};partnersChoosing=true;partnersError=false;partnersClosing=false;partnersLevel=0;changeScreen(PARTNERS_ROOM);
}
void closePartners(){BattleRadio::stop();partnersClosing=false;changeScreen(HOME);}
void handlePartnersInput(uint32_t now){
 using namespace PocketPartners;
 if(partnersChoosing){
  if(buttons[B].pressed){closePartners();return;}
  if(buttons[LEFT].pressed||buttons[RIGHT].pressed)partnersHost=!partnersHost;
  if(buttons[UP].pressed)partnersLevel=(partnersLevel+partnersUnlocked)%(partnersUnlocked+1);
  if(buttons[DOWN].pressed)partnersLevel=(partnersLevel+1)%(partnersUnlocked+1);
  if(buttons[A].pressed){partnersError=!BattleRadio::begin();if(!partnersError){partners.open(partnersHost,esp_random(),esp_random(),now,partnersLevel);partnersChoosing=false;partnersSentAt=now-100;}}
  return;
 }
 if(partners.link.phase==BlockBattle::LOBBY||partners.link.phase==BlockBattle::SEEK){
  if(buttons[B].pressed){closePartners();return;}
  if(buttons[A].pressed&&partners.link.paired)partners.link.ready=!partners.link.ready;
  return;
 }
 if(partners.disconnected(now)){if(buttons[B].pressed)closePartners();return;}
 if(!partners.started)return;
 Input &in=partners.inputs[partners.local()];
 in.held=(buttons[LEFT].held?MOVE_LEFT:0)|(buttons[RIGHT].held?MOVE_RIGHT:0)|(buttons[B].held?PUSH:0);
 if(buttons[A].pressed){++in.jump;if(partners.game.stage==PLAY&&!partners.paused())playSound(Sound::Bounce,now);}
 if(buttons[UP].pressed)++in.pause;
 if(buttons[DOWN].pressed&&partners.paused())++in.retry;
 if(buttons[B].pressed&&partners.paused())++in.quit;
}
void servicePartners(uint32_t now){
 if(screen!=PARTNERS_ROOM||partnersChoosing)return;
 auto oldStage=partners.game.stage;bool oldKey=partners.game.key,oldGate=partners.game.gate,oldPause=partners.paused();uint8_t oldLevel=partners.game.level;
 BattleRadio::Message message;while(BattleRadio::poll(message)){PocketPartners::Packet p;if(PocketPartners::decode(message.data,message.length,p))partners.receive(message.mac,p,now);}
 partners.update(now);
 if(partners.started){
  if(partners.game.stage==PocketPartners::CLEARED||partners.game.stage==PocketPartners::COMPLETE){
   uint8_t unlocked=partners.game.level+1;if(unlocked>=PocketPartners::LEVELS)unlocked=PocketPartners::LEVELS-1;
   if(unlocked>partnersUnlocked){partnersUnlocked=unlocked;if(storageReady)prefs.putUChar("pocketLevel",unlocked);}
  }
  if(oldStage!=partners.game.stage)playSound(partners.game.stage==PocketPartners::FAILED?Sound::Hurt:Sound::Win,now);
  else if(oldLevel!=partners.game.level)playSound(Sound::Start,now);
  else if(!oldKey&&partners.game.key)playSound(Sound::Pickup,now);
  else if(oldGate!=partners.game.gate)playSound(Sound::Door,now);
  else if(oldPause!=partners.paused())playSound(partners.paused()?Sound::Pause:Sound::Resume,now);
  if(partners.game.stage==PocketPartners::CLOSED&&!partnersClosing){partnersClosing=true;partnersClosingAt=now;}
 }
 if(now-partnersSentAt>=20){partnersSentAt=now;auto p=partners.packet(now);uint8_t data[PocketPartners::PACKET_SIZE];PocketPartners::encode(p,data);BattleRadio::sendRaw(partners.link.paired?partners.link.peer:nullptr,data,sizeof(data));}
 if(partnersClosing&&now-partnersClosingAt>600)closePartners();
}
