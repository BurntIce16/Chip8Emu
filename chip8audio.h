#include "miniaudio/miniaudio.h"

struct BeepState {
    ma_uint32  sr;          // sample rate
    ma_uint32  freq;        // Hz
    ma_uint32  phase;       // 32-bit phase accumulator
    ma_uint32  step;        // phase step per sample
    float      amp;         // 0..1
    volatile int beep_on;   // set to 1 while CHIP-8 sound timer > 0
};

// Remove this line - audio_cb is internal to chip8audio.cpp
// static void audio_cb(ma_device *dev, void *pOutput, const void * /*pInput*/, ma_uint32 frameCount);

// Call once at startup.
bool beep_init(unsigned hz = 440, float volume = 0.25f, ma_uint32 requestedSR = 48000);

// Call at shutdown.
void beep_shutdown();

// helpers
void beep_set_on(bool on);

void beep_set_freq(unsigned hz);

void beep_set_volume(float v);