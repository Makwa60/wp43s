// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "recall.h"

#include "charString.h"
#include "core/memory.h"
#include "debug.h"
#include "defines.h"
#include "error.h"
#include "flags.h"
#include "items.h"
#include "mathematics/compare.h"
#include "matrix.h"
#include "registerValueConversions.h"
#include "registers.h"
#include "stack.h"
#include "store.h"
#include <stdbool.h>

#include "wp43.h"

#if !defined(TESTSUITE_BUILD)
  static bool recallElementReal(real34Matrix_t *matrix) {
    const int16_t i = getIRegisterAsInt(true);
    const int16_t j = getJRegisterAsInt(true);

    liftStack();
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE, amNone);
    real34Copy(&matrix->matrixElements[i * matrix->header.matrixColumns + j], REGISTER_REAL34_DATA(REGISTER_X));
    return false;
  }

  static bool recallElementComplex(complex34Matrix_t *matrix) {
    const int16_t i = getIRegisterAsInt(true);
    const int16_t j = getJRegisterAsInt(true);

    liftStack();
    reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE, amNone);
    complex34Copy(&matrix->matrixElements[i * matrix->header.matrixColumns + j], REGISTER_COMPLEX34_DATA(REGISTER_X));
    return false;
  }
#endif // !TESTSUITE_BUILD



void fnRecall(uint16_t regist) {
  if(regInRange(regist)) {
    if(REGISTER_X <= regist && regist <= getStackTop()) {
      copySourceRegisterToDestRegister(regist, TEMP_REGISTER_1);
      liftStack();
      copySourceRegisterToDestRegister(TEMP_REGISTER_1, REGISTER_X);
    }
    else {
      if(getSystemFlag(FLAG_ASLIFT)) {
        fnRollUp(NOPARAM);
      }
      copySourceRegisterToDestRegister(regist, REGISTER_X);
      if(getRegisterDataType(REGISTER_X) == dtShortInteger) {
        *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) &= shortIntegerMask;
      }
    }
  }
}



void fnLastX(uint16_t unusedButMandatoryParameter) {
  fnRecall(REGISTER_L);
}



void fnRecallAdd(uint16_t regist) {
  if(regInRange(regist)) {
    if(programRunStop == PGM_RUNNING && regist == REGISTER_L) {
      copySourceRegisterToDestRegister(REGISTER_L, SAVED_REGISTER_L);
      if(lastErrorCode != ERROR_NONE) {
        return;
      }
    }
    if(!saveLastX()) {
      return;
    }
    if(programRunStop == PGM_RUNNING) {
      copySourceRegisterToDestRegister(REGISTER_Y, SAVED_REGISTER_Y);
    }
    copySourceRegisterToDestRegister(REGISTER_X, REGISTER_Y);
    copySourceRegisterToDestRegister(regist == REGISTER_Y ? SAVED_REGISTER_Y : regist == REGISTER_L ? SAVED_REGISTER_L : regist, REGISTER_X);
    if(getRegisterDataType(REGISTER_X) == dtShortInteger) {
      *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) &= shortIntegerMask;
    }

    addition[getRegisterDataType(REGISTER_X)][getRegisterDataType(REGISTER_Y)]();

    copySourceRegisterToDestRegister(SAVED_REGISTER_Y, REGISTER_Y);

    adjustResult(REGISTER_X, false, true, REGISTER_X, regist, -1);
  }
}



void fnRecallSub(uint16_t regist) {
  if(regInRange(regist)) {
    if(programRunStop == PGM_RUNNING && regist == REGISTER_L) {
      copySourceRegisterToDestRegister(REGISTER_L, SAVED_REGISTER_L);
      if(lastErrorCode != ERROR_NONE) {
        return;
      }
    }
    if(!saveLastX()) {
      return;
    }
    if(programRunStop == PGM_RUNNING) {
      copySourceRegisterToDestRegister(REGISTER_Y, SAVED_REGISTER_Y);
    }
    copySourceRegisterToDestRegister(REGISTER_X, REGISTER_Y);
    copySourceRegisterToDestRegister(regist == REGISTER_Y ? SAVED_REGISTER_Y : regist == REGISTER_L ? SAVED_REGISTER_L : regist, REGISTER_X);
    if(getRegisterDataType(REGISTER_X) == dtShortInteger) {
      *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) &= shortIntegerMask;
    }

    subtraction[getRegisterDataType(REGISTER_X)][getRegisterDataType(REGISTER_Y)]();

    copySourceRegisterToDestRegister(SAVED_REGISTER_Y, REGISTER_Y);

    adjustResult(REGISTER_X, false, true, REGISTER_X, regist, -1);
  }
}



