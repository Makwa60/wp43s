// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/bn.h"

#include "debug.h"
#include "error.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

static void bnTypeError(void) {
  displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
  errorMoreInfo("cannot calculate Bernoulli number for %s", getRegisterDataTypeName(REGISTER_X, true, false));
}



void fnBn(uint16_t unusedButMandatoryParameter) {
  real_t x, res;

  if(!saveLastX()) {
    return;
  }

  if(getRegisterDataType(REGISTER_X) == dtLongInteger) {
    convertLongIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);
  }
  else if(getRegisterDataType(REGISTER_X) == dtReal34 && getRegisterAngularMode(REGISTER_X) == amNone) {
    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);
  }
  else {
    bnTypeError();
    goto cleanup;
  }

  WP34S_Bernoulli(&x, &res, false, &ctxtReal39);
  if(realIsNaN(&res)) {
    displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      moreInfoOnError("In function fnBn:", "k must be a non-negative integer", NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
  }
  else {
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
    convertRealToReal34ResultRegister(&res, REGISTER_X);
  }

cleanup:
  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}



void fnBnStar(uint16_t unusedButMandatoryParameter) {
  real_t x, res;

  if(!saveLastX()) {
    return;
  }

  if(getRegisterDataType(REGISTER_X) == dtLongInteger) {
    convertLongIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);
  }
  else if(getRegisterDataType(REGISTER_X) == dtReal34 && getRegisterAngularMode(REGISTER_X) == amNone) {
    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);
  }
  else {
    bnTypeError();
    goto cleanup;
  }

  WP34S_Bernoulli(&x, &res, true, &ctxtReal39);
  if(realIsNaN(&res)) {
    displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      moreInfoOnError("In function fnBnStar:", "k must be a positive integer", NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
  }
  else {
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
    convertRealToReal34ResultRegister(&res, REGISTER_X);
  }

cleanup:
  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}
