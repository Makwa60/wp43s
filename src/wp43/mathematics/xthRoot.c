// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/xthRoot.h"

#include "constantPointers.h"
#include "debug.h"
#include "defines.h"
#include "display.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "items.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/division.h"
#include "mathematics/matrix.h"
#include "mathematics/power.h"
#include "mathematics/toPolar.h"
#include "mathematics/toRect.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"
#include "typeDefinitions.h"

#include "wp43.h"

void xthRootLonILonI(void);
void xthRootLonIShoI(void);
void xthRootLonIReal(void);
void xthRootLonICplx(void);
void xthRootRemaLonI(void);
void xthRootRemaShoI(void);
void xthRootRemaReal(void);
void xthRootRemaCplx(void);
void xthRootCxmaLonI(void);
void xthRootCxmaShoI(void);
void xthRootCxmaReal(void);
void xthRootCxmaCplx(void);
void xthRootShoILonI(void);
void xthRootShoIShoI(void);
void xthRootShoIReal(void);
void xthRootShoICplx(void);
void xthRootRealLonI(void);
void xthRootRealShoI(void);
void xthRootRealReal(void);
void xthRootRealCplx(void);
void xthRootCplxLonI(void);
void xthRootCplxShoI(void);
void xthRootCplxReal(void);
void xthRootCplxCplx(void);
#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void xthRootError   (void);
#else // (EXTRA_INFO_ON_CALC_ERROR == 1)
  #define xthRootError typeError
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

TO_QSPI void (* const xthRoot[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS][NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX |    regY ==>   1                2                3                4             5             6             7                8                9                10
//      V               Long integer     Real34           Complex34        Time          Date          String        Real34 mat       Complex34 m      Short integer    ConfigData
/*  1 Long integer  */ {xthRootLonILonI, xthRootRealLonI, xthRootCplxLonI, xthRootError, xthRootError, xthRootError, xthRootRemaLonI, xthRootCxmaLonI, xthRootShoILonI, xthRootError},
/*  2 Real34        */ {xthRootLonIReal, xthRootRealReal, xthRootCplxReal, xthRootError, xthRootError, xthRootError, xthRootRemaReal, xthRootCxmaReal, xthRootShoIReal, xthRootError},
/*  3 Complex34     */ {xthRootLonICplx, xthRootRealCplx, xthRootCplxCplx, xthRootError, xthRootError, xthRootError, xthRootRemaCplx, xthRootCxmaCplx, xthRootShoICplx, xthRootError},
/*  4 Time          */ {xthRootError,    xthRootError,    xthRootError,    xthRootError, xthRootError, xthRootError, xthRootError,    xthRootError,    xthRootError,    xthRootError},
/*  5 Date          */ {xthRootError,    xthRootError,    xthRootError,    xthRootError, xthRootError, xthRootError, xthRootError,    xthRootError,    xthRootError,    xthRootError},
/*  6 String        */ {xthRootError,    xthRootError,    xthRootError,    xthRootError, xthRootError, xthRootError, xthRootError,    xthRootError,    xthRootError,    xthRootError},
/*  7 Real34 mat    */ {xthRootError,    xthRootError,    xthRootError,    xthRootError, xthRootError, xthRootError, xthRootError,    xthRootError,    xthRootError,    xthRootError},
/*  8 Complex34 mat */ {xthRootError,    xthRootError,    xthRootError,    xthRootError, xthRootError, xthRootError, xthRootError,    xthRootError,    xthRootError,    xthRootError},
/*  9 Short integer */ {xthRootLonIShoI, xthRootRealShoI, xthRootCplxShoI, xthRootError, xthRootError, xthRootError, xthRootRemaShoI, xthRootCxmaShoI, xthRootShoIShoI, xthRootError},
/* 10 Config data   */ {xthRootError,    xthRootError,    xthRootError,    xthRootError, xthRootError, xthRootError, xthRootError,    xthRootError,    xthRootError,    xthRootError}
};

#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void xthRootError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot obtain xthRoot of %s\nto %s",
        getRegisterDataTypeName(REGISTER_Y, true, false),
        getRegisterDataTypeName(REGISTER_X, true, false));
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



