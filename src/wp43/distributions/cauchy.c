// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "distributions/cauchy.h"

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

static bool checkParamCauchy(real_t *x, real_t *i, real_t *j) {
  if(   ((getRegisterDataType(REGISTER_X) != dtReal34) && (getRegisterDataType(REGISTER_X) != dtLongInteger))
     || ((getRegisterDataType(REGISTER_I) != dtReal34) && (getRegisterDataType(REGISTER_I) != dtLongInteger))
     || ((getRegisterDataType(REGISTER_J) != dtReal34) && (getRegisterDataType(REGISTER_J) != dtLongInteger))) {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        sprintf(errorMessage, "Values in register X, I and J must be of the real or long integer type");
        moreInfoOnError("In function checkParamCauchy:", errorMessage, NULL, NULL);
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
      moreInfoOnError("In function checkParamCauchy:", "cannot calculate for " STD_gamma " " STD_LESS_EQUAL " 0", NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    return false;
  }
  return true;
}



void fnCauchyP(uint16_t unusedButMandatoryParameter) {
  real_t val, ans, x0, gamma;

  if(!saveLastX()) {
    return;
  }

  if(checkParamCauchy(&val, &x0, &gamma)) {
    WP34S_Pdf_Cauchy(&val, &x0, &gamma, &ans, &ctxtReal39);
    reallocateRegister(REGISTER_X, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
    convertRealToReal34ResultRegister(&ans, REGISTER_X);
  }

  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}



void fnCauchyL(uint16_t unusedButMandatoryParameter) {
  real_t val, ans, x0, gamma;

  if(!saveLastX()) {
    return;
  }

  if(checkParamCauchy(&val, &x0, &gamma)) {
    WP34S_Cdf_Cauchy(&val, &x0, &gamma, &ans, &ctxtReal39);
    reallocateRegister(REGISTER_X, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
    convertRealToReal34ResultRegister(&ans, REGISTER_X);
  }

  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}



void fnCauchyR(uint16_t unusedButMandatoryParameter) {
  real_t val, ans, x0, gamma;

  if(!saveLastX()) {
    return;
  }

  if(checkParamCauchy(&val, &x0, &gamma)) {
    WP34S_Cdfu_Cauchy(&val, &x0, &gamma, &ans, &ctxtReal39);
    reallocateRegister(REGISTER_X, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
    convertRealToReal34ResultRegister(&ans, REGISTER_X);
  }

  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}



void fnCauchyI(uint16_t unusedButMandatoryParameter) {
  real_t val, ans, x0, gamma;

  if(!saveLastX()) {
    return;
  }

  if(checkParamCauchy(&val, &x0, &gamma)) {
    if((!getSystemFlag(FLAG_SPCRES)) && (realCompareLessEqual(&val, const_0) || realCompareGreaterEqual(&val, const_1))) {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function fnCauchyI:", "the argument must be 0 < x < 1", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
    else {
      WP34S_Qf_Cauchy(&val, &x0, &gamma, &ans, &ctxtReal39);
      if(realIsNaN(&ans)) {
        displayCalcErrorMessage(ERROR_NO_ROOT_FOUND, ERR_REGISTER_LINE, REGISTER_X);
        #if (EXTRA_INFO_ON_CALC_ERROR == 1)
          moreInfoOnError("In function fnCauchyI:", "WP34S_Qf_Chi2 did not converge", NULL, NULL);
        #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
      }
      else {
        reallocateRegister(REGISTER_X, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
        convertRealToReal34ResultRegister(&ans, REGISTER_X);
      }
    }
  }

  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}



/******************************************************
 * This functions are borrowed from the WP34S project
 ******************************************************/

void WP34S_Pdf_Cauchy(const real_t *x, const real_t *x0, const real_t *gamma, real_t *res, realContext_t *realContext) {
  WP34S_cdf_cauchy_xform(x, x0, gamma, res, realContext);
  if(realIsSpecial(res)) {
    realZero(res); /* Can only be infinite which has zero probability */
    return;
  }
  realMultiply(res, res, res, realContext);
  realAdd(res, const_1, res, realContext);
  realMultiply(res, gamma, res, realContext);
  realMultiply(res, const_pi, res, realContext);
  realDivide(const_1, res, res, realContext);
}



void WP34S_Cdfu_Cauchy(const real_t *x, const real_t *x0, const real_t *gamma, real_t *res, realContext_t *realContext) {
  WP34S_cdf_cauchy_common(x, x0, gamma, true, res, realContext);
}



void WP34S_Cdf_Cauchy(const real_t *x, const real_t *x0, const real_t *gamma, real_t *res, realContext_t *realContext) {
  WP34S_cdf_cauchy_common(x, x0, gamma, false, res, realContext);
}



void WP34S_cdf_cauchy_common(const real_t *x, const real_t *x0, const real_t *gamma, bool complementary, real_t *res, realContext_t *realContext) {
  real_t p;

  WP34S_cdf_cauchy_xform(x, x0, gamma, &p, realContext);
  if(realIsSpecial(&p)) {
    realCopy(const_plusInfinity, res);
    return;
  }
  WP34S_Atan(&p, &p, realContext);
  realDivide(&p, const_pi, &p, realContext);
  if(complementary) {
    realChangeSign(&p);
  }
  realAdd(&p, const_1on2, res, realContext);
}



void WP34S_cdf_cauchy_xform(const real_t *x, const real_t *x0, const real_t *gamma, real_t *res, realContext_t *realContext) {
  realSubtract(x, x0, res, realContext);
  realDivide(res, gamma, res, realContext);
}



void WP34S_Qf_Cauchy(const real_t *x, const real_t *x0, const real_t *gamma, real_t *res, realContext_t *realContext) {
  real_t p, s, c;

  realSubtract(x, const_1on2, &p, realContext);
  realMultiply(&p, const_pi, &p, realContext);
  WP34S_SinCosTanTaylor(&p, false, &s, &c, &p, realContext);
  realMultiply(&p, gamma, &p, realContext);
  realAdd(&p, x0, res, realContext);
}
