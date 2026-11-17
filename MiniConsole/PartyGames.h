#pragma once
#include <stdint.h>

namespace PartyGames {
inline uint32_t randomStep(uint32_t &state) {
  if (!state) state = 0x9e3779b9;
  state ^= state << 13; state ^= state >> 17; state ^= state << 5;
  return state;
}

struct ReactionDuel {
  enum Phase : uint8_t { READY, WAIT, GO, RESULT } phase = READY;
  enum Winner : uint8_t { PLAYER_A, PLAYER_B, TIE, NONE } winner = NONE;
  uint8_t scoreA = 0, scoreB = 0;
  bool falseStart = false;
  uint32_t startedAt = 0, waitMs = 0, reactionMs = 0;

  void begin(uint32_t now, uint32_t seed) {
    startedAt = now; waitMs = 1500 + randomStep(seed) % 3501;
    reactionMs = 0; falseStart = false; winner = NONE; phase = WAIT;
  }
  void finish(Winner result) {
    winner = result; phase = RESULT;
    if (result == PLAYER_A && scoreA < 99) ++scoreA;
    if (result == PLAYER_B && scoreB < 99) ++scoreB;
  }
  void update(uint32_t now, bool a = false, bool b = false) {
    if (phase != WAIT && phase != GO) return;
    uint32_t elapsed = now - startedAt;
    if (phase == WAIT && elapsed >= waitMs) phase = GO;
    if (a || b) {
      falseStart = phase == WAIT;
      reactionMs = falseStart ? 0 : elapsed - waitMs;
      if (a && b) finish(TIE);
      else if (falseStart) finish(a ? PLAYER_B : PLAYER_A);
      else finish(a ? PLAYER_A : PLAYER_B);
    } else if (phase == GO && elapsed - waitMs >= 10000) finish(NONE);
  }
};

struct HighLow {
  enum Phase : uint8_t { READY, GUESS, FLIP, RESULT } phase = READY;
  enum Outcome : uint8_t { CORRECT, WRONG, PUSH } outcome = PUSH;
  uint8_t deck[52] = {}, used = 0, current = 0, revealed = 0;
  uint8_t streak = 0, best = 0;
  bool higher = true;
  uint32_t flippedAt = 0;
  static uint8_t rank(uint8_t card) { return card % 13 + 2; } // Ace high.
  static uint8_t suit(uint8_t card) { return card / 13; }
  void begin(uint32_t seed) {
    for (uint8_t i = 0; i < 52; ++i) deck[i] = i;
    for (int i = 51; i > 0; --i) {
      int j = randomStep(seed) % (i + 1);
      uint8_t temp = deck[i]; deck[i] = deck[j]; deck[j] = temp;
    }
    used = 1; current = revealed = deck[0]; streak = 0; phase = GUESS;
  }
  bool guess(bool up, uint32_t now) {
    if (phase != GUESS || used >= 52) return false;
    higher = up; revealed = deck[used++]; flippedAt = now; phase = FLIP;
    return true;
  }
  void update(uint32_t now) {
    if (phase != FLIP || now - flippedAt < 280) return;
    int difference = int(rank(revealed)) - int(rank(current));
    outcome = !difference ? PUSH : ((difference > 0) == higher ? CORRECT : WRONG);
    if (outcome == CORRECT) { ++streak; if (streak > best) best = streak; }
    else if (outcome == WRONG) streak = 0;
    phase = RESULT;
  }
  bool next() {
    if (phase != RESULT || used >= 52) return false;
    current = revealed; phase = GUESS; return true;
  }
};

struct PassBomb {
  enum Phase : uint8_t { READY, RUNNING, EXPLODED } phase = READY;
  uint32_t startedAt = 0, fuseMs = 0;
  uint16_t passes = 0;
  void begin(uint32_t now, uint32_t seed) {
    startedAt = now; fuseMs = 15000 + randomStep(seed) % 20001;
    passes = 0; phase = RUNNING;
  }
  void update(uint32_t now) {
    if (phase == RUNNING && now - startedAt >= fuseMs) phase = EXPLODED;
  }
  void pass(uint32_t now) {
    update(now); // A press at the deadline cannot pass an already exploded bomb.
    if (phase == RUNNING && passes < 999) ++passes;
  }
};
} // namespace PartyGames
