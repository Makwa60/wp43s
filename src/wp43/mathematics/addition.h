// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/addition.h
 */
#if !defined(ADDITION_H)
  #define ADDITION_H

  #include "defines.h"
  #include <stdint.h>

  /**
   * regX ==> regL and regY + regX ==> regX
   * Drops Y, enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnAdd      (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void addError   (void);
  #else // (EXTRA_INFO_ON_CALC_ERROR == 1)
    #define addError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

#endif // !ADDITION_H
