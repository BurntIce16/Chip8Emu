// miniaudio_beep.hpp
#include "chip8audio.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"
#include <stdint.h>

static ma_device g_device;
static BeepState g = {48000, 440, 0, 0, 0.25f, 0};



static void recompute_step()
{
    // step = freq * 2^32 / sr
    g.step = (ma_uint64)g.freq * 0x100000000ull / (g.sr ? g.sr : 48000);
}

static void audio_cb(ma_device *dev, void *pOutput, const void * /*pInput*/, ma_uint32 frameCount)
{
    (void)dev;
    float *out = (float *)pOutput; // using ma_format_f32
    for (ma_uint32 i = 0; i < frameCount; ++i)
    {
        g.phase += g.step;
        float s = (g.phase & 0x80000000u) ? +g.amp : -g.amp; // square
        out[i] = g.beep_on ? s : 0.0f;
    }
}

// Call once at startup.
bool beep_init(unsigned hz, float volume, ma_uint32 requestedSR)
{
    g.freq = (ma_uint32)hz;
    g.amp = volume;

    ma_device_config cfg = ma_device_config_init(ma_device_type_playback);
    cfg.playback.format = ma_format_f32;
    cfg.playback.channels = 1;
    cfg.sampleRate = requestedSR;
    cfg.dataCallback = audio_cb;

    if (ma_device_init(nullptr, &cfg, &g_device) != MA_SUCCESS)
        return false;

    g.sr = g_device.sampleRate; // actual SR chosen by backend
    recompute_step();

    return ma_device_start(&g_device) == MA_SUCCESS;
}

// Call at shutdown.
void beep_shutdown()
{
    ma_device_uninit(&g_device);
}

// helpers
void beep_set_on(bool on)
{
    g.beep_on = on ? 1 : 0;
}

void beep_set_freq(unsigned hz)
{
    g.freq = (ma_uint32)hz;
    recompute_step();
}

void beep_set_volume(float v /*0..1*/)
{
    g.amp = v;
}
