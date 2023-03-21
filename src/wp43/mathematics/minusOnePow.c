// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/minusOnePow.h"

#include "constantPointers.h"
#include "debug.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "integers.h"
#include "items.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/cos.h"
#include "mathematics/matrix.h"
#include "mathematics/sin.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

TO_QSPI void (* const m1Pow[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1            2          3          4           5           6           7          8           9             10
//          Long integer Real34     complex34  Time        Date        String      Real34 mat Complex34 m Short integer Config data
            m1PowLonI,   m1PowReal, m1PowCplx, m1PowError, m1PowError, m1PowError, m1PowRema, m1PowCxma,  m1PowShoI,    m1PowError
};



/********************************************//**
 * \brief Data type error in exp
 *
 * \param void
 * \return void
 ***********************************************/
#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void m1PowError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    sprintf(errorMessage, "cannot calculate (-1)" STD_SUP_x " for %s", getRegisterDataTypeName(REGISTER_X, true, false));
    moreInfoOnError("In function fnM1Pow:", errorMessage, NULL, NULL);
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



/********************************************//**
 * \brief regX ==> regL and 10^regX ==> regX
 * enables stack lift and refreshes the stack
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnM1Pow(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  m1Pow[getRegisterDataType(REGISTER_X)]();

  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}



/**********************************************************************
 * In all the functions below:
 * if X is a number then X = a + ib
 * The variables a and b are used for intermediate calculations
 ***********************************************************************/

void m1PowLonI(void) {
  longInteger_t lgInt, exponent;

  longIntegerInit(lgInt);
  uIntToLongInteger(1, lgInt);

  convertLongIntegerRegisterToLongInteger(REGISTER_X, exponent);
  if(longIntegerIsOdd(exponent)) {
    longIntegerChangeSign(lgInt);
  }

  convertLongIntegerToLongIntegerRegister(lgInt, REGISTER_X);

  longIntegerFree(lgInt);
  longIntegerFree(exponent);
}



void m1PowRema(void) {
  elementwiseRema(m1PowReal);
}



void m1PowCxma(void) {
  elementwiseCxma(m1PowCplx);
}



void m1PowShoI(void) {
  int32_t signExponent;
  uint64_t valueExponent = WP34S_extract_value(*(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)), &signExponent);
  int32_t odd = valueExponent & 1;

  if(shortIntegerMode == SIM_UNSIGN && odd) {
    setSystemFlag(FLAG_OVERFLOW);
  }
  else {
    clearSystemFlag(FLAG_OVERFLOW);
  }

  *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) = WP34S_build_value((uint64_t)1, odd);
}



void m1PowReal(void) {
  if(real34IsInfinite(REGISTER_REAL34_DATA(REGISTER_X))) {
    convertRealToReal34ResultRegister(const_NaN, REGISTER_X);
    setRegisterAngularMode(REGISTER_X, amNone);
    return;
  }

  real_t x;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);

  if(realIsAnInteger(&x)) {
    realDivideRemainder(&x, const_2, &x, &ctxtReal39);
    realCopy(realIsZero(&x) ? const_1 : const__1, &x);
  }
  else if(getSystemFlag(FLAG_CPXRES)) {
    real_t xi;
    realMultiply(const_pi, &x, &xi, &ctxtReal39);
    if(realIsAHalfInteger(&x)) {
      realDivideRemainder(&x, const_2, &x, &ctxtReal39);
      realCopy((realCompareAbsLessThan(&x, const_1) ^ realIsPositive(&x)) ? const__1 : const_1, &xi);
      realZero(&x);
    }
    else {
      WP34S_Cvt2RadSinCosTan(&xi, amRadian, &xi, &x, NULL, &ctxtReal39);
    }
    reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
    convertRealToReal34ResultRegister(&x, REGISTER_X);
    convertRealToImag34ResultRegister(&xi, REGISTER_X);
    return;
  }
  else if(realIsAHalfInteger(&x)) {
    realCopy(const_0, &x);
  }
  else {
    realMultiply(const_pi, &x, &x, &ctxtReal39);
    WP34S_Cvt2RadSinCosTan(&x, amRadian, NULL, &x, NULL, &ctxtReal39);
  }

  convertRealToReal34ResultRegister(&x, REGISTER_X);
  setRegisterAngularMode(REGISTER_X, amNone);
}



void m1PowCplx(void) {
  real_t real, imag;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &real);
  realDivideRemainder(&real, const_2, &imag, &ctxtReal39);
  bool realIsInt = realIsAnInteger(&real);
  bool realIsHalfInt = realIsAHalfInteger(&real);
  bool realIsOdd = realIsInt && (!realIsZero(&imag));
  bool realIs2np3on2 = realIsHalfInt && (realCompareAbsLessThan(&imag, const_1) ^ realIsPositive(&imag));
  realMultiply(const_pi, &real, &real, &ctxtReal39);

  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &imag);
  realMultiply(const_pi, &imag, &imag, &ctxtReal39);

  angularMode_t savedAngularMode = currentAngularMode;
  currentAngularMode = amRadian;

  if(realIsInt) {
    realDivideRemainder(&real, const_2, &real, &ctxtReal39);
    if(realIsZero(&imag)) {
      realCopy(realIsOdd ? const__1 : const_1, &real);
    }
    else {
      realChangeSign(&imag);
      realExp(&imag, &real, &ctxtReal39);
      if(realIsOdd) {
        realChangeSign(&real);
      }
      realZero(&imag);
    }
  }
  else if(realIsHalfInt) {
    if(realIsZero(&imag)) {
      realCopy(realIs2np3on2 ? const__1 : const_1, &imag);
    }
    else {
      realChangeSign(&imag);
      realExp(&imag, &imag, &ctxtReal39);
      if(realIs2np3on2) {
        realChangeSign(&imag);
      }
    }
    realZero(&real);
  }
  else {
    real_t sr, si;
    sinComplex(&real, &imag, &sr, &si, &ctxtReal39);
    cosComplex(&real, &imag, &real, &imag, &ctxtReal39);
    realSubtract(&real, &si, &real, &ctxtReal39);
    realAdd(&imag, &sr, &imag, &ctxtReal39);
  }
  convertRealToReal34ResultRegister(&real, REGISTER_X);
  convertRealToImag34ResultRegister(&imag, REGISTER_X);

  currentAngularMode = savedAngularMode;
}
