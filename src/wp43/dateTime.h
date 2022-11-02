// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file dateTime.h
 */
#if !defined(DATETIME_H)
  #define DATETIME_H

  #include "realType.h"
  #include "typeDefinitions.h"
  #include <stdbool.h>
  #include <stdint.h>

  void fnTicks                 (uint16_t unusedButMandatoryParameter);

  void fnSetDateFormat         (uint16_t dateFormat);

  /**
   * Convert internal date representation to Julian day number (floored)
   *
   * \param[in] source
   * \param[out] destination
   */
  void internalDateToJulianDay (const real34_t *source, real34_t *destination);

  /**
   * Convert Julian day number (floored) to internal date representation
   *
   * \param[in] source
   * \param[out] destination
   */
  void julianDayToInternalDate (const real34_t *source, real34_t *destination);

  /**
   * Check date argument and get Julian day
   *
   * \param[in] regist register
   * \param[out] jd Julian day (floored)
   * \return true if valid
   */
  bool checkDateArgument     (calcRegister_t regist, real34_t *jd);

  /**
   * Check for leap year
   *
   * \param[in] year
   * \return true if leap year
   */
  bool isLeapYear            (const real34_t *year);

  /**
   * Check if date is valid (e.g. 30 Feb is invalid)
   *
   * \param[in] year
   * \param[in] month
   * \param[in] day
   * \return true if valid
   */
  bool isValidDay            (const real34_t *year, const real34_t *month, const real34_t *day);

  /**
   * Convert date into Julian day number floored
   *
   * \param[in] year
   * \param[in] month
   * \param[in] day
   * \param[out] jd
   */
  void composeJulianDay        (const real34_t *year, const real34_t *month, const real34_t *day, real34_t *jd);

  void composeJulianDay_g      (const real34_t *year, const real34_t *month, const real34_t *day, real34_t *jd);
  void composeJulianDay_j      (const real34_t *year, const real34_t *month, const real34_t *day, real34_t *jd);

  /**
   * Convert Julian day number (floored) into date
   *
   * \param[in] jd
   * \param[out] year
   * \param[out] month
   * \param[out] day
   */
  void decomposeJulianDay      (const real34_t *jd, real34_t *year, real34_t *month, real34_t *day);

  /**
   * Get day of week from date in given register
   *
   * \param[in] regist register
   * \return day of week (1 = Monday, 7 = Sunday), 0 if invalid
   */
  uint32_t getDayOfWeek        (calcRegister_t regist);

  /**
   * Check date range
   *
   * \param[in] time34
   */
  void checkDateRange          (const real34_t *date34);

  /**
   * Convert H.MMSS into seconds
   *
   * \param[in] src H.MMSS-formatted time value (for input)
   * \param[out] dest time value in seconds (internal representation)
   */
  void hmmssToSeconds          (const real34_t *src, real34_t *dest);

  /**
   * Convert H.MMSS in given register into time
   *
   * \param[in] regist register
   */
  void hmmssInRegisterToSeconds(calcRegister_t regist);

  /**
   * Check time range
   *
   * \param[in] time34
   */
  void checkTimeRange          (const real34_t *time34);

  void fnJulianToDate          (uint16_t unusedButMandatoryParameter);
  void fnDateToJulian          (uint16_t unusedButMandatoryParameter);
  void fnIsLeap                (uint16_t unusedButMandatoryParameter);
  void fnSetFirstGregorianDay  (uint16_t unusedButMandatoryParameter);
  void fnGetFirstGregorianDay  (uint16_t unusedButMandatoryParameter);
  void fnXToDate               (uint16_t unusedButMandatoryParameter);
  void fnYear                  (uint16_t unusedButMandatoryParameter);
  void fnMonth                 (uint16_t unusedButMandatoryParameter);
  void fnDay                   (uint16_t unusedButMandatoryParameter);
  void fnWday                  (uint16_t unusedButMandatoryParameter);
  void fnDateTo                (uint16_t unusedButMandatoryParameter);
  void fnToDate                (uint16_t unusedButMandatoryParameter);
  void fnToHr                  (uint16_t unusedButMandatoryParameter);
  void fnToHms                 (uint16_t unusedButMandatoryParameter);
  void fnDate                  (uint16_t unusedButMandatoryParameter);
  void fnTime                  (uint16_t unusedButMandatoryParameter);
  void fnSetDate               (uint16_t unusedButMandatoryParameter);
  void fnSetTime               (uint16_t unusedButMandatoryParameter);

  /**
   * Gets the system date
   *
   * \param[out] dateString String receiving the system date.  Must be at least 11 bytes long (yyyy-mm-dd plus terminating 0)
   */
  void getDateString           (char *dateString);

  /**
   * Gets the system time
   *
   * \param[out] timeString String receiving the system time. Must be at least 8 bytes long (03:15pm plus terminating 0)
   * \return milliseconds until the string will change
   */
  uint32_t getTimeString           (char *timeString);

#endif // !DATETIME_H
