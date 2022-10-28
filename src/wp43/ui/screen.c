// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "ui/screen.h"

#include "assign.h"
#include "apps/apps.h"
#include "apps/bugScreen.h"
#include "apps/flagBrowser.h"
#include "apps/fontBrowser.h"
#include "apps/registerBrowser.h"
#include "apps/timerApp.h"
#include "calcMode.h"
#include "charString.h"
#include "constantPointers.h"
#include "core/memory.h"
#include "curveFitting.h"
#include "dateTime.h"
#include "debug.h"
#include "defines.h"
#include "display.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "hal/debug.h"
#include "hal/lcd.h"
#include "hal/system.h"
#include "hal/time.h"
#include "hal/timer.h"
#include "items.h"
#include "longIntegerType.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/incDec.h"
#include "matrix.h"
#include "plotstat.h"
#include "programming/manage.h"
#include "registers.h"
#include "registerValueConversions.h"
#include "ui/bufferize.h"
#include "ui/cursor.h"
#include "ui/keyboard.h"
#include "ui/softmenus.h"
#include "ui/statusBar.h"
#include "ui/tam.h"
#include "version.h"
#include <stdbool.h>
#include <string.h>

#include "wp43.h"

#if !defined(TESTSUITE_BUILD)
  static const char *whoStr1 = "WP43" STD_SPACE_3_PER_EM "by" STD_SPACE_3_PER_EM "Pauli," STD_SPACE_3_PER_EM "Walter," STD_SPACE_3_PER_EM "Mihail," STD_SPACE_3_PER_EM "Jaco," STD_SPACE_3_PER_EM "and" STD_SPACE_3_PER_EM "Martin";
  static const char *versionStr = "WP43" STD_SPACE_3_PER_EM VERSION_STRING;

  /* Names of day of week */
  TO_QSPI static const char *nameOfWday_en[8] = {"invalid day of week",                                   "Monday",            "Tuesday",                     "Wednesday",               "Thursday",           "Friday",             "Saturday",             "Sunday"};
  /*
  TO_QSPI static const char *nameOfWday_de[8] = {"ung" STD_u_DIARESIS "ltiger Wochentag",                 "Montag",            "Dienstag",                    "Mittwoch",                "Donnerstag",         "Freitag",            "Samstag",              "Sonntag"};
  TO_QSPI static const char *nameOfWday_fr[8] = {"jour de la semaine invalide",                           "lundi",             "mardi",                       "mercredi",                "jeudi",              "vendredi",           "samedi",               "dimanche"};
  TO_QSPI static const char *nameOfWday_es[8] = {"d" STD_i_ACUTE "a inv" STD_a_ACUTE "lido de la semana", "lunes",             "martes",                      "mi" STD_e_ACUTE "rcoles", "jueves",             "viernes",            "s" STD_a_ACUTE "bado", "domingo"};
  TO_QSPI static const char *nameOfWday_it[8] = {"giorno della settimana non valido",                     "luned" STD_i_GRAVE, "marted" STD_i_GRAVE,          "mercoled" STD_i_GRAVE,    "gioved" STD_i_GRAVE, "venerd" STD_i_GRAVE, "sabato",               "domenica"};
  TO_QSPI static const char *nameOfWday_pt[8] = {"dia inv" STD_a_ACUTE "lido da semana",                  "segunda-feira",     "ter" STD_c_CEDILLA "a-feira", "quarta-feira",            "quinta-feira",       "sexta-feira",        "s" STD_a_ACUTE "bado", "domingo"};
  */
#endif // !TESTSUITE_BUILD



void clearScreen(void) {
  lcd_fill_rect(0, 0, SCREEN_WIDTH, 240, LCD_SET_VALUE);
}



