// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/w_negative.h"

#include "constantPointers.h"
#include "debug.h"
#include "defines.h"
#include "error.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/matrix.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

void wNegLonI(void);
void wNegReal(void);
void wNegCplx(void);
#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void wNegError  (void);
#else // (EXTRA_INFO_ON_CALC_ERROR == 1)
  #define wNegError typeError
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

void wNegRema(void);
void wNegCxma(void);

TO_QSPI void (* const WNegative[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1            2         3          4          5          6          7          8           9             10
//          Long integer Real34    Complex34  Time       Date       String     Real34 mat Complex34 m Short integer Config data
            wNegLonI,    wNegReal, wNegCplx,  wNegError, wNegError, wNegError, wNegRema,  wNegCxma,   wNegError,    wNegError
};

#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void wNegError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    sprintf(errorMessage, "cannot calculate Wm for %s", getRegisterDataTypeName(REGISTER_X, true, false));
    moreInfoOnError("In function fnWNegative:", errorMessage, NULL, NULL);
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



void fnWnegative(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  WNegative[getRegisterDataType(REGISTER_X)]();

  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}



void wNegLonI(void) {
  real_t x;

  convertLongIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);
  reallocateRegister(REGISTER_X, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
  convertRealToReal34ResultRegister(&x, REGISTER_X);
  wNegReal();
}



void wNegReal(void) {
  real_t x, res, threshold;

  realDivide(const__1, const_eE, &threshold, &ctxtReal39);
  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);

  if(getRegisterAngularMode(REGISTER_X) == amNone) {
    if(realCompareGreaterEqual(&x, &threshold) && realCompareLessEqual(&x, const_0)) {
      WP34S_LambertW(&x, &res, true, &ctxtReal39);
      convertRealToReal34ResultRegister(&res, REGISTER_X);
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function wNegCplx:", "X < -e^(-1) || 0 < X", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
  }
  else {
    wNegError();
  }
}



void wNegCplx(void) {
  real_t xr, xi, res;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xr);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xi);
  if(realIsZero(&xi)) {
    wNegReal();
    convertRealToReal34ResultRegister(&res, REGISTER_X);
  }
  else {
    displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      moreInfoOnError("In function wNegCplx:", "Cannot calculate Wm for complex number with non-zero imaginary part", NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
  }
}



void wNegRema(void) {
  elementwiseRema(wNegReal);
}



void wNegCxma(void) {
  elementwiseCxma(wNegCplx);
}
