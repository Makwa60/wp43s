// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/tanh.h
 */
#if !defined(TANH_H)
  #define TANH_H

  #include "realType.h"
  #include <stdint.h>

  /**
   * regX ==> regL and tanh(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnTanh        (uint16_t unusedButMandatoryParameter);

  uint8_t TanhComplex(const real_t *xReal, const real_t *xImag, real_t *rReal, real_t *rImag, realContext_t *realContext);

#endif // !TANH_H
