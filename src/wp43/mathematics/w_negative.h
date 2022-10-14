// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/w_negative.h
 */
#if !defined(W_NEGATIVE_H)
  #define W_NEGATIVE_H

  #include <stdint.h>

  /**
   * regX ==> regL and W(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnWnegative(uint16_t unusedButMandatoryParameter);

#endif // !W_NEGATIVE_H
