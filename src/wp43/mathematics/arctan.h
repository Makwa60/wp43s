// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/arctan.h
 */
#if !defined(ARCTAN_H)
  #define ARCTAN_H

  #include "realType.h"
  #include <stdint.h>

  /**
   * regX ==> regL and arctan(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnArctan   (uint16_t unusedButMandatoryParameter);

  uint8_t ArctanComplex(real_t *xReal, real_t *xImag, real_t *rReal, real_t *rImag, realContext_t *realContext);

#endif // !ARCTAN_H
