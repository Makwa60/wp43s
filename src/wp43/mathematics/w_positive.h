// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/w_positive.h
 */
#if !defined(W_POSITIVE_H)
  #define W_POSITIVE_H

  #include "defines.h"
  #include <stdint.h>

  void fnWpositive(uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void wPosError  (void);
  #else // (EXTRA_INFO_ON_CALC_ERROR == 1)
    #define wPosError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void wPosLonI   (void);
  void wPosReal   (void);
  void wPosCplx   (void);

#endif // !W_POSITIVE_H
