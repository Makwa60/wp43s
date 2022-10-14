// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/arccos.h
 */
#if !defined(ARCCOS_H)
  #define ARCCOS_H

  #include "defines.h"
  #include <stdint.h>

  void fnArccos   (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void arccosError(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR == 1)
    #define arccosError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void arccosLonI (void);
  void arccosRema (void);
  void arccosCxma (void);
  void arccosReal (void);
  void arccosCplx (void);

#endif // !ARCCOS_H
