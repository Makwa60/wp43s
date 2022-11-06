// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/arg.h"

#include "constantPointers.h"
#include "debug.h"
#include "defines.h"
#include "error.h"
#include "conversionAngles.h"
#include "mathematics/matrix.h"
#include "mathematics/toPolar.h"
#include "registerValueConversions.h"
#include "registers.h"

#include "wp43.h"

void argReal    (void);
void argCplx    (void);
void argCxma    (void);
#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void argError   (void);
#else // (EXTRA_INFO_ON_CALC_ERROR == 1)
  #define argError typeError
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

TO_QSPI void (* const arg[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1            2           3           4            5            6            7            8            9             10
//          Long integer Real34      Complex34   Time         Date         String       Real34 mat   Complex34 m  Short integer Config data
            argError,    argReal,    argCplx,    argError,    argError,    argError,    argError,    argCxma,     argError,     argError
};

#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void argError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot calculate arg for %s", getRegisterDataTypeName(REGISTER_X, true, false));
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



void fnArg(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  arg[getRegisterDataType(REGISTER_X)]();

  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}



void argReal(void) {
  if(real34IsNaN(REGISTER_REAL34_DATA(REGISTER_X))) {
    //let it stay NAN
  }
  else {
    if(real34IsPositive(REGISTER_REAL34_DATA(REGISTER_X)) || real34IsZero(REGISTER_REAL34_DATA(REGISTER_X))){
      reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
      convertRealToReal34ResultRegister(const_0, REGISTER_X);
      convertAngle34FromTo(REGISTER_REAL34_DATA(REGISTER_X), amDegree, currentAngularMode);
      setRegisterAngularMode(REGISTER_X, currentAngularMode);
    }
    else
      if(real34IsNegative(REGISTER_REAL34_DATA(REGISTER_X))) {
        reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
        convertRealToReal34ResultRegister(const_180, REGISTER_X);
        convertAngle34FromTo(REGISTER_REAL34_DATA(REGISTER_X), amDegree, currentAngularMode);
        setRegisterAngularMode(REGISTER_X, currentAngularMode);
      }
  }
}



void argCplx(void) {
  real_t real, imag;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &real);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &imag);
  realRectangularToPolar(&real, &imag, &real, &imag, &ctxtReal39);
  convertAngleFromTo(&imag, amRadian, currentAngularMode, &ctxtReal39);

  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, currentAngularMode);
  convertRealToReal34ResultRegister(&imag, REGISTER_X);
}



void argCxma(void) {
  #if !defined(TESTSUITE_BUILD)
    complex34Matrix_t cMat;
    real34Matrix_t rMat;
    real34_t dummy;

    linkToComplexMatrixRegister(REGISTER_X, &cMat);
    if(realMatrixInit(&rMat, cMat.header.matrixRows, cMat.header.matrixColumns)) {
      for(uint16_t i = 0; i < cMat.header.matrixRows * cMat.header.matrixColumns; ++i) {
        real34RectangularToPolar(VARIABLE_REAL34_DATA(&cMat.matrixElements[i]), VARIABLE_IMAG34_DATA(&cMat.matrixElements[i]), &dummy, &rMat.matrixElements[i]);
        convertAngle34FromTo(&rMat.matrixElements[i], amRadian, currentAngularMode);
      }

      convertReal34MatrixToReal34MatrixRegister(&rMat, REGISTER_X);
      realMatrixFree(&rMat);
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  #endif // !TESTSUITE_BUILD
}
