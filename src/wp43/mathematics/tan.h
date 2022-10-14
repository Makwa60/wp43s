// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/tan.h
 */
#if !defined(TAN_H)
  #define TAN_H

  #include "realType.h"
  #include "typeDefinitions.h"
  #include <stdint.h>

  /**
   * regX ==> regL and tan(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnTan                    (uint16_t unusedButMandatoryParameter);

  void longIntegerAngleReduction(calcRegister_t regist, angularMode_t angularMode, real_t *reducedAngle);

  uint8_t TanComplex            (const real_t *xReal, const real_t *xImag, real_t *rReal, real_t *rImag, realContext_t *realContext);

#endif // !TAN_H
