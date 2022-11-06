// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/cube.h"

#include "debug.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "integers.h"
#include "items.h"
#include "mathematics/multiplication.h"
#include "matrix.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void cubeError(void);
#else // (EXTRA_INFO_ON_CALC_ERROR == 1)
  #define cubeError typeError
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

TO_QSPI void (* const cube[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1            2         3         4          5          6          7          8           9             10
//          Long integer Real34    complex34 Time       Date       String     Real34 mat Complex34 m Short integer Config data
            cubeLonI,    cubeReal, cubeCplx, cubeError, cubeError, cubeError, cubeRema,  cubeCxma,   cubeShoI,     cubeError
};

#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void cubeError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot cube %s", getRegisterDataTypeName(REGISTER_X, true, false));
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



void fnCube(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  cube[getRegisterDataType(REGISTER_X)]();

  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}



void cubeLonI(void) {
  longInteger_t x, c;

  convertLongIntegerRegisterToLongInteger(REGISTER_X, x);
  longIntegerInit(c);
  longIntegerMultiply(x, x, c);
  longIntegerMultiply(c, x, c);
  convertLongIntegerToLongIntegerRegister(c, REGISTER_X);
  longIntegerFree(x);
  longIntegerFree(c);
}



void cubeRema(void) {
  elementwiseRema(cubeReal);
}



void cubeCxma(void) {
  elementwiseCxma(cubeCplx);
}



void cubeShoI(void) {
  uint64_t square;
  square = WP34S_intMultiply(*(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)), *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)));
  *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) = WP34S_intMultiply(square, *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)));
}



void cubeReal(void) {
  if(real34IsInfinite(REGISTER_REAL34_DATA(REGISTER_X)) && !getSystemFlag(FLAG_SPCRES)) {
    displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot use " STD_PLUS_MINUS STD_INFINITY " as X input of ^3 when flag D is not set");
    return;
  }

  real_t x, xSquared;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);

  realMultiply(&x, &x, &xSquared, &ctxtReal39);
  realMultiply(&xSquared, &x, &x, &ctxtReal39);

  convertRealToReal34ResultRegister(&x, REGISTER_X);
  setRegisterAngularMode(REGISTER_X, amNone);
}



void cubeCplx(void) {
  real_t a, b, realSquare, imagSquare;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &a);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &b);

  mulComplexComplex(&a, &b, &a, &b, &realSquare, &imagSquare, &ctxtReal39);
  mulComplexComplex(&realSquare, &imagSquare, &a, &b, &a, &b, &ctxtReal39);

  convertRealToReal34ResultRegister(&a, REGISTER_X);
  convertRealToImag34ResultRegister(&b, REGISTER_X);
}
