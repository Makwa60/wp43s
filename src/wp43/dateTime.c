// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "dateTime.h"

#include "constantPointers.h"
#include "debug.h"
#include "error.h"
#include "flags.h"
#include "hal/time.h"
#include "items.h"
#include "mathematics/comparisonReals.h"
#include "registers.h"
#include "registerValueConversions.h"
#include "stack.h"
#include <stdbool.h>
#include <string.h>

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



void fnSetDateFormat(uint16_t dateFormat) {
  switch(dateFormat) {
    case ITM_DMY: {
      clearSystemFlag(FLAG_MDY);
      clearSystemFlag(FLAG_YMD);
      setSystemFlag(FLAG_DMY);
      break;
    }

    case ITM_MDY: {
      clearSystemFlag(FLAG_DMY);
      clearSystemFlag(FLAG_YMD);
      setSystemFlag(FLAG_MDY);
      break;
    }

    case ITM_YMD: {
      clearSystemFlag(FLAG_MDY);
      clearSystemFlag(FLAG_DMY);
      setSystemFlag(FLAG_YMD);
      break;
    }

    default: {
    }
  }
}



void internalDateToJulianDay(const real34_t *source, real34_t *destination) {
  real34Subtract(source, const34_43200, destination);
  real34Divide(destination, const34_86400, destination), real34ToIntegralValue(destination, destination, DEC_ROUND_FLOOR);
}



void julianDayToInternalDate(const real34_t *source, real34_t *destination) {
  real34ToIntegralValue(source, destination, DEC_ROUND_FLOOR);
  real34Multiply(destination, const34_86400, destination);
  real34Add(destination, const34_43200, destination);
}



bool checkDateArgument(calcRegister_t regist, real34_t *jd) {
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
  switch(getRegisterDataType(regist)) {
    case dtDate: {
      internalDateToJulianDay(REGISTER_REAL34_DATA(regist), jd);
      return true;
    }

    case dtReal34: {
      if(getRegisterAngularMode(regist) == amNone) {
        reallocateRegister(TEMP_REGISTER_1, dtReal34, REAL34_SIZE_IN_BYTES, amNone); // make sure TEMP_REGISTER_1 is not of dtDate type here
        convertReal34RegisterToDateRegister(regist, TEMP_REGISTER_1);
        if(getRegisterDataType(TEMP_REGISTER_1) != dtDate) {
          return false; // invalid date
        }
        internalDateToJulianDay(REGISTER_REAL34_DATA(TEMP_REGISTER_1), jd);
        return true;
      }
      /* fallthrough */
    }

    default: {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("data type %s cannot be converted to date!", getRegisterDataTypeName(REGISTER_X, false, false));
      return false;
    }
  }
  #pragma GCC diagnostic pop
}



bool isLeapYear(const real34_t *year) {
  real34_t val, val2;
  int32_t  y400; // year mod 400
  bool     isGregorian;

  composeJulianDay(year, const34_2, const34_28, &val);
  uInt32ToReal34(firstGregorianDay, &val2);
  isGregorian = real34CompareGreaterEqual(&val, &val2);

  real34Divide(year, const34_400, &val), real34ToIntegralValue(&val, &val, DEC_ROUND_FLOOR);
  real34Multiply(&val, const34_400, &val);
  real34Subtract(year, &val, &val);
  y400 = real34ToInt32(&val);

  if(y400 == 0) {
    return true;
  }
  else if(isGregorian && (y400 % 100 == 0)) {
    return false;
  }

  return (y400 % 4 == 0);
}



