// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/sqrt1Px2.h
 */
#if !defined(SQRT1PX2_H)
  #define SQRT1PX2_H

  #include "defines.h"
  #include "realType.h"
  #include <stdint.h>

  void fnSqrt1Px2   (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void sqrt1Px2Error(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR != 1)
    #define sqrt1Px2Error typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void sqrt1Px2LonI (void);
  void sqrt1Px2Rema (void);
  void sqrt1Px2Cxma (void);
  void sqrt1Px2ShoI (void);
  void sqrt1Px2Real (void);
  void sqrt1Px2Cplx (void);
  void sqrt1Px2Complex(const real_t *real, const real_t *imag, real_t *resReal, real_t *resImag, realContext_t *realContext);

#endif // !SQRT1PX2_H
