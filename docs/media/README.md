# Preview provenance

`snake.png`, `motorways.png`, and `partners.png` are lossless PNG conversions of host-test RGB frames. They use the actual TFT firmware renderer with deterministic test state. They are not photographs, live radio captures, or measured hardware-performance results.

Generate the source PPM frames from the repository root with:

```sh
DISPLAY_PROFILE=tft sh MiniConsole/tests/run_tests.sh /tmp/miniconsole-previews
```

The PNG copies retain pixel data and structural image chunks only.
