// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "hal/system.h"

#include "charString.h"
#include "error.h"
#include "fonts.h"
#include "hal/time.h"
#include "hal/timer.h"
#include "ui/keyboard.h"
#include "wp43-dmcp.h"
#include <stdio.h>
#include <dmcp.h>

#include "wp43.h"

void systemScreenshot(void) {
  xcopy(tmpString, aimBuffer, ERROR_MESSAGE_LENGTH + AIM_BUFFER_LENGTH + NIM_BUFFER_LENGTH);       //backup portion of the "message buffer" area in DMCP used by ERROR..AIM..NIM buffers, to the tmpstring area in DMCP. DMCP uses this area during create_screenshot.
  create_screenshot(0);      //Screen dump
  xcopy(aimBuffer,tmpString, ERROR_MESSAGE_LENGTH + AIM_BUFFER_LENGTH + NIM_BUFFER_LENGTH);        //   This total area must be less than the tmpString storage area, which it is.
}



const char *systemMaker(void) {
  return "Hardware" STD_SPACE_3_PER_EM "by" STD_SPACE_3_PER_EM "SwissMicros";
}



void systemProcessEvents(void) {
  timeUptime = timeCurrentMs();
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
  SET_ST(STAT_PGM_END);
}
