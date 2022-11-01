// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "hal/io.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

static FILE *_ioFileHandle = NULL;

const char *_ioFileNameFromFilePath(ioFilePath_t path) {
  switch(path) {
    case ioPathSaveFile:
      return "wp43.sav";
    case ioPathPgmFile:
      return "wp43.dat";
    case ioPathTestPgms:
      return "res/dmcp/testPgms.bin";
    case ioPathBackup:
      return "backup.bin";
    default:
      return false;
  }
}



bool ioFileOpen(ioFilePath_t path, ioFileMode_t mode) {
  assert(_ioFileHandle == NULL);
  const char *filemode;
  const char *filename = _ioFileNameFromFilePath(path);
  if(!filename) {
    return false;
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
      return false;
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



bool ioFileRemove(ioFilePath_t path, uint32_t *errorNumber) {
  assert(_ioFileHandle == NULL);
  const char *filename = _ioFileNameFromFilePath(path);
  if(!filename) {
    return false;
  }
  int result = remove(filename);
  if(result == -1 && errorNumber != NULL) {
    *errorNumber = errno;
  }
  return result != -1;
}
