// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file core/memory.h
 */
#if !defined(MEMORY_H)
  #define MEMORY_H

  #include <stdbool.h>
  #include <stdint.h>
  #include <stddef.h>

  // The 6 followoing functions are only there to know who allocates and frees memory
  void    *allocWp43            (size_t sizeInBytes);
  void    *reallocWp43          (void *pcMemPtr, size_t oldSizeInBytes, size_t newSizeInBytes);
  void    freeWp43              (void *pcMemPtr, size_t sizeInBytes);

  void    *allocGmp             (size_t sizeInBytes);
  void    *reallocGmp           (void *pcMemPtr, size_t oldSizeInBytes, size_t newSizeInBytes);
  void    freeGmp               (void *pcMemPtr, size_t sizeInBytes);

  int32_t getFreeRamMemory      (void);
  void    resizeProgramMemory   (size_t newSizeInBytes);
  bool    isMemoryBlockAvailable(size_t sizeInBytes);

  #if !defined(DMCP_BUILD)
    void    debugMemory         (const char *message);
  #endif // !DMCP_BUILD

  #if defined(PC_BUILD)
    void ramDump(void);
  #endif // PC_BUILD

  // The following macros are for avoid crash in case that the memory is full. The corresponding label `cleanup_***` is needed AFTER freeing the memory.
  #define checkedAllocate2(var, sizeInBytes, label) var = allocWp43(sizeInBytes); if(!var) {displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE); goto label;}
  #define checkedAllocate(var, sizeInBytes) checkedAllocate2(var, sizeInBytes, cleanup_##var)

#endif // !MEMORY_H
