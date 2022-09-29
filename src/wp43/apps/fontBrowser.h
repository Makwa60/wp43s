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
     * The font browser draw routine.
     */
    void fontBrowserDraw(void);

    /**
     * The font browser application.
     *
     * \param[in] unusedButMandatoryParameter
     */
    void fnFontBrowser  (uint16_t unusedButMandatoryParameter);
  #else
    #pragma GCC diagnostic ignored "-Wunused-parameter"

    static inline void fontBrowserInit(void) {}
    static inline void fontBrowserDraw(void) {}
    static inline void fnFontBrowser  (uint16_t unusedButMandatoryParameter) {}
  #endif // !TESTSUITE_BUILD
#endif // !FONTBROWSER_H
