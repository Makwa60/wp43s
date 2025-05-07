// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/power.h"

#include "constantPointers.h"
#include "debug.h"
#include "error.h"
#include "flags.h"
#include "integers.h"
#include "items.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/exp.h"
#include "mathematics/ln.h"
#include "mathematics/matrix.h"
#include "mathematics/toPolar.h"
#include "mathematics/toRect.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

TO_QSPI void (* const power[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS][NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void) = {
// regX |    regY ==>   1            2            3            4         5         6         7            8            9             10
//      V               Long integer Real34       Complex34    Time      Date      String    Real34 mat   Complex34 m  Short integer Config data
/*  1 Long integer  */ {powLonILonI, powRealLonI, powCplxLonI, powError, powError, powError, powRemaLonI, powCxmaLonI, powShoILonI,  powError},
/*  2 Real34        */ {powLonIReal, powRealReal, powCplxReal, powError, powError, powError, powRemaReal, powCxmaReal, powShoIReal,  powError},
/*  3 Complex34     */ {powLonICplx, powRealCplx, powCplxCplx, powError, powError, powError, powRemaCplx, powCxmaCplx, powShoICplx,  powError},
/*  4 Time          */ {powError,    powError,    powError,    powError, powError, powError, powError,    powError,    powError,     powError},
/*  5 Date          */ {powError,    powError,    powError,    powError, powError, powError, powError,    powError,    powError,     powError},
/*  6 String        */ {powError,    powError,    powError,    powError, powError, powError, powError,    powError,    powError,     powError},
/*  7 Real34 mat    */ {powError,    powError,    powError,    powError, powError, powError, powError,    powError,    powError,     powError},
/*  8 Complex34 mat */ {powError,    powError,    powError,    powError, powError, powError, powError,    powError,    powError,     powError},
/*  9 Short integer */ {powLonIShoI, powRealShoI, powCplxShoI, powError, powError, powError, powRemaShoI, powCxmaShoI, powShoIShoI,  powError},
/* 10 Config data   */ {powError,    powError,    powError,    powError, powError, powError, powError,    powError,    powError,     powError}
};



/********************************************//**
 * \brief Data type error in power
 *
 * \param[in] unusedButMandatoryParameter
 * \return void
 ***********************************************/
#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void powError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    sprintf(errorMessage, "cannot raise %s", getRegisterDataTypeName(REGISTER_Y, true, false));
    sprintf(errorMessage + ERROR_MESSAGE_LENGTH/2, "to %s", getRegisterDataTypeName(REGISTER_X, true, false));
    moreInfoOnError("In function fnPower:", errorMessage, errorMessage + ERROR_MESSAGE_LENGTH/2, NULL);
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



static bool realIsAnOddInteger(const real_t *x) {
  if(realIsAnInteger(x)) {
    real_t y;
    realDivideRemainder(x, const_2, &y, &ctxtReal75);
    return !realIsZero(&y);
  }
  else {
    return false;
  }
}
void PowerReal(const real_t *y, const real_t *x, real_t *res, realContext_t *realContext) {
  real_t lny;
  if((realIsNaN(y) || realIsNaN(x)) && !realCompareEqual(y, const_1) && !realIsZero(x)) {
    realCopy(const_NaN, res);
  }
  else if(realIsZero(y)) {
    const bool isOdd = realIsNegative(y) && !realIsSpecial(x) && !realIsZero(x) && realIsAnOddInteger(x);
    if(realIsZero(x)) {
      realCopy(const_NaN, res);
    }
    else if(realIsNegative(x)) {
      realCopy(const_plusInfinity, res);
    }
    else {
      realZero(res);
    }
    if(isOdd) {
      realChangeSign(res);
    }
  }
  else if(realIsZero(x)) {
    realCopy(const_1, res);
  }
  else if(realCompareEqual(y, const_1)) {
    realCopy(const_1, res);
  }
  else if(realIsInfinite(x) && realCompareEqual(y, const__1)) {
    realCopy(const_1, res);
  }
  else if(realIsInfinite(x) && !realIsZero(y) && !realIsSpecial(y)) {
    if(realCompareAbsLessThan(y, const_1)) {
      realCopy(realIsNegative(x) ? const_plusInfinity : const_0, res);
    }
    else {
      realCopy(realIsPositive(x) ? const_plusInfinity : const_0, res);
    }
  }
  else if(realIsInfinite(y)) {
    const bool isOdd = realIsNegative(y) && !realIsSpecial(x) && realIsAnOddInteger(x);
    realCopy(realIsPositive(x) ? const_plusInfinity : const_0, res);
    if(isOdd) {
      realChangeSign(res);
    }
  }
  else if(realIsNegative(y) && realIsAnInteger(x) && realIsPositive(x)) {
    const bool isOdd = realIsAnOddInteger(x);
    realCopyAbs(y, &lny);
    realLn(&lny, &lny, realContext);
    realMultiply(x, &lny, res, realContext);
    realExp(res, res, realContext);
    fflush(stdout);
    if(isOdd) {
      realChangeSign(res);
    }
  }
  else {
    realLn(y, &lny, realContext);
    realMultiply(x, &lny, res, realContext);
    realExp(res, res, realContext);
  }
}



static bool real34IsAnOddInteger(const real34_t *x) {
  if(real34IsAnInteger(x)) {
    real34_t y;
    real34DivideRemainder(x, const34_2, &y);
    return !real34IsZero(&y);
  }
  else {
    return false;
  }
}
void Power34Real(const real34_t *y, const real34_t *x, real34_t *res) {
  real34_t lny, yy, xx, x1;
  uint32_t exponent = 0;
  bool inv = real34IsNegative(x);
  bool neg = real34IsNegative(y) && real34IsAnInteger(x) && real34IsPositive(x);
  if((real34IsNaN(y) || real34IsNaN(x)) && !real34CompareEqual(y, const34_1) && !real34IsZero(x)) {
    realToReal34(const_NaN, res);
    return;
  }
  else if(real34IsZero(y)) {
    const bool isOdd = real34IsNegative(y) && !real34IsSpecial(x) && !real34IsZero(x) && real34IsAnOddInteger(x);
    if(real34IsZero(x)) {
      realToReal34(const_NaN, res);
    }
    else if(real34IsNegative(x)) {
      realToReal34(const_plusInfinity, res);
    }
    else {
      real34Zero(res);
    }
    if(isOdd) {
      real34ChangeSign(res);
    }
    return;
  }
  else if(real34IsZero(x)) {
    real34Copy(const34_1, res);
    return;
  }
  else if(real34CompareEqual(y, const34_1)) {
    real34Copy(const34_1, res);
    return;
  }
  else if(real34IsInfinite(x) && real34CompareEqual(y, const34__1)) {
    real34Copy(const34_1, res);
    return;
  }
  else if(real34IsInfinite(x) && !real34IsZero(y) && !real34IsSpecial(y)) {
    if(real34CompareAbsLessThan(y, const34_1)) {
      realToReal34(real34IsNegative(x) ? const_plusInfinity : const_0, res);
    }
    else {
      realToReal34(real34IsPositive(x) ? const_plusInfinity : const_0, res);
    }
    return;
  }
  else if(real34IsInfinite(y)) {
    const bool isOdd = real34IsNegative(y) && !real34IsSpecial(x) && real34IsAnOddInteger(x);
    realToReal34(real34IsPositive(x) ? const_plusInfinity : const_0, res);
    if(isOdd) {
      real34ChangeSign(res);
    }
    return;
  }
  real34Copy(y, &yy);
  if(neg) {
    real34ChangeSign(&yy);
  }
  real34CopyAbs(x, &x1);
  real34Ln(&yy, &lny);
  if(real34CompareLessThan(&x1, const34_2p32)) {
    exponent = real34ToUInt32(&x1);
    real34ToIntegralValue(&x1, &xx, DEC_ROUND_DOWN);
    real34Subtract(&x1, &xx, &xx);
  }
  real34Multiply(&xx, &lny, res);
  real34Exp(res, res);
  while(exponent > 0) {
    if(exponent & 0x1) {
      real34Multiply(res, &yy, res);
    }
    exponent >>= 1;
    real34Multiply(&yy, &yy, &yy);
  }
  if(inv) {
    real34Divide(const34_1, res, res);
  }
  if(neg) {
    real34DivideRemainder(x, const34_2, &lny);
    if(!real34IsZero(&lny)) {
      real34ChangeSign(res);
    }
  }
}




/********************************************//**
 * \brief regX ==> regL and regY ^ regX ==> regX
 * Drops Y, enables stack lift and refreshes the stack
 *
 * \param[in] unusedButMandatoryParameter
 * \return void
 ***********************************************/
void fnPower(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  power[getRegisterDataType(REGISTER_X)][getRegisterDataType(REGISTER_Y)]();

  adjustResult(REGISTER_X, true, true, REGISTER_X, REGISTER_Y, -1);
}



/******************************************************************************************************************************************************************************************/
/* long integer ^ ...                                                                                                                                                                     */
/******************************************************************************************************************************************************************************************/

void longIntegerPower(longInteger_t base, longInteger_t exponent, longInteger_t result) {
  if(longIntegerIsZero(exponent)) {
    uIntToLongInteger(1, result);
  }
  else if(longIntegerIsZero(base)) {
    uIntToLongInteger(0, result);
  }
  else if((longIntegerCompareInt(base, 1) == 0 || longIntegerCompareInt(base, -1) == 0) && longIntegerCompareInt(exponent, -1) == 0) {
    longIntegerCopy(base, result);
  }
  else if(longIntegerIsNegative(exponent)) {
    uIntToLongInteger(0, result);
  }
  else {
    uIntToLongInteger(1, result);

    while(!longIntegerIsZero(exponent)) {
      if(longIntegerIsOdd(exponent)) {
       longIntegerMultiply(result, base, result);
      }

      longIntegerDivide2(exponent, exponent);

      if(!longIntegerIsZero(exponent)) {
        longIntegerSquare(base, base);
      }
    }
  }
}



/********************************************//**
 * \brief Y(long integer) ^ X(long integer) ==> X(long integer)
 *
 * \param void
 * \return void
 ***********************************************/
void powLonILonI(void) {
  longInteger_t base, exponent;

  convertLongIntegerRegisterToLongInteger(REGISTER_Y, base);
  convertLongIntegerRegisterToLongInteger(REGISTER_X, exponent);

  if(longIntegerIsZero(exponent) && longIntegerIsZero(base)) {
    displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      moreInfoOnError("In function powLonILonI: Cannot calculate 0^0!", NULL, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

    longIntegerFree(base);
    longIntegerFree(exponent);
    return;
  }

  if((longIntegerCompareInt(base, 1) == 0 || longIntegerCompareInt(base, -1) == 0) && longIntegerCompareInt(exponent, -1) == 0) {
    convertLongIntegerToLongIntegerRegister(base, REGISTER_X);
    longIntegerFree(base);
    longIntegerFree(exponent);
    return;
  }
  else if(longIntegerIsNegative(exponent)) {
    convertLongIntegerRegisterToReal34Register(REGISTER_X, REGISTER_X);
    powLonIReal();
    return;
  }

  longInteger_t result;

  longIntegerInit(result);
  longIntegerPower(base, exponent, result);

  convertLongIntegerToLongIntegerRegister(result, REGISTER_X);

  longIntegerFree(result);
  longIntegerFree(base);
  longIntegerFree(exponent);
}



/********************************************//**
 * \brief Y(long integer) ^ X(short integer) ==> X(long integer)
 *
 * \param void
 * \return void
 ***********************************************/
void powLonIShoI(void) {
  convertShortIntegerRegisterToLongIntegerRegister(REGISTER_X, REGISTER_X);
  powLonILonI();
}



/********************************************//**
 * \brief Y(short integer) ^ X(long integer) ==> X(short integer)
 *
 * \param void
 * \return void
 ***********************************************/
void powShoILonI(void) {
  int32_t base = getRegisterShortIntegerBase(REGISTER_Y);
  real_t x, a;
  convertLongIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);

  convertShortIntegerRegisterToLongIntegerRegister(REGISTER_Y, REGISTER_Y);
  powLonILonI();
  if(realIsPositive(&x)) {
    convertLongIntegerRegisterToReal(REGISTER_X, &a, &ctxtReal39);
    convertLongIntegerRegisterToShortIntegerRegister(REGISTER_X, REGISTER_X);
    setRegisterShortIntegerBase(REGISTER_X, base);
    convertShortIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);
    if(realCompareEqual(&x, &a)) {
      clearSystemFlag(FLAG_OVERFLOW);
    }
    else {
      setSystemFlag(FLAG_OVERFLOW);
    }
  }
}



