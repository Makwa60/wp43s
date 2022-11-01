// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/w_inverse.h"

#include "constantPointers.h"
#include "debug.h"
#include "defines.h"
#include "error.h"
#include "fonts.h"
#include "mathematics/wp34s.h"
#include "matrix.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

void wInvLonI   (void);
void wInvReal   (void);
void wInvCplx   (void);
void wInvRema   (void);
void wInvCxma   (void);

#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void wInvError  (void);
#else // (EXTRA_INFO_ON_CALC_ERROR == 1)
  #define wInvError typeError
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

TO_QSPI void (* const WInverse[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1            2         3          4          5          6          7          8           9             10
//          Long integer Real34    Complex34  Time       Date       String     Real34 mat Complex34 m Short integer Config data
            wInvLonI,    wInvReal, wInvCplx,  wInvError, wInvError, wInvError, wInvRema,  wInvCxma,   wInvError,    wInvError
};

#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void wInvError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    sprintf(errorMessage, "cannot calculate Wp for %s", getRegisterDataTypeName(REGISTER_X, true, false));
    moreInfoOnError("In function fnWinverse:", errorMessage, NULL, NULL);
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



void fnWinverse(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  WInverse[getRegisterDataType(REGISTER_X)]();

  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}



void wInvLonI(void) {
  real_t x;

  convertLongIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);
  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BLOCKS, amNone);
  convertRealToReal34ResultRegister(&x, REGISTER_X);
  wInvReal();
}



void wInvReal(void) {
  real_t x, res, threshold;

  realDivide(const__1, const_eE, &threshold, &ctxtReal39);
  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);

  if(getRegisterAngularMode(REGISTER_X) == amNone) {
    WP34S_InverseW(&x, &res, &ctxtReal39);
    convertRealToReal34ResultRegister(&res, REGISTER_X);
  }
  else {
    wInvError();
  }
}



void wInvCplx(void) {
  real_t xr, xi, resr, resi;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xr);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xi);
  WP34S_InverseComplexW(&xr, &xi, &resr, &resi, &ctxtReal39);
  convertRealToReal34ResultRegister(&resr, REGISTER_X);
  convertRealToImag34ResultRegister(&resi, REGISTER_X);
}



void wInvRema(void) {
  elementwiseRema(wInvReal);
}



void wInvCxma(void) {
  elementwiseCxma(wInvCplx);
}
