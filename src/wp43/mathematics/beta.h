// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/beta.h
 */
#if !defined(BETA_H)
  #define BETA_H

  #include "defines.h"
  #include <stdint.h>

  /**
   * regX ==> regL and beta(regX, RegY) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnBeta      (uint16_t unusedButMandatoryParameter);

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
