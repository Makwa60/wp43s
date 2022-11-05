// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/cpyx.h
 */
#if !defined(CPYX_H)
  #define CPYX_H

  #include "realType.h"
  #include <stdint.h>

  /**
   * regX ==> regL and Cyx(regX, RegY) ==> regX
   * enables stack lift and refreshes the stack.
   * C(n,k) = n! / [k! * (n-k)!]
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnCyx(uint16_t unusedButMandatoryParameter);

  /**
   * regX ==> regL and Pyx(regX, RegY) ==> regX
   * enables stack lift and refreshes the stack.
   * P(n,k) = n! / (n-k)!
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnPyx(uint16_t unusedButMandatoryParameter);

  void logCyxReal(real_t *y, real_t *x, real_t *result, realContext_t *realContext);

  /*
   * When combOrPerm == CP_COMBINATION => Cyx
   *      combOrPerm == CP_PERMUTATION => Pyx
   */
  void cpyxLonILonI(uint16_t combOrPerm);
  void cpyxLonIReal(uint16_t combOrPerm);
  void cpyxLonICplx(uint16_t combOrPerm);
  void cpyxLonIShoI(uint16_t combOrPerm);

  void cpyxRealLonI(uint16_t combOrPerm);
  void cpyxRealReal(uint16_t combOrPerm);
  void cpyxRealCplx(uint16_t combOrPerm);
  void cpyxRealShoI(uint16_t combOrPerm);

  void cpyxCplxLonI(uint16_t combOrPerm);
  void cpyxCplxReal(uint16_t combOrPerm);
  void cpyxCplxCplx(uint16_t combOrPerm);
  void cpyxCplxShoI(uint16_t combOrPerm);

  void cpyxShoILonI(uint16_t combOrPerm);
  void cpyxShoIReal(uint16_t combOrPerm);
  void cpyxShoICplx(uint16_t combOrPerm);
  void cpyxShoIShoI(uint16_t combOrPerm);

#endif // !CPYX_H
