// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file logicalOps/xnor.h
 */
#if !defined(XNOR_H)
  #define XNOR_H

  #include "defines.h"
  #include <stdint.h>

  /**
   * regX ==> regL XNOR regY ÷ regX ==> regX
   * Drops Y, enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnLogicalXnor(uint16_t unusedButMandatoryParameter);

  void xnorLonILonI (void);
  void xnorLonIReal (void);
  void xnorRealLonI (void);
  void xnorRealReal (void);
  void xnorShoIShoI (void);

#endif // !XNOR_H
