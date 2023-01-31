// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/exp.h"

#include "constantPointers.h"
#include "debug.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "items.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/matrix.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

TO_QSPI void (* const Exp[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1            2        3         4         5         6         7          8           9             10
//          Long integer Real34   Complex34 Time      Date      String    Real34 mat Complex34 m Short integer Config data
            expLonI,     expReal, expCplx,  expError, expError, expError, expRema,   expCxma,    expShoI,      expError
};



/********************************************//**
 * \brief Data type error in exp
 *
 * \param void
 * \return void
 ***********************************************/
#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void expError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    sprintf(errorMessage, "cannot calculate Exp for %s", getRegisterDataTypeName(REGISTER_X, true, false));
    moreInfoOnError("In function fnExp:", errorMessage, NULL, NULL);
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



/********************************************//**
 * \brief regX ==> regL and exp(regX) ==> regX
 * enables stack lift and refreshes the stack
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnExp(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  Exp[getRegisterDataType(REGISTER_X)]();

  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}



// Abramowitz and Stegun §4.2.1
void real34Exp(const real34_t *xin, real34_t *res) {
  real34_t x, xx, k, r, r0, r1;
  bool neg = real34IsNegative(xin);
  bool termOdd = true;

  real34CopyAbs(xin, &x);
  real34Copy(const34_1, res);
  real34Copy(const34_1, &k);
  real34Copy(const34_1, &xx);
  real34Copy(const34_1, &xx);
  real34Copy(const34_1, &r0);
  real34Copy(const34_0, &r1);

  // Taylor series
  do {
    real34Copy(res, &r);

    real34Multiply(&xx, &x, &xx);
    real34Divide(&xx, &k, &xx);
    real34Add(res, &xx, res);
    real34Add(termOdd ? &r1 : &r0, &xx, termOdd ? &r1 : &r0);
    real34Add(&k, const34_1, &k);
    termOdd = !termOdd;
  } while(!real34IsSpecial(res) && !real34CompareEqual(res, &r));
  real34Add(&r0, &r1, res);

  if(neg) {
    real34Divide(const34_1, res, res);
  }
}



void expComplex(const real_t *real, const real_t *imag, real_t *resReal, real_t *resImag, realContext_t *realContext) {
  real_t expa, sin, cos;

  if(realIsZero(imag)) {
   realExp(real, resReal, realContext);
   realZero(resImag);
   return;
  }

  if(realIsSpecial(real) || realIsSpecial(imag)) {
    realCopy(const_NaN, resReal);
    realCopy(const_NaN, resImag);
    return;
  }

 realExp(real, &expa, realContext);
 WP34S_Cvt2RadSinCosTan(imag, amRadian, &sin, &cos, NULL, realContext);
 realMultiply(&expa, &cos, resReal, realContext);
 realMultiply(&expa, &sin, resImag, realContext);
}



/**********************************************************************
 * In all the functions below:
 * if X is a number then X = a + ib
 * The variables a and b are used for intermediate calculations
 ***********************************************************************/

void expLonI(void) {
  #if USE_REAL34_FUNCTIONS == 1
    if(getSystemFlag(FLAG_FASTFN)) {
      convertLongIntegerRegisterToReal34Register(REGISTER_X, REGISTER_X);
      real34Exp(REGISTER_REAL34_DATA(REGISTER_X), REGISTER_REAL34_DATA(REGISTER_X));
      setRegisterAngularMode(REGISTER_X, amNone);
    }
    else
  #endif // USE_REAL34_FUNCTIONS == 1
  {
    real_t a;

    convertLongIntegerRegisterToReal(REGISTER_X, &a, &ctxtReal39);
    realExp(&a, &a, &ctxtReal39);
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
    convertRealToReal34ResultRegister(&a, REGISTER_X);
  }
}



void expRema(void) {
  elementwiseRema(expReal);
}



void expCxma(void) {
  elementwiseCxma(expCplx);
}



void expShoI(void) {
  #if USE_REAL34_FUNCTIONS == 1
    if(getSystemFlag(FLAG_FASTFN)) {
      convertShortIntegerRegisterToReal34Register(REGISTER_X, REGISTER_X);
      real34Exp(REGISTER_REAL34_DATA(REGISTER_X), REGISTER_REAL34_DATA(REGISTER_X));
      setRegisterAngularMode(REGISTER_X, amNone);
    }
    else
  #endif // USE_REAL34_FUNCTIONS == 1
  {
    real_t x;

    convertShortIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);
    realExp(&x, &x, &ctxtReal39);
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
    convertRealToReal34ResultRegister(&x, REGISTER_X);
  }
}



void expReal(void) {
  if(real34IsInfinite(REGISTER_REAL34_DATA(REGISTER_X)) && !getSystemFlag(FLAG_SPCRES)) {
    displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      moreInfoOnError("In function expReal:", "cannot use " STD_PLUS_MINUS STD_INFINITY " as X input of exp when flag D is not set", NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    return;
  }

  #if USE_REAL34_FUNCTIONS == 1
    if(getSystemFlag(FLAG_FASTFN)) {
      real34Exp(REGISTER_REAL34_DATA(REGISTER_X), REGISTER_REAL34_DATA(REGISTER_X));
    }
    else
  #endif // USE_REAL34_FUNCTIONS == 1
  {
    real_t x;

    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);
    realExp(&x, &x, &ctxtReal39);
    convertRealToReal34ResultRegister(&x, REGISTER_X);
  }
  setRegisterAngularMode(REGISTER_X, amNone);
}



void expCplx(void) {
  real_t zReal, zImag;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &zReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &zImag);

  expComplex(&zReal, &zImag, &zReal, &zImag, &ctxtReal39);

  convertRealToReal34ResultRegister(&zReal, REGISTER_X);
  convertRealToImag34ResultRegister(&zImag, REGISTER_X);
}
