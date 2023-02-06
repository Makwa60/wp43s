// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/arccosh.h"

#include "constantPointers.h"
#include "debug.h"
#include "defines.h"
#include "error.h"
#include "flags.h"
#include "items.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/ln.h"
#include "mathematics/lnPOne.h"
#include "mathematics/matrix.h"
#include "mathematics/squareRoot.h"
#include "mathematics/toRect.h"
#include "mathematics/toPolar.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

void arccoshLonI(void);
void arccoshRema(void);
void arccoshCxma(void);
void arccoshReal(void);
void arccoshCplx(void);
#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void arccoshError(void);
#else // (EXTRA_INFO_ON_CALC_ERROR == 1)
  #define arccoshError typeError
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

TO_QSPI void (* const arccosh[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1            2            3            4             5             6             7            8            9             10
//          Long integer Real34       Complex34    Time          Date          String        Real34 mat   Complex34 m  Short integer Config data
            arccoshLonI, arccoshReal, arccoshCplx, arccoshError, arccoshError, arccoshError, arccoshRema, arccoshCxma, arccoshError, arccoshError
};



#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void arccoshError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot calculate arccosh for %s", getRegisterDataTypeName(REGISTER_X, true, false));
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



void fnArccosh(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  arccosh[getRegisterDataType(REGISTER_X)]();

  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}



void arccoshLonI(void) {
  real_t x;

  convertLongIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);
  if(realCompareLessThan(&x, const_1)) {
    if(getFlag(FLAG_CPXRES)) {
      reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
      convertRealToReal34ResultRegister(&x, REGISTER_X);
      real34Zero(REGISTER_IMAG34_DATA(REGISTER_X));
      arccoshCplx();
    }
    else if(getSystemFlag(FLAG_SPCRES)) {
      reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
      convertRealToReal34ResultRegister(const_NaN, REGISTER_X);
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("X < 1\nand CPXRES is not set!");
    }
    return;
  }

#if USE_REAL34_FUNCTIONS == 1
  if(getSystemFlag(FLAG_FASTFN)) {
    convertLongIntegerRegisterToReal34Register(REGISTER_X, REGISTER_X);
    real34Arcosh(REGISTER_REAL34_DATA(REGISTER_X), REGISTER_REAL34_DATA(REGISTER_X));
    setRegisterAngularMode(REGISTER_X, amNone);
  }
  else
#endif // USE_REAL34_FUNCTIONS == 1
  {
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);

    realArcosh(&x, &x, &ctxtReal39);

    convertRealToReal34ResultRegister(&x, REGISTER_X);
  }
}



void arccoshRema(void) {
  elementwiseRema(arccoshReal);
}



void arccoshCxma(void) {
  elementwiseCxma(arccoshCplx);
}



void arccoshReal(void) {
  if(real34CompareLessThan(REGISTER_REAL34_DATA(REGISTER_X), const34_1)) {
    if(getFlag(FLAG_CPXRES)) {
      real34_t x;
      real34Copy(REGISTER_REAL34_DATA(REGISTER_X), &x);
      reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
      real34Copy(&x, REGISTER_REAL34_DATA(REGISTER_X));
      real34Zero(REGISTER_IMAG34_DATA(REGISTER_X));
      arccoshCplx();
      setRegisterAngularMode(REGISTER_X, amNone);
    }
    else if(getSystemFlag(FLAG_SPCRES)) {
      convertRealToReal34ResultRegister(const_NaN, REGISTER_X);
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("X < 1\nand CPXRES is not set!");
    }
    return;
  }

#if USE_REAL34_FUNCTIONS == 1
  if(getSystemFlag(FLAG_FASTFN)) {
    if(!real34IsSpecial(REGISTER_REAL34_DATA(REGISTER_X))) {
      real34Arcosh(REGISTER_REAL34_DATA(REGISTER_X), REGISTER_REAL34_DATA(REGISTER_X));
    }
  }
  else
#endif // USE_REAL34_FUNCTIONS == 1
  {
    real_t x;

    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);

    realArcosh(&x, &x, &ctxtReal39);

    convertRealToReal34ResultRegister(&x, REGISTER_X);
  }
  setRegisterAngularMode(REGISTER_X, amNone);
}



