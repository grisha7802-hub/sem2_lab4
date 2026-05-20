#!/usr/bin/env bash
set -euo pipefail
CMAKE_BIN="/opt/homebrew/bin/cmake"
if [ ! -x "$CMAKE_BIN" ]; then
  CMAKE_BIN="/usr/local/bin/cmake"
fi
"$CMAKE_BIN" -S . -B build
"$CMAKE_BIN" --build build -j2
./build/lr3_tests
