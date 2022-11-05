// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "programming/manage.h"

#include "calcMode.h"
#include "charString.h"
#include "config.h"
#include "core/memory.h"
#include "dateTime.h"
#include "defines.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "hal/gui.h"
#include "items.h"
#include "programming/decode.h"
#include "programming/flash.h"
#include "programming/lblGtoXeq.h"
#include "programming/nextStep.h"
#include "realType.h"
#include "registers.h"
#include "registerValueConversions.h"
#include "sort.h"
#include "ui/bufferize.h"
#include "ui/cursor.h"
#include "ui/screen.h"
#include "ui/softmenus.h"
#include "ui/tam.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "wp43.h"

// Structure of the program memory.
// In this example the RAM is 16384 blocks (from 0 to 16383) of 4 bytes = 65536 bytes.
// The program memory occupies the end of the RAM area.
//
//  +-----+------+----+----------+
//  |Block| Step |Code|    OP    |
//  |     |loc/gl|    |          |
//  +-----+------+----+----------+
//  |16374| 1/ 1 |   1| LBL 'P1' | <-- beginOfProgramMemory
//  |     |      | 253|          |
//  |     |      |   2|          |
//  |     |      | 'P'|          |
//  |16375|      | '1'|          |  ^
//  |     | 2/ 2 | 114| 1.       |  | 1 block = 4 bytes
//  |     |      |   6|          |  |
//  |     |      |   1|          |  v
//  |16376|      | '1'|          |
//  |     | 3/ 3 |  95| +        |
//  |     | 4/ 4 | 133| END      |
//  |     |      | 168|          |
//  |16377| 1/ 5 |   1| LBL 'P2' | <-- beginOfCurrentProgram  <-- firstDisplayedStep   firstDisplayedStepNumber = 5   firstDisplayedLocalStepNumber = 1
//  |     |      | 253|          |
//  |     |      |   2|          |
//  |     |      | 'P'|          |
//  |16378|      | '2'|          |
//  |     | 2/ 6 | 114| 22.      | <-- currentStep       this inequality is always true: beginOfCurrentProgram ≤ currentStep < endOfCurrentProgram
//  |     |      |   6|          |     currentStepNumber = 6   currentLocalStepNumber = 2
//  |     |      |   2|          |     currentProgramNumber = 2
//  |16379|      | '2'|          |
//  |     |      | '2'|          |
//  |     | 3/ 7 |  95| +        |
//  |     | 4/ 8 | 133| END      |
//  |16380|      | 168|          |
//  |     | 1/ 9 |   2| LBL 'P3' | <-- endOfCurrentProgram
//  |     |      | 253|          |
//  |     |      |   2|          |
//  |16381|      | 'P'|          |
//  |     |      | '3'|          |
//  |     | 2/10 | 114| 3.       |
//  |     |      |   6|          |
//  |16382|      |   1|          |
//  |     |      | '3'|          |
//  |     | 3/11 |  95| +        |
//  |     | 4/12 | 133| END      |
//  |16383| 5/13 | 168|          |
//  |     |      | 255| .END.    | <-- firstFreeProgramByte
//  |     |      | 255|          |
//  |     |      |   ?|          | free byte     This byte is the end of the RAM area
//  +-----+------+----+----------+
//
//  freeProgramBytes = 1

void scanLabelsAndPrograms(void) {
  uint32_t stepNumber = 0;
  uint8_t *nextStep, *step = beginOfProgramMemory;
  uint16_t numberOfProgramsInRam;

  freeWp43(labelList, sizeof(labelList_t) * numberOfLabels);
  freeWp43(programList, sizeof(programList_t) * numberOfPrograms);

  numberOfLabels = 0;
  numberOfPrograms = 1;
  while(*step != 255 || *(step + 1) != 255) { // .END.
    if(*step == ITM_LBL) { // LBL
      numberOfLabels++;
    }
    if((*step & 0x7f) == (ITM_END >> 8) && *(step + 1) == (ITM_END & 0xff)) { // END
      numberOfPrograms++;
    }
    step = findNextStep_ram(step);
  }

  labelList = allocWp43(sizeof(labelList_t) * (numberOfLabels + numberOfLabelsInFlash));
  if(labelList == NULL) {
    // unlikely
    lastErrorCode = ERROR_RAM_FULL;
    return;
  }

  programList = allocWp43(sizeof(programList_t) * (numberOfPrograms + numberOfProgramsInFlash));
  if(programList == NULL) {
    // unlikely
    lastErrorCode = ERROR_RAM_FULL;
    return;
  }

  numberOfLabels = 0;
  step = beginOfProgramMemory;
  programList[0].instructionPointer.ram = step;
  programList[0].step = (0 + 1);
  numberOfPrograms = 1;
  stepNumber = 1;
  while(*step != 255 || *(step + 1) != 255) { // .END.
    nextStep = findNextStep_ram(step);
    if(*step == 1) { // LBL
      labelList[numberOfLabels].program = numberOfPrograms;
      if(*(step + 1) <= 109) { // Local label
        labelList[numberOfLabels].step = -stepNumber;
        labelList[numberOfLabels].labelPointer.ram = step + 1;
      }
      else { // Global label
        labelList[numberOfLabels].step = stepNumber;
        labelList[numberOfLabels].labelPointer.ram = step + 2;
      }

      labelList[numberOfLabels].instructionPointer.ram = nextStep;
      numberOfLabels++;
    }

    if((*step & 0x7f) == (ITM_END >> 8) && *(step + 1) == (ITM_END & 0xff)) { // END
      programList[numberOfPrograms].instructionPointer.ram = step + 2;
      programList[numberOfPrograms].step = stepNumber + 1;
      numberOfPrograms++;
    }

    step = nextStep;
    stepNumber++;
  }

  numberOfProgramsInRam = numberOfPrograms;

  for(int i = 0; i < numberOfLabelsInFlash; ++i) {
    labelList[numberOfLabels] = flashLabelList[i];
    labelList[numberOfLabels].program -= numberOfProgramsInRam;
    numberOfLabels++;
  }
  for(int i = 0; i < numberOfProgramsInFlash; ++i) {
    programList[numberOfPrograms] = flashProgramList[i];
    numberOfPrograms++;
  }

  defineCurrentProgramFromCurrentStep();
}



