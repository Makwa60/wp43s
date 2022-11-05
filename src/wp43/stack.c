// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "stack.h"

#include "calcMode.h"
#include "charString.h"
#include "constantPointers.h"
#include "core/memory.h"
#include "mathematics/comparisonReals.h"
#include "error.h"
#include "flags.h"
#include "mathematics/matrix.h"
#include "registers.h"
#include "registerValueConversions.h"
#include "stats.h"

#include "wp43.h"

void fnClX(uint16_t unusedButMandatoryParameter) {
  clearRegister(REGISTER_X);
}



void fnClearStack(uint16_t unusedButMandatoryParameter) {
  for(calcRegister_t regist=REGISTER_X; regist<=getStackTop(); regist++) {
    clearRegister(regist);
  }
}



void fnDrop(uint16_t unusedButMandatoryParameter) {
  freeRegisterData(REGISTER_X);
  for(calcRegister_t regist=REGISTER_X; regist<getStackTop(); regist++) {
    globalRegister[regist] = globalRegister[regist + 1];
  }

  uint16_t sizeInBytes = TO_BYTES(getRegisterFullSize(getStackTop()));
  setRegisterDataPointer(getStackTop() - 1, allocWp43(sizeInBytes));
  xcopy(REGISTER_DATA(getStackTop() - 1), REGISTER_DATA(getStackTop()), sizeInBytes);
}



void liftStack(void) {
  if(getSystemFlag(FLAG_ASLIFT)) {
    if(currentInputVariable != INVALID_VARIABLE) {
      currentInputVariable |= 0x8000;
    }
    freeRegisterData(getStackTop());
    for(uint16_t i=getStackTop(); i>REGISTER_X; i--) {
      globalRegister[i] = globalRegister[i-1];
    }
  }
  else {
    freeRegisterData(REGISTER_X);
  }

  setRegisterDataPointer(REGISTER_X, allocWp43(REAL34_SIZE_IN_BYTES));
  setRegisterDataType(REGISTER_X, dtReal34, amNone);
}



void fnDropY(uint16_t unusedButMandatoryParameter) {
  freeRegisterData(REGISTER_Y);
  for(uint16_t i=REGISTER_Y; i<getStackTop(); i++) {
    globalRegister[i] = globalRegister[i+1];
  }

  uint16_t sizeInBytes = TO_BYTES(getRegisterFullSize(getStackTop()));
  void *dataPtr = allocWp43(sizeInBytes);
  if(dataPtr) {
    setRegisterDataPointer(getStackTop() - 1, dataPtr);
    xcopy(REGISTER_DATA(getStackTop() - 1), REGISTER_DATA(getStackTop()), sizeInBytes);
  }
  else {
    lastErrorCode = ERROR_RAM_FULL;
  }
}



void fnRollUp(uint16_t unusedButMandatoryParameter) {
  registerHeader_t savedRegisterHeader = globalRegister[getStackTop()];

  for(uint16_t i=getStackTop(); i>REGISTER_X; i--) {
    globalRegister[i] = globalRegister[i-1];
  }
  globalRegister[REGISTER_X] = savedRegisterHeader;
}



void fnRollDown(uint16_t unusedButMandatoryParameter) {
  registerHeader_t savedRegisterHeader = globalRegister[REGISTER_X];

  for(uint16_t i=REGISTER_X; i<getStackTop(); i++) {
    globalRegister[i] = globalRegister[i+1];
  }
  globalRegister[getStackTop()] = savedRegisterHeader;
}



void fnDisplayStack(uint16_t numberOfStackLines) {
  displayStack = numberOfStackLines;
}



static void _swapRegs(uint16_t srcReg, uint16_t regist) {
  registerHeader_t savedRegisterHeader = globalRegister[srcReg];

  if(regist <= LAST_GLOBAL_REGISTER) {
    globalRegister[srcReg] = globalRegister[regist];
    globalRegister[regist] = savedRegisterHeader;
  }
  else if(regist < FIRST_LOCAL_REGISTER + currentNumberOfLocalRegisters) {
    globalRegister[srcReg] = currentLocalRegisters[regist - FIRST_LOCAL_REGISTER];
    currentLocalRegisters[regist - FIRST_LOCAL_REGISTER] = savedRegisterHeader;
  }
  else if(regist <= LAST_LOCAL_REGISTER) {
    displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("local register .%02d is not defined!", regist - FIRST_LOCAL_REGISTER);
  }
  else if(regist <= LAST_TEMP_REGISTER) {
    displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo(errorMessage, "register %d is unsupported!", regist);
  }
  else if(regist < FIRST_NAMED_VARIABLE + numberOfNamedVariables) {
    globalRegister[srcReg] = allNamedVariables[regist - FIRST_NAMED_VARIABLE].header;
    allNamedVariables[regist - FIRST_NAMED_VARIABLE].header = savedRegisterHeader;
  }
  else {
    displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo(errorMessage, "register %d is unsupported!", regist);
  }
}



