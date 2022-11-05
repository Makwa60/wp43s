// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "distributions/weibull.h"

#include "constantPointers.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"
#include <stdbool.h>

#include "wp43.h"

static bool checkParamWeibull(real_t *x, real_t *i, real_t *j) {
  if(   ((getRegisterDataType(REGISTER_X) != dtReal34) && (getRegisterDataType(REGISTER_X) != dtLongInteger))
     || ((getRegisterDataType(REGISTER_I) != dtReal34) && (getRegisterDataType(REGISTER_I) != dtLongInteger))
     || ((getRegisterDataType(REGISTER_J) != dtReal34) && (getRegisterDataType(REGISTER_J) != dtLongInteger))) {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("Values in register X, I and J must be of the real or long integer type");
      return false;
  }

  if(getRegisterDataType(REGISTER_X) == dtReal34) {
    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), x);
  }
  else { // long integer
    convertLongIntegerRegisterToReal(REGISTER_X, x, &ctxtReal39);
  }

  if(getRegisterDataType(REGISTER_I) == dtReal34) {
    real34ToReal(REGISTER_REAL34_DATA(REGISTER_I), i);
  }
  else { // long integer
    convertLongIntegerRegisterToReal(REGISTER_I, i, &ctxtReal39);
  }

  if(getRegisterDataType(REGISTER_J) == dtReal34) {
    real34ToReal(REGISTER_REAL34_DATA(REGISTER_J), j);
  }
  else { // long integer
    convertLongIntegerRegisterToReal(REGISTER_J, j, &ctxtReal39);
  }

  if(getSystemFlag(FLAG_SPCRES)) {
    return true;
  }
  else if(realIsNegative(x)) {
    displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot calculate for x < 0");
    return false;
  }
  else if(realIsZero(i) || realIsNegative(i) || realIsZero(j) || realIsNegative(j)) {
    displayCalcErrorMessage(ERROR_INVALID_DISTRIBUTION_PARAM, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot calculate for b " STD_LESS_EQUAL " 0 or t " STD_LESS_EQUAL " 0");
    return false;
  }
  return true;
}



void fnWeibullP(uint16_t unusedButMandatoryParameter) {
  real_t val, shape, lifetime, ans;

  if(!saveLastX()) {
    return;
  }

  if(checkParamWeibull(&val, &shape, &lifetime)) {
    WP34S_Pdf_Weib(&val, &shape, &lifetime, &ans, &ctxtReal39);
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
    convertRealToReal34ResultRegister(&ans, REGISTER_X);
  }

  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}



void fnWeibullL(uint16_t unusedButMandatoryParameter) {
  real_t val, shape, lifetime, ans;

  if(!saveLastX()) {
    return;
  }

  if(checkParamWeibull(&val, &shape, &lifetime)) {
    WP34S_Cdf_Weib(&val, &shape, &lifetime, &ans, &ctxtReal39);
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
    convertRealToReal34ResultRegister(&ans, REGISTER_X);
  }

  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}



void fnWeibullR(uint16_t unusedButMandatoryParameter) {
  real_t val, shape, lifetime, ans;

  if(!saveLastX()) {
    return;
  }

  if(checkParamWeibull(&val, &shape, &lifetime)) {
    WP34S_Cdfu_Weib(&val, &shape, &lifetime, &ans, &ctxtReal39);
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
    convertRealToReal34ResultRegister(&ans, REGISTER_X);
  }

  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}



void fnWeibullI(uint16_t unusedButMandatoryParameter) {
  real_t val, shape, lifetime, ans;

  if(!saveLastX()) {
    return;
  }

  if(checkParamWeibull(&val, &shape, &lifetime)) {
    if((!getSystemFlag(FLAG_SPCRES)) && (realCompareLessEqual(&val, const_0) || realCompareGreaterEqual(&val, const_1))) {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("the argument must be 0 < x < 1");
    }
    else {
      WP34S_Qf_Weib(&val, &shape, &lifetime, &ans, &ctxtReal39);
      reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
      convertRealToReal34ResultRegister(&ans, REGISTER_X);
    }
  }

  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}



/******************************************************
 * This functions are borrowed from the WP34S project
 ******************************************************/

void WP34S_Pdf_Weib(const real_t *x, const real_t *b, const real_t *t, real_t *res, realContext_t *realContext) {
  real_t p, q, r;

  realDivide(x, b, &p, realContext);
  if(realIsSpecial(&p) || realIsNegative(&p) || realIsZero(&p)) {
    realZero(res);
    return;
  }
  realPower(&p, t, &q, realContext);
  realMultiply(&q, const__1, &r, realContext);
  realExp(&r, &r, realContext);
  realMultiply(&r, &q, &r, realContext);
  realDivide(&r, &p, &r, realContext);
  realMultiply(&r, t, &r, realContext);
  realDivide(&r, b, res, realContext);
}



void WP34S_Cdfu_Weib(const real_t *x, const real_t *b, const real_t *t, real_t *res, realContext_t *realContext) {
  real_t p;

  realDivide(x, b, &p, realContext);
  if(realIsNegative(&p) || realIsZero(&p)) {
    realCopy(const_1, res);
    return;
  }
  if(realIsSpecial(&p)) {
    realZero(res);
    return;
  }
  realPower(&p, t, &p, realContext);
  realChangeSign(&p);
  realExp(&p, res, realContext);
}



void WP34S_Cdf_Weib(const real_t *x, const real_t *b, const real_t *t, real_t *res, realContext_t *realContext) {
  real_t p;

  realDivide(x, b, &p, realContext);
  if(realIsNegative(&p) || realIsZero(&p)) {
    realZero(res);
    return;
  }
  if(realIsSpecial(&p)) {
    realCopy(const_1, res);
    return;
  }
  realPower(&p, t, &p, realContext);
  realChangeSign(&p);
  WP34S_ExpM1(&p, res, realContext);
  realChangeSign(res);
}



void WP34S_Qf_Weib(const real_t *x, const real_t *b, const real_t *t, real_t *res, realContext_t *realContext) {
  /* (-ln(1-p) ^ (1/k)) * J */
  real_t p, q;

  realMultiply(x, const__1, &p, realContext);
  WP34S_Ln1P(&p, &p, realContext);
  realChangeSign(&p);
  realDivide(const_1, t, &q, realContext);
  realPower(&p, &q, &p, realContext);
  realMultiply(&p, b, res, realContext);
}
