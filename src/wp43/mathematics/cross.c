// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/cross.h"

#include "constantPointers.h"
#include "debug.h"
#include "error.h"
#include "fonts.h"
#include "items.h"
#include "mathematics/matrix.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

static void crossDataTypeError(void);

TO_QSPI void (* const cross[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS][NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX |    regY ==>    1                    2                    3                    4                    5                    6                    7                    8                    9                   10
//      V                Long integer         Real34               Complex34            Time                 Date                 String               Real34 mat           Complex34 mat        Short integer       Config data
/*  1 Long integer  */ { crossDataTypeError,  crossDataTypeError,  crossCplxLonI,       crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError, crossDataTypeError},
/*  2 Real34        */ { crossDataTypeError,  crossDataTypeError,  crossCplxReal,       crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError, crossDataTypeError},
/*  3 Complex34     */ { crossLonICplx,       crossRealCplx,       crossCplxCplx,       crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossShoICplx,      crossDataTypeError},
/*  4 Time          */ { crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError, crossDataTypeError},
/*  5 Date          */ { crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError, crossDataTypeError},
/*  6 String        */ { crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError, crossDataTypeError},
/*  7 Real34 mat    */ { crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossRemaRema,       crossCpmaRema,       crossDataTypeError, crossDataTypeError},
/*  8 Complex34 mat */ { crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossRemaCpma,       crossCpmaCpma,       crossDataTypeError, crossDataTypeError},
/*  9 Short integer */ { crossDataTypeError,  crossDataTypeError,  crossCplxShoI,       crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError, crossDataTypeError},
/* 10 Config data   */ { crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError,  crossDataTypeError, crossDataTypeError}
};

static void crossDataTypeError(void) {
  displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);

  errorMoreInfo("cannot raise %s\nto %s",
      getRegisterDataTypeName(REGISTER_Y, true, false),
      getRegisterDataTypeName(REGISTER_X, true, false));
}



void fnCross(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  cross[getRegisterDataType(REGISTER_X)][getRegisterDataType(REGISTER_Y)]();

  adjustResult(REGISTER_X, true, true, REGISTER_X, -1, -1);
}



static void crossCplx(real_t *xReal, real_t *xImag, real_t *yReal, real_t *yImag, real_t *rReal, realContext_t *realContext) {
  real_t t;

  realMultiply(xReal, yImag, &t, realContext);        // t = xReal * yImag
  realMultiply(yReal, xImag, rReal, realContext);         // r = yReal * xImag
  realSubtract(rReal, &t, rReal, realContext);   // r = r - t
}



void crossRealCplx(void) {
  real_t xReal, xImag, yReal, yImag;
  real_t rReal;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xImag);

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);
  real34ToReal(const34_0, &yImag);

  crossCplx(&xReal, &xImag, &yReal, &yImag, &rReal, &ctxtReal39);

  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
  convertRealToReal34ResultRegister(&rReal, REGISTER_X);
  setRegisterAngularMode(REGISTER_X, amNone);
}



void crossLonICplx(void) {
  real_t xReal, xImag, yReal, yImag;
  real_t rReal;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xImag);

  convertLongIntegerRegisterToReal(REGISTER_Y, &yReal, &ctxtReal39);
  real34ToReal(const34_0, &yImag);

  crossCplx(&xReal, &xImag, &yReal, &yImag, &rReal, &ctxtReal39);

  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
  convertRealToReal34ResultRegister(&rReal, REGISTER_X);
  setRegisterAngularMode(REGISTER_X, amNone);
}



void crossShoICplx(void) {
  real_t xReal, xImag, yReal, yImag;
  real_t rReal;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xImag);

  convertShortIntegerRegisterToReal(REGISTER_Y, &yReal, &ctxtReal39);
  real34ToReal(const34_0, &yImag);

  crossCplx(&xReal, &xImag, &yReal, &yImag, &rReal, &ctxtReal39);

  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
  convertRealToReal34ResultRegister(&rReal, REGISTER_X);
  setRegisterAngularMode(REGISTER_X, amNone);
}