void fnRecallMult(uint16_t regist) {
  if(regInRange(regist)) {
    if(programRunStop == PGM_RUNNING && regist == REGISTER_L) {
      copySourceRegisterToDestRegister(REGISTER_L, SAVED_REGISTER_L);
      if(lastErrorCode != ERROR_NONE) {
        return;
      }
    }
    if(!saveLastX()) {
      return;
    }
    if(programRunStop == PGM_RUNNING) {
      copySourceRegisterToDestRegister(REGISTER_Y, SAVED_REGISTER_Y);
    }
    copySourceRegisterToDestRegister(REGISTER_X, REGISTER_Y);
    copySourceRegisterToDestRegister(regist == REGISTER_Y ? SAVED_REGISTER_Y : regist == REGISTER_L ? SAVED_REGISTER_L : regist, REGISTER_X);
    if(getRegisterDataType(REGISTER_X) == dtShortInteger) {
      *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) &= shortIntegerMask;
    }

    multiplication[getRegisterDataType(REGISTER_X)][getRegisterDataType(REGISTER_Y)]();

    copySourceRegisterToDestRegister(SAVED_REGISTER_Y, REGISTER_Y);

    adjustResult(REGISTER_X, false, true, REGISTER_X, regist, -1);
  }
}



void fnRecallDiv(uint16_t regist) {
  if(regInRange(regist)) {
    if(programRunStop == PGM_RUNNING && regist == REGISTER_L) {
      copySourceRegisterToDestRegister(REGISTER_L, SAVED_REGISTER_L);
      if(lastErrorCode != ERROR_NONE) {
        return;
      }
    }
    if(!saveLastX()) {
      return;
    }
    if(programRunStop == PGM_RUNNING) {
      copySourceRegisterToDestRegister(REGISTER_Y, SAVED_REGISTER_Y);
    }
    copySourceRegisterToDestRegister(REGISTER_X, REGISTER_Y);
    copySourceRegisterToDestRegister(regist == REGISTER_Y ? SAVED_REGISTER_Y : regist == REGISTER_L ? SAVED_REGISTER_L : regist, REGISTER_X);
    if(getRegisterDataType(REGISTER_X) == dtShortInteger) {
      *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) &= shortIntegerMask;
    }

    division[getRegisterDataType(REGISTER_X)][getRegisterDataType(REGISTER_Y)]();

    copySourceRegisterToDestRegister(SAVED_REGISTER_Y, REGISTER_Y);

    adjustResult(REGISTER_X, false, true, REGISTER_X, regist, -1);
  }
}



void fnRecallMin(uint16_t regist) {
  if(regInRange(regist)) {
    if(programRunStop == PGM_RUNNING && regist == REGISTER_L) {
      copySourceRegisterToDestRegister(REGISTER_L, SAVED_REGISTER_L);
      if(lastErrorCode != ERROR_NONE) {
        return;
      }
    }
    if(!saveLastX()) {
      return;
    }
    if(regist >= FIRST_RESERVED_VARIABLE && regist < LAST_RESERVED_VARIABLE && allReservedVariables[regist - FIRST_RESERVED_VARIABLE].header.pointerToRegisterData == WP43_NULL) {
      copySourceRegisterToDestRegister(regist == REGISTER_L ? SAVED_REGISTER_L : regist, TEMP_REGISTER_1);
      regist = TEMP_REGISTER_1;
    }
    registerMin(REGISTER_X, regist, REGISTER_X);
  }
}



void fnRecallMax(uint16_t regist) {
  if(regInRange(regist)) {
    if(programRunStop == PGM_RUNNING && regist == REGISTER_L) {
      copySourceRegisterToDestRegister(REGISTER_L, SAVED_REGISTER_L);
      if(lastErrorCode != ERROR_NONE) {
        return;
      }
    }
    if(!saveLastX()) {
      return;
    }
    if(regist >= FIRST_RESERVED_VARIABLE && regist < LAST_RESERVED_VARIABLE && allReservedVariables[regist - FIRST_RESERVED_VARIABLE].header.pointerToRegisterData == WP43_NULL) {
      copySourceRegisterToDestRegister(regist == REGISTER_L ? SAVED_REGISTER_L : regist, TEMP_REGISTER_1);
      regist = TEMP_REGISTER_1;
    }
    registerMax(REGISTER_X, regist, REGISTER_X);
  }
}



