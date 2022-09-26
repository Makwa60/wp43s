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

#include "timerApp.h"

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
#include "screen.h"
#include "softmenus.h"
#include "stats.h"
#include "timer.h"
#include "typeDefinitions.h"
#include <assert.h>

#include "wp43.h"


void fnTicks(uint16_t unusedButMandatoryParameter) {
  uint32_t tim;
  longInteger_t lgInt;

  tim = timeUptimeMs() / 100;

  liftStack();
  longIntegerInit(lgInt);
  uIntToLongInteger(tim, lgInt);
  convertLongIntegerToLongIntegerRegister(lgInt, REGISTER_X);
  longIntegerFree(lgInt);
}



#if !defined(TESTSUITE_BUILD)
  static void _antirewinder(uint32_t currTime) {
    if(currTime < timerStartTime) {
      timerValue += 86400000u - timerStartTime;
      if(timerTotalTime > 0) {
        timerTotalTime += 86400000u - timerStartTime;
      }
      timerStartTime = 0u;
    }
    else if(currTime >= timerStartTime + 3600000u) {
      timerValue += 3600000u;
      if(timerTotalTime > 0) {
        timerTotalTime += 3600000u;
      }
      timerStartTime += 3600000u;
    }
  }

  static uint32_t _getTimerValue(void) {
    uint32_t currTime = timeCurrentMs();

    if(timerStartTime == TIMER_APP_STOPPED) {
      return timerValue;
    }

    _antirewinder(currTime);
    return currTime - timerStartTime + timerValue;
  }

  //#if defined(PC_BUILD)
  //  static gboolean _updateTimer(gpointer unusedData) {
  //    if(calcMode != cmTimerApp) {
  //      return FALSE;
  //    }
  //    fnUpdateTimerApp();
  //    return timerStartTime != TIMER_APP_STOPPED;
  //  }
  //#endif // PC_BUILD

  void fnTimerApp(uint16_t unusedButMandatoryParameter) {
    calcModeEnter(cmTimerApp);
    rbr1stDigit = true;
    watchIconEnabled = false;
    if(timerStartTime != TIMER_APP_STOPPED) {
      timerStart(TO_TIMER_APP, TO_TIMER_APP, TIMER_APP_PERIOD);
      //#if defined(PC_BUILD)
      //  gdk_threads_add_timeout(100, _updateTimer, NULL);
      //#endif // PC_BUILD
    }
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
    timerCraAndDeciseconds ^= 0x80u;
  }

  void fnResetTimerApp(uint16_t unusedButMandatoryParameter) {
    timerValue = 0;
    timerTotalTime = 0;
    if(timerStartTime != TIMER_APP_STOPPED) {
      timerStartTime = timeCurrentMs();
      timerStart(TO_TIMER_APP, TO_TIMER_APP, TIMER_APP_PERIOD);
    }
    rbr1stDigit = true;
  }

  void fnStartStopTimerApp(void) {
    if(timerStartTime == TIMER_APP_STOPPED) {
      setSystemFlag(FLAG_RUNTIM);
      timerStartTime = timeCurrentMs();
      timerStart(TO_TIMER_APP, TO_TIMER_APP, TIMER_APP_PERIOD);
      //#if defined(PC_BUILD)
      //  gdk_threads_add_timeout(100, _updateTimer, NULL);
      //#endif // PC_BUILD
    }
    else {
      fnStopTimerApp();
    }
    rbr1stDigit = true;
  }

  void fnStopTimerApp(void) {
    if(timerStartTime != TIMER_APP_STOPPED) {
      const uint32_t msec = timeCurrentMs();
      timerValue += msec - timerStartTime;
      if(timerTotalTime > 0) {
        timerTotalTime += msec - timerStartTime;
      }
      timerStartTime = TIMER_APP_STOPPED;
      timerStop(TO_TIMER_APP);
    }
    clearSystemFlag(FLAG_RUNTIM);
    watchIconEnabled = false;
  }

  void fnShowTimerApp(void) {
    assert(calcMode == cmTimerApp);

    const uint32_t msec = _getTimerValue();
    clearRegisterLine(REGISTER_T, true, true);

    tmpString[0] = 0;

    if(timerTotalTime > 0) {
      const uint32_t tmsec = msec - timerValue + timerTotalTime;
      if(timerCraAndDeciseconds & 0x80u) {
        sprintf(tmpString, "%2" PRIu32 ":%02" PRIu32 ":%02" PRIu32 ".%" PRIu32 STD_SUP_T "  ", tmsec / 3600000u, tmsec % 3600000u / 60000u, tmsec % 60000u / 1000u, tmsec % 1000u / 100u);
      }
      else {
        sprintf(tmpString, "%2" PRIu32 ":%02" PRIu32 ":%02" PRIu32 STD_SUP_T STD_SPACE_PUNCTUATION STD_SPACE_FIGURE "  ", tmsec / 3600000u, tmsec % 3600000u / 60000u, tmsec % 60000u / 1000u);
      }
    }

    if(timerCraAndDeciseconds & 0x80u) {
      sprintf(tmpString + stringByteLength(tmpString), "%2" PRIu32 ":%02" PRIu32 ":%02" PRIu32 ".%" PRIu32 " ", msec / 3600000u, msec % 3600000u / 60000u, msec % 60000u / 1000u, msec % 1000u / 100u);
    }
    else {
      sprintf(tmpString + stringByteLength(tmpString), "%2" PRIu32 ":%02" PRIu32 ":%02" PRIu32 STD_SPACE_PUNCTUATION STD_SPACE_FIGURE " ", msec / 3600000u, msec % 3600000u / 60000u, msec % 60000u / 1000u);
    }

    if(rbr1stDigit) {
      sprintf(tmpString + stringByteLength(tmpString), "[%02" PRIu32 "]", (uint32_t)(timerCraAndDeciseconds & 0x7fu));
    }
    else if(aimBuffer[AIM_BUFFER_LENGTH / 2] == 0) {
      sprintf(tmpString + stringByteLength(tmpString), "[" STD_CURSOR STD_SPACE_FIGURE "]");
    }
    else {
      sprintf(tmpString + stringByteLength(tmpString), "[%" PRId32 STD_CURSOR "]", (int32_t)(aimBuffer[AIM_BUFFER_LENGTH / 2] - '0'));
    }
    showString(tmpString, &numericFont, 1, Y_POSITION_OF_REGISTER_T_LINE, vmNormal, true, true);

    bool_t timerMenu = false;
    for(int i = 0; i < SOFTMENU_STACK_SIZE; i++) {
      if(softmenu[softmenuStack[i].softmenuId].menuItem == -MNU_TIMERF) {
        timerMenu = true;
        break;
      }
    }
    if(!timerMenu) {
      showSoftmenu(-MNU_TIMERF);
    }
    if(softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_TIMERF) {
      guiSetLayout(glTimerApp);
    }
  }

  void fnUpdateTimerApp(void) {
    if(calcMode == cmTimerApp) {
      fnShowTimerApp();
      displayShiftAndTamBuffer();
      #if defined(DMCP_BUILD)
        refreshLcd();
        lcd_refresh();
      #else // !DMCP_BUILD
        refreshLcd();
      #endif // DMCP_BUILD
    }
  }

  void fnEnterTimerApp(void) {
    if(rbr1stDigit) {
      real_t tmp;
      uInt32ToReal(_getTimerValue() / 100u, &tmp);
      tmp.exponent -= 1;
      reallocateRegister(timerCraAndDeciseconds & 0x7fu, dtTime, REAL34_SIZE, amNone);
      realToReal34(&tmp, REGISTER_REAL34_DATA(timerCraAndDeciseconds & 0x7fu));
      fnUpTimerApp();
    }
    else if(aimBuffer[AIM_BUFFER_LENGTH / 2] == 0) {
      rbr1stDigit = true;
    }
    else {
      timerCraAndDeciseconds = (timerCraAndDeciseconds & 0x80u) + (uint8_t)(aimBuffer[AIM_BUFFER_LENGTH / 2] - '0');
      rbr1stDigit = true;
    }
  }

  void fnDotTimerApp(void) {
    const uint32_t msec = _getTimerValue();
    real_t tmp;

    uInt32ToReal(msec / 100u, &tmp);
    tmp.exponent -= 1;
    reallocateRegister(timerCraAndDeciseconds & 0x7fu, dtTime, REAL34_SIZE, amNone);
    realToReal34(&tmp, REGISTER_REAL34_DATA(timerCraAndDeciseconds & 0x7fu));

    fnUpTimerApp();

    if(timerTotalTime > 0) {
      timerTotalTime += msec - timerValue;
    }
    else {
      timerTotalTime = msec;
    }
    timerValue = 0;
    if(timerStartTime != TIMER_APP_STOPPED) {
      timerStartTime = timeCurrentMs();
      timerStart(TO_TIMER_APP, TO_TIMER_APP, TIMER_APP_PERIOD);
    }
  }

  void fnPlusTimerApp(void) {
    const uint32_t msec = _getTimerValue();
    real_t tmp;

    uInt32ToReal(msec / 100u, &tmp);
    tmp.exponent -= 1;
    reallocateRegister(timerCraAndDeciseconds & 0x7fu, dtTime, REAL34_SIZE, amNone);
    realToReal34(&tmp, REGISTER_REAL34_DATA(timerCraAndDeciseconds & 0x7fu));
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

    fnUpTimerApp();

    if(timerTotalTime > 0) {
      timerTotalTime += msec - timerValue;
    }
    else {
      timerTotalTime = msec;
    }
    timerValue = 0;
    if(timerStartTime != TIMER_APP_STOPPED) {
      timerStartTime = timeCurrentMs();
      timerStart(TO_TIMER_APP, TO_TIMER_APP, TIMER_APP_PERIOD);
    }

    refreshScreen();
  }

  void fnUpTimerApp(void) {
    if((timerCraAndDeciseconds & 0x7fu) >= 99u) {
      timerCraAndDeciseconds &= 0x80u;
    }
    else {
      ++timerCraAndDeciseconds;
    }
    rbr1stDigit = true;
  }

  void fnDownTimerApp(void) {
    if((timerCraAndDeciseconds & 0x7fu) == 0u) {
      timerCraAndDeciseconds |= 99u;
    }
    else {
      --timerCraAndDeciseconds;
    }
    rbr1stDigit = true;
  }

  void fnDigitKeyTimerApp(uint16_t digit) {
    if(rbr1stDigit || aimBuffer[AIM_BUFFER_LENGTH / 2] == 0) {
      aimBuffer[AIM_BUFFER_LENGTH / 2    ] = digit + '0';
      aimBuffer[AIM_BUFFER_LENGTH / 2 + 1] = 0;
      rbr1stDigit = false;
    }
    else {
      timerCraAndDeciseconds = (timerCraAndDeciseconds & 0x80u) + (uint8_t)(aimBuffer[AIM_BUFFER_LENGTH / 2] - '0') * 10u + digit;
      rbr1stDigit = true;
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
      timerValue = val;
      if(timerStartTime != TIMER_APP_STOPPED) {
        timerStartTime = timeCurrentMs();
        timerStart(TO_TIMER_APP, TO_TIMER_APP, TIMER_APP_PERIOD);
      }
    }
  }

  void fnBackspaceTimerApp(void) {
    if(rbr1stDigit) {
      fnResetTimerApp(NOPARAM);
    }
    else if(aimBuffer[AIM_BUFFER_LENGTH / 2] == 0) {
      rbr1stDigit = true;
    }
    else {
      aimBuffer[AIM_BUFFER_LENGTH / 2] = 0;
    }
  }

  void fnLeaveTimerApp(void) {
    popSoftmenu();
    rbr1stDigit = true;
    calcModeLeave();
    watchIconEnabled = (timerStartTime != TIMER_APP_STOPPED);
  }

  void fnPollTimerApp(void) { // poll every minute not to rewind the timer
    if(calcMode != cmTimerApp && timerStartTime != TIMER_APP_STOPPED) {
      _antirewinder(timeCurrentMs());
    }
  }
#else // TESTSUITE_BUILD
  void fnTimerApp(uint16_t unusedButMandatoryParameter) {}
  void fnAddTimerApp(uint16_t unusedButMandatoryParameter) {}
  void fnDecisecondTimerApp(uint16_t unusedButMandatoryParameter) {}
  void fnResetTimerApp(uint16_t unusedButMandatoryParameter) {}
  void fnStartStopTimerApp(void) {}
  void fnStopTimerApp(void) {}
  void fnShowTimerApp(void) {}
  void fnUpdateTimerApp(void) {}
  void fnEnterTimerApp(void) {}
  void fnDotTimerApp(void) {}
  void fnPlusTimerApp(void) {}
  void fnUpTimerApp(void) {}
  void fnDownTimerApp(void) {}
  void fnDigitKeyTimerApp(uint16_t digit) {}
  void fnRecallTimerApp(uint16_t regist) {}
  void fnBackspaceTimerApp(void) {}
  void fnLeaveTimerApp(void) {}
  void fnPollTimerApp(void) {}
#endif // !TESTSUITE_BUILD TESTSUITE_BUILD
