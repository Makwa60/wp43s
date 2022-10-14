// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/xthRoot.h
 */
#if !defined(XTHROOT_H)
  #define XTHROOT_H

  #include "realType.h"
  #include <stdint.h>

  /**
   * regX ==> regL and regY ^ (1/regX) ==> regX
   * Drops Y, enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnXthRoot  (uint16_t unusedButMandatoryParameter);

  /**
   * y^(1/x)
   *
   * \param[in] Expecting x,y
   * \return REGISTER Y unchanged. REGISTER X with result of y^x
   */
  void xthRootReal(real_t *yy, real_t *xx, realContext_t *realContext);

#endif // !XTHROOT_H