void deleteStepsFromTo(uint8_t *from, uint8_t *to) {
  uint16_t opSize = to - from;

  xcopy(from, to, (firstFreeProgramByte - to) + 2);
  firstFreeProgramByte -= opSize;
  freeProgramBytes += opSize;
  scanLabelsAndPrograms();
}



void fnClPAll(uint16_t confirmation) {
  if(confirmation == NOT_CONFIRMED) {
    setConfirmationMode(fnClPAll);
  }
  else {
    resizeProgramMemory(TO_BYTES(1)); // 1 block for an empty program
    *(beginOfProgramMemory + 0)   = (ITM_END >> 8) | 0x80;
    *(beginOfProgramMemory + 1)   =  ITM_END       & 0xff;
    *(beginOfProgramMemory + 2)   = 255; // .END.
    *(beginOfProgramMemory + 3)   = 255; // .END.
    firstFreeProgramByte          = beginOfProgramMemory + 2;
    freeProgramBytes              = 0;
    currentStep.ram               = beginOfProgramMemory;
    firstDisplayedStep.ram        = beginOfProgramMemory;
    firstDisplayedLocalStepNumber = 0;
    currentLocalStepNumber        = 1;
    temporaryInformation          = TI_NO_INFO;
    programRunStop                = PGM_STOPPED;
    scanLabelsAndPrograms();
  }
}



void fnClP(uint16_t unusedButMandatoryParameter) {
  if(programList[currentProgramNumber - 1].step < 0) { // flash memory
    uint16_t savedCurrentProgramNumber = currentProgramNumber;

    deleteFromFlashPgmLibrary(beginOfCurrentProgram.flash, endOfCurrentProgram.flash - ((currentProgramNumber == numberOfPrograms) ? 2 : 0));

    scanFlashPgmLibrary();
    scanLabelsAndPrograms();

    if(savedCurrentProgramNumber >= numberOfPrograms) { // The last program
      fnGotoDot(programList[numberOfPrograms - 2].step);
    }
    else { // Not the last program
      fnGotoDot(programList[savedCurrentProgramNumber - 1].step);
    }
  }
  else if(beginOfCurrentProgram.ram == beginOfProgramMemory && *endOfCurrentProgram.ram == 255 && *(endOfCurrentProgram.ram + 1) == 255) { // There is only one program in memory
    fnClPAll(CONFIRMED);
  }
  else {
    uint16_t savedCurrentProgramNumber = currentProgramNumber;

    deleteStepsFromTo(beginOfCurrentProgram.ram, endOfCurrentProgram.ram - ((currentProgramNumber == (numberOfPrograms - numberOfProgramsInFlash)) ? 2 : 0));
    scanLabelsAndPrograms();
    // unlikely fails

    if(savedCurrentProgramNumber >= (numberOfPrograms - numberOfProgramsInFlash)) { // The last program
      fnGotoDot(programList[numberOfPrograms - numberOfProgramsInFlash - 2].step);
    }
    else { // Not the last program
      fnGotoDot(programList[savedCurrentProgramNumber - 1].step);
    }
  }
}



static uint32_t _getProgramSize(void) {
  if(currentProgramNumber == (numberOfPrograms - numberOfProgramsInFlash)) {
    uint16_t numberOfSteps = 1;
    uint8_t *step = programList[currentProgramNumber - 1].instructionPointer.ram;
    while(*step != 255 || *(step + 1) != 255) { // .END.
      ++numberOfSteps;
      step = findNextStep_ram(step);
    }
    return (uint32_t)(step - programList[currentProgramNumber - 1].instructionPointer.ram + 2);
    // TODO: last program in flash
  }
  else {
    return (uint32_t)(programList[currentProgramNumber].instructionPointer.any - programList[currentProgramNumber - 1].instructionPointer.any);
  }
}



void defineCurrentProgramFromGlobalStepNumber(int16_t globalStepNumber) {
  currentProgramNumber = 0;
  while(abs(globalStepNumber) >= abs(programList[currentProgramNumber].step) || (globalStepNumber < 0 && programList[currentProgramNumber].step > 0)) {
    currentProgramNumber++;
    if(globalStepNumber > 0 && currentProgramNumber >= (numberOfPrograms - numberOfProgramsInFlash)) {
      break;
    }
    if(currentProgramNumber >= numberOfPrograms) {
      break;
    }
  }

  if(programList[currentProgramNumber - 1].step < 0) {
    if(currentProgramNumber >= numberOfPrograms) {
      endOfCurrentProgram.flash = programList[currentProgramNumber - 1].instructionPointer.flash + _getProgramSize();
    }
    else {
      endOfCurrentProgram.flash = programList[currentProgramNumber].instructionPointer.flash;
    }
    beginOfCurrentProgram.flash = programList[currentProgramNumber - 1].instructionPointer.flash;
  }
  else {
    if(currentProgramNumber == (numberOfPrograms - numberOfProgramsInFlash)) {
      endOfCurrentProgram.ram = programList[currentProgramNumber - 1].instructionPointer.ram + _getProgramSize();
    }
    else {
      endOfCurrentProgram.ram = programList[currentProgramNumber].instructionPointer.ram;
    }
    beginOfCurrentProgram.ram = programList[currentProgramNumber - 1].instructionPointer.ram;
  }
}



void defineCurrentProgramFromCurrentStep(void) {
  if(beginOfProgramMemory <= currentStep.ram && currentStep.ram <= firstFreeProgramByte) {
    currentProgramNumber = 0;
    while(currentStep.ram >= programList[currentProgramNumber].instructionPointer.ram) {
      currentProgramNumber++;
      if(currentProgramNumber >= (numberOfPrograms - numberOfProgramsInFlash)) {
        break;
      }
    }

    if(currentProgramNumber >= (numberOfPrograms - numberOfProgramsInFlash)) {
      endOfCurrentProgram.ram = programList[currentProgramNumber - 1].instructionPointer.ram + _getProgramSize();
    }
    else {
      endOfCurrentProgram.ram = programList[currentProgramNumber].instructionPointer.ram;
    }
    beginOfCurrentProgram.ram = programList[currentProgramNumber - 1].instructionPointer.ram;
  }
}



void scrollPemBackwards(void) {
  if(firstDisplayedLocalStepNumber > 0)
    --firstDisplayedLocalStepNumber;
  defineFirstDisplayedStep();
}



void scrollPemForwards(void) {
  if(getNumberOfSteps() > 6) {
    if(currentLocalStepNumber > 3) {
      ++firstDisplayedLocalStepNumber;
      firstDisplayedStep = findNextStep(firstDisplayedStep);
    }
    else if(currentLocalStepNumber == 3) {
      firstDisplayedLocalStepNumber = 1;
    }
  }
}



