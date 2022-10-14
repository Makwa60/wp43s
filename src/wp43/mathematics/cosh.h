// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/cosh.h
 */
#if !defined(COSH_H)
  #define COSH_H

  #include "defines.h"
  #include <stdint.h>

  void fnCosh   (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void coshError(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR == 1)
    #define coshError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void coshLonI (void);
  void coshRema (void);
  void coshCxma (void);
  void coshReal (void);
  void coshCplx (void);

#endif // !COSH_H
