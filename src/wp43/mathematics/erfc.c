// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/erfc.h"

#include "debug.h"
#include "error.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

TO_QSPI void (* const Erfc[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1            2         3          4          5          6          7          8           9             10
//          Long integer Real34    Complex34  Time       Date       String     Real34 mat Complex34 m Short integer Config data
            erfcLonI,    erfcReal, erfcError, erfcError, erfcError, erfcError, erfcError, erfcError,  erfcError,    erfcError
};



/********************************************//**
 * \brief Data type error in erfc
 *
 * \param void
 * \return void
 ***********************************************/
#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void erfcError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    sprintf(errorMessage, "cannot calculate Erfc for %s", getRegisterDataTypeName(REGISTER_X, true, false));
    moreInfoOnError("In function fnErfc:", errorMessage, NULL, NULL);
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



/********************************************//**
 * \brief regX ==> regL and erfc(regX) ==> regX
 * enables stack lift and refreshes the stack
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnErfc(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  Erfc[getRegisterDataType(REGISTER_X)]();

  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}



void erfcLonI(void) {
  real_t x;

  convertLongIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);
  WP34S_Erfc(&x, &x, &ctxtReal39);
  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE, amNone);
  convertRealToReal34ResultRegister(&x, REGISTER_X);
}



void erfcReal(void) {
  real_t x;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);
  WP34S_Erfc(&x, &x, &ctxtReal39);
  convertRealToReal34ResultRegister(&x, REGISTER_X);
}
