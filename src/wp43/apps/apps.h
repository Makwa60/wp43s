// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file apps/apps.h
 */
#if !defined(APPS_H)
  #define APPS_H

  #include "hal/gui.h"
  #include <stdbool.h>
  #include <stdint.h>

  // Interface for other modules

  /**
   * App identifier
   */
  typedef enum {
    appNone            = 0,
    appRegisterBrowser = 1,
    appFlagBrowser     = 2,
    appFontBrowser     = 3,
    appBugScreen       = 4,
  } appId_t;
  extern appId_t currentApp;


  /**
   * Handles a key press when in app mode.
   * It should be assumed that the app will completely handle the key
   * press and no work need be done elsewhere related to this key press.
   * This should be called on key press, not on release as app keys are
   * not NOPable by holding.
   *
   * \param[in] item the item indicating the key that is pressed
   */
  void        appsHandleKey(int16_t item);

  /**
   * Draw the app main screen.
   * Apps replace the entire screen except for the status bar which
   * should continue to be drawn in the usual way. This should be
   * called whenever a screen refresh is needed.
   */
  void        appsDraw     (void);

  /**
   * Return the current key layout for the running app.
   *
   * \return GUI layout that the app uses
   */
  guiLayout_t appsGetLayout(void);

  // Interface for apps

  typedef bool (*appsKeyHandler_t)(int16_t item);
  typedef void (*appsDraw_t)      (void);

  void appsEnter(guiLayout_t layout, appsKeyHandler_t keyHandler, appsDraw_t draw);

#endif // !APPS_H
