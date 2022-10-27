// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "hal/time.h"

#include <assert.h>
#include <stdio.h>
#include <dmcp.h>

void timeGetTimeInfo(timeInfo_t *ti) {
  assert(ti != NULL);
  tm_t timeInfo;
  dt_t dateInfo;
  rtc_read(&timeInfo, &dateInfo);
  ti->hour = timeInfo.hour;
  ti->min  = timeInfo.min;
  ti->sec  = timeInfo.sec;
  ti->csec = timeInfo.csec;
}



void timeGetDateInfo(dateInfo_t *di) {
  assert(di != NULL);
  tm_t timeInfo;
  dt_t dateInfo;
  rtc_read(&timeInfo, &dateInfo);
  di->year  = dateInfo.year;
  di->month = dateInfo.month;
  di->day   = dateInfo.day;
}



void timeSetTimeInfo(timeInfo_t *ti) {
  assert(ti != NULL);
  tm_t timeInfo;
  dt_t dateInfo;
  rtc_read(&timeInfo, &dateInfo);
  timeInfo.hour = ti->hour;
  timeInfo.min  = ti->min;
  timeInfo.sec  = ti->sec;
  timeInfo.csec = ti->csec;
  rtc_write(&timeInfo, &dateInfo);
}



void timeSetDateInfo(dateInfo_t *di) {
  assert(di != NULL);
  tm_t timeInfo;
  dt_t dateInfo;
  rtc_read(&timeInfo, &dateInfo);
  dateInfo.year  = di->year;
  dateInfo.month = di->month;
  dateInfo.day   = di->day;
  rtc_write(&timeInfo, &dateInfo);
}



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



void timeSleep(uint32_t timeInMs) {
  sys_delay(timeInMs);
}