/********************************************//**
 * \brief Y(long integer) ^ X(real34) ==> X(real34)
 *
 * \param void
 * \return void
 ***********************************************/
void powLonIReal(void) {
  convertLongIntegerRegisterToReal34Register(REGISTER_Y, REGISTER_Y);
  powRealReal();
}



/********************************************//**
 * \brief Y(real34) ^ X(long integer) ==> X(real34)
 *
 * \param void
 * \return void
 ***********************************************/
void powRealLonI(void) {
  convertLongIntegerRegisterToReal34Register(REGISTER_X, REGISTER_X);
  powRealReal();
}



/********************************************//**
 * \brief Y(long integer) ^ X(complex34) ==> X(complex34)
 *
 * \param void
 * \return void
 ***********************************************/
void powLonICplx(void) {
  real_t y;

  convertLongIntegerRegisterToReal(REGISTER_Y, &y, &ctxtReal39);
  reallocateRegister(REGISTER_Y, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
  convertRealToReal34ResultRegister(&y, REGISTER_Y);
  real34Zero(REGISTER_IMAG34_DATA(REGISTER_Y));
  powCplxCplx();
}



/********************************************//**
 * \brief Y(complex34) ^ X(long integer) ==> X(complex34)
 *
 * \param void
 * \return void
 ***********************************************/
void powCplxLonI(void) {
  real_t x;

  convertLongIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);
  reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
  convertRealToReal34ResultRegister(&x, REGISTER_X);
  real34Zero(REGISTER_IMAG34_DATA(REGISTER_X));
  powCplxCplx();
}



