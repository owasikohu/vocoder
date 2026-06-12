#!/usr/bin/env bash
#
# Build the native CLAP plugin (Windows / macOS / Linux).
# Output: build/Vocoder.clap
#
# Requirements:
#   * CMake >= 3.24, a C++17 compiler
#   * Linux: libgtk-3-dev, libwebkit2gtk-4.1-dev, pkg-config
#   * Windows: WebView2 runtime (usually already installed)
#   * macOS: WKWebView (built in)
#
set -euo pipefail
cd "$(dirname "$0")/.."

BUILD_DIR=${BUILD_DIR:-build}

cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" --target vocoder_clap --config Release -j

echo
echo "Looking for the built .clap ..."
find "$BUILD_DIR" -name '*.clap' -print

# Optional install on Linux/macOS:
#   Linux:  cp -r build/**/Vocoder.clap ~/.clap/
#   macOS:  cp -r build/**/Vocoder.clap ~/Library/Audio/Plug-Ins/CLAP/
