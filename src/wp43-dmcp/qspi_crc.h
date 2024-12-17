// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

// This file is used in the first pass of the DMCP build so that the real QSPI
// size can be determined

#ifdef USE_GEN_QSPI_CRC
 #ifdef XPB
  #include "generated_qspiX_crc.h"
 #else
  #include "generated_qspi_crc.h"
 #endif
#else
  #define QSPI_DATA_SIZE 82620
  #define QSPI_DATA_CRC 0x000CFED6
#endif