bool isValidDay(const real34_t *year, const real34_t *month, const real34_t *day) {
  real34_t val;

  // Year (this rejects year -4713 and earlier)
  real34ToIntegralValue(year, &val, DEC_ROUND_FLOOR), real34Subtract(year, &val, &val);
  if(!real34IsZero(&val)) {
    return false;
  }
  real34Compare(year, const34__4712, &val);
  if(real34ToInt32(&val) < 0) {
    return false;
  }

  // Day
  real34ToIntegralValue(day, &val, DEC_ROUND_FLOOR), real34Subtract(day, &val, &val);
  if(!real34IsZero(&val)) {
    return false;
  }
  real34Compare(day, const34_1, &val);
  if(real34ToInt32(&val) < 0) {
    return false;
  }
  real34Compare(day, const34_31, &val);
  if(real34ToInt32(&val) > 0) {
    return false;
  }

  // Month
  real34ToIntegralValue(month, &val, DEC_ROUND_FLOOR), real34Subtract(month, &val, &val);
  if(!real34IsZero(&val)) {
    return false;
  }
  real34Compare(month, const34_1, &val);
  if(real34ToInt32(&val) < 0) {
    return false;
  }
  real34Compare(month, const34_12, &val);
  if(real34ToInt32(&val) > 0) {
    return false;
  }

  // Thirty days hath September...
  if(real34ToInt32(day) == 31) {
    switch(real34ToInt32(month)) {
      case 2:
      case 4:
      case 6:
      case 9:
      case 11: {
        return false;
      }
      default: {
        return true;
      }
    }
  }

  // February
  if(real34ToInt32(month) == 2) {
    if(real34ToInt32(day) == 30) {
      return false;
    }
    else if((real34ToInt32(day) == 29) && (!isLeapYear(year))) {
      return false;
    }
  }

  // Check for Julian-Gregorian gap
  if(firstGregorianDay > 0u) { // Gregorian
    real34_t y, m, d;
    composeJulianDay(year, month, day, &val);
    decomposeJulianDay(&val, &y, &m, &d);
    if(!real34CompareEqual(year, &y) || !real34CompareEqual(month, &m) || !real34CompareEqual(day, &d)) {
      return false;
    }
  }

  // Valid date
  return true;
}



static void divInt(const real34_t *p, const real34_t *q, real34_t *r) {
  real34_t tmp;
  real34Divide(p, q, &tmp), real34ToIntegralValue(&tmp, r, DEC_ROUND_DOWN);
}



static void divInt2(const real34_t *p, const real34_t *q, real34_t *r) {
  real34_t tmp;
  real34Divide(p, q, &tmp), real34ToIntegralValue(&tmp, r, DEC_ROUND_FLOOR);
}



static void modInt(const real34_t *p, const real34_t *q, real34_t *r) {
  real34_t tmp;
  divInt2(p, q, &tmp), real34Multiply(&tmp, q, &tmp), real34Subtract(p, &tmp, r);
}



void composeJulianDay(const real34_t *year, const real34_t *month, const real34_t *day, real34_t *jd) {
  real34_t fg, y, m, d;

  uInt32ToReal34(firstGregorianDay, &fg);
  real34Copy(year, &y); real34Copy(month, &m); real34Copy(day, &d);
  composeJulianDay_g(&y, &m, &d, jd);
  if((firstGregorianDay > 0u) && real34CompareLessThan(jd, &fg)) {
    composeJulianDay_j(&y, &m, &d, jd);
  }
}

// Gregorian
void composeJulianDay_g(const real34_t *year, const real34_t *month, const real34_t *day, real34_t *jd) {
  real34_t m_14_12; // round_down((month - 14) / 12)
  real34_t a;

  // round_down((month - 14) / 12)
  real34Subtract(month, const34_14, &m_14_12);
  divInt(&m_14_12, const34_12, &m_14_12);

  real34Add(year, const34_4800, &a);
  real34Add(&a, &m_14_12, &a);
  real34Multiply(&a, const34_1461, &a);
  divInt(&a, const34_4, jd);

  real34Multiply(&m_14_12, const34_12, &a);
  real34Subtract(month, &a, &a);
  real34Subtract(&a, const34_2, &a);
  real34Multiply(&a, const34_367, &a);
  divInt(&a, const34_12, &a);
  real34Add(jd, &a, jd);

  real34Add(year, const34_4900, &a);
  real34Add(&a, &m_14_12, &a);
  divInt(&a, const34_100, &a);
  real34Multiply(&a, const34_3, &a);
  divInt(&a, const34_4, &a);
  real34Subtract(jd, &a, jd);

  real34Add(jd, day, jd);
  real34Subtract(jd, const34_32075, jd);
}

// Julian
void composeJulianDay_j(const real34_t *year, const real34_t *month, const real34_t *day, real34_t *jd) {
  real34_t a;

  real34Multiply(year, const34_367, jd);

  real34Subtract(month, const34_9, &a);
  divInt(&a, const34_7, &a);
  real34Add(&a, year, &a);
  real34Add(&a, const34_5001, &a);
  real34Multiply(&a, const34_7, &a);
  divInt(&a, const34_4, &a);
  real34Subtract(jd, &a, jd);

  real34Multiply(month, const34_275, &a);
  divInt(&a, const34_9, &a);
  real34Add(jd, &a, jd);

  real34Add(jd, day, jd);
  real34Add(jd, const34_1729777, jd);
}



