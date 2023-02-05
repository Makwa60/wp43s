// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/logxy.h"

#include "constantPointers.h"
#include "debug.h"
#include "error.h"
#include "flags.h"
#include "integers.h"
#include "items.h"
#include "longIntegerType.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/division.h"
#include "mathematics/integerPart.h"
#include "mathematics/ln.h"
#include "mathematics/matrix.h"
#include "mathematics/power.h"
#include "mathematics/wp34s.h"
#include "realType.h"
#include "registers.h"
#include "registerValueConversions.h"
#include <stdbool.h>

#include "wp43.h"

TO_QSPI void (* const logBaseX[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS][NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])() = {
// regX |    regY ==>    1              2              3              4           5           6           7              8              9              10
//      V                Long integer   Real34         Complex34      Time        Date        String      Real34 mat     Complex34 mat  Short integer  Config data
/*  1 Long integer  */ { logxyLonILonI, logxyRealLonI, logxyCplxLonI, logxyError, logxyError, logxyError, logxyRemaLonI, logxyCxmaLonI, logxyShoILonI, logxyError },
/*  2 Real34        */ { logxyLonIReal, logxyRealReal, logxyCplxReal, logxyError, logxyError, logxyError, logxyRemaReal, logxyCxmaReal, logxyShoIReal, logxyError },
/*  3 Complex34     */ { logxyLonICplx, logxyRealCplx, logxyCplxCplx, logxyError, logxyError, logxyError, logxyRemaCplx, logxyCxmaCplx, logxyShoICplx, logxyError },
/*  4 Time          */ { logxyError,    logxyError,    logxyError,    logxyError, logxyError, logxyError, logxyError,    logxyError,    logxyError,    logxyError },
/*  5 Date          */ { logxyError,    logxyError,    logxyError,    logxyError, logxyError, logxyError, logxyError,    logxyError,    logxyError,    logxyError },
/*  6 String        */ { logxyError,    logxyError,    logxyError,    logxyError, logxyError, logxyError, logxyError,    logxyError,    logxyError,    logxyError },
/*  7 Real34 mat    */ { logxyError,    logxyError,    logxyError,    logxyError, logxyError, logxyError, logxyError,    logxyError,    logxyError,    logxyError },
/*  8 Complex34 mat */ { logxyError,    logxyError,    logxyError,    logxyError, logxyError, logxyError, logxyError,    logxyError,    logxyError,    logxyError },
/*  9 Short integer */ { logxyLonIShoI, logxyRealShoI, logxyCplxShoI, logxyError, logxyError, logxyError, logxyRemaShoI, logxyCxmaShoI, logxyShoIShoI, logxyError },
/* 10 Config data   */ { logxyError,    logxyError,    logxyError,    logxyError, logxyError, logxyError, logxyError,    logxyError,    logxyError,    logxyError }
};

/********************************************//**
 * \brief Data type error in logxy
 *
 * \param void
 * \return void
 ***********************************************/
#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void logxyError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    sprintf(errorMessage, "cannot calculate Log of %s with base %s", getRegisterDataTypeName(REGISTER_Y, true, false), getRegisterDataTypeName(REGISTER_X, true, false));
    moreInfoOnError("In function fnLogXY:", errorMessage, NULL, NULL);
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

