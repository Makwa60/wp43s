// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file logicalOps/and.h
 */
#if !defined(AND_H)
  #define AND_H

  #include "defines.h"
  #include <stdint.h>

  /**
   * regX ==> regL AND regY ÷ regX ==> regX
   * Drops Y, enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnLogicalAnd(uint16_t unusedButMandatoryParameter);

  void andLonILonI (void);
  void andLonIReal (void);
  void andRealLonI (void);
  void andRealReal (void);
  void andShoIShoI (void);

#endif // !AND_H
