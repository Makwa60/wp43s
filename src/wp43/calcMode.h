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
