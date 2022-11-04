// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file logicalOps/nor.h
 */
#if !defined(NOR_H)
  #define NOR_H

  #include "defines.h"
  #include <stdint.h>

  /**
   * regX ==> regL NOR regY ÷ regX ==> regX
   * Drops Y, enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnLogicalNor(uint16_t unusedButMandatoryParameter);

  void norLonILonI (void);
  void norLonIReal (void);
  void norRealLonI (void);
  void norRealReal (void);
  void norShoIShoI (void);

#endif // !NOR_H