void fnSwapX(uint16_t regist) {
  _swapRegs(REGISTER_X, regist);
}



void fnSwapY(uint16_t regist) {
  _swapRegs(REGISTER_Y, regist);
}



void fnSwapZ(uint16_t regist) {
  _swapRegs(REGISTER_Z, regist);
}



void fnSwapT(uint16_t regist) {
  _swapRegs(REGISTER_T, regist);
}



void fnSwapXY(uint16_t unusedButMandatoryParameter) {
  registerHeader_t savedRegisterHeader = globalRegister[REGISTER_X];

  globalRegister[REGISTER_X] = globalRegister[REGISTER_Y];
  globalRegister[REGISTER_Y] = savedRegisterHeader;
}



void fnShuffle(uint16_t regist_order) {
  for(int i=0; i<4; i++) {
    registerHeader_t savedRegisterHeader = globalRegister[REGISTER_X + i];
    globalRegister[REGISTER_X + i] = savedStackRegister[i];
    savedStackRegister[i] = savedRegisterHeader;
  }
  for(int i=0; i<4; i++) {
    uint16_t regist_offset = (regist_order >> (i*2)) & 3;
    copySourceRegisterToDestRegister(SAVED_REGISTER_X + regist_offset, REGISTER_X + i);
  }
}



void fnFillStack(uint16_t unusedButMandatoryParameter) {
  uint16_t dataTypeX         = getRegisterDataType(REGISTER_X);
  uint16_t dataSizeXinBytes  = TO_BYTES(getRegisterFullSize(REGISTER_X));
  uint16_t tag               = getRegisterTag(REGISTER_X);

  for(uint16_t i=REGISTER_Y; i<=getStackTop(); i++) {
    freeRegisterData(i);
    setRegisterDataType(i, dataTypeX, tag);
    void *newDataPointer = allocWp43(dataSizeXinBytes);
    if(newDataPointer) {
      setRegisterDataPointer(i, newDataPointer);
      xcopy(newDataPointer, REGISTER_DATA(REGISTER_X), dataSizeXinBytes);
    }
    else {
      lastErrorCode = ERROR_RAM_FULL;
      return;
    }
  }
}



void fnGetStackSize(uint16_t unusedButMandatoryParameter) {
  longInteger_t stack;

  liftStack();

  longIntegerInit(stack);
  uIntToLongInteger(getSystemFlag(FLAG_SSIZE8) ? 8 : 4, stack);
  convertLongIntegerToLongIntegerRegister(stack, REGISTER_X);
  longIntegerFree(stack);
}



void saveForUndo(void) {
  if(((calcMode == cmNim || calcMode == cmAim || calcMode == cmMim) && thereIsSomethingToUndo) || calcMode == cmNoUndo) {
    #if defined(DEBUGUNDO)
      if(thereIsSomethingToUndo) {
        printf(">>> saveForUndo; calcMode = %i, nothing stored as there is something to undo\n", calcMode);
      }
      if(calcMode == cmNim || calcMode == cmAim || calcMode == cmMim || calcMode == cmNoUndo) {
        printf(">>> saveForUndo; calcMode = %i, nothing stored, wrong mode\n", calcMode);
      }
    #endif // DEBUGUNDO
    return;
  }
  #if defined(DEBUGUNDO)
    printf(">>> in saveForUndo, saving; calcMode = %i pre:thereIsSomethingToUndo = %i ;", calcMode, thereIsSomethingToUndo);
    printf("Clearing TEMP_REGISTER_2_SAVED_STATS\n\n");
  #endif // DEBUGUNDO

  clearRegister(TEMP_REGISTER_2_SAVED_STATS); //clear it here for every saveForUndo call, and explicitly set it in fnEditMatrix() and fnEqSolvGraph() only
  SAVED_SIGMA_LAct = 0;

  savedSystemFlags = systemFlags;

  if(currentInputVariable != INVALID_VARIABLE) {
    if(currentInputVariable & 0x8000) {
      currentInputVariable |= 0x4000;
    }
    else {
      currentInputVariable &= 0xbfff;
    }
  }

  if(entryStatus & 0x01) {
    entryStatus |= 0x02;
  }
  else {
    entryStatus &= 0xfd;
  }

  for(calcRegister_t regist=getStackTop(); regist>=REGISTER_X; regist--) {
    copySourceRegisterToDestRegister(regist, SAVED_REGISTER_X - REGISTER_X + regist);
    if(lastErrorCode == ERROR_RAM_FULL) {
      #if defined(PC_BUILD)
        printf("In function saveForUndo: not enough space for saving register #%" PRId16 "!\n", regist);
        fflush(stdout);
      #endif // PC_BUILD
      goto failed;
    }
  }

  copySourceRegisterToDestRegister(REGISTER_L, SAVED_REGISTER_L);
  if(lastErrorCode == ERROR_RAM_FULL) {
    #if defined(PC_BUILD)
      printf("In function saveForUndo: not enough space for saving register L!\n");
      fflush(stdout);
    #endif // PC_BUILD
    goto failed;
  }

  lrSelectionUndo = lrSelection;
  if(statisticalSumsPointer == NULL) { // There are no statistical sums to save for undo
    if(savedStatisticalSumsPointer != NULL) {
      freeWp43(savedStatisticalSumsPointer, NUMBER_OF_STATISTICAL_SUMS * REAL_SIZE_IN_BYTES);
      savedStatisticalSumsPointer = NULL;
    }
  }
  else { // There are statistical sums to save for undo
    lrChosenUndo = lrChosen;
    if(savedStatisticalSumsPointer == NULL) {
      savedStatisticalSumsPointer = allocWp43(NUMBER_OF_STATISTICAL_SUMS * REAL_SIZE_IN_BYTES);
    }
    xcopy(savedStatisticalSumsPointer, statisticalSumsPointer, NUMBER_OF_STATISTICAL_SUMS * TO_BYTES(TO_BLOCKS(REAL_SIZE_IN_BYTES)));
  }

  thereIsSomethingToUndo = true;
  return;

failed:
  for(calcRegister_t regist=getStackTop(); regist>=REGISTER_X; regist--) {
    clearRegister(SAVED_REGISTER_X - REGISTER_X + regist);
  }
  clearRegister(SAVED_REGISTER_L);
  thereIsSomethingToUndo = false;
  lastErrorCode = ERROR_RAM_FULL;
  return;
}



