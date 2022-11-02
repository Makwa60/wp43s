// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "hal/time.h"

#include "wp43-dmcp.h"
#include <assert.h>
#include <stdio.h>
#include <dmcp.h>

static tm_t _timeInfo;
static dt_t _dateInfo;
static uint32_t _timeUptime;

void timeGetTimeInfo(timeInfo_t *ti) {
  assert(ti != NULL);
  ti->hour = _timeInfo.hour;
  ti->min  = _timeInfo.min;
  ti->sec  = _timeInfo.sec;
  ti->csec = _timeInfo.csec;
}



void timeGetDateInfo(dateInfo_t *di) {
  assert(di != NULL);
  di->year  = _dateInfo.year;
  di->month = _dateInfo.month;
  di->day   = _dateInfo.day;
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
  return _timeUptime;
}



uint32_t timeUptimeMs(void) {
  return _timeUptime;
}



void timeSleep(uint32_t timeInMs) {
  sys_delay(timeInMs);
}



void timeCapture(void) {
  rtc_read(&_timeInfo, &_dateInfo);
  _timeUptime = (uint32_t)_timeInfo.hour * 3600000u +
                (uint32_t)_timeInfo.min * 60000u +
                (uint32_t)_timeInfo.sec * 1000u +
                (uint32_t)_timeInfo.csec * 10u;
}
