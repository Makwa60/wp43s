// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "hal/io.h"

#include "dateTime.h"
#include "defines.h"
#include "wp43-gtk.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

static FILE *_ioFileHandle = NULL;

int file_selection_screen(const char * title, const char * base_dir, const char * ext, int disp_save, int overwrite_check, void * data) {
      GtkFileChooserNative *native;
      gint res;

	  if (disp_save) {
		GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
        native = gtk_file_chooser_native_new (title,
                                              GTK_WINDOW(frmCalc),
                                              action,
                                              "_Save",
                                              "_Cancel");
      } else{
		GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
        native = gtk_file_chooser_native_new (title,
                                              GTK_WINDOW(frmCalc),
                                              action,
                                              "_Load",
                                              "_Cancel");
	  }
	  	  
      GtkFileChooser *chooser = GTK_FILE_CHOOSER (native);

	  if (overwrite_check) {
		  gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);
	  }
	  
      gtk_file_chooser_set_current_folder(chooser,base_dir);
	  gtk_file_chooser_set_current_name (chooser,"untitled"STATE_EXT);
	  GtkFileFilter *filter = gtk_file_filter_new ();
      gtk_file_filter_add_pattern (filter, ext);
	  gtk_file_chooser_add_filter(chooser, filter);
      res = gtk_native_dialog_run (GTK_NATIVE_DIALOG (native));
      if (res == GTK_RESPONSE_ACCEPT)
      {
		char *filename;
        filename = gtk_file_chooser_get_filename (chooser);
		strcpy(data, filename);
		if (disp_save) {
		  char * fe = data+strlen(filename)-4;
		  const char * ee = ext+1;
		  if (strcmp(fe,ee) != 0) strcat(data,ee);     //filename doesn't have the expected extension 
        }		
        g_free(filename);	
        g_object_unref (native);
        return 1;
      } else { 
        g_object_unref (native);
        return 0;
	  }	
}
						  

const char *_ioFileNameFromFilePath(ioFilePath_t path) {
  static char tmpFileName[40];
  char * base_dir;
  int ret = 0;
  switch(path) {
    case ioPathSaveFile:
      return "wp43.sav";
    case ioPathPgmFile:
      return "wp43.dat";
    case ioPathTestPgms:
      return BASEPATH "res/dmcp/testPgms.bin";
    case ioPathBackup:
      return "backup.bin";
    case ioPathRegDump:
      strcpy(tmpFileName, "regx-");
      getTimeStampString(tmpFileName + strlen(tmpFileName));
      strcat(tmpFileName, ".tsv");
      return tmpFileName;
    case ioPathSaveStateFile:
    case ioPathLoadStateFile:
	  base_dir = g_get_current_dir();
	  if (path == ioPathSaveStateFile) {
	    ret = file_selection_screen("Save State File", base_dir, "*"STATE_EXT, 1, 1, tmpFileName);
      } else if (path == ioPathLoadStateFile) {
	    ret = file_selection_screen("Load State File", base_dir, "*"STATE_EXT, 0, 0, tmpFileName);
      }
	  g_free(base_dir);
	  if (ret == 0) {
		return 0;
	  } else { 
        return tmpFileName;
	  }
    default:
      return 0;
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
