// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/beta.h"

#include "constantPointers.h"
#include "debug.h"
#include "error.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/division.h"
#include "mathematics/multiplication.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"
#include <stdbool.h>

#include "wp43.h"

TO_QSPI void (* const beta[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS][NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])() = {
// regX |    regY ==>    1             2             3             4          5          6          7          8          9          10
//      V                Long integer  Real34        Complex34     Time       Date       String     Real34 mat Complex34  mat Short  Config data
/*  1 Long integer  */ { betaLonILonI, betaRealLonI, betaCplxLonI, betaError, betaError, betaError, betaError, betaError, betaError, betaError },
/*  2 Real34        */ { betaLonIReal, betaRealReal, betaCplxReal, betaError, betaError, betaError, betaError, betaError, betaError, betaError },
/*  3 Complex34     */ { betaLonICplx, betaRealCplx, betaCplxCplx, betaError, betaError, betaError, betaError, betaError, betaError, betaError },
/*  4 Time          */ { betaError,    betaError,    betaError,    betaError, betaError, betaError, betaError, betaError, betaError, betaError },
/*  5 Date          */ { betaError,    betaError,    betaError,    betaError, betaError, betaError, betaError, betaError, betaError, betaError },
/*  6 String        */ { betaError,    betaError,    betaError,    betaError, betaError, betaError, betaError, betaError, betaError, betaError },
/*  7 Real34 mat    */ { betaError,    betaError,    betaError,    betaError, betaError, betaError, betaError, betaError, betaError, betaError },
/*  8 Complex34 mat */ { betaError,    betaError,    betaError,    betaError, betaError, betaError, betaError, betaError, betaError, betaError },
/*  9 Short integer */ { betaError,    betaError,    betaError,    betaError, betaError, betaError, betaError, betaError, betaError, betaError },
/* 10 Config data   */ { betaError,    betaError,    betaError,    betaError, betaError, betaError, betaError, betaError, betaError, betaError }
};

/********************************************//**
 * \brief Data type error in beta
 *
 * \param void
 * \return void
 ***********************************************/
#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void betaError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    sprintf(errorMessage, "cannot calculate Beta of (%s, %s)", getRegisterDataTypeName(REGISTER_Y, true, false), getRegisterDataTypeName(REGISTER_X, true, false));
    moreInfoOnError("In function fnBeta:", errorMessage, NULL, NULL);
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



