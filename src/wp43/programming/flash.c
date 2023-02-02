// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "programming/flash.h"

#include "charString.h"
#include "config.h"
#include "core/memory.h"
#include "defines.h"
#include "error.h"
#include "hal/io.h"
#include "items.h"
#include "programming/manage.h"
#include "programming/nextStep.h"
#include "sort.h"
#include <string.h>
#include <stdlib.h>

#include "wp43.h"

static void save(const void *buffer, uint32_t size) {
  ioFileWrite(buffer, size);
}



static uint32_t load(void *buffer, uint32_t size) {
  return ioFileRead(buffer, size);
}



static void _addSpaceAfterPrograms(uint16_t sizeInBytes) {
  if(freeProgramBytes < sizeInBytes) {
    uint8_t *oldBeginOfProgramMemory = beginOfProgramMemory;
    uint32_t programSizeInBytes = TO_BYTES(RAM_SIZE_IN_BLOCKS - freeMemoryRegions[numberOfFreeMemoryRegions - 1].address - freeMemoryRegions[numberOfFreeMemoryRegions - 1].sizeInBlocks);
    uint32_t newProgramSizeInBytes = TO_BYTES(TO_BLOCKS(programSizeInBytes - freeProgramBytes + sizeInBytes));
    freeProgramBytes      += newProgramSizeInBytes - programSizeInBytes;
    resizeProgramMemory(newProgramSizeInBytes);
    if(programList[currentProgramNumber - 1].step > 0) { // RAM
      currentStep.ram           = currentStep.ram           - oldBeginOfProgramMemory + beginOfProgramMemory;
      firstDisplayedStep.ram    = firstDisplayedStep.ram    - oldBeginOfProgramMemory + beginOfProgramMemory;
      beginOfCurrentProgram.ram = beginOfCurrentProgram.ram - oldBeginOfProgramMemory + beginOfProgramMemory;
      endOfCurrentProgram.ram   = endOfCurrentProgram.ram   - oldBeginOfProgramMemory + beginOfProgramMemory;
    }
  }

  firstFreeProgramByte   += sizeInBytes;
  freeProgramBytes       -= sizeInBytes;
}



static bool _addEndNeeded(void) {
  if(firstFreeProgramByte <= beginOfProgramMemory) {
    return false;
  }
  if(firstFreeProgramByte == beginOfProgramMemory + 1) {
    return true;
  }
  if(isAtEndOfProgram(firstFreeProgramByte - 2)) {
    return false;
  }
  return true;
}


void fnPRcl(uint16_t unusedButMandatoryParameter) {
  uint32_t pgmSizeInByte = endOfCurrentProgram.any - beginOfCurrentProgram.any;
  bool fromFlash = (programList[currentProgramNumber - 1].step < 0); // flash memory

  if(_addEndNeeded()) {
    _addSpaceAfterPrograms(2);
    *(firstFreeProgramByte - 2) = (ITM_END >> 8) | 0x80;
    *(firstFreeProgramByte - 1) =  ITM_END       & 0xff;
    *(firstFreeProgramByte    ) = 0xffu;
    *(firstFreeProgramByte + 1) = 0xffu;
    scanLabelsAndPrograms();
    pgmSizeInByte = endOfCurrentProgram.any - beginOfCurrentProgram.any;
    if(fromFlash) { // flash memory
      ++currentProgramNumber;
    }
  }

  _addSpaceAfterPrograms(pgmSizeInByte);
  if(fromFlash) { // flash memory
    readStepInFlashPgmLibrary(firstFreeProgramByte - pgmSizeInByte, pgmSizeInByte, beginOfCurrentProgram.flash);
    ++currentProgramNumber;
  }
  else { // RAM
    xcopy(firstFreeProgramByte - pgmSizeInByte, beginOfCurrentProgram.ram, pgmSizeInByte);
  }
  *(firstFreeProgramByte    ) = 0xffu;
  *(firstFreeProgramByte + 1) = 0xffu;
  scanLabelsAndPrograms();
}



