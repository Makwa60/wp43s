// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file wp43-dmcp.h
 */
#if !defined(WP43_DMCP_H)
  #define WP43_DMCP_H

  #include <stdint.h>

  extern uint32_t nextTimerRefresh;

  int  convertKeyCode       (int key);
  void dmcpResetAutoOff     (void);
  void dmcpCheckPowerStatus (void);

#endif // WP43_DMCP_H
