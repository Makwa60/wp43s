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
 * \file conjugate.c
 ***********************************************/

#include "mathematics/conjugate.h"

#include "debug.h"
#include "error.h"
#include "flags.h"
#include "items.h"
#include "matrix.h"
#include "registers.h"

#include "wp43.h"



TO_QSPI void (* const conjugate[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1            2          3         4          5          6          7          8           9             10
//          Long integer Real34     Complex34 Time       Date       String     Real34 mat Complex34 m Short integer Config data
            conjError,   conjError, conjCplx, conjError, conjError, conjError, conjError, conjCxma,   conjError,    conjError
};



/********************************************//**
 * \brief Data type error in exp
 *
 * \param void
 * \return void
 ***********************************************/
#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void conjError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    sprintf(errorMessage, "cannot calculate conj for %s", getRegisterDataTypeName(REGISTER_X, true, false));
    moreInfoOnError("In function fnConjugate:", errorMessage, NULL, NULL);
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



/********************************************//**
 * \brief regX ==> regL and conj(regX) ==> regX
 * enables stack lift and refreshes the stack
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnConjugate(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  conjugate[getRegisterDataType(REGISTER_X)]();
}



void conjCxma(void) {
#if !defined(TESTSUITE_BUILD)
  complex34Matrix_t cMat;

  linkToComplexMatrixRegister(REGISTER_X, &cMat);

  for(uint16_t i = 0; i < cMat.header.matrixRows * cMat.header.matrixColumns; ++i) {
    real34ChangeSign(VARIABLE_IMAG34_DATA(&cMat.matrixElements[i]));
    if(real34IsZero(VARIABLE_IMAG34_DATA(&cMat.matrixElements[i])) && !getSystemFlag(FLAG_SPCRES)) {
      real34SetPositiveSign(VARIABLE_IMAG34_DATA(&cMat.matrixElements[i]));
    }
  }
#endif // !TESTSUITE_BUILD
}



void conjCplx(void) {
  real34ChangeSign(REGISTER_IMAG34_DATA(REGISTER_X));
  if(real34IsZero(REGISTER_IMAG34_DATA(REGISTER_X)) && !getSystemFlag(FLAG_SPCRES)) {
    real34SetPositiveSign(REGISTER_IMAG34_DATA(REGISTER_X));
  }
}
