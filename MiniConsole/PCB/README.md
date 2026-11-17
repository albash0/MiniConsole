# MiniConsole OLED carrier V1.7

Open `MiniConsoleCarrier.kicad_pro` in KiCad 10. The checked schematic, routed PCB, local symbols/footprints and unpacked manufacturing outputs are included. Use the native KiCad design as the authority; the EAGLE/Fusion interchange files may require copper-polygon regeneration and DRC after import.

The carrier is 70.1 × 30.1 mm with a 65.9 × 26.1 mm case mounting pattern. This revision adds five rear test pads and moves Left/Right inward 0.50 mm each. The expansion header is above Up. VCC and logic are 3.3 V. See `documentation/BOM.csv`, `manufacturing/`, and the retained geometry/check reports in `documentation/`.

Pin assignments and display setup are in [the user guide](../../docs/USER_GUIDE.md). The TFT carrier's firmware A/B mapping follows the physical assembled console and differs from older board labels. On the TFT board, read [the battery wiring correction](../PCB-TFT114/wiring/README.md) before wiring the battery. The PCB reference and copper pin numbers take precedence over conflicting silkscreen.

The original hardware records report no DRC/ERC violations with the configured rules. Those are historical CAD checks, not a new hardware test. Confirm module dimensions, polarity, clearances and the fabrication house's rules before manufacturing. Component models are nominal fit references; their original notices are under `3dmodels/`.
