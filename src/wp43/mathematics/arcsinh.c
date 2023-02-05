// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/arcsinh.h"

#include "constantPointers.h"
#include "debug.h"
#include "defines.h"
#include "error.h"
#include "flags.h"
#include "items.h"
#include "mathematics/ln.h"
#include "mathematics/matrix.h"
#include "mathematics/toRect.h"
#include "mathematics/toPolar.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

void arcsinhLonI (void);
void arcsinhRema (void);
void arcsinhCxma (void);
void arcsinhReal (void);
void arcsinhCplx (void);
#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void arcsinhError(void);
#else // (EXTRA_INFO_ON_CALC_ERROR == 1)
  #define arcsinhError typeError
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

TO_QSPI void (* const arcsinh[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1             2            3            4             5             6             7            8            9             10
//          Long integer  Real34       Complex34    Time          Date          String        Real34 mat   Complex34 m  Short integer Config data
            arcsinhLonI,  arcsinhReal, arcsinhCplx, arcsinhError, arcsinhError, arcsinhError, arcsinhRema, arcsinhCxma, arcsinhError, arcsinhError
};

#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void arcsinhError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot calculate arcsinh for %s", getRegisterDataTypeName(REGISTER_X, true, false));
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



void fnArcsinh(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  arcsinh[getRegisterDataType(REGISTER_X)]();

  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}



void arcsinhLonI(void) {
  #if USE_REAL34_FUNCTIONS == 1
    if(getSystemFlag(FLAG_FASTFN)) {
      convertLongIntegerRegisterToReal34Register(REGISTER_X, REGISTER_X);
      WP34S_ArSinh34(REGISTER_REAL34_DATA(REGISTER_X), REGISTER_REAL34_DATA(REGISTER_X));
      setRegisterAngularMode(REGISTER_X, amNone);
    }
    else
  #endif // USE_REAL34_FUNCTIONS == 1
  {
    real_t x, xSquared;

    convertLongIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);

    // arcsinh(x) = ln(x + sqrt(x² + 1))
    realMultiply(&x, &x, &xSquared, &ctxtReal39);
    realAdd(&xSquared, const_1, &xSquared, &ctxtReal39);
    realSquareRoot(&xSquared, &xSquared, &ctxtReal39);
    realAdd(&xSquared, &x, &x, &ctxtReal39);
    realLn(&x, &x, &ctxtReal39);

    convertRealToReal34ResultRegister(&x, REGISTER_X);
  }
}



void arcsinhRema(void) {
  elementwiseRema(arcsinhReal);
}



void arcsinhCxma(void) {
  elementwiseCxma(arcsinhCplx);
}



void arcsinhReal(void) {
  #if USE_REAL34_FUNCTIONS == 1
    if(getSystemFlag(FLAG_FASTFN)) {
      if(!real34IsSpecial(REGISTER_REAL34_DATA(REGISTER_X))) {
        WP34S_ArSinh34(REGISTER_REAL34_DATA(REGISTER_X), REGISTER_REAL34_DATA(REGISTER_X));
      }
    }
    else
  #endif // USE_REAL34_FUNCTIONS == 1
  {
    real_t x;

    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);
    ArcsinhReal(&x, &x, &ctxtReal51);
    convertRealToReal34ResultRegister(&x, REGISTER_X);
  }
  setRegisterAngularMode(REGISTER_X, amNone);
}



void arcsinhCplx(void) {
  real_t xReal, xImag, rReal, rImag;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xImag);

  ArcsinhComplex(&xReal, &xImag, &rReal, &rImag, &ctxtReal39);

  convertRealToReal34ResultRegister(&rReal, REGISTER_X);
  convertRealToImag34ResultRegister(&rImag, REGISTER_X);
}



uint8_t ArcsinhReal(const real_t *x, real_t *res, realContext_t *realContext) {
  if(realIsInfinite(x)) {
    realCopy(x, res);
  }
  else {
    WP34S_ArcSinh(x, res, realContext);
  }

  return ERROR_NONE;
}



uint8_t ArcsinhComplex(const real_t *xReal, const real_t *xImag, real_t *rReal, real_t *rImag, realContext_t *realContext) {
  real_t a, b;

  realCopy(xReal, &a);
  realCopy(xImag, &b);

  // arcsinh(z) = ln(z + sqrt(z² + 1))
  // calculate z²   real part
  realMultiply(&b, &b, rReal, realContext);
  realChangeSign(rReal);
  realFMA(&a, &a, rReal, rReal, realContext);

  // calculate z²   imaginary part
  realMultiply(&a, &b, rImag, realContext);
  realMultiply(rImag, const_2, rImag, realContext);

  // calculate z² + 1
  realAdd(rReal, const_1, rReal, realContext);

  // calculate sqrt(z² + 1)
  realRectangularToPolar(rReal, rImag, rReal, rImag, realContext);
  realSquareRoot(rReal, rReal, realContext);
  realMultiply(rImag, const_1on2, rImag, realContext);
  realPolarToRectangular(rReal, rImag, rReal, rImag, realContext);

  // calculate z + sqrt(z² + 1)
  realAdd(&a, rReal, rReal, realContext);
  realAdd(&b, rImag, rImag, realContext);

  // calculate ln(z + sqtr(z² + 1))
  realRectangularToPolar(rReal, rImag, &a, &b, realContext);
  realLn(&a, &a, realContext);

  realCopy(&a, rReal);
  realCopy(&b, rImag);

  return ERROR_NONE;
}
