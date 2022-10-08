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

#include "timer.h"

#include "defines.h"
#include "hal/time.h"
#include <stdio.h>

#include "wp43.h"

typedef struct {
  void          (*func)(uint16_t); ///< Function called to execute the timer
  uint16_t      param;             ///< 1st parameter to the above
  uint32_t      timer_will_expire; ///<
  timerStatus_t state;             ///<
} kb_timer_t;

static kb_timer_t  timer[MAX_TIMER_ID];
static uint32_t    timerLastCalled;
#if defined(DMCP_BUILD)
  static bool_t      mutexRefreshTimer = false;
#endif // DMCP_BUILD



static void _rebuildTimerRefresh(void) {
  #if defined(DMCP_BUILD)
    uint32_t next;

    if(mutexRefreshTimer == false) {
      nextTimerRefresh = 0;
      for(int i = 0; i < MAX_TIMER_ID; i++) {
        if(timer[i].state == tsRunning) {
          next = timer[i].timer_will_expire;
          if(nextTimerRefresh != 0 && next < nextTimerRefresh) {
            nextTimerRefresh = next;
          }
          if(nextTimerRefresh == 0) {
            nextTimerRefresh = next;
          }
        }
      }
    }
  #endif // DMCP_BUILD
}



// This function is called every 5 ms by a GTK timer, or when
// nextTimerRefresh has elapsed (on DMCP)
void timerRefresh(void) {
  uint32_t now = timeUptimeMs();

  if(now < timerLastCalled) {
    for(int i = 0; i < MAX_TIMER_ID; i++) {
      if(timer[i].state == tsRunning) {
        timer[i].state = tsCompleted;
        #if defined(DMCP_BUILD)
          mutexRefreshTimer = true;
        #endif // DMCP_BUILD
        timer[i].func(timer[i].param);      // Callback to configured function
        #if defined(DMCP_BUILD)
          mutexRefreshTimer = false;
        #endif // DMCP_BUILD
      }
    }
  }
  else {
    for(int i = 0; i < MAX_TIMER_ID; i++) {
      if(timer[i].state == tsRunning) {
        if(timer[i].timer_will_expire <= now) {
          timer[i].state = tsCompleted;
          #if defined(DMCP_BUILD)
            mutexRefreshTimer = true;
          #endif // DMCP_BUILD
          timer[i].func(timer[i].param);    // Callback to configured function
          #if defined(DMCP_BUILD)
            mutexRefreshTimer = false;
          #endif // DMCP_BUILD
        }
      }
    }
  }

  timerLastCalled = now;

  #if defined(DMCP_BUILD)
    _rebuildTimerRefresh();
  #endif // DMCP_BUILD
}



void timerDummyTest(uint16_t param) {
#if defined(PC_BUILD) || defined(TESTSUITE_BUILD)
  printf("timerDummyTest called  %u\n", param);
#endif // PC_BUILD || TESTSUITE_BUILD
}



void timerReset(void) {
  #if !defined(TESTSUITE_BUILD)
    timerLastCalled = 0;
  #endif // !TESTSUITE_BUILD

  for(int i = 0; i < MAX_TIMER_ID; i++) {
    timer[i].state = tsUnused;
    timer[i].func = timerDummyTest;
    timer[i].param = 0;
  }

  #if defined(DMCP_BUILD)
    mutexRefreshTimer = false;
  #endif
  _rebuildTimerRefresh();
}



void timerConfig(timerId_t nr, void(*func)(uint16_t), uint16_t param) {
  if(nr < MAX_TIMER_ID) {
    timer[nr].func  = func;
    timer[nr].param = param;
    timer[nr].state = tsStopped;
  }
  _rebuildTimerRefresh();
}



void timerStart(timerId_t nr, uint16_t param, uint32_t time) {
  uint32_t now = timeUptimeMs();

  if(nr < MAX_TIMER_ID) {
    timer[nr].param = param;
    timer[nr].timer_will_expire = (uint32_t)(now + time);
    #if defined(PC_BUILD)
      // The type has change so it can no longer be less than zero
      // Is this needed now, or should it be written differently
      if(timer[nr].timer_will_expire < 0) {
        timer[nr].timer_will_expire = time;
      }
    #endif // PC_BUILD
    timer[nr].state = tsRunning;
  }
  _rebuildTimerRefresh();
}



void timerStop(timerId_t nr) {
  if(nr < MAX_TIMER_ID && timer[nr].state != tsUnused) {
    timer[nr].state = tsStopped;
  }
  _rebuildTimerRefresh();
}


void timerExec(timerId_t nr) {
  if(nr < MAX_TIMER_ID && timer[nr].state == tsRunning) {
    timer[nr].state = tsCompleted;
    timer[nr].func(timer[nr].param);        // Callback to configured function
  }
}



void timerDel(timerId_t nr) {
  if(nr < MAX_TIMER_ID) {
    timer[nr].state = tsUnused;
  }
  _rebuildTimerRefresh();
}



uint16_t timerGetParam(timerId_t nr) {
  uint16_t result = 0;

  if(nr < MAX_TIMER_ID) {
    result = timer[nr].param;
  }

  return result;
}



timerStatus_t timerGetStatus(timerId_t nr) {
  timerStatus_t result = tsUnused;

  if(nr < MAX_TIMER_ID) {
    result = timer[nr].state;
  }

  return result;
}
