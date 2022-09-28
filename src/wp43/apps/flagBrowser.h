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
 * \file apps/flagBrowser.h
 */
#if !defined(FLAGBROWSER_H)
  #define FLAGBROWSER_H

  #include <stdint.h>

  #if !defined(TESTSUITE_BUILD)
    /**
     * The flag browser draw routine.
     */
    void flagBrowserDraw(void);

    /**
     * The flag browser application.
     *
     * \param[in] unusedButMandatoryParameter uint16_t
     */
    void fnFlagBrowser  (uint16_t unusedButMandatoryParameter);
  #else
    static inline void fnFlagBrowser  (uint16_t unusedButMandatoryParameter) {}
    static inline void flagBrowserDraw(void) {}
  #endif // !TESTSUITE_BUILD
#endif // !FLAGBROWSER_H
