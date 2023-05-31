// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file programming/flash.h
 */
#if !defined(FLASH_H)
  #define FLASH_H

  #include <stdint.h>
  #include <stdbool.h>

  void initFlashPgmLibrary         (void);
  void scanFlashPgmLibrary         (void);
  void addToFlashPgmLibrary        (void);
  void deleteFromFlashPgmLibrary   (uint32_t fromAddr, uint32_t toAddr);
  void readStepInFlashPgmLibrary   (uint8_t *buffer, uint16_t bufferSize, uint32_t pointer);
  void executeStepInFlashPgmLibrary(void);
  
  void _addSpaceAfterPrograms      (uint16_t sizeInBytes);
  bool _addEndNeeded               (void);
  
#endif // !FLASH_H
