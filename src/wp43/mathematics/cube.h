// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/cube.h
 */
#if !defined(CUBE_H)
  #define CUBE_H

  #include "defines.h"
  #include <stdint.h>

  /**
   * regX ==> regL and regX × regX × regX ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnCube   (uint16_t unusedButMandatoryParameter);

  void cubeLonI (void);
  void cubeRema (void);
  void cubeCxma (void);
  void cubeShoI (void);
  void cubeReal (void);
  void cubeCplx (void);

#endif // !CUBE_H
