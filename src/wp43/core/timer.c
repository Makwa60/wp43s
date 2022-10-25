// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "core/timer.h"

#include "defines.h"
#include "hal/time.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "wp43.h"

typedef struct {
  timerCallback_t func;
  uint16_t        param;
  uint32_t        expire;
  timerStatus_t   state;
} timerState_t;

static timerState_t _timer[MAX_TIMER_ID];

// Assume that the expiry is in the future and calculate the difference
// If the difference is large (> UINT32_MAX/2) then the timer has expired
static uint32_t _timerDiff(uint32_t currTime, uint32_t expire) {
  if(expire < currTime) {
    return (UINT32_MAX - currTime) + expire + 1;
  }
  return expire - currTime;
}



uint32_t timerRun(void) {
  uint32_t currTime = timeUptimeMs();
  bool anyRemaining = false;
  uint32_t timeUntilNextRun = UINT32_MAX;

  for(int i = 0; i < MAX_TIMER_ID; i++) {
    if(_timer[i].state == tsRunning) {
      uint32_t diff = _timerDiff(currTime, _timer[i].expire);
      if(diff > (UINT32_MAX / 2) || diff == 0) {
        _timer[i].state = tsCompleted;
        _timer[i].func(_timer[i].param);
        if(_timer[i].state == tsRunning) {
          // The callback restarted the timer, so recompute the difference
          diff = _timerDiff(currTime, _timer[i].expire);
        }
      }
      if(_timer[i].state == tsRunning && diff < timeUntilNextRun) {
        anyRemaining = true;
        timeUntilNextRun = diff;
      }
    }
  }

  if(!anyRemaining) {
    anyRemaining = false;
  }
  return anyRemaining ? timeUntilNextRun : 0;
}



void timerDummyTest(uint16_t param) {
}



void timerReset(void) {
  for(int i = 0; i < MAX_TIMER_ID; i++) {
    _timer[i].state = tsUnused;
  }
}



void timerConfig(timerId_t nr, timerCallback_t func) {
  assert(nr < MAX_TIMER_ID);

  _timer[nr].func  = func;
  _timer[nr].state = tsStopped;
}



void timerStart(timerId_t nr, uint16_t param, uint32_t time) {
  assert(nr < MAX_TIMER_ID);
  assert(_timer[nr].state != tsUnused);

  _timer[nr].param  = param;
  _timer[nr].expire = timeUptimeMs() + time;
  _timer[nr].state  = tsRunning;
}



void timerStop(timerId_t nr) {
  assert(nr < MAX_TIMER_ID);
  assert(_timer[nr].state != tsUnused);

  _timer[nr].state = tsStopped;
}



timerStatus_t timerGetStatus(timerId_t nr) {
  assert(nr < MAX_TIMER_ID);

  return _timer[nr].state;
}