void arccoshCplx(void) {
  real_t a, b, real, imag;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &a);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &b);

  // arccosh(z) = ln(z + sqrt(z² - 1))
  // calculate z²   real part
  realMultiply(&b, &b, &real, &ctxtReal39);
  realChangeSign(&real);
  realFMA(&a, &a, &real, &real, &ctxtReal39);

  // calculate z²   imaginary part
  realMultiply(&a, &b, &imag, &ctxtReal39);
  realMultiply(&imag, const_2, &imag, &ctxtReal39);

  // calculate z² - 1
  realSubtract(&real, const_1, &real, &ctxtReal39);

  // calculate sqrt(z² - 1)
  realRectangularToPolar(&real, &imag, &real, &imag, &ctxtReal39);
  realSquareRoot(&real, &real, &ctxtReal39);
  realMultiply(&imag, const_1on2, &imag, &ctxtReal39);
  realPolarToRectangular(&real, &imag, &real, &imag, &ctxtReal39);

  // calculate z + sqrt(z² - 1)
  realAdd(&a, &real, &real, &ctxtReal39);
  realAdd(&b, &imag, &imag, &ctxtReal39);

  // calculate ln(z + sqtr(z² - 1))
  realRectangularToPolar(&real, &imag, &a, &b, &ctxtReal39);
  WP34S_Ln(&a, &a, &ctxtReal39);

  convertRealToReal34ResultRegister(&a, REGISTER_X);
  convertRealToImag34ResultRegister(&b, REGISTER_X);
}



void realArcosh(const real_t *x, real_t *res, realContext_t *realContext) {
  real_t xSquared;

  if(realCompareLessThan(x, const_1)) {
    realCopy(const_NaN, res);
    return;
  }
  else if(realCompareEqual(x, const_1)) {
    realZero(res);
    return;
  }

  // arccosh(x) = ln(x + sqrt(x² - 1))
  realMultiply(x, x, &xSquared, realContext);
  realSubtract(&xSquared, const_1, &xSquared, realContext);
  realSquareRoot(&xSquared, &xSquared, realContext);
  if(realCompareLessThan(x, const_2)) {
    // ln(x + a) = ln((x + a)/(1 + a)) + ln(1 + a) where a = sqrt(x² - 1)
    real_t xyx, logSqrtX2m1;
    realLn1P(&xSquared, &logSqrtX2m1, realContext);
    realAdd(x, &xSquared, &xyx, realContext);
    realAdd(&xSquared, const_1, &xSquared, realContext);
    realDivide(&xyx, &xSquared, &xyx, realContext);
    realLn(&xyx, &xyx, realContext);
    realAdd(&xyx, &logSqrtX2m1, res, realContext);
  }
  else {
    realAdd(&xSquared, x, res, realContext);
    realLn(res, res, realContext);
  }
}



void real34Arcosh(const real34_t *x, real34_t *res) {
  real34_t xSquared;

  if(real34CompareLessThan(x, const34_1)) {
    realToReal34(const_NaN, res);
    return;
  }
  else if(real34CompareEqual(x, const34_1)) {
    real34Zero(res);
    return;
  }

  // arccosh(x) = ln(x + sqrt(x² - 1))
  real34Multiply(x, x, &xSquared);
  real34Subtract(&xSquared, const34_1, &xSquared);
  real34SquareRoot(&xSquared, &xSquared);
  if(real34CompareLessThan(x, const34_2)) {
    // ln(x + a) = ln((x + a)/(1 + a)) + ln(1 + a) where a = sqrt(x² - 1)
    real34_t xyx, logSqrtX2m1;
    real34Ln1P(&xSquared, &logSqrtX2m1);
    real34Add(x, &xSquared, &xyx);
    real34Add(&xSquared, const34_1, &xSquared);
    real34Divide(&xyx, &xSquared, &xyx);
    real34Ln(&xyx, &xyx);
    real34Add(&xyx, &logSqrtX2m1, res);
  }
  else {
    real34Add(&xSquared, x, res);
    real34Ln(res, res);
  }
}
