// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/arccosh.h
 */
#if !defined(ARCCOSH_H)
  #define ARCCOSH_H

  #include "realType.h"
  #include <stdint.h>

  /**
   * regX ==> regL and arccosh(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnArccosh (uint16_t unusedButMandatoryParameter);

  void realArcosh(const real_t *x, real_t *res, realContext_t *realContext);
  void real34Arcosh(const real34_t *x, real34_t *res);

#endif // !ARCCOSH_H
