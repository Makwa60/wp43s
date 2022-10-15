// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

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
