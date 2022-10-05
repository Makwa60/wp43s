/* This file is part of 43S.
 *
 * 43S is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * 43S is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 43S.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "fractions.h"

#include "constantPointers.h"
#include "debug.h"
#include "error.h"
#include "mathematics/comparisonReals.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"



void fnDenMax(uint16_t unusedButMandatoryParameter) {
  real_t reX;

  if(!saveLastX()) {
    return;
  }

  if(getRegisterDataType(REGISTER_X) == dtReal34) {
    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &reX);
  }
  else if(getRegisterDataType(REGISTER_X) == dtLongInteger) {
    convertLongIntegerRegisterToReal(REGISTER_X, &reX, &ctxtReal39);
  }
  else if(getRegisterDataType(REGISTER_X) == dtShortInteger) {
    convertShortIntegerRegisterToReal(REGISTER_X, &reX, &ctxtReal39);
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      moreInfoOnError("In function fnDenMax:", getRegisterDataTypeName(REGISTER_X, true, false), "cannot be converted!", NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    return;
  }

  if(realIsNaN(&reX)) {
    displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      moreInfoOnError("In function fnDenMax:", "cannot use NaN as X input of fnDenMax", NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    return;
  }

  if(realIsSpecial(&reX) || realCompareLessThan(&reX, const_1) || realCompareGreaterEqual(&reX, const_9999)) {
    denMax = MAX_DENMAX;
  }
  else {
    int32_t den = realToInt32(&reX);

    if(den == 1) {
      longInteger_t lgInt;

      longIntegerInit(lgInt);
      uIntToLongInteger(denMax, lgInt);
      convertLongIntegerToLongIntegerRegister(lgInt, REGISTER_X);
      longIntegerFree(lgInt);
    }
    else {
      denMax = den;
    }
  }
}



void fraction(calcRegister_t regist, int16_t *sign, uint64_t *intPart, uint64_t *numer, uint64_t *denom, int16_t *lessEqualGreater) {
  // temp0 = fractional_part(absolute_value(real number))
  // temp1 = continued fraction calculation --> factional_part(1 / temp1)  initialized with temp0
  // delta = difference between the best faction and the real number

  //printf("0 regist = "); printRegisterToConsole(regist, 0); printf("\n");
  real34_t temp0;

  if(getRegisterDataType(regist) == dtReal34) {
    real34Copy(REGISTER_REAL34_DATA(regist), &temp0);
  }
  else {
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "%s cannot be shown as a fraction!", getRegisterDataTypeName(regist, true, false));
      moreInfoOnError("In function fraction:", errorMessage, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    *sign             = 0;
    *intPart          = 0;
    *numer            = 0;
    *denom            = 0;
    *lessEqualGreater = 0;

    return;
  }

  if(real34IsZero(&temp0)) {
    *sign             = 0;
    *intPart          = 0;
    *numer            = 0;
    *denom            = 1;
    *lessEqualGreater = 0;

    return;
  }

  //printf("1 temp0 = "); printReal34ToConsole(&temp0); printf("\n");
  if(real34IsNegative(&temp0)) {
    *sign = -1;
    real34SetPositiveSign(&temp0);
  }
  else {
    *sign = 1;
  }

  real34_t delta, temp3;
  realToReal34(const_9999, &delta);
  //printf("2 delta = "); printReal34ToConsole(&delta); printf("\n");

  *intPart = real34ToUInt32(&temp0);
  uInt32ToReal34(*intPart, &temp3);
  real34Subtract(&temp0, &temp3, &temp0);
  //printf("3 partie_decimale = temp0 = "); printReal34ToConsole(&temp0); printf("\n");

  //*******************
  //* Any denominator *
  //*******************
  if(getSystemFlag(FLAG_DENANY)) { // denominator up to D.MAX
    uint64_t iPart[20], ex, bestNumer=0, bestDenom=1;
    uint32_t invalidOperation;
    int16_t i, j;

    real34_t temp1, temp4;

    // Calculate the continued fraction
    *denom = 1;
    i = 0;
    iPart[0] = *intPart;

    real34Copy(&temp0, &temp1);
    //printf("4 partie_decimale = temp0 = "); printReal34ToConsole(&temp0); printf("\n");

    if(real34CompareAbsLessThan(&temp1, const34_1e_6)) {
      real34Zero(&temp1);
    }

    decContextClearStatus(&ctxtReal34, DEC_Invalid_operation);
    invalidOperation = 0;
    while(*denom < denMax && !real34IsZero(&temp1) && !invalidOperation) {
      real34Divide(const34_1, &temp1, &temp1);
      //printf("  5 1/partie_decimale = temp1 = "); printReal34ToConsole(&temp1); printf("\n");
      iPart[++i] = real34ToUInt32(&temp1);
      uInt32ToReal34(iPart[i], &temp3);
      invalidOperation = decContextGetStatus(&ctxtReal34) & DEC_Invalid_operation;
      decContextClearStatus(&ctxtReal34, DEC_Invalid_operation);
      real34Subtract(&temp1, &temp3, &temp1);
      if(real34CompareAbsLessThan(&temp1, const34_1e_6)) {
        real34Zero(&temp1);
      }
      //printf("  6 partie_decimale de 1/partie_decimale = temp1 = "); printReal34ToConsole(&temp1); printf("\n");

      *numer = 1;
      *denom = iPart[i];
      //printf("  7 numer=%" PRIu64 " denom=%" PRIu64 "\n", *numer, *denom);
      for(j=i; j>1; j--) {
        *numer += *denom * iPart[j-1];
        ex = *numer; *numer = *denom; *denom = ex;
        //printf("    8 numer=%" PRIu64 " denom=%" PRIu64 "\n", *numer, *denom);
      }
      //printf("  9 numer=%" PRIu64 " denom=%" PRIu64 "\n", *numer, *denom);

      if(*denom <= denMax) {
        uInt32ToReal34(*numer, &temp3);
        //printf("A partie_decimale = temp3 = "); printReal34ToConsole(&temp3); printf("\n");
        uInt32ToReal34(*denom, &temp4);
        //printf("B partie_decimale = temp4 = "); printReal34ToConsole(&temp4); printf("\n");

        //printf("   C temp3 "); printReal34ToConsole(&temp3); printf(" / temp4 "); printReal34ToConsole(&temp4);
        real34Divide(&temp3, &temp4, &temp3);
        //printf(" = temp3 "); printReal34ToConsole(&temp3); printf("\n");


        //printf("   D temp3 "); printReal34ToConsole(&temp3); printf(" - temp0 "); printReal34ToConsole(&temp0);
        real34Subtract(&temp3, &temp0, &temp3);
        //printf(" = temp3 "); printReal34ToConsole(&temp3); printf("\n");

        real34SetPositiveSign(&temp3);
        //printf("   E temp3 = "); printReal34ToConsole(&temp3); printf("\n");

        //printf("   F temp3 "); printReal34ToConsole(&temp3); printf(" - delta "); printReal34ToConsole(&delta);
        real34Subtract(&temp3, &delta, &temp3);
        //printf(" = temp3 "); printReal34ToConsole(&temp3); printf("\n");

        if(real34IsNegative(&temp3)) {
          real34Add(&temp3, &delta, &delta);
          bestNumer = *numer;
          bestDenom = *denom;
          //printf("  G bestNumer=%" PRIu64 " BestDenom=%" PRIu64 "\n", bestNumer, bestDenom);
          //printf("  H delta = "); printReal34ToConsole(&delta); printf("\n");
        }
      }

      *numer = 1;
      *denom = iPart[i] + 1;
      //printf("  I numer=%" PRIu64 " denom=%" PRIu64 "\n", *numer, *denom);
      for(j=i; j>1; j--) {
        *numer += *denom * iPart[j-1];
        ex = *numer; *numer = *denom; *denom = ex;
        //printf("    J numer=%" PRIu64 " denom=%" PRIu64 "\n", *numer, *denom);
      }
      //printf("  K numer=%" PRIu64 " denom=%" PRIu64 "\n", *numer, *denom);

      if(*denom <= denMax) {
        uInt32ToReal34(*numer, &temp3);
        uInt32ToReal34(*denom, &temp4);
        real34Divide(&temp3, &temp4, &temp3);
        real34Subtract(&temp3, &temp0, &temp3);
        real34SetPositiveSign(&temp3);
        real34Subtract(&temp3, &delta, &temp3);
        if(real34IsNegative(&temp3)) {
          real34Add(&temp3, &delta, &delta);
          bestNumer = *numer;
          bestDenom = *denom;
          //printf("  L bestNumer=%" PRIu64 " BestDenom=%" PRIu64 "\n", bestNumer, bestDenom);
          //printf("  M delta = "); printReal34ToConsole(&delta); printf("\n");
        }
      }
    }

    *numer = bestNumer;
    *denom = bestDenom;

    if(*numer == 1 && *denom == 1) {
      *numer = 0;
      *intPart += 1;
    }
  }

  //*******************
  //* Fix denominator *
  //*******************
  else if(getSystemFlag(FLAG_DENFIX)) { // denominator is D.MAX
    *denom = denMax;

    uInt32ToReal34(denMax, &delta);
    real34FMA(&delta, &temp0, const34_1on2, &temp3);
    *numer = real34ToUInt32(&temp3);
  }

  //******************************
  //* Factors of max denominator *
  //******************************
  else { // deniminator is a factor of D.MAX
    uint64_t bestNumer=0, bestDenom=1;

    real34_t temp4;

    // TODO: we can certainly do better here
    for(uint32_t i=1; i<=denMax; i++) {
      if(denMax % i == 0) {
        uInt32ToReal34(i, &temp4);
        real34FMA(&temp4, &temp0, const34_1on2, &temp3);
        *numer = real34ToUInt32(&temp3);

        uInt32ToReal34(*numer, &temp3);
        uInt32ToReal34(i, &temp4);
        real34Divide(&temp3, &temp4, &temp3);
        real34Subtract(&temp3, &temp0, &temp3);
        real34SetPositiveSign(&temp3);
        real34Subtract(&temp3, &delta, &temp3);
        if(real34IsNegative(&temp3)) {
          real34Add(&temp3, &delta, &delta);
          bestNumer = *numer;
          bestDenom = i;
        }
      }
    }

    *numer = bestNumer;
    *denom = bestDenom;
  }

  // The register value
  real_t r;
  real34ToReal(REGISTER_REAL34_DATA(regist), &r);

  // The fraction value
  real_t f, d;
  uInt32ToReal(*intPart, &f);
  uInt32ToReal(*denom, &d);
  realMultiply(&f, &d, &f, &ctxtReal39);
  uInt32ToReal(*numer, &d);
  realAdd(&f, &d, &f, &ctxtReal39);
  uInt32ToReal(*denom, &d);
  realDivide(&f, &d, &f, &ctxtReal39);
  if(*sign == -1) {
    realChangeSign(&f);
  }

  realSubtract(&f, &r, &f, &ctxtReal39);

  if(realIsZero(&f)) {
    *lessEqualGreater = 0;
  }
  else if(realIsNegative(&f)) {
    *lessEqualGreater = -1;
  }
  else {
    *lessEqualGreater = 1;
  }

  if(!getSystemFlag(FLAG_PROPFR)) { // d/c
    *numer += *denom * *intPart;
    *intPart = 0;
  }
}