void decomposeJulianDay(const real34_t *jd, real34_t *year, real34_t *month, real34_t *day) {
  real34_t e, h, tmp1;

  real34Add(jd, const34_1401, &e);
  uInt32ToReal34(firstGregorianDay, &tmp1);
  // proleptic Gregorian calendar is used if firstGregorianDay == 0: for special purpose only!
  if((firstGregorianDay == 0u) || real34CompareGreaterEqual(jd, &tmp1)) { // Gregorian
    real34Multiply(jd, const34_4, &tmp1);
    real34Add(&tmp1, const34_274277, &tmp1);
    divInt2(&tmp1, const34_146097, &tmp1);
    real34Multiply(&tmp1, const34_3, &tmp1);
    divInt2(&tmp1, const34_4, &tmp1);
    real34Subtract(&tmp1, const34_38, &tmp1);
    real34Add(&e, &tmp1, &e);
  }

  real34Multiply(&e, const34_4, &e);
  real34Add(&e, const34_3, &e);

  modInt(&e, const34_1461, &h);
  divInt2(&h, const34_4, &h);
  real34Multiply(&h, const34_5, &h);
  real34Add(&h, const34_2, &h);

  modInt(&h, const34_153, day);
  divInt2(day, const34_5, day);
  real34Add(day, const34_1, day);

  divInt2(&h, const34_153, month);
  real34Add(month, const34_2, month);
  modInt(month, const34_12, month);
  real34Add(month, const34_1, month);

  divInt2(&e, const34_1461, year);
  real34Subtract(year, const34_4716, year);
  real34Subtract(const34_14, month, &tmp1);
  divInt2(&tmp1, const34_12, &tmp1);
  real34Add(year, &tmp1, year);
}



uint32_t getDayOfWeek(calcRegister_t regist) {
  real34_t date34;
  if(checkDateArgument(regist, &date34)) {
    modInt(&date34, const34_7, &date34);
    real34Add(&date34, const34_1, &date34);
    return real34ToUInt32(&date34);
  }
  else {
    return 0u;
  }
}



void checkDateRange(const real34_t *date34) {
  if(real34CompareGreaterEqual(date34, const34_maxDate) || real34IsNegative(date34)) {
    displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("value of date type is too large");
    return;
  }
}



void hmmssToSeconds(const real34_t *src, real34_t *dest) {
  real34_t time34, real34, value34;
  int32_t sign;

  sign = real34IsNegative(src);

  // Hours
  real34CopyAbs(src, &time34);
  real34ToIntegralValue(&time34, &real34, DEC_ROUND_DOWN);
  real34Multiply(&real34, const34_3600, dest);

  // Minutes
  real34Subtract(&time34, &real34, &time34);
  real34Multiply(&time34, const34_100, &time34);
  real34ToIntegralValue(&time34, &real34, DEC_ROUND_DOWN);
  real34Multiply(const34_60, &real34, &value34);
  real34Add(dest, &value34, dest);

  // Seconds
  real34Subtract(&time34, &real34, &time34);
  real34Multiply(&time34, const34_100, &time34);
  real34Add(dest, &time34, dest);

  // Sign
  if(sign) {
    real34SetNegativeSign(dest);
  }
}



void hmmssInRegisterToSeconds(calcRegister_t regist) {
  real34_t real34;

  real34Copy(REGISTER_REAL34_DATA(regist), &real34);
  reallocateRegister(regist, dtTime, REAL34_SIZE_IN_BYTES, amNone);
  hmmssToSeconds(&real34, REGISTER_REAL34_DATA(regist));
  checkTimeRange(REGISTER_REAL34_DATA(regist));
}



void checkTimeRange(const real34_t *time34) {
  real34_t t;

  real34CopyAbs(time34, &t);
  if(real34CompareGreaterEqual(&t, const34_maxTime)) {
    displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("value of time type is too large");
    return;
  }
}



