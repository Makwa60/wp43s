// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/ln.h"

#include "constantPointers.h"
#include "debug.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "items.h"
#include "lookupTables.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/toPolar.h"
#include "mathematics/matrix.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

TO_QSPI void (* const ln[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1            2       3         4        5        6        7          8           9             10
//          Long integer Real34  complex34 Time     Date     String   Real34 mat Complex34 m Short integer Config data
            lnLonI,      lnReal, lnCplx,   lnError, lnError, lnError, lnRema,    lnCxma,     lnShoI,       lnError
};



/********************************************//**
 * \brief Data type error in ln
 *
 * \param void
 * \return void
 ***********************************************/
#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void lnError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    sprintf(errorMessage, "cannot calculate Ln for %s", getRegisterDataTypeName(REGISTER_X, true, false));
    moreInfoOnError("In function fnLn:", errorMessage, NULL, NULL);
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



/********************************************//**
 * \brief regX ==> regL and ln(regX) ==> regX
 * enables stack lift and refreshes the stack
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnLn(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  ln[getRegisterDataType(REGISTER_X)]();

  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}



// Abramowitz and Stegun §4.1.27
void real34Ln(const real34_t *xin, real34_t *res) {
  real34_t x, xm1xp1, xm1xp1sq, xm1xp1pw, r;
  int32_t exponent = 0, exponent2 = 0;
  int32_t k = 1;

  // Special cases
  if(real34IsZero(xin)) {
    realToReal34(const_minusInfinity, res);
    return;
  }
  else if(real34IsNegative(xin) || real34IsNaN(xin)) {
    realToReal34(const_NaN, res);
    return;
  }
  else if(real34IsInfinite(xin)) {
    realToReal34(const_plusInfinity, res);
    return;
  }
  else if(real34CompareEqual(xin, const34_1)) {
    real34Zero(res);
    return;
  }

  real34Copy(xin, &x);

  // reduce value to make convergence faster
  exponent = real34GetExponent(&x);
  real34SetExponent(&x, 0);
  for(int k = -1; real34CompareGreaterEqual(&x, const34_10); --k) {
    real34SetExponent(&x, k);
    ++exponent;
  }
  while(real34CompareGreaterEqual(&x, const34_2)) {
    real34Multiply(&x, const34_1on2, &x);
    ++exponent2;
  }

  // 1st term
  real34Copy(&x, &xm1xp1);
  real34Copy(&x, &xm1xp1sq);
  real34Subtract(&xm1xp1, const34_1, &xm1xp1);
  real34Add(&xm1xp1sq, const34_1, &xm1xp1sq);
  real34Divide(&xm1xp1, &xm1xp1sq, &xm1xp1);
  real34Multiply(&xm1xp1, &xm1xp1, &xm1xp1sq);

  real34Copy(&xm1xp1, &xm1xp1pw);
  real34Add(&xm1xp1, &xm1xp1, res);

  // Taylor series
  do {
    real34Copy(res, &r);

    real34Multiply(&xm1xp1pw, &xm1xp1sq, &xm1xp1pw);
    if(k < 100) {
      real34FMA(&xm1xp1pw, taylorCoeffLn34 + (k++), res, res);
    }
    else {
      real34_t kx2p1;
      int32ToReal34(k * 2 + 1, &kx2p1); real34Divide(&xm1xp1pw, &kx2p1, &kx2p1);
      real34Add(res, &kx2p1, res);
      real34Add(res, &kx2p1, res);
      ++k;
    }
  } while(!real34CompareEqual(res, &r));

  // Add pre-calculated logarithm
  int32ToReal34(exponent2, &r);
  real34FMA(const34_ln2, &r, res, res);
  int32ToReal34(exponent, &r);
  real34FMA(const34_ln10, &r, res, res);
}



void lnComplex(const real_t *real, const real_t *imag, real_t *lnReal, real_t *lnImag, realContext_t *realContext) {
  if(realIsZero(real) && realIsZero(imag)) {
    realCopy(const_minusInfinity, lnReal);
    realZero(lnImag);
  }
  else {
    realRectangularToPolar(real, imag, lnReal, lnImag, realContext);
    WP34S_Ln(lnReal, lnReal, realContext);
  }
}



/**********************************************************************
 * In all the functions below:
 * if X is a number then X = a + ib
 * The variables a and b are used for intermediate calculations
 ***********************************************************************/

void lnLonI(void) {
  longInteger_t lgInt;

  convertLongIntegerRegisterToLongInteger(REGISTER_X, lgInt);

  if(longIntegerIsZero(lgInt)) {
    if(getSystemFlag(FLAG_SPCRES)) {
      reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
      convertRealToReal34ResultRegister(const_minusInfinity, REGISTER_X);
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function lnLonI:", "cannot calculate Ln(0)", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
  }
  #if USE_REAL34_FUNCTIONS == 1
    else if(getSystemFlag(FLAG_FASTFN) && longIntegerIsPositive(lgInt)) {
      convertLongIntegerRegisterToReal34Register(REGISTER_X, REGISTER_X);
      real34Ln(REGISTER_REAL34_DATA(REGISTER_X), REGISTER_REAL34_DATA(REGISTER_X));
      setRegisterAngularMode(REGISTER_X, amNone);
    }
  #endif // USE_REAL34_FUNCTIONS == 1
  else {
    real_t x;

    convertLongIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);

    if(longIntegerIsPositive(lgInt)) {
      WP34S_Ln(&x, &x, &ctxtReal39);
      reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
      convertRealToReal34ResultRegister(&x, REGISTER_X);
     }
    else if(getFlag(FLAG_CPXRES)) {
      realSetPositiveSign(&x);
      WP34S_Ln(&x, &x, &ctxtReal39);
      reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
      convertRealToReal34ResultRegister(&x, REGISTER_X);
      convertRealToImag34ResultRegister(const_pi, REGISTER_X);
    }
    else if(getSystemFlag(FLAG_SPCRES)) {
      reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
      convertRealToReal34ResultRegister(const_NaN, REGISTER_X);
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function lnLonI:", "cannot calculate Ln of a negative number when CPXRES is not set!", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
  }

  longIntegerFree(lgInt);
}



void lnRema(void) {
  elementwiseRema(lnReal);
}



void lnCxma(void) {
  elementwiseCxma(lnCplx);
}



void lnShoI(void) {
  real_t x;

  convertShortIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);
  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);

  if(realIsZero(&x)) {
    if(getSystemFlag(FLAG_SPCRES)) {
      convertRealToReal34ResultRegister(const_minusInfinity, REGISTER_X);
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function lnShoI:", "cannot calculate Ln(0)", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
  }
  else {
    if(realIsPositive(&x)) {
      WP34S_Ln(&x, &x, &ctxtReal39);
      convertRealToReal34ResultRegister(&x, REGISTER_X);
     }
    else if(getFlag(FLAG_CPXRES)) {
      realSetPositiveSign(&x);
      WP34S_Ln(&x, &x, &ctxtReal39);
      reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
      convertRealToReal34ResultRegister(&x, REGISTER_X);
      convertRealToImag34ResultRegister(const_pi, REGISTER_X);
    }
    else if(getSystemFlag(FLAG_SPCRES)) {
      convertRealToReal34ResultRegister(const_NaN, REGISTER_X);
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function lnShoI:", "cannot calculate Ln of a negative number when CPXRES is not set!", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
  }
}



void lnReal(void) {
  if(real34IsZero(REGISTER_REAL34_DATA(REGISTER_X))) {
    if(getSystemFlag(FLAG_SPCRES)) {
      convertRealToReal34ResultRegister(const_minusInfinity, REGISTER_X);
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function lnReal:", "cannot calculate Ln(0)", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
  }

  else if(real34IsInfinite(REGISTER_REAL34_DATA(REGISTER_X))) {
    if(!getSystemFlag(FLAG_SPCRES)) {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function lnReal:", "cannot use " STD_PLUS_MINUS STD_INFINITY " as X input of ln when flag D is not set", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
      return;
    }
    else if(getFlag(FLAG_CPXRES)) {
      if(real34IsPositive(REGISTER_REAL34_DATA(REGISTER_X))) {
        convertRealToReal34ResultRegister(const_plusInfinity, REGISTER_X);
      }
      else {
        reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
        convertRealToReal34ResultRegister(const_plusInfinity, REGISTER_X);
        convertRealToImag34ResultRegister(const_pi, REGISTER_X);
      }
    }
    else {
      convertRealToReal34ResultRegister(const_NaN, REGISTER_X);
    }
  }

  #if USE_REAL34_FUNCTIONS == 1
    else if(getSystemFlag(FLAG_FASTFN) && real34IsPositive(REGISTER_REAL34_DATA(REGISTER_X))) {
      real34Ln(REGISTER_REAL34_DATA(REGISTER_X), REGISTER_REAL34_DATA(REGISTER_X));
    }
  #endif // USE_REAL34_FUNCTIONS == 1

  else {
    real_t x;

    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);
    if(real34IsPositive(REGISTER_REAL34_DATA(REGISTER_X))) {
      WP34S_Ln(&x, &x, &ctxtReal39);
      convertRealToReal34ResultRegister(&x, REGISTER_X);
    }
    else if(getFlag(FLAG_CPXRES)) {
      realSetPositiveSign(&x);
      WP34S_Ln(&x, &x, &ctxtReal39);
      reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
      convertRealToReal34ResultRegister(&x, REGISTER_X);
      convertRealToImag34ResultRegister(const_pi, REGISTER_X);
    }
    else if(getSystemFlag(FLAG_SPCRES)) {
      convertRealToReal34ResultRegister(const_NaN, REGISTER_X);
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function lnReal:", "cannot calculate Ln of a negative number when CPXRES is not set!", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
  }
  setRegisterAngularMode(REGISTER_X, amNone);
}



void lnCplx(void) {
  if(real34IsZero(REGISTER_REAL34_DATA(REGISTER_X)) && real34IsZero(REGISTER_IMAG34_DATA(REGISTER_X))) {
    if(getSystemFlag(FLAG_SPCRES)) {
      convertRealToReal34ResultRegister(const_minusInfinity, REGISTER_X);
      real34Zero(REGISTER_IMAG34_DATA(REGISTER_X));
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function lnCplx:", "cannot calculate Ln(0)", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
  }
  else {
    real_t xReal, xImag;

    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
    real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xImag);

    lnComplex(&xReal, &xImag, &xReal, &xImag, &ctxtReal39);

    convertRealToReal34ResultRegister(&xReal, REGISTER_X);
    convertRealToImag34ResultRegister(&xImag, REGISTER_X);
  }
}
