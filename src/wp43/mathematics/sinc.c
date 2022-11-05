// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/sinc.h"

#include "constantPointers.h"
#include "conversionAngles.h"
#include "debug.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "items.h"
#include "mathematics/division.h"
#include "mathematics/tan.h"
#include "mathematics/wp34s.h"
#include "matrix.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

TO_QSPI void (* const Sinc[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX ==> 1            2         3         4          5          6          7          8           9             10
//          Long integer Real34    Complex34 Time       Date       String     Real34 mat Complex34 m Short integer ConfigData
            sincLonI,    sincReal, sincCplx, sincError, sincError, sincError, sincRema,  sincCxma,   sincError,    sincError
};



/********************************************//**
 * \brief Data type error in sinc
 *
 * \param void
 * \return void
 ***********************************************/
#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void sincError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    sprintf(errorMessage, "cannot calculate Sinc for %s", getRegisterDataTypeName(REGISTER_X, true, false));
    moreInfoOnError("In function fnSinc:", errorMessage, NULL, NULL);
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



/********************************************//**
 * \brief regX ==> regL and sinc(regX) ==> regX
 * enables stack lift and refreshes the stack
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnSinc(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  Sinc[getRegisterDataType(REGISTER_X)]();

  adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
}



void sincComplex(const real_t *real, const real_t *imag, real_t *resReal, real_t *resImag, realContext_t *realContext) {
  // sin(a + ib) = sin(a)*cosh(b) + i*cos(a)*sinh(b)
  // sinc (a + ib) = sin(a + ib) / (a + ib), for the allowable conditions
  real_t rr;
  real_t ii;

  //fnCvtFromCurrentAngularMode(amRadian);

  realCopy(real, &rr);
  realCopy(imag, &ii);

  if(realIsZero(&rr) && realIsZero(&ii)) {
    realCopy(const_1, resReal);
    realCopy(const_0, resImag);
  }
  else {
    real_t sina, cosa, sinhb, coshb, sinR, sinImag;

    WP34S_Cvt2RadSinCosTan(real, amRadian, &sina, &cosa, NULL, realContext);
    WP34S_SinhCosh(imag, &sinhb, &coshb, realContext);

    realMultiply(&sina, &coshb, resReal, realContext);
    realMultiply(&cosa, &sinhb, resImag, realContext);

    realCopy(resReal, &sinR);
    realCopy(resImag, &sinImag);
    divComplexComplex(&sinR, &sinImag, &rr, &ii, resReal, resImag, realContext);
  }
}



void sincLonI(void) {
  real_t x;

  convertLongIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);

  if(realIsZero(&x)) {
    realCopy(const_1, &x);
  }
  else {
    real_t xx;

    realCopy(&x, &xx);
    longIntegerAngleReduction(REGISTER_X, amRadian, &x);
    WP34S_Cvt2RadSinCosTan(&x, amRadian, &x, NULL, NULL, &ctxtReal39);
    realDivide(&x, &xx, &x, &ctxtReal39);
  }

  reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
  convertRealToReal34ResultRegister(&x, REGISTER_X);
}



void sincRema(void) {
  elementwiseRema(sincReal);
}



void sincCxma(void) {
  elementwiseCxma(sincCplx);
}



void sincReal(void) {
  if(real34IsInfinite(REGISTER_REAL34_DATA(REGISTER_X))) {
    if(getSystemFlag(FLAG_SPCRES)) {
      convertRealToReal34ResultRegister(const_0, REGISTER_X);
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function sincReal:", "cannot divide a real34 by " STD_PLUS_MINUS STD_INFINITY " when flag D is not set", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
      return;
    }
  }

  else {
    real_t x;

    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);

    if(realIsZero(&x)) {
      realCopy(const_1, &x);
    }
    else {
      real_t sine;
      angularMode_t registerAngularMode = getRegisterAngularMode(REGISTER_X);
      if(registerAngularMode != amNone) {
        convertAngleFromTo(&x, registerAngularMode, amRadian, &ctxtReal39);
      }
      WP34S_Cvt2RadSinCosTan(&x, amRadian, &sine, NULL, NULL, &ctxtReal39);
      realDivide(&sine, &x, &x, &ctxtReal39);
    }
    convertRealToReal34ResultRegister(&x, REGISTER_X);
  }

  setRegisterAngularMode(REGISTER_X, amNone);
}



void sincCplx(void) {
  real_t zReal, zImag;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &zReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &zImag);

  sincComplex(&zReal, &zImag, &zReal, &zImag, &ctxtReal39);

  convertRealToReal34ResultRegister(&zReal, REGISTER_X);
  convertRealToImag34ResultRegister(&zImag, REGISTER_X);
}