void fnXthRoot(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  xthRoot[getRegisterDataType(REGISTER_X)][getRegisterDataType(REGISTER_Y)]();

  adjustResult(REGISTER_X, true, true, REGISTER_X, REGISTER_Y, -1);
}



/* (a+ib) ^ (1/(c+id))
 *
 * \param[in] Expecting a,b,c,d:   Y = a +ib;   X = c +id
 * \return REGISTER Y unchanged. REGISTER X with result of (a+ib) ^ (1/(c+id))
 */
static void _xthRootComplex(const real_t *aa, const real_t *bb, const real_t *cc, const real_t *dd, realContext_t *realContext) {
  real_t theta, a, b, c, d;

  realCopy(aa, &a);
  realCopy(bb, &b);
  realCopy(cc, &c);
  realCopy(dd, &d);

  if(!getSystemFlag(FLAG_SPCRES)) {
    if(realIsZero(&c)&&realIsZero(&d)) {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("0th Root is not defined!");
      return;
     }
    else {
      if(realIsNaN(&a)||realIsNaN(&b)||realIsNaN(&c)||realIsNaN(&d)) {
        reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
        convertRealToReal34ResultRegister(const_NaN, REGISTER_X);
        convertRealToImag34ResultRegister(const_NaN, REGISTER_X);
        return;
      }
    }
  }

  divRealComplex(const_1, &c, &d, &c, &d, realContext);

  //From power.c
  realRectangularToPolar(&a, &b, &a, &theta, realContext);
  WP34S_Ln(&a, &a, realContext);
  realMultiply(&a, &d, &b, realContext);
  realFMA(&theta, &c, &b, &b, realContext);
  realChangeSign(&theta);
  realMultiply(&a, &c, &a, realContext);
  realFMA(&theta, &d, &a, &a, realContext);
  realExp(&a, &c, realContext);
  realPolarToRectangular(const_1, &b, &a, &b, realContext);
  realMultiply(&c, &b, &d, realContext);
  realMultiply(&c, &a, &c, realContext);

  reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
  convertRealToReal34ResultRegister(&c, REGISTER_X);
  convertRealToImag34ResultRegister(&d, REGISTER_X);
}



