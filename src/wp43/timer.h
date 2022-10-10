// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

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
