// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/arctan.h
 */
#if !defined(ARG_H)
  #define ARG_H

  #include <stdint.h>

  /**
   * regX ==> regL and arctan(regX) ==> regX
   * regX ==> regL and arg(regX) = arctan(Im(regX) / Re(regX)) ==> regX
   * enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter uint16_t
   */
  void fnArg      (uint16_t unusedButMandatoryParameter);

#endif // !ARG_H
