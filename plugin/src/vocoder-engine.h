#pragma once
//
// VocoderEngine: a small, self-contained polyphonic wrapper around the Faust
// `vocoderDsp`. It keeps NVOICES independent DSP instances and does its own
// (deliberately simple) voice allocation, instead of pulling in Faust's heavier
// poly-dsp.h (which drags in MIDI/GUI/JSON/threading machinery). Each voice's
// parameters are reached through a Faust `APIUI`, addressed by index.
//
// The original vocoder.dsp exposes:
//   * voice-driven params : /vocoderDsp/freq, /gain, /gate, /bend
//   * "knob" params       : everything else (osc/noise/vocoder/mic/vol)
// The voice-driven ones are set from note events; the knobs are exposed to the
// host as automatable CLAP parameters and to the webview UI.
//
#include "faust-dsp.h"

#include <array>
#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <cstring>

class VocoderEngine {
public:
	static constexpr int NVOICES = 8;

	enum class Kind { slider, toggle, intSlider };

	struct KnobInfo {
		int apiIndex;       // index into each voice's APIUI
		uint32_t id;        // stable CLAP param id (hashed from the address)
		std::string address;
		std::string group;  // "OSC1" / "OSC2" / "NOISE" / "VOCODER" / "" (top level)
		std::string name;   // leaf label, e.g. "detune"
		double min, max, init, step;
		Kind kind;
	};

	// ---- setup / lifecycle ----------------------------------------------

	// Build the per-voice param maps and discover the parameter layout. Must be
	// called once, after the engine has been constructed in its final location
	// (the voices' DSP objects must not move afterwards, as APIUI stores raw
	// pointers into them — std::array guarantees that here).
	void setup() {
		for (auto &v : voices_) v.dsp.buildUserInterface(&v.api);

		auto &api = voices_[0].api;
		int n = api.getParamsCount();
		for (int i = 0; i < n; ++i) {
			std::string addr = api.getParamAddress(i);
			if (endsWith(addr, "/freq"))  { idxFreq_ = i; continue; }
			if (endsWith(addr, "/gain"))  { idxGain_ = i; continue; }
			if (endsWith(addr, "/gate"))  { idxGate_ = i; continue; }
			if (endsWith(addr, "/bend"))  { idxBend_ = i; continue; }

			KnobInfo k;
			k.apiIndex = i;
			k.address  = addr;
			k.min  = api.getParamMin(i);
			k.max  = api.getParamMax(i);
			k.init = api.getParamInit(i);
			k.step = api.getParamStep(i);
			splitAddress(addr, k.group, k.name);
			k.kind = classify(k.min, k.max, k.step);
			k.id   = hashAddress(addr);
			knobs_.push_back(k);
		}
	}

	void init(double sampleRate) {
		sampleRate_ = sampleRate;
		releaseFrames_ = int(sampleRate * 0.5); // 0.5s release tail before freeing
		for (auto &v : voices_) {
			v.dsp.init(int(sampleRate));
			v.note = -1;
			v.gateOn = false;
			v.releaseCountdown = 0;
		}
		// Re-apply the current knob values (init() clears DSP state).
		for (size_t i = 0; i < knobs_.size(); ++i) setKnob(int(i), knobValues_.empty() ? knobs_[i].init : knobValues_[i]);
	}

	void prepare(uint32_t maxFrames) {
		scratchL_.assign(maxFrames, 0.f);
		scratchR_.assign(maxFrames, 0.f);
		silence_.assign(maxFrames, 0.f);
		if (knobValues_.empty()) {
			knobValues_.resize(knobs_.size());
			for (size_t i = 0; i < knobs_.size(); ++i) knobValues_[i] = knobs_[i].init;
		}
	}

	void reset() {
		for (auto &v : voices_) {
			v.dsp.instanceClear();
			v.note = -1;
			v.gateOn = false;
			v.releaseCountdown = 0;
		}
	}

	// ---- parameters ------------------------------------------------------

	const std::vector<KnobInfo> &knobs() const { return knobs_; }

	double knobValue(int knobIndex) const { return knobValues_[knobIndex]; }

	void setKnob(int knobIndex, double value) {
		if (knobIndex < 0 || knobIndex >= int(knobs_.size())) return;
		if (!knobValues_.empty()) knobValues_[knobIndex] = value;
		int api = knobs_[knobIndex].apiIndex;
		for (auto &v : voices_) v.api.setParamValue(api, value);
	}

	int knobIndexForId(uint32_t id) const {
		for (size_t i = 0; i < knobs_.size(); ++i) if (knobs_[i].id == id) return int(i);
		return -1;
	}

	// ---- notes -----------------------------------------------------------

