// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file programming/decode.h
 */
#if !defined(DECODE_H)
  #define DECODE_H

  #include "typeDefinitions.h"
  #include <stdint.h>

  void decodeOneStep          (pgmPtr_t step);
  void decodeOneStep_ram      (uint8_t *step);
  #if !defined(DMCP_BUILD)
    void listPrograms         (void);
    void listLabelsAndPrograms(void);
  #endif // !DMCP_BUILD

#endif // !DECODE_H
