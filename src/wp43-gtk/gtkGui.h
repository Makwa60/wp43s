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
   * \struct gdkKeyMap_t
   * Structure keeping the mapping between character items and GDK_KEY values.
   */
  typedef struct {
    int16_t   item;
    uint32_t  gdkKey;
  } gdkKeyMap_t;

  /**
   * \struct deadKeys_t
   * Structure keeping the mapping between character items and their equivalent when dead keys are used.
   */
  typedef struct {
    int16_t   item;
    int16_t   item_macron;
    int16_t   item_acute;
    int16_t   item_breve;
    int16_t   item_grave;
    int16_t   item_diaresis;
    int16_t   item_tilde;
    int16_t   item_circ;
    int16_t   item_caron;
    int16_t   item_ogonek;
    int16_t   item_ring;
    int16_t   item_cedilla;
    int16_t   item_stroke;
    int16_t   item_dot;
  }   deadKeysMap_t;
  /**
   * Creates the calc's GUI window with all the widgets.
   */
  void     setupUI                     (void);
  bool     guiUseTamL                  (void);
  gboolean setAlphaCaseToCapsLockState (void);

  extern GtkWidget      *grid;
  extern calcKeyboard_t  calcKeyboard[MAX_KEYS];
  extern guiLayout_t     currentBezel;
  #if (SCREEN_800X480 == 0)
    extern GtkWidget *bezelImage[MAX_GUI_LAYOUTS];
    extern int bezelX[MAX_GUI_LAYOUTS], bezelY[MAX_GUI_LAYOUTS];
  #endif // (SCREEN_800X480 == 0)

#endif // !GTKGUI_H
