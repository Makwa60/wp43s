// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/arcsin.h
 */
#if !defined(ARCSIN_H)
  #define ARCSIN_H

  #include "defines.h"
  #include "realType.h"
  #include <stdint.h>

  void fnArcsin   (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void arcsinError(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR == 1)
    #define arcsinError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void arcsinLonI (void);
  void arcsinRema (void);
  void arcsinCxma (void);
  void arcsinReal (void);
  void arcsinCplx (void);

  uint8_t ArcsinComplex(const real_t *xReal, const real_t *xImag, real_t *rReal, real_t *rImag, realContext_t *realContext);

#endif // !ARCSIN_H
