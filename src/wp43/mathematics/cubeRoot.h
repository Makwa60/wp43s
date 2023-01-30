// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/cubeRoot.h
 */
#if !defined(CUBEROOT_H)
  #define CUBEROOT_H

  #include "defines.h"
  #include "realType.h"
  #include <stdint.h>

  /**
   * regX ==> regL and curt(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnCubeRoot(uint16_t unusedButMandatoryParameter);

  void curtLonI   (void);
  void curtRema   (void);
  void curtCxma   (void);
  void curtShoI   (void);
  void curtReal   (void);
  void curtCplx   (void);

  void curtComplex(const real_t *real, const real_t *imag, real_t *resReal, real_t *resImag, realContext_t *realContext);
  void realCubeRoot(const real_t *x, real_t *res, realContext_t *realContext);

#endif // !CUBEROOT_H
