// tinyc.games audio - copyright 2023 Jer Wilson
//
// 1. Add SDL_INIT_AUDIO to your SDL_Init()
// 2. Call audioinit() before playing sound
// 3. Play tones with audio_tone()

#include <math.h>
#include <SDL2/SDL_audio.h>
#include "audio.h"

struct envelope envs[NUM_ENVS];
struct note music_notes[] = {
    {"A" , 0,   27.500, 12.374}, // Piano low
    {"A#", 0,   29.135, 11.680},
    {"B" , 0,   30.868, 11.024},
    {"C" , 1,   32.703, 10.405},
    {"C#", 1,   34.648,  9.821},
    {"D" , 1,   36.708,  9.270},
    {"D#", 1,   38.891,  8.750},
    {"E" , 1,   41.203,  8.259},
    {"F" , 1,   43.654,  7.795},
    {"F#", 1,   46.249,  7.358},
    {"G" , 1,   48.999,  6.945},
    {"G#", 1,   51.913,  6.555},
    {"A" , 1,   55.000,  6.187},
    {"A#", 1,   58.270,  5.840},
    {"B" , 1,   61.735,  5.512},
    {"C" , 2,   65.406,  5.203},
    {"C#", 2,   69.296,  4.911},
    {"D" , 2,   73.416,  4.635},
    {"D#", 2,   77.782,  4.375},
    {"E" , 2,   82.407,  4.129},
    {"F" , 2,   87.307,  3.898},
    {"F#", 2,   92.499,  3.679},
    {"G" , 2,   97.999,  3.472},
    {"G#", 2,  103.826,  3.278},
    {"A" , 2,  110.000,  3.094},
    {"A#", 2,  116.541,  2.920},
    {"B" , 2,  123.471,  2.756},
    {"C" , 3,  130.813,  2.601},
    {"C#", 3,  138.591,  2.455},
    {"D" , 3,  146.832,  2.318},
    {"D#", 3,  155.563,  2.187},
    {"E" , 3,  164.814,  2.065},
    {"F" , 3,  174.614,  1.949},
    {"F#", 3,  184.997,  1.839},
    {"G" , 3,  195.998,  1.736},
    {"G#", 3,  207.652,  1.639},
    {"A" , 3,  220.000,  1.547},
    {"A#", 3,  233.082,  1.460},
    {"B" , 3,  246.942,  1.378},
    {"C" , 4,  261.626,  1.301}, // middle C
    {"C#", 4,  277.183,  1.228},
    {"D" , 4,  293.665,  1.159},
    {"D#", 4,  311.127,  1.094},
    {"E" , 4,  329.628,  1.032},
    {"F" , 4,  349.228,  0.974},
    {"F#", 4,  369.994,  0.920},
    {"G" , 4,  391.995,  0.868},
    {"G#", 4,  415.305,  0.819},
    {"A" , 4,  440.000,  0.773}, // A 440
    {"A#", 4,  466.164,  0.730},
    {"B" , 4,  493.883,  0.689},
    {"C" , 5,  523.251,  0.650},
    {"C#", 5,  554.365,  0.614},
    {"D" , 5,  587.330,  0.579},
    {"D#", 5,  622.254,  0.547},
    {"E" , 5,  659.255,  0.516},
    {"F" , 5,  698.456,  0.487},
    {"F#", 5,  739.989,  0.460},
    {"G" , 5,  783.991,  0.434},
    {"G#", 5,  830.609,  0.410},
    {"A" , 5,  880.000,  0.387},
    {"A#", 5,  932.328,  0.365},
    {"B" , 5,  987.767,  0.345},
    };

void audio_tone(int shape, int note_lo, int note_hi,
                double attack, double decay, double sustain, double release)
{
        int note = note_lo + rand() % (note_hi - note_lo + 1);
        if (note < A0 || note > C6) return;

        envs[rand() % NUM_ENVS] = (struct envelope){
                .shape      = shape,
                .start_freq = music_notes[note].frequency,
                .volume     = 1.0 + 0.08 * music_notes[note].wavelength,
                .attack     = (attack                            ) / 1000.f,
                .decay      = (attack + decay                    ) / 1000.f,
                .sustain    = (attack + decay + sustain          ) / 1000.f,
                .release    = (attack + decay + sustain + release) / 1000.f,
        };
}

static void mix_audio(void *unused, unsigned char *stream, int len)
{
        short *out = (short*)stream;

        for (int j = 0; j < len/2; j++)
        {
                int samp = 0;

                for (int n = 0; n < NUM_ENVS; n++)
                {
                        struct envelope *e = envs + n;
                        if (e->pos >= e->release)
                                continue;

                        double freq = e->start_freq;
                        double wl   = 1.0 / freq;
                        double wl2  = wl / 2.0;
                        double frac = fmod(e->pos, wl);
                        double t = e->pos;
                        e->pos += 1.0 / 44100;

                        double veloc = e->volume * 2200;
                        if      (t <= e->attack ) veloc *= t / e->attack * 1.5;
                        else if (t <= e->decay  ) veloc *= (e->decay - t) / (e->decay - e->attack) * .5 + 1.0;
                        else if (t <= e->sustain) ;
                        else if (t <= e->release) veloc *= (e->release - t) / (e->release - e->sustain);

                        switch (e->shape)
                        {
                                case SINE:     samp += veloc * sin(frac * freq * 6.28318531);                      break;
                                case SQUARE:   samp += frac > wl2 ? veloc : -veloc;                                break;
                                case TRIANGLE: samp += (4 * freq * (frac > wl2 ? (wl - frac) : frac) - 1) * veloc; break;
                        }
                }

                out[j] = samp >  32767 ?  32768 :
                         samp < -32767 ? -32767 :
                         samp;
        }
}

void audio_init()
{
        SDL_AudioDeviceID id;
        SDL_AudioSpec actual, desired = {
                .freq = 44100,
                .format = AUDIO_S16LSB,
                .channels = 1,
                .samples = 512,
                .callback = mix_audio,
        };
        id = SDL_OpenAudioDevice(NULL, 0, &desired, &actual, SDL_AUDIO_ALLOW_ANY_CHANGE);
        if (!id)
                fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
        else
                SDL_PauseAudioDevice(id, 0);
}