void fnPem(uint16_t unusedButMandatoryParameter) {
  #if !defined(TESTSUITE_BUILD)
    ///////////////////////////////////////////////////////////////////////////////////////
    // For this function to work properly we need the following variables set properly:
    //  - currentProgramNumber
    //  - currentLocalStepNumber
    //  - firstDisplayedLocalStepNumber
    //  - firstDisplayedStep
    //
    uint32_t currentStepNumber, firstDisplayedStepNumber;
    uint16_t line, firstLine;
    uint16_t stepsThatWouldBeDisplayed = 7;
    uint8_t *step, *nextStep;
    uint8_t *tmpSteps = NULL;
    bool     lblOrEnd;
    bool     inTamMode = tamIsActive() && programList[currentProgramNumber - 1].step > 0;
    uint16_t numberOfSteps = getNumberOfSteps();
    uint16_t linesOfCurrentStep = 1;
    bool     cursorAppeared = false;

    if(calcMode != cmPem) {
      calcMode = cmPem;
      hourGlassIconEnabled = false;
      aimBuffer[0] = 0;
      currentInputVariable = INVALID_VARIABLE;
      return;
    }

    if(currentLocalStepNumber < firstDisplayedLocalStepNumber) {
      firstDisplayedLocalStepNumber = currentLocalStepNumber;
      defineFirstDisplayedStep();
    }

    if(currentLocalStepNumber == 0) {
      currentLocalStepNumber = 1;
      pemCursorIsZerothStep = true;
    }
    else if(currentLocalStepNumber > 1) {
      pemCursorIsZerothStep = false;
    }
    currentStepNumber        = currentLocalStepNumber        + abs(programList[currentProgramNumber - 1].step) - 1;
    firstDisplayedStepNumber = firstDisplayedLocalStepNumber + abs(programList[currentProgramNumber - 1].step) - 1;
    step                     = firstDisplayedStep.any;
    programListEnd           = false;
    lastProgramListEnd       = false;

    if(firstDisplayedLocalStepNumber == 0) {
      showString("0000:" STD_SPACE_4_PER_EM, &standardFont, 1, Y_POSITION_OF_REGISTER_T_LINE, (pemCursorIsZerothStep && !tamIsActive() && aimBuffer[0] == 0) ? vmReverse : vmNormal, false, true);
      sprintf(tmpString, "{ Prgm #%d: %" PRIu32 " bytes / %" PRIu16 " step%s }", currentProgramNumber, _getProgramSize(),
                                                                               numberOfSteps, numberOfSteps == 1 ? "" : "s");
      showString(tmpString, &standardFont, 42, Y_POSITION_OF_REGISTER_T_LINE, vmNormal, false, false);
      firstLine = 1;
    }
    else {
      firstLine = 0;
    }

    int lineOffset = 0, lineOffsetTam = 0;

    if(programList[currentProgramNumber - 1].step < 0) { // Flash
      tmpSteps = allocWp43(400 * 7);
      readStepInFlashPgmLibrary(tmpSteps, 400 * 7, firstDisplayedStep.flash);
      step = tmpSteps;
    }

    for(line=firstLine; line<7; line++) {
      nextStep = findNextStep_ram(step);
      //uint16_t stepSize = (uint16_t)(nextStep - step);
      sprintf(tmpString, "%04d:" STD_SPACE_4_PER_EM, firstDisplayedLocalStepNumber + line - lineOffset + lineOffsetTam);
      if(firstDisplayedStepNumber + line - lineOffset == currentStepNumber) {
        tamOverPemYPos = Y_POSITION_OF_REGISTER_T_LINE + 21 * line;
        showString(tmpString, &standardFont, 1, tamOverPemYPos, ((pemCursorIsZerothStep && !tamIsActive() && aimBuffer[0] == 0) || (tamIsActive() && (programList[currentProgramNumber - 1].step > 0))) ? vmNormal : vmReverse, false, true);
        if(programList[currentProgramNumber - 1].step < 0) { // Flash
          currentStep.flash = step - tmpSteps + firstDisplayedStep.flash;
        }
        else { // RAM
          currentStep.ram = step;
        }
      }
      else {
        showString(tmpString, &standardFont, 1, Y_POSITION_OF_REGISTER_T_LINE + 21 * line, vmNormal,  false, true);
      }
      lblOrEnd = (*step == ITM_LBL) || ((*step == ((ITM_END >> 8) | 0x80)) && (*(step + 1) == (ITM_END & 0xff))) || ((*step == 0xff) && (*(step + 1) == 0xff));
      if(programList[currentProgramNumber - 1].step > 0) {
        if((!pemCursorIsZerothStep && firstDisplayedStepNumber + line - lineOffset == currentStepNumber + 1) || (line == 1 && tamIsActive() && pemCursorIsZerothStep)) {
          tamOverPemYPos = Y_POSITION_OF_REGISTER_T_LINE + 21 * line;
          if(tamIsActive()) {
            line += 1;
            lineOffset += 1;
            lineOffsetTam += 1;
            showString(tmpString, &standardFont, 1, tamOverPemYPos, vmReverse, false, true);
            if(line >= 7) {
              break;
            }
            sprintf(tmpString, "%04d:" STD_SPACE_4_PER_EM, firstDisplayedLocalStepNumber + line - lineOffset + lineOffsetTam);
            showString(tmpString, &standardFont, 1, Y_POSITION_OF_REGISTER_T_LINE + 21 * line, vmNormal, false, true);
          }
        }
        else if(firstDisplayedStepNumber + line - lineOffset == currentStepNumber && lblOrEnd && (*step != ITM_LBL)) {
          if(tamIsActive()) {
            line += 1;
            lineOffset += 1;
            lineOffsetTam += 1;
            showString(tmpString, &standardFont, 1, tamOverPemYPos, vmReverse, false, true);
            if(line >= 7) {
              break;
            }
            sprintf(tmpString, "%04d:" STD_SPACE_4_PER_EM, firstDisplayedLocalStepNumber + line - lineOffset + lineOffsetTam);
            showString(tmpString, &standardFont, 1, Y_POSITION_OF_REGISTER_T_LINE + 21 * line, vmNormal, false, true);
          }
        }
      }
      decodeOneStep_ram(step);
      if(firstDisplayedStepNumber + line - lineOffset == currentStepNumber && !tamIsActive()) {
        if(getSystemFlag(FLAG_ALPHA)) {
          char *tstr = tmpString + stringByteLength(tmpString) - 2;
          *(tstr++) = STD_CURSOR[0];
          *(tstr++) = STD_CURSOR[1];
          *(tstr++) = STD_RIGHT_SINGLE_QUOTE[0];
          *(tstr++) = STD_RIGHT_SINGLE_QUOTE[1];
          *(tstr++) = 0;
        }
        else if(aimBuffer[0] != 0) {
          char *tstr = tmpString + stringByteLength(tmpString);
          *(tstr++) = STD_CURSOR[0];
          *(tstr++) = STD_CURSOR[1];
          *(tstr++) = 0;
        }
      }

      // Split long lines
      int numberOfExtraLines = 0;
      int offset = 0;
      while(offset <= 1500 && stringWidth(tmpString + offset, &standardFont, false, false) > 337) {
        numberOfExtraLines++;
        xcopy(tmpString + 2100, tmpString + offset, stringByteLength(tmpString + offset) + 1);
        while(stringWidth(tmpString + offset, &standardFont, false, false) > 337) {
          tmpString[offset + stringLastGlyph(tmpString + offset)] = 0;
        }
        xcopy(tmpString + offset + 300, tmpString + 2100 + stringByteLength(tmpString + offset), stringByteLength(tmpString + 2100 + stringByteLength(tmpString + offset)) + 1);
        offset += 300;
      }
      stepsThatWouldBeDisplayed -= numberOfExtraLines;
      if(firstDisplayedStepNumber + line - lineOffset == currentStepNumber) {
        linesOfCurrentStep += numberOfExtraLines;
      }

      char *cursorPos = strstr(tmpString, STD_CURSOR);
      showString(tmpString, &standardFont, lblOrEnd ? 42 : 62, Y_POSITION_OF_REGISTER_T_LINE + 21 * line, vmNormal,  false, false);
      if(cursorPos) {
        *cursorPos = 0;
        cursorShow(true, 62 + stringWidth(tmpString, &standardFont, false, true), Y_POSITION_OF_REGISTER_T_LINE + 21 * line);
        cursorAppeared = true;
      }
      offset = 300;
      while(numberOfExtraLines && line <= 5) {
        cursorPos = strstr(tmpString + offset, STD_CURSOR);
        showString(tmpString + offset, &standardFont, 62, Y_POSITION_OF_REGISTER_T_LINE + 21 * (++line), vmNormal,  false, false);
        if(cursorPos) {
          *cursorPos = 0;
          cursorShow(true, 62 + stringWidth(tmpString, &standardFont, false, true), Y_POSITION_OF_REGISTER_T_LINE + 21 * line);
          cursorAppeared = true;
        }
        numberOfExtraLines--;
        offset += 300;
        lineOffset++;
      }

      if(!cursorAppeared) {
        cursorHide();
      }

      if((*step == ((ITM_END >> 8) | 0x80)) && (*(step + 1) == (ITM_END & 0xff))) {
        programListEnd = true;
        if(*nextStep == 255 && *(nextStep + 1) == 255) {
          lastProgramListEnd = true;
        }
        break;
      }
      if((*step == 255) && (*(step + 1) == 255)) {
        programListEnd = true;
        lastProgramListEnd = true;
        break;
      }
      step = nextStep;
    }

    if(programList[currentProgramNumber - 1].step < 0) { // Flash
      freeWp43(tmpSteps, 400 * 7);
    }

    if(aimBuffer[0] != 0 && linesOfCurrentStep > 4) { // Limited to 4 lines so as not to cause crash or freeze
      if(getSystemFlag(FLAG_ALPHA)) {
        pemAlpha(ITM_BACKSPACE);
      }
      else {
        pemAddNumber(ITM_BACKSPACE);
      }
      clearScreen();
      showSoftmenuCurrentPart();
      fnPem(NOPARAM);
    }
    if((currentLocalStepNumber + (inTamMode ? (currentLocalStepNumber < numberOfSteps ? 2 : 1) : 0)) >= (firstDisplayedLocalStepNumber + stepsThatWouldBeDisplayed)) {
      firstDisplayedLocalStepNumber = currentLocalStepNumber - stepsThatWouldBeDisplayed + 1 + (inTamMode ? (currentLocalStepNumber < numberOfSteps ? 2 : 1) : 0);
      if(inTamMode && (firstDisplayedLocalStepNumber > 1) && (currentLocalStepNumber + 1 >= (firstDisplayedLocalStepNumber + stepsThatWouldBeDisplayed))) {
        ++firstDisplayedLocalStepNumber;
      }
      defineFirstDisplayedStep();
      clearScreen();
      showSoftmenuCurrentPart();
      fnPem(NOPARAM);
    }
  #endif // !TESTSUITE_BUILD
}



