// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/xthRoot.h
 */
#if !defined(XTHROOT_H)
  #define XTHROOT_H
  // Coded by JM, based on power.h

  #include "defines.h"
  #include <stdint.h>
  #include "typeDefinitions.h"

  void fnXthRoot      (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void xthRootError   (void);
  #else // (EXTRA_INFO_ON_CALC_ERROR == 1)
    #define xthRootError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void xthRootReal(real_t *yy, real_t *xx, realContext_t *realContext);

  //      RegYRegX
  void xthRootLonILonI(void);
  void xthRootLonIShoI(void);
  void xthRootLonIReal(void);
  void xthRootLonICplx(void);

  //      RegYRegX
  void xthRootRemaLonI(void);
  void xthRootRemaShoI(void);
  void xthRootRemaReal(void);
  void xthRootRemaCplx(void);

  //      RegYRegX
  void xthRootCxmaLonI(void);
  void xthRootCxmaShoI(void);
  void xthRootCxmaReal(void);
  void xthRootCxmaCplx(void);

  //      RegYRegX
  void xthRootShoILonI(void);
  void xthRootShoIShoI(void);
  void xthRootShoIReal(void);
  void xthRootShoICplx(void);

  //      RegYRegX
  void xthRootRealLonI(void);
  void xthRootRealShoI(void);
  void xthRootRealReal(void);
  void xthRootRealCplx(void);

  //      RegYRegX
  void xthRootCplxLonI(void);
  void xthRootCplxShoI(void);
  void xthRootCplxReal(void);
  void xthRootCplxCplx(void);

#endif // !XTHROOT_H
