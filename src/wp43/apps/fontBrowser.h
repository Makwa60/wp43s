// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file apps/fontBrowser.h
 */
#if !defined(FONTBROWSER_H)
  #define FONTBROWSER_H

  #include <stdint.h>

  extern uint8_t currentFntScr;

  #if !defined(TESTSUITE_BUILD)
    /**
     * The font browser application initialisation.
     */
    void fontBrowserInit(void);

    /**
     * The font browser application.
     *
     * \param[in] unusedButMandatoryParameter
     */
    void fnFontBrowser  (uint16_t unusedButMandatoryParameter);
  #else
    #pragma GCC diagnostic ignored "-Wunused-parameter"

    static inline void fontBrowserInit(void) {}
    static inline void fnFontBrowser  (uint16_t unusedButMandatoryParameter) {}
  #endif // !TESTSUITE_BUILD

#endif // !FONTBROWSER_H
