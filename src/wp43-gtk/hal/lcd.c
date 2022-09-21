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

#include "hal/lcd.h"

#include "defines.h"
#include "wp43.h"


void setBlackPixel(uint32_t x, uint32_t y) {
  if(x>=SCREEN_WIDTH || y>=SCREEN_HEIGHT) {
    printf("In function setBlackPixel: x=%u, y=%u outside the screen!\n", x, y);
    return;
  }

  *(screenData + y*screenStride + x) = ON_PIXEL;
  screenChange = true;
}



void setWhitePixel(uint32_t x, uint32_t y) {
  if(x>=SCREEN_WIDTH || y>=SCREEN_HEIGHT) {
    printf("In function setWhitePixel: x=%u, y=%u outside the screen!\n", x, y);
    return;
  }

  *(screenData + y*screenStride + x) = OFF_PIXEL;
  screenChange = true;
}



void flipPixel(uint32_t x, uint32_t y) {
  if(x>=SCREEN_WIDTH || y>=SCREEN_HEIGHT) {
    printf("In function flipPixel: x=%u, y=%u outside the screen!\n", x, y);
    return;
  }

  if(*(screenData + y*screenStride + x) == OFF_PIXEL) {
    *(screenData + y*screenStride + x) = ON_PIXEL;
  }
  else {
    *(screenData + y*screenStride + x) = OFF_PIXEL;
  }
  screenChange = true;
}



void lcd_fill_rect(uint32_t x, uint32_t y, uint32_t dx, uint32_t dy, int val) {
  uint32_t line, col, pixelColor, *pixel, endX = x + dx, endY = y + dy;

  if(endX > SCREEN_WIDTH || endY > SCREEN_HEIGHT) {
    printf("In function lcd_fill_rect: x=%u, y=%u, dx=%u, dy=%u, val=%d outside the screen!\n", x, y, dx, dy, val);
    return;
  }

  pixelColor = (val == LCD_SET_VALUE ? OFF_PIXEL : ON_PIXEL);
  for(line=y; line<endY; line++) {
    for(col=x, pixel=screenData + line*screenStride + x; col<endX; col++, pixel++) {
      *pixel = pixelColor;
    }
  }

  screenChange = true;
}
