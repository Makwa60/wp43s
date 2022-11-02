// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/percent.h"

#include "constantPointers.h"
#include "debug.h"
#include "error.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

static void dataTtypeError(void);

TO_QSPI void (* const percent[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS][NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX |    regY ==>    1               2                3               4               5               6               7               8               9               10
//      V                Long integer    Real34           Complex34       Time            Date            String          Real34 mat      Complex34 mat   Short integer   Config data
/*  1 Long integer  */ {percentLonILonI, percentRealLonI, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError},
/*  2 Real34        */ {percentLonIReal, percentRealReal, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError},
/*  3 Complex34     */ {dataTtypeError,  dataTtypeError,  dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError},
/*  4 Time          */ {dataTtypeError,  dataTtypeError,  dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError},
/*  5 Date          */ {dataTtypeError,  dataTtypeError,  dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError},
/*  6 String        */ {dataTtypeError,  dataTtypeError,  dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError},
/*  7 Real34 mat    */ {dataTtypeError,  dataTtypeError,  dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError},
/*  8 Complex34 mat */ {dataTtypeError,  dataTtypeError,  dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError},
/*  9 Short integer */ {dataTtypeError,  dataTtypeError,  dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError},
/* 10 Config data   */ {dataTtypeError,  dataTtypeError,  dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError, dataTtypeError}
};

//=============================================================================
// Error handling
//-----------------------------------------------------------------------------

/********************************************//**
 * \brief Data type error in %
 *
 * \param void
 * \return void
 ***********************************************/
static void dataTtypeError(void) {
  displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    sprintf(errorMessage, "cannot raise %s", getRegisterDataTypeName(REGISTER_Y, true, false));
    sprintf(errorMessage + ERROR_MESSAGE_LENGTH/2, "to %s", getRegisterDataTypeName(REGISTER_X, true, false));
    moreInfoOnError("In function fnPercent:", errorMessage, errorMessage + ERROR_MESSAGE_LENGTH/2, NULL);
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
}

//=============================================================================
// Main function
//-----------------------------------------------------------------------------

/********************************************//**
 * \brief regX ==> regL and Percent(regX, RegY) ==> regX
 * enables stack lift and refreshes the stack.
 * Calculate x*y/100
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnPercent(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  percent[getRegisterDataType(REGISTER_X)][getRegisterDataType(REGISTER_Y)]();

  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}

//=============================================================================
// Percent calculation functions
//-----------------------------------------------------------------------------

static void percentReal(real_t *xReal, real_t *yReal, real_t *rReal, realContext_t *realContext) {
  realMultiply(xReal, yReal, rReal, realContext);     // rReal = xReal * yReal
  realDivide(rReal, const_100, rReal, realContext);   // rReal = rReal / 100.0
}

/********************************************//**
 * \brief Percent(Y(long integer), X(long integer)) ==> X(real34)
 *
 * \param void
 * \return void
 ***********************************************/
void percentLonILonI(void) {
  real_t xReal, yReal;
  real_t rReal;

  convertLongIntegerRegisterToReal(REGISTER_X, &xReal, &ctxtReal39);
  convertLongIntegerRegisterToReal(REGISTER_Y, &yReal, &ctxtReal39);

  percentReal(&xReal, &yReal, &rReal, &ctxtReal34);

  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BLOCKS, amNone);
  convertRealToReal34ResultRegister(&rReal, REGISTER_X);
  setRegisterAngularMode(REGISTER_X, amNone);
}

/********************************************//**
 * \brief Percent(Y(long integer), X(real34)) ==> X(real34)
 *
 * \param void
 * \return void
 ***********************************************/
void percentLonIReal(void) {
  real_t xReal, yReal;
  real_t rReal;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  convertLongIntegerRegisterToReal(REGISTER_Y, &yReal, &ctxtReal39);

  percentReal(&xReal, &yReal, &rReal, &ctxtReal34);

  convertRealToReal34ResultRegister(&rReal, REGISTER_X);
}

/********************************************//**
 * \brief Percent(Y(real34), X(long integer)) ==> X(real34)
 *
 * \param void
 * \return void
 ***********************************************/
void percentRealLonI(void) {
  real_t xReal, yReal;
  real_t rReal;

  convertLongIntegerRegisterToReal(REGISTER_X, &xReal, &ctxtReal39);
  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);

  percentReal(&xReal, &yReal, &rReal, &ctxtReal34);

  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BLOCKS, amNone);
  convertRealToReal34ResultRegister(&rReal, REGISTER_X);
  setRegisterAngularMode(REGISTER_X, amNone);
}

/********************************************//**
 * \brief Percent(Y(real34), X(real34)) ==> X(real34)
 *
 * \param void
 * \return void
 ***********************************************/
void percentRealReal(void) {
  real_t xReal, yReal;
  real_t rReal;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);

  percentReal(&xReal, &yReal, &rReal, &ctxtReal34);

  convertRealToReal34ResultRegister(&rReal, REGISTER_X);
}
