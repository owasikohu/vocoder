#!/usr/bin/env bash
#
# Regenerate the C++ DSP from the Faust source.
# Output: plugin/faust/vocoder-dsp.cpp  (class `vocoderDsp`)
#
# Requires the Faust compiler (https://faust.grame.fr). On Debian/Ubuntu:
#   sudo apt-get install faust
#
set -euo pipefail
cd "$(dirname "$0")/.."

SRC=src/vocoder.dsp
OUT=plugin/faust/vocoder-dsp.cpp

if ! command -v faust >/dev/null 2>&1; then
	echo "error: 'faust' compiler not found." >&2
	echo "Install it (e.g. 'sudo apt-get install faust'), or generate the file with" >&2
	echo "a libfaust build — see README.md ('Regenerating the DSP')." >&2
	exit 1
fi

echo "Generating $OUT from $SRC ..."
faust -lang cpp -cn vocoderDsp -o "$OUT" "$SRC"
echo "done."
