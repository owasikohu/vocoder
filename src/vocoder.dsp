declare options "[midi:on][nvoices:12]";
import("stdfaust.lib");


f = hslider("freq",300,50,2000,0.01);
bend = ba.semi2ratio(hslider("bend[midi:pitchwheel]",0,-2,2,0.01)) : si.polySmooth(gate,0.999,1);
gain = hslider("gain",1,0,10,0.01);
gate = button("gate");
freq = f*bend;

osc1_group(x) = vgroup("OSC1",x);

osc1_pitch  = osc1_group(2 ^ hslider("octave", 0, -1, 1, 1));         // オクターブシフト
osc1_detune = osc1_group(ba.semi2ratio(hslider("detune", 0, -50, 50, 1)/100)); // cent単位 detune
osc1_level  = osc1_group(hslider("level[group:osc1]", 1, 0, 1, 0.01));

osc1_freq = freq * osc1_pitch * osc1_detune;

osc1_squ = os.square(osc1_freq)*osc1_group(checkbox("squ")); 
osc1_tri = os.triangle(osc1_freq)*osc1_group(checkbox("tri"));
osc1_saw = os.sawtooth(osc1_freq)*osc1_group(checkbox("saw"));

osc1 = (osc1_squ + osc1_tri + osc1_saw)*osc1_level;



osc2_group(x) = vgroup("OSC2",x);

osc2_pitch  = osc2_group(2 ^ hslider("octave", 0, -1, 1, 1));         // オクターブシフト
osc2_detune = osc2_group(ba.semi2ratio(hslider("detune", 0, -50, 50, 1)/100)); // cent単位 detune
osc2_level  = osc2_group(hslider("level[group:osc2]", 1, 0, 1, 0.01));

osc2_freq = freq * osc2_pitch * osc2_detune;

osc2_squ = os.square(osc2_freq)*osc2_group(checkbox("squ")); 
osc2_tri = os.triangle(osc2_freq)*osc2_group(checkbox("tri"));
osc2_saw = os.sawtooth(osc2_freq)*osc2_group(checkbox("saw"));

osc2 = (osc2_squ + osc2_tri + osc2_saw)*osc2_level;

noise_group(x) = vgroup("NOISE",x);
noise_level = noise_group(hslider("level", 0, 0, 1, 0.01));

noise = no.noise*noise_level;
 
modulator = _;

carrier = (osc1+osc2+noise)*gate;

vocoder_group(x) = vgroup("VOCODER",x);

vocoder_attack = vocoder_group(hslider("attack", 0, 0, 10, 0.01));
vocoder_release = vocoder_group(hslider("release", 0, 0, 10, 0.01));
vocoder_bw = vocoder_group(hslider("bandwidth", 1, 0.1, 2, 0.01));
process = ve.vocoder(10, vocoder_attack, vocoder_release, vocoder_bw, modulator, carrier)*gain <: _,_;
