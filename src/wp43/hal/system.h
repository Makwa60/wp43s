// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file hal/system.h
 */
#if !defined(SYSTEM_H)
  #define SYSTEM_H

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
   * Quit the application.
   * This is called after the state has been saved.
   */
  void systemQuit(void);

#endif // !SYSTEM_H
