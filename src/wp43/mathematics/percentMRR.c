// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/percentMRR.h"

#include "constantPointers.h"
#include "debug.h"
#include "error.h"
#include "flags.h"
#include "items.h"
#include "registers.h"
#include "registerValueConversions.h"
#include "stack.h"
#include <stdbool.h>

#include "wp43.h"

static bool convertRegisterToReal(calcRegister_t regist, real_t *r) {
  bool result = true;

  switch(getRegisterDataType(regist)) {
    case dtLongInteger: {
      convertLongIntegerRegisterToReal(regist, r, &ctxtReal34);
      break;
    }

    case dtReal34: {
      real34ToReal(REGISTER_REAL34_DATA(regist), r);
      break;
    }

    default: {
      result = false;
    }
  }

  return result;
}

static void percentMRR() {
  real_t xReal, yReal, zReal;

  /*
   * Convert register X.
   */
  if(!convertRegisterToReal(REGISTER_X, &xReal)) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "cannot %%MRR with %s in X", getRegisterDataTypeName(REGISTER_X, true, false));
      moreInfoOnError("In function fnPercentMRR:", errorMessage, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    return;
  }
  /*
   * Convert register Y.
   */
  if(!convertRegisterToReal(REGISTER_Y, &yReal)) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_Y);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "cannot %%MRR with %s in Y", getRegisterDataTypeName(REGISTER_Y, true, false));
      moreInfoOnError("In function fnPercentMRR:", errorMessage, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    return;
  }
  /*
   * Convert register Z
   */
  if(!convertRegisterToReal(REGISTER_Z, &zReal)) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_Z);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "cannot %%MRR with %s in Z", getRegisterDataTypeName(REGISTER_Z, true, false));
      moreInfoOnError("In function fnPercentMRR:", errorMessage, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    return;
  }
  /*
   * Calculate %MRR
   */
  real_t q;

  if(realIsZero(&xReal) && realIsZero(&yReal)) {
    if(getSystemFlag(FLAG_SPCRES)) {
      realCopy(const_NaN, &q);
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function fnPercentMRTR:", "cannot divide x=0 by y=0", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
      return;
    }
  }
  else if(realIsZero(&yReal)) {
    if(getSystemFlag(FLAG_SPCRES)) {
      realCopy((realIsPositive(&xReal) ? const_plusInfinity : const_minusInfinity), &q);
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function fnPercentMRTR:", "cannot divide a real by 0", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
      return;
    }
  }

  realDivide(&xReal, &yReal, &q, &ctxtReal39);        // q = x/y
  realDivide(const_1, &zReal, &zReal, &ctxtReal39);   // z = 1/z
  realPower(&q, &zReal, &q, &ctxtReal39);             // q = pow(x/y, 1/z)
  realSubtract(&q, const_1, &q, &ctxtReal39);         // q = pow(x/y, 1/z) - 1
  realMultiply(&q, const_100, &q, &ctxtReal39);       // q = 100 * ( pow(x/y, 1/z) - 1 )

  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BLOCKS, amNone);
  convertRealToReal34ResultRegister(&q, REGISTER_X);
  setRegisterAngularMode(REGISTER_X, amNone);
}

/********************************************//**
 * \brief regX ==> regL and %MRR(regX, RegY, RegZ) ==> regX
 * enables stack lift and refreshes the stack.
 * Calculate the %MRR.
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnPercentMRR(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  percentMRR();

  adjustResult(REGISTER_X, true, true, REGISTER_X, -1, -1);
  if(lastErrorCode == 0) {
    fnDropY(NOPARAM);
  }
}
