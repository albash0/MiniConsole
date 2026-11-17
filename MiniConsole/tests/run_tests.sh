#!/bin/sh
set -eu
sketch_dir=$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)
test_build=$(mktemp -d /tmp/miniconsole-tests.XXXXXX)
trap 'rm -rf "$test_build"' EXIT
cxx=${CXX:-clang++}
if [ "$#" -eq 0 ]; then set -- "$test_build/previews"; fi
mkdir -p "$1"
display_flags=""
if [ "${DISPLAY_PROFILE:-oled}" = tft ]; then display_flags="-DMINICONSOLE_TFT114=1"; fi
for test in snake blocks arcade suspend party doom_input theme snake_motion sound motorways motorways_endless motorways_balance block_battle partners; do
  "$cxx" -std=c++17 -O1 -g -fsanitize=address,undefined -fno-sanitize-recover=undefined "$sketch_dir/tests/${test}_test.cpp" -o "$test_build/$test"
  "$test_build/$test"
done
for test in ui suspend_ui going_out party_ui sound_ui music_ui motorways_ui motorways_tools motorways_diagonal block_battle_ui partners_ui; do
"$cxx" -std=c++17 -O1 -g -fsanitize=address,undefined -fno-sanitize-recover=undefined -DARDUINO=10819 -DMINICONSOLE_HOST_TEST=1 $display_flags \
  -I"$sketch_dir/tests/host" -I"$sketch_dir/../libraries/Adafruit_GFX_Library" \
  -I"$sketch_dir/../libraries/DiablitoOLED/src" \
  -I"$sketch_dir/../libraries/Adafruit_BusIO" \
  "$sketch_dir/tests/${test}_test.cpp" "$sketch_dir/../libraries/Adafruit_GFX_Library/Adafruit_GFX.cpp" \
  -o "$test_build/$test"
"$test_build/$test" "$@"
done
for test in tft appearance_ui native_games themed_games game_theme_ui; do
"$cxx" -std=c++17 -O1 -g -fsanitize=address,undefined -fno-sanitize-recover=undefined -DARDUINO=10819 -DMINICONSOLE_HOST_TEST=1 -DMINICONSOLE_TFT114=1 \
  -I"$sketch_dir/tests/host" -I"$sketch_dir/../libraries/Adafruit_GFX_Library" \
  -I"$sketch_dir/../libraries/DiablitoOLED/src" \
  -I"$sketch_dir/../libraries/Adafruit_BusIO" \
  "$sketch_dir/tests/${test}_test.cpp" "$sketch_dir/../libraries/Adafruit_GFX_Library/Adafruit_GFX.cpp" \
  -o "$test_build/$test"
"$test_build/$test" "$@"
done
