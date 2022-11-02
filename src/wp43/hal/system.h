// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file hal/system.h
 */
#if !defined(SYSTEM_H)
  #define SYSTEM_H

  #include <stdint.h>

  /**
   * Take a screenshot.
   * Creates an image file with an appropriate name that is a bitmap
   * of the contents of the LCD.
   */
  void systemScreenshot(void);

  /**
   * Identify the maker of the platform.
   *
   * \return string identifying the maker that will appear in the WHO?
   * screen
   */
  const char *systemMaker(void);

  /**
   * Process events in the platform event queue.
   * These events should trigger the usual btnPressed or timerRun calls
   * as usual. This method is called when there is a long delay before
   * returning to the main loop, such as when executing a program.
   */
  void systemProcessEvents(void);

  /**
   * Quit the application.
   * This is called after the state has been saved.
   */
  void systemQuit(void);

  /**
   * Get the current battery voltage if applicable.
   * Systems without a battery should return the fixed value 3100.
   *
   * \return battery voltage
   */
  uint32_t systemBatteryVoltage(void);

#endif // !SYSTEM_H
