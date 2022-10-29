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
