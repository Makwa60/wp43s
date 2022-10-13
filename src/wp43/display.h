// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file display.h
 */
#if !defined(DISPLAY_H)
  #define DISPLAY_H

  #include "longIntegerType.h"
  #include "realType.h"
  #include "typeDefinitions.h"
  #include <stdbool.h>
  #include <stdint.h>

  /**
   * Sets the display format FIX and refreshes the stack
   *
   * \param[in] displayFormatN Display format
   */
  void fnDisplayFormatFix                (uint16_t displayFormatN);

  /**
   * Sets the display format SCI and refreshes the stack
   *
   * \param[in] displayFormatN Display format
   */
  void fnDisplayFormatSci                (uint16_t displayFormatN);

  /**
   * \brief Sets the display format ENG and refreshes the stack
   *
   * \param[in] displayFormatN Display format
   */
  void fnDisplayFormatEng                (uint16_t displayFormatN);

  /**
   * Sets the display format ALL and refreshes the stack
   *
   * \param[in] displayFormatN Display format
   */
  void fnDisplayFormatAll                (uint16_t displayFormatN);

  /**
   * Sets the number of digits afer the period and refreshes the stack
   *
   * \param[in] displayFormatN Display format
   */
  void fnDisplayFormatDsp                (uint16_t displayFormatN);

  /**
   * Sets the GAP in number represemtation and refreshes the stack
   *
   * \param[in] gap
   */
  void fnDisplayFormatGap                (uint16_t gap);

  /**
   * Sets the display format for time and refreshes the stack
   *
   * \param[in] displayFormatN Display format
   */
  void fnDisplayFormatTime               (uint16_t displayFormatN);

  void fnShow                            (uint16_t unusedButMandatoryParameter);
  void mimShowElement                    (void);
  void fnView                            (uint16_t regist);
  void real34ToDisplayString             (const real34_t *real34, uint32_t tag, char *displayString, const font_t *font, int16_t maxWidth, int16_t displayHasNDigits, bool limitExponent, const char *separator, bool frontSpace);

  /**
   * Formats a real
   *
   * \param[out] displayString Result string
   * \param[in]  x Value to format
   */
  void real34ToDisplayString2            (const real34_t *real34, char *displayString, int16_t displayHasNDigits, bool limitExponent, const char *separator, bool noFix, bool frontSpace);

  void complex34ToDisplayString          (const complex34_t *complex34, char *displayString, const font_t *font, int16_t maxWidth, int16_t displayHasNDigits, bool limitExponent, const char *separator, bool frontSpace);
  void complex34ToDisplayString2         (const complex34_t *complex34, char *displayString, int16_t displayHasNDigits, bool limitExponent, const char *separator, bool frontSpace);
  void angle34ToDisplayString2           (const real34_t *angle34, uint8_t mode, char *displayString, int16_t displayHasNDigits, bool limitExponent, const char *separator, bool frontSpace);

  /**
   * Formats a fraction
   *
   * \param[in] regist
   * \param[in] displayString
   */
  void fractionToDisplayString           (calcRegister_t regist, char *displayString);

  void shortIntegerToDisplayString       (calcRegister_t regist, char *displayString, bool determineFont);
  void longIntegerRegisterToDisplayString(calcRegister_t regist, char *displayString, int32_t strLg, int16_t maxWidth, int16_t maxExp, const char *separator);
  void dateToDisplayString               (calcRegister_t regist, char *displayString);
  void timeToDisplayString               (calcRegister_t regist, char *displayString, bool ignoreTDisp);
  void real34MatrixToDisplayString       (calcRegister_t regist, char *displayString);
  void complex34MatrixToDisplayString    (calcRegister_t regist, char *displayString);

  /**
   * Adds the power of 10 using numeric font to displayString
   *
   * \param[out] displayString Result string
   * \param[in]  exponent Power of 10 to format
   */
  void exponentToDisplayString           (int32_t exponent, char *displayString, char *displayValueString, bool nimMode, const char *separator);

  void supNumberToDisplayString          (int32_t supNumber, char *displayString, char *displayValueString, bool insertGap, const char *separator);
  void subNumberToDisplayString          (int32_t subNumber, char *displayString, char *displayValueString);
  void longIntegerToAllocatedString      (const longInteger_t lgInt, char *str, int32_t strLen);

#endif // !DISPLAY_H
