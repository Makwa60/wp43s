// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/sinc.h
 */
#if !defined(SINC_H)
  #define SINC_H
  // Coded by JM, based on sin.h

  #include "defines.h"
  #include "realType.h"
  #include <stdint.h>

  void fnSinc  (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void sincError(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR != 1)
    #define sincError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void sincLonI (void);
  void sincRema (void);
  void sincCxma (void);
  void sincReal (void);
  void sincCplx (void);
  void sincComplex(const real_t *real, const real_t *imag, real_t *resReal, real_t *resImag, realContext_t *realContext);

#endif // !SINC_H
