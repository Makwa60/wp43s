// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "hal/time.h"

#include <assert.h>
#include <time.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

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
  return 0;
}



uint32_t timeUptimeMs(void) {
  return 0;
}



void timeSleep(uint32_t param) {
}