void fnJulianToDate(uint16_t unusedButMandatoryParameter) {
  real34_t date;

  if(!saveLastX()) {
    return;
  }

  switch(getRegisterDataType(REGISTER_X)) {
    case dtLongInteger: {
      convertLongIntegerRegisterToReal34Register(REGISTER_X, REGISTER_X);
      julianDayToInternalDate(REGISTER_REAL34_DATA(REGISTER_X), &date);
      reallocateRegister(REGISTER_X, dtDate, REAL34_SIZE_IN_BYTES, amNone);
      real34Copy(&date, REGISTER_REAL34_DATA(REGISTER_X));
      break;
    }

    case dtReal34: {
      real34ToIntegralValue(REGISTER_REAL34_DATA(REGISTER_X), REGISTER_REAL34_DATA(REGISTER_X), DEC_ROUND_DOWN);
      julianDayToInternalDate(REGISTER_REAL34_DATA(REGISTER_X), &date);
      reallocateRegister(REGISTER_X, dtDate, REAL34_SIZE_IN_BYTES, amNone);
      real34Copy(&date, REGISTER_REAL34_DATA(REGISTER_X));
      break;
    }

    default: {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("data type %s cannot be converted to date!", getRegisterDataTypeName(REGISTER_X, false, false));
      return;
    }
  }

  // check range
  checkDateRange(REGISTER_REAL34_DATA(REGISTER_X));
  if(lastErrorCode != 0) {
    undo();
  }
}



void fnDateToJulian(uint16_t unusedButMandatoryParameter) {
  real34_t jd34;

  if(!saveLastX()) {
    return;
  }

  if(checkDateArgument(REGISTER_X, &jd34)) {
    convertReal34ToLongIntegerRegister(&jd34, REGISTER_X, DEC_ROUND_FLOOR);
  }
}



void fnIsLeap(uint16_t unusedButMandatoryParameter) {
  real34_t y, m, d, j;

  if(checkDateArgument(REGISTER_X, &j)) {
    decomposeJulianDay(&j, &y, &m, &d);
    temporaryInformation = (isLeapYear(&y) ? TI_TRUE : TI_FALSE);
  }
}



void fnSetFirstGregorianDay(uint16_t unusedButMandatoryParameter) {
  real34_t jd34;
  const uint32_t fgd = firstGregorianDay;

  if((getRegisterDataType(REGISTER_X) == dtReal34) && (getRegisterAngularMode(REGISTER_X) == amNone)) {
    firstGregorianDay = 0u; // proleptic Gregorian mode
    if(checkDateArgument(REGISTER_X, &jd34)) {
      firstGregorianDay = real34ToUInt32(&jd34);
    }
    else {
      firstGregorianDay = fgd;
    }
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      if(getRegisterDataType(REGISTER_X) == dtDate) {
        errorMoreInfo("data type %s is disabled as input because of complicated Julian-Gregorian issue!", getRegisterDataTypeName(REGISTER_X, false, false));
      }
      else {
        errorMoreInfo("data type %s cannot be interpreted as a date!", getRegisterDataTypeName(REGISTER_X, false, false));
      }
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
  }
}



void fnGetFirstGregorianDay(uint16_t unusedButMandatoryParameter) {
  real34_t j;

  uInt32ToReal34(firstGregorianDay, &j);
  liftStack();
  reallocateRegister(REGISTER_X, dtDate, REAL34_SIZE_IN_BYTES, amNone);
  julianDayToInternalDate(&j, REGISTER_REAL34_DATA(REGISTER_X));
}



void fnXToDate(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
  switch(getRegisterDataType(REGISTER_X)) {
    case dtDate: {
      /* already in date: do nothing */
      break;
    }

    case dtReal34: {
      if(getRegisterAngularMode(REGISTER_X) == amNone) {
        convertReal34RegisterToDateRegister(REGISTER_X, REGISTER_X);
        checkDateRange(REGISTER_REAL34_DATA(REGISTER_X));
        if(lastErrorCode != 0) {
          undo();
        }
        break;
      }
      /* fallthrough */
    }

    default: {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("data type %s cannot be converted to date!", getRegisterDataTypeName(REGISTER_X, false, false));
      return;
    }
  }
  #pragma GCC diagnostic pop
}



void fnYear(uint16_t unusedButMandatoryParameter) {
  real34_t y, m, d, j;

  if(!saveLastX()) {
    return;
  }

  if(checkDateArgument(REGISTER_X, &j)) {
    decomposeJulianDay(&j, &y, &m, &d);
    convertReal34ToLongIntegerRegister(&y, REGISTER_X, DEC_ROUND_FLOOR);
  }
}



