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
 * \file hal/io.h
 */
#if !defined(IO_H)
  #define IO_H

  #include <stdint.h>

  typedef void ioFile_t;
  typedef enum {
    IOPATH_SAVEFILE   = 0,
    IOPATH_PGMFILE    = 1,
    IOPATH_TESTPGMS   = 2,
    IOPATH_BACKUP     = 3
  } ioFilePath_t;
  typedef enum {
    IOMODE_READ   = 0,
    IOMODE_WRITE  = 1,
    IOMODE_UPDATE = 2
  } ioFileMode_t;

  ioFile_t *ioFileOpen(ioFilePath_t path, ioFileMode_t mode);
  void ioFileWrite(ioFile_t *file, const void *buffer, uint32_t size);
  uint32_t ioFileRead(ioFile_t *file, void *buffer, uint32_t size);
  void ioFileClose(ioFile_t *file);

#endif // IO_H
