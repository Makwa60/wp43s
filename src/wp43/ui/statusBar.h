// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file ui/statusBar.h
 * Status bar management.
 */
#if !defined(STATUSBAR_H)
  #define STATUSBAR_H

  #include <stdint.h>

  #if !defined(TESTSUITE_BUILD)
    /**
     * Refreshes the status bar.
     */
    void refreshStatusBar       (void);

    /**
     * Shows or hides the hourglass icon in the status bar.
     */
    void showHideHourGlass      (void);

    void cbTimeUpdate           (uint16_t param);

    #if defined(DMCP_BUILD)
      typedef enum {
        psUsb        = 0,
        psBattery    = 1,
        psBatteryLow = 2
      } powerStatus_t;

      void setPowerStatus         (powerStatus_t status);
    #endif // DMCP_BUILD
  #else
    #pragma GCC diagnostic ignored "-Wunused-parameter"

    static inline void cbTimeUpdate(uint16_t param) {}
  #endif // !TESTSUITE_BUILD

#endif // !STATUSBAR_H
