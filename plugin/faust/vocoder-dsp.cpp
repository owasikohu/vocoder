/* ------------------------------------------------------------
name: "vocoderDsp"
Code generated with Faust 2.85.5 (https://faust.grame.fr)
Compilation options: -lang cpp -fpga-mem-th 4 -ct 1 -cn vocoderDsp -es 1 -mcd 16 -mdd 1024 -mdy 33 -single -ftz 0
------------------------------------------------------------ */

#ifndef  __vocoderDsp_H__
#define  __vocoderDsp_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <math.h>

#ifndef FAUSTCLASS 
#define FAUSTCLASS vocoderDsp
#endif

#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

#if defined(_WIN32)
#define RESTRICT __restrict
#else
#define RESTRICT __restrict__
#endif

static float vocoderDsp_faustpower2_f(float value) {
	return value * value;
}

class vocoderDsp : public dsp {
	
 private:
	
	int iVec0[2];
	int fSampleRate;
	float fConst0;
	float fConst1;
	float fConst2;
	FAUSTFLOAT fHslider0;
	float fConst3;
	int iRec1[2];
	FAUSTFLOAT fHslider1;
	FAUSTFLOAT fHslider2;
	FAUSTFLOAT fHslider3;
	FAUSTFLOAT fHslider4;
	FAUSTFLOAT fHslider5;
	float fConst4;
	float fRec2[2];
	FAUSTFLOAT fCheckbox0;
	float fRec5[2];
	float fVec1[2];
	float fConst5;
	int IOTA0;
	float fVec2[4096];
	float fConst6;
	float fRec4[2];
	FAUSTFLOAT fCheckbox1;
	float fConst7;
	FAUSTFLOAT fCheckbox2;
	FAUSTFLOAT fHslider6;
	FAUSTFLOAT fHslider7;
	FAUSTFLOAT fHslider8;
	float fRec6[2];
	FAUSTFLOAT fCheckbox3;
	float fRec9[2];
	float fVec3[2];
	float fVec4[4096];
	float fRec8[2];
	FAUSTFLOAT fCheckbox4;
	FAUSTFLOAT fCheckbox5;
	FAUSTFLOAT fHslider9;
	FAUSTFLOAT fHslider10;
	FAUSTFLOAT fButton0;
	float fRec0[3];
	FAUSTFLOAT fHslider11;
	float fRec11[3];
	FAUSTFLOAT fHslider12;
	FAUSTFLOAT fHslider13;
	float fRec10[2];
	float fConst8;
	float fConst9;
	float fConst10;
	float fRec12[3];
	float fRec14[3];
	float fRec13[2];
	float fConst11;
	float fConst12;
	float fConst13;
	float fRec15[3];
	float fRec17[3];
	float fRec16[2];
	float fConst14;
	float fConst15;
	float fConst16;
	float fRec18[3];
	float fRec20[3];
	float fRec19[2];
	float fConst17;
	float fConst18;
	float fConst19;
	float fRec21[3];
	float fRec23[3];
	float fRec22[2];
	float fConst20;
	float fConst21;
	float fConst22;
	float fRec24[3];
	float fRec26[3];
	float fRec25[2];
	float fConst23;
	float fConst24;
	float fConst25;
	float fRec27[3];
	float fRec29[3];
	float fRec28[2];
	float fConst26;
	float fConst27;
	float fConst28;
	float fRec30[3];
	float fRec32[3];
	float fRec31[2];
	float fConst29;
	float fConst30;
	float fConst31;
	float fRec33[3];
	float fRec35[3];
	float fRec34[2];
	float fConst32;
	float fConst33;
	float fConst34;
	float fRec36[3];
	float fRec38[3];
	float fRec37[2];
	FAUSTFLOAT fHslider14;
	
 public:
	vocoderDsp() {
	}
	
	vocoderDsp(const vocoderDsp&) = default;
	
	virtual ~vocoderDsp() = default;
	
	vocoderDsp& operator=(const vocoderDsp&) = default;
	