/********************************************//**
 * \brief regX ==> regL and log(regX)(RegY) ==> regX
 * enables stack lift and refreshes the stack
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnLogXY(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  logBaseX[getRegisterDataType(REGISTER_X)][getRegisterDataType(REGISTER_Y)]();

  adjustResult(REGISTER_X, true, true, REGISTER_X, -1, -1);
}

static void logXYComplex(const real_t *xReal, const real_t *xImag, const real_t *yReal, const real_t *yImag, real_t *rReal, real_t *rImag, realContext_t *realContext) {
  real_t lnxReal, lnxImag;

  // lnComplex handle the case where y = 0+i0.
  lnComplex(yReal, yImag, rReal, rImag, realContext);                             //   r = Ln(y)
  lnComplex(xReal, xImag, &lnxReal, &lnxImag, realContext);                       // lnx = Ln(x)
  divComplexComplex(rReal, rImag, &lnxReal, &lnxImag, rReal, rImag, realContext); // r = Ln(y) / Ln(x)
}

#define COMPLEX_IS_ZERO(real, imag) (realIsZero(real) && (imag==NULL || realIsZero(imag)))

static bool checkArgs(const real_t *xReal, const real_t *xImag, const real_t *yReal, const real_t *yImag) {
  /*
   * Log(0, 0) = +Inf
   * Log(x, 0) = -Inf x!=0
   * Log(0, y) = NaN  y!=0
   */
  if(COMPLEX_IS_ZERO(xReal, xImag) && COMPLEX_IS_ZERO(yReal, yImag)) {
    displayCalcErrorMessage(ERROR_OVERFLOW_PLUS_INF, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot calculate LogXY with x=0 and y=0");
  }
  else if(!COMPLEX_IS_ZERO(xReal, xImag) && COMPLEX_IS_ZERO(yReal, yImag)) {
    displayCalcErrorMessage(ERROR_OVERFLOW_MINUS_INF, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot calculate LogXY with x=0 and y!=0");
  }
  else if(COMPLEX_IS_ZERO(xReal, xImag) && !COMPLEX_IS_ZERO(yReal, yImag)) {
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
    convertRealToReal34ResultRegister(const_NaN, REGISTER_X);
  }
  else {
    return true;
  }

  return false;
}

static void logxy(const real_t *xReal, const real_t *yReal, realContext_t *realContext) {
  real_t rReal, rImag;

  /*
   * Log(0, 0) = +Inf
   * Log(x, 0) = -Inf x!=0
   * Log(0, y) = NaN  y!=0
   */
  if(checkArgs(xReal, NULL, yReal, NULL)) {
    if(realIsNegative(xReal)) {
      if(getFlag(FLAG_CPXRES)) {
        logXYComplex(xReal, const_0, yReal, const_0, &rReal, &rImag, realContext);

        if(realIsZero(&rImag)) {
          reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
          convertRealToReal34ResultRegister(&rReal, REGISTER_X);
        }
        else {
          reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
          convertRealToReal34ResultRegister(&rReal, REGISTER_X);
          convertRealToImag34ResultRegister(&rImag, REGISTER_X);
        }
      }
      else {
        displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
        errorMoreInfo("cannot calculate LogXY with x<0 when flag I is not set");
      }
    }
    else {
      real_t logBase;
      realLn(yReal, &rReal, realContext);
      realLn(xReal, &logBase, realContext);
      realDivide(&rReal, &logBase, &rReal, realContext);

      reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
      convertRealToReal34ResultRegister(&rReal, REGISTER_X);
    }
  }
}

#if USE_REAL34_FUNCTIONS == 1
static void logxy34(void) {
  if(real34IsSpecial(REGISTER_REAL34_DATA(REGISTER_Y)) || real34IsNegative(REGISTER_REAL34_DATA(REGISTER_Y)) || real34IsZero(REGISTER_REAL34_DATA(REGISTER_Y)) ||
     real34IsSpecial(REGISTER_REAL34_DATA(REGISTER_X)) || real34IsNegative(REGISTER_REAL34_DATA(REGISTER_X)) || real34IsZero(REGISTER_REAL34_DATA(REGISTER_X))) {
      real_t x, y;

      real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &y);
      real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);

      logxy(&x, &y, &ctxtReal39);
  }
  else {
    real34Ln(REGISTER_REAL34_DATA(REGISTER_Y), REGISTER_REAL34_DATA(REGISTER_Y));
    real34Ln(REGISTER_REAL34_DATA(REGISTER_X), REGISTER_REAL34_DATA(REGISTER_X));
    real34Divide(REGISTER_REAL34_DATA(REGISTER_Y), REGISTER_REAL34_DATA(REGISTER_X), REGISTER_REAL34_DATA(REGISTER_X));
  }
}
#endif // USE_REAL34_FUNCTIONS == 1


void logxyLonILonI(void) {
  real_t x, y;
  longInteger_t antilog, yy, xx, rr;

  convertLongIntegerRegisterToLongInteger(REGISTER_Y, antilog);
  convertLongIntegerRegisterToLongInteger(REGISTER_Y, yy);
  convertLongIntegerRegisterToLongInteger(REGISTER_X, xx);

  #if USE_REAL34_FUNCTIONS == 1
    if(getSystemFlag(FLAG_FASTFN)) {
      convertLongIntegerRegisterToReal34Register(REGISTER_Y, REGISTER_Y);
      convertLongIntegerRegisterToReal34Register(REGISTER_X, REGISTER_X);
      logxy34();
    }
    else
  #endif // USE_REAL34_FUNCTIONS == 1
  {
    convertLongIntegerRegisterToReal(REGISTER_Y, &y, &ctxtReal39);
    convertLongIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);

    logxy(&x, &y, &ctxtReal39);
  }

  if(getRegisterDataType(REGISTER_X) == dtReal34 && real34IsAnInteger(REGISTER_REAL34_DATA(REGISTER_X))) {
    longIntegerInit(rr);
    convertReal34ToLongInteger(REGISTER_REAL34_DATA(REGISTER_X), rr, DEC_ROUND_DOWN);
    longIntegerPower(xx, rr, yy);
    fflush(stdout);
    if(longIntegerCompare(antilog, yy) == 0) {
      ipReal();
    }
    longIntegerFree(rr);
  }

  longIntegerFree(xx);
  longIntegerFree(yy);
  longIntegerFree(antilog);
}



