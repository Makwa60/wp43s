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

#include "hal/time.h"

#include <stdio.h>
#include <dmcp.h>

uint32_t timeCurrentMs(void) {
  tm_t timeInfo;
  dt_t dateInfo;

  rtc_read(&timeInfo, &dateInfo);
  return (uint32_t)timeInfo.hour * 3600000u +
         (uint32_t)timeInfo.min * 60000u +
         (uint32_t)timeInfo.sec * 1000u +
         (uint32_t)timeInfo.csec * 10u;
}



uint32_t timeUptimeMs(void) {
  return (uint32_t)sys_current_ms();
}