static void _insertInProgram(const uint8_t *dat, uint16_t sizeInBytes) {
  int16_t _dynamicMenuItem = dynamicMenuItem;
  uint16_t globalStepNumber;
  if(freeProgramBytes < sizeInBytes) {
    uint8_t *oldBeginOfProgramMemory = beginOfProgramMemory;
    uint32_t programSizeInBytes = TO_BYTES(RAM_SIZE_IN_BLOCKS - freeMemoryRegions[numberOfFreeMemoryRegions - 1].address - freeMemoryRegions[numberOfFreeMemoryRegions - 1].sizeInBlocks);
    uint32_t newProgramSizeInBytes = TO_BYTES(TO_BLOCKS(programSizeInBytes - freeProgramBytes + sizeInBytes));
    freeProgramBytes      += newProgramSizeInBytes - programSizeInBytes;
    resizeProgramMemory(newProgramSizeInBytes);
    currentStep.ram           = currentStep.ram           - oldBeginOfProgramMemory + beginOfProgramMemory;
    firstDisplayedStep.ram    = firstDisplayedStep.ram    - oldBeginOfProgramMemory + beginOfProgramMemory;
    beginOfCurrentProgram.ram = beginOfCurrentProgram.ram - oldBeginOfProgramMemory + beginOfProgramMemory;
    endOfCurrentProgram.ram   = endOfCurrentProgram.ram   - oldBeginOfProgramMemory + beginOfProgramMemory;
  }
  for(uint8_t *pos = firstFreeProgramByte + 1 + sizeInBytes; pos > currentStep.ram; --pos) {
    *pos = *(pos - sizeInBytes);
  }
  for(uint16_t i = 0; i < sizeInBytes; ++i) {
    *(currentStep.ram++) = *(dat++);
  }
  firstFreeProgramByte    += sizeInBytes;
  freeProgramBytes        -= sizeInBytes;
  currentLocalStepNumber  += 1;
  endOfCurrentProgram.ram += sizeInBytes;
  globalStepNumber = currentLocalStepNumber + programList[currentProgramNumber - 1].step - 1;
  scanLabelsAndPrograms();
  dynamicMenuItem = -1;
  fnGotoDot(globalStepNumber);
  dynamicMenuItem = _dynamicMenuItem;
}



