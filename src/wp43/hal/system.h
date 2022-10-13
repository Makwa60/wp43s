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

#endif // !SYSTEM_H
