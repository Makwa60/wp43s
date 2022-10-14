// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/atan2.h
 */
#if !defined(ATAN2_H)
  #define ATAN2_H

  #include "defines.h"
  #include <stdint.h>

  void fnAtan2      (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void atan2Error   (void);
  #else // (EXTRA_INFO_ON_CALC_ERROR == 1)
    #define atan2Error typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void atan2LonILonI(void);
  void atan2RealLonI(void);
  void atan2RemaLonI(void);
  void atan2LonIReal(void);
  void atan2RealReal(void);
  void atan2RemaReal(void);
  void atan2LonIRema(void);
  void atan2RealRema(void);
  void atan2RemaRema(void);

#endif // !ATAN2_H