void pemAlpha(int16_t item) {
  #if !defined(TESTSUITE_BUILD)
    if(!getSystemFlag(FLAG_ALPHA)) {
      shiftF = false;
      shiftG = false;
      aimBuffer[0] = 0;
      alphaCase = AC_UPPER;
      nextChar = NC_NORMAL;

      if(softmenuStack[0].softmenuId == 0) { // MyMenu
        softmenuStack[0].softmenuId = 1; // MyAlpha
      }

      setSystemFlag(FLAG_ALPHA);

      calcModeUpdateGui();

      tmpString[0] = ITM_LITERAL;
      tmpString[1] = (char)STRING_LABEL_VARIABLE;
      tmpString[2] = 0;
      _insertInProgram((uint8_t *)tmpString, 3);
      --currentLocalStepNumber;
      currentStep = findPreviousStep(currentStep);
    }
    if(indexOfItems[item].func == addItemToBuffer) {
      int32_t len = stringByteLength(aimBuffer);
      if(alphaCase == AC_LOWER) {
        if(ITM_A <= item && item <= ITM_Z) {
          item += 26;
        }
        else if(ITM_ALPHA <= item && item <= ITM_OMEGA) {
          item += 36;
        }
      }
      item = convertItemToSubOrSup(item, nextChar);
      if(len < (256 - stringByteLength(indexOfItems[item].itemSoftmenuName)) && stringGlyphLength(aimBuffer) < 196) {
        xcopy(aimBuffer + len, indexOfItems[item].itemSoftmenuName, stringByteLength(indexOfItems[item].itemSoftmenuName) + 1);
      }
    }
    else if(item == ITM_DOWN_ARROW) {
      nextChar = NC_SUBSCRIPT;
    }
    else if(item == ITM_UP_ARROW) {
      nextChar = NC_SUPERSCRIPT;
    }
    else if(item == ITM_BACKSPACE) {
      if(aimBuffer[0] == 0) {
        deleteStepsFromTo(currentStep.ram, findNextStep_ram(currentStep.ram));
        clearSystemFlag(FLAG_ALPHA);
        calcModeUpdateGui();
        return;
      }
      else {
        aimBuffer[stringLastGlyph(aimBuffer)] = 0;
      }
    }
    else if(item == ITM_ENTER) {
      pemCloseAlphaInput();
      --firstDisplayedLocalStepNumber;
      defineFirstDisplayedStep();
      return;
    }

    deleteStepsFromTo(currentStep.ram, findNextStep_ram(currentStep.ram));
    tmpString[0] = ITM_LITERAL;
    tmpString[1] = (char)STRING_LABEL_VARIABLE;
    tmpString[2] = stringByteLength(aimBuffer);
    xcopy(tmpString + 3, aimBuffer, stringByteLength(aimBuffer));
    _insertInProgram((uint8_t *)tmpString, stringByteLength(aimBuffer) + 3);
    --currentLocalStepNumber;
    currentStep = findPreviousStep(currentStep);
    if(!programListEnd) {
      scrollPemBackwards();
    }
  #endif // !TESTSUITE_BUILD
}



void pemCloseAlphaInput(void) {
  #if !defined(TESTSUITE_BUILD)
    aimBuffer[0] = 0;
    clearSystemFlag(FLAG_ALPHA);
    calcModeUpdateGui();
    ++currentLocalStepNumber;
    currentStep = findNextStep(currentStep);
    ++firstDisplayedLocalStepNumber;
    firstDisplayedStep = findNextStep(firstDisplayedStep);
  #endif // !TESTSUITE_BUILD
}



void pemAddNumber(int16_t item) {
  #if !defined(TESTSUITE_BUILD)
    if(aimBuffer[0] == 0) {
      tmpString[0] = ITM_LITERAL;
      tmpString[1] = STRING_LONG_INTEGER;
      tmpString[2] = 0;
      _insertInProgram((uint8_t *)tmpString, 3);
      memset(nimBufferDisplay, 0, NIM_BUFFER_LENGTH);
      --currentLocalStepNumber;
      currentStep = findPreviousStep(currentStep);
      switch(item) {
        case ITM_EXPONENT : {
          aimBuffer[0] = '+';
          aimBuffer[1] = '1';
          aimBuffer[2] = '.';
          aimBuffer[3] = 0;
          nimNumberPart = NP_REAL_FLOAT_PART;
          lastIntegerBase = 0;
          break;
        }

        case ITM_PERIOD : {
          aimBuffer[0] = '+';
          aimBuffer[1] = '0';
          aimBuffer[2] = 0;
          nimNumberPart = NP_INT_10;
          break;
        }

        case ITM_0 :
        case ITM_1 :
        case ITM_2 :
        case ITM_3 :
        case ITM_4 :
        case ITM_5 :
        case ITM_6 :
        case ITM_7 :
        case ITM_8 :
        case ITM_9 :
        case ITM_A :
        case ITM_B :
        case ITM_C :
        case ITM_D :
        case ITM_E :
        case ITM_F : {
          aimBuffer[0] = '+';
          aimBuffer[1] = 0;
          nimNumberPart = NP_EMPTY;
          break;
        }
      }
    }
    if(item == ITM_BACKSPACE && ((aimBuffer[0] == '+' && aimBuffer[1] != 0 && aimBuffer[2] == 0) || aimBuffer[1] == 0)) {
      aimBuffer[0] = 0;
    }
    else {
      addItemToNimBuffer(item);
      if(stringByteLength(aimBuffer) > 255) {
        addItemToNimBuffer(ITM_BACKSPACE);
      }
    }
    clearSystemFlag(FLAG_ALPHA);

    if(aimBuffer[0] != '!') {
      deleteStepsFromTo(currentStep.ram, findNextStep_ram(currentStep.ram));
      if(aimBuffer[0] != 0) {
        const char *numBuffer = aimBuffer[0] == '+' ? aimBuffer + 1 : aimBuffer;
        tmpString[0] = ITM_LITERAL;
        switch(nimNumberPart) {
          //case NP_INT_16:
          //case NP_INT_BASE: {
          //  tmpString[1] = STRING_SHORT_INTEGER;
          //  break;
          //}
          case NP_REAL_FLOAT_PART:
          case NP_REAL_EXPONENT:
          case NP_FRACTION_DENOMINATOR: {
            tmpString[1] = STRING_REAL34;
            break;
          }
          case NP_COMPLEX_INT_PART:
          case NP_COMPLEX_FLOAT_PART:
          case NP_COMPLEX_EXPONENT: {
            tmpString[1] = STRING_COMPLEX34;
            break;
          }
          default: {
            tmpString[1] = STRING_LONG_INTEGER;
            break;
          }
        }
        tmpString[2] = stringByteLength(numBuffer);
        xcopy(tmpString + 3, numBuffer, stringByteLength(numBuffer));
        _insertInProgram((uint8_t *)tmpString, stringByteLength(numBuffer) + 3);
        --currentLocalStepNumber;
        currentStep = findPreviousStep(currentStep);
        if(!programListEnd) {
          scrollPemBackwards();
        }
      }
      calcMode = cmPem;
    }
    else {
      aimBuffer[0] = 0;
    }
  #endif // !TESTSUITE_BUILD
}



