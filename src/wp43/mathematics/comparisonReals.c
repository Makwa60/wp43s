// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/comparisonReals.h"

#include "constantPointers.h"
#include "registerValueConversions.h"
#include <stdbool.h>

#include "wp43.h"

bool real34CompareAbsGreaterThan(const real34_t *number1, const real34_t *number2) {
  real34_t num1, num2;

  real34CopyAbs(number1, &num1);
  real34CopyAbs(number2, &num2);
  real34Compare(&num1, &num2, &num2);
  return real34ToInt32(&num2) > 0;
}



bool real34CompareAbsLessThan(const real34_t *number1, const real34_t *number2) {
  real34_t num1, num2;

  real34CopyAbs(number1, &num1);
  real34CopyAbs(number2, &num2);
  real34Compare(&num1, &num2, &num2);
  return real34ToInt32(&num2) < 0;
}



bool real34CompareEqual(const real34_t *number1, const real34_t *number2) {
  real34_t compare;

  real34Compare(number1, number2, &compare);
  return real34ToInt32(&compare) == 0;
}



bool real34CompareGreaterEqual(const real34_t *number1, const real34_t *number2) {
  real34_t compare;

  real34Compare(number1, number2, &compare);
  return real34ToInt32(&compare) >= 0;
}



/* never used
bool real34CompareGreaterThan(const real34_t *number1, const real34_t *number2) {
  real34_t compare;

  real34Compare(number1, number2, &compare);
  return real34ToInt32(&compare) > 0;
}
*/



bool real34CompareLessEqual(const real34_t *number1, const real34_t *number2) {
  real34_t compare;

  real34Compare(number1, number2, &compare);
  return real34ToInt32(&compare) <= 0;
}



bool real34CompareLessThan(const real34_t *number1, const real34_t *number2) {
  real34_t compare;

  real34Compare(number1, number2, &compare);
  return real34ToInt32(&compare) < 0;
}



bool realCompareAbsGreaterThan(const real_t *number1, const real_t *number2) {
  real_t num1, num2;
  int32_t cmp;

  realCopyAbs(number1, &num1);
  realCopyAbs(number2, &num2);
  realCompare(&num1, &num2, &num2, &ctxtReal75);
  cmp = realToInt32(&num2);
  return cmp > 0;
}


/*
bool realCompareAbsGreaterEqual(const real_t *number1, const real_t *number2) {
  real_t num1, num2;
  int32_t cmp;

  realCopyAbs(number1, &num1);
  realCopyAbs(number2, &num2);
  realCompare(&num1, &num2, &num2, &ctxtReal75);
  cmp = realToInt32(&num2);
  return cmp >= 0;
}
*/


bool realCompareAbsLessThan(const real_t *number1, const real_t *number2) {
  real_t num1, num2;
  int32_t cmp;

  realCopyAbs(number1, &num1);
  realCopyAbs(number2, &num2);
  realCompare(&num1, &num2, &num2, &ctxtReal75);
  cmp = realToInt32(&num2);
  return cmp < 0;
}



bool realCompareEqual(const real_t *number1, const real_t *number2) {
  real_t compare;
  int32_t cmp;

  realCompare(number1, number2, &compare, &ctxtReal75);
  cmp = realToInt32(&compare);
  return cmp == 0;
}



bool realCompareGreaterEqual(const real_t *number1, const real_t *number2) {
  real_t compare;
  int32_t cmp;

  realCompare(number1, number2, &compare, &ctxtReal75);
  cmp = realToInt32(&compare);
  return cmp >= 0;
}



bool realCompareGreaterThan(const real_t *number1, const real_t *number2) {
  real_t compare;
  int32_t cmp;

  realCompare(number1, number2, &compare, &ctxtReal75);
  cmp = realToInt32(&compare);
  return cmp > 0;
}



bool realCompareLessEqual(const real_t *number1, const real_t *number2) {
  real_t compare;
  int32_t cmp;

  realCompare(number1, number2, &compare, &ctxtReal75);
  cmp = realToInt32(&compare);
  return cmp <= 0;
}



bool realCompareLessThan(const real_t *number1, const real_t *number2) {
  real_t compare;
  int32_t cmp;

  realCompare(number1, number2, &compare, &ctxtReal75);
  cmp = realToInt32(&compare);
  return cmp < 0;
}



bool real34IsAnInteger(const real34_t *x) {
  real34_t y;

  if(real34IsNaN(x) || real34IsInfinite(x)) {
    return false;
  }

  real34ToIntegralValue(x, &y, DEC_ROUND_DOWN);
  real34Subtract(x, &y, &y);

  return real34CompareEqual(&y, const34_0);
}



bool realIsAnInteger(const real_t *x) {
  real_t y;

  if(realIsNaN(x)) {
    return false;
  }

  if(realIsInfinite(x)) {
    return true;
  }

  realToIntegralValue(x, &y, DEC_ROUND_DOWN, &ctxtReal75);
  realSubtract(x, &y, &y, &ctxtReal75);

  return realCompareEqual(&y, const_0);
}



/*
int16_t realIdenticalDigits(real_t *a, real_t *b) {
  int16_t counter, smallest;

  if(realGetExponent(a) != realGetExponent(b)) {
    return 0;
  }

  realGetCoefficient(a, tmpString);
  realGetCoefficient(b, tmpString + TMP_STR_LENGTH/2);
  smallest = min(a->digits, b->digits);
  counter = 0;

  while(counter < smallest && tmpString[counter] == tmpString[TMP_STR_LENGTH/2 + counter]) {
    counter++;
  }

  return counter;
}
*/
