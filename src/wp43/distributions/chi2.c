// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "distributions/chi2.h"

#include "constantPointers.h"
#include "distributions/normal.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/ln.h"
#include "mathematics/lnPOne.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"
#include <stdbool.h>

#include "wp43.h"

bool checkRegisterNoFP(calcRegister_t reg) {
  real34_t flooredI;

  if(getRegisterDataType(reg) == dtLongInteger) {
    return true;
  }
  else if(getRegisterDataType(reg) == dtReal34) {
    real34ToIntegralValue(REGISTER_REAL34_DATA(reg), &flooredI, DEC_ROUND_FLOOR);
    return real34CompareEqual(REGISTER_REAL34_DATA(reg), &flooredI);
  }
  else {
    return false;
  }
}



static bool checkParamChi2(real_t *x, real_t *i) {
  if(   ((getRegisterDataType(REGISTER_X) != dtReal34) && (getRegisterDataType(REGISTER_X) != dtLongInteger))
     || ((getRegisterDataType(REGISTER_I) != dtReal34) && (getRegisterDataType(REGISTER_I) != dtLongInteger))) {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("Values in register X and I must be of the real or long integer type");
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

  if(!checkRegisterNoFP(REGISTER_I)) {
    displayCalcErrorMessage(ERROR_INVALID_DISTRIBUTION_PARAM, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("k is not an integer");
    return false;
  }
  else if(getSystemFlag(FLAG_SPCRES)) {
    return true;
  }
  else if(realIsNegative(x)) {
    displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot calculate for x < 0");
    return false;
  }
  else if(realIsZero(i) || realIsNegative(i)) {
    displayCalcErrorMessage(ERROR_INVALID_DISTRIBUTION_PARAM, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot calculate for k " STD_LESS_EQUAL " 0");
    return false;
  }
  return true;
}



void fnChi2P(uint16_t unusedButMandatoryParameter) {
  real_t val, ans, dof;

  if(!saveLastX()) {
    return;
  }

  if(checkParamChi2(&val, &dof)) {
    WP34S_Pdf_Chi2(&val, &dof, &ans, &ctxtReal39);
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
    convertRealToReal34ResultRegister(&ans, REGISTER_X);
  }

  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}



void fnChi2L(uint16_t unusedButMandatoryParameter) {
  real_t val, ans, dof;

  if(!saveLastX()) {
    return;
  }

  if(checkParamChi2(&val, &dof)) {
    WP34S_Cdf_Chi2(&val, &dof, &ans, &ctxtReal39);
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
    convertRealToReal34ResultRegister(&ans, REGISTER_X);
  }

  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}



void fnChi2R(uint16_t unusedButMandatoryParameter) {
  real_t val, ans, dof;

  if(!saveLastX()) {
    return;
  }

  if(checkParamChi2(&val, &dof)) {
    WP34S_Cdfu_Chi2(&val, &dof, &ans, &ctxtReal39);
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
    convertRealToReal34ResultRegister(&ans, REGISTER_X);
  }

  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}



void fnChi2I(uint16_t unusedButMandatoryParameter) {
  real_t val, ans, dof;

  if(!saveLastX()) {
    return;
  }

  if(checkParamChi2(&val, &dof)) {
    if((!getSystemFlag(FLAG_SPCRES)) && (realCompareLessEqual(&val, const_0) || realCompareGreaterEqual(&val, const_1))) {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("the argument must be 0 < x < 1");
    }
    else {
      WP34S_Qf_Chi2(&val, &dof, &ans, &ctxtReal39);
      if(realIsNaN(&ans)) {
        displayCalcErrorMessage(ERROR_NO_ROOT_FOUND, ERR_REGISTER_LINE, REGISTER_X);
        errorMoreInfo("WP34S_Qf_Chi2 did not converge");
      }
      else {
        reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
        convertRealToReal34ResultRegister(&ans, REGISTER_X);
      }
    }
  }

  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}



/******************************************************
 * This functions are borrowed from the WP34S project
 ******************************************************/

void WP34S_Pdf_Chi2(const real_t *x, const real_t *k, real_t *res, realContext_t *realContext) {
  real_t p, q, r, s;

  if(realCompareLessEqual(x, const_0)) {
    realCopy(const_0, res);
    return;
  }

  realMultiply(k, const_1on2, &p, realContext);
  realLn(x, &q, realContext);
  realMultiply(x, const_1on2, &r, realContext);
  realChangeSign(&r);
  realSubtract(&p, const_1, &s, realContext);
  realMultiply(&q, &s, &q, realContext);
  realAdd(&r, &q, &q, realContext);
  WP34S_LnGamma(&p, &r, realContext);
  realSubtract(&q, &r, &q, realContext);
  realMultiply(&p, const_ln2, &r, realContext);
  realSubtract(&q, &r, &q, realContext);
  realExp(&q, res, realContext);
}



void WP34S_Cdfu_Chi2(const real_t *x, const real_t *k, real_t *res, realContext_t *realContext) {
  real_t p, q;

  if(realCompareLessEqual(x, const_0)) {
    realCopy(const_1, res);
    return;
  }
  if(realIsInfinite(x)) {
    realCopy(const_0, res);
    return;
  }

  realMultiply(x, const_1on2, &p, realContext);
  realMultiply(k, const_1on2, &q, realContext);
  WP34S_GammaP(&p, &q, res, realContext, true, true);
}



void WP34S_Cdf_Chi2(const real_t *x, const real_t *k, real_t *res, realContext_t *realContext) {
  real_t p, q;

  if(realCompareLessEqual(x, const_0)) {
    realCopy(const_0, res);
    return;
  }
  if(realIsInfinite(x)) {
    realCopy(const_1, res);
    return;
  }

  realMultiply(x, const_1on2, &p, realContext);
  realMultiply(k, const_1on2, &q, realContext);
  WP34S_GammaP(&p, &q, res, realContext, false, true);
}



void WP34S_Qf_Chi2(const real_t *x, const real_t *k, real_t *res, realContext_t *realContext) {
  real_t p, q, r, s, t, reg0;
  int32_t loops;

  if(realCompareEqual(x, const_0)) {
    realCopy(const_0, res);
  }

  realCopy(x, &reg0);
  loops = 6;
  realCopy(realCompareEqual(k, const_1) ? const_0 : k, &q);
  realChangeSign(&q);
  realPower(const_19on10, &q, &p, realContext);
  realDivide(&p, const_pi, &p, realContext);
  if(realCompareGreaterEqual(&reg0, &p)) {
    WP34S_qf_q_est(&reg0, &q, NULL, realContext);
    realDivide(const_111on500, k, &s, realContext);
    realSquareRoot(&s, &r, realContext);
    realMultiply(&q, &r, &q, realContext);
    realAdd(&q, const_1, &q, realContext);
    realSubtract(&q, &s, &q, realContext);
    realMultiply(&q, &q, &r, realContext);
    realMultiply(&r, &q, &q, realContext);
    realMultiply(&q, k, &q, realContext);
    realMultiply(const_eE, k, &r, realContext);
    realAdd(&r, const_8, &r, realContext);
    if(realCompareGreaterEqual(&q, &r)) {
      realMultiply(&q, const_1on2, &q, realContext);
      realLn(&q, &q, realContext);
      realMultiply(k, const_1on2, &t, realContext);
      realSubtract(&t, const_1, &t, realContext);
      realMultiply(&q, &t, &q, realContext);
      realChangeSign(&q);
      realMultiply(&reg0, const__1, &t, realContext);
      realLn1P(&t, &t, realContext);
      realAdd(&q, &t, &q, realContext);
      realMultiply(k, const_1on2, &t, realContext);
      WP34S_LnGamma(&t, &t, realContext);
      realAdd(&q, &t, &q, realContext);
      realMultiply(&q, const_2, &q, realContext);
      realChangeSign(&q);
    }
  }
  else { // chi2_q_low
    realDivide(&reg0, k, &q, realContext);
    realMultiply(&q, const_1on2, &q, realContext);
    realLn(&q, &q, realContext);
    realMultiply(k, const_1on2, &r, realContext);
    WP34S_LnGamma(&r, &r, realContext);
    realAdd(&q, &r, &q, realContext);
    realMultiply(&q, const_2, &q, realContext);
    realDivide(&q, k, &q, realContext);
    realExp(&q, &q, realContext);
    realMultiply(&q, const_2, &q, realContext);
  }

  do { // chi2_q_refine
    if(realCompareLessThan(&q, k)) {
      WP34S_Cdf_Chi2(&q, k, &p, realContext);
      realDivide(&p, &reg0, &r, realContext);
      realLn(&r, &r, realContext);
    }
    else { // chi2_q_big
      realSubtract(const_1, &reg0, &r, realContext);
      WP34S_Cdfu_Chi2(&q, k, &s, realContext);
      realSubtract(const_1, &s, &p, realContext);
      realSubtract(&r, &s, &r, realContext);
      realDivide(&r, &reg0, &r, realContext);
      realLn1P(&r, &r, realContext);
    }
    // chi2_q_common
    WP34S_Pdf_Chi2(&q, k, &s, realContext);
    realDivide(&s, &p, &p, realContext);
    realDivide(&r, &p, &r, realContext);
    realSubtract(k, const_2, &s, realContext);
    realSubtract(&s, &q, &s, realContext);
    realDivide(&s, &q, &s, realContext);
    realMultiply(&p, const_2, &p, realContext);
    realSubtract(&s, &p, &p, realContext);
    realMultiply(&p, const_1on4, &p, realContext);
    realMultiply(&p, &r, &p, realContext);
    realChangeSign(&p);
    realAdd(&p, const_1, &p, realContext);
    realDivide(&r, &p, &r, realContext);
    realChangeSign(&r);
    realAdd(&q, &r, &p, realContext);
    // SHOW_CONVERGENCE
    realCopy(const_1, &r); r.exponent -= 32 /*14*/;
    if(WP34S_RelativeError(&p, &q, &r, realContext)) {
      realCopy(&p, res);
      return;
    }
    realCopy(&p, &q);
  } while(--loops > 0);

    realCopy(const_NaN, res); // ERR 20
}
