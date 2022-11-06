// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file mathematics/changeSign.h
 */
#if !defined(CHANGESIGN_H)
  #define CHANGESIGN_H

  #include "defines.h"
  #include <stdint.h>

  /**
   * rexX ==> regL and -regX ==> regX
   * Drops Y, enables stack lift and refreshes the stack
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnChangeSign(uint16_t unusedButMandatoryParameter);

  //      RegX
  void chsLonI     (void);
  void chsRema     (void);
  void chsCxma     (void);
  void chsShoI     (void);
  void chsReal     (void);
  void chsCplx     (void);

#endif // !CHANGESIGN_H
