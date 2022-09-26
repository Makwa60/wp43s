/* This file is part of 43S.
 *
 * 43S is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * 43S is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 43S.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "hal/system.h"

#include "charString.h"
#include "wp43.h"
#include <stdio.h>
#include <dmcp.h>

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
