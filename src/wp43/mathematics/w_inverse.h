// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/w_inverse.h
 */
#if !defined(W_INVERSE_H)
  #define W_INVERSE_H

  #include "defines.h"
  #include <stdint.h>

  void fnWinverse(uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void wInvError  (void);
  #else // (EXTRA_INFO_ON_CALC_ERROR == 1)
    #define wInvError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void wInvLonI   (void);
  void wInvReal   (void);
  void wInvCplx   (void);

#endif // !W_INVERSE_H
