// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

// Imported from C47

/**
 * \file median.c
 */

#include "mathematics/mean.h"

#include "constantPointers.h"
#include "core/memory.h"
#include "debug.h"
#include "error.h"
#include "flags.h"
#include "items.h"
#include "mathematics/comparisonReals.h"
#include "matrix.h"
#include "registers.h"
#include "registerValueConversions.h"
#include "stack.h"
#include "stats.h"

#include "wp43.h"


/*
 * Linear interpolation between a and b by an amount p.
 * p = 0 returns a, p = 1 returns b.
 *
 * There are two basic formulae for this but both have issues.
 *     lerp = a + p * (b - a) has cancellation issues but is monotonic
 *     lerp = (1 - p) * a + p * b is only monotonic if a * b < 0
 * we work in high precision and special case both
 */
static void lerp(const real_t *a, const real_t *b, const real_t *p, real_t *res) {
  real_t x;

  if (realIsNaN(a) || realIsNaN(b) || realIsNaN(p) || realIsInfinite(p)) {
    realCopy(const_NaN, res);
  } else if (realIsInfinite(a)) {
    if (realIsInfinite(b)) {    // both infinite, either NaN or one of them
      if (realIsNegative(a) == realIsNegative(b))
        realCopy(a, res);
      else
        realCopy(const_NaN, res);
    } else
      realCopy(a, res);         // a only is infinite, return a
  } else if (realIsInfinite(b)) {
    realCopy(b, res);           // b only is infinite, return b
  } else if (realCompareEqual(a, b)) {
    realCopy(a, res);           // both same, return one
  } else if (realIsNegative(a) != realIsNegative(b)) {
    /* a * b < 0, use lerp = (1 - p) * a + p * b */
    realCopy(p, &x);
    realChangeSign(&x);
    realFMA(&x, a, a, &x, &ctxtReal75);   // a - p * a
    realFMA(p, b, &x, res, &ctxtReal75);  // p * b + (a - p * a)
  } else {
    /* a * b > 0, use lerp = a + p * (b - a) */
    realSubtract(b, a, &x, &ctxtReal75);
    realFMA(&x, p, a, res, &ctxtReal75);
  }
}

static int statsRealCompare(const void *v1, const void *v2)
{
  const real_t *r1 = (const real_t *)v1;
  const real_t *r2 = (const real_t *)v2;
  real_t compare;

  realCompare(r1, r2, &compare, &ctxtReal75);

  if (realIsZero(&compare))
    return 0;
  if (realIsNegative(&compare))
    return -1;
  return 1;
}

/*
 * Compute an arbitrary percentile.
 * Note that per is [0, 1] rather than a percentage.
 */
static void computePercentileSorted(real_t *data, uint16_t n, const real_t *per, real_t *percentile) {
  real_t c, p, t;
  int posn;

  uInt32ToReal(n - 1, &c);
  realMultiply(&c, per, &t, &ctxtReal39);
  realToIntegralValue(&t, &p, DEC_ROUND_DOWN, &ctxtReal39);
  posn = realToInt32(&p);

  if (posn >= n - 1)
    realCopy(data + n - 1, percentile);
  else if (posn < 0)
    realCopy(data, percentile);
  else {
    realSubtract(&t, &p, &c, &ctxtReal39);  // FP(position)
    lerp(data + posn, data + posn + 1, &c, percentile);
  }
}

static void computePercentileUnsorted(real_t *data, uint16_t n, const real_t *x, real_t *percentile) {
  qsort(data, n, sizeof(*data), &statsRealCompare);
  computePercentileSorted(data, n, x, percentile);
}

static void computeMedianSorted(real_t *data, uint16_t n, real_t *median) {
  // Compute directly rather than using the percentile funtion to avoid rounding
  if (n & 1) {  // Odd number of values
    realCopy(data + n / 2, median);
  } else {      // Even number of values
    realAdd(data + n / 2 - 1, data + n / 2, median, &ctxtReal39);
    realMultiply(median, const_1on2, median, &ctxtReal39);
  }
}

static void computeQ1Sorted(real_t *data, uint16_t n, real_t *quartile) {
#if USE_PERCENTILE_FOR_MEDIAN
  computePercentileSorted(data, n, const_1on4, median);
#else
  computeMedianSorted(data, n / 2, quartile);
#endif
}

static void computeQ3Sorted(real_t *data, uint16_t n, real_t *quartile) {
#if USE_PERCENTILE_FOR_MEDIAN
  real_t p;

  realMultiply(const_1on4, const_3, &p, &ctxtReal39);
  computePercentileSorted(data, n, &p, median);
#else
  computeMedianSorted(data + n / 2 + (n & 1), n / 2, quartile);
#endif
}

static void computeMedianUnsorted(real_t *data, uint16_t n, const real_t *unusedButMandatoryParameter, real_t *median) {
  qsort(data, n, sizeof(*data), &statsRealCompare);
  computeMedianSorted(data, n, median);
}

static void computeQ1Unsorted(real_t *data, uint16_t n, const real_t *unusedButMandatoryParameter, real_t *quartile) {
  qsort(data, n, sizeof(*data), &statsRealCompare);
  computeQ1Sorted(data, n, quartile);
}

