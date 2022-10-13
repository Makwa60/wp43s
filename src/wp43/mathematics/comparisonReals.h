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
  int16_t realIdenticalDigits         (real_t *a, real_t *b);
#endif // !COMPARISONREALS_H
