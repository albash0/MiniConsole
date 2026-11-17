#include "../PartyGames.h"
#include <cassert>
#include <cstdio>
#include <limits>
using namespace PartyGames;
int main() {
  ReactionDuel duel;
  duel.begin(100, 23); assert(duel.waitMs >= 1500 && duel.waitMs <= 5000);
  duel.update(100 + duel.waitMs - 1, true, false);
  assert(duel.phase == ReactionDuel::RESULT && duel.falseStart && duel.winner == ReactionDuel::PLAYER_B && duel.scoreB == 1);
  duel.update(9000, true, false); assert(duel.scoreB == 1);
  duel.begin(10000, 24); duel.update(10000 + duel.waitMs); assert(duel.phase == ReactionDuel::GO);
  duel.update(10000 + duel.waitMs + 187, true, false);
  assert(duel.winner == ReactionDuel::PLAYER_A && !duel.falseStart && duel.reactionMs == 187 && duel.scoreA == 1);
  duel.begin(20000, 25); duel.update(20001, true, true);
  assert(duel.winner == ReactionDuel::TIE && duel.scoreA == 1 && duel.scoreB == 1);
  duel.begin(21000, 26); duel.update(21000 + duel.waitMs, true, true);
  assert(duel.winner == ReactionDuel::TIE && !duel.falseStart);
  duel.begin(30000, 27); duel.update(30000 + duel.waitMs + 10000);
  assert(duel.winner == ReactionDuel::NONE && duel.phase == ReactionDuel::RESULT);
  duel.scoreA = 99; duel.begin(40000, 28); duel.update(40000 + duel.waitMs, true, false); assert(duel.scoreA == 99);

  HighLow cards;
  for (uint32_t seed = 0; seed < 500; ++seed) {
    cards.begin(seed); bool seen[52] = {};
    for (auto card : cards.deck) { assert(card < 52 && !seen[card]); seen[card] = true; }
    uint32_t now = 0;
    while (cards.used < 52) {
      uint8_t old = cards.current, next = cards.deck[cards.used], prior = cards.streak;
      bool up = (cards.used % 2) != 0;
      assert(cards.guess(up, now)); assert(!cards.guess(!up, now));
      cards.update(now + 279); assert(cards.phase == HighLow::FLIP);
      cards.update(now + 280); assert(cards.phase == HighLow::RESULT);
      int delta = int(HighLow::rank(next)) - int(HighLow::rank(old));
      if (!delta) assert(cards.outcome == HighLow::PUSH && cards.streak == prior);
      else if ((delta > 0) == up) assert(cards.outcome == HighLow::CORRECT && cards.streak == prior + 1);
      else assert(cards.outcome == HighLow::WRONG && !cards.streak);
      if (cards.used < 52) assert(cards.next() && cards.current == next);
      now += 300;
    }
    assert(!cards.next() && !cards.guess(true, now));
  }
  // Equal ranks in different suits preserve a nonzero streak.
  cards.begin(42); cards.current = 0; cards.deck[1] = 13; cards.streak = 4;
  cards.guess(false, 0); cards.update(280); assert(cards.outcome == HighLow::PUSH && cards.streak == 4);
  assert(HighLow::rank(12) == 14 && HighLow::rank(0) == 2 && HighLow::suit(51) == 3);

  PassBomb bomb;
  for (uint32_t seed = 0; seed < 500; ++seed) {
    bomb.begin(500, seed); assert(bomb.fuseMs >= 15000 && bomb.fuseMs <= 35000);
    uint32_t deadline = 500 + bomb.fuseMs;
    bomb.pass(deadline - 1); assert(bomb.phase == PassBomb::RUNNING && bomb.passes == 1);
    bomb.pass(deadline); assert(bomb.phase == PassBomb::EXPLODED && bomb.passes == 1);
    bomb.pass(deadline + 1000); assert(bomb.passes == 1);
  }
  bomb.begin(0, 1); bomb.passes = 999; bomb.pass(10); assert(bomb.passes == 999);
  uint32_t start = std::numeric_limits<uint32_t>::max() - 100;
  bomb.begin(start, 42); bomb.update(start + bomb.fuseMs - 1); assert(bomb.phase == PassBomb::RUNNING);
  bomb.update(start + bomb.fuseMs); assert(bomb.phase == PassBomb::EXPLODED);
  duel.begin(start, 42); duel.update(start + duel.waitMs + 200, false, true);
  assert(duel.winner == ReactionDuel::PLAYER_B && duel.reactionMs == 200);
  cards.begin(42); cards.guess(true, start); cards.update(start + 280); assert(cards.phase == HighLow::RESULT);
  puts("Party rules passed: reaction fairness/ties/timeout, 500 unique decks, ties/streaks/deck end, 500 random fuses, deadline passes and clock wrap.");
}
