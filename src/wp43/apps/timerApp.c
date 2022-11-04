// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "apps/timerApp.h"

#include "calcMode.h"
#include "charString.h"
#include "constantPointers.h"
#include "debug.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "hal/gui.h"
#include "hal/time.h"
#include "hal/timer.h"
#include "items.h"
#include "realType.h"
#include "registers.h"
#include "registerValueConversions.h"
#include "stack.h"
#include "stats.h"
#include "ui/cursor.h"
#include "ui/screen.h"
#include "ui/softmenus.h"
#include <assert.h>
#include <stdbool.h>

#include "wp43.h"

#define TIMER_APP_REDRAW_PERIOD             100   // in milliseconds
#define TIMER_APP_DETECT_WRAP_AROUND_PERIOD 60000 // in milliseconds

timerAppState_t timerAppState = {0, 0, 0, true, true, false, 0, 0};

static void _timerAppUpdate(void);

void cbTimerAppRedraw(uint16_t unusedButMandatoryParameter) {
  timerStart(tidTimerAppRedraw, NOPARAM, TIMER_APP_REDRAW_PERIOD);
  _timerAppUpdate();
}



void timerAppResetState(void) {
  timerAppState.startUptime     = 0;
  timerAppState.lapTime         = 0;
  timerAppState.totalTime       = 0;
  timerAppState.isFirstDigit    = true;
  timerAppState.showDeciseconds = true;
  timerAppState.currentRegister = 0;
}