	void metadata(Meta* m) { 
		m->declare("analyzers.lib/amp_follower_ar:author", "Jonatan Liljedahl, revised by Romain Michon");
		m->declare("analyzers.lib/name", "Faust Analyzer Library");
		m->declare("analyzers.lib/version", "1.3.0");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "1.22.0");
		m->declare("compile_options", "-lang cpp -fpga-mem-th 4 -ct 1 -cn vocoderDsp -es 1 -mcd 16 -mdd 1024 -mdy 33 -single -ftz 0");
		m->declare("filename", "vocoderDsp");
		m->declare("filters.lib/fir:author", "Julius O. Smith III");
		m->declare("filters.lib/fir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/fir:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/iir:author", "Julius O. Smith III");
		m->declare("filters.lib/iir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/iir:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/lowpass0_highpass1", "MIT-style STK-4.3 license");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/pole:author", "Julius O. Smith III");
		m->declare("filters.lib/pole:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/pole:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/resonbp:author", "Julius O. Smith III");
		m->declare("filters.lib/resonbp:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/resonbp:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf2:author", "Julius O. Smith III");
		m->declare("filters.lib/tf2:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf2:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf2s:author", "Julius O. Smith III");
		m->declare("filters.lib/tf2s:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf2s:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/version", "1.7.1");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.9.0");
		m->declare("name", "vocoderDsp");
		m->declare("noises.lib/name", "Faust Noise Generator Library");
		m->declare("noises.lib/version", "1.5.0");
		m->declare("options", "[midi:on][nvoices:8]");
		m->declare("oscillators.lib/lf_sawpos:author", "Bart Brouns, revised by Stéphane Letz");
		m->declare("oscillators.lib/lf_sawpos:licence", "STK-4.3");
		m->declare("oscillators.lib/name", "Faust Oscillator Library");
		m->declare("oscillators.lib/saw2ptr:author", "Julius O. Smith III");
		m->declare("oscillators.lib/saw2ptr:license", "STK-4.3");
		m->declare("oscillators.lib/sawN:author", "Julius O. Smith III");
		m->declare("oscillators.lib/sawN:license", "STK-4.3");
		m->declare("oscillators.lib/version", "1.7.0");
		m->declare("platform.lib/name", "Generic Platform Library");
		m->declare("platform.lib/version", "1.3.0");
		m->declare("signals.lib/name", "Faust Routing Library");
		m->declare("signals.lib/onePoleSwitching:author", "Jonatan Liljedahl, revised by Dario Sanfilippo");
		m->declare("signals.lib/onePoleSwitching:licence", "STK-4.3");
		m->declare("signals.lib/version", "1.6.0");
		m->declare("vaeffects.lib/name", "Faust Virtual Analog Filter Effect Library");
		m->declare("vaeffects.lib/oneVocoderBand:author", "Romain Michon");
		m->declare("vaeffects.lib/version", "1.5.0");
	}

	virtual int getNumInputs() {
		return 1;
	}
	virtual int getNumOutputs() {
		return 2;
	}
	
	static void classInit(int sample_rate) {
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		fConst0 = std::min<float>(1.92e+05f, std::max<float>(1.0f, static_cast<float>(fSampleRate)));
		fConst1 = std::tan(40212.387f / fConst0);
		fConst2 = 2.0f * (1.0f - 1.0f / vocoderDsp_faustpower2_f(fConst1));
		fConst3 = 1.0f / fConst1;
		fConst4 = 1.0f / fConst0;
		fConst5 = 0.25f * fConst0;
		fConst6 = 0.5f * fConst0;
		fConst7 = 4.0f / fConst0;
		fConst8 = std::tan(21549.283f / fConst0);
		fConst9 = 2.0f * (1.0f - 1.0f / vocoderDsp_faustpower2_f(fConst8));
		fConst10 = 1.0f / fConst8;
		fConst11 = std::tan(11547.976f / fConst0);
		fConst12 = 2.0f * (1.0f - 1.0f / vocoderDsp_faustpower2_f(fConst11));
		fConst13 = 1.0f / fConst11;
		fConst14 = std::tan(6188.4067f / fConst0);
		fConst15 = 2.0f * (1.0f - 1.0f / vocoderDsp_faustpower2_f(fConst14));
		fConst16 = 1.0f / fConst14;
		fConst17 = std::tan(3316.2852f / fConst0);
		fConst18 = 2.0f * (1.0f - 1.0f / vocoderDsp_faustpower2_f(fConst17));
		fConst19 = 1.0f / fConst17;
		fConst20 = std::tan(1777.1532f / fConst0);
		fConst21 = 2.0f * (1.0f - 1.0f / vocoderDsp_faustpower2_f(fConst20));
		fConst22 = 1.0f / fConst20;
		fConst23 = std::tan(952.3528f / fConst0);
		fConst24 = 2.0f * (1.0f - 1.0f / vocoderDsp_faustpower2_f(fConst23));
		fConst25 = 1.0f / fConst23;
		fConst26 = std::tan(510.35324f / fConst0);
		fConst27 = 2.0f * (1.0f - 1.0f / vocoderDsp_faustpower2_f(fConst26));
		fConst28 = 1.0f / fConst26;
		fConst29 = std::tan(273.49152f / fConst0);
		fConst30 = 2.0f * (1.0f - 1.0f / vocoderDsp_faustpower2_f(fConst29));
		fConst31 = 1.0f / fConst29;
		fConst32 = std::tan(146.56049f / fConst0);
		fConst33 = 2.0f * (1.0f - 1.0f / vocoderDsp_faustpower2_f(fConst32));
		fConst34 = 1.0f / fConst32;
	}
	
	virtual void instanceResetUserInterface() {
		fHslider0 = static_cast<FAUSTFLOAT>(1.0f);
		fHslider1 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider2 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider3 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider4 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider5 = static_cast<FAUSTFLOAT>(3e+02f);
		fCheckbox0 = static_cast<FAUSTFLOAT>(0.0f);
		fCheckbox1 = static_cast<FAUSTFLOAT>(0.0f);
		fCheckbox2 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider6 = static_cast<FAUSTFLOAT>(1.0f);
		fHslider7 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider8 = static_cast<FAUSTFLOAT>(0.0f);
		fCheckbox3 = static_cast<FAUSTFLOAT>(0.0f);
		fCheckbox4 = static_cast<FAUSTFLOAT>(0.0f);
		fCheckbox5 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider9 = static_cast<FAUSTFLOAT>(1.0f);
		fHslider10 = static_cast<FAUSTFLOAT>(1.0f);
		fButton0 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider11 = static_cast<FAUSTFLOAT>(1.0f);
		fHslider12 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider13 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider14 = static_cast<FAUSTFLOAT>(1.0f);
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
			iVec0[l0] = 0;
		}
		for (int l1 = 0; l1 < 2; l1 = l1 + 1) {
			iRec1[l1] = 0;
		}
		for (int l2 = 0; l2 < 2; l2 = l2 + 1) {
			fRec2[l2] = 0.0f;
		}
		for (int l3 = 0; l3 < 2; l3 = l3 + 1) {
			fRec5[l3] = 0.0f;
		}
		for (int l4 = 0; l4 < 2; l4 = l4 + 1) {
			fVec1[l4] = 0.0f;
		}
		IOTA0 = 0;
		for (int l5 = 0; l5 < 4096; l5 = l5 + 1) {
			fVec2[l5] = 0.0f;
		}
		for (int l6 = 0; l6 < 2; l6 = l6 + 1) {
			fRec4[l6] = 0.0f;
		}
		for (int l7 = 0; l7 < 2; l7 = l7 + 1) {
			fRec6[l7] = 0.0f;
		}
		for (int l8 = 0; l8 < 2; l8 = l8 + 1) {
			fRec9[l8] = 0.0f;
		}
		for (int l9 = 0; l9 < 2; l9 = l9 + 1) {
			fVec3[l9] = 0.0f;
		}
		for (int l10 = 0; l10 < 4096; l10 = l10 + 1) {
			fVec4[l10] = 0.0f;
		}
		for (int l11 = 0; l11 < 2; l11 = l11 + 1) {
			fRec8[l11] = 0.0f;
		}
		for (int l12 = 0; l12 < 3; l12 = l12 + 1) {
			fRec0[l12] = 0.0f;
		}
		for (int l13 = 0; l13 < 3; l13 = l13 + 1) {
			fRec11[l13] = 0.0f;
		}
		for (int l14 = 0; l14 < 2; l14 = l14 + 1) {
			fRec10[l14] = 0.0f;
		}
		for (int l15 = 0; l15 < 3; l15 = l15 + 1) {
			fRec12[l15] = 0.0f;
		}
		for (int l16 = 0; l16 < 3; l16 = l16 + 1) {
			fRec14[l16] = 0.0f;
		}
		for (int l17 = 0; l17 < 2; l17 = l17 + 1) {
			fRec13[l17] = 0.0f;
		}
		for (int l18 = 0; l18 < 3; l18 = l18 + 1) {
			fRec15[l18] = 0.0f;
		}
		for (int l19 = 0; l19 < 3; l19 = l19 + 1) {
			fRec17[l19] = 0.0f;
		}
		for (int l20 = 0; l20 < 2; l20 = l20 + 1) {
			fRec16[l20] = 0.0f;
		}
		for (int l21 = 0; l21 < 3; l21 = l21 + 1) {
			fRec18[l21] = 0.0f;
		}
		for (int l22 = 0; l22 < 3; l22 = l22 + 1) {
			fRec20[l22] = 0.0f;
		}
		for (int l23 = 0; l23 < 2; l23 = l23 + 1) {
			fRec19[l23] = 0.0f;
		}
		for (int l24 = 0; l24 < 3; l24 = l24 + 1) {
			fRec21[l24] = 0.0f;
		}
		for (int l25 = 0; l25 < 3; l25 = l25 + 1) {
			fRec23[l25] = 0.0f;
		}
		for (int l26 = 0; l26 < 2; l26 = l26 + 1) {
			fRec22[l26] = 0.0f;
		}
		for (int l27 = 0; l27 < 3; l27 = l27 + 1) {
			fRec24[l27] = 0.0f;
		}
		for (int l28 = 0; l28 < 3; l28 = l28 + 1) {
			fRec26[l28] = 0.0f;
		}
		for (int l29 = 0; l29 < 2; l29 = l29 + 1) {
			fRec25[l29] = 0.0f;
		}
		for (int l30 = 0; l30 < 3; l30 = l30 + 1) {
			fRec27[l30] = 0.0f;
		}
		for (int l31 = 0; l31 < 3; l31 = l31 + 1) {
			fRec29[l31] = 0.0f;
		}
		for (int l32 = 0; l32 < 2; l32 = l32 + 1) {
			fRec28[l32] = 0.0f;
		}
		for (int l33 = 0; l33 < 3; l33 = l33 + 1) {
			fRec30[l33] = 0.0f;
		}
		for (int l34 = 0; l34 < 3; l34 = l34 + 1) {
			fRec32[l34] = 0.0f;
		}
		for (int l35 = 0; l35 < 2; l35 = l35 + 1) {
			fRec31[l35] = 0.0f;
		}
		for (int l36 = 0; l36 < 3; l36 = l36 + 1) {
			fRec33[l36] = 0.0f;
		}
		for (int l37 = 0; l37 < 3; l37 = l37 + 1) {
			fRec35[l37] = 0.0f;
		}
		for (int l38 = 0; l38 < 2; l38 = l38 + 1) {
			fRec34[l38] = 0.0f;
		}
		for (int l39 = 0; l39 < 3; l39 = l39 + 1) {
			fRec36[l39] = 0.0f;
		}
		for (int l40 = 0; l40 < 3; l40 = l40 + 1) {
			fRec38[l40] = 0.0f;
		}
		for (int l41 = 0; l41 < 2; l41 = l41 + 1) {
			fRec37[l41] = 0.0f;
		}
	}
	
	virtual void init(int sample_rate) {
		classInit(sample_rate);
		instanceInit(sample_rate);
	}
	
	virtual void instanceInit(int sample_rate) {
		instanceConstants(sample_rate);
		instanceResetUserInterface();
		instanceClear();
	}
	
	virtual vocoderDsp* clone() {
		return new vocoderDsp(*this);
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("vocoderDsp");
		ui_interface->openVerticalBox("NOISE");
		ui_interface->addHorizontalSlider("level", &fHslider1, FAUSTFLOAT(0.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
		ui_interface->closeBox();
		ui_interface->openVerticalBox("OSC1");
		ui_interface->addHorizontalSlider("detune", &fHslider8, FAUSTFLOAT(0.0f), FAUSTFLOAT(-5e+01f), FAUSTFLOAT(5e+01f), FAUSTFLOAT(1.0f));
		ui_interface->declare(&fHslider9, "group", "osc1");
		ui_interface->addHorizontalSlider("level", &fHslider9, FAUSTFLOAT(1.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
		ui_interface->addHorizontalSlider("octave", &fHslider7, FAUSTFLOAT(0.0f), FAUSTFLOAT(-1.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(1.0f));
		ui_interface->addCheckButton("saw", &fCheckbox3);
		ui_interface->addCheckButton("squ", &fCheckbox5);
		ui_interface->addCheckButton("tri", &fCheckbox4);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("OSC2");
		ui_interface->addHorizontalSlider("detune", &fHslider3, FAUSTFLOAT(0.0f), FAUSTFLOAT(-5e+01f), FAUSTFLOAT(5e+01f), FAUSTFLOAT(1.0f));
		ui_interface->declare(&fHslider6, "group", "osc2");
		ui_interface->addHorizontalSlider("level", &fHslider6, FAUSTFLOAT(1.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
		ui_interface->addHorizontalSlider("octave", &fHslider2, FAUSTFLOAT(0.0f), FAUSTFLOAT(-1.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(1.0f));
		ui_interface->addCheckButton("saw", &fCheckbox0);
		ui_interface->addCheckButton("squ", &fCheckbox2);
		ui_interface->addCheckButton("tri", &fCheckbox1);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("VOCODER");
		ui_interface->addHorizontalSlider("attack", &fHslider13, FAUSTFLOAT(0.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(0.1f), FAUSTFLOAT(0.001f));
		ui_interface->addHorizontalSlider("bandwidth", &fHslider0, FAUSTFLOAT(1.0f), FAUSTFLOAT(0.1f), FAUSTFLOAT(2.0f), FAUSTFLOAT(0.01f));
		ui_interface->addHorizontalSlider("release", &fHslider12, FAUSTFLOAT(0.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(0.1f), FAUSTFLOAT(0.001f));
		ui_interface->closeBox();
		ui_interface->declare(&fHslider4, "midi", "pitchwheel");
		ui_interface->addHorizontalSlider("bend", &fHslider4, FAUSTFLOAT(0.0f), FAUSTFLOAT(-2.0f), FAUSTFLOAT(2.0f), FAUSTFLOAT(0.01f));
		ui_interface->addHorizontalSlider("freq", &fHslider5, FAUSTFLOAT(3e+02f), FAUSTFLOAT(5e+01f), FAUSTFLOAT(2e+03f), FAUSTFLOAT(0.01f));
		ui_interface->addHorizontalSlider("gain", &fHslider10, FAUSTFLOAT(1.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
		ui_interface->addButton("gate", &fButton0);
		ui_interface->addHorizontalSlider("mic", &fHslider11, FAUSTFLOAT(1.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1e+01f), FAUSTFLOAT(0.01f));
		ui_interface->addHorizontalSlider("vol", &fHslider14, FAUSTFLOAT(1.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1e+01f), FAUSTFLOAT(0.01f));
		ui_interface->closeBox();
	}
	
	virtual void compute(int count, FAUSTFLOAT** RESTRICT inputs, FAUSTFLOAT** RESTRICT outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		float fSlow0 = static_cast<float>(fHslider0);
		float fSlow1 = 0.46411327f * fSlow0;
		float fSlow2 = fConst3 * (fConst3 - fSlow1) + 1.0f;
		float fSlow3 = fConst3 * (fConst3 + fSlow1) + 1.0f;
		float fSlow4 = 1.0f / fSlow3;
		float fSlow5 = 4.656613e-10f * static_cast<float>(fHslider1);
		float fSlow6 = std::pow(2.0f, static_cast<float>(fHslider2));
		float fSlow7 = static_cast<float>(fHslider5) * std::pow(2.0f, 0.083333336f * static_cast<float>(fHslider4));
		float fSlow8 = fSlow7 * std::pow(2.0f, 0.00083333335f * static_cast<float>(fHslider3));
		float fSlow9 = fSlow8 * fSlow6;
		float fSlow10 = std::max<float>(1.1920929e-07f, std::fabs(fSlow9));
		float fSlow11 = fConst4 * fSlow10;
		float fSlow12 = 1.0f - fConst0 / fSlow10;
		float fSlow13 = static_cast<float>(fCheckbox0);
		float fSlow14 = std::max<float>(fSlow9, 23.44895f);
		float fSlow15 = std::max<float>(2e+01f, std::fabs(fSlow14));
		float fSlow16 = fConst4 * fSlow15;
		float fSlow17 = fConst5 / fSlow15;
		float fSlow18 = std::max<float>(0.0f, std::min<float>(2047.0f, fConst6 / fSlow14));
		int iSlow19 = static_cast<int>(fSlow18);
		int iSlow20 = iSlow19 + 1;
		float fSlow21 = std::floor(fSlow18);
		float fSlow22 = fSlow18 - fSlow21;
		float fSlow23 = fSlow21 + (1.0f - fSlow18);
		float fSlow24 = fConst7 * fSlow8 * static_cast<float>(fCheckbox1) * fSlow6;
		float fSlow25 = static_cast<float>(fCheckbox2);
		float fSlow26 = static_cast<float>(fHslider6);
		float fSlow27 = std::pow(2.0f, static_cast<float>(fHslider7));
		float fSlow28 = fSlow7 * std::pow(2.0f, 0.00083333335f * static_cast<float>(fHslider8));
		float fSlow29 = fSlow28 * fSlow27;
		float fSlow30 = std::max<float>(1.1920929e-07f, std::fabs(fSlow29));
		float fSlow31 = fConst4 * fSlow30;
		float fSlow32 = 1.0f - fConst0 / fSlow30;
		float fSlow33 = static_cast<float>(fCheckbox3);
		float fSlow34 = std::max<float>(fSlow29, 23.44895f);
		float fSlow35 = std::max<float>(2e+01f, std::fabs(fSlow34));
		float fSlow36 = fConst4 * fSlow35;
		float fSlow37 = fConst5 / fSlow35;
		float fSlow38 = std::max<float>(0.0f, std::min<float>(2047.0f, fConst6 / fSlow34));
		int iSlow39 = static_cast<int>(fSlow38);
		int iSlow40 = iSlow39 + 1;
		float fSlow41 = std::floor(fSlow38);
		float fSlow42 = fSlow38 - fSlow41;
		float fSlow43 = fSlow41 + (1.0f - fSlow38);
		float fSlow44 = fConst7 * fSlow28 * static_cast<float>(fCheckbox4) * fSlow27;
		float fSlow45 = static_cast<float>(fCheckbox5);
		float fSlow46 = static_cast<float>(fHslider9);
		float fSlow47 = static_cast<float>(fButton0) * static_cast<float>(fHslider10);
		float fSlow48 = static_cast<float>(fHslider11);
		float fSlow49 = fConst3 / fSlow3;
		float fSlow50 = static_cast<float>(fHslider12);
		int iSlow51 = std::fabs(fSlow50) < 1.1920929e-07f;
		float fSlow52 = ((iSlow51) ? 0.0f : std::exp(-(fConst4 / ((iSlow51) ? 1.0f : fSlow50))));
		float fSlow53 = static_cast<float>(fHslider13);
		int iSlow54 = std::fabs(fSlow53) < 1.1920929e-07f;
		float fSlow55 = ((iSlow54) ? 0.0f : std::exp(-(fConst4 / ((iSlow54) ? 1.0f : fSlow53))));
		float fSlow56 = 0.46411327f * fSlow0;
		float fSlow57 = fConst10 * (fConst10 - fSlow56) + 1.0f;
		float fSlow58 = fConst10 * (fConst10 + fSlow56) + 1.0f;
		float fSlow59 = 1.0f / fSlow58;
		float fSlow60 = fConst10 / fSlow58;
		float fSlow61 = 0.46411327f * fSlow0;
		float fSlow62 = fConst13 * (fConst13 - fSlow61) + 1.0f;
		float fSlow63 = fConst13 * (fConst13 + fSlow61) + 1.0f;
		float fSlow64 = 1.0f / fSlow63;
		float fSlow65 = fConst13 / fSlow63;
		float fSlow66 = 0.46411327f * fSlow0;
		float fSlow67 = fConst16 * (fConst16 - fSlow66) + 1.0f;
		float fSlow68 = fConst16 * (fConst16 + fSlow66) + 1.0f;
		float fSlow69 = 1.0f / fSlow68;
		float fSlow70 = fConst16 / fSlow68;
		float fSlow71 = fConst19 * (fConst19 - fSlow1) + 1.0f;
		float fSlow72 = fConst19 * (fConst19 + fSlow1) + 1.0f;
		float fSlow73 = 1.0f / fSlow72;
		float fSlow74 = fConst19 / fSlow72;
		float fSlow75 = 0.46411327f * fSlow0;
		float fSlow76 = fConst22 * (fConst22 - fSlow75) + 1.0f;
		float fSlow77 = fConst22 * (fConst22 + fSlow75) + 1.0f;
		float fSlow78 = 1.0f / fSlow77;
		float fSlow79 = fConst22 / fSlow77;
		float fSlow80 = fConst25 * (fConst25 - fSlow1) + 1.0f;
		float fSlow81 = fConst25 * (fConst25 + fSlow1) + 1.0f;
		float fSlow82 = 1.0f / fSlow81;
		float fSlow83 = fConst25 / fSlow81;
		float fSlow84 = 0.46411327f * fSlow0;
		float fSlow85 = fConst28 * (fConst28 - fSlow84) + 1.0f;
		float fSlow86 = fConst28 * (fConst28 + fSlow84) + 1.0f;
		float fSlow87 = 1.0f / fSlow86;
		float fSlow88 = fConst28 / fSlow86;
		float fSlow89 = fConst31 * (fConst31 - fSlow84) + 1.0f;
		float fSlow90 = fConst31 * (fConst31 + fSlow84) + 1.0f;
		float fSlow91 = 1.0f / fSlow90;
		float fSlow92 = fConst31 / fSlow90;
		float fSlow93 = fConst34 * (fConst34 - fSlow84) + 1.0f;
		float fSlow94 = fConst34 * (fConst34 + fSlow84) + 1.0f;
		float fSlow95 = 1.0f / fSlow94;
		float fSlow96 = fConst34 / fSlow94;
		float fSlow97 = static_cast<float>(fHslider14);
		for (int i0 = 0; i0 < count; i0 = i0 + 1) {
			iVec0[0] = 1;
			iRec1[0] = 1103515245 * iRec1[1] + 12345;
			float fTemp0 = fSlow11 + fRec2[1] + -1.0f;
			int iTemp1 = fTemp0 < 0.0f;
			float fTemp2 = fSlow11 + fRec2[1];
			fRec2[0] = ((iTemp1) ? fTemp2 : fTemp0);
			float fRec3 = ((iTemp1) ? fTemp2 : fSlow11 + fRec2[1] + fSlow12 * fTemp0);
			int iTemp3 = 1 - iVec0[1];
			float fTemp4 = ((iTemp3) ? 0.0f : fSlow16 + fRec5[1]);
			fRec5[0] = fTemp4 - std::floor(fTemp4);
			float fTemp5 = vocoderDsp_faustpower2_f(2.0f * fRec5[0] + -1.0f);
			fVec1[0] = fTemp5;
			float fTemp6 = static_cast<float>(iVec0[1]);
			float fTemp7 = fSlow17 * fTemp6 * (fTemp5 - fVec1[1]);
			fVec2[IOTA0 & 4095] = fTemp7;
			float fTemp8 = fSlow23 * fVec2[(IOTA0 - iSlow19) & 4095] + fSlow22 * fVec2[(IOTA0 - iSlow20) & 4095];
			fRec4[0] = fTemp7 + 0.999f * fRec4[1] - fTemp8;
			float fTemp9 = fSlow31 + fRec6[1] + -1.0f;
			int iTemp10 = fTemp9 < 0.0f;
			float fTemp11 = fSlow31 + fRec6[1];
			fRec6[0] = ((iTemp10) ? fTemp11 : fTemp9);
			float fRec7 = ((iTemp10) ? fTemp11 : fSlow31 + fRec6[1] + fSlow32 * fTemp9);
			float fTemp12 = ((iTemp3) ? 0.0f : fSlow36 + fRec9[1]);
			fRec9[0] = fTemp12 - std::floor(fTemp12);
			float fTemp13 = vocoderDsp_faustpower2_f(2.0f * fRec9[0] + -1.0f);
			fVec3[0] = fTemp13;
			float fTemp14 = fSlow37 * fTemp6 * (fTemp13 - fVec3[1]);
			fVec4[IOTA0 & 4095] = fTemp14;
			float fTemp15 = fSlow43 * fVec4[(IOTA0 - iSlow39) & 4095] + fSlow42 * fVec4[(IOTA0 - iSlow40) & 4095];
			fRec8[0] = fTemp14 + 0.999f * fRec8[1] - fTemp15;
			float fTemp16 = fSlow47 * (fSlow46 * (fSlow45 * (fTemp14 - fTemp15) + fSlow44 * fRec8[0] + fSlow33 * (2.0f * fRec7 + -1.0f)) + fSlow26 * (fSlow25 * (fTemp7 - fTemp8) + fSlow24 * fRec4[0] + fSlow13 * (2.0f * fRec3 + -1.0f)) + fSlow5 * static_cast<float>(iRec1[0]));
			fRec0[0] = fTemp16 - fSlow4 * (fSlow2 * fRec0[2] + fConst2 * fRec0[1]);
			float fTemp17 = fSlow48 * static_cast<float>(input0[i0]);
			fRec11[0] = fTemp17 - fSlow4 * (fSlow2 * fRec11[2] + fConst2 * fRec11[1]);
			float fTemp18 = std::fabs(fSlow49 * (fRec11[0] - fRec11[2]));
			float fTemp19 = ((fTemp18 > fRec10[1]) ? fSlow55 : fSlow52);
			fRec10[0] = fTemp18 * (1.0f - fTemp19) + fRec10[1] * fTemp19;
			fRec12[0] = fTemp16 - fSlow59 * (fSlow57 * fRec12[2] + fConst9 * fRec12[1]);
			fRec14[0] = fTemp17 - fSlow59 * (fSlow57 * fRec14[2] + fConst9 * fRec14[1]);
			float fTemp20 = std::fabs(fSlow60 * (fRec14[0] - fRec14[2]));
			float fTemp21 = ((fTemp20 > fRec13[1]) ? fSlow55 : fSlow52);
			fRec13[0] = fTemp20 * (1.0f - fTemp21) + fRec13[1] * fTemp21;
			fRec15[0] = fTemp16 - fSlow64 * (fSlow62 * fRec15[2] + fConst12 * fRec15[1]);
			fRec17[0] = fTemp17 - fSlow64 * (fSlow62 * fRec17[2] + fConst12 * fRec17[1]);
			float fTemp22 = std::fabs(fSlow65 * (fRec17[0] - fRec17[2]));
			float fTemp23 = ((fTemp22 > fRec16[1]) ? fSlow55 : fSlow52);
			fRec16[0] = fTemp22 * (1.0f - fTemp23) + fRec16[1] * fTemp23;
			fRec18[0] = fTemp16 - fSlow69 * (fSlow67 * fRec18[2] + fConst15 * fRec18[1]);
			fRec20[0] = fTemp17 - fSlow69 * (fSlow67 * fRec20[2] + fConst15 * fRec20[1]);
			float fTemp24 = std::fabs(fSlow70 * (fRec20[0] - fRec20[2]));
			float fTemp25 = ((fTemp24 > fRec19[1]) ? fSlow55 : fSlow52);
			fRec19[0] = fTemp24 * (1.0f - fTemp25) + fRec19[1] * fTemp25;
			fRec21[0] = fTemp16 - fSlow73 * (fSlow71 * fRec21[2] + fConst18 * fRec21[1]);
			fRec23[0] = fTemp17 - fSlow73 * (fSlow71 * fRec23[2] + fConst18 * fRec23[1]);
			float fTemp26 = std::fabs(fSlow74 * (fRec23[0] - fRec23[2]));
			float fTemp27 = ((fTemp26 > fRec22[1]) ? fSlow55 : fSlow52);
			fRec22[0] = fTemp26 * (1.0f - fTemp27) + fRec22[1] * fTemp27;
			fRec24[0] = fTemp16 - fSlow78 * (fSlow76 * fRec24[2] + fConst21 * fRec24[1]);
			fRec26[0] = fTemp17 - fSlow78 * (fSlow76 * fRec26[2] + fConst21 * fRec26[1]);
			float fTemp28 = std::fabs(fSlow79 * (fRec26[0] - fRec26[2]));
			float fTemp29 = ((fTemp28 > fRec25[1]) ? fSlow55 : fSlow52);
			fRec25[0] = fTemp28 * (1.0f - fTemp29) + fRec25[1] * fTemp29;
			fRec27[0] = fTemp16 - fSlow82 * (fSlow80 * fRec27[2] + fConst24 * fRec27[1]);
			fRec29[0] = fTemp17 - fSlow82 * (fSlow80 * fRec29[2] + fConst24 * fRec29[1]);
			float fTemp30 = std::fabs(fSlow83 * (fRec29[0] - fRec29[2]));
			float fTemp31 = ((fTemp30 > fRec28[1]) ? fSlow55 : fSlow52);
			fRec28[0] = fTemp30 * (1.0f - fTemp31) + fRec28[1] * fTemp31;
			fRec30[0] = fTemp16 - fSlow87 * (fSlow85 * fRec30[2] + fConst27 * fRec30[1]);
			fRec32[0] = fTemp17 - fSlow87 * (fSlow85 * fRec32[2] + fConst27 * fRec32[1]);
			float fTemp32 = std::fabs(fSlow88 * (fRec32[0] - fRec32[2]));
			float fTemp33 = ((fTemp32 > fRec31[1]) ? fSlow55 : fSlow52);
			fRec31[0] = fTemp32 * (1.0f - fTemp33) + fRec31[1] * fTemp33;
			fRec33[0] = fTemp16 - fSlow91 * (fSlow89 * fRec33[2] + fConst30 * fRec33[1]);
			fRec35[0] = fTemp17 - fSlow91 * (fSlow89 * fRec35[2] + fConst30 * fRec35[1]);
			float fTemp34 = std::fabs(fSlow92 * (fRec35[0] - fRec35[2]));
			float fTemp35 = ((fTemp34 > fRec34[1]) ? fSlow55 : fSlow52);
			fRec34[0] = fTemp34 * (1.0f - fTemp35) + fRec34[1] * fTemp35;
			fRec36[0] = fTemp16 - fSlow95 * (fSlow93 * fRec36[2] + fConst33 * fRec36[1]);
			fRec38[0] = fTemp17 - fSlow95 * (fSlow93 * fRec38[2] + fConst33 * fRec38[1]);
			float fTemp36 = std::fabs(fSlow96 * (fRec38[0] - fRec38[2]));
			float fTemp37 = ((fTemp36 > fRec37[1]) ? fSlow55 : fSlow52);
			fRec37[0] = fTemp36 * (1.0f - fTemp37) + fRec37[1] * fTemp37;
			float fTemp38 = fSlow97 * (fSlow96 * fRec37[0] * (fRec36[0] - fRec36[2]) + fSlow92 * fRec34[0] * (fRec33[0] - fRec33[2]) + fSlow88 * fRec31[0] * (fRec30[0] - fRec30[2]) + fSlow83 * fRec28[0] * (fRec27[0] - fRec27[2]) + fSlow79 * fRec25[0] * (fRec24[0] - fRec24[2]) + fSlow74 * fRec22[0] * (fRec21[0] - fRec21[2]) + fSlow70 * fRec19[0] * (fRec18[0] - fRec18[2]) + fSlow65 * fRec16[0] * (fRec15[0] - fRec15[2]) + fSlow60 * fRec13[0] * (fRec12[0] - fRec12[2]) + fSlow49 * fRec10[0] * (fRec0[0] - fRec0[2]));
			output0[i0] = static_cast<FAUSTFLOAT>(fTemp38);
			output1[i0] = static_cast<FAUSTFLOAT>(fTemp38);
			iVec0[1] = iVec0[0];
			iRec1[1] = iRec1[0];
			fRec2[1] = fRec2[0];
			fRec5[1] = fRec5[0];
			fVec1[1] = fVec1[0];
			IOTA0 = IOTA0 + 1;
			fRec4[1] = fRec4[0];
			fRec6[1] = fRec6[0];
			fRec9[1] = fRec9[0];
			fVec3[1] = fVec3[0];
			fRec8[1] = fRec8[0];
			fRec0[2] = fRec0[1];
			fRec0[1] = fRec0[0];
			fRec11[2] = fRec11[1];
			fRec11[1] = fRec11[0];
			fRec10[1] = fRec10[0];
			fRec12[2] = fRec12[1];
			fRec12[1] = fRec12[0];
			fRec14[2] = fRec14[1];
			fRec14[1] = fRec14[0];
			fRec13[1] = fRec13[0];
			fRec15[2] = fRec15[1];
			fRec15[1] = fRec15[0];
			fRec17[2] = fRec17[1];
			fRec17[1] = fRec17[0];
			fRec16[1] = fRec16[0];
			fRec18[2] = fRec18[1];
			fRec18[1] = fRec18[0];
			fRec20[2] = fRec20[1];
			fRec20[1] = fRec20[0];
			fRec19[1] = fRec19[0];
			fRec21[2] = fRec21[1];
			fRec21[1] = fRec21[0];
			fRec23[2] = fRec23[1];
			fRec23[1] = fRec23[0];
			fRec22[1] = fRec22[0];
			fRec24[2] = fRec24[1];
			fRec24[1] = fRec24[0];
			fRec26[2] = fRec26[1];
			fRec26[1] = fRec26[0];
			fRec25[1] = fRec25[0];
			fRec27[2] = fRec27[1];
			fRec27[1] = fRec27[0];
			fRec29[2] = fRec29[1];
			fRec29[1] = fRec29[0];
			fRec28[1] = fRec28[0];
			fRec30[2] = fRec30[1];
			fRec30[1] = fRec30[0];
			fRec32[2] = fRec32[1];
			fRec32[1] = fRec32[0];
			fRec31[1] = fRec31[0];
			fRec33[2] = fRec33[1];
			fRec33[1] = fRec33[0];
			fRec35[2] = fRec35[1];
			fRec35[1] = fRec35[0];
			fRec34[1] = fRec34[0];
			fRec36[2] = fRec36[1];
			fRec36[1] = fRec36[0];
			fRec38[2] = fRec38[1];
			fRec38[1] = fRec38[0];
			fRec37[1] = fRec37[0];
		}
	}

};

#endif
