# Public source validation — 2026-11-17

Validated against the clean source export using Arduino ESP32 core 3.3.8 and a C++17 host compiler.

- Full host test runner passed for OLED and TFT profiles under AddressSanitizer/UndefinedBehaviorSanitizer. Undefined behavior now stops the runner immediately.
- Both MiniConsole firmware profiles compile, including the bundled DIABLITO adaptation. Host tests stub Doom; no new physical flash or gameplay test was performed for this publication.
- Fixed an OLED library-icon color-table overread when selecting Pocket Partners. A compile-time check now ties the table size to the cartridge count.
- Host scripts now create preview directories automatically on a fresh checkout.
- Plain rear STL exports each have one closed, watertight boundary shell. Only individual front/rear/button exports are included.
- Fabrication outputs are byte-identical to the working project's retained outputs. No new ERC/DRC run or hardware fabrication was performed for this export.
- Text and binary-header scans found no known private destination, private contact/path, device-MAC, credential, or personalized-case patterns in the exported files. Device flash dumps, saved-state records, logs, native CAD archives and local history were excluded. PNG descriptive metadata and STEP provenance headers were removed.
- The public repository starts with a fresh history and uses the maintainer's GitHub noreply commit address.

These are source-export checks. They do not replace physical fit, battery wiring, runtime audio/gameplay or radio-range verification.
