// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "hal/gui.h"

#include "calcMode.h"
#include "gtkGui.h"
#include "items.h"
#include "ui/tam.h"
#include <stdbool.h>

#include "wp43.h"

#if (SCREEN_800X480 == 0)
  static void _guiShowKey(guiLayout_t layout, int key, bool show) {
    if(show) {
      gtk_fixed_move(GTK_FIXED(grid), calcKeyboard[key].keyImage[layout], calcKeyboard[key].x, calcKeyboard[key].y);
    } else {
      gtk_fixed_move(GTK_FIXED(grid), calcKeyboard[key].keyImage[layout], -999, -999);
    }
  }



  void guiSetLayout(guiLayout_t layout) {
    currentBezel = layout;

    for(guiLayout_t i = 0; i < MAX_GUI_LAYOUTS; i++) {
      if(layout == i) {
        gtk_fixed_move(GTK_FIXED(grid), bezelImage[i], bezelX[i], bezelY[i]);
        for (int key = 0; key < MAX_KEYS; key++) {
          if(key == 10) {
            if(layout == glTam && guiUseTamL()) {
              _guiShowKey(TAM_L_LAYOUT, key, true);
              _guiShowKey(i,            key, false);
            } else {
              _guiShowKey(i,            key, true);
              _guiShowKey(TAM_L_LAYOUT, key, false);
            }
          } else {
            _guiShowKey(i, key, true);
          }
        }
      } else {
        gtk_fixed_move(GTK_FIXED(grid), bezelImage[i], -999, -999);
        for (int key = 0; key < MAX_KEYS; key++) {
          _guiShowKey(i, key, false);
        }
      }
    }
  }
#else
  void guiSetLayout(guiLayout_t layout) {}
#endif
