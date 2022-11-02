// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "hal/time.h"

#include <assert.h>
#include <glib.h>
#include <time.h>
#include <unistd.h>

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



void timeSleep(uint32_t timeInMs) {
  usleep(timeInMs * 1000);
}
