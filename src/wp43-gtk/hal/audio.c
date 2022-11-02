// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "hal/audio.h"

#include "hal/time.h"
#include <stdio.h>
#if defined(__MINGW64__)
  #define NOMINMAX
  #include <windows.h>
#elif defined(WITH_PULSEAUDIO)
  #include <pulse/simple.h>
#endif

#pragma GCC diagnostic ignored "-Wunused-parameter"

#if defined(WITH_PULSEAUDIO)
  void _audioPulse(uint32_t frequency, uint32_t durationMs) {
    pa_simple *s;
    pa_sample_spec ss;

    ss.format   = PA_SAMPLE_S16NE;
    ss.channels = 1;
    ss.rate     = 44100;

    s = pa_simple_new(NULL, "WP43", PA_STREAM_PLAYBACK, NULL, "BEEP/TONE", &ss, NULL, NULL, NULL);

    if(s) {
      size_t bufSize = (ss.rate * durationMs) / 1000; 
      int16_t *samples = (int16_t *)malloc(bufSize * sizeof(int16_t));
      int errCode;
      uint64_t p = 0;

      for(unsigned int i = 0; i < bufSize; ++i) {
        samples[i] = p < ((uint64_t)ss.rate * 500) ? 10362 : -10362;
        p += (uint64_t)frequency;
        p %= (uint64_t)ss.rate * 1000;
      }

      pa_simple_write(s, samples, bufSize * sizeof(int16_t), &errCode);
      pa_simple_drain(s, &errCode);
      free(samples);
      pa_simple_free(s);
    }
  }
#endif // WITH_PULSEAUDIO

void audioTone(uint32_t frequency) {
  #if defined(__MINGW64__)
    char filename[32];
    sprintf(filename, "res\\tone\\tone%" PRIu32 ".wav", frequency);
    PlaySoundA(filename, NULL, SND_FILENAME | SND_SYNC | SND_NODEFAULT);
  #elif defined(WITH_PULSEAUDIO)
    _audioPulse(frequency, 250);
  #endif // WITH_PULSEAUDIO
}



void audioShutter(void) {
  #if defined(WITH_PULSEAUDIO)
    _audioPulse(100000, 5);
    timeSleep(100);
    _audioPulse(400000, 5);
  #endif // WITH_PULSEAUDIO
}
