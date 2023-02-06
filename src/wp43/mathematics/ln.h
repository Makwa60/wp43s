// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/ln.h
 */
#if !defined(LN_H)
  #define LN_H

  #include "defines.h"
  #include "realType.h"
  #include <stdint.h>

  void fnLn   (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void lnError(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR != 1)
    #define lnError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void lnLonI (void);
  void lnRema (void);
  void lnCxma (void);
  void lnShoI (void);
  void lnReal (void);
  void lnCplx (void);
  void lnComplex(const real_t *real, const real_t *imag, real_t *lnReal, real_t *lnImag, realContext_t *realContext);
  void real34Ln (const real34_t *x, real34_t *res);
  void realLn   (const real_t *x, real_t *res, realContext_t *realContext);

#endif // !LN_H