/******************************************************************************************************************************************************************************************/
/* time ^ ...                                                                                                                                                                             */
/******************************************************************************************************************************************************************************************/

/******************************************************************************************************************************************************************************************/
/* date ^ ...                                                                                                                                                                             */
/******************************************************************************************************************************************************************************************/

/******************************************************************************************************************************************************************************************/
/* string ^ ...                                                                                                                                                                           */
/******************************************************************************************************************************************************************************************/

/******************************************************************************************************************************************************************************************/
/* real34 matrix ^ ...                                                                                                                                                                    */
/******************************************************************************************************************************************************************************************/

/********************************************//**
 * \brief Y(real34 matrix) ^ X(long integer) ==> X(real34 matrix)
 *
 * \param void
 * \return void
 ***********************************************/
void powRemaLonI(void) {
  elementwiseRemaLonI(powRealLonI);
}



/********************************************//**
 * \brief Y(real34 matrix) ^ X(short integer) ==> X(real34 matrix)
 *
 * \param void
 * \return void
 ***********************************************/
void powRemaShoI(void) {
  elementwiseRemaShoI(powRealShoI);
}



/********************************************//**
 * \brief Y(real34 matrix) ^ X(real34) ==> X(real34 matrix)
 *
 * \param void
 * \return void
 ***********************************************/
