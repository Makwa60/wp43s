// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/sinh.h
 */
#if !defined(SINH_H)
  #define SINH_H

  #include "defines.h"
  #include <stdint.h>

  void fnSinh   (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void sinhError(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR != 1)
    #define sinhError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void sinhLonI (void);
  void sinhRema (void);
  void sinhCxma (void);
  void sinhReal (void);
  void sinhCplx (void);

#endif // !SINH_H
