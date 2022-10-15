// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file ui/softmenus.h
 * List of softmenus and related functions.
 */
#if !defined(SOFTMENUS_H)
  #define SOFTMENUS_H

  #include "typeDefinitions.h"
  #include <stdbool.h>
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
    void   showSoftkey            (const char *label, int16_t xSoftkey, int16_t ySoftKey, videoMode_t videoMode, bool topLine, bool bottomLine);

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
    bool   currentSoftmenuScrolls (void);
    bool   isAlphabeticSoftmenu   (void);
  #endif // !TESTSUITE_BUILD

  char    *dynmenuGetLabel        (int16_t menuitem);

#endif // !SOFTMENUS_H
