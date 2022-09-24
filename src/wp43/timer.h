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
 * \file timer.h
 */
#if !defined(TIMER_H)
  #define TIMER_H

  #include <stdint.h>

  typedef enum {
    TMR_UNUSED    = 0,
    TMR_STOPPED   = 1,
    TMR_RUNNING   = 2,
    TMR_COMPLETED = 3
  } timerStatus_t;

  void          timerRefresh   (void);
  void          timerReset     (void);
  void          timerDummyTest (uint16_t param);
  void          timerConfig    (uint8_t nr, void(*func)(uint16_t), uint16_t param);
  void          timerStart     (uint8_t nr, uint16_t param, uint32_t time);      // Start Timer, 0..n-1
  void          timerStop      (uint8_t nr);                                     // Stop Timer, 0..n-1
  void          timerExec      (uint8_t nr);                                     // Execute Timer, 0..n-1
  void          timerDel       (uint8_t nr);                                     // Delete Timer, 0..n-1
  uint16_t      timerGetParam  (uint8_t nr);
  timerStatus_t timerGetStatus (uint8_t nr);

#endif // !TIMER_H
