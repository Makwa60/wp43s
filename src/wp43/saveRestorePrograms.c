// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "saveRestoreCalcState.h"
#include "saveRestorePrograms.h"

#include "apps/apps.h"
#include "apps/bugScreen.h"
#include "apps/flagBrowser.h"
#include "apps/fontBrowser.h"
#include "apps/registerBrowser.h"
#include "apps/timerApp.h"
#include "assign.h"
#include "calcMode.h"
#include "charString.h"
#include "config.h"
#include "core/memory.h"
#include "display.h"
#include "error.h"
#include "flags.h"
#include "flags.h"
#include "hal/gui.h"
#include "hal/io.h"
#include "items.h"
#include "mathematics/matrix.h"
#include "plotstat.h"
#include "programming/flash.h"
#include "programming/lblGtoXeq.h"
#include "programming/manage.h"
#include "programming/nextStep.h"
#include "registers.h"
#include "registerValueConversions.h"
#include "solver/equation.h"
#include "solver/graph.h"
#include "sort.h"
#include "stats.h"
#include "ui/cursor.h"
#include "ui/screen.h"
#include "ui/softmenus.h"
#include "ui/tam.h"
#if defined(DMCP_BUILD)
  #include <dmcp.h>
  #include <wp43-dmcp.h>
#endif // DMCP_BUILD
#include <stdbool.h>
#include <string.h>

#include "wp43.h"

#define PROGRAM_VERSION                     01  // Original version
#define OLDEST_COMPATIBLE_PROGRAM_VERSION   01  // Original version
#define BACKUP_FORMAT                       00  // Same program format as in backup file
#define TEXT_FORMAT                         01  // Text program format - for future use 

// Structure of the program file.
// Format: ASCII
// Each line after line 6 contains the decimal value of a program byte
//
//  +-----+---------------------------+
//  | Line|         Content           |
//  +-----+---------------------------+
//  |  1  |   "PROGRAM_FILE_FORMAT"   |
//  |  2  |       <file format>       |
//  |  3  |"WP43_program_file_version"|
//  |  4  |   <program file version>  |
//  |  5  |         "PROGRAM"         |
//  |  6  |       <program size>      |
//  |  7  |    <first program byte>   |
//  | ... |            ...            |
//  |  n  |    <last program byte>    |
//  +-----+---------------------------+
//


void fnSaveProgram(uint16_t label) {
#if !defined(TESTSUITE_BUILD)
  uint32_t programVersion = PROGRAM_VERSION;
  ioFilePath_t path;
//  char tmpString[3000];             //The concurrent use of the global tmpString 
//                                  //as target does not work while the source is at
//                                  //tmpRegisterString = tmpString + START_REGISTER_VALUE;
//                                  //Temporary solution is to use a local variable of sufficient length for the target.
  uint32_t i;
 
#if defined(DMCP_BUILD)
  // Don't pass through if the power is insufficient  
  if ( power_check_screen() ) return;
#endif
 
  // Find program boundaries
  const uint16_t savedCurrentLocalStepNumber = currentLocalStepNumber;
  uint16_t savedCurrentProgramNumber = currentProgramNumber;
  // no argument – need to save current program
  if(label == 0 && !tam.alpha && tam.digitsSoFar == 0) {
  }
  // Existing global label
  else if(label >= FIRST_LABEL && label <= LAST_LABEL) {        
    fnGoto(label);
  }
  // Invalid label
  else {                                      
    displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "label %" PRIu16 " is not a global label", label);
      moreInfoOnError("In function fnSaveProgram:", errorMessage, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    return;
  }
  
  // program in flash memory : return without saving
  if(programList[currentProgramNumber - 1].step < 0) { // flash memory
    displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
    return;
  }

  path = ioPathSaveProgram;

  if(!ioFileOpen(path, ioModeWrite)) {
    #if !defined(DMCP_BUILD)
      printf("Cannot save program!\n");
    #endif
	displayCalcErrorMessage(ERROR_CANNOT_ACCESS_FILE, ERR_REGISTER_LINE, REGISTER_X);
    return;
  }

  // PROGRAM file version
  sprintf(tmpString, "PROGRAM_FILE_FORMAT\n%" PRIu8 "\n", (uint8_t)BACKUP_FORMAT);
  ioFileWrite(tmpString, strlen(tmpString));
  sprintf(tmpString, "WP43_program_file_version\n%" PRIu32 "\n", (uint32_t)programVersion);
  ioFileWrite(tmpString, strlen(tmpString));

  // Program
  size_t currentSizeInBytes = endOfCurrentProgram.ram - ((currentProgramNumber == (numberOfPrograms - numberOfProgramsInFlash)) ? 2 : 0) - beginOfCurrentProgram.ram;
  sprintf(tmpString, "PROGRAM\n%" PRIu32 "\n", (uint32_t)currentSizeInBytes);
  ioFileWrite(tmpString, strlen(tmpString));

  // Save program bytes
  for(i=0; i<currentSizeInBytes; i++) {
    sprintf(tmpString, "%" PRIu8 "\n", beginOfCurrentProgram.ram[i]);
    ioFileWrite(tmpString, strlen(tmpString));
  }
  // If last program in memory then add .END. statement
  if (currentProgramNumber == (numberOfPrograms - numberOfProgramsInFlash)) {
    sprintf(tmpString, "255\n255\n");
    ioFileWrite(tmpString, strlen(tmpString));
  } 

  ioFileClose();

  currentLocalStepNumber = savedCurrentLocalStepNumber;
  currentProgramNumber = savedCurrentProgramNumber;

  temporaryInformation = TI_SAVED;
#endif // !TESTSUITE_BUILD
}


