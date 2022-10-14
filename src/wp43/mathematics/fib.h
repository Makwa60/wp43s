// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/fib.h
 */
#if !defined(FIB_H)
  #define FIB_H

  #include "defines.h"
  #include "realType.h"
  #include <stdint.h>

  void fnFib   (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void fibError(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR != 1)
    #define fibError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void fibLonI (void);
  void fibReal (void);
  void fibCplx (void);

  uint8_t FibonacciReal(const real_t *n, real_t *res, realContext_t *realContext);
  uint8_t FibonacciComplex(const real_t *nReal, const real_t *nImag, real_t *resReal, real_t *resImag, realContext_t *realContext);

#endif // !FIB_H
