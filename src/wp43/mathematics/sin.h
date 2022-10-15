// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/sin.h
 */
#if !defined(SIN_H)
  #define SIN_H

  #include "defines.h"
  #include "realType.h"
  #include <stdint.h>

  void fnSin   (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void sinError(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR != 1)
    #define sinError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void sinLonI (void);
  void sinRema (void);
  void sinCxma (void);
  void sinReal (void);
  void sinCplx (void);
  void sinComplex(const real_t *real, const real_t *imag, real_t *resReal, real_t *resImag, realContext_t *realContext);

#endif // !SIN_H
