// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file logicalOps/nor.h
 */
#if !defined(NOR_H)
  #define NOR_H

  #include "defines.h"
  #include <stdint.h>

  /**
   * regX ==> regL NOR regY ÷ regX ==> regX
   * Drops Y, enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnLogicalNor(uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    /**
     * Data type error in NOR
     */
    void norError24  (void);
  #else // (EXTRA_INFO_ON_CALC_ERROR == 1)
    #define norError24 typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void norError31  (void);
  void norLonILonI (void);
  void norLonIReal (void);
  void norRealLonI (void);
  void norRealReal (void);
  void norShoIShoI (void);

#endif // !NOR_H
