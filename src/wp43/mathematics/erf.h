// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/erf.h
 */
#if !defined(ERF_H)
  #define ERF_H

  #include "defines.h"
  #include <stdint.h>

  void fnErf   (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void erfError(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR == 1)
    #define erfError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void erfLonI (void);
  void erfReal (void);

#endif // !ERF_H
