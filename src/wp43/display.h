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
#if !defined(DISPLAY_H)
  #define DISPLAY_H

  #include "longIntegerType.h"
  #include "realType.h"
  #include "typeDefinitions.h"
  #include <stdint.h>

  void fnDisplayFormatFix                (uint16_t displayFormatN);
  void fnDisplayFormatSci                (uint16_t displayFormatN);
  void fnDisplayFormatEng                (uint16_t displayFormatN);
  void fnDisplayFormatAll                (uint16_t displayFormatN);
  void fnDisplayFormatDsp                (uint16_t displayFormatN);
  void fnDisplayFormatGap                (uint16_t gap);
  void fnDisplayFormatTime               (uint16_t displayFormatN);
  void fnShow                            (uint16_t unusedButMandatoryParameter);
  void mimShowElement                    (void);
  void fnView                            (uint16_t regist);
  void real34ToDisplayString             (const real34_t *real34, uint32_t tag, char *displayString, const font_t *font, int16_t maxWidth, int16_t displayHasNDigits, bool_t limitExponent, const char *separator, bool_t frontSpace);
  void real34ToDisplayString2            (const real34_t *real34, char *displayString, int16_t displayHasNDigits, bool_t limitExponent, const char *separator, bool_t noFix, bool_t frontSpace);
  void complex34ToDisplayString          (const complex34_t *complex34, char *displayString, const font_t *font, int16_t maxWidth, int16_t displayHasNDigits, bool_t limitExponent, const char *separator, bool_t frontSpace);
  void complex34ToDisplayString2         (const complex34_t *complex34, char *displayString, int16_t displayHasNDigits, bool_t limitExponent, const char *separator, bool_t frontSpace);
  void angle34ToDisplayString2           (const real34_t *angle34, uint8_t mode, char *displayString, int16_t displayHasNDigits, bool_t limitExponent, const char *separator, bool_t frontSpace);
  void fractionToDisplayString           (calcRegister_t regist, char *displayString);
  void shortIntegerToDisplayString       (calcRegister_t regist, char *displayString, bool_t determineFont);
  void longIntegerRegisterToDisplayString(calcRegister_t regist, char *displayString, int32_t strLg, int16_t maxWidth, int16_t maxExp, const char *separator);
  void dateToDisplayString               (calcRegister_t regist, char *displayString);
  void timeToDisplayString               (calcRegister_t regist, char *displayString, bool_t ignoreTDisp);
  void real34MatrixToDisplayString       (calcRegister_t regist, char *displayString);
  void complex34MatrixToDisplayString    (calcRegister_t regist, char *displayString);
  void exponentToDisplayString           (int32_t exponent, char *displayString, char *displayValueString, bool_t nimMode, const char *separator);
  void supNumberToDisplayString          (int32_t supNumber, char *displayString, char *displayValueString, bool_t insertGap, const char *separator);
  void subNumberToDisplayString          (int32_t subNumber, char *displayString, char *displayValueString);
  void longIntegerToAllocatedString      (const longInteger_t lgInt, char *str, int32_t strLen);
#endif // !DISPLAY_H
