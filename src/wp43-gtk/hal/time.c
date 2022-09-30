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

#include <assert.h>
#include <glib.h>
#include <time.h>

void timeGetTimeInfo(timeInfo_t *ti) {
  assert(ti != NULL);
  time_t epoch = time(NULL);
  struct tm *timeInfo = localtime(&epoch);
  ti->hour = timeInfo->tm_hour;
  ti->min  = timeInfo->tm_min;
  ti->sec  = timeInfo->tm_sec;
  ti->csec = 0;
}



void timeGetDateInfo(dateInfo_t *di) {
  assert(di != NULL);
  time_t epoch = time(NULL);
  struct tm *timeInfo = localtime(&epoch);
  di->year  = timeInfo->tm_year + 1900;
  di->month = timeInfo->tm_mon + 1;
  di->day   = timeInfo->tm_mday;
}



uint32_t timeCurrentMs(void) {
  return (uint32_t)(g_get_real_time() % 86400000000uLL / 1000uLL);
}



uint32_t timeUptimeMs(void) {
  return (uint32_t)(g_get_monotonic_time() / 1000);
}
