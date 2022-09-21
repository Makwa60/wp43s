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
 * \file lcd.h
 * LCD related functions.
 */
#if !defined(LCD_H)
  #define LCD_H

  #include <stdint.h>

  #if defined(DMCP_BUILD)
    #include <dmcp.h>

    static inline void setBlackPixel(uint32_t x, uint32_t y) {
      bitblt24(x, 1, y, 1, BLT_OR,   BLT_NONE);
    }
    static inline void setWhitePixel(uint32_t x, uint32_t y) {
      bitblt24(x, 1, y, 1, BLT_ANDN, BLT_NONE);
    }
    static inline void flipPixel(uint32_t x, uint32_t y) {
      bitblt24(x, 1, y, 1, BLT_XOR,  BLT_NONE);
    }
  #else
    void     lcd_fill_rect                      (uint32_t x, uint32_t y, uint32_t dx, uint32_t dy, int val); // clone from the DMCP function

      /**
       * Sets a black pixel on the screen.
       *
       * \param[in] x x coordinate from 0 (left) to 399 (right)
       * \param[in] y y coordinate from 0 (top) to 239 (bottom)
       * \return void
       */
    void     setBlackPixel                      (uint32_t x, uint32_t y);

      /**
       * Sets a white pixel on the screen.
       *
       * \param[in] x x coordinate from 0 (left) to 399 (right)
       * \param[in] y y coordinate from 0 (top) to 239 (bottom)
       */
    void     setWhitePixel                      (uint32_t x, uint32_t y);

      /**
       * Turns a black pixel to a white pixel or vice versa on the screen.
       *
       * \param[in] x x coordinate from 0 (left) to 399 (right)
       * \param[in] y y coordinate from 0 (top) to 239 (bottom)
       */
    void     flipPixel                          (uint32_t x, uint32_t y);
  #endif // DMCP_BUILD

#endif // !LCD_H
