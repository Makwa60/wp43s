// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/sin.h"

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

TO_QSPI void (* const Sin[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1            2        3         4         5         6         7          8           9             10
//          Long integer Real34   Complex34 Time      Date      String    Real34 mat Complex34 m Short integer Config data
            sinLonI,     sinReal, sinCplx,  sinError, sinError, sinError, sinRema,   sinCxma,    sinError,     sinError
};



/********************************************//**
 * \brief Data type error in sin
 *
 * \param void
 * \return void
 ***********************************************/
#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void sinError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    sprintf(errorMessage, "cannot calculate Sin for %s", getRegisterDataTypeName(REGISTER_X, true, false));
    moreInfoOnError("In function fnSin:", errorMessage, NULL, NULL);
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



/********************************************//**
 * \brief regX ==> regL and sin(regX) ==> regX
 * enables stack lift and refreshes the stack
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnSin(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  Sin[getRegisterDataType(REGISTER_X)]();

  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}



void real34Sin(const real34_t *x, real34_t *res) {
  real34_t angle, angle2, k, s, compare;
  int32_t i = 1;

  if(real34IsSpecial(x)) {
    realToReal34(const_NaN, res);
  }
  else {
    real34Copy(x, &angle);
    real34Multiply(&angle, &angle, &angle2);
    real34Copy(&angle, res);
    real34Copy(&angle, &k);
    real34Copy(&angle, &s);
    do {
      real34Multiply(&k, &angle2, &k);
      if(i < 100) {
        real34Multiply(&k, maclaurinCoeffSin34 + i, &k);
      }
      else {
        real34_t nx2p1;
        int32ToReal34(i * 2,     &nx2p1); real34Divide(&k, &nx2p1, &k);
        int32ToReal34(i * 2 + 1, &nx2p1); real34Divide(&k, &nx2p1, &k);
        real34ChangeSign(&k);
      }
      ++i;
      real34Copy(res, &s);
      real34Add(res, &k, res);
      real34Compare(res, &s, &compare);
    } while(!real34IsZero(&compare));
  }
}



void sinComplex(const real_t *real, const real_t *imag, real_t *resReal, real_t *resImag, realContext_t *realContext) {
  // sin(a + ib) = sin(a)*cosh(b) + i*cos(a)*sinh(b)
  real_t sina, cosa, sinhb, coshb;

  WP34S_Cvt2RadSinCosTan(real, amRadian, &sina, &cosa, NULL, realContext);
  WP34S_SinhCosh(imag, &sinhb, &coshb, realContext);

  realMultiply(&sina, &coshb, resReal, realContext);
  realMultiply(&cosa, &sinhb, resImag, realContext);
}



void sinLonI(void) {
  real_t x;

  if(currentAngularMode == amMultPi) {
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
    real34Zero(REGISTER_REAL34_DATA(REGISTER_X));
  }
  else {
    longIntegerAngleReduction(REGISTER_X, currentAngularMode, &x);
    WP34S_Cvt2RadSinCosTan(&x, currentAngularMode, &x, NULL, NULL,
      #if USE_REAL34_FUNCTIONS == 1
        getSystemFlag(FLAG_FASTFN) ? &ctxtReal34 :
      #endif // USE_REAL34_FUNCTIONS == 1
      &ctxtReal39
    );
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
    convertRealToReal34ResultRegister(&x, REGISTER_X);
  }
}



void sinRema(void) {
  elementwiseRema(sinReal);
}



void sinCxma(void) {
  elementwiseCxma(sinCplx);
}



void sinReal(void) {
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

    WP34S_Cvt2RadSinCosTan(&x, (xAngularMode == amNone ? currentAngularMode : xAngularMode), &x, NULL, NULL,
      #if USE_REAL34_FUNCTIONS == 1
        getSystemFlag(FLAG_FASTFN) ? &ctxtReal34 :
      #endif // USE_REAL34_FUNCTIONS == 1
      &ctxtReal39
    );
    convertRealToReal34ResultRegister(&x, REGISTER_X);
  }
  setRegisterAngularMode(REGISTER_X, amNone);
}



void sinCplx(void) {
  real_t zReal, zImag;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &zReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &zImag);

  sinComplex(&zReal, &zImag, &zReal, &zImag, &ctxtReal39);

  convertRealToReal34ResultRegister(&zReal, REGISTER_X);
  convertRealToImag34ResultRegister(&zImag, REGISTER_X);
}