void xthRootReal(real_t *yy, real_t *xx, realContext_t *realContext) {
  real_t r, o, x, y;
  uint8_t telltale;

  realCopy(xx, &x);
  realCopy(yy, &y);

  telltale = 0;
  if(getSystemFlag(FLAG_SPCRES)) {
    //0
    if(   ((realIsZero(&y)                            && (realCompareGreaterEqual(&x, const_0) || (realIsInfinite(&x) && realIsPositive(&x)))))
       || ((realIsInfinite(&y) && realIsPositive(&y)) && (realCompareLessThan(&x, const_0) && (!realIsInfinite(&x))))
      ) {
      telltale += 1;
      realCopy(const_0, &o);
    }

    //1
    if(((realCompareGreaterEqual(&y, const_0) || (realIsInfinite(&y) && realIsPositive(&y))) && realIsInfinite(&x))) {
      telltale += 2;
      realCopy(const_1, &o);
    }

    //inf
    if(   (realIsZero(&y) && (realCompareLessThan(&x, const_0) && (!realIsInfinite(&x)))) // (y=0.) AND (-inf < x < 0)
       || ((realIsInfinite(&y) && realIsPositive(&y))          && (realCompareGreaterEqual(&x, const_0) && (!realIsInfinite(&x)))) // (y=+inf)  AND (0>= x > inf)
      ) {
      telltale += 4;
      realCopy(const_plusInfinity, &o);
    }

    //NaN
    realDivideRemainder(&x, const_2, &r, realContext);
    if(    (realIsNaN(&x) || realIsNaN(&y))
       || ((realCompareLessThan(&y, const_0) || (realIsInfinite(&y) && realIsNegative(&y))) && (realIsInfinite(&x)   ))                  // (-inf <= y < 0)  AND (x =(inf or -inf))
       || ((realCompareLessThan(&y, const_0) && (!realIsInfinite(&y)                      ) && (!realIsAnInteger(&x))))                  // (-inf < y < 0)  AND (x in non-integer)
       || ((realIsInfinite(&y) && realIsNegative(&y)) && (realIsZero(&r) && realCompareGreaterThan(&x, const_0) && realIsAnInteger(&x))) // (y=-inf) AND (x is even > 0) [zero r means n/2 has no remainder, therefore even]
      ) {
      telltale += 8;
      realCopy(const_NaN, &o);
    }

    //-inf
    realAdd(&x, const_1, &r, realContext);
    realDivideRemainder(&r, const_2, &r, realContext);
    if((realIsInfinite(&y) && realIsNegative(&y)) && (realIsZero(&r) && realCompareGreaterThan(&x, const_0) && realIsAnInteger(&x))) { // (y=-inf) AND (x is odd > 0) [zero r means (n+1)/2 has no remainder, therefore even]
      telltale += 16;
      realCopy(const_minusInfinity, &o);
    }


    if(telltale != 0) {
      reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
      convertRealToReal34ResultRegister(&o, REGISTER_X);
      setRegisterAngularMode(REGISTER_X, amNone);
      return;
    }
  }
  else { // not DANGER
    if(realIsZero(&x)) {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("0th Root is not defined!");
      return;
    }
    else {
      if(realIsNaN(&x)||realIsNaN(&y)) {
        reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
        convertRealToReal34ResultRegister(const_NaN, REGISTER_X);
        setRegisterAngularMode(REGISTER_X, amNone);
        return;
      }
    }
  }

  if(realIsPositive(&y)) {                                         //positive base, no problem, get the power function y^(1/x)
    realDivide(const_1, &x, &x, realContext);

    PowerReal(&y, &x, &x, realContext);
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
    convertRealToReal34ResultRegister(&x, REGISTER_X);
    setRegisterAngularMode(REGISTER_X, amNone);
    return;
  }  //fall through, but returned
  else {
    if(realIsNegative(&y)) {
      realDivideRemainder(&x, const_2, &r, realContext);
      if(realIsZero(&r)) {                                          // negative base and even exp     (zero means no remainder means even)
        if(!getFlag(FLAG_CPXRES)) {
          displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
          errorMoreInfo("cannot do complex xthRoots when CPXRES is not set");
          return;
        }
        else {
          _xthRootComplex(&y, const_0, &x, const_0, realContext);
        }
      } //fall through after Root CC
      else {
        realAdd(&x, const_1, &r, realContext);
        realDivideRemainder(&r, const_2, &r, realContext);
        if(realIsZero(&r)) {                                        // negative base and odd exp
          realDivide(const_1,&x, &x, realContext);

          realSetPositiveSign(&y);
          PowerReal(&y, &x, &x, realContext);
          realSetNegativeSign(&x);

          reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
          convertRealToReal34ResultRegister(&x, REGISTER_X);
          setRegisterAngularMode(REGISTER_X, amNone);
          return;
        } //fall though, but returned
        else {      //neither odd nor even, i.e. not integer
          if(!getFlag(FLAG_CPXRES)) {
            displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
            errorMoreInfo("cannot do complex xthRoots when CPXRES is not set");
            return;
          }
          else {
            _xthRootComplex(&y, const_0, &x, const_0, realContext);
          }
        }
      } //fall through
    }
    else {
      if(realIsZero(&y)) {
        reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);

        if(!realIsZero(&x)) {                                         // zero base and non-zero exp
          convertRealToReal34ResultRegister(const_1, REGISTER_X);
        }
        else {                                                        // zero base and zero exp
          convertRealToReal34ResultRegister(const_NaN, REGISTER_X);
        }

        setRegisterAngularMode(REGISTER_X, amNone);
      } //fall through, but returned
    }
  }
}


