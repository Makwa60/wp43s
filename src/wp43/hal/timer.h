// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file hal/timer.h
 */
#if !defined(TIMER_H)
  #define TIMER_H

  #include <stdbool.h>
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

  typedef void (*timerCallback_t)(uint16_t param);

  uint32_t timerRun       (void);

  /**
   * Cancel all running timers.
   * Timers need to be restarted after this call, and should also be
   * configured with ::timerConfig.
   */
  void     timerReset     (void);

  /**
   * Configure the callback function for a given timer.
   * This function will only be called once when the timer has expired.
   * If it should be called repeatedly then the callback function should
   * call ::timerStart again. ::timerConfig must be called before
   * ::timerStart is called.
   *
   * \param[in] nr timer ID
   * \param[in] func callback function called when a timer expires
   */
  void     timerConfig    (timerId_t nr, timerCallback_t func);

  /**
   * Set an expiry on a given timer.
   * The callback function configured in ::timerConfig will be called when
   * the specified time has elapsed. It will only be called once, not
   * repeatedly. If it is required to repeat then the callback function
   * configured by ::timerConfig should call ::timerStart again.
   *
   * \param[in] nr timer ID
   * \param[in] param parameter that is passed to the callback function
   * \param[in] time time is milliseconds until the timer expires
   */
  void     timerStart     (timerId_t nr, uint16_t param, uint32_t time);

  /**
   * Stop a timer.
   * This will stop a running timer causing it to not fire. It is safe
   * to call on a stopped timer, but will have no effect. The timer must
   * be configured with ::timerConfig before ::timerStop is called though.
   *
   * \param[in] nr timer ID
   */
  void     timerStop      (timerId_t nr);

  /**
   * Query whether a timer is running.
   * Returns whether the given timer is running, that is that it has
   * been started with ::timerStart, not stopped with ::timerStop, and
   * it has not yet reached the expiry time.
   *
   * \param[in] nr timer ID
   * \return true if the timer is still running
   */
  bool     timerIsRunning (timerId_t nr);

#endif // !TIMER_H