void logxyRealLonI(void) {
  #if USE_REAL34_FUNCTIONS == 1
    if(getSystemFlag(FLAG_FASTFN)) {
      convertLongIntegerRegisterToReal34Register(REGISTER_X, REGISTER_X);
      logxy34();
    }
    else
  #endif // USE_REAL34_FUNCTIONS == 1
  {
    real_t x, y;

    real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &y);
    convertLongIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);

    logxy(&x, &y, &ctxtReal39);
  }
}



void logxyCplxLonI(void) {
  real_t xReal, xImag, yReal, yImag, rReal, rImag;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_Y), &yImag);

  convertLongIntegerRegisterToReal(REGISTER_X, &xReal, &ctxtReal39);
  real34ToReal(const34_0, &xImag);

  if(checkArgs(&xReal, &xImag, &yReal, &yImag)) {
    logXYComplex(&xReal, &xImag, &yReal, &yImag, &rReal, &rImag, &ctxtReal39);

    reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
    convertRealToReal34ResultRegister(&rReal, REGISTER_X);
    convertRealToImag34ResultRegister(&rImag, REGISTER_X);
  }
}



void logxyShoILonI(void) {
  real_t x, y;
  int32_t base = getRegisterShortIntegerBase(REGISTER_Y);

  convertShortIntegerRegisterToReal(REGISTER_Y, &y, &ctxtReal39);
  convertLongIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);

  logxy(&x, &y, &ctxtReal39);

  if(getRegisterDataType(REGISTER_X) == dtReal34) {
    if(real34IsNaN(REGISTER_REAL34_DATA(REGISTER_X)) && !getSystemFlag(FLAG_SPCRES)) {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("cannot calculate LogXY with x=0");
      return;
    }
    else if(real34IsAnInteger(REGISTER_REAL34_DATA(REGISTER_X))) {
      clearSystemFlag(FLAG_CARRY);
    }
    else {
      setSystemFlag(FLAG_CARRY);
    }
    fnChangeBase((uint16_t)base);
  }
}



void logxyLonIReal(void) {
  #if USE_REAL34_FUNCTIONS == 1
    if(getSystemFlag(FLAG_FASTFN)) {
      convertLongIntegerRegisterToReal34Register(REGISTER_Y, REGISTER_Y);
      logxy34();
    }
    else
  #endif // USE_REAL34_FUNCTIONS == 1
  {
    real_t x, y;

    convertLongIntegerRegisterToReal(REGISTER_Y, &y, &ctxtReal39);
    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);

    logxy(&x, &y, &ctxtReal39);
  }
}



void logxyRealReal(void) {
  #if USE_REAL34_FUNCTIONS == 1
    if(getSystemFlag(FLAG_FASTFN)) {
      logxy34();
    }
    else
  #endif // USE_REAL34_FUNCTIONS == 1
  {
    real_t x, y;

    real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &y);
    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);

    logxy(&x, &y, &ctxtReal39);
  }
}



void logxyCplxReal(void) {
  real_t xReal, xImag, yReal, yImag, rReal, rImag;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_Y), &yImag);

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(const34_0, &xImag);

  if(checkArgs(&xReal, &xImag, &yReal, &yImag)) {
    logXYComplex(&xReal, &xImag, &yReal, &yImag, &rReal, &rImag, &ctxtReal39);

    reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
    convertRealToReal34ResultRegister(&rReal, REGISTER_X);
    convertRealToImag34ResultRegister(&rImag, REGISTER_X);
  }
}



void logxyShoIReal(void) {
  real_t x, y;
  int32_t base = getRegisterShortIntegerBase(REGISTER_Y);

  convertShortIntegerRegisterToReal(REGISTER_Y, &y, &ctxtReal39);
  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);

  logxy(&x, &y, &ctxtReal39);

  if(getRegisterDataType(REGISTER_X) == dtReal34) {
    if(real34IsNaN(REGISTER_REAL34_DATA(REGISTER_X)) && !getSystemFlag(FLAG_SPCRES)) {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("cannot calculate LogXY with x=0");
      return;
    }
    else if(real34IsAnInteger(REGISTER_REAL34_DATA(REGISTER_X))) {
      clearSystemFlag(FLAG_CARRY);
    }
    else {
      setSystemFlag(FLAG_CARRY);
    }
    fnChangeBase((uint16_t)base);
  }
}



void logxyLonICplx(void) {
  real_t xReal, xImag, yReal, yImag, rReal, rImag;

  convertLongIntegerRegisterToReal(REGISTER_Y, &yReal, &ctxtReal39);
  real34ToReal(const34_0, &yImag);

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xImag);

  if(checkArgs(&xReal, &xImag, &yReal, &yImag)) {
    logXYComplex(&xReal, &xImag, &yReal, &yImag, &rReal, &rImag, &ctxtReal39);

    convertRealToReal34ResultRegister(&rReal, REGISTER_X);
    convertRealToImag34ResultRegister(&rImag, REGISTER_X);
  }
}



