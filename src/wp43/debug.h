// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file debug.h
 */
#if !defined(DEBUG_H)
  #define DEBUG_H

  #include "defines.h"
  #include "realType.h"
  #include "typeDefinitions.h"
  #include <stdbool.h>
  #include <stdint.h>
  #if defined(PC_BUILD)
    #include <gtk/gtk.h>
    #include <gdk/gdk.h>
  #endif // PC_BUILD

  #if (DEBUG_PANEL == 1)
    extern GtkWidget *lbl1[DEBUG_LINES], *lbl2[DEBUG_LINES];
    extern GtkWidget *btnBitFields, *btnFlags, *btnRegisters, *btnLocalRegisters, *btnTmpAndSavedStackRegisters;
    extern GtkWidget *chkHexaString;
    extern int16_t debugWidgetDx, debugWidgetDy;

    void  btnBitFieldsClicked          (GtkWidget* w ,gpointer data);
    void  btnFlagsClicked              (GtkWidget* w ,gpointer data);
    void  btnRegistersClicked          (GtkWidget* w ,gpointer data);
    void  btnLocalRegistersClicked     (GtkWidget* w ,gpointer data);
    void  btnStatisticalSumsClicked    (GtkWidget* w ,gpointer data);
    void  btnNamedVariablesClicked     (GtkWidget* w ,gpointer data);
    void  btnSavedStackRegistersClicked(GtkWidget* w ,gpointer data);
    void  chkHexaStringClicked         (GtkWidget* w ,gpointer data);

    /**
     * Refreshes the debug window
     */
    void  refreshDebugPanel            (void);

    /**
     * Returns the name of a display format
     *
     * \param[in] df Display format
     * \return Name of the display format
     */
    char *getDisplayFormatName               (uint16_t df);

    /**
     * Returns the name of a time format
     *
     * \param[in] tf Time format
     * \return Name of the time format
     */
    char *getTimeFormatName                  (bool tf);

    /**
     * Returns the name of a boolean
     *
     * \param[in] b Boolean
     * \return Name of the boolean
     */
    char *getBooleanName                     (bool b);

    /**
     * Returns the name of a RBR mode
     *
     * \param[in] mode
     * \return Name of the mode
     */
    char *getRbrModeName                     (uint16_t mode);

    /**
     * Returns the name of a rounding mode
     *
     * \param[in] rm Rounding mode
     * \return Name of the rounding mode
     */
    char *getRoundingModeName                (uint16_t rm);

    /**
     * Returns the name of a calc mode
     *
     * \param[in] cm Calc mode
     * \return Name of the calc mode
     */
    char *getCalcModeName                    (uint16_t cm);

    /**
     * Returns the name of a "next char indicator"
     *
     * \param[in] nc Next char indicator
     * \return Name of the "next char indicator"
     */
    char *getNextCharName                    (uint16_t nc);

    /**
     * Returns the name of a complex unit
     *
     * \param[in] cu Complex unit
     * \return Name of the complex unit
     */
    char *getComplexUnitName                 (bool cu);

    /**
     * Returns the name of a Product sign
     *
     * \param[in] ps Product sign
     * \return Name of the product sign
     */
    char *getProductSignName                 (bool ps);

    /**
     * Returns the name of a fraction type
     *
     * \param[in] ft Fraction type
     * \return Name of the fraction type
     */
    char *getFractionTypeName                (bool ft);

    /**
     * Returns the name of denominator mode 1 type
     *
     * \param[in] ft Fraction type
     * \return Name of the fraction type
     */
    char *getFractionDenom1ModeName          (bool ft);

    /**
     * Returns the name of denominator mode 2 type
     *
     * \param[in] ft Fraction type
     * \return Name of the fraction type
     */
    char *getFractionDenom2ModeName          (bool ft);

    /**
     * Returns the name of a radix mark
     *
     * \param[in] rm Radix mark
     * \return Name of the radix mark
     */
    char *getRadixMarkName                   (bool rm);

    /**
     * Returns the name of a display override mode
     *
     * \param[in] dio Display override mode
     * \return Name of the override mode
     */
    char *getDisplayOvrName                  (bool dio);

    /**
     * Returns the name of a stack size
     *
     * \param[in] ss Stack size
     * \return Name of the stack size
     */
    char *getStackSizeName                   (bool ss);

    /**
     * Returns the name of a complex mode
     *
     * \param[in] cm Complex mode
     * \return Name of the complex mode
     */
    char *getComplexModeName                 (bool cm);

    /**
     * Returns the name of an alpha case mode
     *
     * \param[in] ac Alphacase mode
     * \return Name of the alphacase mode
     */
    char *getAlphaCaseName                   (uint16_t ac);

    /**
     * Returns the name of an alpha selection menu
     *
     * \param[in] asm Alpha selection menu
     * \return Name of the alpha selection menu
     */
    char *getAlphaSelectionMenuName          (uint16_t alsm);

    char *getCursorFontName                  (uint16_t cf);

    /**
     * Returns the name of a system flag
     *
     * \param[in] sf flag
     * \return Name of the system flag
     */
    char *getSystemFlagName                  (uint16_t sf);

    void  memoryDump                         (bool bitFields, bool globalFlags, bool globalRegisters, bool localFlags, bool FIRSTLOCALREGISTERs, bool otherVars);
  #endif // (DEBUG_PANEL == 1)

  #if (DEBUG_PANEL == 1) || (DEBUG_REGISTER_L == 1) || (DEBUG_STAT >= 1)
    char *debuggerReal34                     (real34_t *real34);

    /**
     * Formats a real34 for the debug window
     *
     * \param[in] str String receiving the value
     * \param[in] real34 Value
     */
    void  formatReal34Debug                  (char *str, real34_t *real34);

    /**
     * Formats a real for the debug window
     *
     * \param[in] str String receiving the value
     * \param[in] real Value
     */
    void  formatRealDebug                    (char *str, real_t *real);

    /**
     * Formats a complex34 for the debug window
     *
     * \param[in] str String receiving the value
     * \param[in] x Value
     */
    void  formatComplex34Debug               (char *str, void *addr);
  #endif // (DEBUG_PANEL == 1) || (DEBUG_REGISTER_L == 1)  || (DEBUG_STAT >= 1)

  /**
   * Returns the name of a data type
   *
   * \param[in] dt Data type
   * \return Name of the data type
   */
  char *getDataTypeName                     (uint16_t dt, bool article, bool padWithBlanks);

  /**
   * Returns the name of a data type of a register
   *
   * \param[in] dt register
   * \return Name of the data type
   */
  char *getRegisterDataTypeName             (calcRegister_t regist, bool article, bool padWithBlanks);

  char *getRegisterTagName                  (calcRegister_t regist, bool padWithBlanks);

  /**
   * Returns the name of an integer mode
   *
   * \param[in] im Integer mode
   * \return Name of the integer mode
   */
  char *getShortIntegerModeName             (uint16_t im);

  /**
   * Returns the name of a angular mode
   *
   * \param[in] am Angular mode
   * \return Name of the angular mode
   */
  char *getAngularModeName                  (angularMode_t angularMode);

  /**
   * Returns the single name of a curvefitting mode, or ??? if multiple names are defined in bits
   *
   * \param[in] am curvefitting mode
   * \return Name of the curvefitting mode
   */
  char *getCurveFitModeName                 (uint16_t selection);

  /**
   * Returns all selected names of the curve fit types
   * \note that a single bit EXCLUDES a method
   *
   * \param[in] dt Data type
   * \return Name of the curvefit type
   */
  char *getCurveFitModeNames                (uint16_t selection);

  /**
   * Returns the formula of a curvefitting mode
   *
   * \param[in] am curvefitting mode
   * \return Formula of the curvefitting mode
   */
  char *getCurveFitModeFormula              (uint16_t selection);

  /**
   * Remove trailing spaces from the curvefitting mode name
   */
  char *eatSpacesEnd                        (const char * ss);

  /**
   * Remove spaces from the curvefitting mode name
   */
  char *eatSpacesMid                        (const char * ss);

  #if defined(PC_BUILD)
    void dumpScreenToConsole(void);
  #endif // PC_BUILD

  #if defined(PC_BUILD) || defined(TESTSUITE_BUILD)
    void testRegisters    (const char *text);
    void memoryDump2      (const char *text);
  #endif // PC_BUILD || TESTSUITE_BUILD

#endif // !DEBUG_H
