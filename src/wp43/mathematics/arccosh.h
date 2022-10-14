// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/arccosh.h
 */
#if !defined(ARCCOSH_H)
  #define ARCCOSH_H

  #include "defines.h"
  #include "realType.h"
  #include <stdint.h>

  void fnArccosh   (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void arccoshError(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR == 1)
    #define arccoshError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void arccoshLonI (void);
  void arccoshRema (void);
  void arccoshCxma (void);
  void arccoshReal (void);
  void arccoshCplx (void);
  void realArcosh  (const real_t *x, real_t *res, realContext_t *realContext);

#endif // !ARCCOSH_H
