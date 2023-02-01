// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/lnPOne.h"

#include "constantPointers.h"
#include "debug.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "items.h"
#include "lookupTables.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/ln.h"
#include "mathematics/matrix.h"
#include "mathematics/toPolar.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

TO_QSPI void (* const lnP1[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1            2         3         4          5          6          7          8          9         10
//          Long integer Real34    complex34 Time       Date       String     Real34 mat Complex34 m Short integer Config data
            lnP1LonI,    lnP1Real, lnP1Cplx, lnP1Error, lnP1Error, lnP1Error, lnP1Rema,  lnP1Cxma,  lnP1ShoI, lnP1Error
};



/********************************************//**
 * \brief Data type error in lnP1
 *
 * \param void
 * \return void
 ***********************************************/
#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void lnP1Error(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    sprintf(errorMessage, "cannot calculate Ln(1 + x) for %s", getRegisterDataTypeName(REGISTER_X, true, false));
    moreInfoOnError("In function fnLnP1:", errorMessage, NULL, NULL);
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



/********************************************//**
 * \brief regX ==> regL and lnP1(regX) ==> regX
 * enables stack lift and refreshes the stack
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnLnP1(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  lnP1[getRegisterDataType(REGISTER_X)]();

  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}



// Abramowitz and Stegun §4.1.24
void real34Ln1P(const real34_t *xin, real34_t *res) {
  real34_t x, xx, r;
  int32_t k = 1;

  if(real34CompareAbsGreaterThan(xin, const34_1on2)) {
    real34Add(xin, const34_1, &x);
    real34Ln(&x, res);
    return;
  }

  real34Copy(xin, &x);

  // 1st term
  real34Copy(&x, &xx);
  real34Copy(&x, res);

  // Taylor series
  do {
    real34Copy(res, &r);

    real34Multiply(&xx, &x, &xx);
    if(k < 100) {
      real34FMA(&xx, maclaurinCoeffLn1P34 + (k++), res, res);
    }
    else { // unlikely
      real34_t kp1;
      int32ToReal34(k + 1, &kp1); real34Divide(&xx, &kp1, &kp1);
      if(k % 2 == 1) {
        real34SetNegativeSign(&kp1);
      }
      real34Add(res, &kp1, res);
      ++k;
    }
  } while(!real34CompareEqual(res, &r));
}

void realLn1P(const real_t *xin, real_t *res, realContext_t *realContext) {
  real_t x, xx, r;
  int32_t k = 1;

  if(realCompareAbsGreaterThan(xin, const_1on2)) {
    realAdd(xin, const_1, &x, realContext);
    realLn(&x, res, realContext);
    return;
  }

  realCopy(xin, &x);

  // 1st term
  realCopy(&x, &xx);
  realCopy(&x, res);

  // Taylor series
  do {
    realCopy(res, &r);

    realMultiply(&xx, &x, &xx, realContext);
    //if(k < 100) {
    //  real34FMA(&xx, maclaurinCoeffLn1P34 + (k++), res, res);
    //}
    //else { // unlikely
      real_t kp1;
      int32ToReal(k + 1, &kp1); realDivide(&xx, &kp1, &kp1, realContext);
      if(k % 2 == 1) {
        realSetNegativeSign(&kp1);
      }
      realAdd(res, &kp1, res, realContext);
      ++k;
    //}
  } while(!realCompareEqual(res, &r));
}



void lnP1Complex(const real_t *real, const real_t *imag, real_t *lnReal, real_t *lnImag, realContext_t *realContext) {
  real_t r;

  realAdd(real, const_1, &r, realContext);
  if(realIsZero(&r) && realIsZero(imag)) {
    realCopy(const_minusInfinity, lnReal);
    realZero(lnImag);
  }
  else if(realIsPositive(&r) && realIsZero(imag)) {
    WP34S_Ln1P(real, lnReal, realContext);
    realZero(lnImag);
  }
  else {
    realRectangularToPolar(&r, imag, lnReal, lnImag, realContext);
    WP34S_Ln(lnReal, lnReal, realContext);
  }
}



/**********************************************************************
 * In all the functions below:
 * if X is a number then X = a + ib
 * 1 added to X
 * The variables a and b are used for intermediate calculations
 ***********************************************************************/

void lnP1LonI(void) {
  longInteger_t lgInt;

  convertLongIntegerRegisterToLongInteger(REGISTER_X, lgInt);
  longIntegerAddUInt(lgInt, 1, lgInt);

  if(longIntegerIsZero(lgInt)) {
    if(getSystemFlag(FLAG_SPCRES)) {
      reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
      convertRealToReal34ResultRegister(const_minusInfinity, REGISTER_X);
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function lnP1LonI:", "cannot calculate Ln(0) in Ln(1 + x)", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
  }
  #if USE_REAL34_FUNCTIONS == 1
    else if(getSystemFlag(FLAG_FASTFN) && longIntegerIsPositive(lgInt)) {
      convertLongIntegerRegisterToReal34Register(REGISTER_X, REGISTER_X);
      real34Ln1P(REGISTER_REAL34_DATA(REGISTER_X), REGISTER_REAL34_DATA(REGISTER_X));
      setRegisterAngularMode(REGISTER_X, amNone);
    }
  #endif // USE_REAL34_FUNCTIONS == 1
  else {
    real_t x;

    convertLongIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);
    realAdd(&x, const_1, &x, &ctxtReal39);

    if(longIntegerIsPositive(lgInt)) {
      realLn(&x, &x, &ctxtReal39);
      reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
      convertRealToReal34ResultRegister(&x, REGISTER_X);
     }
    else if(getFlag(FLAG_CPXRES)) {
      realSetPositiveSign(&x);
      realLn(&x, &x, &ctxtReal39);
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
        moreInfoOnError("In function lnP1LonI:", "cannot calculate Ln of a negative number when CPXRES is not set!", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
  }

  longIntegerFree(lgInt);
}



void lnP1Rema(void) {
  elementwiseRema(lnP1Real);
}



void lnP1Cxma(void) {
  elementwiseCxma(lnP1Cplx);
}



void lnP1ShoI(void) {
  real_t x;

  convertShortIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);
  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
  realAdd(&x, const_1, &x, &ctxtReal39);

  if(realIsZero(&x)) {
    if(getSystemFlag(FLAG_SPCRES)) {
      convertRealToReal34ResultRegister(const_minusInfinity, REGISTER_X);
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function lnP1ShoI:", "cannot calculate Ln(0) in Ln(1 + x)", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
  }
  else {
    if(realIsPositive(&x)) {
      realLn(&x, &x, &ctxtReal39);
      convertRealToReal34ResultRegister(&x, REGISTER_X);
     }
    else if(getFlag(FLAG_CPXRES)) {
      realSetPositiveSign(&x);
      realLn(&x, &x, &ctxtReal39);
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
        moreInfoOnError("In function lnP1ShoI:", "cannot calculate Ln of a negative number when CPXRES is not set!", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
  }
}



void lnP1Real(void) {
  #if USE_REAL34_FUNCTIONS == 1
    if(getSystemFlag(FLAG_FASTFN) && !real34IsSpecial(REGISTER_REAL34_DATA(REGISTER_X)) && real34CompareGreaterThan(REGISTER_REAL34_DATA(REGISTER_X), const34__1)) {
      real34Ln1P(REGISTER_REAL34_DATA(REGISTER_X), REGISTER_REAL34_DATA(REGISTER_X));
      setRegisterAngularMode(REGISTER_X, amNone);
      return;
    }
  #endif // USE_REAL34_FUNCTIONS == 1

  real_t x;
  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);

  if(realIsNaN(&x)) {
    convertRealToReal34ResultRegister(const_NaN, REGISTER_X);
  }

  else if(realCompareEqual(&x, const__1)) {
    if(getSystemFlag(FLAG_SPCRES)) {
      convertRealToReal34ResultRegister(const_minusInfinity, REGISTER_X);
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function lnP1Real:", "cannot calculate Ln(0) in Ln(1 + x)", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
  }

  else if(realIsInfinite(&x)) {
    if(!getSystemFlag(FLAG_SPCRES)) {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function lnP1Real:", "cannot use " STD_PLUS_MINUS STD_INFINITY " as X input of ln(x+1) when flag D is not set", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
      return;
    }
    else if(getFlag(FLAG_CPXRES)) {
      if(realIsPositive(&x)) {
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

  else {
    if(realCompareGreaterThan(&x, const__1)) {
      realLn1P(&x, &x, &ctxtReal39);
      convertRealToReal34ResultRegister(&x, REGISTER_X);
    }
    else if(getFlag(FLAG_CPXRES)) {
      realAdd(&x, const_1, &x, &ctxtReal39);
      realSetPositiveSign(&x);
      realLn(&x, &x, &ctxtReal39);
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
        moreInfoOnError("In function lnP1Real:", "cannot calculate Ln of a negative number when CPXRES is not set!", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
  }
  setRegisterAngularMode(REGISTER_X, amNone);
}



void lnP1Cplx(void) {
  real34_t r;
  int32ToReal34(1, &r);
  real34Add(REGISTER_REAL34_DATA(REGISTER_X),&r,&r);
  if(real34IsZero(&r) && real34IsZero(REGISTER_IMAG34_DATA(REGISTER_X))) {
    if(getSystemFlag(FLAG_SPCRES)) {
      convertRealToReal34ResultRegister(const_minusInfinity, REGISTER_X);
      real34Zero(REGISTER_IMAG34_DATA(REGISTER_X));
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function lnP1Cplx:", "cannot calculate Ln(0) in Ln(1 + x)", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
  }
  else {
    real_t xReal, xImag;

    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
    real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xImag);

    lnP1Complex(&xReal, &xImag, &xReal, &xImag, &ctxtReal75);

    convertRealToReal34ResultRegister(&xReal, REGISTER_X);
    convertRealToImag34ResultRegister(&xImag, REGISTER_X);
  }
}
