// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file logicalOps/setClearFlipBits.h
 */
#if !defined(SETCLEARFLIPBITS_H)
  #define SETCLEARFLIPBITS_H

  #include <stdint.h>

  /**
   * regX ==> regL and CB(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnCb(uint16_t bit);

  /**
   * regX ==> regL and SB(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnSb(uint16_t bit);

  /**
   * regX ==> regL and FB(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnFb(uint16_t bit);

  /**
   * bit clear in register X
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnBc(uint16_t bit);

  /**
   * bit set in register X
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnBs(uint16_t bit);

#endif // !SETCLEARFLIPBITS_H
