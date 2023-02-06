// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/cosh.h"

#include "constantPointers.h"
#include "debug.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "items.h"
#include "mathematics/matrix.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void coshError(void);
#else // (EXTRA_INFO_ON_CALC_ERROR == 1)
  #define coshError typeError
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

TO_QSPI void (* const Cosh[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1             2         3          4          5          6          7           8            9             10
//          Long integer  Real34    Complex34  Time       Date       String     Real34 mat  Complex34 m  Short integer Config data
            coshLonI,     coshReal, coshCplx,  coshError, coshError, coshError, coshRema,   coshCxma,    coshError,    coshError
};

#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void coshError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot calculate Cos for %s", getRegisterDataTypeName(REGISTER_X, true, false));
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



void fnCosh(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  Cosh[getRegisterDataType(REGISTER_X)]();

  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}



void coshLonI(void) {
#if USE_REAL34_FUNCTIONS == 1
  if(getSystemFlag(FLAG_FASTFN)) {
    convertLongIntegerRegisterToReal34Register(REGISTER_X, REGISTER_X);
    WP34S_SinhCosh34(REGISTER_REAL34_DATA(REGISTER_X), NULL, REGISTER_REAL34_DATA(REGISTER_X));
  }
  else
#endif // USE_REAL34_FUNCTIONS == 1
  {
    real_t cosh;

    convertLongIntegerRegisterToReal(REGISTER_X, &cosh, &ctxtReal39);
    WP34S_SinhCosh(&cosh, NULL, &cosh, &ctxtReal39);

    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
    convertRealToReal34ResultRegister(&cosh, REGISTER_X);
  }
}



void coshRema(void) {
  elementwiseRema(coshReal);
}



void coshCxma(void) {
  elementwiseCxma(coshCplx);
}



void coshReal(void) {
  if(real34IsInfinite(REGISTER_REAL34_DATA(REGISTER_X)) && !getSystemFlag(FLAG_SPCRES)) {
    displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot use " STD_PLUS_MINUS STD_INFINITY " as X input of cosh when flag D is not set");
    return;
  }

#if USE_REAL34_FUNCTIONS == 1
  if(getSystemFlag(FLAG_FASTFN)) {
    WP34S_SinhCosh34(REGISTER_REAL34_DATA(REGISTER_X), NULL, REGISTER_REAL34_DATA(REGISTER_X));
  }
  else
#endif // USE_REAL34_FUNCTIONS == 1
  {
    real_t x;

    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);
    WP34S_SinhCosh(&x, NULL, &x, &ctxtReal39);
    convertRealToReal34ResultRegister(&x, REGISTER_X);
  }
  setRegisterAngularMode(REGISTER_X, amNone);
}



void coshCplx(void) {
  // cosh(a + i b) = cosh(a) cos(b) + i sinh(a) sin(b)
  real_t a, b, sinha, cosha, sinb, cosb;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &a);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &b);

  WP34S_SinhCosh(&a, &sinha, &cosha, &ctxtReal39);
  WP34S_Cvt2RadSinCosTan(&b, amRadian, &sinb, &cosb, NULL, &ctxtReal39);

  realMultiply(&cosha, &cosb, &a, &ctxtReal39);
  realMultiply(&sinha, &sinb, &b, &ctxtReal39);

  convertRealToReal34ResultRegister(&a, REGISTER_X);
  convertRealToImag34ResultRegister(&b, REGISTER_X);
}