void powRemaReal(void) {
  elementwiseRemaReal(powRealReal);
}



/********************************************//**
 * \brief Y(real34 matrix) ^ X(complex34) ==> X(complex34 matrix)
 *
 * \param void
 * \return void
 ***********************************************/
void powRemaCplx(void) {
  convertReal34MatrixRegisterToComplex34MatrixRegister(REGISTER_Y, REGISTER_Y);
  powCxmaCplx();
}



/******************************************************************************************************************************************************************************************/
/* complex34 matrix ^ ...                                                                                                                                                                 */
/******************************************************************************************************************************************************************************************/

/********************************************//**
 * \brief Y(complex34 matrix) ^ X(long integer) ==> X(complex34 matrix)
 *
 * \param void
 * \return void
 ***********************************************/
void powCxmaLonI(void) {
  elementwiseCxmaLonI(powCplxLonI);
}



/********************************************//**
 * \brief Y(complex34 matrix) ^ X(short integer) ==> X(complex34 matrix)
 *
 * \param void
 * \return void
 ***********************************************/
void powCxmaShoI(void) {
  elementwiseCxmaShoI(powCplxShoI);
}



/********************************************//**
 * \brief Y(complex34 matrix) ^ X(real34) ==> X(complex34 matrix)
 *
 * \param void
 * \return void
 ***********************************************/
void powCxmaReal(void) {
  elementwiseCxmaReal(powCplxReal);
}



/********************************************//**
 * \brief Y(complex34 matrix) ^ X(complex34) ==> X(complex34 matrix)
 *
 * \param void
 * \return void
 ***********************************************/
void powCxmaCplx(void) {
  elementwiseCxmaCplx(powCplxCplx);
}



/******************************************************************************************************************************************************************************************/
/* short integer ^ ...                                                                                                                                                                    */
/******************************************************************************************************************************************************************************************/

