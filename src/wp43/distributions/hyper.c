// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "distributions/hyper.h"

#include "constantPointers.h"
#include "distributions/chi2.h"
#include "distributions/f.h"
#include "distributions/poisson.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/cpyx.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"
#include <stdbool.h>

#include "wp43.h"

static bool checkParamHyper(real_t *x, real_t *i, real_t *j, real_t *k) {
  real_t ik, xmin, xmax;

  if(   ((getRegisterDataType(REGISTER_X) != dtReal34) && (getRegisterDataType(REGISTER_X) != dtLongInteger))
     || ((getRegisterDataType(REGISTER_I) != dtReal34) && (getRegisterDataType(REGISTER_I) != dtLongInteger))
     || ((getRegisterDataType(REGISTER_J) != dtReal34) && (getRegisterDataType(REGISTER_J) != dtLongInteger))
     || ((getRegisterDataType(REGISTER_K) != dtReal34) && (getRegisterDataType(REGISTER_K) != dtLongInteger))) {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        sprintf(errorMessage, "Values in register X, I, J and K must be of the real or long integer type");
        moreInfoOnError("In function checkParamNegBinom:", errorMessage, NULL, NULL);
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

  if(getRegisterDataType(REGISTER_K) == dtReal34) {
    real34ToReal(REGISTER_REAL34_DATA(REGISTER_K), k);
  }
  else { // long integer
    convertLongIntegerRegisterToReal(REGISTER_K, k, &ctxtReal39);
  }

  realMultiply(i, k, &ik, &ctxtReal39), realToIntegralValue(&ik, &ik, DEC_ROUND_HALF_EVEN, &ctxtReal39);
  realAdd(j, &ik, &xmin, &ctxtReal39);
  realSubtract(&xmin, k, &xmin, &ctxtReal39);
  if(realIsNegative(&xmin)) {
    realCopy(const_0, &xmin);
  }
  realCopy(realCompareLessThan(&ik, j) ? &ik : j, &xmax);

  if((!checkRegisterNoFP(REGISTER_J)) || (!checkRegisterNoFP(REGISTER_K))) {
    displayCalcErrorMessage(ERROR_INVALID_DISTRIBUTION_PARAM, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      moreInfoOnError("In function checkParamHyper:", "n and/or N is not an integer", NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    return false;
  }
  else if(getSystemFlag(FLAG_SPCRES)) {
    return true;
  }
  else if(realIsNegative(x) || realCompareLessThan(x, &xmin) || realCompareGreaterThan(x, &xmax)) {
    displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      moreInfoOnError("In function checkParamHyper:", "cannot calculate for x < max(0, n + pN - N) or x > min(n, pN)", NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    return false;
  }
  else if(realIsNegative(i) || realCompareGreaterThan(i, const_1) || realIsNegative(j) || realCompareGreaterThan(j, k) || realIsNegative(k)) {
    displayCalcErrorMessage(ERROR_INVALID_DISTRIBUTION_PARAM, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      moreInfoOnError("In function checkParamHyper:", "the parameters must be integer, and 0 " STD_LESS_EQUAL " p " STD_LESS_EQUAL " 1, 0 " STD_LESS_EQUAL " n " STD_LESS_EQUAL " N, and N " STD_GREATER_EQUAL " 0", NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    return false;
  }
  return true;
}



void fnHypergeometricP(uint16_t unusedButMandatoryParameter) {
  real_t val, ans, prob, samp, batch;

  if(!saveLastX()) {
    return;
  }

  if(checkParamHyper(&val, &prob, &samp, &batch)) {
    if(realIsAnInteger(&val)) {
      pdf_Hypergeometric(&val, &prob, &samp, &batch, &ans, &ctxtReal39);
    }
    else {
      realZero(&ans);
    }
    if(realIsNaN(&ans)) {
      displayCalcErrorMessage(ERROR_INVALID_DISTRIBUTION_PARAM, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function fnHypergeometricP:", "a parameter is invalid", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
    else {
      reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE, amNone);
      convertRealToReal34ResultRegister(&ans, REGISTER_X);
    }
  }

  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}



void fnHypergeometricL(uint16_t unusedButMandatoryParameter) {
  real_t val, ans, prob, samp, batch;

  if(!saveLastX()) {
    return;
  }

  if(checkParamHyper(&val, &prob, &samp, &batch)) {
    cdf_Hypergeometric(&val, &prob, &samp, &batch, &ans, &ctxtReal75);
    if(realIsNaN(&ans)) {
      displayCalcErrorMessage(ERROR_INVALID_DISTRIBUTION_PARAM, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function fnNegBinomialL:", "a parameter is invalid", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
    else {
      reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE, amNone);
      convertRealToReal34ResultRegister(&ans, REGISTER_X);
    }
  }

  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}



void fnHypergeometricR(uint16_t unusedButMandatoryParameter) {
  real_t val, ans, prob, samp, batch;

  if(!saveLastX()) {
    return;
  }

  if(checkParamHyper(&val, &prob, &samp, &batch)) {
    cdfu_Hypergeometric(&val, &prob, &samp, &batch, &ans, &ctxtReal75);
    if(realIsNaN(&ans)) {
      displayCalcErrorMessage(ERROR_INVALID_DISTRIBUTION_PARAM, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function fnHypergeometricR:", "a parameter is invalid", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
    else {
      reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE, amNone);
      convertRealToReal34ResultRegister(&ans, REGISTER_X);
    }
  }

  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}



void fnHypergeometricI(uint16_t unusedButMandatoryParameter) {
  real_t val, ans, prob, samp, batch;

  if(!saveLastX()) {
    return;
  }

  if(checkParamHyper(&val, &prob, &samp, &batch)) {
    if((!getSystemFlag(FLAG_SPCRES)) && (realCompareLessEqual(&val, const_0) || realCompareGreaterEqual(&val, const_1))) {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function fnNegBinomialI:", "the argument must be 0 < x < 1", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
    else {
      qf_Hypergeometric(&val, &prob, &samp, &batch, &ans, &ctxtReal75);
      if(realIsNaN(&ans)) {
        displayCalcErrorMessage(ERROR_NO_ROOT_FOUND, ERR_REGISTER_LINE, REGISTER_X);
        #if (EXTRA_INFO_ON_CALC_ERROR == 1)
          moreInfoOnError("In function fnNegBinomialI:", "WP34S_Qf_Binomial did not converge", NULL, NULL);
        #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
      }
      else {
        reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE, amNone);
        convertRealToReal34ResultRegister(&ans, REGISTER_X);
      }
    }
  }

  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}



// PDF(k; n, K, N) = (K C k) ((N-K) C (n-k)) / (N C n)
void pdf_Hypergeometric(const real_t *x, const real_t *p0, const real_t *n, const real_t *n0, real_t *res, realContext_t *realContext) {
  real_t a, b, c, q, x0;

  realMultiply(p0, n0, &x0, realContext); // p0 == x0 / n0

  realCopy(&x0, &a), realCopy(x, &b), logCyxReal(&a, &b, &q, realContext);     // K C k
  realSubtract(n0, &x0, &a, realContext), realSubtract(n, x, &b, realContext);
  logCyxReal(&a, &b, &c, realContext), realAdd(&q, &c, &q, realContext);       // (N-K) C (n-k)
  realCopy(n0, &a), realCopy(n, &b);
  logCyxReal(&a, &b, &c, realContext), realSubtract(&q, &c, &q, realContext);  // N C n
  realExp(&q, res, realContext);
}



static void cdf_Hypergeometric_common(const real_t *x, const real_t *p0, const real_t *n, const real_t *n0, bool complementary, real_t *res, realContext_t *realContext) {
  real_t a, b, c, x0, binomPart;
  real_t a1, a2, a3, b1, b2, i, hypergeomPart, cvgTol;
  bool   signHgp = false;

  realMultiply(p0, n0, &x0, realContext); // p0 == x0 / n0
  realCopy(const_1, &cvgTol), cvgTol.exponent -= realContext->digits - 2;

  // (n C (k+1)) ((N-n) C (K-k-1)) / (N C K)
  realCopy(n, &a), realAdd(x, const_1, &b, realContext);
  logCyxReal(&a, &b, &binomPart, realContext);                                                // n C (k+1)
  realSubtract(n0, n, &a, realContext), realSubtract(&x0, x, &b, realContext), realSubtract(&b, const_1, &b, realContext);
  logCyxReal(&a, &b, &c, realContext), realAdd(&binomPart, &c, &binomPart, realContext);      // (N-n) C (K-k-1)
  realCopy(n0, &a), realCopy(&x0, &b);
  logCyxReal(&a, &b, &c, realContext), realSubtract(&binomPart, &c, &binomPart, realContext); // N C K

  // generalized hypergeometric function 3F2
  realCopy(const_1, &a1);
  realAdd(x, const_1, &a2, realContext);
  realAdd(&a2, const_1, &b1, realContext);
  realSubtract(&a2, n, &a3, realContext);
  realSubtract(&a2, &x0, &a2, realContext);
  realAdd(&b1, n0, &b2, realContext);
  realSubtract(&b2, &x0, &b2, realContext);
  realSubtract(&b2, n, &b2, realContext);
  realCopy(const_0, &hypergeomPart);
  realCopy(const_1, &i);

  if(complementary) {
    realExp(&binomPart, &b, realContext);
  }
  else {
    WP34S_ExpM1(&binomPart, &b, realContext);
    realSetPositiveSign(&b);
  }

  do {
    realCopy(&b, &c);

    if(realIsZero(&a2) || realIsZero(&a3) || realIsZero(&b1) || realIsZero(&b2)) {
      break;
    }

    signHgp = (realIsNegative(&a1) ? 1 : 0) ^ (realIsNegative(&a2) ? 1 : 0) ^ (realIsNegative(&a3) ? 1 : 0) ^ (realIsNegative(&b1) ? 1 : 0) ^ (realIsNegative(&b2) ? 1 : 0);
    realCopyAbs(&a1, &a), WP34S_Ln(&a, &a, realContext), realAdd(&hypergeomPart, &a, &hypergeomPart, realContext);
    realCopyAbs(&a2, &a), WP34S_Ln(&a, &a, realContext), realAdd(&hypergeomPart, &a, &hypergeomPart, realContext);
    realCopyAbs(&a3, &a), WP34S_Ln(&a, &a, realContext), realAdd(&hypergeomPart, &a, &hypergeomPart, realContext);
    realCopyAbs(&b1, &a), WP34S_Ln(&a, &a, realContext), realSubtract(&hypergeomPart, &a, &hypergeomPart, realContext);
    realCopyAbs(&b2, &a), WP34S_Ln(&a, &a, realContext), realSubtract(&hypergeomPart, &a, &hypergeomPart, realContext);
    WP34S_Ln(&i, &a, realContext), realSubtract(&hypergeomPart, &a, &hypergeomPart, realContext);

    realAdd(&binomPart, &hypergeomPart, &a, realContext);
    realExp(&a, &a, realContext);
    if(signHgp) {
      realSetNegativeSign(&a);
    }
    if(complementary) {
      realAdd(&b, &a, &b, realContext);
    }
    else {
      realSubtract(&b, &a, &b, realContext);
    }

    realAdd(&a1, const_1, &a1, realContext);
    realAdd(&a2, const_1, &a2, realContext);
    realAdd(&a3, const_1, &a3, realContext);
    realAdd(&b1, const_1, &b1, realContext);
    realAdd(&b2, const_1, &b2, realContext);
    realAdd(&i, const_1, &i, realContext);
  } while((!WP34S_RelativeError(&b, &c, &cvgTol, realContext)) && (!realIsSpecial(&b)));

  realCopy(&b, res);
  realSetPositiveSign(res);
}



void cdfu_Hypergeometric(const real_t *x, const real_t *p0, const real_t *n, const real_t *n0, real_t *res, realContext_t *realContext) {
  real_t p;

  realToIntegralValue(x, &p, DEC_ROUND_CEILING, realContext);
  if(realCompareLessThan(&p, const_1)) {
    realCopy(const_1, res);
    return;
  }
  realSubtract(&p, const_1, &p, realContext);

  cdf_Hypergeometric_common(&p, p0, n, n0, true, res, realContext);
}



void cdf_Hypergeometric(const real_t *x, const real_t *p0, const real_t *n, const real_t *n0, real_t *res, realContext_t *realContext) {
  real_t p;

  realToIntegralValue(x, &p, DEC_ROUND_FLOOR, realContext);
  cdf_Hypergeometric2(&p, p0, n, n0, res, realContext);
}



static void mode_Hypergeometric(const real_t *p0, const real_t *n, const real_t *n0, real_t *res, realContext_t *realContext) {
  real_t x0, a, q;

  realMultiply(p0, n0, &x0, realContext); // p0 == x0 / n0

  realAdd(n, const_1, &a, realContext);
  WP34S_Ln(&a, &q, realContext);
  realAdd(&x0, const_1, &a, realContext);
  WP34S_Ln(&a, &a, realContext), realAdd(&q, &a, &q, realContext);
  realAdd(n0, const_2, &a, realContext);
  WP34S_Ln(&a, &a, realContext), realSubtract(&q, &a, &q, realContext);
  realExp(&q, &q, realContext);
  realToIntegralValue(&q, res, DEC_ROUND_FLOOR, realContext);
}



void cdf_Hypergeometric2(const real_t *x, const real_t *p0, const real_t *n, const real_t *n0, real_t *res, realContext_t *realContext) {
  real_t x0, mode, pdf, i, cdf, cdf0;

  if(realCompareLessThan(x, const_0)) {
    realZero(res);
    return;
  }

  realMultiply(p0, n0, &x0, realContext); // p0 == x0 / n0
  mode_Hypergeometric(p0, n, n0, &mode, realContext);

  if(realCompareLessThan(x, &mode)) {
    realCopy(x, &i);
    realCopy(const_0, &cdf);
    do {
      realCopy(&cdf, &cdf0);
      pdf_Hypergeometric(&i, p0, n, n0, &pdf, realContext);
      realAdd(&cdf, &pdf, &cdf, realContext);
      realSubtract(&i, const_1, &i, realContext);
    } while((!WP34S_RelativeError(&cdf, &cdf0, const_1e_37, realContext)) && (!realIsNegative(&i)));
    realCopy(&cdf, res);
  }
  else {
    cdf_Hypergeometric_common(x, p0, n, n0, false, res, realContext);
  }
}



void qf_Hypergeometric(const real_t *x, const real_t *p0, const real_t *n, const real_t *n0, real_t *res, realContext_t *realContext) {
  real_t x0, mean, var, s;

  if(realCompareLessEqual(x, const_0)) {
    realZero(res);
    return;
  }

  realMultiply(p0, n0, &x0, realContext); // p0 == x0 / n0

  WP34S_Ln(n,   &var, realContext);
  WP34S_Ln(&x0, &s,   realContext), realAdd     (&var, &s, &var, realContext);
  WP34S_Ln(n0,  &s,   realContext), realSubtract(&var, &s, &var, realContext);
  realExp(&var, &mean, realContext);      // mean = nK/N

  realSubtract(n0, &x0,     &s, realContext), WP34S_Ln(&s, &s, realContext), realAdd     (&var, &s, &var, realContext);
                                              WP34S_Ln(n0, &s, realContext), realSubtract(&var, &s, &var, realContext);
  realSubtract(n0, n,       &s, realContext), WP34S_Ln(&s, &s, realContext), realAdd     (&var, &s, &var, realContext);
  realSubtract(n0, const_1, &s, realContext), WP34S_Ln(&s, &s, realContext), realSubtract(&var, &s, &var, realContext);
  realExp(&var, &var, realContext);       // variance = (nK/N) ((N-K)/N) ((N-n)/(N-1))
  realSquareRoot(&var, &var, realContext);

  WP34S_normal_moment_approx(x, &var, &mean, &s, realContext);
  WP34S_Qf_Newton(QF_NEWTON_HYPERGEOMETRIC, x, &s, p0, n, n0, res, realContext);
}
