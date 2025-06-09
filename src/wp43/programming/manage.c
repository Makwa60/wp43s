// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "programming/manage.h"

#include "assign.h"
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
#include "programming/lblGtoXeq.h"
#include "programming/nextStep.h"
#include "realType.h"
#include "registers.h"
#include "registerValueConversions.h"
#include "sort.h"
#include "ui/bufferize.h"
#include "ui/cursor.h"
#include "ui/genericEditor.h"
#include "ui/keyboard.h"
#include "ui/screen.h"
#include "ui/softmenus.h"
#include "ui/tam.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#if defined(PC_BUILD)
#include "gtkGui.h"
#endif

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



bool isAtEndOfPrograms(const uint8_t *step) {
  return (step == NULL) || (*step == 255 && *(step + 1) == 255);
}



bool checkOpCodeOfStep(const uint8_t *step, uint16_t op) {
  if(op < 128) {
    return step && *step == op;
  }
  else {
    return step && *step == ((op >> 8) | 0x80) && *(step + 1) == (op & 0xff);
  }
}



void scanLabelsAndPrograms(void) {
  uint32_t stepNumber = 0;
  uint8_t *nextStep, *step = beginOfProgramMemory;

  freeWp43(labelList, sizeof(labelList_t) * numberOfLabels);
  freeWp43(programList, sizeof(programList_t) * numberOfPrograms);

  numberOfLabels = 0;
  numberOfPrograms = 1;
  while(!isAtEndOfPrograms(step)) { // .END.
    if(*step == ITM_LBL) { // LBL
      numberOfLabels++;
    }
    if(isAtEndOfProgram(step)) { // END
      numberOfPrograms++;
    }
    step = findNextStep(step);
  }

  labelList = allocWp43(sizeof(labelList_t) * numberOfLabels);
  if(labelList == NULL) {
    // unlikely
    lastErrorCode = ERROR_RAM_FULL;
    return;
  }

  programList = allocWp43(sizeof(programList_t) * numberOfPrograms);
  if(programList == NULL) {
    // unlikely
    lastErrorCode = ERROR_RAM_FULL;
    return;
  }

  numberOfLabels = 0;
  step = beginOfProgramMemory;
  programList[0].instructionPointer = step;
  programList[0].step = (0 + 1);
  numberOfPrograms = 1;
  stepNumber = 1;
  while(!isAtEndOfPrograms(step)) { // .END.
    nextStep = findNextStep(step);
    if(checkOpCodeOfStep(step, ITM_LBL)) { // LBL
      labelList[numberOfLabels].program = numberOfPrograms;
      if(*(step + 1) <= 109) { // Local label
        labelList[numberOfLabels].step = -stepNumber;
        labelList[numberOfLabels].labelPointer = step + 1;
      }
      else { // Global label
        labelList[numberOfLabels].step = stepNumber;
        labelList[numberOfLabels].labelPointer = step + 2;
      }

      labelList[numberOfLabels].instructionPointer = nextStep;
      numberOfLabels++;
    }

    if(isAtEndOfProgram(step)) { // END
      programList[numberOfPrograms].instructionPointer = step + 2;
      programList[numberOfPrograms].step = stepNumber + 1;
      numberOfPrograms++;
    }

    step = nextStep;
    stepNumber++;
  }

  //The folowing 2 lines added to address the FFFFFFFF issue in old state files
  firstFreeProgramByte = step;
  freeProgramBytes = (((uint8_t *)(ram + RAM_SIZE_IN_BLOCKS)) - firstFreeProgramByte) - 2;

  defineCurrentProgramFromCurrentStep();
}



void deleteStepsFromTo(uint8_t *from, uint8_t *to) {
  uint16_t opSize = to - from;

  xcopy(from, to, (firstFreeProgramByte - to) + 2);
  firstFreeProgramByte -= opSize;
  freeProgramBytes += opSize;
  scanLabelsAndPrograms();
}



static void _removeLabelsAssignments() {
  int16_t i;
  char label[15];
  uint8_t labelLength;
  for (i=0; i<numberOfLabels; i++) {
    if((labelList[i].program == currentProgramNumber) && (labelList[i].step > 0)) {
      labelLength = labelList[i].labelPointer[0];
      xcopy(label, labelList[i].labelPointer + 1, labelList[i].labelPointer[0]);
      label[labelLength]=0;
      removeUserItemAssignments(ITM_XEQ,label);   // Remove label assignments
    }
  }
}