/* long integer ^ ... */

void xthRootLonILonI(void) {
  real_t x, y;
  longInteger_t base, exponent, l;

  convertLongIntegerRegisterToLongInteger(REGISTER_Y, base);
  convertLongIntegerRegisterToLongInteger(REGISTER_X, exponent);

  if(longIntegerIsZero(exponent)) {    // 1/0 is not possible
    displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("Cannot divide by 0!");
    longIntegerFree(base);
    longIntegerFree(exponent);
    return;
  }

  if(longIntegerIsZero(base)) {          //base=0 -->  0
    uIntToLongInteger(0, base);
    convertLongIntegerToLongIntegerRegister(base, REGISTER_X);
    longIntegerFree(base);
    longIntegerFree(exponent);
    return;
  }

  if(longIntegerCompareUInt(base, 2147483640) == -1) {
    int32_t exp;

    exp = longIntegerToInt(exponent);
    if(longIntegerIsPositive(base)) {                                 // pos base
      longIntegerInit(l);
      if(longIntegerRoot(base, exp, l)) {                             // if integer xthRoot found, return
        convertLongIntegerToLongIntegerRegister(l, REGISTER_X);
        longIntegerFree(base);
        longIntegerFree(exponent);
        longIntegerFree(l);
        return;
      }
      longIntegerFree(l);
    }
    else {
      if(longIntegerIsNegative(base)) {                                 // neg base and even exponent
        if(longIntegerIsOdd(exponent)) {
          longIntegerChangeSign(base);
          longIntegerInit(l);
          if(longIntegerRoot(base, exp, l)) {                           // if negative integer xthRoot found, return
            longIntegerChangeSign(l);
            convertLongIntegerToLongIntegerRegister(l, REGISTER_X);
            longIntegerFree(base);
            longIntegerFree(exponent);
            longIntegerFree(l);
            return;
          }
          longIntegerFree(l);
          longIntegerChangeSign(base);
        }
      }
    }
  }

  longIntegerToAllocatedString(exponent, tmpString, TMP_STR_LENGTH);
  stringToReal(tmpString, &x, &ctxtReal39);
  longIntegerToAllocatedString(base, tmpString, TMP_STR_LENGTH);
  stringToReal(tmpString, &y, &ctxtReal39);

  longIntegerFree(base);
  longIntegerFree(exponent);

  xthRootReal(&y, &x, &ctxtReal39);
}



void xthRootLonIShoI(void) {
  convertShortIntegerRegisterToLongIntegerRegister(REGISTER_X, REGISTER_X);
  xthRootLonILonI();
}



void xthRootShoILonI(void) {
  uint32_t base = getRegisterShortIntegerBase(REGISTER_Y);

  convertShortIntegerRegisterToLongIntegerRegister(REGISTER_Y, REGISTER_Y);

  xthRootLonILonI();

  if(getRegisterDataType(REGISTER_X) == dtLongInteger) {
    convertLongIntegerRegisterToShortIntegerRegister(REGISTER_X, REGISTER_X);
    setRegisterShortIntegerBase(REGISTER_X, base);
  }
}



void xthRootLonIReal(void) {
  real_t x, y;

  if(!real34IsZero(REGISTER_REAL34_DATA(REGISTER_X)) && real34IsAnInteger(REGISTER_REAL34_DATA(REGISTER_X))) {
    convertReal34ToLongIntegerRegister(REGISTER_REAL34_DATA(REGISTER_X), REGISTER_X, DEC_ROUND_DOWN);
    xthRootLonILonI();
  }
  else {
    convertLongIntegerRegisterToReal(REGISTER_Y, &y, &ctxtReal39);
    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);

    xthRootReal(&y, &x, &ctxtReal39);
  }
}



