// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/arctanh.h
 */
#if !defined(ARCTANH_H)
  #define ARCTANH_H

  #include <stdint.h>

  /**
   * regX ==> regL and arctanh(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnArctanh   (uint16_t unusedButMandatoryParameter);

#endif // !ARCTANH_H
