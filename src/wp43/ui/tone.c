// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "ui/tone.h"

#include "flags.h"
#include "hal/audio.h"
#include "ui/screen.h"
#include <stdio.h>

#include "wp43.h"

TO_QSPI uint32_t frequency[10] = {164814, 220000, 246942, 277183, 293665, 329628, 369995, 415305, 440000, 554365};

static void _tonePlay(uint16_t toneNum) {
  if(getSystemFlag(FLAG_QUIET)) {
    return;
  }
  if(toneNum < 10) {
    audioTone(frequency[toneNum]);
  }
}



void fnTone(uint16_t toneNum) {
  _tonePlay(toneNum);
}



void fnBeep(uint16_t unusedButMandatoryParameter) {
  _tonePlay(8);
  _tonePlay(5);
  _tonePlay(9);
  _tonePlay(8);
}
