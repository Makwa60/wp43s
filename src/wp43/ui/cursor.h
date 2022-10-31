// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file ui/cursor.h
 */
#if !defined(CURSOR_H)
  #define CURSOR_H

  #include "typeDefinitions.h"
  #include <stdbool.h>
  #include <stdint.h>

  extern bool          cursorEnabled;
  extern uint32_t      xCursor;
  extern uint32_t      yCursor;
  extern const font_t *cursorFont;

  void cbCursorBlink(uint16_t param);

  /**
   * Hides the cursor.
   */
  void cursorHide(void);

  void cursorShow(bool useStandardFont, uint32_t x, uint32_t y);

  void cursorDraw(void);

#endif // !CURSOR_H
