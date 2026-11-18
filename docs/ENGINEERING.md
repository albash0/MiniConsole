# MiniConsole engineering notes

This project brings game logic, displays, storage, wireless communication, and mechanical design into one handheld. The examples below describe the implemented choices and their tradeoffs. Source links and tests provide the evidence; the public-source validation report records the checks performed for publication.

## One game library across two displays

The OLED provides 128 × 64 monochrome pixels. The TFT provides a 240 × 135 RGB565 landscape canvas, with portrait presentation for Tetris. A single RGB565 panel buffer requires **64,800 bytes**, compared with **1,024 bytes** for a packed 128 × 64 monochrome buffer. Those figures describe one framebuffer, not the complete application's memory use.

The firmware keeps the game rules and checkpoint formats shared. The TFT backend can map existing logical drawing calls onto its color canvas, while native renderers provide higher-resolution menus and selected games. The full-panel buffer attempts PSRAM allocation first, with a heap fallback. Doom's additional working buffers are part of the separate engine integration.

| Requirement | Implemented choice | Tradeoff and evidence |
| --- | --- | --- |
| Support both display versions | Compile-time profiles with shared game rules and saves | Reduces duplicated gameplay code; rendering still needs checks for both geometries. [Display backend](../MiniConsole/ConsoleTFT114.h), [native UI](../MiniConsole/TFTNativeUI.h). |
| Improve color presentation | Buffered RGB565 output and native renderers for selected games | Uses more memory and transfer bandwidth than monochrome. [Native games](../MiniConsole/TFTGames.h), [rendering tests](../MiniConsole/tests/native_games_test.cpp). |
| Keep the MCU responsive during Doom | The Doom display task yields a FreeRTOS tick after presenting a frame | Leaves time for the idle task/watchdog rather than monopolizing a core. [Platform integration](../libraries/DiablitoOLED/src/DoomPort.cpp). |

This approach lets the OLED version retain its compact interface while the TFT version gains clearer text, color, and animation. A full performance comparison would need repeatable measurements on both physical devices using the same scene and firmware revision.

## Preserve progress across power cycles

The shared checkpoint serializes game state into a versioned record with a checksum. Saving is tied to pause events and verified before the UI reports success. Doom has a separate native save area. Battle sessions do not replace a solo checkpoint.

The tests check round trips, corrupted/truncated data, compatibility with older formats, and resume behavior through the controller. These checks exercise serialization and lifecycle behavior; hardware power interruption remains a separate test.

Evidence: [checkpoint implementation](../MiniConsole/SuspendSave.h), [codec tests](../MiniConsole/tests/suspend_test.cpp), [controller tests](../MiniConsole/tests/suspend_ui_test.cpp).

## Keep two consoles in a consistent world

Pocket Partners uses a host-authoritative simulation: the host advances shared physics, the client sends inputs, and snapshots communicate the resulting world. This gives one authority for crates, gates, keys, and completion. The protocol includes checksums, session binding, repeated input counters, and timeout handling. Brief stalls freeze play; a longer loss shows a disconnected state.

The tradeoff is dependence on the host and a responsive radio link. Host tests exercise lost/duplicate packets and shared pause/retry behavior; they cannot measure RF range or interference. Earlier development records describe nearby two-device testing. Range remains unmeasured.

Evidence: [co-op session](../MiniConsole/PartnersLink.h), [protocol and physics tests](../MiniConsole/tests/partners_test.cpp), [replays for all ten rooms](../MiniConsole/tests/partners_solutions.h).

## A regression found during source preparation

Selecting Pocket Partners in the OLED library exposed an out-of-bounds read: the icon color table still had seven entries after an eighth cartridge was added. The table now includes the new entry, and a compile-time assertion checks its size against the game count. Sanitizer failures now stop the host runner immediately.

This was a source-level fix, validated with both host display profiles and both firmware builds. It was not followed by a new hardware flash during publication. See [the validation report](../VALIDATION.md).

## Mechanical fit is another interface

The keyed case couples PCB mounting axes, screen height, switch travel, button retention, and battery clearance. Individual STEP and STL parts are supplied with two front and two plain rear choices. The thicker rear floor reduces internal battery clearance; assembly notes describe that constraint.

The [build photographs](../README.md#from-prototype-to-finished-handheld) show the OLED protoboard, custom TFT PCB, a printed enclosure fit test, and assembled handhelds. They document physical iteration; printer tolerance, button travel, and the complete internal battery/cable stack still need checking for a reproduced build. [Case geometry and assembly notes](../hardware/case/README.md).

## Next useful measurements

- Compare display-transfer time and game frame time on both versions under a repeatable workload.
- Measure battery current and runtime with a specified cell and display brightness.
- Record wireless range and recovery behavior in a defined test environment.
- Measure button travel and document the complete internal battery/cable stack and assembly clearances.
