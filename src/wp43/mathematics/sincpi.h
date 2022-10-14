// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/sinc.h
 */
#if !defined(SINCPI_H)
  #define SINCPI_H
  // Coded by JM, based on sinc.h

  #include "defines.h"
  #include "realType.h"
  #include <stdint.h>

  void fnSincpi(uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void sincpiError(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR != 1)
    #define sincpiError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void sincpiLonI (void);
  void sincpiRema (void);
  void sincpiCxma (void);
  void sincpiReal (void);
  void sincpiCplx (void);
  void sincpiComplex(const real_t *real, const real_t *imag, real_t *resReal, real_t *resImag, realContext_t *realContext);

#endif // !SINCPI_H
