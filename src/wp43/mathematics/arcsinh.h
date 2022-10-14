// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/arcsinh.h
 */
#if !defined(ARCSINH_H)
  #define ARCSINH_H

  #include "defines.h"
  #include "realType.h"
  #include <stdint.h>

  void fnArcsinh   (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void arcsinhError(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR == 1)
    #define arcsinhError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void arcsinhLonI (void);
  void arcsinhRema (void);
  void arcsinhCxma (void);
  void arcsinhReal (void);
  void arcsinhCplx (void);

  uint8_t ArcsinhReal(const real_t *x, real_t *res, realContext_t *realContext);
  uint8_t ArcsinhComplex(const real_t *xReal, const real_t *xImag, real_t *rReal, real_t *rImag, realContext_t *realContext);

#endif // !ARCSINH_H
