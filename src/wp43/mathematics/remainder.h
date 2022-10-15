// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/remainder.h
 */
#if !defined(REMAINDER_H)
  #define REMAINDER_H

  #include "defines.h"
  #include <stdint.h>

  void fnRmd(uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void rmdError(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR != 1)
    #define rmdError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  //      RegYRegX
  void rmdLonILonI(void);
  void rmdLonIShoI(void);
  void rmdLonIReal(void);

  //      RegYRegX
  void rmdShoILonI(void);
  void rmdShoIShoI(void);
  void rmdShoIReal(void);

  //      RegYRegX
  void rmdRealLonI(void);
  void rmdRealShoI(void);
  void rmdRealReal(void);

#endif // !REMAINDER_H
