// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

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

  *(screenData + y*screenStride + x) ^= (ON_PIXEL ^ OFF_PIXEL);
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
