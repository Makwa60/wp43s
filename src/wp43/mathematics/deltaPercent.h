// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/percentT.h
 */
#if !defined(DELTAPERCENT_H)
  #define DELTAPERCENT_H

  #include <stdint.h>

  /**
   * regX ==> regL and DeltaPercent(regX, RegY) ==> regX
   * enables stack lift and refreshes the stack.
   * Calculate x*y/100
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnDeltaPercent(uint16_t unusedButMandatoryParameter);

  void deltaPercentLonILonI(void);
  void deltaPercentLonIReal(void);
  void deltaPercentRealLonI(void);
  void deltaPercentRealReal(void);

#endif // !DELTAPERCENT_H
