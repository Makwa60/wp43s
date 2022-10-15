// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file logicalOps/not.h
 */
#if !defined(NOT_H)
  #define NOT_H

  #include "defines.h"
  #include <stdint.h>

  /**
   * regX ==> regL and not(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnLogicalNot(uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    /**
     * Data type error in logical not
     */
    void notError    (void);
  #else // (EXTRA_INFO_ON_CALC_ERROR == 1)
    #define notError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void notLonI     (void);
  void notShoI     (void);
  void notReal     (void);

#endif // !NOT_H
