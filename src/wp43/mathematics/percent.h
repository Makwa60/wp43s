// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/percent.h
 */
#if !defined(PERCENT_H)
  #define PERCENT_H

  #include <stdint.h>

  void fnPercent(uint16_t unusedButMandatoryParameter);

  void percentLonILonI(void);
  void percentLonIReal(void);
  void percentRealLonI(void);
  void percentRealReal(void);

#endif // !PERCENT_H
