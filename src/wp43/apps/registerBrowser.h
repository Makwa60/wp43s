// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file apps/registerBrowser.h
 */
#if !defined(REGISTERBROWSER_H)
  #define REGISTERBROWSER_H

  #include <stdbool.h>
  #include <stdint.h>

  extern bool    showContent;
  extern uint8_t rbrMode;
  extern int16_t currentRegisterBrowserScreen;

  #if !defined(TESTSUITE_BUILD)
    /**
     * The register browser initialisation.
     */
    void registerBrowserInit(void);

    /**
     * The register browser.
     *
     * \param[in] unusedButMandatoryParameter uint16_t
     */
    void fnRegisterBrowser(uint16_t unusedButMandatoryParameter);
  #else
    #pragma GCC diagnostic ignored "-Wunused-parameter"

    static inline void registerBrowserInit(void) {}
    static inline void fnRegisterBrowser(uint16_t unusedButMandatoryParameter) {}
  #endif // !TESTSUITE_BUILD

#endif // !REGISTERBROWSER_H
