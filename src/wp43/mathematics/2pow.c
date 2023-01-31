// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/2pow.h"

#include "constantPointers.h"
#include "debug.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "integers.h"
#include "items.h"
#include "mathematics/exp.h"
#include "mathematics/matrix.h"
#include "mathematics/toRect.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

void twoPowLonI (void);
void twoPowRema (void);
void twoPowCxma (void);
void twoPowShoI (void);
void twoPowReal (void);
void twoPowCplx (void);
#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void twoPowError(void);
#else // (EXTRA_INFO_ON_CALC_ERROR == 1)
  #define twoPowError typeError
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

TO_QSPI void (* const twoPow[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1            2           3           4            5            6            7           8           9             10
//          Long integer Real34      Complex34   Time         Date         String       Real34 mat  Complex34 m Short integer Config data
            twoPowLonI,  twoPowReal, twoPowCplx, twoPowError, twoPowError, twoPowError, twoPowRema, twoPowCxma, twoPowShoI,   twoPowError
};

#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void twoPowError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot calculate 2" STD_SUP_x " for %s", getRegisterDataTypeName(REGISTER_X, true, false));
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



void fn2Pow(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  twoPow[getRegisterDataType(REGISTER_X)]();

  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}



void realPower2(const real_t *x, real_t *res, realContext_t *realContext) {
  realMultiply(x, const_ln2, res, realContext);
  realExp(res, res, realContext);
}



void real34Power2(const real34_t *x, real34_t *res) {
  real34Multiply(x, const34_ln2, res);
  real34Exp(res, res);
}



void twoPowLonI(void) {
  int32_t exponentSign;
  longInteger_t base, exponent;

  longIntegerInit(base);
  intToLongInteger(2, base);
  convertLongIntegerRegisterToLongInteger(REGISTER_X, exponent);

  longIntegerSetPositiveSign(base);

  exponentSign = longIntegerSign(exponent);
  longIntegerSetPositiveSign(exponent);

  if(longIntegerIsZero(exponent)) {
    uIntToLongInteger(1, base);
    convertLongIntegerToLongIntegerRegister(base, REGISTER_X);
    longIntegerFree(base);
    longIntegerFree(exponent);
    return;
  }
  else if(exponentSign == -1) {
    uIntToLongInteger(0, base);
    convertLongIntegerToLongIntegerRegister(base, REGISTER_X);
    longIntegerFree(base);
    longIntegerFree(exponent);
    return;
  }

  longInteger_t power;

  longIntegerInit(power);
  uIntToLongInteger(1, power);

  for(uint32_t i=0; !longIntegerIsZero(exponent) && lastErrorCode == 0; i++) {
    if(longIntegerIsOdd(exponent)) {
     longIntegerMultiply(power, base, power);
    }

    longIntegerDivideUInt(exponent, 2, exponent);

    if(!longIntegerIsZero(exponent)) {
      longIntegerSquare(base, base);
    }
  }

  convertLongIntegerToLongIntegerRegister(power, REGISTER_X);

  longIntegerFree(power);
  longIntegerFree(base);
  longIntegerFree(exponent);
}



void twoPowRema(void) {
  elementwiseRema(twoPowReal);
}



void twoPowCxma(void) {
  elementwiseCxma(twoPowCplx);
}



void twoPowShoI(void) {
  *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) = WP34S_int2pow(*(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)));
}



void twoPowReal(void) {
  if(real34IsInfinite(REGISTER_REAL34_DATA(REGISTER_X)) && !getSystemFlag(FLAG_SPCRES)) {
    displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot use " STD_PLUS_MINUS STD_INFINITY " as X input of 2" STD_SUP_x " when flag D is not set");
    return;
  }

  #if USE_REAL34_FUNCTIONS == 1
    if(1) {
      real34Power2(REGISTER_REAL34_DATA(REGISTER_X), REGISTER_REAL34_DATA(REGISTER_X));
    }
    else
  #endif // USE_REAL34_FUNCTIONS == 1
  {
    real_t x;

    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);
    realPower2(&x, &x, &ctxtReal39);
    convertRealToReal34ResultRegister(&x, REGISTER_X);
  }
  setRegisterAngularMode(REGISTER_X, amNone);
}



void twoPowCplx(void) {
  real_t a, b, factor;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &a);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &b);

  // ln(2) * (a + bi) --> (a + bi)
  realMultiply(const_ln2, &a, &a, &ctxtReal39);
  realMultiply(const_ln2, &b, &b, &ctxtReal39);

  // exp(ln(2) * (a + bi)) --> (a + bi)
  realExp(&a, &factor, &ctxtReal39);
  realPolarToRectangular(const_1, &b, &a, &b, &ctxtReal39);
  realMultiply(&factor, &a, &a, &ctxtReal39);
  realMultiply(&factor, &b, &b, &ctxtReal39);

  convertRealToReal34ResultRegister(&a, REGISTER_X);
  convertRealToImag34ResultRegister(&b, REGISTER_X);
}
