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
 * \file ui/statusBar.h
 * Status bar management.
 */
#if !defined(STATUSBAR_H)
  #define STATUSBAR_H

  #if !defined(TESTSUITE_BUILD)
    /**
     * Refreshes the status bar.
     */
    void refreshStatusBar       (void);

    /**
     * Displays date and time in the status bar.
     */
    void showDateTime           (void);

    /**
     * Shows or hides the hourglass icon in the status bar.
     */
    void showHideHourGlass      (void);

    #if defined(DMCP_BUILD)
      /**
       * Shows or hides the USB or low battery icon in the status bar.
       */
      void showHideUsbLowBattery(void);
    #endif // DMCP_BUILD
  #endif // !TESTSUITE_BUILD
#endif // !STATUSBAR_H
