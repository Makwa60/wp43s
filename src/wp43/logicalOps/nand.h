// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file logicalOps/nand.h
 */
#if !defined(NAND_H)
  #define NAND_H

  #include "defines.h"
  #include <stdint.h>

  /**
   * regX ==> regL NAND regY ÷ regX ==> regX
   * Drops Y, enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnLogicalNand(uint16_t unusedButMandatoryParameter);

  void nandLonILonI (void);
  void nandLonIReal (void);
  void nandRealLonI (void);
  void nandRealReal (void);
  void nandShoIShoI (void);

#endif // !NAND_H
