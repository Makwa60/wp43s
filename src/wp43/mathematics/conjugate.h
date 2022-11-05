// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/conjugate.h
 */
#if !defined(CONJUGATE_H)
  #define CONJUGATE_H

  #include "defines.h"
  #include <stdint.h>

  /**
   * regX ==> regL and conj(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnConjugate(uint16_t unusedButMandatoryParameter);

  void conjCxma   (void);
  void conjCplx   (void);

#endif // !CONJUGATE_H
