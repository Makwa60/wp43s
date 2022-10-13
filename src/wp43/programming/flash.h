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