void fnPSto(uint16_t unusedButMandatoryParameter) {
  if(programList[currentProgramNumber - 1].step > 0) { // RAM
    char lblName[16];
    char flashPgmName[16];
    uint32_t pgmSize = endOfCurrentProgram.ram - beginOfCurrentProgram.ram;

    // Check for labels in RAM
    lblName[0] = 0;
    for(int i = 0; i < numberOfLabels; ++i) {
      if(labelList[i].program == currentProgramNumber && labelList[i].step > 0) {
        xcopy(lblName, labelList[i].labelPointer.ram + 1, *(labelList[i].labelPointer.ram));
        lblName[*(labelList[i].labelPointer.ram)] = 0;
        break;
      }
    }
    if(lblName[0] == 0) {
      displayCalcErrorMessage(ERROR_NO_GLOBAL_LABEL, ERR_REGISTER_LINE, REGISTER_X);
      return;
    }

    // Check for memory
    if(getFreeFlash() < pgmSize) {
      displayCalcErrorMessage(ERROR_FLASH_MEMORY_FULL, ERR_REGISTER_LINE, REGISTER_X);
      return;
    }

    // Check for END before .END.
    if(!isAtEndOfProgram(firstFreeProgramByte - 2)) {
      _addSpaceAfterPrograms(2);
      *(firstFreeProgramByte - 2) = (ITM_END >> 8) | 0x80;
      *(firstFreeProgramByte - 1) =  ITM_END       & 0xff;
      *(firstFreeProgramByte    ) = 0xffu;
      *(firstFreeProgramByte + 1) = 0xffu;
      scanLabelsAndPrograms();
      pgmSize = endOfCurrentProgram.ram - beginOfCurrentProgram.ram;
    }

    // Check for labels in Flash
    for(int i = 0; i < numberOfLabels; ++i) {
      if(labelList[i].program < 0 && labelList[i].step > 0) {
        readStepInFlashPgmLibrary((uint8_t *)flashPgmName, 16, labelList[i].labelPointer.flash);
        flashPgmName[flashPgmName[0] + 1] = 0;
        if(compareString(lblName, &flashPgmName[1], CMP_NAME) == 0) {
          uint16_t programNumber = abs(labelList[i].program);

          deleteFromFlashPgmLibrary(programList[programNumber - 1].instructionPointer.flash, programList[programNumber].instructionPointer.flash);

          scanFlashPgmLibrary();
          scanLabelsAndPrograms();
          break;
        }
      }
    }

    // Append to Flash
    if(!ioFileOpen(ioPathPgmFile, ioModeUpdate)) {
      displayCalcErrorMessage(ERROR_NO_BACKUP_DATA, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("cannot find or read backup data file wp43.sav");
      return;
    }

    ioFileSeek(sizeOfFlashPgmLibrary);
    save(beginOfCurrentProgram.ram, pgmSize);
    save(firstFreeProgramByte, 2); // 0xffff

    ioFileClose();

    scanFlashPgmLibrary();
    scanLabelsAndPrograms();
  }
  else {
    displayCalcErrorMessage(ERROR_FLASH_MEMORY_WRITE_PROTECTED, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot copy a program from FM to FM");
  }
}



void deleteFromFlashPgmLibrary(uint32_t fromAddr, uint32_t toAddr) {
  if(!ioFileOpen(ioPathPgmFile, ioModeUpdate)) {
    displayCalcErrorMessage(ERROR_NO_BACKUP_DATA, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot find or read backup data file wp43.sav");
    return;
  }

  --toAddr; // argument is 1-based
  --fromAddr; // must convert to 0-based
  do {
    ioFileSeek(toAddr);
    load(tmpString, FLASH_PGM_PAGE_SIZE);
    ioFileSeek(fromAddr);
    save(tmpString, FLASH_PGM_PAGE_SIZE);
    fromAddr += FLASH_PGM_PAGE_SIZE;
    toAddr += FLASH_PGM_PAGE_SIZE;
  } while(toAddr < (sizeOfFlashPgmLibrary + 2));

  ioFileClose();
}



void readStepInFlashPgmLibrary(uint8_t *buffer, uint16_t bufferSize, uint32_t pointer) {
  if(!ioFileOpen(ioPathPgmFile, ioModeRead)) {
    displayCalcErrorMessage(ERROR_NO_BACKUP_DATA, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot find or read backup data file wp43.sav");
    return;
  }

  ioFileSeek(pointer - 1);
  load(buffer, bufferSize);

  ioFileClose();
}



void scanFlashPgmLibrary(void) {
  if(!ioFileOpen(ioPathPgmFile, ioModeRead)) {
    initFlashPgmLibrary();
    if(!ioFileOpen(ioPathPgmFile, ioModeRead)) {
      displayCalcErrorMessage(ERROR_NO_BACKUP_DATA, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("cannot find or read backup data file wp43.sav");
      return;
    }
  }

  uint8_t *nextStep, *step = (uint8_t *)tmpString;
  int32_t seekPos = 0;
  uint32_t stepNumber = 0;

  freeWp43(flashLabelList, sizeof(labelList_t) * numberOfLabelsInFlash);
  freeWp43(flashProgramList, sizeof(programList_t) * numberOfProgramsInFlash);

  numberOfLabelsInFlash = 0;
  numberOfProgramsInFlash = 1;

  load(tmpString, FLASH_PGM_PAGE_SIZE + 32);
  while(!isAtEndOfPrograms(step)) { // .END.
    if(*step == ITM_LBL) { // LBL
      numberOfLabelsInFlash++;
    }
    if(isAtEndOfProgram(step)) { // END
      numberOfProgramsInFlash++;
    }
    step = findNextStep_ram(step);
    if(step >= (uint8_t *)tmpString + FLASH_PGM_PAGE_SIZE) {
      seekPos += FLASH_PGM_PAGE_SIZE;
      ioFileSeek(seekPos);
      load(tmpString, FLASH_PGM_PAGE_SIZE + 32);
      step -= FLASH_PGM_PAGE_SIZE;
    }
  }

  sizeOfFlashPgmLibrary = (uint32_t)(((intptr_t)step - (intptr_t)tmpString) + seekPos);

  flashLabelList = allocWp43(sizeof(labelList_t) * numberOfLabelsInFlash);
  if(flashLabelList == NULL) {
    // unlikely
    lastErrorCode = ERROR_RAM_FULL;
    return;
  }

  flashProgramList = allocWp43(sizeof(programList_t) * numberOfProgramsInFlash);
  if(flashProgramList == NULL) {
    // unlikely
    lastErrorCode = ERROR_RAM_FULL;
    return;
  }

  seekPos = 0;
  ioFileSeek(seekPos);
  load(tmpString, FLASH_PGM_PAGE_SIZE + 32);

  numberOfLabelsInFlash = 0;
  step = (uint8_t *)tmpString;
  flashProgramList[0].instructionPointer.flash = 1;
  flashProgramList[0].step = -1;

  numberOfProgramsInFlash = 1;
  stepNumber = 1;
  while(!isAtEndOfPrograms(step)) { // .END.
    nextStep = findNextStep_ram(step);
    if(checkOpCodeOfStep(step, ITM_LBL)) { // LBL
      flashLabelList[numberOfLabelsInFlash].program = -numberOfProgramsInFlash;
      if(*(step + 1) <= 104) { // Local label
        flashLabelList[numberOfLabelsInFlash].step = -stepNumber;
        flashLabelList[numberOfLabelsInFlash].labelPointer.flash = step - (uint8_t *)tmpString + 1 + seekPos + 1;
      }
      else { // Global label
        flashLabelList[numberOfLabelsInFlash].step = stepNumber;
        flashLabelList[numberOfLabelsInFlash].labelPointer.flash = step - (uint8_t *)tmpString + 2 + seekPos + 1;
      }

      flashLabelList[numberOfLabelsInFlash].instructionPointer.flash = nextStep - (uint8_t *)tmpString + seekPos + 1;
      numberOfLabelsInFlash++;
    }

    if(isAtEndOfProgram(step)) { // END
      flashProgramList[numberOfProgramsInFlash].instructionPointer.flash = step - (uint8_t *)tmpString + 2 + seekPos + 1;
      flashProgramList[numberOfProgramsInFlash].step = -(stepNumber + 1);
      numberOfProgramsInFlash++;
    }

    step = nextStep;
    stepNumber++;

    if(step >= (uint8_t *)tmpString + FLASH_PGM_PAGE_SIZE) {
      seekPos += FLASH_PGM_PAGE_SIZE;
      ioFileSeek(seekPos);
      load(tmpString, FLASH_PGM_PAGE_SIZE + 32);
      step -= FLASH_PGM_PAGE_SIZE;
    }
  }

  ioFileClose();
}



void initFlashPgmLibrary(void) {
  if(!ioFileOpen(ioPathPgmFile, ioModeWrite)) {
    #if !defined(DMCP_BUILD)
      printf("Cannot SAVE in file wp43.dat!\n");
    #endif // !DMCP_BUILD
    return;
  }

  memset(tmpString, 0, FLASH_PGM_PAGE_SIZE);
  tmpString[0] = -1;
  tmpString[1] = -1;
  save(tmpString, FLASH_PGM_PAGE_SIZE);
  tmpString[0] = 0;
  tmpString[1] = 0;
  for(int i = 1; i < FLASH_PGM_NUMBER_OF_PAGES; ++i) {
    save(tmpString, FLASH_PGM_PAGE_SIZE);
  }

  ioFileClose();
}
