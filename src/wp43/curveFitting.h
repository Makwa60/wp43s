// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file curveFitting.h
 */
#if !defined(CURVEFITTING_H)
  #define CURVEFITTING_H

  #include "realType.h"
  #include <stdint.h>

  extern   realContext_t              *realContext;

  /**
   * Sets the curve fitting mode
   *
   * Input from defines, is "1" to exclude a method, examples:
   * LinF=CF_LINEAR_FITTING = 1. This "1" excludes LinF
   * 448 = 1 1100 0000, Excluding 3 param models.
   * 0   = 0 0000 0000, Exlcluding nothing
   * 510 = 1 1111 1110, Excludes everything except LINF (default)
   * 511 not allowed from keyboard, as it is internally used to allow ORTHOF.
   *
   * The internal representation reverses the logic, i.e. ones represent allowed methods
   *
   * \param[in] curveFitting Curve fitting mode
   */
  void     fnCurveFitting             (uint16_t curveFitting);

  void     processCurvefitSelection   (uint16_t selection, real_t *RR_, real_t *SMI_, real_t *aa0, real_t *aa1, real_t *aa2);

  /**
   * Calculates the curve fitting parameters r, smi, a0, a1, a2
   *
   * \param[in] curveFitting uint16_t Curve fitting mode, and pointers to the real variables
   */
  void     processCurvefitSelectionAll(uint16_t selection, real_t *RR_, real_t *MX, real_t *MX2, real_t *SX2, real_t *SY2, real_t *SMI_, real_t *aa0, real_t *aa1, real_t *aa2);

  /**
   * Finds the best curve fit
   *
   * Do not convert from 0 to 511 here. Conversion only done after input.
   * Default of 0 is defined in ReM to be the same as 511
   *
   * \param[in] curveFitting Curve fitting mode. Binary positions indicate which curves to be considered.
   */
  void     fnProcessLR                (uint16_t unusedButMandatoryParameter);

  uint16_t lrCountOnes                (uint16_t curveFitting);
  uint16_t  minLRDataPoints           (uint16_t selection);
  void     yIsFnx                     (uint8_t  USEFLOAT, uint16_t selection, double x, double *y, double a0, double a1, double a2, real_t *XX, real_t *YY, real_t *RR, real_t *SMI, real_t *aa0, real_t *aa1, real_t *aa2);
  void     fnYIsFnx                   (uint16_t unusedButMandatoryParameter);
  void     fnXIsFny                   (uint16_t unusedButMandatoryParameter);

  /**
   * Sets X to the set L.R.
   *
   * \param[in] unusedButMandatoryParameter
   */
  void     fnCurveFittingLR           (uint16_t unusedButMandatoryParameter);

#endif // !CURVEFITTING_H
