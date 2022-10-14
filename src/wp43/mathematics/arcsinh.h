// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/arcsinh.h
 */
#if !defined(ARCSINH_H)
  #define ARCSINH_H

  #include "realType.h"
  #include <stdint.h>

  /**
   * regX ==> regL and arcsinh(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnArcsinh   (uint16_t unusedButMandatoryParameter);

  uint8_t ArcsinhReal(const real_t *x, real_t *res, realContext_t *realContext);
  uint8_t ArcsinhComplex(const real_t *xReal, const real_t *xImag, real_t *rReal, real_t *rImag, realContext_t *realContext);

#endif // !ARCSINH_H
