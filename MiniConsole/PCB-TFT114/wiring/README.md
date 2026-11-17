# Battery positive wiring reference

The diagram is a newly drawn rear-view layout based on the PCB net assignments, not an illustration of the ESP module's own battery pads.

- J2 pin 1: BAT_PLUS, front coordinates (29.60, 22.10) mm; pin 2: GND at (27.06, 22.10).
- J3 pin 1: BAT_SW, front coordinates (15.60, 27.00) mm; pin 2: GND at (18.14, 27.00).
- Rear-view X is 70.10 minus front-view X, with USB down. Thus J2 positive is left; J3 positive is right.
- The existing B.SilkS mirrored text `B+  B-` at (16.87, 29.15) reads with the wrong polarity order on the rear. The copper/pin-number assignments above take precedence.

Source: MiniConsoleTFT114.kicad_pcb. Manufacturing files are unchanged. Before power, verify the left J3 hole is continuous with a known carrier GND pad with battery and USB disconnected. Wire J3 positive to the actual ESP module pad marked B+.
