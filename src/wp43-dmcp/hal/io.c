// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "hal/io.h"

#include "typeDefinitions.h"

#include <assert.h>
#include <stdio.h>
#include <dmcp.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

static bool _ioWriteEnabled = false;
static bool _ioReadEnabled  = false;

bool ioFileOpen(ioFilePath_t path, ioFileMode_t mode) {
  assert(!_ioWriteEnabled && !_ioReadEnabled);
  const TCHAR *filename;
  BYTE filemode;
  switch(path) {
    case ioPathSaveFile:
      filename = "SAVFILES\\wp43.sav";
      break;
    case ioPathPgmFile:
      filename = "LIBRARY\\wp43.dat";
      break;
    case ioPathTestPgms:
      filename = "testPgms.bin";
      break;
    default:
      return 0;
  }
  switch(mode) {
    case ioModeRead:
      filemode = FA_READ;
      _ioReadEnabled = true;
      break;
    case ioModeWrite:
      filemode = FA_CREATE_ALWAYS | FA_WRITE;
      _ioWriteEnabled = true;
      break;
    case ioModeUpdate:
      filemode = FA_READ | FA_WRITE | FA_OPEN_EXISTING;
      _ioWriteEnabled = true;
      _ioReadEnabled = true;
      break;
    default:
      return 0;
  }
  if(mode != ioModeRead) {
    sys_disk_write_enable(1);
  }
  if(mode == ioModeWrite) {
    if(path == ioPathSaveFile) {
      check_create_dir("SAVFILES");
    }
    if(path == ioPathPgmFile) {
      check_create_dir("LIBRARY");
    }
  }
  FRESULT result = f_open(ppgm_fp, filename, filemode);
  if(result != FR_OK) {
    if(mode != ioModeRead) {
      sys_disk_write_enable(0);
    }
  }
  return (result == FR_OK);
}



void ioFileWrite(const void *buffer, uint32_t size) {
  assert(_ioWriteEnabled);
  UINT bytesWritten;
  f_write(ppgm_fp, buffer, size, &bytesWritten);
}



uint32_t ioFileRead(void *buffer, uint32_t size) {
  assert(_ioReadEnabled);
  UINT bytesRead;
  f_read(ppgm_fp, buffer, size, &bytesRead);
  return bytesRead;
}



void ioFileSeek(uint32_t position) {
  assert(_ioWriteEnabled || _ioReadEnabled);
  f_lseek(ppgm_fp, position);
}



void ioFileClose(void) {
  assert(_ioWriteEnabled || _ioReadEnabled);
  f_close(ppgm_fp);
  if(_ioWriteEnabled) {
    sys_disk_write_enable(0);
  }
  _ioWriteEnabled = false;
  _ioReadEnabled = false;
}
