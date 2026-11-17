#include "../MiniConsole.ino"
#include <cassert>
void press(Key k,uint32_t t){for(auto &b:buttons)b=Button{};buttons[k].pressed=true;clockNow=t;handleInput(t);}
int main(){Preferences::flash.clear();setup();menuItem=BLOCKS;startBlocks(1000);press(A,1100);assert(pauseSaved);auto solo=Preferences::flash.at("miniconsole/suspend");
 changeScreen(BLOCKS_READY);press(A,1200);assert(battleMode&&screen==BATTLE_LOBBY&&portraitScreen());render(1250);
 press(A,1300);assert(BattleRadio::running&&!battleChoosing&&battle.host);uint8_t mac[]={1,2,3,4,5,6};
 BlockBattle::Match join;join.open(false,44,123,1300);assert(join.receive(mac,battle.packet(1310),1320));uint8_t client[]={2,3,4,5,6,7};assert(battle.receive(client,join.packet(1330),1340));
 battle.ready=join.ready=true;battle.receive(client,join.packet(1350),1360);serviceBattle(1360);assert(battle.phase==BlockBattle::COUNTDOWN);render(1370);
 battle.receive(client,join.packet(4300),4301);serviceBattle(4370);assert(screen==BLOCKS_PLAYING&&battle.phase==BlockBattle::RUNNING&&blocks.randomState&&gameSpeed==speed);render(4400);
 assert(Preferences::flash.at("miniconsole/suspend")==solo);assert(!saveCheckpoint(4400));
 auto y=blocks.y;press(A,4500);assert(screen==BLOCKS_PAUSED&&battle.paused);serviceBattle(8000);assert(blocks.y==y);render(8100);
 press(A,8200);serviceBattle(8210);assert(screen==BLOCKS_PLAYING&&!battle.paused);
 // Recover peer liveness then inject one queued garbage line between pieces.
 battle.receive(client,join.packet(8220),8221);serviceBattle(8221);battle.pending=1;
 blocks.hardDrop();afterBlocksLock(8230);assert(!battle.pending&&blocks.board[19][0]<=8);render(8240);
 // A topout is transmitted and resolves as a loss, never a solo high score/save.
 blocks.alive=false;clockNow=8300;finishBlocksIfNeeded();assert(battle.dead);changeScreen(BLOCKS_OVER);press(A,8350);assert(battleMode&&screen==BLOCKS_OVER&&!battleChoosing);serviceBattle(9100);assert(screen==BLOCKS_OVER&&battle.outcome==BlockBattle::LOSS);render(9110);
 assert(Preferences::flash.at("miniconsole/suspend")==solo);press(B,9200);serviceBattle(9801);assert(screen==HOME&&!battleMode&&!BattleRadio::running);
 puts("Block battle UI: entry/lobby/countdown, play/pause, garbage render, result/exit and solo checkpoint preservation passed.");}
