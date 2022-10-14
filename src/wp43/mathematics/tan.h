// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/tan.h
 */
#if !defined(TAN_H)
  #define TAN_H

  #include "defines.h"
  #include "realType.h"
  #include "typeDefinitions.h"
  #include <stdint.h>

  void fnTan                    (uint16_t unusedButMandatoryParameter);
  void longIntegerAngleReduction(calcRegister_t regist, angularMode_t angularMode, real_t *reducedAngle);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void tanError                 (void);
  #else // (EXTRA_INFO_ON_CALC_ERROR != 1)
    #define tanError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void tanLonI                  (void);
  void tanRema                  (void);
  void tanCxma                  (void);
  void tanReal                  (void);
  void tanCplx                  (void);

  uint8_t TanComplex(const real_t *xReal, const real_t *xImag, real_t *rReal, real_t *rImag, realContext_t *realContext);

#endif // !TAN_H