void xthRootRealLonI(void) {
  real_t x, y;

  convertLongIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);
  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &y);

  xthRootReal(&y, &x, &ctxtReal39);
}



void xthRootLonICplx(void) {
  real_t a, b, c, d;

  convertLongIntegerRegisterToReal(REGISTER_Y, &a, &ctxtReal39);
  real34ToReal(const_0, &b);
  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &c);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &d);

  _xthRootComplex(&a, &b, &c, &d, &ctxtReal39);
 }



void xthRootCplxLonI(void) {
  real_t a, b, c, d;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &a);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_Y), &b);
  convertLongIntegerRegisterToReal(REGISTER_X, &c, &ctxtReal39);
  real34ToReal(const_0, &d);

  _xthRootComplex(&a, &b, &c, &d, &ctxtReal39);
}



/* time ^ ... */

/* date ^ ... */

/* string ^ ... */

/* real34 matrix ^ ... */

void xthRootRemaLonI(void) {
  elementwiseRemaLonI(xthRootRealLonI);
}



void xthRootRemaShoI(void) {
  elementwiseRemaShoI(xthRootRealShoI);
}



void xthRootRemaReal(void) {
  elementwiseRemaReal(xthRootRealReal);
}



void xthRootRemaCplx(void) {
  #if !defined(TESTSUITE_BUILD)
    convertReal34MatrixRegisterToComplex34MatrixRegister(REGISTER_Y, REGISTER_Y);
    xthRootCxmaCplx();
  #endif // !TESTSUITE_BUILD
}



/* complex34 matrix ^ ... */

void xthRootCxmaLonI(void) {
  elementwiseCxmaLonI(xthRootCplxLonI);
}



void xthRootCxmaShoI(void) {
  elementwiseCxmaShoI(xthRootCplxShoI);
}



void xthRootCxmaReal(void) {
  elementwiseCxmaReal(xthRootCplxReal);
}



void xthRootCxmaCplx(void) {
  elementwiseCxmaCplx(xthRootCplxCplx);
}



void xthRootShoIShoI(void) {
  uint32_t base = getRegisterShortIntegerBase(REGISTER_Y);

  convertShortIntegerRegisterToLongIntegerRegister(REGISTER_X, REGISTER_X);
  convertShortIntegerRegisterToLongIntegerRegister(REGISTER_Y, REGISTER_Y);

  xthRootLonILonI();

  if(getRegisterDataType(REGISTER_X) == dtLongInteger) {
    convertLongIntegerRegisterToShortIntegerRegister(REGISTER_X, REGISTER_X);
    setRegisterShortIntegerBase(REGISTER_X, base);
  }
}



void xthRootShoIReal(void) {
  real_t x, y;

  if(!real34IsZero(REGISTER_REAL34_DATA(REGISTER_X)) && real34IsAnInteger(REGISTER_REAL34_DATA(REGISTER_X))) {
    uint32_t base = getRegisterShortIntegerBase(REGISTER_Y);
    convertReal34ToLongIntegerRegister(REGISTER_REAL34_DATA(REGISTER_X), REGISTER_X, DEC_ROUND_DOWN);
    convertShortIntegerRegisterToLongIntegerRegister(REGISTER_Y, REGISTER_Y);

    xthRootLonILonI();

    if(getRegisterDataType(REGISTER_X) == dtLongInteger) {
      convertLongIntegerRegisterToShortIntegerRegister(REGISTER_X, REGISTER_X);
      setRegisterShortIntegerBase(REGISTER_X, base);
    }
  }
  else {
    convertShortIntegerRegisterToReal(REGISTER_Y, &y, &ctxtReal39);
    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);
    xthRootReal(&y, &x, &ctxtReal39);
  }
}



void xthRootRealShoI(void) {
  real_t x, y;

  convertShortIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);
  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &y);

  xthRootReal(&y, &x, &ctxtReal39);
}



