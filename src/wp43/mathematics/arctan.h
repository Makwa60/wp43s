// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/arctan.h
 */
#if !defined(ARCTAN_H)
  #define ARCTAN_H

  #include "defines.h"
  #include "realType.h"
  #include <stdint.h>

  void fnArctan   (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void arctanError(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR == 1)
    #define arctanError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void arctanLonI (void);
  void arctanRema (void);
  void arctanCxma (void);
  void arctanReal (void);
  void arctanCplx (void);

  uint8_t ArctanComplex(real_t *xReal, real_t *xImag, real_t *rReal, real_t *rImag, realContext_t *realContext);

#endif // !ARCTAN_H
