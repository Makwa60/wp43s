// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "ui/cursor.h"

#include "fonts.h"
#include "hal/lcd.h"
#include "hal/timer.h"
#include "ui/screen.h"

#include "wp43.h"

bool           cursorEnabled;
uint32_t       xCursor;
uint32_t       yCursor;
const font_t  *cursorFont;

void drawCursor(bool show) {
  #if !defined(TESTSUITE_BUILD)
    if(show) {
      showGlyph(STD_CURSOR, cursorFont, xCursor, yCursor, vmNormal, true, false);
    }
    else {
      if(cursorFont == &standardFont) {
        lcd_fill_rect(xCursor, yCursor + 10,  6,  6, LCD_SET_VALUE);
      }
      else {
        lcd_fill_rect(xCursor, yCursor + 15, 13, 13, LCD_SET_VALUE);
      }
    }
  #endif // !TESTSUITE_BUILD
}



void cursorHide(void) {
  if(cursorEnabled) {
    drawCursor(false);
    cursorEnabled = false;
  }
}



void cursorShow(bool useStandardFont, uint32_t x, uint32_t y) {
  xCursor = x;
  yCursor = y;
  cursorFont = useStandardFont ? &standardFont : &numericFont;
  cursorEnabled = true;
  drawCursor(true);
}



void cursorDraw(void) {
  drawCursor(true);
}
