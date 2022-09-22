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
 * \file gui.h
 */
#if !defined(GTKGUI_H)
  #define GTKGUI_H

  /**
   * \struct calcKeyboard_t
   * Structure keeping key images, image sizes, and image locations.
   */
  typedef struct {
    int x, y;
    int width[4], height[4];
    GtkWidget *keyImage[4];
  } calcKeyboard_t;

  /**
   * Creates the calc's GUI window with all the widgets.
   */
  void setupUI(void);

  extern GtkWidget      *grid;
  extern calcKeyboard_t  calcKeyboard[43];
  #if (SCREEN_800X480 == 0)
    extern GtkWidget *bezelImage[3];
    extern int bezelX[3], bezelY[3];
  #endif // (SCREEN_800X480 == 0)
#endif // !GTKGUI_H
