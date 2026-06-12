# Vocoder

A polyphonic **vocoder synth** written in [Faust](https://faust.grame.fr), shipped
as a **cross-platform [CLAP](https://cleveraudio.org) plugin** with an **HTML/JS
webview UI**.

* DSP: [src/vocoder.dsp](src/vocoder.dsp) — 2 oscillators + noise into a 10-band
  vocoder, played as a MIDI instrument. Your audio input (e.g. a voice) is the
  modulator; the synth is the carrier.
* One C++ source set builds a **native CLAP** (Windows / macOS / Linux) and a
  **WCLAP** (WebAssembly, for web hosts).

## How it works

```
 ui/ (HTML+JS)  ──postMessage(CBOR)──►  VocoderPlugin (CLAP)  ──►  VocoderEngine  ──►  8 × vocoderDsp (Faust)
   webview                                   │  webview-gui (native webview: WebKitGTK / WKWebView / WebView2)
                                             └─ params / notes / audio
```

* **[plugin/faust/vocoder-dsp.cpp](plugin/faust/vocoder-dsp.cpp)** — the Faust DSP
  compiled to a C++ class (`vocoderDsp`). Regenerate with `scripts/gen-faust.sh`.
* **[plugin/src/vocoder-engine.h](plugin/src/vocoder-engine.h)** — a tiny
  polyphonic wrapper: 8 independent `vocoderDsp` voices with simple voice
  allocation, parameters reached through a Faust `APIUI`. (We deliberately avoid
  Faust's heavier `poly-dsp.h`, which keeps the WASM build simple.)
* **[plugin/src/vocoder-plugin.h](plugin/src/vocoder-plugin.h)** — the CLAP plugin:
  audio ports (mono mic in, stereo out), a note input, automatable parameters
  (the DSP's non–voice-driven "knobs", discovered automatically), state, and the
  webview GUI.
* **[plugin/ui/](plugin/ui/)** — the GUI. `app.js` builds the controls from an
  `init` message and talks to the plugin with CBOR-encoded messages. Served from
  memory, so it works for native and WASM builds alike.

The webview is hosted via [webview-gui](https://github.com/geraintluff/webview-gui),
which embeds the OS-native webview into the host's plugin window and implements
`clap.gui`. The plugin/UI message bridge and project structure follow the
[signalsmith-clap-cpp](https://github.com/geraintluff/signalsmith-clap-cpp) examples.

## Build (native CLAP)

Requirements:

* CMake ≥ 3.24 and a C++17 compiler
* **Linux:** `libgtk-3-dev`, `libwebkit2gtk-4.1-dev`, `pkg-config`
  ```sh
  sudo apt-get install cmake pkg-config build-essential libgtk-3-dev libwebkit2gtk-4.1-dev
  ```
* **macOS:** Xcode command-line tools (WKWebView is built in)
* **Windows:** Visual Studio + the WebView2 runtime (usually already present)

Then:

```sh
./scripts/build-native.sh        # -> build/Vocoder.clap
```

Install it where your DAW looks for CLAP plugins:

* Linux: `cp build/Vocoder.clap ~/.clap/`
* macOS: `cp -r build/Vocoder.clap ~/Library/Audio/Plug-Ins/CLAP/`
* Windows: copy `Vocoder.clap` to `%COMMONPROGRAMFILES%\CLAP\`

Dependencies (clap, clap-wrapper, webview-gui + choc) are fetched automatically by
CMake. Building cross-platform binaries is done per-OS (or via CI) — the code is
portable, but each `.clap` must be built on its target platform.

### Using it in a DAW

Add the plugin to a track, route audio (a voice) into its input, and play MIDI
notes for the carrier pitch. Enable an oscillator (OSC1/OSC2 `saw`/`squ`/`tri`)
in the UI — with all oscillators off the carrier is silent.

## Build (WCLAP / WebAssembly)

Requires the [WASI SDK](https://github.com/WebAssembly/wasi-sdk):

```sh
export WASI_SDK=/path/to/wasi-sdk
./scripts/build-wclap.sh
```

The resulting WCLAP runs in web CLAP hosts such as the
[browser test host](https://github.com/Signalsmith-Audio/wasm-clap-browserhost).

## Regenerating the DSP

After editing [src/vocoder.dsp](src/vocoder.dsp):

```sh
./scripts/gen-faust.sh           # needs the `faust` compiler
```

This rewrites `plugin/faust/vocoder-dsp.cpp`. The engine discovers the parameter
layout at runtime, so adding/removing DSP controls needs no C++ changes — they
appear automatically as CLAP parameters and in the UI.

## Layout

```
src/vocoder.dsp              Faust source (the only thing you normally edit)
plugin/faust/               Faust-generated C++ DSP
plugin/src/                 CLAP plugin (engine, plugin, factory, entry)
plugin/ui/                  webview GUI (index.html, app.js, style.css, cbor.min.js)
third_party/                vendored single-header deps + minimal Faust headers
scripts/                    gen-faust / build-native / build-wclap
cmake/embed-resource.cmake  embeds ui/* as byte arrays
```

## Licenses

This project is MIT (see [LICENSE](LICENSE)). Vendored third-party code:
`third_party/cbor-walker` and `third_party/signalsmith-clap` are from
signalsmith-clap-cpp (Boost Software License 1.0); `third_party/faust` are Faust
architecture headers (used to build Faust-generated code). clap, clap-wrapper,
webview-gui and choc are fetched at build time under their own licenses.
