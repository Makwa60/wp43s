// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/expMOne.h"

#include "constantPointers.h"
#include "debug.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "items.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/exp.h"
#include "mathematics/matrix.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

TO_QSPI void (* const ExpM1[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1            2          3           4           5           6           7            8             9               10
//          Long integer Real34     Complex34   Time        Date        String      Real34 mat   Complex34 m   Short integer   Config data
            expM1LonI,   expM1Real, expM1Cplx,  expM1Error, expM1Error, expM1Error, expM1Rema,   expM1Cxma,    expM1ShoI,      expM1Error
};



/********************************************//**
 * \brief Data type error in expM1
 *
 * \param void
 * \return void
 ***********************************************/
#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void expM1Error(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    sprintf(errorMessage, "cannot calculate Exp(x)-1 for %s", getRegisterDataTypeName(REGISTER_X, true, false));
    moreInfoOnError("In function fnExpM1:", errorMessage, NULL, NULL);
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



/********************************************//**
 * \brief regX ==> regL and expM1(regX) ==> regX
 * enables stack lift and refreshes the stack
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnExpM1(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  ExpM1[getRegisterDataType(REGISTER_X)]();

  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}






void realExpM1(const real_t *xin, real_t *res, realContext_t *realContext) {
  real_t x, xx, k, r;

  if(!realCompareAbsLessThan(xin, const_1)) {
    realExp(xin, res, realContext);
    realSubtract(res, const_1, res, realContext);
    return;
  }

  realCopy(xin, &x);
  realCopy(const_0, res);
  realCopy(const_1, &k);
  realCopy(const_1, &xx);

  // Taylor series
  do {
    realCopy(res, &r);

    realMultiply(&xx, &x, &xx, realContext);
    realDivide(&xx, &k, &xx, realContext);
    realAdd(res, &xx, res, realContext);
    realAdd(&k, const_1, &k, realContext);
  } while(!realIsSpecial(res) && !realCompareEqual(res, &r));
}



void real34ExpM1(const real34_t *xin, real34_t *res) {
  real34_t x, xx, k, r, r0, r1;
  bool termOdd = true;

  if(!real34CompareAbsLessThan(xin, const34_1)) {
    real34Exp(xin, res);
    real34Subtract(res, const34_1, res);
    return;
  }

  real34Copy(xin, &x);
  real34Copy(const34_0, res);
  real34Copy(const34_1, &k);
  real34Copy(const34_1, &xx);
  real34Copy(const34_0, &r0);
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
}



void expM1Complex(const real_t *real, const real_t *imag, real_t *resReal, real_t *resImag, realContext_t *realContext) {
  real_t expa, sin, cos;

  if(realIsZero(imag)) {
    if(realIsInfinite(real) && realIsNegative(real)) {
      realCopy(const__1, resReal);
      realZero(resImag);
      return;
    }
    if(realCompareAbsLessThan(real, const_1)) {
      WP34S_ExpM1(real, resReal, realContext);
      realZero(resImag);
      return;
    }
    realExp(real, resReal, realContext);
    realSubtract(resReal, const_1, resReal, realContext);
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
  realSubtract(resReal, const_1, resReal, realContext);
}



/**********************************************************************
 * In all the functions below:
 * if X is a number then X = a + ib
 * The variables a and b are used for intermediate calculations
 * 1 is subtracted at the end
 ***********************************************************************/

void expM1LonI(void) {
  #if USE_REAL34_FUNCTIONS == 1
  if(1) {
    convertLongIntegerRegisterToReal34Register(REGISTER_X, REGISTER_X);
    real34ExpM1(REGISTER_REAL34_DATA(REGISTER_X), REGISTER_REAL34_DATA(REGISTER_X));
    setRegisterAngularMode(REGISTER_X, amNone);
  }
  else
  #endif // USE_REAL34_FUNCTIONS == 1
  {
    real_t a;

    convertLongIntegerRegisterToReal(REGISTER_X, &a, &ctxtReal39);
    realExp(&a, &a, &ctxtReal39);
    realSubtract(&a, const_1, &a, &ctxtReal39);
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
    convertRealToReal34ResultRegister(&a, REGISTER_X);
  }
}



void expM1Rema(void) {
  elementwiseRema(expM1Real);
}



void expM1Cxma(void) {
  elementwiseCxma(expM1Cplx);
}



void expM1ShoI(void) {
  real_t x;

  convertShortIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);
  realExp(&x, &x, &ctxtReal39);
  realSubtract(&x, const_1, &x, &ctxtReal39);
  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
  convertRealToReal34ResultRegister(&x, REGISTER_X);
}



void expM1Real(void) {
  if(real34IsInfinite(REGISTER_REAL34_DATA(REGISTER_X)) && !getSystemFlag(FLAG_SPCRES)) {
    displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      moreInfoOnError("In function expM1Real:", "cannot use " STD_PLUS_MINUS STD_INFINITY " as X input of exp when flag D is not set", NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    return;
  }

  real_t x;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);

  if(realIsInfinite(&x) && realIsNegative(&x)) {
    realCopy(const__1, &x);
  }
  else if(realIsInfinite(&x) && realIsPositive(&x)) {
    realCopy(const_plusInfinity, &x);
  }
  else if(realIsSpecial(&x)) {
    realCopy(const_NaN, &x);
  }
  #if USE_REAL34_FUNCTIONS == 1
  else if(1) {
    real34ExpM1(REGISTER_REAL34_DATA(REGISTER_X), REGISTER_REAL34_DATA(REGISTER_X));
    setRegisterAngularMode(REGISTER_X, amNone);
    return;
  }
  #endif // USE_REAL34_FUNCTIONS == 1
  else if(realCompareAbsLessThan(&x, const_1)) {
    realExpM1(&x, &x, &ctxtReal51);
  }
  else {
    realExp(&x, &x, &ctxtReal51);
    realSubtract(&x, const_1, &x, &ctxtReal51);
  }
  convertRealToReal34ResultRegister(&x, REGISTER_X);
  setRegisterAngularMode(REGISTER_X, amNone);
}



void expM1Cplx(void) {
  real_t zReal, zImag;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &zReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &zImag);

  expM1Complex(&zReal, &zImag, &zReal, &zImag, &ctxtReal75);

  convertRealToReal34ResultRegister(&zReal, REGISTER_X);
  convertRealToImag34ResultRegister(&zImag, REGISTER_X);
}