static real_t *getXvalues(uint16_t *n) {
  real34Matrix_t stats;
  uint16_t rows, cols, i;
  calcRegister_t regStats;
  real_t *data;

  strcpy(statMx, "STATS");
  regStats = findNamedVariable(statMx);
  if (!isStatsMatrix(&rows, statMx)) {
    displayCalcErrorMessage(ERROR_NO_SUMMATION_DATA, ERR_REGISTER_LINE, REGISTER_X);
    return NULL;
  }
  data = allocWp43(rows * REAL_SIZE_IN_BYTES);
  if (data == NULL) {
    displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    return NULL;
  }
  linkToRealMatrixRegister(regStats, &stats);
  cols = stats.header.matrixColumns;
  for (i = 0; i < rows; i++)
    real34ToReal(stats.matrixElements + i * cols, data + i);
  *n = rows;
  return data;
}

static void getYvalues(real_t *data) {
  real34Matrix_t stats;
  uint16_t rows, cols, i;
  calcRegister_t regStats;

  strcpy(statMx, "STATS");
  regStats = findNamedVariable(statMx);
  linkToRealMatrixRegister(regStats, &stats);
  rows = stats.header.matrixRows;
  cols = stats.header.matrixColumns;
  for (i = 0; i < rows; i++)
    real34ToReal(stats.matrixElements + i * cols + 1, data + i);
}

static void doStatsOperation(void (*func)(real_t *data, uint16_t n, const real_t *arg, real_t *res),
                             const real_t *minDataPoints, const real_t *arg/*, int message*/) {
  uint16_t n;
  real_t *data, x;

  if (checkMinimumDataPoints(minDataPoints)) {
    data = getXvalues(&n);
    if (data != NULL) {
      liftStack();
      setSystemFlag(FLAG_ASLIFT);
      liftStack();

      (*func)(data, n, arg, &x);
      convertRealToReal34ResultRegister(&x, REGISTER_X);

      getYvalues(data);
      (*func)(data, n, arg, &x);
      convertRealToReal34ResultRegister(&x, REGISTER_Y);

      freeWp43(data, n * REAL_SIZE_IN_BYTES);
      //temporaryInformation = message;
      adjustResult(REGISTER_X, false, false, -1, -1, -1);
      adjustResult(REGISTER_Y, false, false, -1, -1, -1);
    }
  }
}

/**********************************************
 * \brief median ==> regX, regY
 * enables stack lift and refreshes the stack.
 * regX = MEDIAN x, regY = MEDIAN y
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnMedianXY(uint16_t unusedButMandatoryParameter) {
  doStatsOperation(&computeMedianUnsorted, const_1, NULL/*, TI_MEDIANX_MEDIANY*/);
}

// Sort the data and compute the median absolute deviation
static void computeMAD(real_t *data, uint16_t n, const real_t *unusedButMandatoryParameter, real_t *mad) {
  uint16_t i;

  computeMedianUnsorted(data, n, NULL, mad);
  for (i = 0; i < n; i++) {
    realSubtract(data + i, mad, data + i, &ctxtReal39);
    if (realIsNegative(data + i))
      realChangeSign(data + i);
  }
  computeMedianUnsorted(data, n, NULL, mad);
}

/**********************************************
 * \brief median absolute deviation ==> regX, regY
 * enables stack lift and refreshes the stack.
 * regX = MAD x, regY = MAD y
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnMADXY              (uint16_t unusedButMandatoryParameter) {
  doStatsOperation(&computeMAD, const_1, NULL/*, TI_MADX_MADY*/);
}

// Sort the data and compute the inter-quartile range
static void computeIQR(real_t *data, uint16_t n, const real_t *unusedButMandatoryParameter, real_t *iqr) {
    real_t t;

    computeQ1Unsorted(data, n, NULL, &t);
    computeQ3Sorted(data, n, iqr);
    realSubtract(iqr, &t, iqr, &ctxtReal39);
}

/**********************************************
 * \brief interquartile range ==> regX, regY
 * enables stack lift and refreshes the stack.
 * regX = IQR x, regY = IQR y
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnIQRXY              (uint16_t unusedButMandatoryParameter) {
  doStatsOperation(&computeIQR, const_3, NULL/*, TI_IQRX_IQRY*/);
}

/**********************************************
 * \brief percentile regX ==> regX, regY
 * enables stack lift and refreshes the stack.
 * regX = xth percentile x, regY = xth percentile y
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnPercentileXY(uint16_t unusedButMandatoryParameter) {
  real_t p;

  switch (getRegisterDataType(REGISTER_X)) {
    case dtLongInteger: {
      convertLongIntegerRegisterToReal(REGISTER_X, &p, &ctxtReal75);
      break;
    }

    case dtShortInteger: {
      convertShortIntegerRegisterToReal(REGISTER_X, &p, &ctxtReal39);
      break;
    }

    case dtReal34: {
      real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &p);
      break;
    }

    default: {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        sprintf(errorMessage, "cannot x%%ile with %s in X", getRegisterDataTypeName(REGISTER_X, true, false));
        moreInfoOnError("In function fnPercentileXY:", errorMessage, NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
      return;
    }
  }

  // Range saturate if out of scope and scale away percentage
  if (realIsNegative(&p))
    realZero(&p);
  else if (realCompareLessThan(&p, const_100))
    realDivide(&p, const_100, &p, &ctxtReal39);
  else if (!realIsNaN(&p))
    realCopy(const_1, &p);
  fnDrop(NOPARAM);
  doStatsOperation(&computePercentileUnsorted, const_1, &p/*, TI_MEDIANX_MEDIANY*/);
}

