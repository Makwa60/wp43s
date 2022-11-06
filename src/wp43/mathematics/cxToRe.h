// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/cxToRe.h
 */
#if !defined(CXTORE_H)
  #define CXTORE_H

  #include <stdint.h>

  /**
   * regX ==> regL and re(regX) ==> regY, im(regX) ==> regX or magnitude(regX) ==> regY, angle(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnCxToRe(uint16_t unusedButMandatoryParameter);

#endif // !CXTORE_H
