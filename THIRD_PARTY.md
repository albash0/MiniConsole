# Third-party notices

The root LICENSE contains GNU GPL version 2. Original MiniConsole firmware, tools, documentation and hardware/CAD design contributions are offered under GPL version 2 or, at your option, any later version. This grant does not replace the licenses of third-party material or grant rights to Doom game assets or trademarks.

## DIABLITO adaptation

The complete locally adapted source is committed under `libraries/DiablitoOLED/`, not an external path or an uninitialized submodule. It derives from [aedile/DIABLITO](https://github.com/aedile/DIABLITO), base commit `8602f28e31e9c35c7b6198f5235543cea9a53588`, itself descended from [kilograham/rp2040-doom](https://github.com/kilograham/rp2040-doom) and [Chocolate Doom](https://github.com/chocolate-doom/chocolate-doom).

Credits include id Software, Simon Howard and contributors, Graham Sanderson, DIABLITO contributors, and all authors named in retained headers. The adaptation changes ESP32-S3 memory mapping, display output for SSD1306/ST7789, six-button input, launcher integration, saves and simplified piezo sound. The original copyright/license headers and `libraries/DiablitoOLED/COPYING` are retained. GPL-derived code is GPL-2.0-or-later; independently licensed upstream portions retain their own notices.

The optional `doom1.whd` is compressed Doom shareware game data, **not GPL source**. Its pinned upstream SHA-256 is `1b3d0335b283e6132679291bf8ba026a16ef5442c3b97c00657a84e69d8100b4` (2,073,676 bytes). It is downloaded separately, excluded from Git, and retains id Software's shareware terms. Do not add commercial WADs or assume source-code licensing applies to game assets.

## Adafruit libraries

`libraries/Adafruit_GFX_Library` retains Adafruit's BSD notice in `license.txt` and font/source notices. `libraries/Adafruit_BusIO` retains its MIT license in `LICENSE`. Their source and license files are bundled for repeatable host builds. Display-specific drivers are installed separately through Arduino Library Manager.

## KiCad library material

Bundled standard symbols, footprints and the two component STEP models originate from KiCad community libraries. They retain CC-BY-SA-4.0 with the KiCad library exception. Model attribution and the exception are documented in each carrier's `3dmodels/ATTRIBUTION.txt` and `KiCad-Libraries-LICENSE.md`. This exception permits independent licensing of electronic designs made using library data. These terms remain applicable to redistributed library material.

The case STEP/STL files are original project design exports. STEP author/path metadata was cleared for the public export; geometry was retained.