void xthRootShoICplx(void) {
  real_t a, b, c, d;

  convertShortIntegerRegisterToReal(REGISTER_Y, &a, &ctxtReal39);
  real34ToReal(const_0, &b);
  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &c);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &d);

  _xthRootComplex(&a, &b, &c, &d, &ctxtReal39);
}



void xthRootCplxShoI(void) {
  real_t a, b, c, d;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &a);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_Y), &b);
  convertShortIntegerRegisterToReal(REGISTER_X, &c, &ctxtReal39);
  real34ToReal(const_0, &d);

  _xthRootComplex(&a, &b, &c, &d, &ctxtReal39);
}



void xthRootRealReal(void) {
  real_t x, y;

  if((real34IsInfinite(REGISTER_REAL34_DATA(REGISTER_X)) || real34IsInfinite(REGISTER_REAL34_DATA(REGISTER_Y))) && !getSystemFlag(FLAG_SPCRES)) {
    displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot use " STD_PLUS_MINUS STD_INFINITY " as X or Y input of xthRoot when flag D is not set");
    return;
  }

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &y);
  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);

  xthRootReal(&y, &x, &ctxtReal39);
}



void xthRootRealCplx(void) {
  real_t a, b, c, d;

  if(real34IsInfinite(REGISTER_REAL34_DATA(REGISTER_Y))) {
    if(real34IsZero(REGISTER_REAL34_DATA(REGISTER_X)) && real34IsZero(REGISTER_IMAG34_DATA(REGISTER_X))) {
      reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
      convertRealToReal34ResultRegister(const_NaN, REGISTER_X);
      convertRealToImag34ResultRegister(const_NaN, REGISTER_X);
    }
    else {
      reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
      convertRealToReal34ResultRegister(const_plusInfinity, REGISTER_X);
      convertRealToImag34ResultRegister(const_plusInfinity, REGISTER_X);
    }
    return;
  }

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &a);
  real34ToReal(const_0, &b);
  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &c);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &d);

  _xthRootComplex(&a, &b, &c, &d, &ctxtReal39);
}



void xthRootCplxReal(void) {
  real_t a, b, c, d;

  if(real34IsInfinite(REGISTER_REAL34_DATA(REGISTER_Y)) || real34IsInfinite(REGISTER_REAL34_DATA(REGISTER_Y))) {
    if(real34IsZero(REGISTER_REAL34_DATA(REGISTER_X))) {
      reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
      convertRealToReal34ResultRegister(const_NaN, REGISTER_X);
      convertRealToImag34ResultRegister(const_NaN, REGISTER_X);
    }
    else {
      reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
      convertRealToReal34ResultRegister(const_plusInfinity, REGISTER_X);
      convertRealToImag34ResultRegister(const_plusInfinity, REGISTER_X);
    }
    return;
  }

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &a);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_Y), &b);
  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &c);
  real34ToReal(const_0, &d);

  _xthRootComplex(&a, &b, &c, &d, &ctxtReal39);
}



void xthRootCplxCplx(void) {
  real_t a, b, c, d;

  if(real34IsInfinite(REGISTER_REAL34_DATA(REGISTER_Y)) || real34IsInfinite(REGISTER_IMAG34_DATA(REGISTER_Y))) {
    if(real34IsZero(REGISTER_REAL34_DATA(REGISTER_X)) && real34IsZero(REGISTER_IMAG34_DATA(REGISTER_X))) {
      reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
      convertRealToReal34ResultRegister(const_NaN, REGISTER_X);
      convertRealToImag34ResultRegister(const_NaN, REGISTER_X);
    }
    else {
      reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
      convertRealToReal34ResultRegister(const_plusInfinity, REGISTER_X);
      convertRealToImag34ResultRegister(const_plusInfinity, REGISTER_X);
    }
    return;
  }

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &a);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_Y), &b);
  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &c);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &d);

  _xthRootComplex(&a, &b, &c, &d, &ctxtReal39);
}
