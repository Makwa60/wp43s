// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file hal/audio.h
 */
#if !defined(AUDIO_H)
  #define AUDIO_H

  #include <stdint.h>

  /**
   * Plays a tone.
   * Each hardware platform that supports playing audio should implement
   * this method to play a short tone of a given frequency. This is the
   * only supported audio playback. Configuration for a "silent mode" is
   * not covered by this function and should be checked by the caller.
   *
   * \param[in] frequency the frequency of the note to play in mHz
   */
  void audioTone(uint32_t frequency);

#endif // !AUDIO_H
