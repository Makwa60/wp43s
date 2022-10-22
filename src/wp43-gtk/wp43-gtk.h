// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file wp43-gtk.h
 */
#if !defined(WP43_GTK_H)
  #define WP43_GTK_H

  #include <gtk/gtk.h>
  #include <stdbool.h>

  extern bool       calcLandscape;
  extern bool       calcAutoLandscapePortrait;
  extern char       debugString[10000];
  extern GtkWidget *screen;
  extern GtkWidget *frmCalc;

#endif // !WP43_GTK_H
