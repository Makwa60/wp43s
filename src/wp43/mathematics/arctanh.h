// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/arctanh.h
 */
#if !defined(ARCTANH_H)
  #define ARCTANH_H

  #include "defines.h"
  #include <stdint.h>

  void fnArctanh   (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void arctanhError(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR == 1)
    #define arctanhError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void arctanhLonI (void);
  void arctanhRema (void);
  void arctanhCxma (void);
  void arctanhReal (void);
  void arctanhCplx (void);

#endif // !ARCTANH_H
