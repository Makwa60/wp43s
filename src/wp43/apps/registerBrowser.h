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
