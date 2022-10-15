// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/erfc.h
 */
#if !defined(ERFC_H)
  #define ERFC_H

  #include "defines.h"
  #include <stdint.h>

  void fnErfc   (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void erfcError(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR == 1)
    #define erfcError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void erfcLonI (void);
  void erfcReal (void);

#endif // !ERFC_H
