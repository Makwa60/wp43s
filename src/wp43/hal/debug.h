// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file hal/debug.h
 */
#if !defined(HAL_DEBUG_H)
  #define HAL_DEBUG_H

  #include "defines.h"

  #if (DEBUG_PANEL == 1)
    void debugInit(void);
  #endif

#endif // !HAL_DEBUG_H