	void noteOn(int /*channel*/, int key, double velocity01) {
		int vi = allocVoice();
		Voice &v = voices_[vi];
		v.note = key;
		v.gateOn = true;
		v.releaseCountdown = 0;
		v.age = ++counter_;
		double hz = 440.0 * std::pow(2.0, (key - 69) / 12.0);
		if (idxFreq_ >= 0) v.api.setParamValue(idxFreq_, hz);
		if (idxGain_ >= 0) v.api.setParamValue(idxGain_, velocity01);
		if (idxGate_ >= 0) v.api.setParamValue(idxGate_, 1.0);
	}

	void noteOff(int /*channel*/, int key) {
		for (auto &v : voices_) {
			if (v.gateOn && v.note == key) {
				v.gateOn = false;
				v.releaseCountdown = releaseFrames_;
				if (idxGate_ >= 0) v.api.setParamValue(idxGate_, 0.0);
				// keep v.note for reference but mark as releasing
			}
		}
	}

	// wheel in [-1, 1]; mapped onto the DSP's `bend` slider range.
	void pitchWheel(int /*channel*/, double wheel) {
		if (idxBend_ < 0) return;
		double v = wheel * bendRangeSemitones_;
		for (auto &voice : voices_) voice.api.setParamValue(idxBend_, v);
	}

	void allNotesOff() {
		for (auto &v : voices_) {
			if (v.gateOn) {
				v.gateOn = false;
				v.releaseCountdown = releaseFrames_;
				if (idxGate_ >= 0) v.api.setParamValue(idxGate_, 0.0);
			}
		}
	}

	// ---- audio -----------------------------------------------------------

	// micIn may be null (no audio input connected) -> a zero buffer is used.
	void process(uint32_t frames, const float *micIn, float *outL, float *outR) {
		std::memset(outL, 0, frames * sizeof(float));
		std::memset(outR, 0, frames * sizeof(float));

		const float *in = micIn ? micIn : silence_.data();
		float *ins[1]  = { const_cast<float *>(in) };
		float *outs[2] = { scratchL_.data(), scratchR_.data() };

		for (auto &v : voices_) {
			bool active = v.gateOn || v.releaseCountdown > 0;
			if (!active) continue;

			v.dsp.compute(int(frames), ins, outs);
			for (uint32_t i = 0; i < frames; ++i) {
				outL[i] += scratchL_[i];
				outR[i] += scratchR_[i];
			}
			if (!v.gateOn) {
				v.releaseCountdown -= int(frames);
				if (v.releaseCountdown <= 0) {
					v.releaseCountdown = 0;
					v.note = -1;
				}
			}
		}
	}

private:
	struct Voice {
		vocoderDsp dsp;
		APIUI api;
		int note = -1;
		bool gateOn = false;
		int releaseCountdown = 0;
		long long age = 0;
	};

	std::array<Voice, NVOICES> voices_;
	std::vector<KnobInfo> knobs_;
	std::vector<double> knobValues_;
	std::vector<float> scratchL_, scratchR_, silence_;

	double sampleRate_ = 48000;
	int releaseFrames_ = 24000;
	long long counter_ = 0;
	int idxFreq_ = -1, idxGain_ = -1, idxGate_ = -1, idxBend_ = -1;
	static constexpr double bendRangeSemitones_ = 2.0;

	int allocVoice() {
		// Prefer a fully free voice (no note, not releasing).
		for (int i = 0; i < NVOICES; ++i) {
			if (voices_[i].note < 0 && voices_[i].releaseCountdown == 0) return i;
		}
		// Otherwise steal the oldest voice.
		int oldest = 0;
		for (int i = 1; i < NVOICES; ++i) {
			if (voices_[i].age < voices_[oldest].age) oldest = i;
		}
		return oldest;
	}

	static bool endsWith(const std::string &s, const char *suffix) {
		size_t n = std::strlen(suffix);
		return s.size() >= n && s.compare(s.size() - n, n, suffix) == 0;
	}

	static void splitAddress(const std::string &addr, std::string &group, std::string &leaf) {
		// addr like "/vocoderDsp/OSC1/detune" or "/vocoderDsp/mic"
		auto last = addr.find_last_of('/');
		leaf = (last == std::string::npos) ? addr : addr.substr(last + 1);
		std::string parent = (last == std::string::npos) ? "" : addr.substr(0, last);
		auto prevSlash = parent.find_last_of('/');
		std::string parentLeaf = (prevSlash == std::string::npos) ? parent : parent.substr(prevSlash + 1);
		group = (parentLeaf == "vocoderDsp") ? "" : parentLeaf;
	}

	static Kind classify(double min, double max, double step) {
		bool integral = step >= 1.0 && std::abs((max - min) / step - std::round((max - min) / step)) < 1e-6;
		if (integral && std::abs(min) < 1e-9 && std::abs(max - 1.0) < 1e-9) return Kind::toggle;
		if (integral) return Kind::intSlider;
		return Kind::slider;
	}

	// FNV-1a 32-bit hash, used to give each parameter a stable CLAP id.
	static uint32_t hashAddress(const std::string &s) {
		uint32_t h = 2166136261u;
		for (unsigned char c : s) { h ^= c; h *= 16777619u; }
		return h;
	}
};
