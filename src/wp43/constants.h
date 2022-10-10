// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file constants.h
 * Functions for constant management
 *
 * For constant description see:
 *   https://physics.nist.gov/cuu/index.html
 */
#if !defined(CONSTANTS_H)
  #define CONSTANTS_H

  #include <stdint.h>

  /**
   * Replaces the X content with the selected constant.
   * Enables \b stack \b lift and refreshes the stack
   *
   * \param[in] constant Index of the constant to store in X
   */
  void fnConstant(const uint16_t constant);

  /**
   * Replaces the X content with pi
   * That is, the circumference to diameter ratio,
   * 3,1415926. Enables \b stack \b lift and refreshes the
   * stack.
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnPi      (uint16_t unusedButMandatoryParameter);

#endif // !CONSTANTS_H
