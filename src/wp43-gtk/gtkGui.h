// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file gtkGui.h
 */
#if !defined(GTKGUI_H)
  #define GTKGUI_H

  #include "hal/gui.h"
  #include <stdbool.h>

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
  bool guiUseTamL(void);

  extern GtkWidget      *grid;
  extern calcKeyboard_t  calcKeyboard[MAX_KEYS];
  extern guiLayout_t     currentBezel;
  #if (SCREEN_800X480 == 0)
    extern GtkWidget *bezelImage[MAX_GUI_LAYOUTS];
    extern int bezelX[MAX_GUI_LAYOUTS], bezelY[MAX_GUI_LAYOUTS];
  #endif // (SCREEN_800X480 == 0)

#endif // !GTKGUI_H
