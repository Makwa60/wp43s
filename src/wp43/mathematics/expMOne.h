// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/expMOne.h
 */
#if !defined(EXPMONE_H)
  #define EXPMONE_H
  // Coded by JM, based on exp.h

  #include "defines.h"
  #include "realType.h"
  #include <stdint.h>

  void fnExpM1   (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void expM1Error(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR != 1)
    #define expM1Error typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void expM1LonI (void);
  void expM1Rema (void);
  void expM1Cxma (void);
  void expM1ShoI (void);
  void expM1Real (void);
  void expM1Cplx (void);
  void expM1Complex(const real_t *real, const real_t *imag, real_t *resReal, real_t *resImag, realContext_t *realContext);
  void realExpM1   (const real_t *xin, real_t *res, realContext_t *realContext);

#endif // !EXPMONE_H
