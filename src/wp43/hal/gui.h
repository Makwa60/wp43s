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

  #if defined(DMCP_BUILD)
    // Inline these empty functions for DMCP to save memory
    static inline void calcModeNormalGui (void) {}
    static inline void calcModeAimGui    (void) {}
    static inline void calcModeTamGui    (void) {}
  #else
    /**
     * Show the keys associated with the normal layout.
     * The graphical user interface should update the labels
     * on the keys to show the outcome of pressing the key in
     * normal mode.
     */
    void calcModeNormalGui (void);

    /**
     * Show the keys associated with Alpha Input Mode.
     * The graphical user interface should update the labels
     * on the keys to show the outcome of pressing the key in
     * alpha mode.
     */
    void calcModeAimGui    (void);

    /**
     * Show the keys associated with Transient Alpha Mode.
     * The graphical user interface should update the labels
     * on the keys to show the outcome of pressing the key in
     * the transient alpha mode.
     */
    void calcModeTamGui    (void);
  #endif
#endif // !GUI_H
