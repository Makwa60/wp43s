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
 * \file gtkScreen.h
 * Screen related functions.
 */
#if !defined(GTKSCREEN_H)
  #define GTKSCREEN_H

  #include "typeDefinitions.h"
  #if defined(PC_BUILD)
    #include <gtk/gtk.h>
    #include <gdk/gdk.h>
  #endif // PC_BUILD

  #if defined(PC_BUILD)
    /**
     * Draws the calc's screen on the PC window widget.
     *
     * \param[in] widget Not used
     * \param[in] cr
     * \param[in] data   Not used
     */
    gboolean drawScreen                         (GtkWidget *widget, cairo_t *cr, gpointer data);

    void     copyScreenToClipboard              (void);
    void     copyRegisterXToClipboard           (void);
    void     copyStackRegistersToClipboardString(char *clipboardString);
    void     copyStackRegistersToClipboard      (void);
    void     copyAllRegistersToClipboard        (void);
    void     copyRegisterToClipboardString      (calcRegister_t regist, char *clipboardString);
  #endif // PC_BUILD

#endif // !GTKSCREEN_H
