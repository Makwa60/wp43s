// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file gtkScreen.h
 * Screen related functions.
 */
#if !defined(GTKSCREEN_H)
  #define GTKSCREEN_H

  #include "typeDefinitions.h"
  #include <gtk/gtk.h>
  #include <gdk/gdk.h>

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

#endif // !GTKSCREEN_H
