// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/w_positive.h
 */
#if !defined(W_POSITIVE_H)
  #define W_POSITIVE_H

  #include <stdint.h>

  /**
   * regX ==> regL and W(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnWpositive(uint16_t unusedButMandatoryParameter);

#endif // !W_POSITIVE_H
