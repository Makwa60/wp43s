// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file ui/bufferize.h
 */
#if !defined(BUFFERIZE_H)
  #define BUFFERIZE_H

  #include <stdint.h>
  #include "realType.h"

  #if !defined(TESTSUITE_BUILD)
    void     cbAsmActive              (uint16_t param);

    void     fnAim                    (uint16_t unusedButMandatoryParameter);
    void     resetAlphaSelectionBuffer(void);
    uint16_t convertItemToSubOrSup    (uint16_t item, int16_t subOrSup);

    /**
     * Adds an item to the alpha buffer.
     *
     * \param[in] item item to add to the buffer
     */
    void     addItemToBuffer          (uint16_t item);

    void     addItemToNimBuffer       (int16_t item);
    void     closeNimWithFraction     (real34_t *dest);
    void     closeNimWithComplex      (real34_t *dest_r, real34_t *dest_i);
    void     closeNim                 (void);
    void     closeAim                 (void);
    void     nimBufferToDisplayBuffer (const char *buffer, char *displayBuffer);
  #else
    #pragma GCC diagnostic ignored "-Wunused-parameter"

    static inline void cbAsmActive(uint16_t param) {}
  #endif // !TESTSUITE_BUILD

#endif // !BUFFERIZE_H