void fnLoadProgram(uint16_t unusedButMandatoryParameter) {
  ioFilePath_t path;
  uint32_t pgmSizeInByte;
  uint32_t i;
  uint8_t *startOfProgram;

  path = ioPathLoadProgram;

  if(!ioFileOpen(path, ioModeRead)) {
    displayCalcErrorMessage(ERROR_CANNOT_ACCESS_FILE, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot find or read backup data file wp43.sav");
    return;
  }
  
  //Check save file version
  uint32_t loadedVersion = 0;
  readLine(tmpString);
  if(strcmp(tmpString, "PROGRAM_FILE_FORMAT") == 0) {
    readLine(aimBuffer); // Format of program instructions (ignore now, there is only one format)
  } else {
    return;
  } 
  readLine(aimBuffer); // param
  readLine(tmpString); // value
  if(strcmp(aimBuffer, "WP43_program_file_version") == 0) {
    loadedVersion = stringToUint32(tmpString);
  } else {
    return;
  } 
  readLine(aimBuffer); // param
  readLine(tmpString); // value
  if(strcmp(aimBuffer, "PROGRAM") == 0) {
    pgmSizeInByte = stringToUint32(tmpString);
  } else {
    return;
  } 

  if(_addEndNeeded()) {
    _addSpaceAfterPrograms(2);
    *(firstFreeProgramByte - 2) = (ITM_END >> 8) | 0x80;
    *(firstFreeProgramByte - 1) =  ITM_END       & 0xff;
    *(firstFreeProgramByte    ) = 0xffu;
    *(firstFreeProgramByte + 1) = 0xffu;
    scanLabelsAndPrograms();
  }

  _addSpaceAfterPrograms(pgmSizeInByte);
  startOfProgram = firstFreeProgramByte - pgmSizeInByte;
  for(i=0; i<pgmSizeInByte; i++) {
    readLine(tmpString); // One byte
    startOfProgram[i] = stringToUint8(tmpString);
  }
 
  *(firstFreeProgramByte    ) = 0xffu;
  *(firstFreeProgramByte + 1) = 0xffu;
  scanLabelsAndPrograms();
  
  ioFileClose();
  
  if(loadedVersion < OLDEST_COMPATIBLE_PROGRAM_VERSION) { // Program incompatibility
    sprintf(tmpString," \n"
                      "   !!! Program version is too old !!!\n"
                      "Not compatible with current version\n"
                      " \n"
                      "It will not be loaded.");
    #if !defined(TESTSUITE_BUILD)
      show_warning(tmpString);
    #endif // TESTSUITE_BUILD
	return;
  }

  temporaryInformation = TI_PROGRAM_LOADED;
}
