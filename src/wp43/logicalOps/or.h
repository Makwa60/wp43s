// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file logicalOps/or.h
 */
#if !defined(OR_H)
  #define OR_H

  #include "defines.h"
  #include <stdint.h>

  /**
   * regX ==> regL OR regY ÷ regX ==> regX
   * Drops Y, enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnLogicalOr(uint16_t unusedButMandatoryParameter);

  void orLonILonI (void);
  void orLonIReal (void);
  void orRealLonI (void);
  void orRealReal (void);
  void orShoIShoI (void);

#endif // !OR_H
