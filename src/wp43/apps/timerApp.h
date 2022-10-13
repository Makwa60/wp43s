// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file apps/timerApp.h
 */
#if !defined(TIMERAPP_H)
  #define TIMERAPP_H

  #include "typeDefinitions.h"
  #include <stdbool.h>
  #include <stdint.h>

  typedef struct {
    uint32_t       startUptime     : 32;
    uint32_t       lapTime         : 32;
    uint32_t       totalTime       : 32;
    bool           showDeciseconds :  1;
    bool           isFirstDigit    :  1;
    bool           started         :  1;
    uint8_t        firstDigit      :  5;
    calcRegister_t currentRegister :  8;
  } timerAppState_t;

  extern timerAppState_t timerAppState;

  void cbTimerAppRedraw           (uint16_t unusedButMandatoryParameter);
  void cbTimerAppDetectWrapAround (uint16_t unusedButMandatoryParameter);

  void timerAppResetState         (void);

  void fnTimerApp                 (uint16_t unusedButMandatoryParameter);
  void fnAddTimerApp              (uint16_t unusedButMandatoryParameter);
  void fnDecisecondTimerApp       (uint16_t unusedButMandatoryParameter);
  void fnResetTimerApp            (uint16_t unusedButMandatoryParameter);
  void fnRecallTimerApp           (uint16_t regist);

  void timerAppStartStop          (void);
  void timerAppStop               (void);
  void timerAppDraw               (void);
  void timerAppLeave              (void);

  void timerAppEnter              (void);
  void timerAppDot                (void);
  void timerAppUp                 (void);
  void timerAppDown               (void);
  void timerAppDigitKey           (uint16_t digit);
  void timerAppPlus               (void);
  void timerAppBackspace          (void);

#endif // !TIMERAPP_H
