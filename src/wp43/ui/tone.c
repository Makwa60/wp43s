// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "ui/tone.h"

#include "flags.h"
#include "hal/audio.h"
#include "ui/screen.h"
#include <assert.h>
#include <stdio.h>

#include "wp43.h"

TO_QSPI uint32_t frequency[10] = {164814, 220000, 246942, 277183, 293665, 329628, 369995, 415305, 440000, 554365};

void fnTone(uint16_t toneNum) {
  assert(toneNum < 10);
  if(!getSystemFlag(FLAG_QUIET)) {
    audioTone(frequency[toneNum]);
  }
}



void fnBeep(uint16_t unusedButMandatoryParameter) {
  fnTone(8);
  fnTone(5);
  fnTone(9);
  fnTone(8);
}
