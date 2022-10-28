// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "hal/system.h"

#include "charString.h"
#include "error.h"
#include "fonts.h"
#include "hal/timer.h"
#include "ui/keyboard.h"
#include <stdio.h>
#include <dmcp.h>

#include "wp43.h"

void systemScreenshot(void) {
  uint16_t currentVolumeSetting, savedVolumeSetting; // used for beep signaling screen shot
  currentVolumeSetting = get_beep_volume();
  savedVolumeSetting = currentVolumeSetting;
  while(currentVolumeSetting < 11) {
    beep_volume_up();
    currentVolumeSetting = get_beep_volume();
  }

  start_buzzer_freq(100000); //Click before screen dump
  sys_delay(5);
  stop_buzzer();

  xcopy(tmpString, aimBuffer, ERROR_MESSAGE_LENGTH + AIM_BUFFER_LENGTH + NIM_BUFFER_LENGTH);       //backup portion of the "message buffer" area in DMCP used by ERROR..AIM..NIM buffers, to the tmpstring area in DMCP. DMCP uses this area during create_screenshot.
  create_screenshot(0);      //Screen dump
  xcopy(aimBuffer,tmpString, ERROR_MESSAGE_LENGTH + AIM_BUFFER_LENGTH + NIM_BUFFER_LENGTH);        //   This total area must be less than the tmpString storage area, which it is.

  start_buzzer_freq(400000); //Click after screen dump
  sys_delay(5);
  stop_buzzer();

  while(currentVolumeSetting != savedVolumeSetting) { //Restore volume
    beep_volume_down();
    currentVolumeSetting = get_beep_volume();
  }
}



const char *systemMaker(void) {
  return "Hardware" STD_SPACE_3_PER_EM "by" STD_SPACE_3_PER_EM "SwissMicros";
}



void systemProcessEvents(void) {
  dmcpCheckPowerStatus();
  dmcpResetAutoOff();
  while(!key_empty()) {
    int key = key_pop();
    key = convertKeyCode(key);
    if(key > 0) {
      wait_for_key_release(0);
      key_pop();
      btnClicked(key);
    }
  }
  timerRun();
}



void systemQuit(void) {
}
