// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/cosh.h
 */
#if !defined(COSH_H)
  #define COSH_H

  #include "defines.h"
  #include <stdint.h>

  /**
   * regX ==> regL and cosh(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnCosh   (uint16_t unusedButMandatoryParameter);

  void coshLonI (void);
  void coshRema (void);
  void coshCxma (void);
  void coshReal (void);
  void coshCplx (void);

#endif // !COSH_H
