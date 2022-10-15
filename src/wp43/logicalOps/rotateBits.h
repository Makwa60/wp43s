// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file logicalOps/rotateBits.h
 */
#if !defined(ROTATEBITS_H)
  #define ROTATEBITS_H

  #include <stdint.h>

  /**
   * regX ==> regL and ASR(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnAsr   (uint16_t numberOfShifts);

  /**
   * regX ==> regL and SL(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnSl    (uint16_t numberOfShifts);

  /**
   * regX ==> regL and SR(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnSr    (uint16_t numberOfShifts);

  /**
   * regX ==> regL and RL(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnRl    (uint16_t numberOfShifts);

  /**
   * regX ==> regL and RLC(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnRlc   (uint16_t numberOfShifts);

  /**
   * regX ==> regL and RR(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnRr    (uint16_t numberOfShifts);

  /**
   * regX ==> regL and RRC(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnRrc   (uint16_t numberOfShifts);

  /**
   * regX ==> regL and LJ(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnLj    (uint16_t numberOfShifts);

  /**
   * regX ==> regL and RJ(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnRj    (uint16_t numberOfShifts);

  void fnMirror(uint16_t unusedButMandatoryParameter);

#endif // !ROTATEBITS_H
