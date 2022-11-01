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



void audioShutter(void) {
  start_buzzer_freq(100000);
  sys_delay(5);
  stop_buzzer();
  sys_delay(100);
  start_buzzer_freq(400000);
  sys_delay(5);
  stop_buzzer();
}
