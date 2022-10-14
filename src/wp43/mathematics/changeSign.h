// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/changeSign.h
 */
#if !defined(CHANGESIGN_H)
  #define CHANGESIGN_H

  #include "defines.h"
  #include <stdint.h>

  void fnChangeSign(uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void chsError    (void);
  #else // (EXTRA_INFO_ON_CALC_ERROR == 1)
    #define chsError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  //      RegX
  void chsLonI     (void);
  void chsRema     (void);
  void chsCxma     (void);
  void chsShoI     (void);
  void chsReal     (void);
  void chsCplx     (void);

#endif // !CHANGESIGN_H
