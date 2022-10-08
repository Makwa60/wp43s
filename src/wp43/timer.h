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

/**
 * \file timer.h
 */
#if !defined(TIMER_H)
  #define TIMER_H

  #include <stdint.h>

  typedef enum {
    tidAutoRepeat               = 0,
    tidTimerAppRedraw           = 1,
    tidTimerAppDetectWrapAround = 2,
    tidKeyboardActive           = 3,
    tidShowNop                  = 4
  } timerId_t;
  #define MAX_TIMER_ID            5

  typedef enum {
    tsUnused    = 0,
    tsStopped   = 1,
    tsRunning   = 2,
    tsCompleted = 3
  } timerStatus_t;

  void          timerRefresh   (void);
  void          timerReset     (void);
  void          timerDummyTest (uint16_t param);
  void          timerConfig    (timerId_t nr, void(*func)(uint16_t), uint16_t param);
  void          timerStart     (timerId_t nr, uint16_t param, uint32_t time);      // Start Timer, 0..n-1
  void          timerStop      (timerId_t nr);                                     // Stop Timer, 0..n-1
  void          timerExec      (timerId_t nr);                                     // Execute Timer, 0..n-1
  void          timerDel       (timerId_t nr);                                     // Delete Timer, 0..n-1
  uint16_t      timerGetParam  (timerId_t nr);
  timerStatus_t timerGetStatus (timerId_t nr);

#endif // !TIMER_H
