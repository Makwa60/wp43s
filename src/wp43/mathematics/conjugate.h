// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/conjugate.h
 */
#if !defined(CONJUGATE_H)
  #define CONJUGATE_H

  #include "defines.h"
  #include <stdint.h>

  void fnConjugate(uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void conjError  (void);
  #else // (EXTRA_INFO_ON_CALC_ERROR == 1)
    #define conjError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void conjCxma   (void);
  void conjCplx   (void);

#endif // !CONJUGATE_H
