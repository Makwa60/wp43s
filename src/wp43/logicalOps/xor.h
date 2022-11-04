// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file logicalOps/xor.h
 */
#if !defined(XOR_H)
  #define XOR_H

  #include "defines.h"
  #include <stdint.h>

  /**
   * regX ==> regL XOR regY ÷ regX ==> regX
   * Drops Y, enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnLogicalXor(uint16_t unusedButMandatoryParameter);

  void xorLonILonI (void);
  void xorLonIReal (void);
  void xorRealLonI (void);
  void xorRealReal (void);
  void xorShoIShoI (void);

#endif // !XOR_H
