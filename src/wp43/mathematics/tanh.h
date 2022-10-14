// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/tanh.h
 */
#if !defined(TANH_H)
  #define TANH_H

  #include "defines.h"
  #include "realType.h"
  #include <stdint.h>

  void fnTanh   (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void tanhError(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR != 1)
    #define tanhError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void tanhLonI (void);
  void tanhRema (void);
  void tanhCxma (void);
  void tanhReal (void);
  void tanhCplx (void);

  uint8_t TanhComplex(const real_t *xReal, const real_t *xImag, real_t *rReal, real_t *rImag, realContext_t *realContext);

#endif // !TANH_H
