// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file gtkDebug.h
 */
#if !defined(GTKDEBUG_H)
  #define GTKDEBUG_H

  #include "defines.h"
  #include <gtk/gtk.h>

  #if (DEBUG_REGISTER_L == 1)
    extern GtkWidget         *lblRegisterL1;
    extern GtkWidget         *lblRegisterL2;
  #endif // (DEBUG_REGISTER_L == 1)
  #if (SHOW_MEMORY_STATUS == 1)
    extern GtkWidget         *lblMemoryStatus;
  #endif // (SHOW_MEMORY_STATUS == 1)

  #if (DEBUG_PANEL == 1)
    extern int16_t debugWindow;

    extern GtkWidget *lbl1[DEBUG_LINES], *lbl2[DEBUG_LINES];
    extern GtkWidget *btnBitFields, *btnFlags, *btnRegisters, *btnLocalRegisters, *btnTmpAndSavedStackRegisters;
    extern GtkWidget *chkHexaString;
    extern int16_t debugWidgetDx, debugWidgetDy;

    void  btnBitFieldsClicked          (GtkWidget *w, gpointer data);
    void  btnFlagsClicked              (GtkWidget *w, gpointer data);
    void  btnRegistersClicked          (GtkWidget *w, gpointer data);
    void  btnLocalRegistersClicked     (GtkWidget *w, gpointer data);
    void  btnStatisticalSumsClicked    (GtkWidget *w, gpointer data);
    void  btnNamedVariablesClicked     (GtkWidget *w, gpointer data);
    void  btnSavedStackRegistersClicked(GtkWidget *w, gpointer data);
    void  chkHexaStringClicked         (GtkWidget *w, gpointer data);
  #endif // (DEBUG_PANEL == 1)

#endif // !GTKDEBUG_H