void fnMonth(uint16_t unusedButMandatoryParameter) {
  real34_t y, m, d, j;

  if(!saveLastX()) {
    return;
  }

  if(checkDateArgument(REGISTER_X, &j)) {
    decomposeJulianDay(&j, &y, &m, &d);
    convertReal34ToLongIntegerRegister(&m, REGISTER_X, DEC_ROUND_FLOOR);
  }
}



void fnDay(uint16_t unusedButMandatoryParameter) {
  real34_t y, m, d, j;

  if(!saveLastX()) {
    return;
  }

  if(checkDateArgument(REGISTER_X, &j)) {
    decomposeJulianDay(&j, &y, &m, &d);
    convertReal34ToLongIntegerRegister(&d, REGISTER_X, DEC_ROUND_FLOOR);
  }
}



void fnWday(uint16_t unusedButMandatoryParameter) {
  const uint32_t dayOfWeek = getDayOfWeek(REGISTER_X);
  longInteger_t result;

  if(!saveLastX()) {
    return;
  }

  if(dayOfWeek != 0) {
    longIntegerInit(result);
    uIntToLongInteger(dayOfWeek, result);
    convertLongIntegerToLongIntegerRegister(result, REGISTER_X);
    longIntegerFree(result);
    temporaryInformation = TI_DAY_OF_WEEK;
  }
}



void fnDateTo(uint16_t unusedButMandatoryParameter) {
  real34_t y, m, d, j;

  if(!saveLastX()) {
    return;
  }

  if(checkDateArgument(REGISTER_X, &j)) {
    liftStack();
    liftStack();
    decomposeJulianDay(&j, &y, &m, &d);
    convertReal34ToLongIntegerRegister(&y, getSystemFlag(FLAG_YMD) ? REGISTER_Z :                                        REGISTER_X, DEC_ROUND_FLOOR);
    convertReal34ToLongIntegerRegister(&m, getSystemFlag(FLAG_MDY) ? REGISTER_Z :                                        REGISTER_Y, DEC_ROUND_FLOOR);
    convertReal34ToLongIntegerRegister(&d, getSystemFlag(FLAG_DMY) ? REGISTER_Z : getSystemFlag(FLAG_MDY) ? REGISTER_Y : REGISTER_X, DEC_ROUND_FLOOR);
  }
}



