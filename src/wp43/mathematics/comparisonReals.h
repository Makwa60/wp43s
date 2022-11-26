// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/comparisonReals.h
 */
#if !defined(COMPARISONREALS_H)
  #define COMPARISONREALS_H

  #include "realType.h"
  #include <stdbool.h>
  #include <stdint.h>

  bool    real34CompareAbsGreaterThan (const real34_t *number1, const real34_t *number2);
  bool    real34CompareAbsGreaterEqual(const real34_t *number1, const real34_t *number2);
  bool    real34CompareAbsLessThan    (const real34_t *number1, const real34_t *number2);
  bool    real34CompareEqual          (const real34_t *number1, const real34_t *number2);
  bool    real34CompareGreaterEqual   (const real34_t *number1, const real34_t *number2);
  //bool    real34CompareGreaterThan    (const real34_t *number1, const real34_t *number2); never used
  bool    real34CompareLessEqual      (const real34_t *number1, const real34_t *number2);
  bool    real34CompareLessThan       (const real34_t *number1, const real34_t *number2);

  bool    realCompareAbsGreaterThan   (const real_t *number1, const real_t *number2);
  //bool    realCompareAbsGreaterEqual  (const real_t *number1, const real_t *number2); never used
  bool    realCompareAbsLessThan      (const real_t *number1, const real_t *number2);
  bool    realCompareEqual            (const real_t *number1, const real_t *number2);
  bool    realCompareGreaterEqual     (const real_t *number1, const real_t *number2);
  bool    realCompareGreaterThan      (const real_t *number1, const real_t *number2);
  bool    realCompareLessEqual        (const real_t *number1, const real_t *number2);
  bool    realCompareLessThan         (const real_t *number1, const real_t *number2);

  bool    real34IsAnInteger           (const real34_t *x);
  bool    realIsAnInteger             (const real_t *x);
  //int16_t realIdenticalDigits         (real_t *a, real_t *b);

#endif // !COMPARISONREALS_H
