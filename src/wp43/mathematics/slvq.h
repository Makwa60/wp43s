// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/slvq.h
 */
#if !defined(SLVQ_H)
  #define SLVQ_H

  #include "realType.h"
  #include <stdint.h>

  void fnSlvq                (uint16_t unusedButMandatoryParameter);

  void solveQuadraticEquation(const real_t *aReal, const real_t *aImag,
                              const real_t *bReal, const real_t *bImag,
                              const real_t *cReal, const real_t *cImag,
                                    real_t *rReal,       real_t *rImag,
                                    real_t *x1Real,      real_t *x1Imag,
                                    real_t *x2Real,      real_t *x2Imag, realContext_t *realContext);

#endif // !SLVQ_H
