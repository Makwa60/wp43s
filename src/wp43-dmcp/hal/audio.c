// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "hal/audio.h"

#include <stdio.h>
#include <dmcp.h> 

void audioTone(uint32_t frequency) {
  start_buzzer_freq(frequency);
  sys_delay(250);
  stop_buzzer();
}
