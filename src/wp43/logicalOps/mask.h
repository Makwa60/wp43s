// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file logicalOps/mask.h
 */
#if !defined(MASK_H)
  #define MASK_H

  #include <stdint.h>

  /**
   * MASKL n ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnMaskl(uint16_t numberOfBits);

  /**
   * MASKR n ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnMaskr(uint16_t numberOfBits);

#endif // !MASK_H