void pemCloseNumberInput(void) {
  #if !defined(TESTSUITE_BUILD)
    deleteStepsFromTo(currentStep.ram, findNextStep_ram(currentStep.ram));
    if(aimBuffer[0] != 0) {
      char *numBuffer = aimBuffer[0] == '+' ? aimBuffer + 1 : aimBuffer;
      char *basePtr = numBuffer;
      char *tmpPtr = tmpString;
      *(tmpPtr++) = ITM_LITERAL;
      switch(nimNumberPart) {
        //case NP_INT_16:
        case NP_INT_BASE: {
          *(tmpPtr++) = STRING_SHORT_INTEGER;
          while(*basePtr != '#') {
            ++basePtr;
          }
          *(basePtr++) = 0;
          *(tmpPtr++) = (char)atoi(basePtr);
          fflush(stdout);
          break;
        }
        case NP_REAL_FLOAT_PART:
        case NP_REAL_EXPONENT:
        case NP_FRACTION_DENOMINATOR: {
          *(tmpPtr++) = STRING_REAL34;
          break;
        }
        case NP_COMPLEX_INT_PART:
        case NP_COMPLEX_FLOAT_PART:
        case NP_COMPLEX_EXPONENT: {
          *(tmpPtr++) = STRING_COMPLEX34;
          break;
        }
        default: {
          *(tmpPtr++) = STRING_LONG_INTEGER;
          break;
        }
      }
      *(tmpPtr++) = stringByteLength(numBuffer);
      xcopy(tmpPtr, numBuffer, stringByteLength(numBuffer));
      _insertInProgram((uint8_t *)tmpString, stringByteLength(numBuffer) + (int32_t)(tmpPtr - tmpString));
      pemCursorIsZerothStep = false;
    }

    aimBuffer[0] = '!';
  #endif // TESTSUITE_BUILD
}



static void _pemCloseTimeInput(void) {
  #if !defined(TESTSUITE_BUILD)
    switch(nimNumberPart) {
      case NP_INT_10:
      case NP_REAL_FLOAT_PART: {
        deleteStepsFromTo(currentStep.ram, findNextStep_ram(currentStep.ram));
        if(aimBuffer[0] != 0) {
          char *numBuffer = aimBuffer[0] == '+' ? aimBuffer + 1 : aimBuffer;
          char *tmpPtr = tmpString;
          *(tmpPtr++) = ITM_LITERAL;
          *(tmpPtr++) = STRING_TIME;
          *(tmpPtr++) = stringByteLength(numBuffer);
          xcopy(tmpPtr, numBuffer, stringByteLength(numBuffer));
          _insertInProgram((uint8_t *)tmpString, stringByteLength(numBuffer) + (int32_t)(tmpPtr - tmpString));
        }

        aimBuffer[0] = '!';
        break;
      }
    }
  #endif // !TESTSUITE_BUILD
}



