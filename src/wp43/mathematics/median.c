// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

// Imported from C47

/**
 * \file median.c
 */

#include "mathematics/mean.h"

#include "constantPointers.h"
#include "core/memory.h"
#include "error.h"
#include "flags.h"
#include "mathematics/comparisonReals.h"
#include "matrix.h"
#include "registers.h"
#include "registerValueConversions.h"
#include "stack.h"
#include "stats.h"

#include "wp43.h"


static int medianCompare(const void *v1, const void *v2)
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

static void computeMedianSorted(real_t *vals, uint16_t numVals, real_t *median) {
  if (numVals & 1) {  // Odd number of values
    realCopy(vals + numVals / 2, median);
  } else {            // Even number of vlaues
    realAdd(vals + numVals / 2 - 1, vals + numVals / 2, median, &ctxtReal39);
    realMultiply(median, const_1on2, median, &ctxtReal39);
  }
}

static void computeQ1Sorted(real_t *vals, uint16_t numVals, real_t *quartile) {
  computeMedianSorted(vals, numVals / 2, quartile);
}

static void computeQ3Sorted(real_t *vals, uint16_t numVals, real_t *quartile) {
  computeMedianSorted(vals + numVals / 2 + (numVals & 1), numVals / 2, quartile);
}

static void computeMedianUnsorted(real_t *vals, uint16_t numVals, real_t *median) {
  qsort(vals, numVals, sizeof(*vals), &medianCompare);
  computeMedianSorted(vals, numVals, median);
}

static void computeQ1Unsorted(real_t *vals, uint16_t numVals, real_t *quartile) {
  qsort(vals, numVals, sizeof(*vals), &medianCompare);
  computeQ1Sorted(vals, numVals, quartile);
}

static void computeQ3Unsorted(real_t *vals, uint16_t numVals, real_t *quartile) {
  qsort(vals, numVals, sizeof(*vals), &medianCompare);
  computeQ3Sorted(vals, numVals, quartile);
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

static void doStatsOperation(void (*func)(real_t *data, uint16_t n, real_t *res),
                             const real_t *minDataPoints/*, int message*/) {
  uint16_t n;
  real_t *data, x;

  if (checkMinimumDataPoints(minDataPoints)) {
    data = getXvalues(&n);
    if (data != NULL) {
      liftStack();
      setSystemFlag(FLAG_ASLIFT);
      liftStack();

      (*func)(data, n, &x);
      convertRealToReal34ResultRegister(&x, REGISTER_X);

      getYvalues(data);
      (*func)(data, n, &x);
      convertRealToReal34ResultRegister(&x, REGISTER_Y);

      freeWp43(data, n * REAL_SIZE_IN_BYTES);
      //temporaryInformation = message;
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
  doStatsOperation(&computeMedianUnsorted, const_1/*, TI_MEDIANX_MEDIANY*/);
}

/**********************************************
 * \brief Q1 ==> regX, regY
 * enables stack lift and refreshes the stack.
 * regX = Lower quartile x, regY = Lower quartile y
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnLowerQuartileXY(uint16_t unusedButMandatoryParameter) {
  doStatsOperation(&computeQ1Unsorted, const_3/*, TI_Q1X_Q1Y*/);
}

/**********************************************
 * \brief Q3 ==> regX, regY
 * enables stack lift and refreshes the stack.
 * regX = Upper quartile x, regY = Upper quartile y
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnUpperQuartileXY(uint16_t unusedButMandatoryParameter) {
  doStatsOperation(&computeQ3Unsorted, const_3/*, TI_Q3X_Q3Y*/);
}

// Sort the data and compute the median absolute deviation
static void computeMAD(real_t *data, uint16_t n, real_t *mad) {
  uint16_t i;

  computeMedianUnsorted(data, n, mad);
  for (i = 0; i < n; i++) {
    realSubtract(data + i, mad, data + i, &ctxtReal39);
    if (realIsNegative(data + i))
      realChangeSign(data + i);
  }
  computeMedianUnsorted(data, n, mad);
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
  doStatsOperation(&computeMAD, const_1/*, TI_MADX_MADY*/);
}

// Sort the data and compute the inter-quartile range
static void computeIQR(real_t *data, uint16_t n, real_t *iqr) {
    real_t t;

    qsort(data, n, sizeof(*data), &medianCompare);
    computeQ1Unsorted(data, n, &t);
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
  doStatsOperation(&computeIQR, const_3/*, TI_IQRX_IQRY*/);
}

