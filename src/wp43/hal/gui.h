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
 * \file hal/gui.h
 */
#if !defined(GUI_H)
  #define GUI_H

  #include "defines.h"

  /**
   * Graphical keyboard layout.
   * Enumeration of the possible layouts of the keys.
   */
  typedef enum {
    glNormal          = 0,
    glAim             = 1,
    glTam             = 2,
    glRegisterBrowser = 3,
    glFlagFontBrowser = 4,
    glTimerApp        = 5
  } guiLayout_t;
  #define MAX_GUI_LAYOUTS 6

  #if defined(DMCP_BUILD)
    // Inline this empty function for DMCP to save memory
    static inline void guiSetLayout(guiLayout_t layout) {}
  #else
    /**
     * Show the keys associated with the specified layout.
     * The graphical user interface should update the labels
     * on the keys to show the outcome of pressing the key in
     * the given layout.
     *
     * \param[in] layout the layout for the key labels
     */
    void guiSetLayout(guiLayout_t layout);
  #endif
#endif // !GUI_H
