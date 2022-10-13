// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

// This file is included by dep/DMCP_SDK/dmcp/sys/pgm_syscalls.c
// It is used to set values in a struct that is read by the DMCP
// The file name must be main.h for this to be included

/**
 * \file main.h
 */
#if !defined(MAIN_H)
  #define MAIN_H

  #include "version.h"

  #define PROGRAM_NAME    "WP43"
  #define PROGRAM_VERSION VERSION_SHORT

#endif // !MAIN_H
