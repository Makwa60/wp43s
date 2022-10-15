// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file programming/manage.h
 */
#if !defined(MANAGE_H)
  #define MANAGE_H

  #include "typeDefinitions.h"
  #include <stdint.h>

  void scanLabelsAndPrograms                   (void);
  void defineCurrentProgramFromGlobalStepNumber(int16_t globalStepNumber);
  void defineCurrentProgramFromCurrentStep     (void);
  void deleteStepsFromTo                       (uint8_t *from, uint8_t *to);
  void fnClPAll                                (uint16_t confirmation);
  void fnClP                                   (uint16_t unusedButMandatoryParameter);
  void fnPem                                   (uint16_t unusedButMandatoryParameter);
  void scrollPemBackwards                      (void);
  void scrollPemForwards                       (void);
  void pemAlpha                                (int16_t item);
  void pemCloseAlphaInput                      (void);
  void pemAddNumber                            (int16_t item);
  void pemCloseNumberInput                     (void);
  void insertStepInProgram                     (int16_t func);
  void addStepInProgram                        (int16_t func);

  calcRegister_t findNamedLabel                (const char *labelName);
  uint16_t       getNumberOfSteps              (void);

#endif // !MANAGE_H
