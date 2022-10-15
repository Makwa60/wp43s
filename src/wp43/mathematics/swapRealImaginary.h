// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/swapRealImaginary.h
 */
#if !defined(SWAPREALIMAGINARY_H)
  #define SWAPREALIMAGINARY_H

  #include "defines.h"
  #include <stdint.h>

  void fnSwapRealImaginary(uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void swapReImError      (void);
  #else // (EXTRA_INFO_ON_CALC_ERROR != 1)
    #define swapReImError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void swapReImCxma       (void);
  void swapReImCplx       (void);

#endif // !SWAPREALIMAGINARY_H
