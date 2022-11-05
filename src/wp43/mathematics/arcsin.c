// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/arcsin.h"

#include "constantPointers.h"
#include "conversionAngles.h"
#include "debug.h"
#include "defines.h"
#include "error.h"
#include "flags.h"
#include "items.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/matrix.h"
#include "mathematics/toRect.h"
#include "mathematics/toPolar.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

void arcsinLonI (void);
void arcsinRema (void);
void arcsinCxma (void);
void arcsinReal (void);
void arcsinCplx (void);
#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void arcsinError(void);
#else // (EXTRA_INFO_ON_CALC_ERROR == 1)
  #define arcsinError typeError
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

TO_QSPI void (* const arcsin[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1            2           3           4            5            6            7           8           9             10
//          Long integer Real34      Complex34   Time         Date         String       Real34 mat  Complex34 m Short integer Config data
            arcsinLonI,  arcsinReal, arcsinCplx, arcsinError, arcsinError, arcsinError, arcsinRema, arcsinCxma, arcsinError,  arcsinError
};

#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void arcsinError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    sprintf(errorMessage, "cannot calculate arcsin for %s", getRegisterDataTypeName(REGISTER_X, true, false));
    moreInfoOnError("In function fnArcsin:", errorMessage, NULL, NULL);
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



void fnArcsin(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  arcsin[getRegisterDataType(REGISTER_X)]();

  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}



void arcsinLonI(void) {
  real_t x;

  convertLongIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);
  if(realCompareAbsGreaterThan(&x, const_1)) {
    if(getFlag(FLAG_CPXRES)) {
      reallocateRegister(REGISTER_X, dtComplex34, TO_BLOCKS(COMPLEX34_SIZE_IN_BYTES), amNone);
      convertRealToReal34ResultRegister(&x, REGISTER_X);
      real34Zero(REGISTER_IMAG34_DATA(REGISTER_X));
      arcsinCplx();
    }
    else if(getSystemFlag(FLAG_SPCRES)) {
      reallocateRegister(REGISTER_X, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), currentAngularMode);
      convertRealToReal34ResultRegister(const_NaN, REGISTER_X);
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function arcsinLonI:", "|X| > 1", "and CPXRES is not set!", NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
    return;
  }

  reallocateRegister(REGISTER_X, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), currentAngularMode);

  if(realIsZero(&x)) {
    real34Zero(REGISTER_REAL34_DATA(REGISTER_X));
  }
  else {
    convertRealToReal34ResultRegister(const_90, REGISTER_X);
    if(realIsNegative(&x)) {
      real34ChangeSign(REGISTER_REAL34_DATA(REGISTER_X));
    }
    convertAngle34FromTo(REGISTER_REAL34_DATA(REGISTER_X), amDegree, currentAngularMode);
  }
}



void arcsinRema(void) {
  elementwiseRema(arcsinReal);
}



void arcsinCxma(void) {
  elementwiseCxma(arcsinCplx);
}



void arcsinReal(void) {
  real_t x;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);
  setRegisterAngularMode(REGISTER_X, currentAngularMode);

  if(realCompareAbsGreaterThan(&x, const_1)) {
    if(getFlag(FLAG_CPXRES)) {
      reallocateRegister(REGISTER_X, dtComplex34, TO_BLOCKS(COMPLEX34_SIZE_IN_BYTES), amNone);
      convertRealToReal34ResultRegister(&x, REGISTER_X);
      real34Zero(REGISTER_IMAG34_DATA(REGISTER_X));
      arcsinCplx();
      return;
    }
    else if(getSystemFlag(FLAG_SPCRES)) {
      convertRealToReal34ResultRegister(const_NaN, REGISTER_X);
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function arcsinReal:", "|X| > 1", "and CPXRES is not set!", NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
      return;
    }
  }

  WP34S_Asin(&x, &x, &ctxtReal39);
  convertAngleFromTo(&x, amRadian, currentAngularMode, &ctxtReal39);
  convertRealToReal34ResultRegister(&x, REGISTER_X);
}



void arcsinCplx(void) {
  real_t xReal, xImag, rReal, rImag;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xImag);

  ArcsinComplex(&xReal, &xImag, &rReal, &rImag, &ctxtReal39);

  convertRealToReal34ResultRegister(&rReal, REGISTER_X);
  convertRealToImag34ResultRegister(&rImag, REGISTER_X);
}

uint8_t ArcsinComplex(const real_t *xReal, const real_t *xImag, real_t *rReal, real_t *rImag, realContext_t *realContext) {
  real_t a, b;

  realCopy(xReal, &a);
  realCopy(xImag, &b);

  // arcsin(z) = -i.ln(iz + sqrt(1 - z�))
  // calculate z�   real part
  realMultiply(&b, &b, rReal, realContext);
  realChangeSign(rReal);
  realFMA(&a, &a, rReal, rReal, realContext);

  // calculate z�   imaginary part
  realMultiply(&a, &b, rImag, realContext);
  realMultiply(rImag, const_2, rImag, realContext);

  // calculate 1 - z�
  realSubtract(const_1, rReal, rReal, realContext);
  realChangeSign(rImag);

  // calculate sqrt(1 - z�)
  realRectangularToPolar(rReal, rImag, rReal, rImag, realContext);
  realSquareRoot(rReal, rReal, realContext);
  realMultiply(rImag, const_1on2, rImag, realContext);
  realPolarToRectangular(rReal, rImag, rReal, rImag, realContext);

  // calculate iz + sqrt(1 - z�)
  realChangeSign(&b);
  realAdd(rReal, &b, rReal, realContext);
  realAdd(rImag, &a, rImag, realContext);

  // calculate ln(iz + sqrt(1 - z�))
  realRectangularToPolar(rReal, rImag, &a, &b, realContext);
  WP34S_Ln(&a, &a, realContext);

  // calculate = -i.ln(iz + sqrt(1 - z�))
  realChangeSign(&a);

  realCopy(&b, rReal);
  realCopy(&a, rImag);

  return ERROR_NONE;
}
