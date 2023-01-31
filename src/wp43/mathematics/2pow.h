// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/2pow.h
 */
#if !defined(FILE_2POW_H)
  #define FILE_2POW_H

  #include "defines.h"
  #include "realType.h"
  #include <stdint.h>

  /**
   * regX ==> regL and 2^regX ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fn2Pow     (uint16_t unusedButMandatoryParameter);

  void realPower2 (const real_t *x, real_t *res, realContext_t *realContext);
  void real34Power2(const real34_t *x, real34_t *res);

#endif // !FILE_2POW_H
