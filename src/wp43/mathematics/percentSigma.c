// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/percentSigma.h"

#include "constantPointers.h"
#include "debug.h"
#include "error.h"
#include "flags.h"
#include "matrix.h"
#include "registers.h"
#include "registerValueConversions.h"
#include <stdbool.h>

#include "wp43.h"

static void dataTypeError(void);

void percentSigmaRema(void);

TO_QSPI void (* const PercentSigma[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1                 2                 3              4              5              6              7                 8              9              10
//          Long integer      Real34            complex34      Time           Date           String         Real34 mat        Complex34 mat  Short integer  Config data
            percentSigmaLonI, percentSigmaReal, dataTypeError, dataTypeError, dataTypeError, dataTypeError, percentSigmaRema, dataTypeError, dataTypeError, dataTypeError
};

//=============================================================================
// Error handling
//-----------------------------------------------------------------------------

/********************************************//**
 * \brief Data type error in %Sigma
 *
 * \param void
 * \return void
 ***********************************************/
static void dataTypeError(void) {
  displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    sprintf(errorMessage, "cannot use %s", getRegisterDataTypeName(REGISTER_X, true, false));
    moreInfoOnError("In function fnPercentSigma:", errorMessage, NULL, NULL);
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
}

//=============================================================================
// Main function
//-----------------------------------------------------------------------------

/********************************************//**
 * \brief regX ==> regL and PercentSigma(regX) ==> regX
 * enables stack lift and refreshes the stack.
 * Calculate %Sigma
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnPercentSigma(uint16_t unusedButMandatoryParameter) {
  if(statisticalSumsPointer == NULL) {
    displayCalcErrorMessage(ERROR_NO_SUMMATION_DATA, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "There is no statistical data available!");
      moreInfoOnError("In function fnPercentSigma:", errorMessage, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
  }
  else {
    if(!saveLastX()) {
    return;
  }

    PercentSigma[getRegisterDataType(REGISTER_X)]();

    adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
  }
}

//=============================================================================
// PercentSigma calculation functions
//-----------------------------------------------------------------------------

static bool percentSigma(real_t *xReal, real_t *rReal, realContext_t *realContext) {
  real34ToReal(SIGMA_X, rReal);    // r = Sum(x)

  if(realIsZero(rReal)) {
    if(getSystemFlag(FLAG_SPCRES)) {
      realCopy((realIsPositive(rReal) ? const_plusInfinity : const_minusInfinity), rReal);
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function fnPercentSigma:", "cannot divide a real by 0", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
      return false;
    }
  }

  realDivide(xReal, rReal, rReal, realContext);       // r = x/Sum(x)
  realMultiply(rReal, const_100, rReal, realContext); // r = 100*x/Sum(x)

  return true;
}

/********************************************//**
 * \brief Percent(X(long integer)) ==> X(real34)
 *
 * \param void
 * \return void
 ***********************************************/
void percentSigmaLonI(void) {
  real_t xReal, rReal;

  convertLongIntegerRegisterToReal(REGISTER_X, &xReal, &ctxtReal39);

  if(percentSigma(&xReal, &rReal, &ctxtReal39)) {
    reallocateRegister(REGISTER_X, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
    convertRealToReal34ResultRegister(&rReal, REGISTER_X);
    setRegisterAngularMode(REGISTER_X, amNone);
  }
}

/********************************************//**
 * \brief Percent(X(real34)) ==> X(real34)
 *
 * \param void
 * \return void
 ***********************************************/
void percentSigmaReal(void) {
  real_t xReal, rReal;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);

  if(percentSigma(&xReal, &rReal, &ctxtReal39)) {
    convertRealToReal34ResultRegister(&rReal, REGISTER_X);
  }
}



void percentSigmaRema(void) {
  elementwiseRema(percentSigmaReal);
}