/********************************************//**
 * \brief Y(short integer) ^ X(short integer) ==> X(short integer)
 *
 * \param void
 * \return void
 ***********************************************/
void powShoIShoI(void) {
  int32_t exponentSign, baseSign;

  uint64_t exponent = WP34S_extract_value(*(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)), &exponentSign);
  uint64_t base = WP34S_extract_value(*(REGISTER_SHORT_INTEGER_DATA(REGISTER_Y)), &baseSign);

  if(base == 1 && exponent == 1 && exponentSign) {
    setRegisterShortIntegerBase(REGISTER_X, getRegisterShortIntegerBase(REGISTER_Y));
    *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) = *(REGISTER_SHORT_INTEGER_DATA(REGISTER_Y));
    return;
  }
  else if(exponentSign) { // exponent is negative
    convertShortIntegerRegisterToReal34Register(REGISTER_X, REGISTER_X);
    convertShortIntegerRegisterToReal34Register(REGISTER_Y, REGISTER_Y);
    powRealReal();
  }
  else {
    setRegisterShortIntegerBase(REGISTER_X, getRegisterShortIntegerBase(REGISTER_Y));
    *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) = WP34S_intPower(*(REGISTER_SHORT_INTEGER_DATA(REGISTER_Y)), *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)));
  }
}



/********************************************//**
 * \brief Y(short integer) ^ X(real34) ==> X(real34)
 *
 * \param void
 * \return void
 ***********************************************/
void powShoIReal(void) {
  convertShortIntegerRegisterToReal34Register(REGISTER_Y, REGISTER_Y);
  powRealReal();
}



/********************************************//**
 * \brief Y(real34) ^ X(short integer) ==> X(real34)
 *
 * \param void
 * \return void
 ***********************************************/
void powRealShoI(void) {
  convertShortIntegerRegisterToReal34Register(REGISTER_X, REGISTER_X);
  powRealReal();
}



/********************************************//**
 * \brief Y(short integer) ^ X(complex34) ==> X(complex34)
 *
 * \param void
 * \return void
 ***********************************************/
void powShoICplx(void) {
  real_t y;

  convertShortIntegerRegisterToReal(REGISTER_Y, &y, &ctxtReal39);
  reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
  convertRealToReal34ResultRegister(&y, REGISTER_Y);
  real34Zero(REGISTER_IMAG34_DATA(REGISTER_Y));
  powCplxCplx();
}



/********************************************//**
 * \brief Y(complex34) ^ X(short integer) ==> X(complex34)
 *
 * \param void
 * \return void
 ***********************************************/
void powCplxShoI(void) {
  real_t x;

  convertShortIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);
  reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
  convertRealToReal34ResultRegister(&x, REGISTER_X);
  real34Zero(REGISTER_IMAG34_DATA(REGISTER_X));
  powCplxCplx();
}



/******************************************************************************************************************************************************************************************/
/* real34 ^ ...                                                                                                                                                                           */
/******************************************************************************************************************************************************************************************/

/********************************************//**
 * \brief Y(real34) ^ X(real34) ==> X(real34)
 *
 * \param void
 * \return void
 ***********************************************/
void powRealReal(void) {
  if(!getSystemFlag(FLAG_SPCRES) && real34IsZero(REGISTER_REAL34_DATA(REGISTER_Y)) && real34IsZero(REGISTER_REAL34_DATA(REGISTER_X))) {
    displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("zeroth power of zero is not defined");
    return;
  }

  real_t y, x;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &y);
  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);

  #if USE_REAL34_FUNCTIONS == 1
    if(getSystemFlag(FLAG_FASTFN)) {
      real34_t r;
      Power34Real(REGISTER_REAL34_DATA(REGISTER_Y), REGISTER_REAL34_DATA(REGISTER_X), &r);
      if(!getFlag(FLAG_CPXRES) || !real34IsNaN(&r)) {
        real34Copy(&r, REGISTER_REAL34_DATA(REGISTER_X));
        setRegisterAngularMode(REGISTER_X, amNone);
        return;
      }
      else {
        realCopy(const_NaN, &x);
      }
    }
    else
  #endif // USE_REAL34_FUNCTIONS == 1
  {
    PowerReal(&y, &x, &x, &ctxtReal39);
  }

  if(getFlag(FLAG_CPXRES) && realIsNaN(&x)) {
    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);

    reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
    convertRealToReal34ResultRegister(&x, REGISTER_X);
    real34Zero(REGISTER_IMAG34_DATA(REGISTER_X));

    reallocateRegister(REGISTER_Y, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
    convertRealToReal34ResultRegister(&y, REGISTER_Y);
    real34Zero(REGISTER_IMAG34_DATA(REGISTER_Y));

    powCplxCplx();
    return;
  }

  convertRealToReal34ResultRegister(&x, REGISTER_X);
  setRegisterAngularMode(REGISTER_X, amNone);
}