void fnClPAll(uint16_t confirmation) {
  if(confirmation == NOT_CONFIRMED) {
    setConfirmationMode(fnClPAll);
  }
  else {
    // Remove assignments of all global labels, before deleting all programs
    removeUserItemAssignments(ITM_XEQ,"");   // Remove all labels assignments

    resizeProgramMemory(TO_BYTES(1)); // 1 block for an empty program
    currentStep                   = beginOfProgramMemory;
    firstFreeProgramByte          = beginOfProgramMemory;
    firstDisplayedStep            = beginOfProgramMemory;
    firstDisplayedLocalStepNumber = 0;
    labelList                     = NULL;
    programList                   = NULL;
    *(beginOfProgramMemory + 0) = 255; // .END.
    *(beginOfProgramMemory + 1) = 255; // .END.
    freeProgramBytes            = 2;

    currentLocalStepNumber        = 1;
    firstDisplayedLocalStepNumber = 0;

    scanLabelsAndPrograms();
    if(programRunStop != PGM_RUNNING) {
      temporaryInformation = TI_CONFIRM_COMPLETED;
    } else {
      temporaryInformation = TI_NO_INFO;
    }
    programRunStop                = PGM_STOPPED;
  }
}



static int _clearProgram(void) {
  if(beginOfCurrentProgram == beginOfProgramMemory && (endOfCurrentProgram >= firstFreeProgramByte || (*endOfCurrentProgram == 255 && *(endOfCurrentProgram + 1) == 255))) { // There is only one program in memory
    fnClPAll(CONFIRMED);
    return 1;
  }
  else {
    // Remove assignments of global labels in the program being deleted, before deleting the program
    _removeLabelsAssignments();

    uint16_t savedCurrentProgramNumber = currentProgramNumber;

    deleteStepsFromTo(beginOfCurrentProgram, endOfCurrentProgram - ((currentProgramNumber == numberOfPrograms) ? 2 : 0));
    scanLabelsAndPrograms();
    // unlikely fails

    if(savedCurrentProgramNumber >= numberOfPrograms) { // The last program
      goToPgmStep(numberOfPrograms - 1, 1);
    }
    else { // Not the last program
      goToPgmStep(savedCurrentProgramNumber, 1);
    }
    return 0;
  }
}



void fnClP2(uint16_t label) {
  if(label == 0 && !tam.alpha && tam.digitsSoFar == 0) {
    uint16_t savedCurrentProgramNumber = currentProgramNumber;
    const int result = _clearProgram();
    if(result == 1 && savedCurrentProgramNumber <= 1) {
      fnGotoDot(1);
    }
  }
  else if(label >= FIRST_LABEL && label <= LAST_LABEL) {
    const uint16_t savedCurrentLocalStepNumber = currentLocalStepNumber;
    uint16_t savedCurrentProgramNumber = currentProgramNumber;
    fnGoto(label);
    const uint16_t programNumberToDelete = currentProgramNumber;
    const int result = _clearProgram();
    switch(result) {
      case 2: {
        fnGotoDot(programList[savedCurrentProgramNumber - 1].step + savedCurrentLocalStepNumber - 1);
        break;
      }
      case 1: {
        if(savedCurrentProgramNumber <= 1) { // RAM
          fnGotoDot(1);
        }
        break;
      }
      case 0: {
        if(programNumberToDelete != savedCurrentProgramNumber) {
          if(programNumberToDelete < savedCurrentProgramNumber) {
            --savedCurrentProgramNumber;
          }
          fnGotoDot(programList[savedCurrentProgramNumber - 1].step + savedCurrentLocalStepNumber - 1);
          break;
        }
        break;
      }
      default: {
        break;
      }
    }
  }
  else {
    displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "label %" PRIu16 " is not a global label", label);
      moreInfoOnError("In function fnClP:", errorMessage, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
  }
}



#if CLP_WITH_MENU != 0
void fnClP(uint16_t label) {
  fnClP2(label);
}
#else /* CLP_WITH_MENU != 0 */
void fnClP(uint16_t confirmation) {
  if(confirmation == NOT_CONFIRMED) {
    setConfirmationMode(fnClP);
  }
  else {
    _clearProgram();
  }
}
#endif /* CLP_WITH_MENU != 0 */



static uint32_t _getProgramSize(void) {
  if(currentProgramNumber == numberOfPrograms) {
    uint16_t numberOfSteps = 1;
    uint8_t *step = programList[currentProgramNumber - 1].instructionPointer;
    while(!isAtEndOfPrograms(step)) { // .END.
      ++numberOfSteps;
      step = findNextStep(step);
    }
    return (uint32_t)(step - programList[currentProgramNumber - 1].instructionPointer + 2);
  }
  else {
    return (uint32_t)(programList[currentProgramNumber].instructionPointer - programList[currentProgramNumber - 1].instructionPointer);
  }
}



