// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/cube.h
 */
#if !defined(CUBE_H)
  #define CUBE_H

  #include "defines.h"
  #include <stdint.h>

  void fnCube   (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void cubeError(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR == 1)
    #define cubeError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void cubeLonI (void);
  void cubeRema (void);
  void cubeCxma (void);
  void cubeShoI (void);
  void cubeReal (void);
  void cubeCplx (void);

#endif // !CUBE_H
