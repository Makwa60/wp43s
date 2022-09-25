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

  #include "hal/gui.h"

  // One extra layout is sometimes needed for one key in TAM
  #define MAX_GTK_LAYOUTS (MAX_GUI_LAYOUTS + 1)
  #define TAM_L_LAYOUT    MAX_GUI_LAYOUTS
  #define MAX_KEYS        43

  /**
   * \struct calcKeyboard_t
   * Structure keeping key images, image sizes, and image locations.
   */
  typedef struct {
    int x, y;
    int width[MAX_GTK_LAYOUTS], height[MAX_GTK_LAYOUTS];
    GtkWidget *keyImage[MAX_GTK_LAYOUTS];
  } calcKeyboard_t;

  /**
   * Creates the calc's GUI window with all the widgets.
   */
  void setupUI(void);

  extern GtkWidget      *grid;
  extern calcKeyboard_t  calcKeyboard[MAX_KEYS];
  extern guiLayout_t     currentBezel;
  #if (SCREEN_800X480 == 0)
    extern GtkWidget *bezelImage[MAX_GUI_LAYOUTS];
    extern int bezelX[MAX_GUI_LAYOUTS], bezelY[MAX_GUI_LAYOUTS];
  #endif // (SCREEN_800X480 == 0)
#endif // !GTKGUI_H