void crossCplxCplx(void) {
  real_t xReal, xImag, yReal, yImag;
  real_t rReal;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xImag);

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_Y), &yImag);

  crossCplx(&xReal, &xImag, &yReal, &yImag, &rReal, &ctxtReal39);

  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
  convertRealToReal34ResultRegister(&rReal, REGISTER_X);
  setRegisterAngularMode(REGISTER_X, amNone);
}



void crossCplxReal(void) {
  real_t xReal, xImag, yReal, yImag;
  real_t rReal;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(const34_0, &xImag);

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_Y), &yImag);

  crossCplx(&xReal, &xImag, &yReal, &yImag, &rReal, &ctxtReal39);

  convertRealToReal34ResultRegister(&rReal, REGISTER_X);
  setRegisterAngularMode(REGISTER_X, amNone);
}



void crossCplxLonI(void) {
  real_t xReal, xImag, yReal, yImag;
  real_t rReal;

  convertLongIntegerRegisterToReal(REGISTER_X, &xReal, &ctxtReal39);
  real34ToReal(const34_0, &xImag);

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_Y), &yImag);

  crossCplx(&xReal, &xImag, &yReal, &yImag, &rReal, &ctxtReal39);

  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
  convertRealToReal34ResultRegister(&rReal, REGISTER_X);
  setRegisterAngularMode(REGISTER_X, amNone);
}



void crossCplxShoI(void) {
  real_t xReal, xImag, yReal, yImag;
  real_t rReal;

  convertShortIntegerRegisterToReal(REGISTER_X, &xReal, &ctxtReal39);
  real34ToReal(const34_0, &xImag);

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_Y), &yImag);

  crossCplx(&xReal, &xImag, &yReal, &yImag, &rReal, &ctxtReal39);

  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
  convertRealToReal34ResultRegister(&rReal, REGISTER_X);
  setRegisterAngularMode(REGISTER_X, amNone);
}



void crossRemaRema(void) {
  #if !defined(TESTSUITE_BUILD)
    real34Matrix_t y, x, res;

    linkToRealMatrixRegister(REGISTER_Y, &y);
    linkToRealMatrixRegister(REGISTER_X, &x);

    if((realVectorSize(&y) == 0) || (realVectorSize(&x) == 0) || (realVectorSize(&y) > 3) || (realVectorSize(&x) > 3)) {
      displayCalcErrorMessage(ERROR_MATRIX_MISMATCH, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("invalid numbers of elements of %d" STD_CROSS "%d-matrix to %d" STD_CROSS "%d-matrix",
          x.header.matrixRows, x.header.matrixColumns,
          y.header.matrixRows, y.header.matrixColumns);
    }
    else {
      crossRealVectors(&y, &x, &res);
      convertReal34MatrixToReal34MatrixRegister(&res, REGISTER_X);
    }
  #endif // !TESTSUITE_BUILD
}



void crossCpmaRema(void) {
  #if !defined(TESTSUITE_BUILD)
    convertReal34MatrixRegisterToComplex34MatrixRegister(REGISTER_X, REGISTER_X);
    crossCpmaCpma();
  #endif // !TESTSUITE_BUILD
}



void crossRemaCpma(void) {
  #if !defined(TESTSUITE_BUILD)
    convertReal34MatrixRegisterToComplex34MatrixRegister(REGISTER_Y, REGISTER_Y);
    crossCpmaCpma();
  #endif // !TESTSUITE_BUILD
}



void crossCpmaCpma(void) {
  #if !defined(TESTSUITE_BUILD)
    complex34Matrix_t y, x, res;

    linkToComplexMatrixRegister(REGISTER_Y, &y);
    linkToComplexMatrixRegister(REGISTER_X, &x);

    if((complexVectorSize(&y) == 0) || (complexVectorSize(&x) == 0) || (complexVectorSize(&y) > 3) || (complexVectorSize(&x) > 3)) {
      displayCalcErrorMessage(ERROR_MATRIX_MISMATCH, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("invalid numbers of elements of %d" STD_CROSS "%d-matrix to %d" STD_CROSS "%d-matrix",
          x.header.matrixRows, x.header.matrixColumns,
          y.header.matrixRows, y.header.matrixColumns);
    }
    else {
      crossComplexVectors(&y, &x, &res);
      convertComplex34MatrixToComplex34MatrixRegister(&res, REGISTER_X);
    }
  #endif // !TESTSUITE_BUILD
}
