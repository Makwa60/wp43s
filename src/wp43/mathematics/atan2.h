// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/atan2.h
 */
#if !defined(ATAN2_H)
  #define ATAN2_H

  #include <stdint.h>

  /**
   * regX ==> regL and atan2(regY, regX) ==> regX
   * Drops Y, enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnAtan2      (uint16_t unusedButMandatoryParameter);

#endif // !ATAN2_H
