// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/roundi.h
 */
#if !defined(ROUNDI_H)
  #define ROUNDI_H

  #include "defines.h"
  #include <stdint.h>

  void fnRoundi   (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void roundiError(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR != 1)
    #define roundiError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void roundiLonI (void);
  void roundiRema (void);
  void roundiReal (void);

#endif // !ROUNDI_H
