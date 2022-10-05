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

#include "apps/fontBrowser.h"

#include "apps/apps.h"
#include "apps/bugScreen.h"
#include "calcMode.h"
#include "charString.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "items.h"
#include "ui/screen.h"
#include <assert.h>
#include <stdlib.h>

#include "wp43.h"

uint8_t currentFntScr;

#if !defined(TESTSUITE_BUILD)
  #define NUMBER_OF_GLYPH_ROWS 107
  #define NUMBER_OF_NUMERIC_FONT_LINES_PER_SCREEN    5
  #define NUMBER_OF_STANDARD_FONT_LINES_PER_SCREEN   8

  static uint16_t glyphRow[NUMBER_OF_GLYPH_ROWS];
  static uint8_t  numLinesNumericFont;
  static uint8_t  numLinesStandardFont;

  void fontBrowserInit(void) {
    uint16_t g;

    numLinesNumericFont  = 0;
    for(g=0; g<numericFont.numberOfGlyphs;) {
      glyphRow[numLinesNumericFont] = numericFont.glyphs[g].charCode & 0xfff0;
      while(g<numericFont.numberOfGlyphs && ((numericFont.glyphs[g].charCode&0xfff0) == glyphRow[numLinesNumericFont])) {
        g++;
      }
      numLinesNumericFont++;
    }

    numScreensNumericFont = numLinesNumericFont / NUMBER_OF_NUMERIC_FONT_LINES_PER_SCREEN;
    if(numLinesNumericFont % NUMBER_OF_NUMERIC_FONT_LINES_PER_SCREEN != 0) {
      numScreensNumericFont++;
    }

    numLinesStandardFont  = 0;
    for(g=0; g<standardFont.numberOfGlyphs;) {
      glyphRow[numLinesNumericFont + numLinesStandardFont] = standardFont.glyphs[g].charCode & 0xfff0;
      while(g<standardFont.numberOfGlyphs && ((standardFont.glyphs[g].charCode&0xfff0) == glyphRow[numLinesNumericFont + numLinesStandardFont])) {
        g++;
      }
      numLinesStandardFont++;
    }

    numScreensStandardFont = numLinesStandardFont / NUMBER_OF_STANDARD_FONT_LINES_PER_SCREEN;
    if(numLinesStandardFont % NUMBER_OF_STANDARD_FONT_LINES_PER_SCREEN != 0) {
      numScreensStandardFont++;
    }

    currentFntScr = 1;

    // NUMBER_OF_GLYPH_ROWS must be more than numLinesNumericFont + numLinesStandardFont
    assert(numLinesNumericFont + numLinesStandardFont <= NUMBER_OF_GLYPH_ROWS);
  }



  static bool_t _fontBrowserKeyHandler(int16_t item) {
    if(item == ITM_UP) {
      if(currentFntScr >= 2) {
        currentFntScr--;
      }
      return true;
    }
    else if(item == ITM_DOWN) {
      if(currentFntScr < numScreensNumericFont + numScreensStandardFont) {
        currentFntScr++;
      }
      return true;
    }
    return false;
  }



  void _fontBrowserDraw(void) {
    uint16_t x, y, first;

    hourGlassIconEnabled = false;

    assert(!getSystemFlag(FLAG_ALPHA));

    if(currentFntScr>=1 && currentFntScr<=numScreensNumericFont) { // Numeric font
      for(x=0; x<=9; x++) {
        showGlyphCode('0'+x, &standardFont, 50+20*x,     20, vmNormal, false, false);
      }
      for(x=0; x<=5; x++) {
        showGlyphCode('A'+x, &standardFont, 50+200+20*x, 20, vmNormal, false, false);
      }

      first = (currentFntScr-1) * NUMBER_OF_NUMERIC_FONT_LINES_PER_SCREEN;
      for(y=first; y<min(currentFntScr * NUMBER_OF_NUMERIC_FONT_LINES_PER_SCREEN, numLinesNumericFont); y++) {
        sprintf(tmpString, "%04X", glyphRow[y]<0x8000 ? glyphRow[y] : glyphRow[y]-0x8000);
        showString(tmpString, &standardFont, 5, NUMERIC_FONT_HEIGHT*(y-first)+43, vmNormal, false, false);
        for(x=0; x<=15; x++) {
          showGlyphCode(glyphRow[y]+x, &numericFont, 46+20*x, NUMERIC_FONT_HEIGHT*(y-first)+40, vmNormal, false, false);
        }
      }

      if(currentFntScr == 1) {
        showString("Numeric font. Press " STD_DOWN_ARROW " or EXIT", &standardFont, 5, 220, vmNormal, false, false);
      }
      else {
        showString("Numeric font. Press " STD_UP_ARROW ", " STD_DOWN_ARROW " or EXIT", &standardFont, 5, 220, vmNormal, false, false);
      }

      sprintf(tmpString, "%d/%d", currentFntScr, numScreensNumericFont+numScreensStandardFont);
      showString(tmpString, &standardFont, SCREEN_WIDTH - stringWidth(tmpString, &standardFont, false, true), 220, vmNormal, false, true);
    }

    else if(currentFntScr>numScreensNumericFont && currentFntScr<=numScreensNumericFont+numScreensStandardFont) { // Standard font
      for(x=0; x<=9; x++) {
        showGlyphCode('0'+x, &standardFont, 50+20*x,     20, vmNormal, false, false);
      }
      for(x=0; x<=5; x++) {
        showGlyphCode('A'+x, &standardFont, 50+200+20*x, 20, vmNormal, false, false);
      }

      first = numLinesNumericFont + (currentFntScr-numScreensNumericFont-1) * NUMBER_OF_STANDARD_FONT_LINES_PER_SCREEN;
      for(y=first; y<min(numLinesNumericFont + (currentFntScr-numScreensNumericFont) * NUMBER_OF_STANDARD_FONT_LINES_PER_SCREEN, numLinesNumericFont+numLinesStandardFont); y++) {
        sprintf(tmpString, "%04X", glyphRow[y]<0x8000 ? glyphRow[y] : glyphRow[y]-0x8000);
        showString(tmpString, &standardFont, 5, STANDARD_FONT_HEIGHT*(y-first)+40, vmNormal, false, false);
        for(x=0; x<=15; x++) {
          showGlyphCode(glyphRow[y]+x, &standardFont, 50+20*x, STANDARD_FONT_HEIGHT*(y-first)+40, vmNormal, false, false);
        }
      }

      if(currentFntScr == numScreensNumericFont+numScreensStandardFont) {
        showString("Standard font. Press " STD_UP_ARROW " or EXIT", &standardFont, 5, 220, vmNormal, false, false);
      }
      else {
        showString("Standard font. Press " STD_UP_ARROW ", " STD_DOWN_ARROW " or EXIT", &standardFont, 5, 220, vmNormal, false, false);
      }

      sprintf(tmpString, "%d/%d", currentFntScr, numScreensNumericFont+numScreensStandardFont);
      showString(tmpString, &standardFont, SCREEN_WIDTH-stringWidth(tmpString, &standardFont, false, true), 220, vmNormal, false, true);
    }
    else {
      bugScreen("In function showFonts: This should never happen!");
    }
  }



  void fnFontBrowser(uint16_t unusedButMandatoryParameter) {
    hourGlassIconEnabled = false;
    clearSystemFlag(FLAG_ALPHA);
    appsEnter(glFlagFontBrowser, _fontBrowserKeyHandler, _fontBrowserDraw);
  }
#endif // !TESTSUITE_BUILD