void fnToDate(uint16_t unusedButMandatoryParameter) {
  real34_t y, m, d, j;
  real34_t *part[3];
  calcRegister_t r[3] = {REGISTER_Z, REGISTER_Y, REGISTER_X};
  int32_t i;

  if(!saveLastX()) {
    return;
  }

  if(getSystemFlag(FLAG_DMY)) {
    part[0] = &d;
    part[1] = &m;
    part[2] = &y;
  }
  else if(getSystemFlag(FLAG_MDY)) {
    part[0] = &m;
    part[1] = &d;
    part[2] = &y;
  }
  else {
    part[0] = &y;
    part[1] = &m;
    part[2] = &d;
  }

  for(i = 0; i < 3; ++i) {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
    switch(getRegisterDataType(r[i])) {
      case dtLongInteger: {
        convertLongIntegerRegisterToReal34(r[i], part[i]);
        break;
      }

      case dtReal34: {
        if(getRegisterAngularMode(r[i]) == amNone) {
          real34ToIntegralValue(REGISTER_REAL34_DATA(r[i]), part[i], DEC_ROUND_DOWN);
          break;
        }
        /* fallthrough */
      }

      default: {
        displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
        errorMoreInfo("data type %s cannot be converted to a real34!", getRegisterDataTypeName(r[i], false, false));
        return;
      }
    }
    #pragma GCC diagnostic pop
  }

  if(!isValidDay(&y, &m, &d)) {
      displayCalcErrorMessage(ERROR_BAD_TIME_OR_DATE_INPUT, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      errorMoreInfo("Invalid date input like 30 Feb.");
      return;
  }

  // valid date
  fnDropY(NOPARAM);
  if(lastErrorCode == ERROR_NONE) {
    fnDropY(NOPARAM);
    if(lastErrorCode == ERROR_NONE) {
      composeJulianDay(&y, &m, &d, &j);
      reallocateRegister(REGISTER_X, dtDate, REAL34_SIZE_IN_BYTES, amNone);
      julianDayToInternalDate(&j, REGISTER_REAL34_DATA(REGISTER_X));

      // check range
      checkDateRange(REGISTER_REAL34_DATA(REGISTER_X));
    }
  }
  if(lastErrorCode != 0) {
    undo();
  }
}



void fnToHr(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  switch(getRegisterDataType(REGISTER_X)) {
    case dtTime: {
      convertTimeRegisterToReal34Register(REGISTER_X, REGISTER_X);
      break;
    }

    default: {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("data type %s cannot be converted to a real34!", getRegisterDataTypeName(REGISTER_X, false, false));
      return;
    }
  }
}



void fnToHms(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
  switch(getRegisterDataType(REGISTER_X)) {
    case dtLongInteger: {
      convertLongIntegerRegisterToTimeRegister(REGISTER_X, REGISTER_X);
      break;
    }

    case dtTime: {
      /* already in hours: do nothing */
      break;
    }

    case dtReal34: {
      if(getRegisterAngularMode(REGISTER_X) == amNone) {
        convertReal34RegisterToTimeRegister(REGISTER_X, REGISTER_X);
        break;
      }
      /* fallthrough */
    }

    default: {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("data type %s cannot be converted to time!", getRegisterDataTypeName(REGISTER_X, false, false));
      return;
    }
  }
  #pragma GCC diagnostic pop
  checkTimeRange(REGISTER_REAL34_DATA(REGISTER_X));
  if(lastErrorCode != 0) {
    undo();
  }
}



void fnDate(uint16_t unusedButMandatoryParameter) {
  real34_t y, m, d, j;
  dateInfo_t dateInfo;

  timeGetDateInfo(&dateInfo);
  uInt32ToReal34(dateInfo.year,  &y);
  uInt32ToReal34(dateInfo.month, &m);
  uInt32ToReal34(dateInfo.day,   &d);

  composeJulianDay(&y, &m, &d, &j);
  liftStack();
  reallocateRegister(REGISTER_X, dtDate, REAL34_SIZE_IN_BYTES, amNone);
  julianDayToInternalDate(&j, REGISTER_REAL34_DATA(REGISTER_X));
  temporaryInformation = TI_DAY_OF_WEEK;
}



void fnTime(uint16_t unusedButMandatoryParameter) {
  real34_t time34;
  timeInfo_t timeInfo;

  timeGetTimeInfo(&timeInfo);
  uInt32ToReal34((uint32_t)timeInfo.hour * 3600u + (uint32_t)timeInfo.min * 60u + (uint32_t)timeInfo.sec, &time34);

  liftStack();
  reallocateRegister(REGISTER_X, dtTime, REAL34_SIZE_IN_BYTES, amNone);
  real34Copy(&time34, REGISTER_REAL34_DATA(REGISTER_X));
}



void fnSetDate(uint16_t unusedButMandatoryParameter) {
  #if defined(DMCP_BUILD)
    dateInfo_t dateInfo;
    real34_t j, y, m, d;

    if(checkDateArgument(REGISTER_X, &j)) {
      decomposeJulianDay(&j, &y, &m, &d);
      dateInfo.year  = real34ToUInt32(&y);
      dateInfo.month = real34ToUInt32(&m);
      dateInfo.day   = real34ToUInt32(&d);
      timeSetDateInfo(&dateInfo);
    }
  #else // !DMCP_BUILD
    displayCalcErrorMessage(ERROR_FUNCTION_NOT_FOUND, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("real calculator only!");
  #endif // DMCP_BUILD
}



void fnSetTime(uint16_t unusedButMandatoryParameter) {
  #if defined(DMCP_BUILD)
    timeInfo_t timeInfo;
    real34_t time34;
    int32_t timeVal;

    if(getRegisterDataType(REGISTER_X) == dtTime) {
      if(real34IsNegative(REGISTER_REAL34_DATA(REGISTER_X)) || real34IsNaN(REGISTER_REAL34_DATA(REGISTER_X)) || real34CompareGreaterEqual(REGISTER_REAL34_DATA(REGISTER_X), const34_86400)) {
        displayCalcErrorMessage(ERROR_BAD_TIME_OR_DATE_INPUT, ERR_REGISTER_LINE, REGISTER_X);
      }
      else {
        real34Multiply(REGISTER_REAL34_DATA(REGISTER_X), const34_100, &time34);
        real34ToIntegralValue(&time34, &time34, DEC_ROUND_DOWN);
        timeVal = real34ToInt32(&time34);
        timeInfo.csec =  timeVal         % 100;
        timeInfo.sec  = (timeVal /= 100) %  60;
        timeInfo.min  = (timeVal /=  60) %  60;
        timeInfo.hour = (timeVal /=  60);
        timeSetTimeInfo(&timeInfo);
      }
    }
    else if(getRegisterDataType(REGISTER_X) == dtReal34) {
      if(real34IsNegative(REGISTER_REAL34_DATA(REGISTER_X)) || real34IsNaN(REGISTER_REAL34_DATA(REGISTER_X)) || real34CompareGreaterEqual(REGISTER_REAL34_DATA(REGISTER_X), const34_24)) {
        displayCalcErrorMessage(ERROR_BAD_TIME_OR_DATE_INPUT, ERR_REGISTER_LINE, REGISTER_X);
      }
      else {
        real34Multiply(REGISTER_REAL34_DATA(REGISTER_X), const34_1e6, &time34);
        real34ToIntegralValue(&time34, &time34, DEC_ROUND_DOWN);
        timeVal = real34ToInt32(&time34);
        timeInfo.csec =  timeVal         % 100;
        timeInfo.sec  = (timeVal /= 100) % 100;
        timeInfo.min  = (timeVal /= 100) % 100;
        timeInfo.hour = (timeVal /= 100);
        if(timeInfo.sec <= 59 && timeInfo.min <= 59 && timeInfo.hour <= 23) {
          timeSetTimeInfo(&timeInfo);
        }
        else {
          displayCalcErrorMessage(ERROR_BAD_TIME_OR_DATE_INPUT, ERR_REGISTER_LINE, REGISTER_X);
        }
      }
    }
    else {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    }
  #else // !DMCP_BUILD
    displayCalcErrorMessage(ERROR_FUNCTION_NOT_FOUND, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("real calculator only!");
  #endif // DMCP_BUILD
}



void getDateString(char *dateString) {
  dateInfo_t dateInfo;

  timeGetDateInfo(&dateInfo);

  if(!getSystemFlag(FLAG_TDM24)) { // 2 digit year
    if(getSystemFlag(FLAG_DMY)) {
      sprintf(dateString, "%02d.%02d.%02d", dateInfo.day, dateInfo.month, dateInfo.year % 100);
    }
    else if(getSystemFlag(FLAG_YMD)) {
      sprintf(dateString, "%02d-%02d-%02d", dateInfo.year % 100, dateInfo.month, dateInfo.day);
    }
    else if(getSystemFlag(FLAG_MDY)) {
      sprintf(dateString, "%02d/%02d/%02d", dateInfo.month, dateInfo.day, dateInfo.year % 100);
    }
    else {
      strcpy(dateString, "?? ?? ????");
    }
  }
  else {// 4 digit year
    if(getSystemFlag(FLAG_DMY)) {
      sprintf(dateString, "%02d.%02d.%04d", dateInfo.day, dateInfo.month, dateInfo.year);
    }
    else if(getSystemFlag(FLAG_YMD)) {
      sprintf(dateString, "%04d-%02d-%02d", dateInfo.year, dateInfo.month, dateInfo.day);
    }
    else if(getSystemFlag(FLAG_MDY)) {
      sprintf(dateString, "%02d/%02d/%04d", dateInfo.month, dateInfo.day, dateInfo.year);
    }
    else {
      strcpy(dateString, "?? ?? ????");
    }
  }
}



uint32_t getTimeString(char *timeString) {
  timeInfo_t timeInfo;

  timeGetTimeInfo(&timeInfo);

  if(!getSystemFlag(FLAG_TDM24)) {
    sprintf(timeString, "%02d:%02d", (timeInfo.hour % 12) == 0 ? 12 : timeInfo.hour % 12, timeInfo.min);
    if(timeInfo.hour >= 12) {
      strcat(timeString, "pm");
    }
    else {
      strcat(timeString, "am");
    }
  }
  else {
    sprintf(timeString, "%02d:%02d", timeInfo.hour, timeInfo.min);
  }
  uint32_t timeToNextMinuteMs = (6000 - ((timeInfo.sec * 100) + timeInfo.csec)) * 10;
  return timeToNextMinuteMs;
}
