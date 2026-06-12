#pragma once
//
// Pulls in the Faust-generated DSP class `vocoderDsp`, together with the minimal
// set of Faust "architecture" support headers it needs (vendored under
// third_party/faust). Regenerate the DSP with scripts/gen-faust.sh after editing
// src/vocoder.dsp.
//
// These four headers provide the `dsp`, `UI`, `Meta` and `APIUI` types that the
// generated file (and our engine) reference. They are deliberately light-weight
// (no MIDI / GUI / JSON / threading), which keeps both the native and the WASM
// (WCLAP) builds simple.
//
#include "faust/gui/meta.h"
#include "faust/gui/UI.h"
#include "faust/dsp/dsp.h"
#include "faust/gui/APIUI.h"

// The Faust-generated DSP (class `vocoderDsp : public dsp`).
#include "vocoder-dsp.cpp"
