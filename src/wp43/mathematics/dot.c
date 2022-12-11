// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/dot.h"

#include "constantPointers.h"
#include "debug.h"
#include "error.h"
#include "fonts.h"
#include "items.h"
#include "mathematics/matrix.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

static void dotDataTypeError(void);

TO_QSPI void (* const dot[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS][NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX |    regY ==>    1                  2                  3                  4                  5                  6                  7                  8                  9                 10
//      V                Long integer       Real34             Complex34          Time               Date               String             Real34 mat         Complex34 mat      Short integer     Config data
/*  1 Long integer  */ { dotDataTypeError,  dotDataTypeError,  dotCplxLonI,       dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError, dotDataTypeError},
/*  2 Real34        */ { dotDataTypeError,  dotDataTypeError,  dotCplxReal,       dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError, dotDataTypeError},
/*  3 Complex34     */ { dotLonICplx,       dotRealCplx,       dotCplxCplx,       dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotShoICplx,      dotDataTypeError},
/*  4 Time          */ { dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError, dotDataTypeError},
/*  5 Date          */ { dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError, dotDataTypeError},
/*  6 String        */ { dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError, dotDataTypeError},
/*  7 Real34 mat    */ { dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotRemaRema,       dotCpmaRema,       dotDataTypeError, dotDataTypeError},
/*  8 Complex34 mat */ { dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotRemaCpma,       dotCpmaCpma,       dotDataTypeError, dotDataTypeError},
/*  9 Short integer */ { dotDataTypeError,  dotDataTypeError,  dotCplxShoI,       dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError, dotDataTypeError},
/* 10 Config data   */ { dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError,  dotDataTypeError, dotDataTypeError}
};

static void dotDataTypeError(void) {
  displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);

  errorMoreInfo("cannot raise %s\nto %s",
      getRegisterDataTypeName(REGISTER_Y, true, false),
      getRegisterDataTypeName(REGISTER_X, true, false));
}



void fnDot(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  dot[getRegisterDataType(REGISTER_X)][getRegisterDataType(REGISTER_Y)]();

  adjustResult(REGISTER_X, true, true, REGISTER_X, -1, -1);
}



static void dotCplx(real_t *xReal, real_t *xImag, real_t *yReal, real_t *yImag, real_t *rReal, realContext_t *realContext) {
  real_t t;

  realMultiply(xReal, yReal, &t, realContext);     // t = xReal * yReal
  realMultiply(xImag, yImag, rReal, realContext);      // r = xImag * yImag
  realAdd(&t, rReal, rReal, realContext);     // r = r + t
}



void dotRealCplx(void) {
  real_t xReal, xImag, yReal, yImag;
  real_t rReal;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xImag);

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);
  real34ToReal(const34_0, &yImag);

  dotCplx(&xReal, &xImag, &yReal, &yImag, &rReal, &ctxtReal39);

  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
  convertRealToReal34ResultRegister(&rReal, REGISTER_X);
  setRegisterAngularMode(REGISTER_X, amNone);
}



void dotLonICplx(void) {
  real_t xReal, xImag, yReal, yImag;
  real_t rReal;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xImag);

  convertLongIntegerRegisterToReal(REGISTER_Y, &yReal, &ctxtReal39);
  real34ToReal(const34_0, &yImag);

  dotCplx(&xReal, &xImag, &yReal, &yImag, &rReal, &ctxtReal39);

  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
  convertRealToReal34ResultRegister(&rReal, REGISTER_X);
  setRegisterAngularMode(REGISTER_X, amNone);
}



void dotShoICplx(void) {
  real_t xReal, xImag, yReal, yImag;
  real_t rReal;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xImag);

  convertShortIntegerRegisterToReal(REGISTER_Y, &yReal, &ctxtReal39);
  real34ToReal(const34_0, &yImag);

  dotCplx(&xReal, &xImag, &yReal, &yImag, &rReal, &ctxtReal39);

  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
  convertRealToReal34ResultRegister(&rReal, REGISTER_X);
  setRegisterAngularMode(REGISTER_X, amNone);
}



void dotCplxCplx(void) {
  real_t xReal, xImag, yReal, yImag;
  real_t rReal;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xImag);

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_Y), &yImag);

  dotCplx(&xReal, &xImag, &yReal, &yImag, &rReal, &ctxtReal39);

  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
  convertRealToReal34ResultRegister(&rReal, REGISTER_X);
  setRegisterAngularMode(REGISTER_X, amNone);
}



