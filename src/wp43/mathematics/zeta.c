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
 * \file zeta.c
 ***********************************************/

#include "mathematics/zeta.h"

#include "constantPointers.h"
#include "debug.h"
#include "error.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/division.h"
#include "mathematics/multiplication.h"
#include "mathematics/power.h"
#include "mathematics/toRect.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"



TO_QSPI void (* const Zeta[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1            2         3          4          5          6          7          8           9             10
//          Long integer Real34    Complex34  Time       Date       String     Real34 mat Complex34 m Short integer Config data
            zetaError,   zetaReal, zetaCplx,  zetaError, zetaError, zetaError, zetaError, zetaError,  zetaError,    zetaError
};



/********************************************//**
 * \brief Data type error in zeta
 *
 * \param void
 * \return void
 ***********************************************/
#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void zetaError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    sprintf(errorMessage, "cannot calculate Zeta for %s", getRegisterDataTypeName(REGISTER_X, true, false));
    moreInfoOnError("In function fnZeta:", errorMessage, NULL, NULL);
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



/********************************************//**
 * \brief regX ==> regL and zeta(regX) ==> regX
 * enables stack lift and refreshes the stack
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnZeta(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) return;

  Zeta[getRegisterDataType(REGISTER_X)]();

  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}



void zetaReal(void) {
  real_t x, res;

  if(getRegisterAngularMode(REGISTER_X) == amNone) {
    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);
    WP34S_Zeta(&x, &res, &ctxtReal39);
    convertRealToReal34ResultRegister(&res, REGISTER_X);
  }
  else {
    zetaError();
  }
}



void zetaCplx(void) {
  real_t xr, xi, rr, ri;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xr);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xi);
  ComplexZeta(&xr, &xi, &rr, &ri, &ctxtReal39);
  convertRealToReal34ResultRegister(&rr, REGISTER_X);
  convertRealToImag34ResultRegister(&ri, REGISTER_X);
}



/**************************************************************************/
/* Complex zeta function implementation based on Jean-Marc Baillard's from:
 * http://hp41programs.yolasite.com/zeta.php
 */

static void zeta_calc_complex(real_t *reg4, real_t *reg5, real_t *reg6, real_t *reg7, realContext_t *realContext) {
  real_t s, p, q, r, reg0, reg1, reg2, reg3, reg8, reg9;

  realCopyAbs(reg7, &p);
  realMultiply(const_piOn2, &p, &q, realContext);
  realMultiply(&p, const_2, &p, realContext);
  WP34S_Ln1P(&p, &p, realContext);
  realAdd(&q, &p, &p, realContext);
  realCopy(const_3, &q); q.exponent += 10;
  WP34S_Ln(&q, &q, realContext);
  realAdd(&p, &q, &p, realContext);
  realSquareRoot(const_8, &q, realContext);
  realAdd(&q, const_3, &q, realContext);
  WP34S_Ln(&q, &q, realContext);
  realDivide(&p, &q, &p, realContext);
  realToIntegralValue(&p, &p, DEC_ROUND_DOWN, realContext);
  realAdd(&p, const_1, &p, realContext);
  realMultiply(&p, const_2, &p, realContext); // for extra digits
  realMultiply(&p, const_2, &p, realContext); // we have
  realCopy(&p, &reg0); realCopy(const_1, &reg1);
  realCopy(&p, &reg2); realCopy(const_1, &reg3);
  realCopy(const_1, reg4);
  realPower(const__1, &p, &p, realContext);
  realChangeSign(&p);
  realCopy(&p, reg5);
  realCopy(const_0, &reg8), realCopy(const_0, &reg9);

  do { // zeta_loop
    realMultiply(reg6, const__1, &q, realContext); realMultiply(reg7, const__1, &p, realContext);
    PowerComplex(&reg0, const_0, &q, &p, &s, &r, realContext);
    realChangeSign(reg5);
    realMultiply(reg4, reg5, &p, realContext);
    realMultiply(&p, &r, &r, realContext), realMultiply(&p, &s, &s, realContext);
    realAdd(&reg8, &s, &reg8, realContext), realAdd(&reg9, &r, &reg9, realContext);
    realMultiply(&reg0, const_2, &p, realContext);
    realMultiply(&p, &reg0, &p, realContext);
    realSubtract(&p, &reg0, &p, realContext);
    realMultiply(&p, &reg3, &p, realContext);
    realPower(&reg2, const_2, &q, realContext);
    realSubtract(&reg0, const_1, &s, realContext);
    realPower(&s, const_2, &s, realContext);
    realSubtract(&q, &s, &q, realContext);
    realMultiply(&q, const_2, &q, realContext);
    realDivide(&p, &q, &p, realContext);
    realCopy(&p, &reg3);
    realAdd(reg4, &p, reg4, realContext);
    realSubtract(&reg0, const_1, &reg0, realContext);
  } while(realCompareGreaterThan(&reg0, const_0));
  realDivide(&reg8, reg4, &reg8, realContext), realDivide(&reg9, reg4, &reg9, realContext);
  realSubtract(const_1, reg6, &p, realContext);
  realMultiply(const_ln2, &p, &p, realContext);
  WP34S_ExpM1(&p, &reg1, realContext);
  realMultiply(reg7, const__1, &p, realContext);
  realMultiply(&p, const_ln2, &p, realContext);
  realPolarToRectangular(const_1, &p, &q, &p, realContext);
  realSubtract(&q, const_1, &r, realContext);
  realMultiply(&q, &reg1, &q, realContext);
  realMultiply(&reg1, &p, &s, realContext);
  realAdd(&s, &p, &s, realContext);
  realAdd(&q, &r, &q, realContext);

  divComplexComplex(&reg8, &reg9, &q, &s, reg4, reg5, realContext);
}

void ComplexZeta(const real_t *xReal, const real_t *xImag, real_t *resReal, real_t *resImag, realContext_t *realContext) {
  real_t p, q, r, s, reg4, reg5, reg6, reg7, reg10, reg11;

  if(realIsZero(xReal) && realIsZero(xImag)) {
    realCopy(const_1on2, resReal);
    realChangeSign(resReal);
    realZero(resImag);
    return;
  }

  realCopy(xReal, &reg6), realCopy(xImag, &reg7);
  realCopy(xReal, &reg10), realCopy(xImag, &reg11);
  if(realCompareGreaterEqual(xReal, const_1on2)) {
    zeta_calc_complex(&reg4, &reg5, &reg6, &reg7, realContext);
    realCopy(&reg4, resReal), realCopy(&reg5, resImag);
  }
  else { // zeta_neg
    realSubtract(const_1, xReal, &reg6, realContext);
    realChangeSign(&reg7);
    zeta_calc_complex(&reg4, &reg5, &reg6, &reg7, realContext);
    realSubtract(const_1, &reg10, &q, realContext), realSubtract(const_0, &reg11, &p, realContext);
    realDivide(&q, const_2, &q, realContext), realDivide(&p, const_2, &p, realContext);
    WP34S_ComplexGamma(&q, &p, &s, &r, realContext);
    mulComplexComplex(&s, &r, &reg4, &reg5, &reg4, &reg5, realContext);
    realCopy(&reg10, &q), realCopy(&reg11, &p);
    realDivide(&q, const_2, &reg10, realContext), realDivide(&p, const_2, &reg11, realContext);
    realSubtract(&q, const_1on2, &q, realContext);
    PowerComplex(const_pi, const_0, &q, &p, &s, &r, realContext);
    mulComplexComplex(&s, &r, &reg4, &reg5, &reg4, &reg5, realContext);
    WP34S_ComplexGamma(&reg10, &reg11, &q, &p, realContext);

    divComplexComplex(&reg4, &reg5, &q, &p, resReal, resImag, realContext);
  }
}
