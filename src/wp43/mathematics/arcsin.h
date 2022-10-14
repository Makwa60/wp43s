// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/arcsin.h
 */
#if !defined(ARCSIN_H)
  #define ARCSIN_H

  #include "realType.h"
  #include <stdint.h>

  /**
   * regX ==> regL and arcsin(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnArcsin   (uint16_t unusedButMandatoryParameter);

  uint8_t ArcsinComplex(const real_t *xReal, const real_t *xImag, real_t *rReal, real_t *rImag, realContext_t *realContext);

#endif // !ARCSIN_H
