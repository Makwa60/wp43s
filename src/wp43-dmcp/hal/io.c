// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "hal/io.h"

#include "dateTime.h"
#include "typeDefinitions.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <dmcp.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

static bool _ioWriteEnabled = false;
static bool _ioReadEnabled  = false;

const char *_ioFileNameFromFilePath(ioFilePath_t path) {
  static char tmpFileName[40];
  int ret = 0;
  switch(path) {
    case ioPathSaveFile:
      return "SAVFILES\\wp43.sav";
    case ioPathPgmFile:
      return "LIBRARY\\wp43.dat";
    case ioPathTestPgms:
      return "testPgms.bin";
    case ioPathRegDump:
      strcpy(tmpFileName, "SAVFILES\\regx-");
      getTimeStampString(tmpFileName + strlen(tmpFileName));
      strcat(tmpFileName, ".tsv");
      return tmpFileName;
    case ioPathSaveStateFile:
      ret = file_selection_screen("Save Calculator State", STATE_DIR, STATE_EXT, save_statefile, 1, 1, tmpFileName);
      if (ret == MRET_EXIT) {
        return 0;
      } else { 
        return tmpFileName;
      }
   case ioPathLoadStateFile:
      ret = file_selection_screen("Load Calculator State", STATE_DIR, STATE_EXT, load_statefile, 0, 0, tmpFileName);
      if (ret == MRET_EXIT) {
        return 0;
      } else { 
        return tmpFileName;
      }
   case ioPathSaveProgram:
      ret = file_selection_screen("Save Program", PROGRAMS_DIR, PRGM_EXT, save_programfile, 1, 1, tmpFileName);
      if (ret == MRET_EXIT) {
        return 0;
      } else { 
        return tmpFileName;
      }
   case ioPathLoadProgram:
      ret = file_selection_screen("Load Program", PROGRAMS_DIR, PRGM_EXT, load_programfile, 0, 0, tmpFileName);
      if (ret == MRET_EXIT) {
        return 0;
      } else { 
        return tmpFileName;
      }
  default:
      return 0;
  }
}



bool ioFileOpen(ioFilePath_t path, ioFileMode_t mode) {
  assert(!_ioWriteEnabled && !_ioReadEnabled);
  const TCHAR *filename = _ioFileNameFromFilePath(path);
  if(!filename) {
    return false;
  }
  BYTE filemode;
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
      return false;
  }
  if(mode != ioModeRead) {
    sys_disk_write_enable(1);
  }
  if(mode == ioModeWrite) {
    if(path == ioPathSaveFile || path == ioPathRegDump) {
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
    _ioWriteEnabled = false;
    _ioReadEnabled  = false;
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



bool ioFileRemove(ioFilePath_t path, uint32_t *errorNumber) {
  assert(!_ioWriteEnabled && !_ioReadEnabled);
  FRESULT result;
  sys_disk_write_enable(1);
  result = f_unlink(_ioFileNameFromFilePath(path));
  if(result != FR_OK && errorNumber != NULL) {
    *errorNumber = result;
  }
  sys_disk_write_enable(0);
  return result == FR_OK;
}

//
int save_statefile(const char * fpath, const char * fname, void * data) {

  lcd_puts(t24,"Saving state ...");
  lcd_puts(t24, fname);  lcd_refresh();

  // Store the state file name
  strcpy(data, fpath);
  set_reset_state_file(fpath);

  // Exit with appropriate code to save state file save
  return MRET_SAVESTATE;
}

int load_statefile(const char * fpath, const char * fname, void * data) {

  // 'Sure' dialog
  lcd_puts(t24, "");
  lcd_puts(t24, "WARNING: Current calculator state");
  lcd_puts(t24, "will be lost.");
  lcd_puts(t24, "");
  lcd_puts(t24, "");
  //lcd_puts(t24, "Are you sure to load this file?");
  lcd_puts(t24, "Press [ENTER] to confirm.");
  lcd_refresh();
  
  wait_for_key_release(-1);

  for(;;) {
    int k1 = runner_get_key(NULL);
    if ( IS_EXIT_KEY(k1) )
      return 0; // Continue the selection screen
    if ( is_menu_auto_off() )
      return MRET_EXIT; // Leave selection screen
    if ( k1 == KEY_ENTER )
      break; // Proceed with load
  }

  lcd_putsRAt(t24, 6, "  Loading ...");
  lcd_refresh_wait();

  // Store the state file name
  strcpy(data, fpath);
  set_reset_state_file(fpath);

  // Exit with appropriate code to load state file
  return MRET_LOADSTATE;
}

int save_programfile(const char * fpath, const char * fname, void * data) {

  lcd_puts(t24,"Saving program ...");
  lcd_puts(t24, fname);  lcd_refresh();

  // Store the program file name
  strcpy(data, fpath);

  // Exit with appropriate code to save state file save
  return MRET_SAVESTATE;
}

int load_programfile(const char * fpath, const char * fname, void * data) {

  lcd_putsRAt(t24, 6, "  Loading ...");
  lcd_refresh_wait();

  // Store the program file name
  strcpy(data, fpath);

  // Exit with appropriate code to load state file
  return MRET_LOADSTATE;
}