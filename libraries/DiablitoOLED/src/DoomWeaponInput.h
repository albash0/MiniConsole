#pragma once
#include <stdint.h>

// B remains Use on every press, including while walking or turning. Weapon
// selection needs B held first, then a fresh Left/Right press; a turn that was
// already held must never turn into an accidental weapon change.
typedef struct {
    uint32_t previous, began, directions;
    uint8_t holding;
} DoomWeaponInput;

static inline uint32_t DoomWeaponInput_update(DoomWeaponInput *state,
                                              uint32_t buttons, uint32_t now,
                                              int gameplay) {
    const uint32_t a=1, b=2, right=8, left=16, turns=right|left;
    uint32_t raw=buttons, fresh=buttons & ~state->previous & turns;
    if (!gameplay || !(buttons & b) || (buttons & a)) {
        state->holding=0;
        state->directions=0;
    } else {
        if (!state->holding) {
            state->holding=1;
            state->began=now;
        }
        state->directions &= buttons & turns;
        if ((uint32_t)(now-state->began)>=400 && (buttons & turns)!=turns)
            state->directions |= fresh;
        if (state->directions & left) buttons |= 128u;
        if (state->directions & right) buttons |= 256u;
        buttons &= ~state->directions;
    }
    state->previous=raw;
    return buttons;
}
