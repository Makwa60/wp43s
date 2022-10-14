// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/arccos.h
 */
#if !defined(ARCCOS_H)
  #define ARCCOS_H

  #include <stdint.h>

  /**
   * regX ==> regL and arccos(regX) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnArccos(uint16_t unusedButMandatoryParameter);

#endif // !ARCCOS_H
