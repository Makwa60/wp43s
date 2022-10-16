// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/unitVector.h
 */
#if !defined(UNITVECTOR_H)
  #define UNITVECTOR_H

  #include <stdint.h>

  /**
   * regX ==> regL and unitVector(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnUnitVector(uint16_t unusedButMandatoryParameter);

  void unitVectorCplx(void);

#endif // !UNITVECTOR_H
