// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/cos.h"

#include "conversionAngles.h"
#include "constantPointers.h"
#include "debug.h"
#include "error.h"
#include "flags.h"
#include "items.h"
#include "lookupTables.h"
#include "mathematics/matrix.h"
#include "mathematics/tan.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void cosError(void);
#else // (EXTRA_INFO_ON_CALC_ERROR == 1)
  #define cosError typeError
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

TO_QSPI void (* const Cos[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1            2        3         4         5         6         7          8           9             10
//          Long integer Real34   Complex34 Time      Date      String    Real34 mat Complex34 m Short integer Config data
            cosLonI,     cosReal, cosCplx,  cosError, cosError, cosError, cosRema,   cosCxma,    cosError,     cosError
};

#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void cosError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot calculate Cos for %s", getRegisterDataTypeName(REGISTER_X, true, false));
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



void fnCos(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  Cos[getRegisterDataType(REGISTER_X)]();

  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}



void real34Cos(const real34_t *x, real34_t *res) {
  real34_t angle, angle2, k, s, compare;
  int32_t i = 1;

  if(real34IsSpecial(x)) {
    realToReal34(const_NaN, res);
  }
  else {
    real34Copy(x, &angle);
    real34Multiply(&angle, &angle, &angle2);
    real34Copy(const34_1, res);
    real34Copy(const34_1, &k);
    real34Copy(const34_1, &s);
    do {
      real34Multiply(&k, &angle2, &k);
      if(i < 100) {
        real34Multiply(&k, maclaurinCoeffCos34 + i, &k);
      }
      else {
        real34_t nx2p1;
        int32ToReal34(i * 2 - 1, &nx2p1); real34Divide(&k, &nx2p1, &k);
        int32ToReal34(i * 2,     &nx2p1); real34Divide(&k, &nx2p1, &k);
        real34ChangeSign(&k);
      }
      ++i;
      real34Copy(res, &s);
      real34Add(res, &k, res);
      real34Compare(res, &s, &compare);
    } while(!real34IsZero(&compare));
  }
}



void cosComplex(const real_t *real, const real_t *imag, real_t *resReal, real_t *resImag, realContext_t *realContext) {
  // cos(a + i b) = cos(a)*cosh(b) - i*sin(a)*sinh(b)
  real_t sina, cosa, sinhb, coshb;

  WP34S_Cvt2RadSinCosTan(real, amRadian, &sina, &cosa, NULL, realContext);
  WP34S_SinhCosh(imag, &sinhb, &coshb, realContext);

  realMultiply(&cosa, &coshb, resReal, realContext);
  realMultiply(&sina, &sinhb, resImag, realContext);
  realChangeSign(resImag);
}



void cosLonI(void) {
  real_t x;

  if(currentAngularMode == amMultPi) {
    longInteger_t lgInt;
    convertLongIntegerRegisterToLongInteger(REGISTER_X, lgInt);
    if(longIntegerIsEven(lgInt)) {
      reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
      real34Copy(const34_1, REGISTER_REAL34_DATA(REGISTER_X));
    }
    else {
      reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
      real34Copy(const34_1, REGISTER_REAL34_DATA(REGISTER_X));
      real34ChangeSign(REGISTER_REAL34_DATA(REGISTER_X));
    }
    longIntegerFree(lgInt);
  }
  else {
    longIntegerAngleReduction(REGISTER_X, currentAngularMode, &x);
    WP34S_Cvt2RadSinCosTan(&x, currentAngularMode, NULL, &x, NULL,
      #if USE_REAL34_FUNCTIONS == 1
        getSystemFlag(FLAG_FASTFN) ? &ctxtReal34 :
      #endif // USE_REAL34_FUNCTIONS == 1
      &ctxtReal39
    );
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
    convertRealToReal34ResultRegister(&x, REGISTER_X);
  }
}



void cosRema(void) {
  elementwiseRema(cosReal);
}



void cosCxma(void) {
  elementwiseCxma(cosCplx);
}



void cosReal(void) {
  if(real34IsInfinite(REGISTER_REAL34_DATA(REGISTER_X))) {
    convertRealToReal34ResultRegister(const_NaN, REGISTER_X);
  }
  else {
    real_t x;
    angularMode_t xAngularMode;

    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);
    xAngularMode = getRegisterAngularMode(REGISTER_X);

    if(xAngularMode == amNone && currentAngularMode == amMultPi) {
      realMultiply(&x, const_pi, &x, &ctxtReal39);
    }

    WP34S_Cvt2RadSinCosTan(&x, (xAngularMode == amNone ? currentAngularMode : xAngularMode), NULL, &x, NULL,
      #if USE_REAL34_FUNCTIONS == 1
        getSystemFlag(FLAG_FASTFN) ? &ctxtReal34 :
      #endif // USE_REAL34_FUNCTIONS == 1
      &ctxtReal39
    );
    convertRealToReal34ResultRegister(&x, REGISTER_X);
  }
  setRegisterAngularMode(REGISTER_X, amNone);
}



void cosCplx(void) {
  real_t zReal, zImag;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &zReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &zImag);

  cosComplex(&zReal, &zImag, &zReal, &zImag, &ctxtReal39);

  convertRealToReal34ResultRegister(&zReal, REGISTER_X);
  convertRealToImag34ResultRegister(&zImag, REGISTER_X);
}
