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
#include "items.h"
#include "realType.h"
#include "registers.h"
#include "registerValueConversions.h"
#include "stack.h"
#include "stats.h"
#include "timer.h"
#include "typeDefinitions.h"
#include "ui/screen.h"
#include "ui/softmenus.h"
#include <assert.h>

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
  static uint32_t _getTimerValue(void) {
    if(timerAppState.started) {
      return (timeUptimeMs() - timerAppState.startUptime) + timerAppState.lapTime;
    }
    return timerAppState.lapTime;
  }

  void fnTimerApp(uint16_t unusedButMandatoryParameter) {
    timerAppState.isFirstDigit = true;
    watchIconEnabled = false;
    if(timerAppState.started) {
      timerStart(tidTimerAppRedraw, NOPARAM, TIMER_APP_REDRAW_PERIOD);
    }
    calcModeEnter(cmTimerApp);
    showSoftmenu(-MNU_TIMERF);
    calcModeUpdateGui();
  }

  void fnAddTimerApp(uint16_t unusedButMandatoryParameter) {
    real_t tmp;

    uInt32ToReal(_getTimerValue() / 100u, &tmp);
    tmp.exponent -= 1;
    realDivide(&tmp, const_3600, &tmp, &ctxtReal39);

    fnStatSum(0);
    if(lastErrorCode != ERROR_NONE) {
      liftStack();
      clearRegister(REGISTER_X);
      lastErrorCode = ERROR_NONE;
    }
    real34Add(REGISTER_REAL34_DATA(REGISTER_X), const34_1, REGISTER_REAL34_DATA(REGISTER_X));
    liftStack();
    realToReal34(&tmp, REGISTER_REAL34_DATA(REGISTER_X));
    fnSigma(1);

    refreshScreen();
  }

  void fnDecisecondTimerApp(uint16_t unusedButMandatoryParameter) {
    timerAppState.showDeciseconds = !timerAppState.showDeciseconds;
  }

  void fnResetTimerApp(uint16_t unusedButMandatoryParameter) {
    if(timerAppState.started) {
      timerAppState.startUptime = timeUptimeMs();
    }
    timerAppState.lapTime   = 0;
    timerAppState.totalTime = 0;
    timerAppState.isFirstDigit = true;
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
      sprintf(tmpString + stringByteLength(tmpString), "[%" PRIu8 STD_CURSOR "]", timerAppState.firstDigit);
    }
    showString(tmpString, &numericFont, 1, Y_POSITION_OF_REGISTER_T_LINE, vmNormal, true, true);
  }

  void _timerAppUpdate(void) {
    if(calcMode == cmTimerApp) {
      timerAppDraw();
      displayShiftAndTamBuffer();
      #if defined(DMCP_BUILD)
        refreshLcd();
        lcd_refresh();
      #else // !DMCP_BUILD
        refreshLcd();
      #endif // DMCP_BUILD
    }
  }

  void timerAppEnter(void) {
    if(timerAppState.isFirstDigit) {
      real_t tmp;
      uInt32ToReal(_getTimerValue() / 100u, &tmp);
      tmp.exponent -= 1;
      reallocateRegister(timerAppState.currentRegister, dtTime, REAL34_SIZE, amNone);
      realToReal34(&tmp, REGISTER_REAL34_DATA(timerAppState.currentRegister));
      timerAppUp();
    }
    else {
      timerAppState.isFirstDigit    = true;
      timerAppState.currentRegister = timerAppState.firstDigit;
    }
  }

  void timerAppDot(void) {
    const uint32_t msec = _getTimerValue();
    real_t tmp;

    uInt32ToReal(msec / 100u, &tmp);
    tmp.exponent -= 1;
    reallocateRegister(timerAppState.currentRegister, dtTime, REAL34_SIZE, amNone);
    realToReal34(&tmp, REGISTER_REAL34_DATA(timerAppState.currentRegister));

    timerAppUp();

    if(timerAppState.totalTime > 0) {
      timerAppState.totalTime += msec - timerAppState.lapTime;
    }
    else {
      timerAppState.totalTime = msec;
    }
    timerAppState.lapTime = 0;
    if(timerAppState.started) {
      timerAppState.startUptime = timeUptimeMs();
    }
  }

  void timerAppPlus(void) {
    const uint32_t msec = _getTimerValue();
    real_t tmp;

    uInt32ToReal(msec / 100u, &tmp);
    tmp.exponent -= 1;
    reallocateRegister(timerAppState.currentRegister, dtTime, REAL34_SIZE, amNone);
    realToReal34(&tmp, REGISTER_REAL34_DATA(timerAppState.currentRegister));
    realDivide(&tmp, const_3600, &tmp, &ctxtReal39);

    fnStatSum(0);
    if(lastErrorCode != ERROR_NONE) {
      liftStack();
      clearRegister(REGISTER_X);
      lastErrorCode = ERROR_NONE;
    }
    real34Add(REGISTER_REAL34_DATA(REGISTER_X), const34_1, REGISTER_REAL34_DATA(REGISTER_X));
    liftStack();
    realToReal34(&tmp, REGISTER_REAL34_DATA(REGISTER_X));
    fnSigma(1);

    timerAppUp();

    if(timerAppState.totalTime > 0) {
      timerAppState.totalTime += msec - timerAppState.lapTime;
    }
    else {
      timerAppState.totalTime = msec;
    }
    timerAppState.lapTime = 0;
    if(timerAppState.started) {
      timerAppState.startUptime = timeUptimeMs();
    }

    refreshScreen();
  }

  void timerAppUp(void) {
    if(timerAppState.currentRegister >= 99) {
      timerAppState.currentRegister = 0;
    }
    else {
      timerAppState.currentRegister++;
    }
    timerAppState.isFirstDigit = true;
  }

  void timerAppDown(void) {
    if(timerAppState.currentRegister == 0) {
      timerAppState.currentRegister = 99;
    }
    else {
      timerAppState.currentRegister--;
    }
    timerAppState.isFirstDigit = true;
  }

  void timerAppDigitKey(uint16_t digit) {
    if(timerAppState.isFirstDigit) {
      timerAppState.isFirstDigit = false;
      timerAppState.firstDigit   = digit;
    }
    else {
      timerAppState.isFirstDigit = true;
      timerAppState.currentRegister = timerAppState.firstDigit * 10u + digit;
    }
  }

  void fnRecallTimerApp(uint16_t regist) {
    real_t tmp;
    bool_t overflow;
    uint32_t val;

    switch(getRegisterDataType(regist)) {
      case dtTime: {
        real34ToReal(REGISTER_REAL34_DATA(regist), &tmp);
        tmp.exponent += 3;
        realToUInt32(&tmp, DEC_ROUND_DOWN, &val, &overflow);
        break;
      }
      case dtReal34: {
        real34ToReal(REGISTER_REAL34_DATA(regist), &tmp);
        realMultiply(&tmp, const_3600, &tmp, &ctxtReal39);
        tmp.exponent += 3;
        realToUInt32(&tmp, DEC_ROUND_HALF_EVEN, &val, &overflow);
        break;
      }
      case dtLongInteger: {
        convertLongIntegerRegisterToReal(regist, &tmp, &ctxtReal39);
        realMultiply(&tmp, const_3600, &tmp, &ctxtReal39);
        tmp.exponent += 3;
        realToUInt32(&tmp, DEC_ROUND_HALF_EVEN, &val, &overflow);
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
      timerAppState.lapTime += val;
    }
  }

  void timerAppBackspace(void) {
    if(timerAppState.isFirstDigit) {
      fnResetTimerApp(NOPARAM);
    }
    else {
      timerAppState.isFirstDigit = true;
    }
  }

  void timerAppLeave(void) {
    popSoftmenu();
    calcModeLeave();
    timerAppState.isFirstDigit = true;
    watchIconEnabled = timerAppState.started;
  }

  static void _antirewinder(uint32_t currTime) {
    if(currTime < timerAppState.startUptime) {
      timerAppState.lapTime += 86400000u - timerAppState.startUptime;
      if(timerAppState.totalTime > 0) {
        timerAppState.totalTime += 86400000u - timerAppState.startUptime;
      }
      timerAppState.startUptime = 0u;
    }
    else if(currTime >= timerAppState.startUptime + 3600000u) {
      timerAppState.lapTime += 3600000u;
      if(timerAppState.totalTime > 0) {
        timerAppState.totalTime += 3600000u;
      }
      timerAppState.startUptime += 3600000u;
    }
  }

  void cbTimerAppDetectWrapAround(uint16_t unusedButMandatoryParameter) {
    if(timerAppState.started) {
      _antirewinder(timeUptimeMs());
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
