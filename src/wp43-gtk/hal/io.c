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

#include <assert.h>
#include <stdio.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

static FILE *_ioFileHandle = NULL;

bool_t ioFileOpen(ioFilePath_t path, ioFileMode_t mode) {
  assert(_ioFileHandle == NULL);
  const char *filename, *filemode;
  switch(path) {
    case ioPathSaveFile:
      filename = "wp43.sav";
      break;
    case ioPathPgmFile:
      filename = "wp43.dat";
      break;
    case ioPathTestPgms:
      filename = "res/dmcp/testPgms.bin";
      break;
    case ioPathBackup:
      filename = "backup.bin";
      break;
    default:
      return 0;
  }
  switch(mode) {
    case ioModeRead:
      filemode = "rb";
      break;
    case ioModeWrite:
      filemode = "wb";
      break;
    case ioModeUpdate:
      filemode = "r+b";
      break;
    default:
      return 0;
  }
  _ioFileHandle = fopen(filename, filemode);
  return (_ioFileHandle != NULL);
}



void ioFileWrite(const void *buffer, uint32_t size) {
  assert(_ioFileHandle != NULL);
  fwrite(buffer, 1, size, _ioFileHandle);
}



uint32_t ioFileRead(void *buffer, uint32_t size) {
  assert(_ioFileHandle != NULL);
  return fread(buffer, 1, size, _ioFileHandle);
}



void ioFileSeek(uint32_t position) {
  assert(_ioFileHandle != NULL);
  fseek(_ioFileHandle, position, SEEK_SET);
}



void ioFileClose(void) {
  assert(_ioFileHandle != NULL);
  fclose(_ioFileHandle);
  _ioFileHandle = NULL;
}
