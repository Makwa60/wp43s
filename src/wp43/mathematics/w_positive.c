// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/w_positive.h"

#include "constantPointers.h"
#include "debug.h"
#include "defines.h"
#include "error.h"
#include "flags.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

void wPosLonI(void);
void wPosReal(void);
void wPosCplx(void);
#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void wPosError  (void);
#else // (EXTRA_INFO_ON_CALC_ERROR == 1)
  #define wPosError typeError
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

TO_QSPI void (* const WPositive[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1            2         3          4          5          6          7          8           9             10
//          Long integer Real34    Complex34  Time       Date       String     Real34 mat Complex34 m Short integer Config data
            wPosLonI,    wPosReal, wPosCplx,  wPosError, wPosError, wPosError, wPosError, wPosError,  wPosError,    wPosError
};

#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void wPosError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    sprintf(errorMessage, "cannot calculate Wp for %s", getRegisterDataTypeName(REGISTER_X, true, false));
    moreInfoOnError("In function fnWpositive:", errorMessage, NULL, NULL);
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



void fnWpositive(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  WPositive[getRegisterDataType(REGISTER_X)]();

  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}



void wPosLonI(void) {
  real_t x;

  convertLongIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);
  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE, amNone);
  convertRealToReal34ResultRegister(&x, REGISTER_X);
  wPosReal();
}



void wPosReal(void) {
  real_t x, res, resi, threshold;

  realDivide(const__1, const_eE, &threshold, &ctxtReal39);
  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);

  if(getRegisterAngularMode(REGISTER_X) == amNone) {
    if(realCompareGreaterEqual(&x, &threshold)) {
      WP34S_LambertW(&x, &res, false, &ctxtReal39);
      convertRealToReal34ResultRegister(&res, REGISTER_X);
    }
    else if(getSystemFlag(FLAG_CPXRES)) {
      WP34S_ComplexLambertW(&x, const_0, &res, &resi, &ctxtReal39);
      reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE, amNone);
      convertRealToReal34ResultRegister(&res, REGISTER_X);
      convertRealToImag34ResultRegister(&resi, REGISTER_X);
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function wPosReal:", "X < -e^(-1)", "and CPXRES is not set!", NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
  }
  else {
    wPosError();
  }
}



void wPosCplx(void) {
  real_t xr, xi, resr, resi;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xr);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xi);
  WP34S_ComplexLambertW(&xr, &xi, &resr, &resi, &ctxtReal39);
  convertRealToReal34ResultRegister(&resr, REGISTER_X);
  convertRealToImag34ResultRegister(&resi, REGISTER_X);
}