#if !defined(TESTSUITE_BUILD)
  uint32_t showGlyphCode(uint16_t charCode, const font_t *font, uint32_t x, uint32_t y, videoMode_t videoMode, bool showLeadingCols, bool showEndingCols) {
    uint32_t  col, row, xGlyph, endingCols;
    int32_t glyphId;
    int8_t   byte, *data;
    const glyph_t  *glyph;

    glyphId = findGlyph(font, charCode);
    if(glyphId >= 0) {
      glyph = (font->glyphs) + glyphId;
    }
    else if(glyphId == -1) {
      generateNotFoundGlyph(-1, charCode);
      glyph = &glyphNotFound;
    }
    else if(glyphId == -2) {
      generateNotFoundGlyph(-2, charCode);
      glyph = &glyphNotFound;
    }
    else {
      glyph = NULL;
    }

    if(glyph == NULL) {
      sprintf(errorMessage, "In function showGlyphCode: %" PRIi32 " is an unexpected value returned by fingGlyph!", glyphId);
      bugScreen(errorMessage);
      return 0;
    }

    data = (int8_t *)glyph->data;

    xGlyph      = showLeadingCols ? glyph->colsBeforeGlyph : 0;
    endingCols  = showEndingCols ? glyph->colsAfterGlyph : 0;

    // Clearing the space needed by the glyph
    lcd_fill_rect(x, y, xGlyph + glyph->colsGlyph + endingCols, glyph->rowsAboveGlyph + glyph->rowsGlyph + glyph->rowsBelowGlyph, (videoMode == vmNormal ? LCD_SET_VALUE : LCD_EMPTY_VALUE));
    y += glyph->rowsAboveGlyph;
    x += xGlyph;

    // Drawing the glyph
    for(row=0; row<glyph->rowsGlyph; row++, y++) {
      // Drawing the columns of the glyph
      int32_t bit = 7;
      for(col=0; col<glyph->colsGlyph; col++) {
        if(bit == 7) {
          byte = *(data++);
        }

        if(byte & 0x80) {// MSB set
          if(videoMode == vmNormal) { // Black pixel for white background
            setBlackPixel(x + col, y);
          }
          else { // White pixel for black background
            setWhitePixel(x +col, y);
          }
        }

        byte <<= 1;

        if(--bit == -1) {
          bit = 7;
        }
      }
    }

    return x + glyph->colsGlyph + endingCols;
  }



  /* Returns the character code from the first glyph of a string.
   *
   * \param[in]     ch     String whose first glyph is to extract
   * \param[in,out] offset Offset which is updated, or null if zero and no update
   * \return Character code for that glyph
   */
  static uint16_t charCodeFromString(const char *ch, uint16_t *offset) {
    uint16_t charCode;
    uint16_t loffset = (offset != 0) ? *offset : 0;

    charCode = (uint8_t)ch[loffset++];
    if(charCode &0x0080) {
      charCode = (charCode << 8) | (uint8_t)ch[loffset++];
    }
    if(offset != 0) {
      *offset = loffset;
    }
    return charCode;
  }



  uint32_t showGlyph(const char *ch, const font_t *font, uint32_t x, uint32_t y, videoMode_t videoMode, bool showLeadingCols, bool showEndingCols) {
    return showGlyphCode(charCodeFromString(ch, 0), font, x, y, videoMode, showLeadingCols, showEndingCols);
  }



  /* Finds the cols and rows for a glyph.
   *
   * \param[in]     ch     const char*   String whose first glyph is to find the bounds for
   * \param[in,out] offset uint16_t*     Offset for string or null if zero should be used
   * \param[in]     font   const font_t* Font to use
   * \param[out]    col    uint32_t*     Number of columns for the glyph
   * \param[out]    row    uint32_t*     Number of rows for the glyph
   */
  static void getGlyphBounds(const char *ch, uint16_t *offset, const font_t *font, uint32_t *col, uint32_t *row) {
    int32_t        glyphId;
    const glyph_t *glyph;

    glyphId = findGlyph(font, charCodeFromString(ch, offset));
    if(glyphId < 0) {
      sprintf(errorMessage, "In function getGlyphBounds: %" PRIi32 " is an unexpected value returned by findGlyph!", glyphId);
      bugScreen(errorMessage);
      return;
    }
    glyph = (font->glyphs) + glyphId;
    *col = glyph->colsBeforeGlyph + glyph->colsGlyph + glyph->colsAfterGlyph;
    *row = glyph->rowsAboveGlyph + glyph->rowsGlyph + glyph->rowsBelowGlyph;
  }



  uint32_t showString(const char *string, const font_t *font, uint32_t x, uint32_t y, videoMode_t videoMode, bool showLeadingCols, bool showEndingCols) {
    uint16_t ch, lg;
    bool     slc, sec;

    lg = stringByteLength(string);

    ch = 0;
    while(string[ch] != 0) {
      if(lg == 1 || (lg == 2 && (string[0] & 0x80))) { // The string is 1 glyph long
        slc = showLeadingCols;
        sec = showEndingCols;
      }
      else if(ch == 0) {// First glyph
        slc = showLeadingCols;
        sec = true;
      }
      else if(ch == lg-1 || (ch == lg-2 && (string[ch] & 0x80))) { // Last glyph
        slc = true;
        sec = showEndingCols;
      }
      else {// Glyph between first and last glyph
        slc = true;
        sec = true;
      }

      x = showGlyphCode(charCodeFromString(string, &ch), font, x, y, videoMode, slc, sec);
    }
    return x;
  }



  /* Finds the cols and rows for a string if showing leading and ending columns.
   *
   * \param[in]  ch   const char*   String to find the bounds of
   * \param[in]  font const font_t* Font to use
   * \param[out] col  uint32_t*     Number of columns for the string
   * \param[out] row  uint32_t*     Number of rows for the string
   */
  static void getStringBounds(const char *string, const font_t *font, uint32_t *col, uint32_t *row) {
    uint16_t ch = 0;
    uint32_t lcol, lrow;
    *col = 0;
    *row = 0;

    while(string[ch] != 0) {
      getGlyphBounds(string, &ch, font, &lcol, &lrow);
      *col += lcol;
      if(lrow > *row) {
        *row = lrow;
      }
    }
  }



  void cbShowNop(uint16_t param) {
    hideFunctionName();
    tmpString[0] = 0;
    showFunctionName(ITM_NOP, 0);
  }



  void showFunctionName(int16_t item, int16_t delayInMs) {
    uint32_t fcol, frow, gcol, grow;
    char *functionName;

    if(tmpString[0] != 0) {
      functionName = tmpString;
    }
    else if(item != MNU_DYNAMIC) {
      functionName = indexOfItems[abs(item)].itemCatalogName;
    }
    else {
      functionName = dynmenuGetLabel(dynamicMenuItem);
    }

    showFunctionNameItem = item;
    if(delayInMs != 0) {
      timerStart(tidShowNop, NOPARAM, delayInMs);
    }
    if(stringWidth(functionName, &standardFont, true, true) + 1 + lineTWidth > SCREEN_WIDTH) {
      clearRegisterLine(REGISTER_T, true, false);
    }

    // Draw over SHIFT f and SHIFT g in case they were present (otherwise they will be obscured by the function name)
    getGlyphBounds(STD_SUP_f, 0, &numericFont, &fcol, &frow);
    getGlyphBounds(STD_SUP_g, 0, &numericFont, &gcol, &grow);
    lcd_fill_rect(0, Y_POSITION_OF_REGISTER_T_LINE, (fcol > gcol ? fcol : gcol), (frow > grow ? frow : grow), LCD_SET_VALUE);

    showString(functionName, &standardFont, 1, Y_POSITION_OF_REGISTER_T_LINE + 6, vmNormal, true, true);
  }



  void hideFunctionName(void) {
    uint32_t col, row;

    getStringBounds(tmpString[0] != 0 ? tmpString : indexOfItems[abs(showFunctionNameItem)].itemCatalogName, &standardFont, &col, &row);
    lcd_fill_rect(1, Y_POSITION_OF_REGISTER_T_LINE+6, col, row, LCD_SET_VALUE);
    showFunctionNameItem = 0;
    timerStop(tidShowNop);
  }



  void clearRegisterLine(calcRegister_t regist, bool clearTop, bool clearBottom) {
    if(REGISTER_X <= regist && regist <= REGISTER_T) {
      uint32_t yStart = Y_POSITION_OF_REGISTER_X_LINE - REGISTER_LINE_HEIGHT*(regist - REGISTER_X);
      uint32_t height = 32;

      if(clearTop) {
        yStart -= 4;
        height += 4;
      }

      if(clearBottom) {
        height += 4;
        if(regist == REGISTER_X) {
          height += 3;
        }
      }

      lcd_fill_rect(0, yStart, SCREEN_WIDTH, height, LCD_SET_VALUE);
    }
  }



  static void viewRegName(char *prefix, int16_t *prefixWidth) {
    if(currentViewRegister < REGISTER_X) {
      sprintf(prefix, "R%02" PRIu16 STD_SPACE_4_PER_EM "=" STD_SPACE_4_PER_EM, currentViewRegister);
    }
    else if(currentViewRegister < FIRST_LOCAL_REGISTER) {
      sprintf(prefix, "%c" STD_SPACE_4_PER_EM "=" STD_SPACE_4_PER_EM, "XYZTABCDLIJK"[currentViewRegister - REGISTER_X]);
    }
    else if(currentViewRegister <= LAST_LOCAL_REGISTER) {
      sprintf(prefix, "R.%02" PRIu16 STD_SPACE_4_PER_EM "=" STD_SPACE_4_PER_EM, (uint16_t)(currentViewRegister - FIRST_LOCAL_REGISTER));
    }
    else if(currentViewRegister >= FIRST_NAMED_VARIABLE && currentViewRegister <= LAST_NAMED_VARIABLE) {
      memcpy(prefix, allNamedVariables[currentViewRegister - FIRST_NAMED_VARIABLE].variableName + 1, allNamedVariables[currentViewRegister - FIRST_NAMED_VARIABLE].variableName[0]);
      strcpy(prefix + allNamedVariables[currentViewRegister - FIRST_NAMED_VARIABLE].variableName[0], STD_SPACE_4_PER_EM "=" STD_SPACE_4_PER_EM);
    }
    else if(currentViewRegister >= FIRST_RESERVED_VARIABLE && currentViewRegister <= LAST_RESERVED_VARIABLE) {
      memcpy(prefix, allReservedVariables[currentViewRegister - FIRST_RESERVED_VARIABLE].reservedVariableName + 1, allReservedVariables[currentViewRegister - FIRST_RESERVED_VARIABLE].reservedVariableName[0]);
      strcpy(prefix + allReservedVariables[currentViewRegister - FIRST_RESERVED_VARIABLE].reservedVariableName[0], STD_SPACE_4_PER_EM "=" STD_SPACE_4_PER_EM);
    }
    else {
      sprintf(prefix, "?" STD_SPACE_4_PER_EM "=" STD_SPACE_4_PER_EM);
    }
    *prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
  }



  static void inputRegName(char *prefix, int16_t *prefixWidth) {
    if((currentInputVariable & 0x3fff) < REGISTER_X) {
      sprintf(prefix, "R%02" PRIu16 "?", (uint16_t)(currentInputVariable & 0x3fff));
    }
    else if((currentInputVariable & 0x3fff) < FIRST_LOCAL_REGISTER) {
      sprintf(prefix, "%c?", "XYZTABCDLIJK"[(currentInputVariable & 0x3fff) - REGISTER_X]);
    }
    else if((currentInputVariable & 0x3fff) <= LAST_LOCAL_REGISTER) {
      sprintf(prefix, "R.%02" PRIu16 "?", (uint16_t)((currentInputVariable & 0x3fff) - FIRST_LOCAL_REGISTER));
    }
    else if((currentInputVariable & 0x3fff) >= FIRST_NAMED_VARIABLE && (currentInputVariable & 0x3fff) <= LAST_NAMED_VARIABLE) {
      memcpy(prefix, allNamedVariables[(currentInputVariable & 0x3fff) - FIRST_NAMED_VARIABLE].variableName + 1, allNamedVariables[(currentInputVariable & 0x3fff) - FIRST_NAMED_VARIABLE].variableName[0]);
      strcpy(prefix + allNamedVariables[(currentInputVariable & 0x3fff) - FIRST_NAMED_VARIABLE].variableName[0], "?");
    }
    else if((currentInputVariable & 0x3fff) >= FIRST_RESERVED_VARIABLE && (currentInputVariable & 0x3fff) <= LAST_RESERVED_VARIABLE) {
      memcpy(prefix, allReservedVariables[(currentInputVariable & 0x3fff) - FIRST_RESERVED_VARIABLE].reservedVariableName + 1, allReservedVariables[(currentInputVariable & 0x3fff) - FIRST_RESERVED_VARIABLE].reservedVariableName[0]);
      strcpy(prefix + allReservedVariables[(currentInputVariable & 0x3fff) - FIRST_RESERVED_VARIABLE].reservedVariableName[0], "?");
    }
    else {
      sprintf(prefix, "??");
    }
    *prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
  }



  void updateMatrixHeightCache(void) {
    int16_t prefixWidth = 0;
    char prefix[200];

    cachedDisplayStack = 4;

    if(getRegisterDataType(REGISTER_X) == dtReal34Matrix || (calcMode == cmMim && getRegisterDataType(matrixIndex) == dtReal34Matrix)) {
      real34Matrix_t matrix;

      if(temporaryInformation == TI_VIEW_REGISTER) {
        viewRegName(prefix, &prefixWidth);
      }
      if(temporaryInformation == TI_NO_INFO && currentInputVariable != INVALID_VARIABLE) {
        inputRegName(prefix, &prefixWidth);
      }
      if(calcMode == cmMim) {
        matrix = openMatrixMIMPointer.realMatrix;
      }
      else {
        linkToRealMatrixRegister(REGISTER_X, &matrix);
      }
      const uint16_t rows = matrix.header.matrixRows;
      const uint16_t cols = matrix.header.matrixColumns;
      bool           smallFont = (rows >= 5);
      int16_t        dummyVal[MATRIX_MAX_COLUMNS * (MATRIX_MAX_ROWS + 1) + 1] = {};
      const int16_t  mtxWidth = getRealMatrixColumnWidths(&matrix, prefixWidth, &numericFont, dummyVal, dummyVal + MATRIX_MAX_COLUMNS, dummyVal + (MATRIX_MAX_ROWS + 1) * MATRIX_MAX_COLUMNS, cols > MATRIX_MAX_COLUMNS ? MATRIX_MAX_COLUMNS : cols);
      if(abs(mtxWidth) > MATRIX_LINE_WIDTH) {
        smallFont = true;
      }
      if(rows == 2 && cols > 1 && !smallFont) {
        cachedDisplayStack = 3;
      }
      if(rows == 3 && cols > 1) {
        cachedDisplayStack = smallFont ? 3 : 2;
      }
      if(rows == 4 && cols > 1) {
        cachedDisplayStack = smallFont ? 2 : 1;
      }
      if(rows >= 5 && cols > 1) {
        cachedDisplayStack = 2;
      }
      if(calcMode == cmMim) {
        cachedDisplayStack -= 2;
      }
      if(cachedDisplayStack > 4) { // in case of overflow
        cachedDisplayStack = 0;
      }
    }
    else if(getRegisterDataType(REGISTER_X) == dtComplex34Matrix || (calcMode == cmMim && getRegisterDataType(matrixIndex) == dtComplex34Matrix)) {
      complex34Matrix_t matrix;
      if(temporaryInformation == TI_VIEW_REGISTER) {
        viewRegName(prefix, &prefixWidth);
      }
      if(temporaryInformation == TI_NO_INFO && currentInputVariable != INVALID_VARIABLE) {
        inputRegName(prefix, &prefixWidth);
      }
      if(calcMode == cmMim) {
        matrix = openMatrixMIMPointer.complexMatrix;
      }
      else
        linkToComplexMatrixRegister(REGISTER_X, &matrix);
      const uint16_t rows = matrix.header.matrixRows;
      const uint16_t cols = matrix.header.matrixColumns;
      bool           smallFont = (rows >= 5);
      int16_t        dummyVal[MATRIX_MAX_COLUMNS * (MATRIX_MAX_ROWS * 2 + 3) + 1] = {};
      const int16_t  mtxWidth = getComplexMatrixColumnWidths(&matrix, prefixWidth, &numericFont, dummyVal, dummyVal + MATRIX_MAX_COLUMNS, dummyVal + MATRIX_MAX_COLUMNS * 2, dummyVal + MATRIX_MAX_COLUMNS * 3, dummyVal + MATRIX_MAX_COLUMNS * (MATRIX_MAX_ROWS + 3), dummyVal + MATRIX_MAX_COLUMNS * (MATRIX_MAX_ROWS * 2 + 3), cols > MATRIX_MAX_COLUMNS ? MATRIX_MAX_COLUMNS : cols);
      if(mtxWidth > MATRIX_LINE_WIDTH) {
        smallFont = true;
      }
      if(rows == 2 && cols > 1 && !smallFont) {
        cachedDisplayStack = 3;
      }
      if(rows == 3 && cols > 1) {
        cachedDisplayStack = smallFont ? 3 : 2;
      }
      if(rows == 4 && cols > 1) {
        cachedDisplayStack = smallFont ? 2 : 1;
      }
      if(rows >= 5 && cols > 1) {
        cachedDisplayStack = 2;
      }
      if(calcMode == cmMim) {
        cachedDisplayStack -= 2;
      }
      if(cachedDisplayStack > 4) { // in case of overflow
        cachedDisplayStack = 0;
      }
    }

    if(calcMode == cmMim && matrixIndex == REGISTER_X) {
      cachedDisplayStack += 1;
    }
  }



  void refreshRegisterLine(calcRegister_t regist) {
    int16_t       w, wLastBaseNumeric, wLastBaseStandard, prefixWidth, lineWidth = 0;
    bool          prefixPre = true;
    bool          prefixPost = true;
    const uint8_t origDisplayStack = displayStack;

    char prefix[200], lastBase[4];

    if((calcMode != cmPlotStat) && (calcMode != cmGraph)) {
      clearRegisterLine(regist, true, (regist != REGISTER_Y));

      if(getRegisterDataType(REGISTER_X) == dtReal34Matrix || (calcMode == cmMim && getRegisterDataType(matrixIndex) == dtReal34Matrix)) {
        displayStack = cachedDisplayStack;
      }
      else if(getRegisterDataType(REGISTER_X) == dtComplex34Matrix || (calcMode == cmMim && getRegisterDataType(matrixIndex) == dtComplex34Matrix)) {
        displayStack = cachedDisplayStack;
      }

      if(temporaryInformation == TI_STATISTIC_LR && (getRegisterDataType(REGISTER_X) != dtReal34)) {
        if(regist == REGISTER_X) {
          if((uint16_t)((~lrSelection) & 0x01FF) == 511) {
            sprintf(tmpString, "L.R. selected to OrthoF.");
          }
          else {
            sprintf(tmpString, "L.R. selected to %03" PRIu16 ".", (uint16_t)((~lrSelection) & 0x01FF));
          }
          #if (EXTRA_INFO_ON_CALC_ERROR == 1)
            sprintf(errorMessage, "BestF is set, but will not work until REAL data points are used.");
            moreInfoOnError("In function refreshRegisterLine:", errorMessage, errorMessages[24], NULL);
          #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
          w = stringWidth(tmpString, &standardFont, true, true);
          showString(tmpString, &standardFont, SCREEN_WIDTH - w, Y_POSITION_OF_REGISTER_X_LINE - REGISTER_LINE_HEIGHT*(regist - REGISTER_X) + 6, vmNormal, true, true);
        }
      }

      else if(temporaryInformation == TI_ARE_YOU_SURE && regist == REGISTER_X) {
        showString("Are you sure?", &standardFont, 1, Y_POSITION_OF_REGISTER_X_LINE - REGISTER_LINE_HEIGHT*(regist - REGISTER_X) + 6, vmNormal, true, true);
      }

      else if(temporaryInformation == TI_WHO && regist == REGISTER_X) {
        const char *whoStr2 = systemMaker();
        if(whoStr2 != NULL) {
          showString(whoStr1, &standardFont, 1, Y_POSITION_OF_REGISTER_X_LINE - REGISTER_LINE_HEIGHT*(regist - REGISTER_X) - 3, vmNormal, true, true);
          showString(whoStr2, &standardFont, 1, Y_POSITION_OF_REGISTER_X_LINE - REGISTER_LINE_HEIGHT*(regist - REGISTER_X) + 17, vmNormal, true, true);
        } else {
          showString(whoStr1, &standardFont, 1, Y_POSITION_OF_REGISTER_X_LINE - REGISTER_LINE_HEIGHT*(regist - REGISTER_X) + 6, vmNormal, true, true);
        }
      }

      else if(temporaryInformation == TI_VERSION && regist == REGISTER_X) {
        showString(versionStr, &standardFont, 1, Y_POSITION_OF_REGISTER_X_LINE - REGISTER_LINE_HEIGHT*(regist - REGISTER_X) + 6, vmNormal, true, true);
      }

      else if(temporaryInformation == TI_FALSE && regist == TRUE_FALSE_REGISTER_LINE) {
        sprintf(tmpString, "false");
        showString(tmpString, &standardFont, 1, Y_POSITION_OF_TRUE_FALSE_LINE + 6, vmNormal, true, true);
      }

      else if(temporaryInformation == TI_TRUE && regist == TRUE_FALSE_REGISTER_LINE) {
        sprintf(tmpString, "true");
        showString(tmpString, &standardFont, 1, Y_POSITION_OF_TRUE_FALSE_LINE + 6, vmNormal, true, true);
      }

      else if(temporaryInformation == TI_RESET && regist == REGISTER_X) {
        sprintf(tmpString, "Data, programs, and definitions cleared");
        w = stringWidth(tmpString, &standardFont, true, true);
        showString(tmpString, &standardFont, SCREEN_WIDTH - w, Y_POSITION_OF_REGISTER_X_LINE - REGISTER_LINE_HEIGHT*(regist - REGISTER_X) + 6, vmNormal, true, true);
      }

      else if(temporaryInformation == TI_SAVED && regist == REGISTER_X) {
        showString("Saved", &standardFont, 1, Y_POSITION_OF_REGISTER_X_LINE - REGISTER_LINE_HEIGHT*(regist - REGISTER_X) + 6, vmNormal, true, true);
      }

      else if(temporaryInformation == TI_BACKUP_RESTORED && regist == REGISTER_X) {
        showString("Backup restored", &standardFont, 1, Y_POSITION_OF_REGISTER_X_LINE - REGISTER_LINE_HEIGHT*(regist - REGISTER_X) + 6, vmNormal, true, true);
      }

      else if(temporaryInformation == TI_UNDO_DISABLED && regist == REGISTER_X) {
        showString("Not enough memory for undo", &standardFont, 1, Y_POSITION_OF_REGISTER_X_LINE - REGISTER_LINE_HEIGHT*(regist - REGISTER_X) + 6, vmNormal, true, true);
      }

      else if(temporaryInformation == TI_SHOW_REGISTER && regist == REGISTER_T) { // L1
        w = stringWidth(tmpString, &standardFont, true, true);
        showString(tmpString, &standardFont, SCREEN_WIDTH - w, Y_POSITION_OF_REGISTER_T_LINE + 21*0, vmNormal, true, true);
      }

      else if(temporaryInformation == TI_SHOW_REGISTER && regist == REGISTER_Z && tmpString[300] != 0) { // L2 & L3
        w = stringWidth(tmpString + 300, &standardFont, true, true);
        showString(tmpString + 300, &standardFont, SCREEN_WIDTH - w, Y_POSITION_OF_REGISTER_T_LINE + 21*1, vmNormal, true, true);

        if(tmpString[600]) {
          w = stringWidth(tmpString + 600, &standardFont, true, true);
          showString(tmpString + 600, &standardFont, SCREEN_WIDTH - w, Y_POSITION_OF_REGISTER_T_LINE + 21*2, vmNormal, true, true);
        }
      }

      else if(temporaryInformation == TI_SHOW_REGISTER && regist == REGISTER_Y && tmpString[900] != 0) { // L4 & L5
        w = stringWidth(tmpString + 900, &standardFont, true, true);
        showString(tmpString + 900, &standardFont, SCREEN_WIDTH - w, Y_POSITION_OF_REGISTER_T_LINE + 21*3, vmNormal, true, true);

        if(tmpString[1200]) {
          w = stringWidth(tmpString + 1200, &standardFont, true, true);
          showString(tmpString + 1200, &standardFont, SCREEN_WIDTH - w, Y_POSITION_OF_REGISTER_T_LINE + 21*4, vmNormal, true, true);
        }
      }

      else if(temporaryInformation == TI_SHOW_REGISTER && regist == REGISTER_X && tmpString[1500] != 0) { // L6 & L7
        w = stringWidth(tmpString + 1500, &standardFont, true, true);
        showString(tmpString + 1500, &standardFont, SCREEN_WIDTH - w, Y_POSITION_OF_REGISTER_T_LINE + 21*5, vmNormal, true, true);

        if(tmpString[1800]) {
          w = stringWidth(tmpString + 1800, &standardFont, true, true);
          showString(tmpString + 1800, &standardFont, SCREEN_WIDTH - w, Y_POSITION_OF_REGISTER_T_LINE + 21*6, vmNormal, true, true);
        }
      }

      else if(regist < REGISTER_X + min(displayStack, origDisplayStack) || (lastErrorCode != 0 && regist == errorMessageRegisterLine) || (temporaryInformation == TI_VIEW_REGISTER && regist == REGISTER_T)) {
        prefixWidth = 0;
        const int16_t baseY = Y_POSITION_OF_REGISTER_X_LINE - REGISTER_LINE_HEIGHT*(regist - REGISTER_X + ((temporaryInformation == TI_VIEW_REGISTER && regist == REGISTER_T) ? 0 : (getRegisterDataType(REGISTER_X) == dtReal34Matrix || getRegisterDataType(REGISTER_X) == dtComplex34Matrix) ? 4 - displayStack : 0));
        calcRegister_t origRegist = regist;
        if(temporaryInformation == TI_VIEW_REGISTER && regist == REGISTER_T) {
          if(currentViewRegister >= FIRST_RESERVED_VARIABLE && currentViewRegister < LAST_RESERVED_VARIABLE && allReservedVariables[currentViewRegister - FIRST_RESERVED_VARIABLE].header.pointerToRegisterData == WP43_NULL) {
            copySourceRegisterToDestRegister(currentViewRegister, TEMP_REGISTER_1);
            regist = TEMP_REGISTER_1;
          }
          else {
            regist = currentViewRegister;
          }
        }

        if(regist == REGISTER_X && currentInputVariable != INVALID_VARIABLE) {
          inputRegName(prefix, &prefixWidth);
        }

        if(lastErrorCode != 0 && regist == errorMessageRegisterLine) {
          if(stringWidth(errorMessages[lastErrorCode], &standardFont, true, true) <= SCREEN_WIDTH - 1) {
            showString(errorMessages[lastErrorCode], &standardFont, 1, Y_POSITION_OF_REGISTER_X_LINE - REGISTER_LINE_HEIGHT*(regist - REGISTER_X) + 6, vmNormal, true, true);
          }
          else {
            #if (EXTRA_INFO_ON_CALC_ERROR == 1)
              sprintf(errorMessage, "Error message %" PRIu8 " is too wide!", (uint8_t)lastErrorCode);
              moreInfoOnError("In function refreshRegisterLine:", errorMessage, errorMessages[lastErrorCode], NULL);
            #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
            sprintf(tmpString, "Error message %" PRIu8 " is too wide!", (uint8_t)lastErrorCode);
            w = stringWidth(tmpString, &standardFont, true, true);
            showString(tmpString, &standardFont, SCREEN_WIDTH - w, Y_POSITION_OF_REGISTER_X_LINE - REGISTER_LINE_HEIGHT*(regist - REGISTER_X) + 6, vmNormal, true, true);
          }
        }

        else if(regist == NIM_REGISTER_LINE && calcMode == cmNim) {
          if(lastIntegerBase != 0) {
            lastBase[0] = '#';
            if(lastIntegerBase > 9) {
              lastBase[1] = '1';
              lastBase[2] = '0' + (lastIntegerBase - 10);
              lastBase[3] = 0;
            }
            else {
              lastBase[1] = '0' + lastIntegerBase;
              lastBase[2] = 0;
            }
            wLastBaseNumeric  = stringWidth(lastBase, &numericFont,  true, true);
            wLastBaseStandard = stringWidth(lastBase, &standardFont, true, true);
          }
          else {
            wLastBaseNumeric  = 0;
            wLastBaseStandard = 0;
          }

          displayNim(nimBufferDisplay, lastBase, wLastBaseNumeric, wLastBaseStandard);
        }

        else if(regist == AIM_REGISTER_LINE && calcMode == cmAim && !tamIsActive()) {
          if(stringWidth(aimBuffer, &standardFont, true, true) < SCREEN_WIDTH - 8) { // 8 is the standard font cursor width
            uint32_t xCursor = showString(aimBuffer, &standardFont, 1, Y_POSITION_OF_NIM_LINE + 6, vmNormal, true, true);
            uint32_t yCursor = Y_POSITION_OF_NIM_LINE + 6;
            cursorShow(true, xCursor, yCursor);
          }
          else {
            w = stringByteLength(aimBuffer) + 1;
            xcopy(tmpString,        aimBuffer, w);
            xcopy(tmpString + 1500, aimBuffer, w);
            while(stringWidth(tmpString, &standardFont, true, true) >= SCREEN_WIDTH - 1) {
              w = stringLastGlyph(tmpString);
              tmpString[w] = 0;
            }

            if(stringWidth(tmpString + 1500 + w, &standardFont, true, true) >= SCREEN_WIDTH - 8) { // 8 is the standard font cursor width
              btnClicked(kcBackspace);
            }
            else {
              showString(tmpString, &standardFont, 1, Y_POSITION_OF_NIM_LINE - 3, vmNormal, true, true);

              uint32_t xCursor = showString(tmpString + 1500 + w, &standardFont, 1, Y_POSITION_OF_NIM_LINE + 18, vmNormal, true, true);
              uint32_t yCursor = Y_POSITION_OF_NIM_LINE + 18;
              cursorShow(true, xCursor, yCursor);
            }
          }
        }

        else if(temporaryInformation == TI_NO_INFO
                && getSystemFlag(FLAG_FRACT)
                    && (    getRegisterDataType(regist) == dtReal34
                         && (
                                (   real34CompareAbsGreaterThan(REGISTER_REAL34_DATA(regist), const34_1e_4)
                                 && real34CompareAbsLessThan(REGISTER_REAL34_DATA(regist), const34_1e6)
                                )
                             || real34IsZero(REGISTER_REAL34_DATA(regist))
                            )
                       )
               ) {
          fractionToDisplayString(regist, tmpString);

          w = stringWidth(tmpString, &numericFont, false, true);
          lineWidth = w;
          if(w <= SCREEN_WIDTH) {
            showString(tmpString, &numericFont, SCREEN_WIDTH - w, baseY, vmNormal, false, true);
          }
          else {
            w = stringWidth(tmpString, &standardFont, false, true);
            lineWidth = w;
            if(w > SCREEN_WIDTH) {
              #if (EXTRA_INFO_ON_CALC_ERROR == 1)
                moreInfoOnError("In function refreshRegisterLine:", "Fraction representation too wide!", tmpString, NULL);
              #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
              strcpy(tmpString, "Fraction representation too wide!");
              w = stringWidth(tmpString, &standardFont, false, true);
              lineWidth = w;
            }
            showString(tmpString, &standardFont, SCREEN_WIDTH - w, baseY, vmNormal, false, true);
          }
        }

        else if(getRegisterDataType(regist) == dtReal34) {
          if(temporaryInformation == TI_THETA_RADIUS) {
            if(regist == REGISTER_Y) {
              strcpy(prefix, "r =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
            else if(regist == REGISTER_X) {
              strcpy(prefix, STD_theta " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          if(temporaryInformation == TI_RADIUS_THETA) {
            if(regist == REGISTER_X) {
              strcpy(prefix, "r =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
            else if(regist == REGISTER_Y) {
              strcpy(prefix, STD_theta " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_X_Y) {
            if(regist == REGISTER_X) {
              strcpy(prefix, "x =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
            else if(regist == REGISTER_Y) {
              strcpy(prefix, "y =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_RE_IM) {
            if(regist == REGISTER_X) {
              strcpy(prefix, "Im =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
            else if(regist == REGISTER_Y) {
              strcpy(prefix, "Re =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_SUMX_SUMY) {
            if(regist == REGISTER_X) {
              strcpy(prefix, STD_SIGMA "x =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
            else if(regist == REGISTER_Y) {
              strcpy(prefix, STD_SIGMA "y =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_XMIN_YMIN) {
            if(regist == REGISTER_X) {
              strcpy(prefix, "x" STD_SUB_m STD_SUB_i STD_SUB_n " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
            else if(regist == REGISTER_Y) {
              strcpy(prefix, "y" STD_SUB_m STD_SUB_i STD_SUB_n " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_XMAX_YMAX) {
            if(regist == REGISTER_X) {
              strcpy(prefix, "x" STD_SUB_m STD_SUB_a STD_SUB_x " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
            else if(regist == REGISTER_Y) {
              strcpy(prefix, "y" STD_SUB_m STD_SUB_a STD_SUB_x " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_SA) {
            if(regist == REGISTER_X) {
              strcpy(prefix, "s(a" STD_SUB_0 ") =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
            else if(regist == REGISTER_Y) {
              strcpy(prefix, "s(a" STD_SUB_1 ") =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_MEANX_MEANY) {
            if(regist == REGISTER_X) {
              strcpy(prefix, STD_x_BAR " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
            else if(regist == REGISTER_Y) {
              strcpy(prefix, STD_y_BAR " =");
               prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
           }

          else if(temporaryInformation == TI_SAMPLSTDDEV) {
            if(regist == REGISTER_X) {
              strcpy(prefix, "s" STD_SUB_x " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
            else if(regist == REGISTER_Y) {
              strcpy(prefix, "s" STD_SUB_y " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_POPLSTDDEV) {
            if(regist == REGISTER_X) {
              strcpy(prefix, STD_sigma STD_SUB_x " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
            else if(regist == REGISTER_Y) {
              strcpy(prefix, STD_sigma STD_SUB_y " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_STDERR) {
            if(regist == REGISTER_X) {
              strcpy(prefix, "s" STD_SUB_m STD_SUB_x " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
            else if(regist == REGISTER_Y) {
              strcpy(prefix, "s" STD_SUB_m STD_SUB_y " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_GEOMMEANX_GEOMMEANY) {
            if(regist == REGISTER_X) {
              strcpy(prefix, STD_x_BAR STD_SUB_G " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
            else if(regist == REGISTER_Y) {
              strcpy(prefix, STD_y_BAR STD_SUB_G " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_GEOMSAMPLSTDDEV) {
            if(regist == REGISTER_X) {
              strcpy(prefix, STD_epsilon STD_SUB_x " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
            else if(regist == REGISTER_Y) {
              strcpy(prefix, STD_epsilon STD_SUB_y " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_GEOMPOPLSTDDEV) {
            if(regist == REGISTER_X) {
              strcpy(prefix, STD_epsilon STD_SUB_m STD_SUB_x " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
            else if(regist == REGISTER_Y) {
              strcpy(prefix, STD_epsilon STD_SUB_m STD_SUB_y " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_GEOMSTDERR) {
            if(regist == REGISTER_X) {
              strcpy(prefix, STD_epsilon STD_SUB_p STD_SUB_x " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
            else if(regist == REGISTER_Y) {
              strcpy(prefix, STD_epsilon STD_SUB_p STD_SUB_y " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_WEIGHTEDMEANX) {
            if(regist == REGISTER_X) {
              strcpy(prefix, STD_x_BAR STD_SUB_w " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_WEIGHTEDSAMPLSTDDEV) {
            if(regist == REGISTER_X) {
              strcpy(prefix, "s" STD_SUB_w " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_WEIGHTEDPOPLSTDDEV) {
            if(regist == REGISTER_X) {
              strcpy(prefix, STD_sigma STD_SUB_w " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_WEIGHTEDSTDERR) {
            if(regist == REGISTER_X) {
              strcpy(prefix, "s" STD_SUB_m STD_SUB_w " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_STATISTIC_HISTO) {
            if(regist == REGISTER_X) {
              strcpy(prefix,STD_UP_ARROW "BIN" " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            } else
            if(regist == REGISTER_Y) {
              strcpy(prefix,STD_DOWN_ARROW "BIN" " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
            else if(regist == REGISTER_Z) {
              strcpy(prefix,"nBINS" " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          //L.R. Display
          else if(temporaryInformation == TI_LR && lrChosen != 0) {
            #define LRWidth 140
            bool prefixPre = false;
            bool prefixPost = false;

            if(lrChosen == CF_CAUCHY_FITTING || lrChosen == CF_GAUSS_FITTING || lrChosen == CF_PARABOLIC_FITTING){
              if(regist == REGISTER_X) {
                strcpy(prefix,getCurveFitModeFormula(lrChosen));
                while(stringWidth(prefix, &standardFont, prefixPre, prefixPost) + 1 < LRWidth) {
                  strcat(prefix,STD_SPACE_6_PER_EM);
                }
                strcat(prefix,"a" STD_SUB_0 " =");
                prefixWidth = stringWidth(prefix, &standardFont, prefixPre, prefixPost) + 1;
              }
              else if(regist == REGISTER_Y) {
                strcpy(prefix,"y = ");
                while(stringWidth(prefix, &standardFont, prefixPre, prefixPost) + 1 < LRWidth) {
                  strcat(prefix,STD_SPACE_6_PER_EM);
                }
                strcat(prefix, "a" STD_SUB_1 " =");
                prefixWidth = stringWidth(prefix, &standardFont, prefixPre, prefixPost) + 1;
              }
              else if(regist == REGISTER_Z) {
                strcpy(prefix, eatSpacesEnd(getCurveFitModeName(lrChosen)));
                if(lrCountOnes(lrSelection)>1) {
                  strcat(prefix,lrChosen == 0 ? "" : STD_SUP_ASTERISK);
                }
                while(stringWidth(prefix, &standardFont, prefixPre, prefixPost) + 1 < LRWidth) {
                  strcat(prefix,STD_SPACE_6_PER_EM);
                }
                strcat(prefix, "a" STD_SUB_2 " =");
                prefixWidth = stringWidth(prefix, &standardFont, prefixPre, prefixPost) + 1;
              }
            }
            else {
              if(regist == REGISTER_X) {
                strcpy(prefix,"y = ");
                strcat(prefix,getCurveFitModeFormula(lrChosen));
                while(stringWidth(prefix, &standardFont, prefixPre, prefixPost) + 1 < LRWidth) {
                  strcat(prefix,STD_SPACE_6_PER_EM);
                }
                strcat(prefix,"a" STD_SUB_0 " =");
                prefixWidth = stringWidth(prefix, &standardFont, prefixPre, prefixPost) + 1;
              }
              else if(regist == REGISTER_Y) {
                strcpy(prefix, eatSpacesEnd(getCurveFitModeName(lrChosen)));
                if(lrCountOnes(lrSelection)>1) {
                  strcat(prefix,lrChosen == 0 ? "" : STD_SUP_ASTERISK);
                }
                while(stringWidth(prefix, &standardFont, prefixPre, prefixPost) + 1 < LRWidth) {
                  strcat(prefix,STD_SPACE_6_PER_EM);
                }
                strcat(prefix, "a" STD_SUB_1 " =");
                prefixWidth = stringWidth(prefix, &standardFont, prefixPre, prefixPost) + 1;
              }
            }
          }

/*
          else if(temporaryInformation == TI_SXY) {
            if(regist == REGISTER_X) {
              strcpy(prefix, "s" STD_SUB_x STD_SUB_y " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_COV) {
            if(regist == REGISTER_X) {
              strcpy(prefix, "s" STD_SUB_m STD_SUB_w " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }
*/
          else if(temporaryInformation == TI_CALCY) {
            if(regist == REGISTER_X) {
              prefix[0]=0;
              if(lrChosen != 0) {
                strcpy(prefix,eatSpacesEnd(getCurveFitModeName(lrChosen)));
                if(lrCountOnes(lrSelection)>1) {
                  strcat(prefix,STD_SUP_ASTERISK);
                }
                strcat(prefix, STD_SPACE_FIGURE);
              }
              strcat(prefix, STD_y_CIRC " =");
              prefixWidth = stringWidth(prefix, &standardFont, false,false) + 1;
            }
          }

          else if(temporaryInformation == TI_CALCX) {
            if(regist == REGISTER_X) {
              prefix[0]=0;
              if(lrChosen != 0) {
                strcpy(prefix,eatSpacesEnd(getCurveFitModeName(lrChosen)));
                if(lrCountOnes(lrSelection)>1) {
                  strcat(prefix,STD_SUP_ASTERISK);
                }
                strcat(prefix, STD_SPACE_FIGURE);
              }
              strcat(prefix, STD_x_CIRC " =");
              prefixWidth = stringWidth(prefix, &standardFont, false,false) + 1;
            }
          }

          else if(temporaryInformation == TI_CALCX2) {
            if(regist == REGISTER_X) {
              prefix[0]=0;
              if(lrChosen != 0) {
                strcpy(prefix,eatSpacesEnd(getCurveFitModeName(lrChosen)));
                if(lrCountOnes(lrSelection)>1) {
                  strcat(prefix,STD_SUP_ASTERISK);
                }
                strcat(prefix, STD_SPACE_FIGURE);
              }
              strcat(prefix, STD_x_CIRC STD_SUB_1 " =" );
              prefixWidth = stringWidth(prefix, &standardFont, false,false) + 1;
            }
            else {
              if(regist == REGISTER_Y) {
                prefix[0]=0;
                if(lrChosen != 0) {
                  strcpy(prefix,eatSpacesEnd(getCurveFitModeName(lrChosen)));
                  if(lrCountOnes(lrSelection)>1) {
                    strcat(prefix,STD_SUP_ASTERISK);
                  }
                  strcat(prefix, STD_SPACE_FIGURE);
                }
                strcat(prefix, STD_x_CIRC STD_SUB_2 " =");
                prefixWidth = stringWidth(prefix, &standardFont, false,false) + 1;
              }
            }
          }

          else if(temporaryInformation == TI_CORR) {
            if(regist == REGISTER_X) {
              prefix[0]=0;
              if(lrChosen != 0) {
                strcpy(prefix,eatSpacesEnd(getCurveFitModeName(lrChosen)));
                if(lrCountOnes(lrSelection)>1) {
                  strcat(prefix,STD_SUP_ASTERISK);
                }
                strcat(prefix, STD_SPACE_FIGURE);
              }
              strcat(prefix, "r =");
              prefixWidth = stringWidth(prefix, &standardFont, false,false) + 1;
            }
          }

          else if(temporaryInformation == TI_SMI) {
            if(regist == REGISTER_X) {
              strcpy(prefix, "s" STD_SUB_m STD_SUB_i " =");
              prefixWidth = stringWidth(prefix, &standardFont, false,false) + 1;
            }
          }
          //L.R. Display



          else if(temporaryInformation == TI_HARMMEANX_HARMMEANY) {
            if(regist == REGISTER_X) {
              strcpy(prefix, STD_x_BAR STD_SUB_H " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
            else if(regist == REGISTER_Y) {
              strcpy(prefix, STD_y_BAR STD_SUB_H " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_RMSMEANX_RMSMEANY) {
            if(regist == REGISTER_X) {
              strcpy(prefix, STD_x_BAR STD_SUB_R STD_SUB_M STD_SUB_S " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
            else if(regist == REGISTER_Y) {
              strcpy(prefix, STD_y_BAR STD_SUB_R STD_SUB_M STD_SUB_S " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_STATISTIC_SUMS) {
            w = realToInt32(SIGMA_N);
            if(regist == REGISTER_Y) {
              if(w == 1) {
                sprintf(prefix, "%03" PRId16 " data point", w);
              }
              else {
                sprintf(prefix, "%03" PRId16 " data points", w);
              }
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
              lcd_fill_rect(0, Y_POSITION_OF_REGISTER_Y_LINE - 2, SCREEN_WIDTH, 1, LCD_EMPTY_VALUE);
            }
          }

          else if(temporaryInformation == TI_STATISTIC_LR) {
             if(regist == REGISTER_X) {
               if( (uint16_t)((~lrSelection) & 0x01FF) == 511) {
                 sprintf(prefix, "L.R. selected to OrthoF");
               }
               else {
                 sprintf(prefix, "L.R. selected to %03" PRIu16, (uint16_t)((~lrSelection) & 0x01FF));
               }
               prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
               //lcd_fill_rect(0, Y_POSITION_OF_REGISTER_Y_LINE - 2, SCREEN_WIDTH, 1, LCD_EMPTY_VALUE);
             }
           }

          else if(temporaryInformation == TI_SOLVER_VARIABLE) {
            if(regist == REGISTER_X) {
              if(currentSolverVariable >= FIRST_RESERVED_VARIABLE) {
                memcpy(prefix, allReservedVariables[currentSolverVariable - FIRST_RESERVED_VARIABLE].reservedVariableName + 1, allReservedVariables[currentSolverVariable - FIRST_RESERVED_VARIABLE].reservedVariableName[0]);
                strcpy(prefix + allReservedVariables[currentSolverVariable - FIRST_RESERVED_VARIABLE].reservedVariableName[0], " =");
              }
              else {
                memcpy(prefix, allNamedVariables[currentSolverVariable - FIRST_NAMED_VARIABLE].variableName + 1, allNamedVariables[currentSolverVariable - FIRST_NAMED_VARIABLE].variableName[0]);
                strcpy(prefix + allNamedVariables[currentSolverVariable - FIRST_NAMED_VARIABLE].variableName[0], " =");
              }
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_ACC) {
            if(regist == REGISTER_X) {
              sprintf(prefix, "ACC =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_ULIM) {
            if(regist == REGISTER_X) {
              sprintf(prefix, STD_UP_ARROW "Lim =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_LLIM) {
            if(regist == REGISTER_X) {
              sprintf(prefix, STD_DOWN_ARROW "Lim =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_INTEGRAL) {
            if(regist == REGISTER_X) {
              sprintf(prefix, STD_INTEGRAL STD_ALMOST_EQUAL);
              prefixWidth = stringWidth(prefix, &numericFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_1ST_DERIVATIVE) {
            if(regist == REGISTER_X) {
              sprintf(prefix, "f' =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_2ND_DERIVATIVE) {
            if(regist == REGISTER_X) {
              sprintf(prefix, "f\" =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T) {
            viewRegName(prefix, &prefixWidth);
          }
          real34ToDisplayString(REGISTER_REAL34_DATA(regist), getRegisterAngularMode(regist), tmpString, &numericFont, SCREEN_WIDTH - prefixWidth, NUMBER_OF_DISPLAY_DIGITS, true, STD_SPACE_PUNCTUATION, true);

          w = stringWidth(tmpString, &numericFont, false, true);
          lineWidth = w;
          if(prefixWidth > 0) {
            if(temporaryInformation == TI_INTEGRAL) {
              showString(prefix, &numericFont, 1, baseY, vmNormal, prefixPre, prefixPost);
            }
            else {
              showString(prefix, &standardFont, 1, baseY + TEMPORARY_INFO_OFFSET, vmNormal, prefixPre, prefixPost);
            }
          }
          showString(tmpString, &numericFont, (temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T) ? prefixWidth : SCREEN_WIDTH - w, baseY, vmNormal, false, true);
        }

        else if(getRegisterDataType(regist) == dtComplex34) {
          if(temporaryInformation == TI_SOLVER_VARIABLE) {
            if(regist == REGISTER_X) {
              memcpy(prefix, allNamedVariables[currentSolverVariable - FIRST_NAMED_VARIABLE].variableName + 1, allNamedVariables[currentSolverVariable - FIRST_NAMED_VARIABLE].variableName[0]);
              strcpy(prefix + allNamedVariables[currentSolverVariable - FIRST_NAMED_VARIABLE].variableName[0], " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          else if(temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T) {
            viewRegName(prefix, &prefixWidth);
          }
          complex34ToDisplayString(REGISTER_COMPLEX34_DATA(regist), tmpString, &numericFont, SCREEN_WIDTH - prefixWidth, NUMBER_OF_DISPLAY_DIGITS, true, STD_SPACE_PUNCTUATION, true);

          w = stringWidth(tmpString, &numericFont, false, true);
          lineWidth = w;
          if(prefixWidth > 0) {
            showString(prefix, &standardFont, 1, baseY + TEMPORARY_INFO_OFFSET, vmNormal, prefixPre, prefixPost);
          }
          showString(tmpString, &numericFont, (temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T) ? prefixWidth : SCREEN_WIDTH - w, baseY, vmNormal, false, true);
        }

        else if(getRegisterDataType(regist) == dtString) {
          if(temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T) {
            viewRegName(prefix, &prefixWidth);
          }
          if(prefixWidth > 0) {
            showString(prefix, &standardFont, 1, baseY + TEMPORARY_INFO_OFFSET, vmNormal, prefixPre, prefixPost);
          }

          w = stringWidth(REGISTER_STRING_DATA(regist), &standardFont, false, true);

          if(w >= SCREEN_WIDTH - prefixWidth) {
            if(regist == REGISTER_X || (temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T)) {
              xcopy(tmpString, REGISTER_STRING_DATA(regist), stringByteLength(REGISTER_STRING_DATA(regist)) + 1);
              do {
                tmpString[stringLastGlyph(tmpString)] = 0;
                w = stringWidth(tmpString, &standardFont, false, true);
              } while(w >= SCREEN_WIDTH - prefixWidth);
              if(temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T) {
                showString(tmpString, &standardFont, prefixWidth     , Y_POSITION_OF_REGISTER_T_LINE - 3, vmNormal, false, true);
              }
              else {
                showString(tmpString, &standardFont, SCREEN_WIDTH - w, Y_POSITION_OF_REGISTER_X_LINE - 3, vmNormal, false, true);
              }

              w = stringByteLength(tmpString);
              xcopy(tmpString, REGISTER_STRING_DATA(regist) + w, stringByteLength(REGISTER_STRING_DATA(regist) + w) + 1);
              w = stringWidth(tmpString, &standardFont, false, true);
              if(w >= SCREEN_WIDTH - prefixWidth) {
                do {
                  tmpString[stringLastGlyph(tmpString)] = 0;
                  w = stringWidth(tmpString, &standardFont, false, true);
                } while(w >= SCREEN_WIDTH - prefixWidth - 14); // 14 is the width of STD_ELLIPSIS
                xcopy(tmpString + stringByteLength(tmpString), STD_ELLIPSIS, 3);
                w += 14;
              }
              if(temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T) {
                showString(tmpString, &standardFont, prefixWidth     , Y_POSITION_OF_REGISTER_T_LINE + 18, vmNormal, false, true);
              }
              else {
                showString(tmpString, &standardFont, SCREEN_WIDTH - w, Y_POSITION_OF_REGISTER_X_LINE + 18, vmNormal, false, true);
              }
            }
            else {
              xcopy(tmpString, REGISTER_STRING_DATA(regist), stringByteLength(REGISTER_STRING_DATA(regist)) + 1);
              do {
                tmpString[stringLastGlyph(tmpString)] = 0;
                w = stringWidth(tmpString, &standardFont, false, true);
              } while(w >= SCREEN_WIDTH - prefixWidth - 14); // 14 is the width of STD_ELLIPSIS
              xcopy(tmpString + stringByteLength(tmpString), STD_ELLIPSIS, 3);
              w += 14;
              lineWidth = w;
              showString(tmpString, &standardFont, SCREEN_WIDTH - w, baseY + 6, vmNormal, false, true);
            }
          }
          else {
            lineWidth = w;
            if(temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T) {
              showString(REGISTER_STRING_DATA(regist), &standardFont, prefixWidth     , baseY + TEMPORARY_INFO_OFFSET, vmNormal, false, true);
            }
            else {
              showString(REGISTER_STRING_DATA(regist), &standardFont, SCREEN_WIDTH - w, baseY + 6                    , vmNormal, false, true);
            }
          }
        }

        else if(getRegisterDataType(regist) == dtShortInteger) {
          if(temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T) {
            viewRegName(prefix, &prefixWidth);
          }
          shortIntegerToDisplayString(regist, tmpString, true);
          if(prefixWidth > 0) {
            showString(prefix, &standardFont, 1, baseY + TEMPORARY_INFO_OFFSET, vmNormal, prefixPre, prefixPost);
          }
          w = stringWidth(tmpString, fontForShortInteger, false, true);
          showString(tmpString, fontForShortInteger, (temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T) ? min(prefixWidth, SCREEN_WIDTH - w) : SCREEN_WIDTH - w, baseY + (fontForShortInteger == &standardFont ? 6 : 0), vmNormal, false, true);
        }

        else if(getRegisterDataType(regist) == dtLongInteger) {
          if(temporaryInformation == TI_SOLVER_VARIABLE) {
            if(regist == REGISTER_X) {
              memcpy(prefix, allNamedVariables[currentSolverVariable - FIRST_NAMED_VARIABLE].variableName + 1, allNamedVariables[currentSolverVariable - FIRST_NAMED_VARIABLE].variableName[0]);
              strcpy(prefix + allNamedVariables[currentSolverVariable - FIRST_NAMED_VARIABLE].variableName[0], " =");
              prefixWidth = stringWidth(prefix, &standardFont, true, true) + 1;
            }
          }

          if(temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T) {
            viewRegName(prefix, &prefixWidth);
          }
          longIntegerRegisterToDisplayString(regist, tmpString, TMP_STR_LENGTH, SCREEN_WIDTH - prefixWidth, 50, STD_SPACE_PUNCTUATION);

          if(temporaryInformation == TI_DAY_OF_WEEK) {
            if(regist == REGISTER_X) {
              if(strcmp(tmpString, "1") == 0) {
                strcpy(prefix, nameOfWday_en[1]);
              }
              else if(strcmp(tmpString, "2") == 0) {
                strcpy(prefix, nameOfWday_en[2]);
              }
              else if(strcmp(tmpString, "3") == 0) {
                strcpy(prefix, nameOfWday_en[3]);
              }
              else if(strcmp(tmpString, "4") == 0) {
                strcpy(prefix, nameOfWday_en[4]);
              }
              else if(strcmp(tmpString, "5") == 0) {
                strcpy(prefix, nameOfWday_en[5]);
              }
              else if(strcmp(tmpString, "6") == 0) {
                strcpy(prefix, nameOfWday_en[6]);
              }
              else if(strcmp(tmpString, "7") == 0) {
                strcpy(prefix, nameOfWday_en[7]);
              }
              else {
                strcpy(prefix, nameOfWday_en[0]);
              }
              showString(prefix, &standardFont, 1, baseY + TEMPORARY_INFO_OFFSET, vmNormal, true, true);
            }
          }

          w = stringWidth(tmpString, &numericFont, false, true);
          lineWidth = w;
          if(prefixWidth > 0) {
            showString(prefix, &standardFont, 1, baseY + TEMPORARY_INFO_OFFSET, vmNormal, prefixPre, prefixPost);
          }

          if(w <= SCREEN_WIDTH) {
            showString(tmpString, &numericFont, (temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T) ? prefixWidth : SCREEN_WIDTH - w, baseY, vmNormal, false, true);
          }
          else {
            w = stringWidth(tmpString, &standardFont, false, true);
            if(w > SCREEN_WIDTH) {
              #if (EXTRA_INFO_ON_CALC_ERROR == 1)
                moreInfoOnError("In function refreshRegisterLine:", "Long integer representation too wide!", tmpString, NULL);
              #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
              strcpy(tmpString, "Long integer representation too wide!");
            }
            w = stringWidth(tmpString, &standardFont, false, true);
            lineWidth = w;
            showString(tmpString, &standardFont, (temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T) ? prefixWidth : SCREEN_WIDTH - w, baseY + 6, vmNormal, false, true);
          }
        }

        else if(getRegisterDataType(regist) == dtTime) {
          if(temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T) {
            viewRegName(prefix, &prefixWidth);
          }
          timeToDisplayString(regist, tmpString, false);
          w = stringWidth(tmpString, &numericFont, false, true);
          if(prefixWidth > 0) {
            showString(prefix, &standardFont, 1, baseY + TEMPORARY_INFO_OFFSET, vmNormal, prefixPre, prefixPost);
          }
          showString(tmpString, &numericFont, (temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T) ? prefixWidth : SCREEN_WIDTH - w, baseY, vmNormal, false, true);
        }

        else if(getRegisterDataType(regist) == dtDate) {
          if(temporaryInformation == TI_DAY_OF_WEEK) {
            if(regist == REGISTER_X) {
              strcpy(prefix, nameOfWday_en[getDayOfWeek(regist)]);
              showString(prefix, &standardFont, 1, baseY + TEMPORARY_INFO_OFFSET, vmNormal, true, true);
            }
          }
          else if(temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T) {
            viewRegName(prefix, &prefixWidth);
          }

          dateToDisplayString(regist, tmpString);
          w = stringWidth(tmpString, &numericFont, false, true);
          if(prefixWidth > 0) {
            showString(prefix, &standardFont, 1, baseY + TEMPORARY_INFO_OFFSET, vmNormal, prefixPre, prefixPost);
          }
          showString(tmpString, &numericFont, (temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T) ? prefixWidth : SCREEN_WIDTH - w, baseY, vmNormal, false, true);
        }

        else if(getRegisterDataType(regist) == dtConfig) {
          if(temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T) {
            viewRegName(prefix, &prefixWidth);
          }
          xcopy(tmpString, "Configuration data", 19);
          w = stringWidth(tmpString, &numericFont, false, true);
          lineWidth = w;
          if(prefixWidth > 0) {
            showString(prefix, &standardFont, 1, baseY + TEMPORARY_INFO_OFFSET, vmNormal, prefixPre, prefixPost);
          }
          showString(tmpString, &numericFont, (temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T) ? prefixWidth : SCREEN_WIDTH - w, baseY, vmNormal, false, true);
        }

        else if(getRegisterDataType(regist) == dtReal34Matrix) {
          if((origRegist == REGISTER_X && calcMode != cmMim) || (temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T)) {
            real34Matrix_t matrix;
            prefixWidth = 0; prefix[0] = 0;
            linkToRealMatrixRegister(regist, &matrix);
            if(temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T) {
              viewRegName(prefix, &prefixWidth);
            }
            showRealMatrix(&matrix, prefixWidth);
            if(lastErrorCode != 0) {
              refreshRegisterLine(errorMessageRegisterLine);
            }
            if(temporaryInformation == TI_TRUE || temporaryInformation == TI_FALSE) {
              refreshRegisterLine(TRUE_FALSE_REGISTER_LINE);
            }
            if(prefixWidth > 0) {
              showString(prefix, &standardFont, 1, baseY + TEMPORARY_INFO_OFFSET, vmNormal, prefixPre, prefixPost);
            }
          }
          else {
            real34MatrixToDisplayString(regist, tmpString);
            w = stringWidth(tmpString, &numericFont, false, true);
            lineWidth = w;
            showString(tmpString, &numericFont, SCREEN_WIDTH - w - 2, baseY, vmNormal, false, true);
          }

          if(temporaryInformation == TI_INACCURATE && regist == REGISTER_X) {
            showString("This result may be inaccurate", &standardFont, 1, Y_POSITION_OF_ERR_LINE, vmNormal, true, true);
          }
        }

        else if(getRegisterDataType(regist) == dtComplex34Matrix) {
          if((origRegist == REGISTER_X && calcMode != cmMim) || (temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T)) {
            complex34Matrix_t matrix;
            linkToComplexMatrixRegister(regist, &matrix);
            if(temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_T) {
              viewRegName(prefix, &prefixWidth);
            }
            showComplexMatrix(&matrix, prefixWidth);
            if(lastErrorCode != 0) {
              refreshRegisterLine(errorMessageRegisterLine);
            }
            if(temporaryInformation == TI_TRUE || temporaryInformation == TI_FALSE) {
              refreshRegisterLine(TRUE_FALSE_REGISTER_LINE);
            }
            if(prefixWidth > 0) {
              showString(prefix, &standardFont, 1, baseY + TEMPORARY_INFO_OFFSET, vmNormal, prefixPre, prefixPost);
            }
          }
          else {
            complex34MatrixToDisplayString(regist, tmpString);
            w = stringWidth(tmpString, &numericFont, false, true);
            lineWidth = w;
            showString(tmpString, &numericFont, SCREEN_WIDTH - w - 2, baseY, vmNormal, false, true);
          }

          if(temporaryInformation == TI_INACCURATE && regist == REGISTER_X) {
            showString("This result may be inaccurate", &standardFont, 1, Y_POSITION_OF_ERR_LINE, vmNormal, true, true);
          }
        }

        else {
          sprintf(tmpString, "Displaying %s: to be coded!", getRegisterDataTypeName(regist, true, false));
          showString(tmpString, &standardFont, SCREEN_WIDTH - stringWidth(tmpString, &standardFont, false, true), baseY + 6, vmNormal, false, true);
        }

        if(temporaryInformation == TI_VIEW_REGISTER && origRegist == REGISTER_X) {
          regist = REGISTER_X;
        }
      }

      if(regist == REGISTER_T) {
        lineTWidth = lineWidth;
      }
    }

    if(getRegisterDataType(REGISTER_X) == dtReal34Matrix || getRegisterDataType(REGISTER_X) == dtComplex34Matrix || calcMode == cmMim) {
      displayStack = origDisplayStack;
    }
  }



  void displayNim(const char *nim, const char *lastBase, int16_t wLastBaseNumeric, int16_t wLastBaseStandard) {
    int16_t w;
    if(stringWidth(nim, &numericFont, true, true) + wLastBaseNumeric <= SCREEN_WIDTH - 16) { // 16 is the numeric font cursor width
      uint32_t xCursor = showString(nim, &numericFont, 0, Y_POSITION_OF_NIM_LINE, vmNormal, true, true);
      uint32_t yCursor = Y_POSITION_OF_NIM_LINE;
      cursorShow(false, xCursor, yCursor);

      if(lastIntegerBase != 0) {
        showString(lastBase, &numericFont, xCursor + 16, Y_POSITION_OF_NIM_LINE, vmNormal, true, true);
      }
    }
    else if(stringWidth(nim, &standardFont, true, true) + wLastBaseStandard <= SCREEN_WIDTH - 8) { // 8 is the standard font cursor width
      uint32_t xCursor = showString(nim, &standardFont, 0, Y_POSITION_OF_NIM_LINE + 6, vmNormal, true, true);
      uint32_t yCursor = Y_POSITION_OF_NIM_LINE + 6;
      cursorShow(true, xCursor, yCursor);

      if(lastIntegerBase != 0) {
        showString(lastBase, &standardFont, xCursor + 8, Y_POSITION_OF_NIM_LINE + 6, vmNormal, true, true);
      }
    }
    else {
      w = stringByteLength(nim) + 1;
      xcopy(tmpString,        nim, w);
      xcopy(tmpString + 1500, nim, w);
      while(stringWidth(tmpString, &standardFont, true, true) >= SCREEN_WIDTH) {
        w = stringLastGlyph(tmpString);
        tmpString[w] = 0;
      }

      if(stringWidth(tmpString + 1500 + w, &standardFont, true, true) + wLastBaseStandard > SCREEN_WIDTH - 8) { // 8 is the standard font cursor width
        btnClicked(kcBackspace);
      }
      else {
        showString(tmpString, &standardFont, 0, Y_POSITION_OF_NIM_LINE - 3, vmNormal, true, true);

        uint32_t xCursor = showString(tmpString + 1500 + w, &standardFont, 0, Y_POSITION_OF_NIM_LINE + 18, vmNormal, true, true);
        uint32_t yCursor = Y_POSITION_OF_NIM_LINE + 18;
        cursorShow(true, xCursor, yCursor);

        if(lastIntegerBase != 0) {
          showString(lastBase, &standardFont, xCursor + 8, Y_POSITION_OF_NIM_LINE + 18, vmNormal, true, true);
        }
      }
    }
  }



  uint32_t _screenTamLineYPosition(void) {
    if(calcMode == cmTimerApp) {
      return Y_POSITION_OF_TAM_ALT_LINE;
    }
    return Y_POSITION_OF_TAM_LINE;
  }



  void clearTamBuffer(void) {
    if(shiftF || shiftG) {
      lcd_fill_rect(18, _screenTamLineYPosition(), 120, 32, LCD_SET_VALUE);
    }
    else {
      lcd_fill_rect(0, _screenTamLineYPosition(), 138, 32, LCD_SET_VALUE);
    }
  }



  void clearShiftState(void) {
    lcd_fill_rect(0, Y_POSITION_OF_REGISTER_T_LINE, 15, NUMERIC_FONT_HEIGHT, LCD_SET_VALUE);
  }



  void displayShiftAndTamBuffer(void) {
    if(calcMode == cmAssign) {
      updateAssignTamBuffer();
    }

    if(calcMode != cmAssign || itemToBeAssigned == 0 || tam.alpha) {
      if(shiftF) {
        showGlyph(STD_SUP_f, &numericFont, 0, _screenTamLineYPosition(), vmNormal, true, true); // f is pixel 4+8+3 wide
      }
      else if(shiftG) {
        showGlyph(STD_SUP_g, &numericFont, 0, _screenTamLineYPosition(), vmNormal, true, true); // g is pixel 4+10+1 wide
      }
    }

    if(tamIsActive() || calcMode == cmAssign) {
      if(calcMode == cmPem) { // Variable line to display TAM informations
        lcd_fill_rect(45+20, tamOverPemYPos, 168, 20, LCD_SET_VALUE);
        showString(tamBuffer, &standardFont, 75+20, tamOverPemYPos, vmNormal,  false, false);
      }
      else { // Fixed line to display TAM informations
        clearTamBuffer();
        showString(tamBuffer, &standardFont, 18, _screenTamLineYPosition() + 6, vmNormal, true, true);
      }
    }
  }



  void refreshScreen(void) {
    switch(calcMode) {
      case cmApp: {
        clearScreen();
        appsDraw();
        refreshStatusBar();
        break;
      }

      case cmPem: {
        clearScreen();
        showSoftmenuCurrentPart();
        fnPem(NOPARAM);
        displayShiftAndTamBuffer();
        refreshStatusBar();
        break;
      }

      case cmNormal:
      case cmAim:
      case cmNim:
      case cmMim:
      case cmEim:
      case cmAssign:
      case cmErrorMessage:
      case cmConfirmation:
      case cmTimerApp: {
        if(calcMode == cmConfirmation) {
          screenUpdatingMode = SCRUPD_AUTO;
        }
        else if(calcMode == cmMim) {
          screenUpdatingMode = (aimBuffer[0] == 0) ? SCRUPD_AUTO : (SCRUPD_MANUAL_STACK | SCRUPD_MANUAL_SHIFT_STATUS);
        }

        if(screenUpdatingMode == SCRUPD_AUTO) {
          clearScreen();
        }
        else {
          if(!(screenUpdatingMode & SCRUPD_MANUAL_STATUSBAR)) {
            lcd_fill_rect(0, 0, SCREEN_WIDTH, Y_POSITION_OF_REGISTER_T_LINE, LCD_SET_VALUE);
          }
          if(!(screenUpdatingMode & (SCRUPD_MANUAL_STACK | SCRUPD_SKIP_STACK_ONE_TIME))) {
            lcd_fill_rect(0, Y_POSITION_OF_REGISTER_T_LINE, SCREEN_WIDTH, 240 - Y_POSITION_OF_REGISTER_T_LINE - SOFTMENU_HEIGHT * 3, LCD_SET_VALUE);
          }
          if(!(screenUpdatingMode & (SCRUPD_MANUAL_MENU | SCRUPD_SKIP_MENU_ONE_TIME))) {
            lcd_fill_rect(0, 240 - SOFTMENU_HEIGHT * 3, SCREEN_WIDTH, SOFTMENU_HEIGHT * 3, LCD_SET_VALUE);
          }
        }

        // The ordering of the 4 lines below is important for SHOW (temporaryInformation == TI_SHOW_REGISTER)
        if(!(screenUpdatingMode & (SCRUPD_MANUAL_STACK | SCRUPD_SKIP_STACK_ONE_TIME))) {
          if(calcMode != cmTimerApp && temporaryInformation != TI_VIEW_REGISTER) {
            refreshRegisterLine(REGISTER_T);
          }
          refreshRegisterLine(REGISTER_Z);
          refreshRegisterLine(REGISTER_Y);
          refreshRegisterLine(REGISTER_X);
          if(temporaryInformation == TI_VIEW_REGISTER) {
            clearRegisterLine(REGISTER_T, true, true);
            refreshRegisterLine(REGISTER_T);
          }
        }
        else if(calcMode == cmNim) {
          refreshRegisterLine(NIM_REGISTER_LINE);
        }


        if(calcMode == cmMim) {
          showMatrixEditor();
        }
        if(calcMode == cmTimerApp) {
          timerAppDraw();
        }

        if(currentSolverStatus & SOLVER_STATUS_INTERACTIVE) {
          bool mvarMenu = false;
          for(int i = 0; i < SOFTMENU_STACK_SIZE; i++) {
            if(softmenu[softmenuStack[i].softmenuId].menuItem == -MNU_MVAR) {
              mvarMenu = true;
              break;
            }
          }
          if(!mvarMenu) {
            if(currentSolverStatus & SOLVER_STATUS_USES_FORMULA) {
              if((currentSolverStatus & SOLVER_STATUS_EQUATION_MODE) == SOLVER_STATUS_EQUATION_INTEGRATE) {
                showSoftmenu(-MNU_Sf);
              }
              else {
                showSoftmenu(-MNU_Solver);
              }
            }
            else {
              currentMvarLabel = INVALID_VARIABLE;
              showSoftmenu(-MNU_MVAR);
            }
          }
        }
        if(calcMode == cmEim) {
          bool mvarMenu = false;
          for(int i = 0; i < SOFTMENU_STACK_SIZE; i++) {
            if(softmenu[softmenuStack[i].softmenuId].menuItem == -MNU_EQ_EDIT) {
              mvarMenu = true;
              break;
            }
          }
          if(!mvarMenu) {
            showSoftmenu(-MNU_EQ_EDIT);
          }
        }

        if(!(screenUpdatingMode & SCRUPD_MANUAL_SHIFT_STATUS)) {
          if(screenUpdatingMode & (SCRUPD_MANUAL_STACK | SCRUPD_SKIP_STACK_ONE_TIME)) {
            clearShiftState();
          }
          displayShiftAndTamBuffer();
        }

        if(!(screenUpdatingMode & (SCRUPD_MANUAL_MENU | SCRUPD_SKIP_MENU_ONE_TIME))) {
          showSoftmenuCurrentPart();
        }

        if(programRunStop == PGM_STOPPED || programRunStop == PGM_WAITING) {
          hourGlassIconEnabled = false;
        }
        if(!(screenUpdatingMode & SCRUPD_MANUAL_STATUSBAR)) {
          refreshStatusBar();
        }
        #if (REAL34_WIDTH_TEST == 1)
          for(int y=Y_POSITION_OF_REGISTER_Y_LINE; y<Y_POSITION_OF_REGISTER_Y_LINE + 2*REGISTER_LINE_HEIGHT; y++ ) {
            setBlackPixel(SCREEN_WIDTH - largeur - 1, y);
          }
        #endif // (REAL34_WIDTH_TEST == 1)
        break;
      }

      case cmGraph: {
        clearScreen();
        displayShiftAndTamBuffer();
        showSoftmenuCurrentPart();
        hourGlassIconEnabled = true;
        refreshStatusBar();
        graphPlotstat(plotSelection);
        hourGlassIconEnabled = false;
        showHideHourGlass();
        refreshStatusBar();
        break;
      }

      case cmPlotStat: {
        clearScreen();
        displayShiftAndTamBuffer();
        showSoftmenuCurrentPart();
        hourGlassIconEnabled = true;
        refreshStatusBar();
        graphPlotstat(plotSelection);
        graphDrawLRline(plotSelection);
        hourGlassIconEnabled = false;
        showHideHourGlass();
        refreshStatusBar();
        break;
      }

      default: {
      }
    }
  }
#endif // !TESTSUITE_BUILD



void fnScreenDump(uint16_t unusedButMandatoryParameter) {
  systemScreenshot();
  screenUpdatingMode |= SCRUPD_SKIP_STACK_ONE_TIME | SCRUPD_SKIP_MENU_ONE_TIME;
}



#if !defined(TESTSUITE_BUILD)
  static int32_t _getPositionFromRegister(calcRegister_t regist, int16_t maxValue) {
    int32_t value;

    if(getRegisterDataType(regist) == dtReal34) {
      real34_t maxValue34;

      int32ToReal34(maxValue, &maxValue34);
      if(real34CompareLessThan(REGISTER_REAL34_DATA(regist), const34_0) || real34CompareLessThan(&maxValue34, REGISTER_REAL34_DATA(regist))) {
        displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
        #if defined(PC_BUILD)
          real34ToString(REGISTER_REAL34_DATA(regist), errorMessage);
          sprintf(tmpString, "x %" PRId16 " = %s:", regist, errorMessage);
          moreInfoOnError("In function _getPositionFromRegister:", tmpString, "this value is negative or too big!", NULL);
        #endif // PC_BUILD
        return -1;
      }
      value = real34ToInt32(REGISTER_REAL34_DATA(regist));
    }

    else if(getRegisterDataType(regist) == dtLongInteger) {
      longInteger_t lgInt;

      convertLongIntegerRegisterToLongInteger(regist, lgInt);
      if(longIntegerCompareUInt(lgInt, 0) < 0 || longIntegerCompareUInt(lgInt, maxValue) > 0) {
        displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
        #if defined(PC_BUILD)
          longIntegerToAllocatedString(lgInt, errorMessage, ERROR_MESSAGE_LENGTH);
          sprintf(tmpString, "register %" PRId16 " = %s:", regist, errorMessage);
          moreInfoOnError("In function _getPositionFromRegister:", tmpString, "this value is negative or too big!", NULL);
        #endif // PC_BUILD
        longIntegerFree(lgInt);
        return -1;
      }
      value = longIntegerToUInt(lgInt);
      longIntegerFree(lgInt);
    }

    else {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      #if defined(PC_BUILD)
        sprintf(errorMessage, "register %" PRId16 " is %s:", regist, getRegisterDataTypeName(regist, true, false));
        moreInfoOnError("In function _getPositionFromRegister:", errorMessage, "not suited for addressing!", NULL);
      #endif // PC_BUILD
      return -1;
    }

  return value;
}



static void getPixelPos(int32_t *x, int32_t *y) {
  *x = _getPositionFromRegister(REGISTER_X, SCREEN_WIDTH  - 1);
  *y = _getPositionFromRegister(REGISTER_Y, SCREEN_HEIGHT - 1);
}
#endif // !TESTSUITE_BUILD



void fnClLcd(uint16_t unusedButMandatoryParameter) {
  #if !defined(TESTSUITE_BUILD)
    int32_t x, y;
    getPixelPos(&x, &y);
    if(lastErrorCode == ERROR_NONE) {
      screenUpdatingMode |= SCRUPD_MANUAL_STATUSBAR | SCRUPD_MANUAL_STACK | SCRUPD_MANUAL_MENU | SCRUPD_MANUAL_SHIFT_STATUS;
      lcd_fill_rect(x, 0, SCREEN_WIDTH - x, SCREEN_HEIGHT - y, LCD_SET_VALUE);
    }
  #endif // !TESTSUITE_BUILD
}



void fnPixel(uint16_t unusedButMandatoryParameter) {
  #if !defined(TESTSUITE_BUILD)
    int32_t x, y;
    getPixelPos(&x, &y);
    if(lastErrorCode == ERROR_NONE) {
      screenUpdatingMode |= SCRUPD_MANUAL_STACK | SCRUPD_MANUAL_MENU | SCRUPD_MANUAL_SHIFT_STATUS;
      if((SCREEN_HEIGHT - y - 1) <= Y_POSITION_OF_REGISTER_T_LINE) {
        screenUpdatingMode |= SCRUPD_MANUAL_STATUSBAR;
      }
      setBlackPixel(x, SCREEN_HEIGHT - y - 1);
    }
  #endif // TESTSUITE_BUILD
}



void fnPoint(uint16_t unusedButMandatoryParameter) {
#if !defined(TESTSUITE_BUILD)
  int32_t x, y;
  getPixelPos(&x, &y);
  if(lastErrorCode == ERROR_NONE) {
    screenUpdatingMode |= SCRUPD_MANUAL_STACK | SCRUPD_MANUAL_MENU | SCRUPD_MANUAL_SHIFT_STATUS;
    if((SCREEN_HEIGHT - y - 2) <= Y_POSITION_OF_REGISTER_T_LINE) {
      screenUpdatingMode |= SCRUPD_MANUAL_STATUSBAR;
    }
    lcd_fill_rect(x - 1, SCREEN_HEIGHT - y - 2, 3, 3, LCD_EMPTY_VALUE);
  }
#endif // TESTSUITE_BUILD
}



void fnAGraph(uint16_t regist) {
  #if !defined(TESTSUITE_BUILD)
    int32_t x, y;
    uint32_t gramod;
    longInteger_t liGramod;
    getPixelPos(&x, &y);
    convertLongIntegerRegisterToLongInteger(RESERVED_VARIABLE_GRAMOD, liGramod);
    gramod = longIntegerToUInt(liGramod);
    longIntegerFree(liGramod);
    if(lastErrorCode == ERROR_NONE) {
      if(getRegisterDataType(regist) == dtShortInteger) {
        uint64_t val;
        int16_t sign;
        const uint8_t savedShortIntegerMode = shortIntegerMode;

        screenUpdatingMode |= SCRUPD_MANUAL_STACK | SCRUPD_MANUAL_MENU | SCRUPD_MANUAL_SHIFT_STATUS;
        if((SCREEN_HEIGHT - y - 1 - (int)shortIntegerWordSize) <= Y_POSITION_OF_REGISTER_T_LINE) {
          screenUpdatingMode |= SCRUPD_MANUAL_STATUSBAR;
        }
        shortIntegerMode = SIM_UNSIGN;
        convertShortIntegerRegisterToUInt64(regist, &sign, &val);
        shortIntegerMode = savedShortIntegerMode;
        for(uint32_t i = 0; i < shortIntegerWordSize; ++i) {
          switch(gramod) {
            case 1: {
              if(!(val & 1)) {
                setWhitePixel(x, SCREEN_HEIGHT - y - 1 - i);
              }
            }
            /* fallthrough */
            case 0: {
              if(val & 1) {
                setBlackPixel(x, SCREEN_HEIGHT - y - 1 - i);
              }
              break;
            }
            case 2: {
              if(val & 1) {
                setWhitePixel(x, SCREEN_HEIGHT - y - 1 - i);
              }
              break;
            }
            case 3: {
              if(val & 1) {
                flipPixel(x, SCREEN_HEIGHT - y - 1 - i);
              }
              break;
            }
          }
          val >>= 1;
        }

      fnInc(REGISTER_X);
    }

      else {
        displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
        #if defined(PC_BUILD)
          sprintf(errorMessage, "register %" PRId16 " is %s:", regist, getRegisterDataTypeName(regist, true, false));
          moreInfoOnError("In function fnAGraph:", errorMessage, "not suited for addressing!", NULL);
        #endif // PC_BUILD
      }
    }
  #endif // !TESTSUITE_BUILD
}
