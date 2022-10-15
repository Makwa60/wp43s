// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/beta.h
 */
#if !defined(BETA_H)
  #define BETA_H

  #include "defines.h"
  #include <stdint.h>

  void fnBeta      (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void betaError   (void);
  #else // (EXTRA_INFO_ON_CALC_ERROR == 1)
    #define betaError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void betaLonILonI(void);
  void betaRealLonI(void);
  void betaCplxLonI(void);

  void betaLonIReal(void);
  void betaRealReal(void);
  void betaCplxReal(void);

  void betaLonICplx(void);
  void betaRealCplx(void);
  void betaCplxCplx(void);

#endif // !BETA_H
