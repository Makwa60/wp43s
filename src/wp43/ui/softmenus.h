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

  /**
   * \enum softmenuIndex_t
   * Index of base menus in softmenu() table.
   */
  typedef enum {
    smMyMenu           =   0,  // MyMenu
    smMyAlpha          =   1,  // MyAlpha
    smMyPFN            =   2,  // MyPFN
    smEqEdit           = 105,  // Equation Editor
  } softmenuIndex_t;
 
  /**
   * \enum smStackMode_t
   * Softmenu stack mode (2 bits).
   */
  typedef enum {
    smNormal           = 0,  // Normal operation
    smAim              = 1,  // Alpha input mode
    smPem              = 2,  // Program entry mode
    smEim              = 3,  // Equation imput mode
  } smStackMode_t;
  
  extern smStackMode_t smStackMode[4];  // Softmenu stack mode stack - depth should be 3 max (normal -> pem -> aim)

  uint8_t *getNthString           (uint8_t *ptr, int16_t n); // Starting with string 0 (the 1st string is returned for n=0)
  void     fnDynamicMenu          (uint16_t unusedButMandatoryParameter);
  void     fnClearMyMenu          (uint16_t unusedButMandatoryParameter);
  void     fnClearMyPFN           (uint16_t unusedButMandatoryParameter);
  void     fnClearMyAlpha         (uint16_t unusedButMandatoryParameter);
  void     fnExitAllMenus         (uint16_t unusedButMandatoryParameter);
  void     initMyPFN              (bool stdMenu); 

    /**
     * Initialize the user menus and prepopulate MyAlpha and MyPFN.
     */  void     initUserMenus          (void);

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
     * Sets currentUserMenu to the id of funcparam in userMenus[] table
     *
     * \param[in] item        function will set currentUserMenu if item == -MNU_DYNAMIC)
     * \param[in] funcParam   user menu name to check for id if item == -MNU_DYNAMIC
     */
     void   setCurrentUserMenu     (int16_t item, char* funcParam);
    
    /**
     * Softmenus Stacks initialization
     */
    void    softmenuStacksInit     (void);

    /**
     * Returns softmenu ID
     */
    int16_t getSoftmenuId          (int index);

    /**
     * Sets softmenu ID
     */
    void    setSoftmenuId          (int index, int16_t menuId);

    /**
     * Returns softmenu first item
     */
    int16_t getSoftmenuFirstItem   (void);

    /**
     * Sets softmenu  first item
     */
    void    setSoftmenuFirstItem   (int16_t item);

    /**
     * Pops a softmenu from the softmenu stack.
     */
    void    popSoftmenu            (void);

    /**
     * Remove a User menu from a softmenu stack.
     */
    void removeUserMenuFromStacks  (int16_t userMenuId);
    
     /**
     * Push a new stackmode to the softmenu stackmode stack.
     */
    void pushSmStackMode           (smStackMode_t stackMode);
    
    /**
     * Pops a stackmode from the softmenu stackmode stack.
     */
    void popSmStackMode            (void);
    
     /**
     * Returns current softmenu stack mode.
     */
    smStackMode_t getSmStackMode   (void);

    void    setCatalogLastPos      (void);
    bool    currentSoftmenuScrolls (void);
    bool    isAlphabeticSoftmenu   (void);
  #endif // !TESTSUITE_BUILD

  char     *dynmenuGetLabel        (int16_t menuitem);
  char     *dynmenuGetLabelWithDup (int16_t menuitem, int16_t *dupNum);

#endif // !SOFTMENUS_H
