// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file core/timer.h
 */
#if !defined(TIMER_H)
  #define TIMER_H

  #include <stdint.h>

  typedef enum {
    tidAutoRepeat               = 0,
    tidTimerAppRedraw           = 1,
    tidTimerAppDetectWrapAround = 2,
    tidShowNop                  = 3,
    tidRefreshLcd               = 4,
    tidAsmActive                = 5,
    tidCursorBlink              = 6,
    tidTimeUpdate               = 7
  } timerId_t;
  #define MAX_TIMER_ID            8

  typedef enum {
    tsUnused    = 0,
    tsStopped   = 1,
    tsRunning   = 2,
    tsCompleted = 3
  } timerStatus_t;

  typedef void (*timerCallback_t)(uint16_t param);

  void          timerDummyTest (uint16_t param);

  uint32_t      timerRun       (void);
  void          timerReset     (void);
  void          timerConfig    (timerId_t nr, timerCallback_t func);
  void          timerStart     (timerId_t nr, uint16_t param, uint32_t time);      // Start Timer, 0..n-1
  void          timerStop      (timerId_t nr);                                     // Stop Timer, 0..n-1
  timerStatus_t timerGetStatus (timerId_t nr);

#endif // !TIMER_H
