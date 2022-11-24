// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/arccos.h"

#include "constantPointers.h"
#include "conversionAngles.h"
#include "debug.h"
#include "defines.h"
#include "error.h"
#include "flags.h"
#include "items.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/ln.h"
#include "mathematics/multiplication.h"
#include "mathematics/sqrt1Px2.h"
#include "mathematics/wp34s.h"
#include "matrix.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void arccosError(void);
#else // (EXTRA_INFO_ON_CALC_ERROR == 1)
  #define arccosError typeError
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

void arccosLonI (void);
void arccosRema (void);
void arccosCxma (void);
void arccosReal (void);
void arccosCplx (void);

TO_QSPI void (* const arccos[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1            2           3           4            5            6            7           8           9             10
//          Long integer Real34      Complex34   Time         Date         String       Real34 mat  Complex34 m Short integer Config data
            arccosLonI,  arccosReal, arccosCplx, arccosError, arccosError, arccosError, arccosRema, arccosCxma, arccosError,  arccosError
};

#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void arccosError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot calculate arccos for %s", getRegisterDataTypeName(REGISTER_X, true, false));
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



void fnArccos(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  arccos[getRegisterDataType(REGISTER_X)]();

  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}



void arccosLonI(void) {
  real_t x;

  convertLongIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);
  if(realCompareAbsGreaterThan(&x, const_1)) {
    if(getFlag(FLAG_CPXRES)) {
      reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
      convertRealToReal34ResultRegister(&x, REGISTER_X);
      real34Zero(REGISTER_IMAG34_DATA(REGISTER_X));
      arccosCplx();
    }
    else if(getSystemFlag(FLAG_SPCRES)) {
      reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, currentAngularMode);
      convertRealToReal34ResultRegister(const_NaN, REGISTER_X);
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("|X| > 1\nand CPXRES is not set!");
    }
    return;
  }

  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, currentAngularMode);

  if(realIsZero(&x)) {
    convertRealToReal34ResultRegister(const_90, REGISTER_X);
    convertAngle34FromTo(REGISTER_REAL34_DATA(REGISTER_X), amDegree, currentAngularMode);
  }
  else {
    if(realIsNegative(&x)) {
      convertRealToReal34ResultRegister(const_180, REGISTER_X);
      convertAngle34FromTo(REGISTER_REAL34_DATA(REGISTER_X), amDegree, currentAngularMode);
    }
    else{
      real34Zero(REGISTER_REAL34_DATA(REGISTER_X));
    }
  }
}



void arccosRema(void) {
  elementwiseRema(arccosReal);
}



void arccosCxma(void) {
  elementwiseCxma(arccosCplx);
}



void arccosReal(void) {
  real_t x;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);
  setRegisterAngularMode(REGISTER_X, currentAngularMode);

  if(realCompareAbsGreaterThan(&x, const_1)) {
    if(getFlag(FLAG_CPXRES)) {
      reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
      convertRealToReal34ResultRegister(&x, REGISTER_X);
      real34Zero(REGISTER_IMAG34_DATA(REGISTER_X));
      arccosCplx();
    }
    else if(getSystemFlag(FLAG_SPCRES)) {
      convertRealToReal34ResultRegister(const_NaN, REGISTER_X);
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("|X| > 1\nand CPXRES is not set!");
    }
    return;
  }

  WP34S_Acos(&x, &x, &ctxtReal39);
  convertAngleFromTo(&x, amRadian, currentAngularMode, &ctxtReal39);
  convertRealToReal34ResultRegister(&x, REGISTER_X);
}



void arccosCplx(void) {
  real_t a, b, real, imag;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &a);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &b);

  // arccos(z) = -i.ln(z + i.sqrt(1 - z²))
  // calculate i.sqrt(1 - z²)
  realMinus(&b, &real, &ctxtReal39);
  sqrt1Px2Complex(&real, &a, &imag, &real, &ctxtReal39);
  realChangeSign(&real);

  // calculate z + i.sqrt(1 - z²)
  realAdd(&a, &real, &real, &ctxtReal39);
  realAdd(&b, &imag, &imag, &ctxtReal39);

  // calculate ln(z + i.sqrt(1 - z²))
  lnComplex(&real, &imag, &real, &imag, &ctxtReal39);

  // calculate = -i.ln(z + i.sqrt(1 - z²))
  realMinus(&real, &a, &ctxtReal39);
  realCopy(&imag, &b);

  convertRealToReal34ResultRegister(&b, REGISTER_X);
  convertRealToImag34ResultRegister(&a, REGISTER_X);
}
