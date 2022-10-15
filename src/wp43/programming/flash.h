// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file programming/flash.h
 */
#if !defined(FLASH_H)
  #define FLASH_H

  #include <stdint.h>

  void fnPSto                      (uint16_t unusedButMandatoryParameter);
  void fnPRcl                      (uint16_t unusedButMandatoryParameter);

  void initFlashPgmLibrary         (void);
  void scanFlashPgmLibrary         (void);
  void addToFlashPgmLibrary        (void);
  void deleteFromFlashPgmLibrary   (uint32_t fromAddr, uint32_t toAddr);
  void readStepInFlashPgmLibrary   (uint8_t *buffer, uint16_t bufferSize, uint32_t pointer);
  void executeStepInFlashPgmLibrary(void);

#endif // !FLASH_H
