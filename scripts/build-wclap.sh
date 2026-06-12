#!/usr/bin/env bash
#
# Build the WebAssembly plugin (WCLAP) — a single portable binary that runs in
# web/browser CLAP hosts (e.g. https://github.com/Signalsmith-Audio/wasm-clap-browserhost).
#
# Requires the WASI SDK (https://github.com/WebAssembly/wasi-sdk). Point WASI_SDK
# at an unpacked SDK:
#   export WASI_SDK=/path/to/wasi-sdk
#
set -euo pipefail
cd "$(dirname "$0")/.."

: "${WASI_SDK:?set WASI_SDK to your unpacked wasi-sdk directory}"
BUILD_DIR=${BUILD_DIR:-build-wclap}

cmake -B "$BUILD_DIR" \
	-DCMAKE_TOOLCHAIN_FILE="$WASI_SDK/share/cmake/wasi-sdk-pthread.cmake" \
	-DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" --target vocoder_wclap

echo
echo "Looking for the built .wclap ..."
find "$BUILD_DIR" -name '*.wclap' -o -name '*wclap*' -print
