// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/conjugate.h"

#include "debug.h"
#include "error.h"
#include "flags.h"
#include "items.h"
#include "mathematics/matrix.h"
#include "registers.h"

#include "wp43.h"

#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void conjError  (void);
#else // (EXTRA_INFO_ON_CALC_ERROR == 1)
  #define conjError typeError
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

TO_QSPI void (* const conjugate[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1            2          3         4          5          6          7          8           9             10
//          Long integer Real34     Complex34 Time       Date       String     Real34 mat Complex34 m Short integer Config data
            conjError,   conjError, conjCplx, conjError, conjError, conjError, conjError, conjCxma,   conjError,    conjError
};

#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void conjError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot calculate conj for %s", getRegisterDataTypeName(REGISTER_X, true, false));
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



void fnConjugate(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  conjugate[getRegisterDataType(REGISTER_X)]();
}



void conjCxma(void) {
#if !defined(TESTSUITE_BUILD)
  complex34Matrix_t cMat;

  linkToComplexMatrixRegister(REGISTER_X, &cMat);

  for(uint16_t i = 0; i < cMat.header.matrixRows * cMat.header.matrixColumns; ++i) {
    real34ChangeSign(VARIABLE_IMAG34_DATA(&cMat.matrixElements[i]));
    if(real34IsZero(VARIABLE_IMAG34_DATA(&cMat.matrixElements[i])) && !getSystemFlag(FLAG_SPCRES)) {
      real34SetPositiveSign(VARIABLE_IMAG34_DATA(&cMat.matrixElements[i]));
    }
  }
#endif // !TESTSUITE_BUILD
}



void conjCplx(void) {
  real34ChangeSign(REGISTER_IMAG34_DATA(REGISTER_X));
  if(real34IsZero(REGISTER_IMAG34_DATA(REGISTER_X)) && !getSystemFlag(FLAG_SPCRES)) {
    real34SetPositiveSign(REGISTER_IMAG34_DATA(REGISTER_X));
  }
}
