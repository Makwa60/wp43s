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

#include "hal/io.h"

#include <stdio.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

ioFile_t *ioFileOpen(ioFilePath_t path, ioFileMode_t mode) {
  const char *filename, *filemode;
  switch(path) {
    case IOPATH_SAVEFILE:
      filename = "wp43.sav";
      break;
    case IOPATH_PGMFILE:
      filename = "wp43.dat";
      break;
    case IOPATH_TESTPGMS:
      filename = "res/dmcp/testPgms.bin";
      break;
    case IOPATH_BACKUP:
      filename = "backup.bin";
      break;
    default:
      return 0;
  }
  switch(mode) {
    case IOMODE_READ:
      filemode = "rb";
      break;
    case IOMODE_WRITE:
      filemode = "wb";
      break;
    case IOMODE_UPDATE:
      filemode = "r+b";
      break;
    default:
      return 0;
  }
  return fopen(filename, filemode);
}



void ioFileWrite(ioFile_t *file, const void *buffer, uint32_t size) {
  fwrite(buffer, 1, size, file);
}



uint32_t ioFileRead(ioFile_t *file, void *buffer, uint32_t size) {
  return fread(buffer, 1, size, file);
}



void ioFileClose(ioFile_t *file) {
  fclose(file);
}
