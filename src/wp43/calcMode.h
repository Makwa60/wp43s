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
 * \file calcMode.h
 */
#if !defined(CALCMODE_H)
  #define CALCMODE_H

  #include "defines.h"
  #include "typeDefinitions.h"
  #include <stdint.h>

  /**
   * \enum calcMode_t
   * Calculator mode (5 bits).
   */
  typedef enum {
    cmNormal           = 0,  // Normal operation
    cmAim              = 1,  // Alpha input mode
    cmNim              = 2,  // Numeric input mode
    cmPem              = 3,  // Program entry mode
    cmAssign           = 4,  // Assign mode
    cmRegisterBrowser  = 5,  // Register browser
    cmFlagBrowser      = 6,  // Flag browser
    cmFontBrowser      = 7,  // Font browser
    cmPlotStat         = 8,  // Plot stats mode
    cmErrorMessage     = 9,  // Error message in one of the register lines
    cmBugOnScreen      = 10, // Bug message on screen
    cmConfirmation     = 11, // Waiting for confirmation or canceling
    cmMim              = 12, // Matrix imput mode tbd reorder
    cmEim              = 13, // Equation imput mode
    cmTimerApp         = 14, // Timer application
    cmGraph            = 15, // Plot graph mode
    cmNoUndo           = 16, // Running functions without undo affected
    cmApp              = 17
  } calcMode_t;

  extern calcMode_t calcMode;

  #if !defined(TESTSUITE_BUILD)
    void fnOff                       (uint16_t unsuedParamButMandatory);

    /**
     * Sets the calc mode.
     *
     * \param[in] newMode new mode
     */
    void calcModeEnter               (calcMode_t newMode);

    /**
     * Leaves the calc mode.
     */
    void calcModeLeave               (void);

    #if !defined(DMCP_BUILD)
      /**
       * Updates the GUI layout based on the calcMode
       */
      void calcModeUpdateGui           (void);
    #else
      static inline void calcModeUpdateGui (void) {}
    #endif // !DMCP_BUILD DMCP_BUILD

    /**
     * Sets the calc mode to alpha selection menu if needed.
     */
    void enterAsmModeIfMenuIsACatalog(int16_t id);

    /**
     * Leaves the alpha selection mode.
     */
    void leaveAsmMode                (void);
  #else // TESTSUITE_BUILD
    #pragma GCC diagnostic ignored "-Wunused-parameter"

    static inline void calcModeEnter               (calcMode_t newMode) {}
    static inline void calcModeLeave               (void) {}
  #endif // !TESTSUITE_BUILD
#endif // !CALCMODE_H