/********************************************//**
 * \brief Y(real34) ^ X(complex34) ==> X(complex34)
 *
 * \param void
 * \return void
 ***********************************************/
void powRealCplx(void) {
  real_t y;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &y);
  reallocateRegister(REGISTER_Y, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
  convertRealToReal34ResultRegister(&y, REGISTER_Y);
  real34Zero(REGISTER_IMAG34_DATA(REGISTER_Y));
  powCplxCplx();
}



/********************************************//**
 * \brief Y(complex34) ^ X(real34) ==> X(complex34)
 *
 * \param void
 * \return void
 ***********************************************/
void powCplxReal(void) {
  real_t x;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);
  reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
  convertRealToReal34ResultRegister(&x, REGISTER_X);
  real34Zero(REGISTER_IMAG34_DATA(REGISTER_X));
  powCplxCplx();
}



/******************************************************************************************************************************************************************************************/
/* complex34 + ...                                                                                                                                                                        */
/******************************************************************************************************************************************************************************************/

/*
 * Calculate y^x for complex numbers.
 */
uint8_t PowerComplex(const real_t *yReal, const real_t *yImag, const real_t *xReal, const real_t *xImag, real_t *rReal, real_t *rImag, realContext_t *realContext) {
  uint8_t errorCode = ERROR_NONE;

  if(realIsInfinite(yReal) || realIsInfinite(yImag)) {
      if(realIsZero(xReal) && realIsZero(xImag)) {
          realCopy(const_NaN, rReal);
          realCopy(const_NaN, rImag);
      }
      else {
          realCopy(const_plusInfinity, rReal);
          realCopy(const_plusInfinity, rImag);
      }
  }
  else if(realIsZero(yReal) && realIsZero(yImag)) {
      if(realIsZero(xReal)) {
          realCopy(const_NaN, rReal);
          realCopy(const_NaN, rImag);
      }
      else {
          realCopy(const_0, rReal);
          realCopy(const_0, rImag);
      }
  }
  else {
      real_t theta;
      real_t tmp;

      realRectangularToPolar(yReal, yImag, rReal, &theta, realContext);
      realLn(rReal, rReal, realContext);

      realMultiply(rReal, xImag, rImag, realContext);
      realFMA(&theta, xReal, rImag, rImag, realContext);
      realChangeSign(&theta);

      realMultiply(rReal, xReal, rReal, realContext);
      realFMA(&theta, xImag, rReal, rReal, realContext);

      realExp(rReal, &tmp, realContext);
      realPolarToRectangular(const_1, rImag, rReal, rImag, realContext);
      realMultiply(&tmp, rImag, rImag, realContext);
      realMultiply(&tmp, rReal, rReal, realContext);
  }

  return errorCode;
}

/********************************************//**
 * \brief Y(complex34) ^ X(complex34) ==> X(complex34)
 *
 * \param void
 * \return void
 ***********************************************/
void powCplxCplx(void) {
  real_t yReal, yImag, xReal, xImag;

  real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &yReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_Y), &yImag);
  real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &xReal);
  real34ToReal(REGISTER_IMAG34_DATA(REGISTER_X), &xImag);

  real_t rReal, rImag;

  uint8_t errorCode = PowerComplex(&yReal, &yImag, &xReal, &xImag, &rReal, &rImag, &ctxtReal39);

  if(errorCode!=ERROR_NONE) {
    displayCalcErrorMessage(errorCode, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "cannot raise %s", getRegisterDataTypeName(REGISTER_Y, true, false));
      sprintf(errorMessage + ERROR_MESSAGE_LENGTH/2, "to %s", getRegisterDataTypeName(REGISTER_X, true, false));
      moreInfoOnError("In function fnPower:", errorMessage, errorMessage + ERROR_MESSAGE_LENGTH/2, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
  }
  else {
    convertRealToReal34ResultRegister(&rReal, REGISTER_X);
    convertRealToImag34ResultRegister(&rImag, REGISTER_X);
  }
}
