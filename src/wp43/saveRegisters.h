// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file saveRegisters.h
 */
#if !defined(SAVEREGISTERS_H)
  #define SAVEREGISTERS_H

  #include "typeDefinitions.h"
  #include <stdint.h>

  #if !defined(TESTSUITE_BUILD)
    void copyRegisterToClipboardString(calcRegister_t regist, char *clipboardString);
  #endif // !defined(TESTSUITE_BUILD)
  void fnDumpRegXToFile(uint16_t unusedButMandatoryParameter);

#endif // !SAVEREGISTERS_H
