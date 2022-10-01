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
 * \file hal/time.h
 */
#if !defined(TIME_H)
  #define TIME_H

  #include <stdint.h>

  /**
   * Structure for the time in a day.
   */
  typedef struct {
    uint8_t hour; ///< hour of the day in 24-hour format
    uint8_t min;  ///< minute of the current hour
    uint8_t sec;  ///< second of the current minute
    uint8_t csec; ///< centisecond (hundreth of a second) of the current second
  } timeInfo_t;

  /**
   * Structure for a date in the Gregorian calendar.
   */
  typedef struct {
    uint16_t year;  ///< year, not offset
    uint8_t  month; ///< month with January being 1
    uint8_t  day;   ///< day of the month starting from 1
  } dateInfo_t;

  /**
   * Get the current time.
   *
   * \param[out] ti pointer to a structure that will be filled out
   */
  void timeGetTimeInfo(timeInfo_t *ti);

  /**
   * Get the current date.
   *
   * \param[out] di pointer to a structure that will be filled out
   */
  void timeGetDateInfo(dateInfo_t *di);

  #if defined(DMCP_BUILD)
    /**
     * Set the current time.
     *
     * \param[in] ti pointer to a structure with the new time
     */
    void timeSetTimeInfo(timeInfo_t *ti);

    /**
     * Set the current date.
     *
     * \param[in] di pointer to a structure with the new date
     */
    void timeSetDateInfo(dateInfo_t *di);
  #endif

  /**
   * Get the current time in milliseconds.
   * This is the wall clock time. It should increase when the
   * program isn't running.
   *
   * \return time in milliseconds
   */
  uint32_t timeCurrentMs(void);

  /**
   * Get the uptime in milliseconds.
   * This doesn't need to reflect the current time but should be
   * monotonically increasing during runtime. It can pause when
   * the program is off.
   *
   * \return time in milliseconds
   */
  uint32_t timeUptimeMs(void);

#endif // !TIME_H