void dotCplxReal(void) {
  real_t xReal, xImag, yReal, yImag;
  real_t rReal;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(const34_0, &xImag);

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_Y), &yImag);

  dotCplx(&xReal, &xImag, &yReal, &yImag, &rReal, &ctxtReal39);

  convertRealToReal34ResultRegister(&rReal, REGISTER_X);
  setRegisterAngularMode(REGISTER_X, amNone);
}



void dotCplxLonI(void) {
  real_t xReal, xImag, yReal, yImag;
  real_t rReal;

  convertLongIntegerRegisterToReal(REGISTER_X, &xReal, &ctxtReal39);
  real34ToReal(const34_0, &xImag);

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_Y), &yImag);

  dotCplx(&xReal, &xImag, &yReal, &yImag, &rReal, &ctxtReal39);

  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
  convertRealToReal34ResultRegister(&rReal, REGISTER_X);
  setRegisterAngularMode(REGISTER_X, amNone);
}



void dotCplxShoI(void) {
  real_t xReal, xImag, yReal, yImag;
  real_t rReal;

  convertShortIntegerRegisterToReal(REGISTER_X, &xReal, &ctxtReal39);
  real34ToReal(const34_0, &xImag);

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_Y), &yImag);

  dotCplx(&xReal, &xImag, &yReal, &yImag, &rReal, &ctxtReal39);

  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
  convertRealToReal34ResultRegister(&rReal, REGISTER_X);
  setRegisterAngularMode(REGISTER_X, amNone);
}



void dotRemaRema(void) {
  #if !defined(TESTSUITE_BUILD)
    real34Matrix_t y, x;
    real34_t res;

    linkToRealMatrixRegister(REGISTER_Y, &y);
    linkToRealMatrixRegister(REGISTER_X, &x);

    if((realVectorSize(&y) == 0) || (realVectorSize(&x) == 0) || (realVectorSize(&y) != realVectorSize(&x))) {
      displayCalcErrorMessage(ERROR_MATRIX_MISMATCH, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("numbers of elements of %d" STD_CROSS "%d-matrix to %d" STD_CROSS "%d-matrix mismatch",
          x.header.matrixRows, x.header.matrixColumns,
          y.header.matrixRows, y.header.matrixColumns);
    }
    else {
      dotRealVectors(&y, &x, &res);
      reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
      real34Copy(&res, REGISTER_REAL34_DATA(REGISTER_X));
    }
  #endif // !TESTSUITE_BUILD
}



void dotCpmaRema(void) {
  #if !defined(TESTSUITE_BUILD)
    convertReal34MatrixRegisterToComplex34MatrixRegister(REGISTER_X, REGISTER_X);
    dotCpmaCpma();
  #endif // !TESTSUITE_BUILD
}



void dotRemaCpma(void) {
  #if !defined(TESTSUITE_BUILD)
    convertReal34MatrixRegisterToComplex34MatrixRegister(REGISTER_Y, REGISTER_Y);
    dotCpmaCpma();
  #endif // !TESTSUITE_BUILD
}



void dotCpmaCpma(void) {
  #if !defined(TESTSUITE_BUILD)
    complex34Matrix_t y, x;
    real34_t res_r, res_i;

    linkToComplexMatrixRegister(REGISTER_Y, &y);
    linkToComplexMatrixRegister(REGISTER_X, &x);

    if((complexVectorSize(&y) == 0) || (complexVectorSize(&x) == 0) || (complexVectorSize(&y) != complexVectorSize(&x))) {
      displayCalcErrorMessage(ERROR_MATRIX_MISMATCH, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("numbers of elements of %d" STD_CROSS "%d-matrix to %d" STD_CROSS "%d-matrix mismatch",
          x.header.matrixRows, x.header.matrixColumns,
          y.header.matrixRows, y.header.matrixColumns);
    }
    else {
      dotComplexVectors(&y, &x, &res_r, &res_i);
      reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
      real34Copy(&res_r, REGISTER_REAL34_DATA(REGISTER_X));
      real34Copy(&res_i, REGISTER_IMAG34_DATA(REGISTER_X));
    }
  #endif // !TESTSUITE_BUILD
}