void fnRecallConfig(uint16_t regist) {
  if(getRegisterDataType(regist) == dtConfig) {
    dtConfigDescriptor_t *configToRecall = REGISTER_CONFIG_DATA(regist);

    xcopy(kbd_usr, configToRecall->kbd_usr, sizeof(kbd_usr));
    recallFromDtConfigDescriptor(shortIntegerMode);
    recallFromDtConfigDescriptor(shortIntegerWordSize);
    recallFromDtConfigDescriptor(displayFormat);
    recallFromDtConfigDescriptor(displayFormatDigits);
    recallFromDtConfigDescriptor(groupingGap);
    recallFromDtConfigDescriptor(currentAngularMode);
    recallFromDtConfigDescriptor(lrSelection);
    recallFromDtConfigDescriptor(lrChosen);
    recallFromDtConfigDescriptor(denMax);
    recallFromDtConfigDescriptor(displayStack);
    recallFromDtConfigDescriptor(firstGregorianDay);
    recallFromDtConfigDescriptor(roundingMode);
    recallFromDtConfigDescriptor(systemFlags);
    synchronizeLetteredFlags();
  }

  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "data type %s cannot be used to recall a configuration!", getRegisterDataTypeName(regist, false, false));
      moreInfoOnError("In function fnRecallConfig:", errorMessage, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
  }
}



void fnRecallStack(uint16_t regist) {
  uint16_t size = getSystemFlag(FLAG_SSIZE8) ? 8 : 4;

  if(regist + size >= REGISTER_X && regist < REGISTER_X) {
    displayCalcErrorMessage(ERROR_STACK_CLASH, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "Cannot execute RCLS, destination register would overlap the stack: %d", regist);
      moreInfoOnError("In function fnRecallStack:", errorMessage, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
  }
  else if((regist >= REGISTER_X && regist < FIRST_LOCAL_REGISTER) || regist + size > FIRST_LOCAL_REGISTER + currentNumberOfLocalRegisters) {
    displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "Cannot execute RCLS, destination register is out of range: %d", regist);
      moreInfoOnError("In function fnRecallStack:", errorMessage, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
  }
  else {
    int i;

    if(!saveLastX()) {
      return;
    }

    for(i=0; i<size; i++) {
      copySourceRegisterToDestRegister(regist + i, REGISTER_X + i);
    }

    for(i=0; i<4; i++) {
      adjustResult(REGISTER_X + i, false, true, REGISTER_X + i, -1, -1);
    }
  }
}



void fnRecallElement(uint16_t unusedButMandatoryParameter) {
  #if !defined(TESTSUITE_BUILD)
    if(matrixIndex == INVALID_VARIABLE) {
      displayCalcErrorMessage(ERROR_NO_MATRIX_INDEXED, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        sprintf(errorMessage, "Cannot execute RCLEL without a matrix indexed");
        moreInfoOnError("In function fnRecallElement:", errorMessage, NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
    else {
      callByIndexedMatrix(recallElementReal, recallElementComplex);
    }
  #endif // !TESTSUITE_BUILD
}



void fnRecallIJ(uint16_t unusedButMandatoryParameter) {
  #if !defined(TESTSUITE_BUILD)
    if(matrixIndex == INVALID_VARIABLE) {
      displayCalcErrorMessage(ERROR_NO_MATRIX_INDEXED, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        sprintf(errorMessage, "Cannot execute RCLIJ without a matrix indexed");
        moreInfoOnError("In function fnRecallIJ:", errorMessage, NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
    else {
      longInteger_t zero;
      longIntegerInit(zero);

      if(!saveLastX()) {
        return;
      }

      liftStack();
      liftStack();

      if(matrixIndex == INVALID_VARIABLE || !regInRange(matrixIndex) || !((getRegisterDataType(matrixIndex) == dtReal34Matrix) || (getRegisterDataType(matrixIndex) == dtComplex34Matrix))) {
        convertLongIntegerToLongIntegerRegister(zero, REGISTER_Y);
        convertLongIntegerToLongIntegerRegister(zero, REGISTER_X);
      }
      else {
        if(getRegisterDataType(REGISTER_I) == dtLongInteger) {
          copySourceRegisterToDestRegister(REGISTER_I, REGISTER_Y);
        }
        else if(getRegisterDataType(REGISTER_I) == dtReal34) {
          convertReal34ToLongIntegerRegister(REGISTER_REAL34_DATA(REGISTER_I), REGISTER_Y, DEC_ROUND_DOWN);
        }
        else {
          convertLongIntegerToLongIntegerRegister(zero, REGISTER_Y);
        }
        if(getRegisterDataType(REGISTER_J) == dtLongInteger) {
          copySourceRegisterToDestRegister(REGISTER_J, REGISTER_X);
        }
        else if(getRegisterDataType(REGISTER_J) == dtReal34) {
          convertReal34ToLongIntegerRegister(REGISTER_REAL34_DATA(REGISTER_J), REGISTER_X, DEC_ROUND_DOWN);
        }
        else {
          convertLongIntegerToLongIntegerRegister(zero, REGISTER_X);
        }
      }

      adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
      adjustResult(REGISTER_Y, false, true, REGISTER_Y, -1, -1);

      longIntegerFree(zero);
    }
  #endif // !TESTSUITE_BUILD
}
