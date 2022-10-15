// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file logicalOps/countBits.h
 */
#if !defined(COUNTBITS_H)
  #define COUNTBITS_H

  #include <stdint.h>

  /**
   * regX ==> regL and countBits(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnCountBits(uint16_t unusedButMandatoryParameter);

#endif // !COUNTBITS_H