/********************************************//**
 * \brief regX ==> regL and beta(regX, RegY) ==> regX
 * enables stack lift and refreshes the stack
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnBeta(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  beta[getRegisterDataType(REGISTER_X)][getRegisterDataType(REGISTER_Y)]();

  adjustResult(REGISTER_X, true, true, REGISTER_X, -1, -1);
}


static bool _beta(real_t *xReal, real_t *xImag, real_t *yReal, real_t *yImag, real_t *rReal, real_t *rImag, realContext_t *realContext) {
  // Beta(x, y) := Gamma(x) * Gamma(y) / Gamma(x+y)
  real_t tReal, tImag;

  if(realCompareLessEqual(xReal, const_0)) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "cannot calculate Beta of (%s, %s) with Re(x)<=0", getRegisterDataTypeName(REGISTER_Y, true, false), getRegisterDataTypeName(REGISTER_X, true, false));
      moreInfoOnError("In function fnBeta:", errorMessage, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    return false;
  }
  else if(realCompareLessEqual(yReal, const_0)) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "cannot calculate Beta of (%s, %s with Re(y)<=0", getRegisterDataTypeName(REGISTER_Y, true, false), getRegisterDataTypeName(REGISTER_X, true, false));
      moreInfoOnError("In function fnBeta:", errorMessage, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    return false;
  }

  WP34S_ComplexGamma(xReal, xImag, &tReal, &tImag, realContext);              // t = Gamma(x)
  WP34S_ComplexGamma(yReal, yImag, rReal, rImag, realContext);                // r = Gamma(y)

  mulComplexComplex(rReal, rImag, &tReal, &tImag, rReal, rImag, realContext); // r = Gamma(x) * Gamma(y)

  realAdd(xReal, yReal, &tReal, realContext);                             // t = x + y
  realAdd(xImag, yImag, &tImag, realContext);

  WP34S_ComplexGamma(&tReal, &tImag, &tReal, &tImag, realContext);            // t = Gamma(x + y);
  divComplexComplex(rReal, rImag, &tReal, &tImag, rReal, rImag, realContext); // r = Gamma(x) * Gamma(y) / Gamma(x + y);

  if(realIsNaN(rImag) || realIsNaN(rReal)) {
    displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "cannot calculate Beta of (%s, %s) out of range", getRegisterDataTypeName(REGISTER_Y, true, false), getRegisterDataTypeName(REGISTER_X, true, false));
      moreInfoOnError("In function fnBeta:", errorMessage, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    return false;
  }

  return true;
}

static void _betaComplex(real_t *xReal, real_t *xImag, real_t *yReal, real_t *yImag, realContext_t *realContext) {
  real_t rReal, rImag;

  if(_beta(xReal, xImag, yReal, yImag, &rReal, &rImag, realContext)) {
    reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BLOCKS, amNone);
    convertRealToReal34ResultRegister(&rReal, REGISTER_X);
    convertRealToImag34ResultRegister(&rImag, REGISTER_X);
  }
}

static void _betaReal(real_t *xReal, real_t *yReal, realContext_t *realContext) {
  real_t rReal, rImag;
  real_t xImag, yImag;

  real34ToReal(const34_0, &xImag);
  real34ToReal(const34_0, &yImag);

  if(_beta(xReal, &xImag, yReal, &yImag, &rReal, &rImag, realContext)) {
    if(realIsZero(&rImag)) {
      reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BLOCKS, amNone);
      convertRealToReal34ResultRegister(&rReal, REGISTER_X);
    }
    else {
      reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BLOCKS, amNone);
      convertRealToReal34ResultRegister(&rReal, REGISTER_X);
      convertRealToImag34ResultRegister(&rImag, REGISTER_X);
    }
  }
}



void betaLonILonI(void) {
  real_t x, y;

  convertLongIntegerRegisterToReal(REGISTER_Y, &y, &ctxtReal39);
  convertLongIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);

  _betaReal(&x, &y, &ctxtReal39);
}



void betaRealLonI(void) {
  real_t x, y;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &y);
  convertLongIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);

  _betaReal(&x, &y, &ctxtReal39);
}



void betaCplxLonI(void) {
  real_t xReal, xImag, yReal, yImag;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_Y), &yImag);

  convertLongIntegerRegisterToReal(REGISTER_X, &xReal, &ctxtReal39);
  real34ToReal(const34_0, &xImag);

  _betaComplex(&xReal, &xImag, &yImag, &yImag, &ctxtReal39);
}



void betaLonIReal(void) {
  real_t x, y;

  convertLongIntegerRegisterToReal(REGISTER_Y, &y, &ctxtReal39);
  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);

  _betaReal(&x, &y, &ctxtReal39);
}



void betaRealReal(void) {
  real_t x, y;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &y);
  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);

  _betaReal(&x, &y, &ctxtReal39);
}



void betaCplxReal(void) {
  real_t xReal, xImag, yReal, yImag;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_Y), &yImag);

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(const34_0, &xImag);

  _betaComplex(&xReal, &xImag, &yImag, &yImag, &ctxtReal39);
}



void betaLonICplx(void) {
  real_t xReal, xImag, yReal, yImag;

  convertLongIntegerRegisterToReal(REGISTER_Y, &yReal, &ctxtReal39);
  real34ToReal(const34_0, &yImag);

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xImag);

  _betaComplex(&xReal, &xImag, &yReal, &yImag, &ctxtReal39);
}



void betaRealCplx(void)  {
  real_t xReal, xImag, yReal, yImag;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);
  real34ToReal(const34_0, &yImag);

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xImag);

  _betaComplex(&xReal, &xImag, &yReal, &yImag, &ctxtReal39);
}



void betaCplxCplx(void)  {
  real_t xReal, xImag, yReal, yImag;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_Y), &yImag);

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xImag);

  _betaComplex(&xReal, &xImag, &yReal, &yImag, &ctxtReal39);
}
