// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/changeSign.h"

#include "debug.h"
#include "error.h"
#include "flags.h"
#include "integers.h"
#include "items.h"
#include "mathematics/matrix.h"
#include "registers.h"

#include "wp43.h"

#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void chsError    (void);
#else // (EXTRA_INFO_ON_CALC_ERROR == 1)
  #define chsError typeError
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

TO_QSPI void (* const chs[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1            2        3         4         5         6         7          8           9             10
//          Long integer Real34   Complex34 Time      Date      String    Real34 mat Complex34 m Short integer Config data
            chsLonI,     chsReal, chsCplx,  chsError, chsError, chsError, chsRema,   chsCxma,    chsShoI,      chsError
};

#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void chsError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot change the sign of %s", getRegisterDataTypeName(REGISTER_X, true, false));
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



void fnChangeSign(uint16_t unusedButMandatoryParameter) {
  //if(!saveLastX()) {
  //  return;
  //}

  chs[getRegisterDataType(REGISTER_X)]();
}



void chsLonI(void) {
  switch(getRegisterLongIntegerSign(REGISTER_X)) {
    case liPositive: {
      setRegisterLongIntegerSign(REGISTER_X, liNegative);
      break;
    }
    case liNegative: {
      setRegisterLongIntegerSign(REGISTER_X, liPositive);
      break;
    }
    default: {
    }
  }
}



void chsRema(void) {
  elementwiseRema(chsReal);
}



void chsCxma(void) {
  elementwiseCxma(chsCplx);
}



void chsShoI(void) {
  *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) = WP34S_intChs(*(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)));
}



void chsReal(void) {
  if(!getSystemFlag(FLAG_SPCRES) && real34IsInfinite(REGISTER_REAL34_DATA(REGISTER_X))) {
    displayCalcErrorMessage(real34IsPositive(REGISTER_REAL34_DATA(REGISTER_X)) ? ERROR_OVERFLOW_MINUS_INF : ERROR_OVERFLOW_PLUS_INF , ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot change infinity sign while D flag is clear");
    return;
  }

  real34ChangeSign(REGISTER_REAL34_DATA(REGISTER_X));

  if(real34IsZero(REGISTER_REAL34_DATA(REGISTER_X)) && !getSystemFlag(FLAG_SPCRES)) {
    real34SetPositiveSign(REGISTER_REAL34_DATA(REGISTER_X));
  }
}



void chsCplx(void) {
  if(!getSystemFlag(FLAG_SPCRES)) {
    if(real34IsInfinite(REGISTER_REAL34_DATA(REGISTER_X))) {
      displayCalcErrorMessage(real34IsPositive(REGISTER_REAL34_DATA(REGISTER_X)) ? ERROR_OVERFLOW_MINUS_INF : ERROR_OVERFLOW_PLUS_INF , ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("cannot change infinity sign of real part while D flag is clear");
      return;
    }

    if(real34IsInfinite(REGISTER_IMAG34_DATA(REGISTER_X))) {
      displayCalcErrorMessage(real34IsPositive(REGISTER_IMAG34_DATA(REGISTER_X)) ? ERROR_OVERFLOW_MINUS_INF : ERROR_OVERFLOW_PLUS_INF , ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("cannot change infinity sign of imaginary part while D flag is clear");
      return;
    }
  }

  complex34ChangeSign(REGISTER_COMPLEX34_DATA(REGISTER_X));

  if(real34IsZero(REGISTER_REAL34_DATA(REGISTER_X)) && !getSystemFlag(FLAG_SPCRES)) {
    real34SetPositiveSign(REGISTER_REAL34_DATA(REGISTER_X));
  }

  if(real34IsZero(REGISTER_IMAG34_DATA(REGISTER_X)) && !getSystemFlag(FLAG_SPCRES)) {
    real34SetPositiveSign(REGISTER_IMAG34_DATA(REGISTER_X));
  }
}
