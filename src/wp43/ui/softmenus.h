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
 * \file ui/softmenus.h
 * List of softmenus and related functions.
 */
#if !defined(SOFTMENUS_H)
  #define SOFTMENUS_H

  #include "typeDefinitions.h"
  #include <stdint.h>

  uint8_t *getNthString           (uint8_t *ptr, int16_t n); // Starting with string 0 (the 1st string is returned for n=0)
  void     fnDynamicMenu          (uint16_t unusedButMandatoryParameter);

  void     fnExitAllMenus         (uint16_t unusedButMandatoryParameter);
  #if !defined(TESTSUITE_BUILD)
    /**
     * Displays one softkey.
     *
     * \param[in] label      Text to display
     * \param[in] xSoftkey   x location of softkey: from 0 (left) to 5 (right)
     * \param[in] ySoftKey   y location of softkey: from 0 (bottom) to 2 (top)
     * \param[in] videoMode  Video mode normal or reverse
     * \param[in] topLine    Draw a top line
     * \param[in] bottomLine Draw a bottom line
     */
    void   showSoftkey            (const char *label, int16_t xSoftkey, int16_t ySoftKey, videoMode_t videoMode, bool_t topLine, bool_t bottomLine);

    /**
     * Displays the current part of the displayed softmenu.
     */
    void   showSoftmenuCurrentPart(void);

    /**
     * Displays a softmenu.
     *
     * \param[in] id ID of softmenu
     */
    void   showSoftmenu           (int16_t id);

    /**
     * Pops a softmenu from the softmenu stack.
     */
    void   popSoftmenu            (void);

    void   setCatalogLastPos      (void);
    bool_t currentSoftmenuScrolls (void);
    bool_t isAlphabeticSoftmenu   (void);
  #endif // !TESTSUITE_BUILD
  char    *dynmenuGetLabel        (int16_t menuitem);
#endif // !SOFTMENUS_H
