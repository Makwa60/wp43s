// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/ceil.h
 */
#if !defined(CEIL_H)
  #define CEIL_H

  #include "defines.h"
  #include <stdint.h>

  /**
   * regX ==> regL and ceil(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnCeil   (uint16_t unusedButMandatoryParameter);

  void ceilLonI (void);
  void ceilRema (void);
  void ceilReal (void);

#endif // !CEIL_H
