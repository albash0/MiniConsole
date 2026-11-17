# MiniConsole TFT case: keyed buttons and plain rear

These are individual neutral CAD/print exports of the 2026-11-09 keyed-button revision. STEP files can be imported into CAD editors; STL files are for printing. Only individual parts are supplied here. Pick one front, one rear and one set of six keyed buttons.

| Part | Choice |
| --- | --- |
| Front | `Front-V2.1-Keyed`: I2C expansion opening; or `Front-V2.1-Keyed-No-I2C`: closed front. Omit the physical expansion header with the closed front. |
| Rear | `Rear-V2.1`: original 1 mm floor; or `Rear-V2.1-Plain-2mm`: solid 2 mm floor. |
| Buttons | `Buttons-six-piece-set-Keyed.stl`, or the six individual `Button-*-Keyed.stl` files. Print one set only. |

Both fronts use the selected 1 mm shorter height and 2 mm outer edge fillet. Top envelope is 78.6 × 32.6 × 7.1 mm. Keyed caps are 5.5 mm diameter; hidden flats limit rotation. Use the keyed fronts and keyed buttons together. All STLs are in millimeters and placed at Z=0 for printing: front outside face down, rear floor down, button faces down.

The 2 mm-floor rear extends 1 mm farther into the case. The nominal battery needs to rise at least 0.55 mm plus clearance. Confirm battery, cable and solder clearance on your own assembly. Recheck screw length with the shorter top; the older M2×12 recommendation applied to a taller case. The carrier mounting pattern is 65.9 × 26.1 mm.

USB-C opening is 10.0 × 4.2 mm; the revised switch notch is retained. CAD and mesh checks were performed in the original development records, but printer tolerances, button feel and the assembled battery fit need physical verification.

Native application archives, combined assemblies, slicer projects and development history are omitted from this source export. The individual supplied parts define the public case variant.
