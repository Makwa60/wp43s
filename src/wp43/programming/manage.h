/* This file is part of 43S.
 *
 * 43S is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * 43S is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 43S.  If not, see <http://www.gnu.org/licenses/>.
 */

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
