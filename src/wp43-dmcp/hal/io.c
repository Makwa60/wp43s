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
#include <dmcp.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

static uint8_t _ioFilesOpen = 0;
static uint8_t _ioWriteEnabled = 0;

// This approach is probably overly complicated because I don't think
// we have more than one file open at a time, so this could be
// significantly simplified. If so, an assertion should be added.
ioFile_t *ioFileOpen(ioFilePath_t path, ioFileMode_t mode) {
  FIL *handle = 0;
  const TCHAR *filename;
  BYTE filemode;
  uint8_t startedWrite = 0;
  switch(path) {
    case IOPATH_SAVEFILE:
      filename = "SAVFILES\\wp43.sav";
      break;
    case IOPATH_PGMFILE:
      filename = "LIBRARY\\wp43.dat";
      break;
    case IOPATH_TESTPGMS:
      filename = "testPgms.bin";
      break;
    default:
      return 0;
  }
  switch(mode) {
    case IOMODE_READ:
      filemode = FA_READ;
      break;
    case IOMODE_WRITE:
      filemode = FA_CREATE_ALWAYS | FA_WRITE;
      break;
    case IOMODE_UPDATE:
      filemode = FA_READ | FA_WRITE | FA_OPEN_EXISTING;
      break;
    default:
      return 0;
  }
  if(mode != IOMODE_READ && !_ioWriteEnabled) {
    startedWrite = 1;
    sys_disk_write_enable(1);
  }
  if(mode == IOMODE_WRITE) {
    if(path == IOPATH_SAVEFILE) {
      check_create_dir("SAVFILES");
    }
    if(path == IOPATH_PGMFILE) {
      check_create_dir("LIBRARY");
    }
  }
  FRESULT result = f_open(handle, filename, filemode);
  if(result != FR_OK) {
    if(startedWrite) {
      sys_disk_write_enable(0);
    }
    return 0;
  }
  _ioFilesOpen++;
  if(startedWrite) {
    _ioWriteEnabled = 1;
  }
  return handle;
}



void ioFileWrite(ioFile_t *file, const void *buffer, uint32_t size) {
  UINT bytesWritten;
  f_write(file, buffer, size, &bytesWritten);
}



uint32_t ioFileRead(ioFile_t *file, void *buffer, uint32_t size) {
  UINT bytesRead;
  f_read(file, buffer, size, &bytesRead);
  return bytesRead;
}



void ioFileClose(ioFile_t *file) {
  f_close(file);
  _ioFilesOpen--;
  if(_ioWriteEnabled && !_ioFilesOpen) {
    _ioWriteEnabled = 0;
    sys_disk_write_enable(0);
  }
}
