// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/zeta.h
 */
#if !defined(ZETA_H)
  #define ZETA_H

  #include <stdint.h>

  /**
   * regX ==> regL and zeta(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnZeta(uint16_t unusedButMandatoryParameter);

#endif // !ZETA_H