void logxyRealCplx(void) {
  real_t xReal, xImag, yReal, yImag, rReal, rImag;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);
  real34ToReal(const34_0, &yImag);

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xImag);

  if(checkArgs(&xReal, &xImag, &yReal, &yImag)) {
    logXYComplex(&xReal, &xImag, &yReal, &yImag, &rReal, &rImag, &ctxtReal39);

    convertRealToReal34ResultRegister(&rReal, REGISTER_X);
    convertRealToImag34ResultRegister(&rImag, REGISTER_X);
  }
}



void logxyCplxCplx(void) {
  real_t xReal, xImag, yReal, yImag, rReal, rImag;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_Y), &yImag);

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xImag);

  if(checkArgs(&xReal, &xImag, &yReal, &yImag)) {
    logXYComplex(&xReal, &xImag, &yReal, &yImag, &rReal, &rImag, &ctxtReal39);

    convertRealToReal34ResultRegister(&rReal, REGISTER_X);
    convertRealToImag34ResultRegister(&rImag, REGISTER_X);
  }
}



void logxyShoICplx(void) {
  real_t xReal, xImag, yReal, yImag, rReal, rImag;

  convertShortIntegerRegisterToReal(REGISTER_Y, &yReal, &ctxtReal39);
  real34ToReal(const34_0, &yImag);

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xImag);

  if(checkArgs(&xReal, &xImag, &yReal, &yImag)) {
    logXYComplex(&xReal, &xImag, &yReal, &yImag, &rReal, &rImag, &ctxtReal39);

    convertRealToReal34ResultRegister(&rReal, REGISTER_X);
    convertRealToImag34ResultRegister(&rImag, REGISTER_X);
  }
}



void logxyLonIShoI(void) {
  convertShortIntegerRegisterToLongIntegerRegister(REGISTER_X, REGISTER_X);
  logxyLonILonI();
}



void logxyRealShoI(void) {
  real_t x, y;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &y);
  convertShortIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);

  logxy(&x, &y, &ctxtReal39);
}



void logxyCplxShoI(void) {
  real_t xReal, xImag, yReal, yImag, rReal, rImag;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_Y), &yImag);

  convertShortIntegerRegisterToReal(REGISTER_X, &xReal, &ctxtReal39);
  real34ToReal(const34_0, &xImag);

  if(checkArgs(&xReal, &xImag, &yReal, &yImag)) {
    logXYComplex(&xReal, &xImag, &yReal, &yImag, &rReal, &rImag, &ctxtReal39);

    reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
    convertRealToReal34ResultRegister(&rReal, REGISTER_X);
    convertRealToImag34ResultRegister(&rImag, REGISTER_X);
  }
}



void logxyShoIShoI(void) {
  real_t x, y;
  int32_t base = getRegisterShortIntegerBase(REGISTER_Y);

  convertShortIntegerRegisterToReal(REGISTER_Y, &y, &ctxtReal39);
  convertShortIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);

  logxy(&x, &y, &ctxtReal39);

  if(getRegisterDataType(REGISTER_X) == dtReal34) {
    if(real34IsNaN(REGISTER_REAL34_DATA(REGISTER_X)) && !getSystemFlag(FLAG_SPCRES)) {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("cannot calculate LogXY with x=0");
      return;
    }
    else if(real34IsAnInteger(REGISTER_REAL34_DATA(REGISTER_X))) {
      clearSystemFlag(FLAG_CARRY);
    }
    else {
      setSystemFlag(FLAG_CARRY);
    }
    fnChangeBase((uint16_t)base);
  }
}



void logxyRemaLonI(void) {
  elementwiseRemaLonI(logxyRealLonI);
}



void logxyCxmaLonI(void) {
  elementwiseCxmaLonI(logxyCplxLonI);
}



void logxyRemaReal(void) {
  elementwiseRemaReal(logxyRealReal);
}



void logxyCxmaReal(void) {
  elementwiseCxmaReal(logxyCplxReal);
}



void logxyRemaCplx(void) {
  #if !defined(TESTSUITE_BUILD)
    convertReal34MatrixRegisterToComplex34MatrixRegister(REGISTER_Y, REGISTER_Y);
    logxyCxmaCplx();
  #endif // !TESTSUITE_BUILD
}



void logxyCxmaCplx(void) {
  elementwiseCxmaCplx(logxyCplxCplx);
}



void logxyRemaShoI(void) {
  elementwiseRemaShoI(logxyRealShoI);
}



void logxyCxmaShoI(void) {
  elementwiseCxmaShoI(logxyCplxShoI);
}
