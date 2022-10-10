// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

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
