// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

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
