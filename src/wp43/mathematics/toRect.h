// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/toRect.h
 */
#if !defined(TORECT_H)
  #define TORECT_H

  #include "realType.h"
  #include <stdint.h>

  void fnToRect                (uint16_t unusedButMandatoryParameter);
  void realPolarToRectangular  (const real_t *magnitude, const real_t *theta, real_t *real, real_t *imag, realContext_t *realContext);

#endif // !TORECT_H
