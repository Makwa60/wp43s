// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

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
    cmPlotStat         = 5,  // Plot stats mode
    cmErrorMessage     = 6,  // Error message in one of the register lines
    cmConfirmation     = 7,  // Waiting for confirmation or canceling
    cmMim              = 8,  // Matrix imput mode tbd reorder
    cmEim              = 9,  // Equation imput mode
    cmTimerApp         = 10, // Timer application
    cmGraph            = 11, // Plot graph mode
    cmNoUndo           = 12, // Running functions without undo affected
    cmApp              = 13  // App mode
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
