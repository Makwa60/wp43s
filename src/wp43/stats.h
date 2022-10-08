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
 * \file stats.h
 * Statistical functions.
 */
#if !defined(STATS_H)
  #define STATS_H

  #include "realType.h"
  #include <stdbool.h>
  #include <stdint.h>

  bool    sigmaPlus             (bool updateSaved, real_t *x, real_t *y);

  /**
   * Adds a value to the statistic registers.
   *
   * \param[in] unusedButMandatoryParameter
   */
  void    fnSigma               (uint16_t plusMinus);

  real_t *statSum               (uint16_t sum);

  void    fnStatSum             (uint16_t sum);

  /**
   * SUM ==> regX, regY.
   * regX = SUM x, regY = SUM y
   *
   * \param[in] unusedButMandatoryParameter
   */
  void    fnSumXY               (uint16_t unusedButMandatoryParameter);

  /**
   * Xmin ==> regX, regY.
   * regX = min x, regY = min y
   *
   * \param[in] unusedButMandatoryParameter
   */
  void    fnXmin                (uint16_t unusedButMandatoryParameter);

  /**
   * Xmax ==> regX, regY.
   * regX = max x, regY = max y
   *
   * \param[in] unusedButMandatoryParameter
   */
  void    fnXmax                (uint16_t unusedButMandatoryParameter);

  void    fnClSigma             (uint16_t unusedButMandatoryParameter);

  /**
   * Verifies that the statistical registers are allocated and that there are enough data.
   * An appropriate error message is displayed if either condition fails.
   *
   * \param[in] unusedButMandatoryParameter
   * \return bool
   */
  bool    isStatsMatrix(uint16_t *rows, char *mx);

  bool    checkMinimumDataPoints(const real_t *n);
  void    initStatisticalSums   (void);
  void    calcSigma             (uint16_t maxOffset);

  void    fnSetLoBin            (uint16_t unusedButMandatoryParameter);
  void    fnSetHiBin            (uint16_t unusedButMandatoryParameter);
  void    fnSetNBins            (uint16_t unusedButMandatoryParameter);
  void    fnConvertStatsToHisto (uint16_t statsVariableToHistogram);

#endif // STATS_H
