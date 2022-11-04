// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/deltaPercent.h"

#include "constantPointers.h"
#include "debug.h"
#include "error.h"
#include "flags.h"
#include "mathematics/comparisonReals.h"
#include "registers.h"
#include "registerValueConversions.h"
#include <stdbool.h>

#include "wp43.h"

static void dataTypeError(void);

TO_QSPI void (* const deltaPercent[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS][NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX |    regY ==>    1                     2                     3              4              5              6              7              8              9              10
//      V                Long integer          Real34                Complex34      Time           Date           String         Real34 mat     Complex34 mat  Short integer  Config data
/*  1 Long integer  */ { deltaPercentLonILonI, deltaPercentRealLonI, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError},
/*  2 Real34        */ { deltaPercentLonIReal, deltaPercentRealReal, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError},
/*  3 Complex34     */ { dataTypeError,        dataTypeError,        dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError},
/*  4 Time          */ { dataTypeError,        dataTypeError,        dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError},
/*  5 Date          */ { dataTypeError,        dataTypeError,        dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError},
/*  6 String        */ { dataTypeError,        dataTypeError,        dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError},
/*  7 Real34 mat    */ { dataTypeError,        dataTypeError,        dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError},
/*  8 Complex34 mat */ { dataTypeError,        dataTypeError,        dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError},
/*  9 Short integer */ { dataTypeError,        dataTypeError,        dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError},
/* 10 Config data   */ { dataTypeError,        dataTypeError,        dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError, dataTypeError}
};

//=============================================================================
// Error handling
//-----------------------------------------------------------------------------

/********************************************//**
 * \brief Data type error in %T
 *
 * \param void
 * \return void
 ***********************************************/
static void dataTypeError(void) {
  displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    sprintf(errorMessage, "cannot raise %s", getRegisterDataTypeName(REGISTER_Y, true, false));
    sprintf(errorMessage + ERROR_MESSAGE_LENGTH/2, "to %s", getRegisterDataTypeName(REGISTER_X, true, false));
    moreInfoOnError("In function fnDeltaPercent:", errorMessage, errorMessage + ERROR_MESSAGE_LENGTH/2, NULL);
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
}

//=============================================================================
// Main function
//-----------------------------------------------------------------------------

/********************************************//**
 * \brief regX ==> regL and DeltaPercent(regX, RegY) ==> regX
 * enables stack lift and refreshes the stack.
 * Calculate x*y/100
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnDeltaPercent(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  deltaPercent[getRegisterDataType(REGISTER_X)][getRegisterDataType(REGISTER_Y)]();

  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}


//=============================================================================
// Delta% calculation functions
//-----------------------------------------------------------------------------

static bool deltaPercentReal(real_t *xReal, real_t *yReal, real_t *rReal, realContext_t *realContext) {
  /*
   * Check x and y
   */
  if(realIsZero(xReal) && realCompareEqual(xReal, yReal)) {
      if(getSystemFlag(FLAG_SPCRES)) {
        realCopy(const_NaN, rReal);
      }
      else {
        displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
        #if (EXTRA_INFO_ON_CALC_ERROR == 1)
          moreInfoOnError("In function fnDeltaPercent:", "cannot divide 0 by 0", NULL, NULL);
        #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
        return false;
      }
  }
  else if(realIsZero(yReal)) {
    if(getSystemFlag(FLAG_SPCRES)) {
      realCopy((realCompareAbsGreaterThan(xReal, yReal) ? const_plusInfinity : const_minusInfinity),rReal);
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function fnDeltaPercent:", "cannot divide a real by y=0", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
      return false;
    }
  }
  else {
    realSubtract(xReal, yReal, rReal, realContext);     // r = x - y
    realDivide(rReal, yReal, rReal, realContext);       // r = (x - y)/y
    realMultiply(rReal, const_100, rReal, realContext); // r = r * 100.0
  }

  return true;
}

/********************************************//**
 * \brief DeltaPercent(Y(long integer), X(long integer)) ==> X(real34)
 *
 * \param void
 * \return void
 ***********************************************/
void deltaPercentLonILonI(void) {
  real_t xReal, yReal;
  real_t rReal;

  convertLongIntegerRegisterToReal(REGISTER_X, &xReal, &ctxtReal39);
  convertLongIntegerRegisterToReal(REGISTER_Y, &yReal, &ctxtReal39);

  if(deltaPercentReal(&xReal, &yReal, &rReal, &ctxtReal34)) {
    reallocateRegister(REGISTER_X, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
    convertRealToReal34ResultRegister(&rReal, REGISTER_X);
    setRegisterAngularMode(REGISTER_X, amNone);
  }
}

/********************************************//**
 * \brief DeltaPercent(Y(long integer), X(real34)) ==> X(real34)
 *
 * \param void
 * \return void
 ***********************************************/
void deltaPercentLonIReal(void) {
  real_t xReal, yReal;
  real_t rReal;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  convertLongIntegerRegisterToReal(REGISTER_Y, &yReal, &ctxtReal39);

  if(deltaPercentReal(&xReal, &yReal, &rReal, &ctxtReal34)) {
    convertRealToReal34ResultRegister(&rReal, REGISTER_X);
  }
}

/********************************************//**
 * \brief DeltaPercent(Y(real34), X(long integer)) ==> X(real34)
 *
 * \param void
 * \return void
 ***********************************************/
void deltaPercentRealLonI(void) {
  real_t xReal, yReal;
  real_t rReal;

  convertLongIntegerRegisterToReal(REGISTER_X, &xReal, &ctxtReal39);
  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);

  if(deltaPercentReal(&xReal, &yReal, &rReal, &ctxtReal34)) {
    reallocateRegister(REGISTER_X, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
    convertRealToReal34ResultRegister(&rReal, REGISTER_X);
    setRegisterAngularMode(REGISTER_X, amNone);
  }
}

/********************************************//**
 * \brief DeltaPercent(Y(real34), X(real34)) ==> X(real34)
 *
 * \param void
 * \return void
 ***********************************************/
void deltaPercentRealReal(void) {
  real_t xReal, yReal;
  real_t rReal;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);

  if(deltaPercentReal(&xReal, &yReal, &rReal, &ctxtReal34)) {
    convertRealToReal34ResultRegister(&rReal, REGISTER_X);
  }
}