static void _pemCloseDateInput(void) {
  #if !defined(TESTSUITE_BUILD)
    if(nimNumberPart == NP_REAL_FLOAT_PART) {
      deleteStepsFromTo(currentStep.ram, findNextStep_ram(currentStep.ram));
      if(aimBuffer[0] != 0) {
        char *numBuffer = aimBuffer[0] == '+' ? aimBuffer + 1 : aimBuffer;
        char *tmpPtr = tmpString;
        *(tmpPtr++) = ITM_LITERAL;
        *(tmpPtr++) = STRING_DATE;

        reallocateRegister(TEMP_REGISTER_1, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
        stringToReal34(numBuffer, REGISTER_REAL34_DATA(TEMP_REGISTER_1));
        convertReal34RegisterToDateRegister(TEMP_REGISTER_1, TEMP_REGISTER_1);
        internalDateToJulianDay(REGISTER_REAL34_DATA(TEMP_REGISTER_1), REGISTER_REAL34_DATA(TEMP_REGISTER_1));

        real34ToString(REGISTER_REAL34_DATA(TEMP_REGISTER_1), tmpPtr + 1);
        *tmpPtr = stringByteLength(tmpPtr + 1);
        ++tmpPtr;

        _insertInProgram((uint8_t *)tmpString, stringByteLength(tmpPtr) + (int32_t)(tmpPtr - tmpString));
      }

      aimBuffer[0] = '!';
    }
  #endif // !TESTSUITE_BUILD
}



static void _pemCloseDmsInput(void) {
  #if !defined(TESTSUITE_BUILD)
    switch(nimNumberPart) {
      case NP_INT_10:
      case NP_REAL_FLOAT_PART: {
        deleteStepsFromTo(currentStep.ram, findNextStep_ram(currentStep.ram));
        if(aimBuffer[0] != 0) {
          char *numBuffer = aimBuffer[0] == '+' ? aimBuffer + 1 : aimBuffer;
          char *tmpPtr = tmpString;
          *(tmpPtr++) = ITM_LITERAL;
          *(tmpPtr++) = STRING_ANGLE_DMS;
          *(tmpPtr++) = stringByteLength(numBuffer);
          xcopy(tmpPtr, numBuffer, stringByteLength(numBuffer));
          _insertInProgram((uint8_t *)tmpString, stringByteLength(numBuffer) + (int32_t)(tmpPtr - tmpString));
        }

        aimBuffer[0] = '!';
        break;
      }
    }
  #endif // !TESTSUITE_BUILD
}



void insertStepInProgram(int16_t func) {
  uint32_t opBytes = (func >= 128) ? 2 : 1;

  if(programList[currentProgramNumber - 1].step < 0) {
    // attempt to modify a program in the flash memory
    displayCalcErrorMessage(ERROR_FLASH_MEMORY_WRITE_PROTECTED, ERR_REGISTER_LINE, REGISTER_X);
    return;
  }

  if(func == ITM_AIM || (!tamIsActive() && getSystemFlag(FLAG_ALPHA))) {
    pemAlpha(func);
    pemCursorIsZerothStep = false;
    return;
  }
  if(indexOfItems[func].func == addItemToBuffer || (!tamIsActive() && aimBuffer[0] != 0 && (func == ITM_CHS || func == ITM_CC || func == ITM_toINT || (nimNumberPart == NP_INT_BASE && (func == ITM_YX || func == ITM_LN || func == ITM_RCL))))) {
    pemAddNumber(func);
    return;
  }
  else if(func == ITM_CONSTpi && aimBuffer[0] != 0 && !getSystemFlag(FLAG_ALPHA) && nimNumberPart == NP_COMPLEX_INT_PART && aimBuffer[strlen(aimBuffer) - 1] == 'i') {
    strcat(aimBuffer, "3.141592653589793238462643383279503");
    pemCloseNumberInput();
    aimBuffer[0] = 0;
    return;
  }
  else if(func == ITM_DMS && aimBuffer[0] != 0 && !getSystemFlag(FLAG_ALPHA) && (nimNumberPart == NP_INT_10 || nimNumberPart == NP_REAL_FLOAT_PART)) {
    _pemCloseDmsInput();
    aimBuffer[0] = 0;
    return;
  }
  if(!tamIsActive() && !tam.alpha && aimBuffer[0] != 0 && func != ITM_toHMS) {
    if(func == ITM_dotD) {
      _pemCloseDateInput();
      if(aimBuffer[0] == '!') {
        aimBuffer[0] = 0;
        return;
      }
    }
    pemCloseNumberInput();
    aimBuffer[0] = 0;
  }

  if(func < 128) {
    tmpString[0] = func;
  }
  else {
    tmpString[0] = (func >> 8) | 0x80;
    tmpString[1] =  func       & 0xff;
  }

  switch(indexOfItems[func].status & PTP_STATUS) {
    case PTP_DISABLED: {
      switch(func) {
        case ITM_KEYG:           // 1498
        case ITM_KEYX: {         // 1499
          int opLen;
          tmpString[0] = (char)((ITM_KEY >> 8) | 0x80);
          tmpString[1] = (char)( ITM_KEY       & 0xff);
          if(tam.keyAlpha) {
            uint16_t nameLength = stringByteLength(aimBuffer + AIM_BUFFER_LENGTH / 2);
            tmpString[2] = (char)INDIRECT_VARIABLE;
            tmpString[3] = (char)nameLength;
            xcopy(tmpString + 4, aimBuffer + AIM_BUFFER_LENGTH / 2, nameLength);
            opLen = nameLength + 4;
          }
          else if(tam.keyIndirect) {
            tmpString[2] = (char)INDIRECT_REGISTER;
            tmpString[3] = tam.key;
            opLen = 4;
          }
          else {
            tmpString[2] = tam.key;
            opLen = 3;
          }

          tmpString[opLen + 0] = (func == ITM_KEYX ? ITM_XEQ : ITM_GTO);
          if(tam.alpha) {
            uint16_t nameLength = stringByteLength(aimBuffer);
            tmpString[opLen + 1] = (char)(tam.indirect ? INDIRECT_VARIABLE : STRING_LABEL_VARIABLE);
            tmpString[opLen + 2] = nameLength;
            xcopy(tmpString + opLen + 3, aimBuffer, nameLength);
            _insertInProgram((uint8_t *)tmpString, nameLength + opLen + 3);
          }
          else if(tam.indirect) {
            tmpString[opLen + 1] = (char)INDIRECT_REGISTER;
            tmpString[opLen + 2] = tam.value;
            _insertInProgram((uint8_t *)tmpString, opLen + 3);
          }
          else {
            tmpString[opLen + 1] = tam.value;
            _insertInProgram((uint8_t *)tmpString, opLen + 2);
          }
          break;
        }

        case ITM_GTOP: {         // 1482
          #if !defined(DMCP_BUILD)
            stringToUtf8(indexOfItems[func].itemCatalogName, (uint8_t *)tmpString);
            printf("insertStepInProgram: %s\n", tmpString);
          #endif // DMCP_BUILD
          break;
        }

        case ITM_CLP: {          // 1425
          fnClP(NOPARAM);
          break;
        }

        case ITM_CLPALL: {       // 1426
          fnClPAll(NOT_CONFIRMED);
          break;
        }

        case ITM_BST: {          // 1734
          fnBst(NOPARAM);
          break;
        }

        case ITM_SST: {          // 1736
          fnSst(NOPARAM);
          break;
        }

        case VAR_ACC: {          // 1192
          tmpString[0] = ITM_STO;
          tmpString[1] = (char)STRING_LABEL_VARIABLE;
          tmpString[2] = 3;
          tmpString[3] = 'A';
          tmpString[4] = 'C';
          tmpString[5] = 'C';
          _insertInProgram((uint8_t *)tmpString, 6);
          break;
        }

        case VAR_ULIM:           // 1193
        case VAR_LLIM: {         // 1194
          tmpString[0] = ITM_STO;
          tmpString[1] = (char)STRING_LABEL_VARIABLE;
          tmpString[2] = 5;
          if(func == VAR_ULIM) {
            tmpString[3] = STD_UP_ARROW[0];
            tmpString[4] = STD_UP_ARROW[1];
          }
          else {
            tmpString[3] = STD_DOWN_ARROW[0];
            tmpString[4] = STD_DOWN_ARROW[1];
          }
          tmpString[5] = 'L';
          tmpString[6] = 'i';
          tmpString[7] = 'm';
          _insertInProgram((uint8_t *)tmpString, 8);
          break;
        }
      }
      break;
    }

    case PTP_NONE: {
      if(func == ITM_toHMS && aimBuffer[0] != 0 && !getSystemFlag(FLAG_ALPHA)) {
        _pemCloseTimeInput();
        if(aimBuffer[0] != '!') {
          pemCloseNumberInput();
          aimBuffer[0] = 0;
          tmpString[0] = (func >> 8) | 0x80;
          tmpString[1] =  func       & 0xff;
          _insertInProgram((uint8_t *)tmpString, 2);
        }
        aimBuffer[0] = 0;
      }
      else {
        _insertInProgram((uint8_t *)tmpString, (func >= 128) ? 2 : 1);
      }
      break;
    }

    case PTP_NUMBER_16: {
      tmpString[2] = (char)(tam.value & 0xff); // little endian
      tmpString[3] = (char)(tam.value >> 8);
      _insertInProgram((uint8_t *)tmpString, 4);
      break;
    }

    case PTP_LITERAL: {
      // nothing to do here
      break;
    }

    default: {
      if(tam.mode == tmCmp && tam.value == TEMP_REGISTER_1) {
        tmpString[opBytes    ] = (char)(real34IsZero(REGISTER_REAL34_DATA(TEMP_REGISTER_1)) ? VALUE_0 : VALUE_1);
        _insertInProgram((uint8_t *)tmpString, opBytes + 1);
      }
      else if((tam.mode == tmFlagR || tam.mode == tmFlagW) && tam.alpha && !tam.indirect) {
        tmpString[opBytes    ] = (char)SYSTEM_FLAG_NUMBER;
        tmpString[opBytes + 1] = tam.value;
        _insertInProgram((uint8_t *)tmpString, opBytes + 2);
      }
      else if(tam.alpha) {
        uint16_t nameLength = stringByteLength(aimBuffer);
        tmpString[opBytes    ] = (char)(tam.indirect ? INDIRECT_VARIABLE : STRING_LABEL_VARIABLE);
        tmpString[opBytes + 1] = nameLength;
        xcopy(tmpString + opBytes + 2, aimBuffer, nameLength);
        _insertInProgram((uint8_t *)tmpString, nameLength + opBytes + 2);
      }
      else if(tam.indirect) {
        tmpString[opBytes    ] = (char)INDIRECT_REGISTER;
        tmpString[opBytes + 1] = tam.value + (tam.dot ? FIRST_LOCAL_REGISTER : 0);
        _insertInProgram((uint8_t *)tmpString, opBytes + 2);
      }
      else {
        tmpString[opBytes    ] = tam.value + (tam.dot ? FIRST_LOCAL_REGISTER : 0);
        _insertInProgram((uint8_t *)tmpString, opBytes + 1);
      }
    }
  }

  aimBuffer[0] = 0;
}



void addStepInProgram(int16_t func) {
  if(programList[currentProgramNumber - 1].step < 0) { // attempt to modify a program in the flash memory
    if(func == ITM_CLP) {
      fnClP(NOPARAM);
    }
    else {
      displayCalcErrorMessage(ERROR_FLASH_MEMORY_WRITE_PROTECTED, ERR_REGISTER_LINE, REGISTER_X);
    }
    aimBuffer[0] = 0;
    return;
  }
  if((!pemCursorIsZerothStep) && ((aimBuffer[0] == 0 && !getSystemFlag(FLAG_ALPHA)) || tamIsActive()) && ((*currentStep.ram != ((ITM_END >> 8) | 0x80)) || (*(currentStep.ram + 1) != (ITM_END & 0xff))) && ((*currentStep.ram != 0xff) || (*(currentStep.ram + 1) != 0xff))) {
    currentStep = findNextStep(currentStep);
    ++currentLocalStepNumber;
  }
  insertStepInProgram(func);
  if((aimBuffer[0] == 0 && !getSystemFlag(FLAG_ALPHA)) || tamIsActive()) {
    currentStep = findPreviousStep(currentStep);
    --currentLocalStepNumber;
    pemCursorIsZerothStep = false;
    if((indexOfItems[func].status & PTP_STATUS) == PTP_DISABLED) {
      switch(func) {
        case VAR_ACC:            // 1192
        case VAR_ULIM:           // 1193
        case VAR_LLIM:           // 1194
        case ITM_CLP:            // 1425
        case ITM_CLPALL:         // 1426
        case ITM_GTOP:           // 1482
        case ITM_KEYG:           // 1498
        case ITM_KEYX:           // 1499
        case ITM_BST:            // 1734
        case ITM_SST: {          // 1736
          break;
        }
        default: {
          return;
        }
      }
    }
    if(!programListEnd) {
      scrollPemBackwards();
    }
  }
}



calcRegister_t findNamedLabel(const char *labelName) {
  for(uint16_t lbl = 0; lbl < numberOfLabels; lbl++) {
    if(labelList[lbl].step > 0) {
      if(labelList[lbl].program < 0) { // Flash
        uint8_t tmpLabel[16];
        readStepInFlashPgmLibrary(tmpLabel, 16, labelList[lbl].labelPointer.flash);
        xcopy(tmpString, tmpLabel + 1, *tmpLabel);
        tmpString[*tmpLabel] = 0;
        if(compareString(tmpString, labelName, CMP_NAME) == 0) {
          return lbl + FIRST_LABEL;
        }
      }
      else { // RAM
        xcopy(tmpString, labelList[lbl].labelPointer.ram + 1, *(labelList[lbl].labelPointer.ram));
        tmpString[*(labelList[lbl].labelPointer.ram)] = 0;
        if(compareString(tmpString, labelName, CMP_NAME) == 0) {
          return lbl + FIRST_LABEL;
        }
      }
    }
  }
  return INVALID_VARIABLE;
}



uint16_t getNumberOfSteps(void) {
  if(currentProgramNumber == (numberOfPrograms - numberOfProgramsInFlash) || currentProgramNumber == numberOfPrograms) {
    uint16_t numberOfSteps = 1;
    uint8_t *step = programList[currentProgramNumber - 1].instructionPointer.ram;
    while(*step != 255 || *(step + 1) != 255) { // .END.
      ++numberOfSteps;
      step = findNextStep_ram(step);
    }
    return numberOfSteps;
  }
  else {
    return abs(programList[currentProgramNumber].step - programList[currentProgramNumber - 1].step);
  }
}
