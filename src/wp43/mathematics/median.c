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

static void computeMedian(real_t *vals, unsigned int numVals, real_t *median) {
  qsort(vals, numVals, sizeof(*vals), &medianCompare);
  if (numVals & 1) {  // Odd number of values
    realCopy(vals + numVals / 2, median);
  } else {            // Even number of vlaues
    realAdd(vals + numVals / 2 - 1, vals + numVals / 2, median, &ctxtReal39);
    realMultiply(median, const_1on2, median, &ctxtReal39);
  }
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

/**********************************************
 * \brief median ==> regX, regY
 * enables stack lift and refreshes the stack.
 * regX = MEDIAN x, regY = MEDIAN y
 *
 * \param[in] unusedButMandatoryParameter uint16_t
 * \return void
 ***********************************************/
void fnMedianXY(uint16_t unusedButMandatoryParameter) {
  uint16_t n;
  real_t *data, median;

  if(checkMinimumDataPoints(const_1)) {
    data = getXvalues(&n);
    if (data != NULL) {
      liftStack();
      setSystemFlag(FLAG_ASLIFT);
      liftStack();

      computeMedian(data, n, &median);
      convertRealToReal34ResultRegister(&median, REGISTER_X);

      getYvalues(data);
      computeMedian(data, n, &median);
      convertRealToReal34ResultRegister(&median, REGISTER_Y);
      freeWp43(data, n * REAL_SIZE_IN_BYTES);
    }
    //temporaryInformation = TI_MEDIANX_MEDIANY;
  }
}

// Sort the data and compute the median absolute deviation
static void calcMAD(real_t *data, uint16_t n, real_t *mad) {
  uint16_t i;

  computeMedian(data, n, mad);
  for (i = 0; i < n; i++) {
    realSubtract(data + i, mad, data + i, &ctxtReal39);
    if (realIsNegative(data + i))
      realChangeSign(data + i);
  }
  computeMedian(data, n, mad);
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
  uint16_t n;
  real_t *data, mad;

  if (checkMinimumDataPoints(const_1)) {
    data = getXvalues(&n);
    if (data != NULL) {
      liftStack();
      setSystemFlag(FLAG_ASLIFT);
      liftStack();

      calcMAD(data, n, &mad);
      convertRealToReal34ResultRegister(&mad, REGISTER_X);

      getYvalues(data);
      calcMAD(data, n, &mad);
      convertRealToReal34ResultRegister(&mad, REGISTER_Y);
      freeWp43(data, n * REAL_SIZE_IN_BYTES);
    }
    //temporaryInformation = TI_MADX_MADY;
  }
}

// Sort the data and compute the inter-quartile range
static void calcIQR(real_t *data, uint16_t n, real_t *iqr) {
    real_t t;
    const uint16_t i = (n - 1) / 4;

    qsort(data, n, sizeof(*data), &medianCompare);
    if (n % 4 == 0) {
      realSubtract(data + (n - i - 1), data + i, iqr, &ctxtReal39);
    } else {
      realAdd(data + i, data + i + 1, iqr, &ctxtReal39);
      realAdd(data + (n - i - 1), data + (n - i - 2), &t, &ctxtReal39);
      realSubtract(&t, iqr, &t, &ctxtReal39);
      realMultiply(&t, const_1on2, iqr, &ctxtReal39);
    }
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
  uint16_t n;
  real_t *data, iqr;

  if(checkMinimumDataPoints(const_3)) {
    data = getXvalues(&n);
    if (data != NULL) {
      liftStack();
      setSystemFlag(FLAG_ASLIFT);
      liftStack();

      calcIQR(data, n, &iqr);
      convertRealToReal34ResultRegister(&iqr, REGISTER_X);

      getYvalues(data);
      calcIQR(data, n, &iqr);
      convertRealToReal34ResultRegister(&iqr, REGISTER_Y);
      freeWp43(data, n * REAL_SIZE_IN_BYTES);
    }
    //temporaryInformation = TI_IQRX_IQRY;
  }
}



