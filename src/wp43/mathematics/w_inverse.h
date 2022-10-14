// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/w_inverse.h
 */
#if !defined(W_INVERSE_H)
  #define W_INVERSE_H

  #include <stdint.h>

  /**
   * regX ==> regL and W^(-1)(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnWinverse(uint16_t unusedButMandatoryParameter);

#endif // !W_INVERSE_H
