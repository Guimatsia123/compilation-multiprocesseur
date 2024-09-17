#pragma once

#define NUM_ENVS 20

enum shape {SINE, SQUARE, TRIANGLE};

struct envelope {
        enum shape shape;
        double start_freq;
        double volume;
        double attack, decay, sustain, release; // envelope timings
        double pos;                             // current play position
	double noiseval;
	int    noisectr;
	int    noisesign;
};
extern struct envelope envs[NUM_ENVS];

enum notes {
        A0, As0, B0, C1, Cs1, D1, Ds1, E1, F1, Fs1, G1, Gs1,
        A1, As1, B1, C2, Cs2, D2, Ds2, E2, F2, Fs2, G2, Gs2,
        A2, As2, B2, C3, Cs3, D3, Ds3, E3, F3, Fs3, G3, Gs3,
        A3, As3, B3, C4, Cs4, D4, Ds4, E4, F4, Fs4, G4, Gs4,
        A4, As4, B4, C5, Cs5, D5, Ds5, E5, F5, Fs5, G5, Gs5,
        A5, As5, B5, C6,
};

struct note {
        char name[3];
        int octave;
        double frequency;
        double wavelength;
};

extern struct note music_notes[];

void audio_init();
void audio_tone(int shape, int note_lo, int note_hi,
                double attack, double decay, double sustain, double release);
