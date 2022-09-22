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

/**
 * \file screen.h
 * Screen related functions.
 */
#if !defined(SCREEN_H)
  #define SCREEN_H

  #include "typeDefinitions.h"
  #include <stdint.h>

  void       fnScreenDump                       (uint16_t unusedButMandatoryParameter);

  void       fnClLcd                            (uint16_t unusedButMandatoryParameter);
  void       fnPixel                            (uint16_t unusedButMandatoryParameter);
  void       fnPoint                            (uint16_t unusedButMandatoryParameter);
  void       fnAGraph                           (uint16_t regist);

  /**
   * Refreshes calc's screen.
   * This function is called every SCREEN_REFRESH_PERIOD ms.
   * - make the cursor blink if needed
   * - refresh date and time in the status bar if needed
   * - refresh the whole screen if needed
   */
  void     refreshLcd                         (void);

  void     clearScreen                        (void);

  void     execTimerApp                         (uint16_t timerType);

  #if !defined(TESTSUITE_BUILD)
    void     refreshScreen                      (void);
    //void     invertPixel                        (uint32_t x, uint32_t y);
    //int      getPixel                           (uint32_t x, uint32_t y);
    /**
     * Displays a 0 terminated string.
     *
     * \param[in] string          String whose first glyph is to display
     * \param[in] font            Font to use
     * \param[in] x               x coordinate where to display the glyph
     * \param[in] y               y coordinate where to display the glyph
     * \param[in] videoMode       Video mode normal or reverse
     * \param[in] showLeadingCols Display the leading empty columns
     * \param[in] showEndingCols  Display the ending empty columns
     * \return x coordinate for the next glyph
     */
    uint32_t showString                         (const char *str,   const font_t *font, uint32_t x, uint32_t y, videoMode_t videoMode, bool_t showLeadingCols, bool_t showEndingCols);

    /**
     * Displays the first glyph of a string.
     *
     * \param[in] ch              String whose first glyph is to display
     * \param[in] font            Font to use
     * \param[in] x               x coordinate where to display the glyph
     * \param[in] y               y coordinate where to display the glyph
     * \param[in] videoMode       Video mode normal or reverse
     * \param[in] showLeadingCols Display the leading empty columns
     * \param[in] showEndingCols  Display the ending empty columns
     * \return x coordinate for the next glyph
     */
    uint32_t showGlyph                          (const char *ch,    const font_t *font, uint32_t x, uint32_t y, videoMode_t videoMode, bool_t showLeadingCols, bool_t showEndingCols);

    /**
     * Displays a glyph using it's Unicode code point.
     *
     * \param[in] charCode        Unicode code point of the glyph to display
     * \param[in] font            Font to use
     * \param[in] x               x coordinate where to display the glyph
     * \param[in] y               y coordinate where to display the glyph
     * \param[in] videoMode       Video mode normal or reverse
     * \param[in] showLeadingCols Display the leading empty columns
     * \param[in] showEndingCols  Display the ending empty columns
     * \return x coordinate for the next glyph
     */
    uint32_t showGlyphCode                      (uint16_t charCode, const font_t *font, uint32_t x, uint32_t y, videoMode_t videoMode, bool_t showLeadingCols, bool_t showEndingCols);

    /**
     * Hides the cursor.
     */
    void     hideCursor                         (void);

    /**
     * Displays the function name.
     * The function name of the currently pressed button is shown in the
     * upper left corner of the T register line.
     *
     * \param[in] item     Item ID to show
     * \param[in] counter  number of 1/10 seconds until NOP
     */
    void     showFunctionName                   (int16_t item, int16_t delayInMs);

    /**
     * Hides the function name.
     * The function name in the upper left corner of the T register line is hidden
     * and the counter is cleared.
     */
    void     hideFunctionName                   (void);

    /**
     * Clears one register line.
     *
     * \param[in] yStart y coordinate from where starting to clear
     */
    void     clearRegisterLine                  (calcRegister_t regist, bool_t clearTop, bool_t clearBottom);

    /**
     * Updates matrix height cache.
     */
    void     updateMatrixHeightCache            (void);

    /**
     * Displays one register line.
     *
     * \param[in] regist Register line to display
     */
    void     refreshRegisterLine                (calcRegister_t regist);

    void     displayNim                         (const char *nim, const char *lastBase, int16_t wLastBaseNumeric, int16_t wLastBaseStandard);
    void     clearTamBuffer                     (void);
    void     clearShiftState                    (void);
    void     displayShiftAndTamBuffer           (void);
  #endif // !TESTSUITE_BUILD
#endif // !SCREEN_H
