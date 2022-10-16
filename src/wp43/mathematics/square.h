// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/square.h
 */
#if !defined(SQUARE_H)
  #define SQUARE_H

  #include "defines.h"
  #include <stdint.h>

  void fnSquare   (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void squareError(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR != 1)
    #define squareError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void squareLonI (void);
  void squareRema (void);
  void squareCxma (void);
  void squareShoI (void);
  void squareReal (void);
  void squareCplx (void);

#endif // !SQUARE_H
