// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/w_negative.h
 */
#if !defined(W_NEGATIVE_H)
  #define W_NEGATIVE_H

  #include "defines.h"
  #include <stdint.h>

  void fnWnegative(uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void wNegError  (void);
  #else // (EXTRA_INFO_ON_CALC_ERROR == 1)
    #define wNegError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void wNegLonI   (void);
  void wNegReal   (void);
  void wNegCplx   (void);

#endif // !W_NEGATIVE_H
