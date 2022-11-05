// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/cos.h
 */
#if !defined(COS_H)
  #define COS_H

  #include "defines.h"
  #include "realType.h"
  #include <stdint.h>

  /**
   * regX ==> regL and cos(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnCos   (uint16_t unusedButMandatoryParameter);

  void cosLonI (void);
  void cosRema (void);
  void cosCxma (void);
  void cosReal (void);
  void cosCplx (void);
  void cosComplex(const real_t *real, const real_t *imag, real_t *resReal, real_t *resImag, realContext_t *realContext);

#endif // !COS_H
