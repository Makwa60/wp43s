// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/dot.h
 */
#if !defined(DOT_H)
  #define DOT_H

  #include <stdint.h>

  /**
   * regX ==> regL and Dot(regX, RegY) ==> regX
   * enables stack lift and refreshes the stack.
   * Calculate the dot (or scalar) product between complex and matrix
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnDot(uint16_t unusedButMandatoryParameter);

  void dotRealCplx(void);
  void dotLonICplx(void);
  void dotShoICplx(void);
  void dotCplxCplx(void);
  void dotCplxReal(void);
  void dotCplxLonI(void);
  void dotCplxShoI(void);

  void dotRemaRema(void);
  void dotCpmaRema(void);
  void dotRemaCpma(void);
  void dotCpmaCpma(void);

#endif // !DOT_H
