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
 * \file apps/timerApp.h
 */
#if !defined(TIMERAPP_H)
  #define TIMERAPP_H

  #include <stdint.h>

  void     fnTimerApp           (uint16_t unusedButMandatoryParameter);
  void     fnAddTimerApp        (uint16_t unusedButMandatoryParameter);
  void     fnDecisecondTimerApp (uint16_t unusedButMandatoryParameter);
  void     fnResetTimerApp      (uint16_t unusedButMandatoryParameter);
  void     fnRecallTimerApp     (uint16_t regist);

  void     timerAppStartStop    (void);
  void     timerAppStop         (void);
  void     timerAppDraw         (void);
  void     timerAppUpdate       (void);
  void     timerAppPoll         (void);
  void     timerAppLeave        (void);

  void     timerAppEnter      (void);
  void     timerAppDot        (void);
  void     timerAppUp         (void);
  void     timerAppDown       (void);
  void     timerAppDigitKey   (uint16_t digit);
  void     timerAppPlus       (void);
  void     timerAppBackspace  (void);

#endif // !TIMERAPP_H
