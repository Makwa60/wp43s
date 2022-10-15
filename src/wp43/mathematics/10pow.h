// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/10pow.h
 */
#if !defined(FILE_10POW_H)
  #define FILE_10POW_H

  #include <stdint.h>

  /**
   * regX ==> regL and 10^regX ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fn10Pow(uint16_t unusedButMandatoryParameter);

#endif // !FILE_10POW_H
