// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/10pow.h
 */
#if !defined(FILE_10POW_H)
  #define FILE_10POW_H

  #include "realType.h"
  #include <stdint.h>

  /**
   * regX ==> regL and 10^regX ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fn10Pow(uint16_t unusedButMandatoryParameter);

  void realPower10(const real_t *x, real_t *res, realContext_t *realContext);

#endif // !FILE_10POW_H
