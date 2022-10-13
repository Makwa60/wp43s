// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "hal/io.h"

#include <assert.h>
#include <stdio.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

static FILE *_ioFileHandle = NULL;

bool ioFileOpen(ioFilePath_t path, ioFileMode_t mode) {
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
