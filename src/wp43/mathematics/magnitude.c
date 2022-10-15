// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/magnitude.h"

#include "debug.h"
#include "error.h"
#include "integers.h"
#include "items.h"
#include "matrix.h"
#include "registerValueConversions.h"
#include "registers.h"
#include "toPolar.h"

#include "wp43.h"

TO_QSPI void (* const magnitude[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1              2              3              4               5               6               7              8              9              10
//          Long integer   Real34         complex34      Time            Date            String          Real34 mat     Complex34 m    Short integer  Config data
            magnitudeLonI, magnitudeReal, magnitudeCplx, magnitudeError, magnitudeError, magnitudeError, magnitudeRema, magnitudeCxma, magnitudeShoI, magnitudeError
};



/********************************************//**
 * \brief Data type error in |x|
 *
 * \param void
 * \return void
 ***********************************************/
#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void magnitudeError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    sprintf(errorMessage, "cannot calculate |x| for %s", getRegisterDataTypeName(REGISTER_X, true, false));
    moreInfoOnError("In function fnMagnitude:", errorMessage, NULL, NULL);
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



/********************************************//**
 * \brief Returns the absolute value of an integer or a real and the magnitude of a complex
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnMagnitude(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  magnitude[getRegisterDataType(REGISTER_X)]();

  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}



void magnitudeLonI(void) {
  setRegisterLongIntegerSign(REGISTER_X, liPositive);
}



void magnitudeRema(void) {
  elementwiseRema(magnitudeReal);
}



void magnitudeCxma(void) {
  #if !defined(TESTSUITE_BUILD)
    complex34Matrix_t cMat;
    real34Matrix_t rMat;
    real34_t dummy;

    linkToComplexMatrixRegister(REGISTER_X, &cMat);
    if(realMatrixInit(&rMat, cMat.header.matrixRows, cMat.header.matrixColumns)) {
      for(uint16_t i = 0; i < cMat.header.matrixRows * cMat.header.matrixColumns; ++i) {
        real34RectangularToPolar(VARIABLE_REAL34_DATA(&cMat.matrixElements[i]), VARIABLE_IMAG34_DATA(&cMat.matrixElements[i]), &rMat.matrixElements[i], &dummy);
      }

      convertReal34MatrixToReal34MatrixRegister(&rMat, REGISTER_X);
      realMatrixFree(&rMat);
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  #endif // !TESTSUITE_BUILD
}



void magnitudeShoI(void) {
  *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) = WP34S_intAbs(*(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)));
}



void magnitudeReal(void) {
  real34SetPositiveSign(REGISTER_REAL34_DATA(REGISTER_X));
  setRegisterAngularMode(REGISTER_X, amNone);
}



void magnitudeCplx(void) {
  real_t a, b, c;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &a);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &b);
  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE, amNone);

  complexMagnitude(&a, &b, &c, &ctxtReal39);

  convertRealToReal34ResultRegister(&c, REGISTER_X);
}

void complexMagnitude(const real_t *a, const real_t *b, real_t *c, realContext_t *realContext) {
  realMultiply(a, a, c, realContext);
  realFMA(b, b, c, c, realContext);
  realSquareRoot(c, c, realContext);
}

