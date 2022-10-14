// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/expt.h
 */
#if !defined(EXPT_H)
  #define EXPT_H

  #include "defines.h"
  #include <stdint.h>

  void fnExpt   (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void exptError(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR != 1)
    #define exptError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void exptLonI (void);
  void exptReal (void);

#endif // !EXPT_H
