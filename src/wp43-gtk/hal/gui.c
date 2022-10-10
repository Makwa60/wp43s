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

#include "hal/gui.h"

#include "calcMode.h"
#include "gtkGui.h"
#include "items.h"
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

  bool _guiUseTamL(void) {
    return (tam.mode == TM_LABEL || (tam.mode == TM_SOLVE && (tam.function != ITM_SOLVE || calcMode != cmPem)) || (tam.mode == TM_KEY && tam.keyInputFinished));
  }

  void guiSetLayout(guiLayout_t layout) {
    currentBezel = layout;

    for(guiLayout_t i = 0; i < MAX_GUI_LAYOUTS; i++) {
      if(layout == i) {
        gtk_fixed_move(GTK_FIXED(grid), bezelImage[i], bezelX[i], bezelY[i]);
        for (int key = 0; key < MAX_KEYS; key++) {
          if(key == 10) {
            if(layout == glTam && _guiUseTamL()) {
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
