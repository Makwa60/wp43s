/* This file is part of 43S.
 *
 * 43S is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * 43S is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 43S.  If not, see <http://www.gnu.org/licenses/>.
 */

/********************************************//**
 * \file normal.c
 ***********************************************/

#include "distributions/normal.h"

#include "constantPointers.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/rsd.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"
#include <stdbool.h>

#include "wp43.h"


static bool checkParamNormal(real_t *x, real_t *i, real_t *j) {
  if(   ((getRegisterDataType(REGISTER_X) != dtReal34) && (getRegisterDataType(REGISTER_X) != dtLongInteger))
     || ((getRegisterDataType(REGISTER_I) != dtReal34) && (getRegisterDataType(REGISTER_I) != dtLongInteger))
     || ((getRegisterDataType(REGISTER_J) != dtReal34) && (getRegisterDataType(REGISTER_J) != dtLongInteger))) {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        sprintf(errorMessage, "Values in register X, I and J must be of the real or long integer type");
        moreInfoOnError("In function checkParamNormal:", errorMessage, NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
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
  else if(realIsZero(j) || realIsNegative(j)) {
    displayCalcErrorMessage(ERROR_INVALID_DISTRIBUTION_PARAM, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      moreInfoOnError("In function checkParamNormal:", "cannot calculate for " STD_sigma " " STD_LESS_EQUAL " 0", NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    return false;
  }
  return true;
}


static void normalP(bool logNormal) {
  real_t val, alval, mu, sigma, ans;

  if(!saveLastX()) {
    return;
  }

  if(checkParamNormal(&val, &mu, &sigma)) {
    if(logNormal && realIsZero(&val)) {
      realZero(&ans);
    }
    else if(logNormal && realIsNegative(&val)) {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function fnLogNormalP:", "cannot calculate for x < 0", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
    else {
      if(logNormal) {
        realCopy(&val, &alval);
        WP34S_Ln(&alval, &val, &ctxtReal39);
      }
      realSubtract(&val, &mu, &val, &ctxtReal39);
      realDivide(&val, &sigma, &val, &ctxtReal39);
      WP34S_Pdf_Q(&val, &ans, &ctxtReal39);
      if(logNormal) {
        realDivide(&ans, &sigma, &ans, &ctxtReal39);
        realDivide(&ans, &alval, &ans, &ctxtReal39);
      }
    }
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE, amNone);
    convertRealToReal34ResultRegister(&ans, REGISTER_X);
  }

  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}


static void normalL(bool logNormal) {
  real_t val, mu, sigma, ans;

  if(!saveLastX()) {
    return;
  }

  if(checkParamNormal(&val, &mu, &sigma)) {
    if(logNormal && realIsZero(&val)) {
      realZero(&ans);
    }
    else if(logNormal && realIsNegative(&val)) {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function fnLogNormalP:", "cannot calculate for x < 0", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
    else {
      if(logNormal) {
        WP34S_Ln(&val, &val, &ctxtReal39);
      }
      realSubtract(&val, &mu, &val, &ctxtReal39);
      realDivide(&val, &sigma, &val, &ctxtReal39);
      WP34S_Cdf_Q(&val, &ans, &ctxtReal39);
    }
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE, amNone);
    convertRealToReal34ResultRegister(&ans, REGISTER_X);
  }

  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}


static void normalR(bool logNormal) {
  real_t val, mu, sigma, ans;

  if(!saveLastX()) {
    return;
  }

  if(checkParamNormal(&val, &mu, &sigma)) {
    if(logNormal && realIsZero(&val)) {
      realCopy(const_1, &ans);
    }
    else if(logNormal && realIsNegative(&val)) {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function fnLogNormalP:", "cannot calculate for x < 0", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
    else {
      if(logNormal) {
        WP34S_Ln(&val, &val, &ctxtReal39);
      }
      realSubtract(&val, &mu, &val, &ctxtReal39);
      realDivide(&val, &sigma, &val, &ctxtReal39);
      WP34S_Cdfu_Q(&val, &ans, &ctxtReal39);
    }
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE, amNone);
    convertRealToReal34ResultRegister(&ans, REGISTER_X);
  }

  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}


static void normalI(bool logNormal) {
  real_t val, mu, sigma, ans;

  if(!saveLastX()) {
    return;
  }

  if(checkParamNormal(&val, &mu, &sigma)) {
    if((!getSystemFlag(FLAG_SPCRES)) && (realCompareLessEqual(&val, const_0) || realCompareGreaterEqual(&val, const_1))) {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function fnNormalI:", "the argument must be 0 < x < 1", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
    else {
      WP34S_Qf_Q(&val, &ans, &ctxtReal39);
      realMultiply(&ans, &sigma, &ans, &ctxtReal39);
      realAdd(&ans, &mu, &ans, &ctxtReal39);
      if(logNormal) {
        realExp(&ans, &ans, &ctxtReal39);
      }
      reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE, amNone);
      convertRealToReal34ResultRegister(&ans, REGISTER_X);
    }
  }

  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}



void fnNormalP(uint16_t unusedButMandatoryParameter) {
  normalP(false);
}

void fnNormalL(uint16_t unusedButMandatoryParameter) {
  normalL(false);
}

void fnNormalR(uint16_t unusedButMandatoryParameter) {
  normalR(false);
}

void fnNormalI(uint16_t unusedButMandatoryParameter) {
  normalI(false);
}



void fnLogNormalP(uint16_t unusedButMandatoryParameter) {
  normalP(true);
}

void fnLogNormalL(uint16_t unusedButMandatoryParameter) {
  normalL(true);
}

void fnLogNormalR(uint16_t unusedButMandatoryParameter) {
  normalR(true);
}

void fnLogNormalI(uint16_t unusedButMandatoryParameter) {
  normalI(true);
}



/******************************************************
 * This functions are borrowed from the WP34S project
 ******************************************************/

static void cdf_q(const real_t *x, real_t *res, realContext_t *realContext, bool upper) {
  real_t p;

  if(upper) {
    if(realIsNegative(x)) {
      goto cdfu_q_flip;
    }
    cdf_q_flip:
    realPower(x, const_2, res, realContext);
    realDivide(res, const_2, res, realContext);
    WP34S_GammaP(res, const_1on2, res, realContext, true, false);
    realDivide(res, const_2, res, realContext);
    realSquareRoot(const_pi, &p, realContext);
    realDivide(res, &p, res, realContext);
    return;
  }
  else {
    if(realIsNegative(x)) {
      goto cdf_q_flip;
    }
    cdfu_q_flip:
    realPower(x, const_2, res, realContext);
    realDivide(res, const_2, res, realContext);
    WP34S_GammaP(res, const_1on2, res, realContext, false, false);
    realSquareRoot(const_pi, &p, realContext);
    realDivide(res, &p, res, realContext);
    realAdd(res, const_1, res, realContext);
    realDivide(res, const_2, res, realContext);
    return;
  }
}

void WP34S_Pdf_Q(const real_t *x, real_t *res, realContext_t *realContext) {
  real_t p;
  realPower(x, const_2, res, realContext);
  realDivide(res, const_2, res, realContext);
  realChangeSign(res);
  realExp(res, res, realContext);
  realSquareRoot(const_2pi, &p, realContext);
  realDivide(res, &p, res, realContext);
}

void WP34S_Cdfu_Q(const real_t *x, real_t *res, realContext_t *realContext) {
  cdf_q(x, res, realContext, true);
}

void WP34S_Cdf_Q(const real_t *x, real_t *res, realContext_t *realContext) {
  cdf_q(x, res, realContext, false);
}

/* This routine that returns a signed guess for the Normal quantile.
 * GNQ takes any 0 < p < 1 and returns a positive or negative estimate
 * for the Normal quantile.
 */
void WP34S_qf_q_est(const real_t *x, real_t *res, real_t* resY, realContext_t *realContext) {
  real_t p, q, r, s;
  bool   isSmall = false;

  // qf_q_int_est
  realMultiply(x, const__1, &p, realContext);
  realAdd(&p, const_1, &p, realContext);
  if(realCompareLessThan(x, &p)) {
    isSmall = true;
    realCopy(x, &p);
  }

  realDivide(const_2, const_10, &q, realContext);

  if(realCompareLessThan(&p, &q)) {
    WP34S_Ln(&p, &q, realContext);
    realMultiply(&q, const_2, &q, realContext);
    realChangeSign(&q);
    realSubtract(&q, const_1, &r, realContext);
    realMultiply(&r, const_2pi, &r, realContext);
    realSquareRoot(&r, &r, realContext);
    realMultiply(&r, &p, &r, realContext);
    WP34S_Ln(&r, &r, realContext);
    realMultiply(&r, const_2, &r, realContext);
    realChangeSign(&r);
    realSquareRoot(&r, &r, realContext);
    int32ToReal(264, &s);
    realDivide(&s, const_1000, &s, realContext);
    realDivide(&s, &q, &q, realContext);
    realAdd(&q, &r, &q, realContext);
  }
  else { // qf_q_mid
    realMultiply(&p, const__1, &q, realContext);
    realAdd(&q, const_1on2, &q, realContext);
    realSquareRoot(const_2pi, &r, realContext);
    realMultiply(&q, &r, &q, realContext);
    realPower(&q, const_3, &r, realContext);
    realDivide(&r, const_5, &r, realContext);
    realAdd(&q, &r, &q, realContext);
  }

  realCopy(&q, res);
  if(resY) {
    realCopy(&p, resY);
  }
  // qf_q_signfix
  if(isSmall) {
    realChangeSign(res);
  }
}

void WP34S_Qf_Q(const real_t *x, real_t *res, realContext_t *realContext) {
  real_t  p, q, r, s, reg0;
  bool    half = false;
  int32_t loops;
  WP34S_qf_q_est(x, &p, &reg0, realContext);
  if(realIsNegative(&p)) {
    half = true;
  }
  loops = 2;
  realSetPositiveSign(&p);
  realAdd(&p, const_1, &q, realContext);
  roundToSignificantDigits(&q, &q, 3, realContext);
  if(realCompareEqual(&q, const_1)) {
    --loops;
    realMultiply(&p, const_1e8, &q, realContext);
    realMultiply(&q, const_1e8, &q, realContext);
    if(realCompareGreaterThan(&q, const_1)) {
      goto qf_q_calc;
    }
  }
  else {
    qf_q_calc:
    do { // qf_q_refine
      if(realCompareGreaterEqual(&p, const_1)) {
        WP34S_Cdfu_Q(&p, &q, realContext);
        realSubtract(&q, &reg0, &q, realContext);
      }
      else { // qf_q_small
        realPower(&p, const_2, &q, realContext);
        realDivide(&q, const_2, &q, realContext);
        WP34S_GammaP(&q, const_1on2, &r, realContext, false, true);
        realDivide(&r, const_2, &q, realContext);
        realChangeSign(&q);
        realSubtract(const_1on2, &reg0, &r, realContext);
        realAdd(&q, &r, &q, realContext);
      }
      // qf_q_common
      WP34S_Pdf_Q(&p, &r, realContext);
      realDivide(&q, &r, &q, realContext);
      realPower(&q, const_3, &r, realContext);
      realPower(&p, const_2, &s, realContext);
      realMultiply(&s, const_2, &s, realContext);
      realAdd(&s, const_1, &s, realContext);
      realMultiply(&r, &s, &r, realContext);
      int32ToReal(6, &s);
      realDivide(&r, &s, &r, realContext);
      realDivide(&p, const_2, &s, realContext);
      realMultiply(&s, &q, &s, realContext);
      realMultiply(&s, &q, &s, realContext);
      realAdd(&s, &r, &r, realContext);
      realAdd(&r, &q, &q, realContext);
      realAdd(&q, &p, &p, realContext);
      // SHOW_CONVERGENCE
    } while(--loops > 0);
  }
  // qf_q_out
  if(half) {
    realChangeSign(&p);
  }
  realCopy(&p, res);
}