#if !defined(TESTSUITE_BUILD)
  static void _timerAppShowCursor(void) {
    const char *str;
    uint32_t length = 1;
    if(timerAppState.totalTime > 0) {
      str = STD_SPACE_FIGURE STD_SPACE_FIGURE ":" STD_SPACE_FIGURE STD_SPACE_FIGURE ":" STD_SPACE_FIGURE STD_SPACE_FIGURE STD_SUP_T STD_SPACE_PUNCTUATION STD_SPACE_FIGURE "  ";
      length += stringWidth(str, &numericFont, true, true);
    }

    str = STD_SPACE_FIGURE STD_SPACE_FIGURE ":" STD_SPACE_FIGURE STD_SPACE_FIGURE ":" STD_SPACE_FIGURE STD_SPACE_FIGURE STD_SPACE_PUNCTUATION STD_SPACE_FIGURE " [" STD_SPACE_FIGURE;
    length += stringWidth(str, &numericFont, true, true);

    _timerAppUpdate();
    cursorShow(false, length, Y_POSITION_OF_REGISTER_T_LINE);
  }



  static uint32_t _getTimerIncrement(uint32_t currTime) {
    if(currTime < timerAppState.startUptime) {
      return (UINT32_MAX - timerAppState.startUptime) + currTime + 1;
    }
    return currTime - timerAppState.startUptime;
  }



  static uint32_t _getTimerValue(void) {
    if(timerAppState.started) {
      return _getTimerIncrement(timeUptimeMs()) + timerAppState.lapTime;
    }
    return timerAppState.lapTime;
  }



  static void _timerAppResetExceptTotal(void) {
    if(timerAppState.started) {
      timerAppState.startUptime = timeUptimeMs();
    }
    timerAppState.lapTime   = 0;
    timerAppState.isFirstDigit = true;
    cursorHide();
  }



  void fnTimerApp(uint16_t unusedButMandatoryParameter) {
    timerAppState.isFirstDigit = true;
    cursorHide();
    watchIconEnabled = false;
    if(timerAppState.started) {
      timerStart(tidTimerAppRedraw, NOPARAM, TIMER_APP_REDRAW_PERIOD);
    }
    calcModeEnter(cmTimerApp);
    showSoftmenu(-MNU_TIMERF);
  }



  void fnDecisecondTimerApp(uint16_t unusedButMandatoryParameter) {
    timerAppState.showDeciseconds = !timerAppState.showDeciseconds;
  }



  void fnResetTimerApp(uint16_t unusedButMandatoryParameter) {
    _timerAppResetExceptTotal();
    timerAppState.totalTime = 0;
  }



  void timerAppStartStop(void) {
    if(timerAppState.started) {
      timerAppStop();
    }
    else {
      setSystemFlag(FLAG_RUNTIM);
      timerAppState.startUptime = timeUptimeMs();
      timerAppState.started     = true;
      timerStart(tidTimerAppRedraw,           NOPARAM, TIMER_APP_REDRAW_PERIOD);
      timerStart(tidTimerAppDetectWrapAround, NOPARAM, TIMER_APP_DETECT_WRAP_AROUND_PERIOD);
      _timerAppUpdate();
    }
    timerAppState.isFirstDigit = true;
    cursorHide();
  }



  void timerAppStop(void) {
    if(timerAppState.started) {
      clearSystemFlag(FLAG_RUNTIM);
      watchIconEnabled = false;
      const uint32_t timeSinceLastLapUpdate = _getTimerValue() - timerAppState.lapTime;
      timerAppState.lapTime += timeSinceLastLapUpdate;
      if(timerAppState.totalTime > 0) {
        timerAppState.totalTime += timeSinceLastLapUpdate;
      }
      timerAppState.started = false;
      timerStop(tidTimerAppRedraw);
      timerStop(tidTimerAppDetectWrapAround);
      _timerAppUpdate();
    }
  }



  void timerAppDraw(void) {
    assert(calcMode == cmTimerApp);

    const uint32_t msec = _getTimerValue();
    clearRegisterLine(REGISTER_T, true, true);

    tmpString[0] = 0;

    if(timerAppState.totalTime > 0) {
      const uint32_t tmsec = (msec - timerAppState.lapTime) + timerAppState.totalTime;
      if(timerAppState.showDeciseconds) {
        sprintf(tmpString, "%2" PRIu32 ":%02" PRIu32 ":%02" PRIu32 ".%" PRIu32 STD_SUP_T "  ", tmsec / 3600000u, tmsec % 3600000u / 60000u, tmsec % 60000u / 1000u, tmsec % 1000u / 100u);
      }
      else {
        sprintf(tmpString, "%2" PRIu32 ":%02" PRIu32 ":%02" PRIu32 STD_SUP_T STD_SPACE_PUNCTUATION STD_SPACE_FIGURE "  ", tmsec / 3600000u, tmsec % 3600000u / 60000u, tmsec % 60000u / 1000u);
      }
    }

    if(timerAppState.showDeciseconds) {
      sprintf(tmpString + stringByteLength(tmpString), "%2" PRIu32 ":%02" PRIu32 ":%02" PRIu32 ".%" PRIu32 " ", msec / 3600000u, msec % 3600000u / 60000u, msec % 60000u / 1000u, msec % 1000u / 100u);
    }
    else {
      sprintf(tmpString + stringByteLength(tmpString), "%2" PRIu32 ":%02" PRIu32 ":%02" PRIu32 STD_SPACE_PUNCTUATION STD_SPACE_FIGURE " ", msec / 3600000u, msec % 3600000u / 60000u, msec % 60000u / 1000u);
    }

    if(timerAppState.isFirstDigit) {
      sprintf(tmpString + stringByteLength(tmpString), "[%02" PRIu16 "]", timerAppState.currentRegister);
    }
    else {
      const char *close = "]";
      sprintf(tmpString + stringByteLength(tmpString), "[%" PRIu8, timerAppState.firstDigit);
      uint32_t closeStart = 1 + stringWidth(tmpString, &numericFont, true, true) + stringWidth(STD_CURSOR, &numericFont, true, true);
      showString(close, &numericFont, closeStart, Y_POSITION_OF_REGISTER_T_LINE, vmNormal, true, true);
      cursorDraw();
    }
    showString(tmpString, &numericFont, 1, Y_POSITION_OF_REGISTER_T_LINE, vmNormal, true, true);
  }



  void _timerAppUpdate(void) {
    if(calcMode == cmTimerApp) {
      timerAppDraw();
      displayShiftAndTamBuffer();
    }
  }



  static void _timerAppGetLapTimeReal(real_t *lapTimeReal) {
    uInt32ToReal(_getTimerValue() / 100u, lapTimeReal);
    lapTimeReal->exponent -= 1;
  }



  void fnAddTimerApp(uint16_t unusedButMandatoryParameter) {
    real_t lapTimeReal, numberOfSamplePoints;
    _timerAppGetLapTimeReal(&lapTimeReal);
    realDivide(&lapTimeReal, const_3600, &lapTimeReal, &ctxtReal39);

    real_t *statSumSamplePoints = statSum(0);
    if(statSumSamplePoints != NULL) {
      realCopy(statSumSamplePoints, &numberOfSamplePoints);
    }
    else {
      realZero(&numberOfSamplePoints);
    }
    realAdd(&numberOfSamplePoints, const_1, &numberOfSamplePoints, &ctxtReal75);
    sigmaPlus(true, &numberOfSamplePoints, &lapTimeReal);
  }



  void timerAppEnter(void) {
    if(timerAppState.isFirstDigit) {
      real_t lapTimeReal;
      _timerAppGetLapTimeReal(&lapTimeReal);
      reallocateRegister(timerAppState.currentRegister, dtTime, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
      realToReal34(&lapTimeReal, REGISTER_REAL34_DATA(timerAppState.currentRegister));
      timerAppUp();
    }
    else {
      timerAppState.isFirstDigit    = true;
      timerAppState.currentRegister = timerAppState.firstDigit;
      cursorHide();
    }
  }



  void timerAppDot(void) {
    if(timerAppState.isFirstDigit) {
      timerAppEnter();

      const uint32_t msec = _getTimerValue();
      if(timerAppState.totalTime > 0) {
        timerAppState.totalTime += msec - timerAppState.lapTime;
      }
      else {
        timerAppState.totalTime = msec;
        _timerAppShowCursor();
      }
      _timerAppResetExceptTotal();
    }
  }



  void timerAppPlus(void) {
    if(timerAppState.isFirstDigit) {
      fnAddTimerApp(NOPARAM);
      timerAppDot();
    }
  }



  void timerAppUp(void) {
    if(timerAppState.currentRegister >= 99) {
      timerAppState.currentRegister = 0;
    }
    else {
      timerAppState.currentRegister++;
    }
    timerAppState.isFirstDigit = true;
    cursorHide();
  }



  void timerAppDown(void) {
    if(timerAppState.currentRegister == 0) {
      timerAppState.currentRegister = 99;
    }
    else {
      timerAppState.currentRegister--;
    }
    timerAppState.isFirstDigit = true;
    cursorHide();
  }



  void timerAppDigitKey(uint16_t digit) {
    if(timerAppState.isFirstDigit) {
      timerAppState.isFirstDigit = false;
      timerAppState.firstDigit   = digit;
      _timerAppShowCursor();
    }
    else {
      timerAppState.isFirstDigit = true;
      timerAppState.currentRegister = timerAppState.firstDigit * 10u + digit;
      cursorHide();
    }
  }



  void fnRecallTimerApp(uint16_t regist) {
    real_t regValueReal;
    bool   overflow;
    uint32_t regValueUInt32;

    switch(getRegisterDataType(regist)) {
      case dtTime: {
        real34ToReal(REGISTER_REAL34_DATA(regist), &regValueReal);
        regValueReal.exponent += 3;
        realToUInt32(&regValueReal, DEC_ROUND_DOWN, &regValueUInt32, &overflow);
        break;
      }
      case dtReal34: {
        real34ToReal(REGISTER_REAL34_DATA(regist), &regValueReal);
        realMultiply(&regValueReal, const_3600, &regValueReal, &ctxtReal39);
        regValueReal.exponent += 3;
        realToUInt32(&regValueReal, DEC_ROUND_HALF_EVEN, &regValueUInt32, &overflow);
        break;
      }
      case dtLongInteger: {
        convertLongIntegerRegisterToReal(regist, &regValueReal, &ctxtReal39);
        realMultiply(&regValueReal, const_3600, &regValueReal, &ctxtReal39);
        regValueReal.exponent += 3;
        realToUInt32(&regValueReal, DEC_ROUND_HALF_EVEN, &regValueUInt32, &overflow);
        break;
      }
      default: {
        displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
        #if (EXTRA_INFO_ON_CALC_ERROR == 1)
          sprintf(errorMessage, "cannot recall %s to the stopwatch", getRegisterDataTypeName(regist, true, false));
          moreInfoOnError("In function fnRecallTimerApp:", errorMessage, NULL, NULL);
        #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
        return;
      }
    }

    if(overflow) {
      displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        sprintf(errorMessage, "the %s does not fit to uint32_t", getRegisterDataTypeName(regist, true, false));
        moreInfoOnError("In function fnRecallTimerApp:", errorMessage, NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
    else {
      timerAppState.lapTime += regValueUInt32;
    }
  }



  void timerAppBackspace(void) {
    if(timerAppState.isFirstDigit) {
      fnResetTimerApp(NOPARAM);
    }
    else {
      timerAppState.isFirstDigit = true;
      cursorHide();
    }
  }



  void timerAppLeave(void) {
    popSoftmenu();
    timerAppState.isFirstDigit = true;
    cursorHide();
    calcModeLeave();
    watchIconEnabled = timerAppState.started;
  }



  void cbTimerAppDetectWrapAround(uint16_t unusedButMandatoryParameter) {
    if(timerAppState.started) {
      uint32_t currTime          = timeUptimeMs();
      uint32_t timeIncrement     = _getTimerIncrement(currTime);
      timerAppState.startUptime  = currTime;
      timerAppState.lapTime     += timeIncrement;
      if(timerAppState.totalTime > 0) {
        timerAppState.totalTime += timeIncrement;
      }
      timerStart(tidTimerAppDetectWrapAround, NOPARAM, TIMER_APP_DETECT_WRAP_AROUND_PERIOD);
    }
  }
#else // TESTSUITE_BUILD
  static void _timerAppUpdate     (void) {}
  void cbTimerAppDetectWrapAround (uint16_t unusedButMandatoryParameter) {}

  void fnTimerApp                 (uint16_t unusedButMandatoryParameter) {}
  void fnAddTimerApp              (uint16_t unusedButMandatoryParameter) {}
  void fnDecisecondTimerApp       (uint16_t unusedButMandatoryParameter) {}
  void fnResetTimerApp            (uint16_t unusedButMandatoryParameter) {}
  void fnRecallTimerApp           (uint16_t regist) {}

  void timerAppStartStop          (void) {}
  void timerAppStop               (void) {}
  void timerAppDraw               (void) {}
  void timerAppLeave              (void) {}

  void timerAppEnter              (void) {}
  void timerAppDot                (void) {}
  void timerAppPlus               (void) {}
  void timerAppUp                 (void) {}
  void timerAppDown               (void) {}
  void timerAppDigitKey           (uint16_t digit) {}
  void timerAppBackspace          (void) {}
#endif // !TESTSUITE_BUILD TESTSUITE_BUILD
