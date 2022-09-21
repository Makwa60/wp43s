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

#include "ui/statusBar.h"

#include "charString.h"
#include "dateTime.h"
#include "flags.h"
#include "fonts.h"
#include "hal/lcd.h"
#include "items.h"
#include "screen.h"

#include "wp43.h"

#if !defined(TESTSUITE_BUILD)
  void refreshStatusBar(void) {
    if(screenUpdatingMode & SCRUPD_MANUAL_STATUSBAR) {
      switch(calcMode) {
        case CM_PEM:
        case CM_REGISTER_BROWSER:
        case CM_FLAG_BROWSER:
        case CM_FONT_BROWSER:
        case CM_PLOT_STAT:
        case CM_CONFIRMATION:
        case CM_MIM:
        case CM_TIMER:
        case CM_GRAPH: {
          screenUpdatingMode &= ~SCRUPD_MANUAL_STATUSBAR;
          break;
        }

        default: {
          return;
        }
      }
    }
    #if (DEBUG_INSTEAD_STATUS_BAR == 1)
      sprintf(tmpString, "%s%d %s/%s  mnu:%s fi:%d", catalog ? "asm:" : "", catalog, tam.mode ? "/tam" : "", getCalcModeName(calcMode),indexOfItems[-softmenu[softmenuStack[0].softmenuId].menuItem].itemCatalogName, softmenuStack[0].firstItem);
      showString(tmpString, &standardFont, X_DATE, 0, vmNormal, true, true);
    #else // DEBUG_INSTEAD_STATUS_BAR != 1
      showDateTime();
      if(calcMode == CM_PLOT_STAT || calcMode == CM_GRAPH) {
        return;    // With graph displayed, only update the time, as the other items are clashing with the graph display screen
      }
      showRealComplexResult();
      showComplexMode();
      showAngularMode();
      showFracMode();
      if(calcMode == CM_MIM) {
        showMatrixMode();
      }
      else if(softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_TVM) {
        showTvmMode();
      }
      else {
        showIntegerMode();
        showOverflowCarry();
      }
      showHideAlphaMode();
      showHideHourGlass();
      showHideWatch();
      showHideSerialIO();
      showHidePrinter();
      showHideUserMode();
      #if defined(DMCP_BUILD)
        showHideUsbLowBattery();
      #else // !DMCP_BUILD
        showHideStackLift();
      #endif // DMCP_BUILD
    #endif // DEBUG_INSTEAD_STATUS_BAR == 1
  }



  void showDateTime(void) {
    lcd_fill_rect(0, 0, X_REAL_COMPLEX, 20, LCD_SET_VALUE);

    getDateString(dateTimeString);
    uint32_t x = showString(dateTimeString, &standardFont, X_DATE, 0, vmNormal, true, true);
    x = showGlyph(getSystemFlag(FLAG_TDM24) ? " " : STD_SPACE_3_PER_EM, &standardFont, x, 0, vmNormal, true, true); // is 0+0+8 pixel wide

    getTimeString(dateTimeString);
    showString(dateTimeString, &standardFont, x, 0, vmNormal, true, false);
  }



  void showRealComplexResult(void) {
    if(getSystemFlag(FLAG_CPXRES)) {
      showGlyph(STD_COMPLEX_C, &standardFont, X_REAL_COMPLEX, 0, vmNormal, true, false); // Complex C is 0+8+3 pixel wide
    }
    else {
      showGlyph(STD_REAL_R,    &standardFont, X_REAL_COMPLEX, 0, vmNormal, true, false); // Real R    is 0+8+3 pixel wide
    }
  }



  void showComplexMode(void) {
    if(getSystemFlag(FLAG_POLAR)) { // polar mode
     showGlyph(STD_SUN,           &standardFont, X_COMPLEX_MODE, 0, vmNormal, true, true); // Sun         is 0+12+2 pixel wide
    }
    else { // rectangular mode
     showGlyph(STD_RIGHT_ANGLE,   &standardFont, X_COMPLEX_MODE, 0, vmNormal, true, true); // Right angle is 0+12+2 pixel wide
    }
  }



  void showAngularMode(void) {
    uint32_t x = 0;

    x = showGlyph(STD_MEASURED_ANGLE, &standardFont, X_ANGULAR_MODE, 0, vmNormal, true, true); // Angle is 0+9 pixel wide

    switch(currentAngularMode) {
      case amRadian: {
        showGlyph(STD_SUP_r,              &standardFont, x, 0, vmNormal, true, false); // r  is 0+6 pixel wide
        break;
      }

      case amMultPi: {
        showGlyph(STD_pi,                 &standardFont, x, 0, vmNormal, true, false); // pi is 0+9 pixel wide
        break;
      }

      case amGrad: {
        showGlyph(STD_SUP_g,              &standardFont, x, 0, vmNormal, true, false); // g  is 0+6 pixel wide
        break;
      }

      case amDegree: {
        showGlyph(STD_DEGREE,             &standardFont, x, 0, vmNormal, true, false); // °  is 0+6 pixel wide
        break;
      }

      case amDMS: {
        showGlyph(STD_RIGHT_DOUBLE_QUOTE, &standardFont, x, 0, vmNormal, true, false); // "  is 0+6 pixel wide
        break;
      }

      default: {
        showGlyph(STD_QUESTION_MARK,      &standardFont, x, 0, vmNormal, true, false); // ?
      }
    }
  }



  void showFracMode(void) {
    if(getSystemFlag(FLAG_DENANY) && denMax == MAX_DENMAX) {
      showString("/max", &standardFont, X_FRAC_MODE, 0, vmNormal, true, true);
    }
    else {
      uint32_t x = 0;

      if((getSystemFlag(FLAG_DENANY) && denMax != MAX_DENMAX) || !getSystemFlag(FLAG_DENANY)) {
        sprintf(errorMessage, "/%" PRIu32, denMax);
        x = showString(errorMessage, &standardFont, X_FRAC_MODE, 0, vmNormal, true, true);
      }

      if(!getSystemFlag(FLAG_DENANY)) {
        if(getSystemFlag(FLAG_DENFIX)) {
          showGlyphCode('f',  &standardFont, x, 0, vmNormal, true, false); // f is 0+7+3 pixel wide
        }
        else {
          showString(PRODUCT_SIGN, &standardFont, x, 0, vmNormal, true, false); // STD_DOT is 0+3+2 pixel wide and STD_CROSS is 0+7+2 pixel wide
        }
      }
    }
  }



  void showIntegerMode(void) {
    if(shortIntegerWordSize <= 9) {
      sprintf(errorMessage, " %" PRIu8 ":%c", shortIntegerWordSize, shortIntegerMode==SIM_1COMPL?'1':(shortIntegerMode==SIM_2COMPL?'2':(shortIntegerMode==SIM_UNSIGN?'u':(shortIntegerMode==SIM_SIGNMT?'s':'?'))));
    }
    else {
      sprintf(errorMessage, "%" PRIu8 ":%c", shortIntegerWordSize, shortIntegerMode==SIM_1COMPL?'1':(shortIntegerMode==SIM_2COMPL?'2':(shortIntegerMode==SIM_UNSIGN?'u':(shortIntegerMode==SIM_SIGNMT?'s':'?'))));
    }

    showString(errorMessage, &standardFont, X_INTEGER_MODE, 0, vmNormal, true, true);
  }



  void showMatrixMode(void) {
    if(getSystemFlag(FLAG_GROW)) {
      sprintf(errorMessage, "grow");
    }
    else {
      sprintf(errorMessage, "wrap");
    }

    showString(errorMessage, &standardFont, X_INTEGER_MODE - 2, 0, vmNormal, true, true);
  }



  void showTvmMode(void) {
    if(getSystemFlag(FLAG_ENDPMT)) {
      sprintf(errorMessage, "END");
    }
    else {
      sprintf(errorMessage, "BEG");
    }

    showString(errorMessage, &standardFont, X_INTEGER_MODE, 0, vmNormal, true, true);
  }



  void showOverflowCarry(void) {
    showGlyph(STD_OVERFLOW_CARRY, &standardFont, X_OVERFLOW_CARRY, 0, vmNormal, true, false); // STD_OVERFLOW_CARRY is 0+6+3 pixel wide

    if(!getSystemFlag(FLAG_OVERFLOW)) { // Overflow flag is cleared
      lcd_fill_rect(X_OVERFLOW_CARRY, 2, 6, 7, LCD_SET_VALUE);
    }

    if(!getSystemFlag(FLAG_CARRY)) { // Carry flag is cleared
      lcd_fill_rect(X_OVERFLOW_CARRY, 12, 6, 7, LCD_SET_VALUE);
    }
  }



  void showHideAlphaMode(void) {
    if(calcMode == CM_AIM || calcMode == CM_EIM || (catalog && catalog != CATALOG_MVAR) || (tam.mode != 0 && tam.alpha) || ((calcMode == CM_PEM || calcMode == CM_ASSIGN) && getSystemFlag(FLAG_ALPHA))) {
      if(alphaCase == AC_UPPER) {
        showString(STD_ALPHA, &standardFont, X_ALPHA_MODE, 0, vmNormal, true, false); // STD_ALPHA is 0+9+2 pixel wide
        setSystemFlag(FLAG_alphaCAP);
      }
      else {
        showString(STD_alpha, &standardFont, X_ALPHA_MODE, 0, vmNormal, true, false); // STD_alpha is 0+9+2 pixel wide
        clearSystemFlag(FLAG_alphaCAP);
      }
    }
    else {
      clearSystemFlag(FLAG_alphaCAP);
    }
  }



  void showHideHourGlass(void) {
    if(screenUpdatingMode & SCRUPD_MANUAL_STATUSBAR) {
      switch(calcMode) {
        case CM_PEM:
        case CM_REGISTER_BROWSER:
        case CM_FLAG_BROWSER:
        case CM_FONT_BROWSER:
        case CM_PLOT_STAT:
        case CM_CONFIRMATION:
        case CM_MIM:
        case CM_TIMER:
        case CM_GRAPH: {
          screenUpdatingMode &= ~SCRUPD_MANUAL_STATUSBAR;
          break;
        }

        default: {
          return;
        }
      }
    }
    switch(programRunStop) {
      case PGM_WAITING: {
        showGlyph(STD_NEG_EXCLAMATION_MARK, &standardFont, (calcMode == CM_PLOT_STAT || calcMode == CM_GRAPH  ? 160-20 : X_HOURGLASS) - 1, 0, vmNormal, true, false);
        break;
      }
      case PGM_RUNNING: {
        lcd_fill_rect((calcMode == CM_PLOT_STAT || calcMode == CM_GRAPH ? 160-20 : X_HOURGLASS) - 1, 0, stringWidth(STD_NEG_EXCLAMATION_MARK, &standardFont, true, false), 20, LCD_SET_VALUE);
        showGlyph(STD_P, &standardFont, (calcMode == CM_PLOT_STAT || calcMode == CM_GRAPH ? 160-20 : X_HOURGLASS) + 1, 0, vmNormal, true, false);
        break;
      }
      default: {
        lcd_fill_rect((calcMode == CM_PLOT_STAT || calcMode == CM_GRAPH ? 160-20 : X_HOURGLASS) - 1, 0, stringWidth(STD_NEG_EXCLAMATION_MARK, &standardFont, true, false), 20, LCD_SET_VALUE);
        if(hourGlassIconEnabled) {
          showGlyph(STD_HOURGLASS, &standardFont, calcMode == CM_PLOT_STAT || calcMode == CM_GRAPH ? 160-20 : X_HOURGLASS, 0, vmNormal, true, false); // is 0+11+3 pixel wide //Shift the hourglass to a visible part of the status bar
        }
      }
    }
  }



  void showHideWatch(void) {
    if(watchIconEnabled) {
      showGlyph(STD_TIMER, &standardFont, X_WATCH, 0, vmNormal, true, false); // is 0+13+1 pixel wide
    }
  }



  void showHideSerialIO(void) {
    if(serialIOIconEnabled) {
      showGlyph(STD_SERIAL_IO, &standardFont, X_SERIAL_IO, 0, vmNormal, true, false); // is 0+8+3 pixel wide
    }
  }



  void showHidePrinter(void) {
    if(printerIconEnabled) {
      showGlyph(STD_PRINTER,   &standardFont, X_PRINTER, 0, vmNormal, true, false); // is 0+12+3 pixel wide
    }
  }



  void showHideUserMode(void) {
    if(getSystemFlag(FLAG_USER)) {
      showGlyph(STD_USER_MODE, &standardFont, X_USER_MODE, 0, vmNormal, false, false); // STD_USER_MODE is 0+12+2 pixel wide
    }
  }



  #if defined(DMCP_BUILD)
    void showHideUsbLowBattery(void) {
      if(getSystemFlag(FLAG_USB)) {
        showGlyph(STD_USB, &standardFont, X_BATTERY, 0, vmNormal, true, false); // is 0+9+2 pixel wide
      }
      else {
        if(getSystemFlag(FLAG_LOWBAT)) {
          showGlyph(STD_BATTERY, &standardFont, X_BATTERY, 0, vmNormal, true, false); // is 0+10+1 pixel wide
        }
      }
    }
  #endif // DMCP_BUILD



  #if !defined(DMCP_BUILD)
    void showHideStackLift(void) {
      if(getSystemFlag(FLAG_ASLIFT)) {
        // Draw S
        setBlackPixel(392,  1);
        setBlackPixel(393,  1);
        setBlackPixel(394,  1);
        setBlackPixel(391,  2);
        setBlackPixel(395,  2);
        setBlackPixel(391,  3);
        setBlackPixel(392,  4);
        setBlackPixel(393,  4);
        setBlackPixel(394,  4);
        setBlackPixel(395,  5);
        setBlackPixel(391,  6);
        setBlackPixel(395,  6);
        setBlackPixel(392,  7);
        setBlackPixel(393,  7);
        setBlackPixel(394,  7);

        // Draw L
        setBlackPixel(391, 10);
        setBlackPixel(391, 11);
        setBlackPixel(391, 12);
        setBlackPixel(391, 13);
        setBlackPixel(391, 14);
        setBlackPixel(391, 15);
        setBlackPixel(391, 16);
        setBlackPixel(392, 16);
        setBlackPixel(393, 16);
        setBlackPixel(394, 16);
        setBlackPixel(395, 16);
      }
      else {
        // Erase S
        setWhitePixel(392,  1);
        setWhitePixel(393,  1);
        setWhitePixel(394,  1);
        setWhitePixel(391,  2);
        setWhitePixel(395,  2);
        setWhitePixel(391,  3);
        setWhitePixel(392,  4);
        setWhitePixel(393,  4);
        setWhitePixel(394,  4);
        setWhitePixel(395,  5);
        setWhitePixel(391,  6);
        setWhitePixel(395,  6);
        setWhitePixel(392,  7);
        setWhitePixel(393,  7);
        setWhitePixel(394,  7);

        // Erase L
        setWhitePixel(391, 10);
        setWhitePixel(391, 11);
        setWhitePixel(391, 12);
        setWhitePixel(391, 13);
        setWhitePixel(391, 14);
        setWhitePixel(391, 15);
        setWhitePixel(391, 16);
        setWhitePixel(392, 16);
        setWhitePixel(393, 16);
        setWhitePixel(394, 16);
        setWhitePixel(395, 16);
      }
    }
  #endif // !DMCP_BUILD
#endif // !TESTSUITE_BUILD