void defineCurrentProgramFromGlobalStepNumber(int16_t globalStepNumber) {
  currentProgramNumber = 0;
  while(globalStepNumber >= programList[currentProgramNumber].step) {
    currentProgramNumber++;
    if(currentProgramNumber >= numberOfPrograms) {
      break;
    }
  }

  if(currentProgramNumber == numberOfPrograms) {
    endOfCurrentProgram = programList[currentProgramNumber - 1].instructionPointer + _getProgramSize();
  }
  else {
    endOfCurrentProgram = programList[currentProgramNumber].instructionPointer;
  }
  beginOfCurrentProgram = programList[currentProgramNumber - 1].instructionPointer;
}



void defineCurrentProgramFromCurrentStep(void) {
  if(beginOfProgramMemory <= currentStep && currentStep <= firstFreeProgramByte) {
    currentProgramNumber = 0;
    while(currentStep >= programList[currentProgramNumber].instructionPointer) {
      currentProgramNumber++;
      if(currentProgramNumber >= numberOfPrograms) {
        break;
      }
    }

    if(currentProgramNumber >= numberOfPrograms) {
      endOfCurrentProgram = programList[currentProgramNumber - 1].instructionPointer + _getProgramSize();
    }
    else {
      endOfCurrentProgram = programList[currentProgramNumber].instructionPointer;
    }
    beginOfCurrentProgram = programList[currentProgramNumber - 1].instructionPointer;
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
    bool     lblOrEnd;
    bool     inTamMode = tamIsActive() && programList[currentProgramNumber - 1].step > 0;
    uint16_t numberOfSteps = getNumberOfSteps();
    uint16_t linesOfCurrentStep = 1;

    if(calcMode != cmPem && !(calcMode == cmConfirmation && previousCalcMode == cmPem)) {
      calcMode = cmPem;
      pushSmStackMode(smPem);  // Select PEM softmenu stack with MyPFN default menu

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
    step                     = firstDisplayedStep;
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

    for(line=firstLine; line<7; line++) {
      nextStep = findNextStep(step);
      //uint16_t stepSize = (uint16_t)(nextStep - step);
      sprintf(tmpString, "%04d:" STD_SPACE_4_PER_EM, firstDisplayedLocalStepNumber + line - lineOffset + lineOffsetTam);
      if(firstDisplayedStepNumber + line - lineOffset == currentStepNumber) {
        tamOverPemYPos = Y_POSITION_OF_REGISTER_T_LINE + 21 * line;
        showString(tmpString, &standardFont, 1, tamOverPemYPos, ((pemCursorIsZerothStep && !tamIsActive() && aimBuffer[0] == 0) || (tamIsActive() && (programList[currentProgramNumber - 1].step > 0))) ? vmNormal : vmReverse, false, true);
        currentStep = step;
      }
      else {
        showString(tmpString, &standardFont, 1, Y_POSITION_OF_REGISTER_T_LINE + 21 * line, vmNormal,  false, true);
      }
      lblOrEnd = checkOpCodeOfStep(step, ITM_LBL) || isAtEndOfProgram(step) || isAtEndOfPrograms(step);
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
      decodeOneStep(step);
      if(firstDisplayedStepNumber + line - lineOffset == currentStepNumber && !tamIsActive()) {
        if(getSystemFlag(FLAG_ALPHA)) {
          insertAlphaCursor(tam.function == ITM_REM ? 6 : 2);
          char *tstr = tmpString + stringByteLength(tmpString);
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
      const char *endStr = NULL;
      while(offset <= 1500 && (*(endStr = stringAfterPixels(tmpString + offset, &standardFont, 337, false, false)) != 0)) {
        int lineByteLength = endStr - (tmpString + offset);
        numberOfExtraLines++;
        xcopy(tmpString + offset + 300, tmpString + offset + lineByteLength, stringByteLength(endStr) + 1);
        tmpString[offset + lineByteLength] = 0;
        offset += 300;
      }
      stepsThatWouldBeDisplayed -= numberOfExtraLines;
      if(firstDisplayedStepNumber + line - lineOffset == currentStepNumber) {
        linesOfCurrentStep += numberOfExtraLines;
      }

      showString(tmpString, &standardFont, lblOrEnd ? 42 : 62, Y_POSITION_OF_REGISTER_T_LINE + 21 * line, vmNormal,  false, false);
      offset = 300;
      while(numberOfExtraLines && line <= 5) {
        showString(tmpString + offset, &standardFont, 62, Y_POSITION_OF_REGISTER_T_LINE + 21 * (++line), vmNormal,  false, false);
        numberOfExtraLines--;
        offset += 300;
        lineOffset++;
      }

      if(isAtEndOfProgram(step)) {
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

    if(calcMode == cmConfirmation) {
      refreshRegisterLine(REGISTER_X);
    }

    if(lastErrorCode != ERROR_NONE) {
      refreshRegisterLine(errorMessageRegisterLine);
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
  //#define printarr(fmt, dat, len)	for (uint16_t i = 0; i < len; i++) printf(fmt, dat[i])
  //printf("**[DL]** _insertInProgram: ");
  //printarr("%d ", dat, sizeInBytes);
  //printf("\n");fflush(stdout);
  int16_t _dynamicMenuItem = dynamicMenuItem;
  uint16_t globalStepNumber;
  if(freeProgramBytes < sizeInBytes) {
    uint8_t *oldBeginOfProgramMemory = beginOfProgramMemory;
    uint32_t programSizeInBytes = TO_BYTES(RAM_SIZE_IN_BLOCKS - TO_WP43MEMPTR(beginOfProgramMemory));
    uint32_t newProgramSizeInBytes = TO_BYTES(TO_BLOCKS(programSizeInBytes - freeProgramBytes + sizeInBytes));
    freeProgramBytes      += newProgramSizeInBytes - programSizeInBytes;
    resizeProgramMemory(newProgramSizeInBytes);
    currentStep           = currentStep           - oldBeginOfProgramMemory + beginOfProgramMemory;
    firstDisplayedStep    = firstDisplayedStep    - oldBeginOfProgramMemory + beginOfProgramMemory;
    beginOfCurrentProgram = beginOfCurrentProgram - oldBeginOfProgramMemory + beginOfProgramMemory;
    endOfCurrentProgram   = endOfCurrentProgram   - oldBeginOfProgramMemory + beginOfProgramMemory;
  }
  for(uint8_t *pos = firstFreeProgramByte + 1 + sizeInBytes; pos > currentStep; --pos) {
    *pos = *(pos - sizeInBytes);
  }
  for(uint16_t i = 0; i < sizeInBytes; ++i) {
    *(currentStep++) = *(dat++);
  }
  firstFreeProgramByte    += sizeInBytes;
  freeProgramBytes        -= sizeInBytes;
  currentLocalStepNumber  += 1;
  endOfCurrentProgram     += sizeInBytes;
  globalStepNumber = currentLocalStepNumber + programList[currentProgramNumber - 1].step - 1;
  scanLabelsAndPrograms();
  dynamicMenuItem = -1;
  goToGlobalStep(globalStepNumber);
  dynamicMenuItem = _dynamicMenuItem;
}


#if !defined(TESTSUITE_BUILD)
static void _closeAlphaMenus(void) {
  popSmStackMode();  // Return to previous softmenu stack which should be smPem
  return;
}
#endif // !TESTSUITE_BUILD


void pemAlpha(int16_t item) {
  #if !defined(TESTSUITE_BUILD)

    bool editCommand = false;
    if(item == ITM_EDIT) {
      int16_t aimFunc = currentStep[0];
      if(aimFunc & 0x80) {
        aimFunc &= 0x7f;
        aimFunc <<= 8;
        aimFunc |= currentStep[1];
      }
      tam.function = aimFunc;
      decodeOneStep(currentStep);
      uint16_t ll = stringByteLength(tmpString);
      if(aimFunc == ITM_LITERAL)  { // literal
        xcopy(aimBuffer, tmpString + 2, ll);        //purposely overshoot aimbuffer, as there is sufficient space
        aimBuffer[ll - 2 - 2] = 0;
        alphaCursor = stringGlyphLength(aimBuffer);
        deleteStepsFromTo(currentStep, findNextStep(currentStep));
        editCommand = true;
        item = 0;
      }
      else if(aimFunc == ITM_REM)  { // REM
        xcopy(aimBuffer, tmpString + 6, ll);        //purposely overshoot aimbuffer, as there is sufficient space
        aimBuffer[ll - 2 - 6] = 0;
        alphaCursor = stringGlyphLength(aimBuffer);
        deleteStepsFromTo(currentStep, findNextStep(currentStep));
        tam.function = aimFunc;
        editCommand = true;
        item = aimFunc;
      }
      else {
        aimBuffer[0] = 0;
        return;
      }
    }

    if(!getSystemFlag(FLAG_ALPHA)) {
      shiftF = false;
      shiftG = false;
      if(!editCommand) {
        aimBuffer[0] = 0;
        alphaCursor = 0;
      }
      alphaCase = AC_UPPER;
      nextChar = NC_NORMAL;

      #if defined(PC_BUILD)
        //setAlphaCaseToCapsLockState();  // Reflect caps lock key status
      #endif

      pushSmStackMode(smAim);  // Select AIM softmenu stack with MyAlpha default menu

      setSystemFlag(FLAG_ALPHA);

      calcModeUpdateGui();

      if(tam.function < 128) { // literal
        tmpString[0] = tam.function;
        tmpString[1] = (char)STRING_LABEL_VARIABLE;
        tmpString[2] = 0;
        _insertInProgram((uint8_t *)tmpString, 3);
      }
      else { // rem
        tmpString[0] = (tam.function >> 8) | 0x80;
        tmpString[1] =  tam.function       & 0x7f;
        tmpString[2] = (char)STRING_LABEL_VARIABLE;
        tmpString[3] = 0;
        _insertInProgram((uint8_t *)tmpString, 4);
      }
      --currentLocalStepNumber;
      currentStep = findPreviousStep(currentStep);
    }
    if(indexOfItems[item].func == addItemToBuffer) {
      int32_t len = stringByteLength(aimBuffer);
      if(alphaCase == AC_LOWER) {
        if(ITM_A <= item && item <= ITM_Z) {
          item += (ITM_a - ITM_A);
        }
        else if(ITM_ALPHA <= item && item <= ITM_OMEGA) {
          item += (ITM_alpha - ITM_ALPHA);
        }
      }
      if ((nextChar == NC_NORMAL) || ((item != ITM_DOWN_ARROW) && (item != ITM_UP_ARROW))) {
        item = convertItemToSubOrSup(item, nextChar);
        if(len < (256 - stringByteLength(indexOfItems[item].itemSoftmenuName)) && stringGlyphLength(aimBuffer) < 196) {
          insertAlphaCharacter(item, &alphaCursor);
        }
      }
    }
    else if(item == ITM_BACKSPACE) {
      if(aimBuffer[0] == 0) {
        deleteStepsFromTo(currentStep, findNextStep(currentStep));
        clearSystemFlag(FLAG_ALPHA);
        calcModeUpdateGui();
        _closeAlphaMenus();
        return;
      }
      else if(alphaCursor > 0) {
          deleteAlphaCharacter(&alphaCursor);
      }
    }
    else if(item == ITM_EXITALLNP) {
      fnExitAllMenus(NOPARAM);
      setSystemFlag(FLAG_ALPHA);
      return;
    }
    else if((item == ITM_ENTER) || (item == ITM_EXIT)) {
      if ((item == ITM_EXIT) && ((softmenu[getSoftmenuId(0)].menuItem != -MNU_MyAlpha) || (softmenu[getSoftmenuId(1)].menuItem != -MNU_MyAlpha))) {
        popSoftmenu();
      } else {
        pemCloseAlphaInput();
        --firstDisplayedLocalStepNumber;
        defineFirstDisplayedStep();
        //_closeAlphaMenus();             => Not needed, already done in pemCloseAlphaInput()
      }
      return;
    }
    else if(item == ITM_USERMODE) {
      fnFlipFlag(FLAG_USER);
      return;
    }


    int16_t aimFunc = currentStep[0];
    if(aimFunc & 0x80) {
      aimFunc &= 0x7f;
      aimFunc <<= 8;
      aimFunc |= currentStep[1];
    }
    deleteStepsFromTo(currentStep, findNextStep(currentStep));
    if(aimFunc < 128) { // literal
      tmpString[0] = aimFunc;
      tmpString[1] = (char)STRING_LABEL_VARIABLE;
      tmpString[2] = stringByteLength(aimBuffer);
      xcopy(tmpString + 3, aimBuffer, stringByteLength(aimBuffer));
      _insertInProgram((uint8_t *)tmpString, stringByteLength(aimBuffer) + 3);
    }
    else { // rem
      tmpString[0] = (aimFunc >> 8) | 0x80;
      tmpString[1] =  aimFunc       & 0x7f;
      tmpString[2] = (char)STRING_LABEL_VARIABLE;
      tmpString[3] = stringByteLength(aimBuffer);
      xcopy(tmpString + 4, aimBuffer, stringByteLength(aimBuffer));
      _insertInProgram((uint8_t *)tmpString, stringByteLength(aimBuffer) + 4);
    }
    --currentLocalStepNumber;
    currentStep = findPreviousStep(currentStep);
    if(!programListEnd) {
      scrollPemBackwards();
    }
  #endif // !TESTSUITE_BUILD
}



void pemCloseAlphaInput(void) {
  #if !defined(TESTSUITE_BUILD)
    #if defined PC_BUILD
      printf("*** pemCloseAlphaInput\n");
    #endif

    aimBuffer[0] = 0;
    clearSystemFlag(FLAG_ALPHA);
    calcModeUpdateGui();
    ++currentLocalStepNumber;
    currentStep = findNextStep(currentStep);
    ++firstDisplayedLocalStepNumber;
    firstDisplayedStep = findNextStep(firstDisplayedStep);
    _closeAlphaMenus();
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
      deleteStepsFromTo(currentStep, findNextStep(currentStep));
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
      nimNumberPart = NP_EMPTY;
    }
  #endif // !TESTSUITE_BUILD
}



void pemCloseNumberInput(void) {
  #if !defined(TESTSUITE_BUILD)
    deleteStepsFromTo(currentStep, findNextStep(currentStep));
    if(aimBuffer[0] != 0) {
      char *numBuffer = aimBuffer[0] == '+' ? aimBuffer + 1 : aimBuffer;
      char *tmpPtr = tmpString;
      uint32_t inputLength = stringByteLength(numBuffer);
      bool doneWithBinaryLiteral = false;
      *(tmpPtr++) = ITM_LITERAL;
      switch(nimNumberPart) {
        //case NP_INT_16:
        case NP_INT_BASE: {
          char *basePtr = numBuffer;
          while(*basePtr != '#') {
            ++basePtr;
          }
          *(basePtr++) = 0;
          inputLength = stringByteLength(numBuffer); // we must update here since the content of numBuffer has been truncated
          if(inputLength >= sizeof(uint64_t) && numBuffer[0] != '-') {
            uint8_t base = (uint8_t)atoi(basePtr);
            uint64_t val = 0;
            *(tmpPtr++) = BINARY_SHORT_INTEGER;
            *(tmpPtr++) = (char)atoi(basePtr);
            for(unsigned int i = 0; i < inputLength; ++i) {
              if('0' <= numBuffer[i] && numBuffer[i] <= '9') {
                val *= base;
                val += numBuffer[i] - '0';
              }
              else if('A' <= numBuffer[i] && numBuffer[i] <= 'F') {
                val *= base;
                val += numBuffer[i] - 'A' + 10;
              }
              else if('a' <= numBuffer[i] && numBuffer[i] <= 'f') {
                val *= base;
                val += numBuffer[i] - 'a' + 10;
              }
            }
            for(unsigned int i = 0; i < sizeof(uint64_t); ++i) {
              *(tmpPtr++) = ((uint8_t *)(&val))[i];
            }
            _insertInProgram((uint8_t *)tmpString, (int32_t)(tmpPtr - tmpString));
            doneWithBinaryLiteral = true;
          }
          else {
            *(tmpPtr++) = STRING_SHORT_INTEGER;
            *(tmpPtr++) = (char)atoi(basePtr);
          }
          break;
        }
        case NP_REAL_FLOAT_PART:
        case NP_REAL_EXPONENT:
        case NP_FRACTION_DENOMINATOR: {
          if(inputLength >= REAL34_SIZE_IN_BYTES) {
            real34_t val;
            *(tmpPtr++) = BINARY_REAL34;
            stringToReal34(numBuffer, &val);
            for(unsigned int i = 0; i < REAL34_SIZE_IN_BYTES; ++i) {
              *(tmpPtr++) = ((uint8_t *)(&val))[i];
            }
            _insertInProgram((uint8_t *)tmpString, (int32_t)(tmpPtr - tmpString));
            doneWithBinaryLiteral = true;
          }
          else {
            *(tmpPtr++) = STRING_REAL34;
          }
          break;
        }
        case NP_COMPLEX_INT_PART:
        case NP_COMPLEX_FLOAT_PART:
        case NP_COMPLEX_EXPONENT: {
          if(inputLength >= COMPLEX34_SIZE_IN_BYTES) {
            real34_t re, im;
            char *imag = numBuffer;
            while(*imag != 'i' && *imag != 0) {
              ++imag;
            }
            if(*imag == 'i') {
              if(imag > numBuffer && *(imag - 1) == '-') {
                *imag = '-'; *(imag - 1) = 0;
              }
              else if(imag > numBuffer && *(imag - 1) == '+') {
                *imag = 0; *(imag - 1) = 0;
                ++imag;
              }
              else {
                *imag = 0;
              }
            }

            *(tmpPtr++) = BINARY_COMPLEX34;
            stringToReal34(numBuffer, &re);
            stringToReal34(imag, &im);
            for(unsigned int i = 0; i < REAL34_SIZE_IN_BYTES; ++i) {
              *(tmpPtr++) = ((uint8_t *)(&re))[i];
            }
            for(unsigned int i = 0; i < REAL34_SIZE_IN_BYTES; ++i) {
              *(tmpPtr++) = ((uint8_t *)(&im))[i];
            }
            _insertInProgram((uint8_t *)tmpString, (int32_t)(tmpPtr - tmpString));
            doneWithBinaryLiteral = true;
          }
          else {
            *(tmpPtr++) = STRING_COMPLEX34;
          }
          break;
        }
        default: {
          *(tmpPtr++) = STRING_LONG_INTEGER;
          break;
        }
      }
      if(!doneWithBinaryLiteral) {
        *(tmpPtr++) = inputLength;
        xcopy(tmpPtr, numBuffer, inputLength);
        _insertInProgram((uint8_t *)tmpString, inputLength + (int32_t)(tmpPtr - tmpString));
        pemCursorIsZerothStep = false;
      }
    }

    aimBuffer[0] = '!';
    nimNumberPart = NP_EMPTY;
  #endif // TESTSUITE_BUILD
}



static void _pemCloseTimeInput(void) {
  #if !defined(TESTSUITE_BUILD)
    switch(nimNumberPart) {
      case NP_INT_10:
      case NP_REAL_FLOAT_PART: {
        deleteStepsFromTo(currentStep, findNextStep(currentStep));
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
      deleteStepsFromTo(currentStep, findNextStep(currentStep));
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
        deleteStepsFromTo(currentStep, findNextStep(currentStep));
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

  if(func == ITM_AIM || ((tam.function == ITM_LITERAL) && (!tamIsActive() && getSystemFlag(FLAG_ALPHA)))) {
    if(aimBuffer[0] != 0 && !getSystemFlag(FLAG_ALPHA)) {
      pemCloseNumberInput();
      aimBuffer[0] = 0;
    }
    tam.function = ITM_LITERAL;
    pemAlpha(func);
    pemCursorIsZerothStep = false;
    return;
  }
  else if(func == ITM_REM || ((tam.function == ITM_REM) && (!tamIsActive() && getSystemFlag(FLAG_ALPHA)))) {
    if(aimBuffer[0] != 0 && !getSystemFlag(FLAG_ALPHA)) {
      pemCloseNumberInput();
      aimBuffer[0] = 0;
    }
    if(catalog) {      // If called from a catalog such as FNCS, exit catalog and Asm Mode
      #if !defined(TESTSUITE_BUILD)
        leaveAsmMode();
        popSoftmenu();
      #endif // !TESTSUITE_BUILD
    }
    tam.function = ITM_REM;
    pemAlpha(func);
    pemCursorIsZerothStep = false;
    return;
  }
  if(indexOfItems[func].func == addItemToBuffer || (!tamIsActive() && aimBuffer[0] != 0 && (func == ITM_CHS || func == ITM_CC || func == ITM_toINT || (nimNumberPart == NP_INT_BASE && (func == ITM_YX || func == ITM_LN || func == ITM_RCL))))) {
    pemAddNumber(func);
    return;
  }
  else if(nimNumberPart == NP_INT_BASE) {
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
        case ITM_EDIT: {         // 1948
          fnEdit(NOPARAM);
          break;
        }

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

        #if CLP_WITH_MENU == 0
          case ITM_CLP: {          // 1425
            fnClP(NOT_CONFIRMED);
            break;
          }
        #endif /* CLP_WITH_MENU == 0 */

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

        case ITM_USERMODE: {       // 1729
          fnFlipFlag(FLAG_USER);
          break;
        }

        case ITM_EXIT: {           // 1737
          fnKeyExit(NOPARAM);
          break;
        }

        case ITM_EXITALLNP: {      // 1810
          fnExitAllMenus(NOPARAM);
          break;
        }

        case ITM_ASSIGN: {         // 1411
          fnAssign(0);
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
      if(func == ITM_BESTF_NO_IND) {  // original BestF without indirection support (little endian parameter)
        tmpString[2] = (char)(tam.value & 0xff); // little endian
        tmpString[3] = (char)(tam.value >> 8);
        _insertInProgram((uint8_t *)tmpString, 4);
      }
      else {                        // new Bestf with indirection support (big endian parameter)
        if(tam.alpha && tam.indirect) {
          uint16_t nameLength = stringByteLength(aimBuffer);
          tmpString[opBytes    ] = (char)(INDIRECT_VARIABLE);
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
        tmpString[2] = (char)(tam.value >> 8);   // BIG endian
        tmpString[3] = (char)(tam.value & 0xff);
        _insertInProgram((uint8_t *)tmpString, 4);
        }
      }
      break;
    }

    case PTP_LITERAL:
    case PTP_REM: {
      // nothing to do here
      break;
    }

    default: {
      if(tam.mode == tmValue && ((indexOfItems[func].status & PTP_STATUS) == PTP_NUMBER_8_16) && tam.value > 250) {
        tmpString[opBytes    ] = (char)CNST_BEYOND_250;
        tmpString[opBytes + 1] = tam.value - 250;
        _insertInProgram((uint8_t *)tmpString, opBytes + 2);
      }
      else if(tam.mode == tmCmp && tam.value == TEMP_REGISTER_1) {
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

  if(func != ITM_EDIT) {
    aimBuffer[0] = 0;
  }
}


void insertUserItemInProgram(int16_t func, char *funcParam) {
  uint32_t opBytes=0;
  uint16_t nameLength = stringByteLength(funcParam);

  if((!pemCursorIsZerothStep) && ((aimBuffer[0] == 0 && !getSystemFlag(FLAG_ALPHA)) || tam.mode) && !isAtEndOfProgram(currentStep) && !isAtEndOfPrograms(currentStep)) {
    currentStep = findNextStep(currentStep);
    ++currentLocalStepNumber;
  }
  if(func < 128) {
    tmpString[opBytes++] = func;
  }
  else {
    tmpString[opBytes++] = (func >> 8) | 0x80;
    tmpString[opBytes++] =  func       & 0x7f;
  }

  tmpString[opBytes    ] = (char)STRING_LABEL_VARIABLE;
  tmpString[opBytes + 1] = nameLength;
  xcopy(tmpString + opBytes + 2, funcParam, nameLength);
  _insertInProgram((uint8_t *)tmpString, nameLength + opBytes + 2);

  currentStep = findPreviousStep(currentStep);
  if(currentLocalStepNumber > 1) {
    --currentLocalStepNumber;
  }
  pemCursorIsZerothStep = false;
  if(!programListEnd) {
    scrollPemBackwards();
  }
}



void addStepInProgram(int16_t func) {
  if((!pemCursorIsZerothStep) && ((aimBuffer[0] == 0 && !getSystemFlag(FLAG_ALPHA)) || tamIsActive()) && !isAtEndOfProgram(currentStep) && !isAtEndOfPrograms(currentStep)) {
    currentStep = findNextStep(currentStep);
    ++currentLocalStepNumber;
  }
  insertStepInProgram(func);
  if((aimBuffer[0] == 0 && !getSystemFlag(FLAG_ALPHA)) || tamIsActive()) {
    currentStep = findPreviousStep(currentStep);
    if(currentLocalStepNumber > 1) {
      --currentLocalStepNumber;
    }
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
        case ITM_SST:            // 1736
        case ITM_EXITALLNP: {    // 1810
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
  return findNamedLabelWithDuplicate(labelName, 0);
}



calcRegister_t findNamedLabelWithDuplicate(const char *labelName, int16_t dupNum) {
  for(uint16_t lbl = 0; lbl < numberOfLabels; lbl++) {
    if(labelList[lbl].step > 0) {
      xcopy(tmpString, labelList[lbl].labelPointer + 1, *(labelList[lbl].labelPointer));
      tmpString[*(labelList[lbl].labelPointer)] = 0;
      if(compareString(tmpString, labelName, CMP_BINARY) == 0) {
        if(dupNum <= 0) {
          return lbl + FIRST_LABEL;
        }
        else {
          --dupNum;
        }
      }
    }
  }
  return INVALID_VARIABLE;
}



uint16_t getNumberOfSteps(void) {
  if(currentProgramNumber == numberOfPrograms) {
    uint16_t numberOfSteps = 1;
    uint8_t *step = programList[currentProgramNumber - 1].instructionPointer;
    while(!isAtEndOfPrograms(step)) { // .END.
      ++numberOfSteps;
      step = findNextStep(step);
    }
    return numberOfSteps;
  }
  else {
    return abs(programList[currentProgramNumber].step - programList[currentProgramNumber - 1].step);
  }
}