void fnUndo(uint16_t unusedButMandatoryParameter) {
  if(thereIsSomethingToUndo) {
    undo();
  }
}



void undo(void) {
  #if defined(DEBUGUNDO)
    printf(">>> Undoing, calcMode = %i ...", calcMode);
  #endif // DEBUGUNDO
  recallStatsMatrix();

  if(currentInputVariable != INVALID_VARIABLE) {
    if(currentInputVariable & 0x4000) {
      currentInputVariable |= 0x8000;
    }
    else {
      currentInputVariable &= 0x7fff;
    }
  }

  if(entryStatus & 0x02) {
    entryStatus |= 0x01;
  }
  else {
    entryStatus &= 0xfe;
  }

  if(SAVED_SIGMA_LAct == +1 && statisticalSumsPointer != NULL) {
    fnSigma(-1);
  }
  else if(SAVED_SIGMA_LAct == -1) {
    clearRegister(REGISTER_X);
    clearRegister(REGISTER_Y);
    convertRealToReal34ResultRegister(&SAVED_SIGMA_LASTX, REGISTER_X);             // Can use stack, as the stack will be undone below
    convertRealToReal34ResultRegister(&SAVED_SIGMA_LASTY, REGISTER_Y);
    fnSigma(+1);
  }

  systemFlags = savedSystemFlags;
  synchronizeLetteredFlags();

  for(calcRegister_t regist=getStackTop(); regist>=REGISTER_X; regist--) {
    copySourceRegisterToDestRegister(SAVED_REGISTER_X - REGISTER_X + regist, regist);
  }

  copySourceRegisterToDestRegister(SAVED_REGISTER_L, REGISTER_L);

  lrSelection = lrSelectionUndo;
  if(savedStatisticalSumsPointer == NULL) { // There are no statistical sums to restore
    if(statisticalSumsPointer != NULL) {
      freeWp43(statisticalSumsPointer, NUMBER_OF_STATISTICAL_SUMS * REAL_SIZE_IN_BYTES);
      statisticalSumsPointer = NULL;
      lrChosen = 0;
    }
  }
  else { // There are statistical sums to restore
    lrChosen = lrChosenUndo;
    if(statisticalSumsPointer == NULL) {
      statisticalSumsPointer = allocWp43(NUMBER_OF_STATISTICAL_SUMS * REAL_SIZE_IN_BYTES);
    }
    xcopy(statisticalSumsPointer, savedStatisticalSumsPointer, NUMBER_OF_STATISTICAL_SUMS * TO_BYTES(TO_BLOCKS(REAL_SIZE_IN_BYTES)));
  }

  SAVED_SIGMA_LAct = 0;
  thereIsSomethingToUndo = false;
  clearRegister(TEMP_REGISTER_2_SAVED_STATS);
  #if defined(DEBUGUNDO)
    printf(">>> Undone, calcMode = %i\n", calcMode);
  #endif // DEBUGUNDO

}
