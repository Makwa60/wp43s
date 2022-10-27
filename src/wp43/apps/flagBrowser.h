// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file apps/flagBrowser.h
 */
#if !defined(FLAGBROWSER_H)
  #define FLAGBROWSER_H

  #include <stdint.h>

  typedef enum {
    fsUninitialized = 0,
    fsScreen1       = 1,
    fsScreen2       = 2,
    fsOnlyOneScreen = 3
  } flagScreen_t;
  extern flagScreen_t flagScreen;

  #if !defined(TESTSUITE_BUILD)
    /**
     * The flag browser initialisation.
     */
    void flagBrowserInit(void);

    /**
     * The flag browser application.
     *
     * \param[in] unusedButMandatoryParameter uint16_t
     */
    void fnFlagBrowser  (uint16_t unusedButMandatoryParameter);
  #else
    #pragma GCC diagnostic ignored "-Wunused-parameter"

    static inline void flagBrowserInit(void) {}
    static inline void fnFlagBrowser  (uint16_t unusedButMandatoryParameter) {}
  #endif // !TESTSUITE_BUILD

#endif // !FLAGBROWSER_H
