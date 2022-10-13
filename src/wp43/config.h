// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file config.h
 */
#if !defined(CONFIG_H)
  #define CONFIG_H

  #include "realType.h"
  #include <stdint.h>

  extern const realRoundingMode_t roundingModeTable[7];

  /**
   * Sets the calc to meet Chinese standards
   *
   * \param[in] unusedButMandatoryParameter
   */
  void     fnConfigChina         (uint16_t unusedButMandatoryParameter);

  /**
   * Sets the calc to meet European standards
   *
   * \param[in] unusedButMandatoryParameter
   */
  void     fnConfigEurope        (uint16_t unusedButMandatoryParameter);

  /**
   * Sets the calc to meet Indian standards
   *
   * \param[in] unusedButMandatoryParameter
   */
  void     fnConfigIndia         (uint16_t unusedButMandatoryParameter);

  /**
   * Sets the calc to meet Japanese standards
   *
   * \param[in] unusedButMandatoryParameter
   */
  void     fnConfigJapan         (uint16_t unusedButMandatoryParameter);

  /**
   * Sets the calc to meet English standards
   *
   * \param[in] unusedButMandatoryParameter
   */
  void     fnConfigUk            (uint16_t unusedButMandatoryParameter);

  /**
   * Sets the calc to meet American standards
   *
   * \param[in] unusedButMandatoryParameter
   */
  void     fnConfigUsa           (uint16_t unusedButMandatoryParameter);

  /**
   * Sets the integer mode
   *
   * \param[in] mode Integer mode
   */
  void     fnIntegerMode         (uint16_t mode);

  /**
   * Displays credits to the brave men who
   * made this project work.
   *
   * \param[in] unusedButMandatoryParameter
   */
  void     fnWho                 (uint16_t unusedButMandatoryParameter);

  /**
   * Displays the version of this software
   *
   * \param[in] unusedButMandatoryParameter
   */
  void     fnVersion             (uint16_t unusedButMandatoryParameter);

  /**
   * Sets X to the amount of free RAM
   *
   * \param[in] unusedButMandatoryParameter
   */
  void     fnFreeMemory          (uint16_t unusedButMandatoryParameter);

  /**
   * Sets X to the value of the rounding mode
   *
   * \param[in] unusedButMandatoryParameter
   */
  void     fnGetRoundingMode     (uint16_t unusedButMandatoryParameter);

  /**
   * Sets the rounding mode
   *
   * \param[in] RM
   */
  void     fnSetRoundingMode     (uint16_t RM);

  /**
   * Sets X to the value of the integer mode
   */
  void     fnGetIntegerSignMode  (uint16_t unusedButMandatoryParameter);

  /**
   * Gets the word size
   *
   * \param[in] unusedButMandatoryParameter
   */
  void     fnGetWordSize         (uint16_t unusedButMandatoryParameter);

  /**
   * Sets the word size
   *
   * \param[in] unusedButMandatoryParameter
   */
  void     fnSetWordSize         (uint16_t WS);

  /**
   * Sets X to the amount of free flash memory
   *
   * \param[in] unusedButMandatoryParameter
   */
  void     fnFreeFlashMemory     (uint16_t unusedButMandatoryParameter);

  /**
   * Sets X to the battery voltage
   *
   * \param[in] unusedButMandatoryParameter
   */
  void     fnBatteryVoltage      (uint16_t unusedButMandatoryParameter);

  /**
   * Retrives the amount of free flash memory
   *
   * \return Number of bytes free
   */
  uint32_t getFreeFlash          (void);

  /**
   * Sets X to the number of signficant digits
   * rounding after each operation
   *
   * \param[in] unusedButMandatoryParameter
   */
  void     fnGetSignificantDigits(uint16_t unusedButMandatoryParameter);

  /**
   * Sets the number of signficant digits
   * rounding after each operation to X
   *
   * \param[in] unusedButMandatoryParameter
   */
  void     fnSetSignificantDigits(uint16_t unusedButMandatoryParameter);

  /**
   * Sets the rounding mode
   *
   * \param[in] RM Rounding mode
   */
  void     fnRoundingMode        (uint16_t RM);

  /**
   * Sets the angular mode
   *
   * \param[in] am Angular mode
   */
  void     fnAngularMode         (uint16_t angularMode);

  /**
   * Activates fraction display and toggles which of improper (like 3/2) or mixed (like 1 1/2) fractions is preferred.
   *
   * \param[in] unusedButMandatoryParameter
   */
  void     fnFractionType        (uint16_t unusedButMandatoryParameter);

  void     restoreStats          (void);
  void     fnReset               (uint16_t confirmation);
  void     setConfirmationMode   (void (*func)(uint16_t));
  void     fnClAll               (uint16_t confirmation);
  void     backToSystem          (uint16_t unusedButMandatoryParameter);
  void     fnRange               (uint16_t unusedButMandatoryParameter);
  void     fnGetRange            (uint16_t unusedButMandatoryParameter);
  void     fnHide                (uint16_t digits);
  void     fnGetHide             (uint16_t unusedButMandatoryParameter);

#endif // !CONFIG_H
