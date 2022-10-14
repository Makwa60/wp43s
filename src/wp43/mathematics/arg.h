// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/arctan.h
 */
#if !defined(ARG_H)
  #define ARG_H

  #include "defines.h"
  #include <stdint.h>

  void fnArg      (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void argError   (void);
  #else // (EXTRA_INFO_ON_CALC_ERROR == 1)
    #define argError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void argReal    (void);
  void argCplx    (void);
  void argCxma    (void);

#endif // !ARG_H
