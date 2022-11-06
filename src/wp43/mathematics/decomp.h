// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/decomp.h
 */
#if !defined(DECOMP_H)
  #define DECOMP_H

  #include "defines.h"
  #include <stdint.h>

  /**
   * regX ==> regL and DECOMP(regX) ==> regX, regY
   * enables stack lift and refreshes the stack.
   * Decomposes x (after converting it to an improper fraction, if applicable), returning a stack with
   * [denominator(x), numerator(x)]
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnDecomp(uint16_t unusedButMandatoryParameter);

  void decompLonI(void);
  void decompReal(void);

#endif // !DECOMP_H
