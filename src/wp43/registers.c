// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "registers.h"

#include "apps/bugScreen.h"
#include "charString.h"
#include "config.h"
#include "constantPointers.h"
#include "core/memory.h"
#include "dateTime.h"
#include "debug.h"
#include "display.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "items.h"
#include "mathematics/compare.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/rsd.h"
#include "matrix.h"
#include "registerValueConversions.h"
#include "saveRestoreCalcState.h"
#include "sort.h"
#include "stack.h"
#include <stdbool.h>
#include <string.h>

#include "wp43.h"

const reservedVariableHeader_t allReservedVariables[] = { // MUST be in the same order as the reserved variables in item.c item 1165 and upwards
/*  0 */  { .header = {.pointerToRegisterData = WP43_NULL,  .dataType = 0,             .tag = 0,           .readOnly = 0, .notUsed = 0}, .reservedVariableName = {1, 'X',  0,   0,   0,   0,   0,   0} },
/*  1 */  { .header = {.pointerToRegisterData = WP43_NULL,  .dataType = 0,             .tag = 0,           .readOnly = 0, .notUsed = 0}, .reservedVariableName = {1, 'Y',  0,   0,   0,   0,   0,   0} },
/*  2 */  { .header = {.pointerToRegisterData = WP43_NULL,  .dataType = 0,             .tag = 0,           .readOnly = 0, .notUsed = 0}, .reservedVariableName = {1, 'Z',  0,   0,   0,   0,   0,   0} },
/*  3 */  { .header = {.pointerToRegisterData = WP43_NULL,  .dataType = 0,             .tag = 0,           .readOnly = 0, .notUsed = 0}, .reservedVariableName = {1, 'T',  0,   0,   0,   0,   0,   0} },
/*  4 */  { .header = {.pointerToRegisterData = WP43_NULL,  .dataType = 0,             .tag = 0,           .readOnly = 0, .notUsed = 0}, .reservedVariableName = {1, 'A',  0,   0,   0,   0,   0,   0} },
/*  5 */  { .header = {.pointerToRegisterData = WP43_NULL,  .dataType = 0,             .tag = 0,           .readOnly = 0, .notUsed = 0}, .reservedVariableName = {1, 'B',  0,   0,   0,   0,   0,   0} },
/*  6 */  { .header = {.pointerToRegisterData = WP43_NULL,  .dataType = 0,             .tag = 0,           .readOnly = 0, .notUsed = 0}, .reservedVariableName = {1, 'C',  0,   0,   0,   0,   0,   0} },
/*  7 */  { .header = {.pointerToRegisterData = WP43_NULL,  .dataType = 0,             .tag = 0,           .readOnly = 0, .notUsed = 0}, .reservedVariableName = {1, 'D',  0,   0,   0,   0,   0,   0} },
/*  8 */  { .header = {.pointerToRegisterData = WP43_NULL,  .dataType = 0,             .tag = 0,           .readOnly = 0, .notUsed = 0}, .reservedVariableName = {1, 'L',  0,   0,   0,   0,   0,   0} },
/*  9 */  { .header = {.pointerToRegisterData = WP43_NULL,  .dataType = 0,             .tag = 0,           .readOnly = 0, .notUsed = 0}, .reservedVariableName = {1, 'I',  0,   0,   0,   0,   0,   0} },
/* 10 */  { .header = {.pointerToRegisterData = WP43_NULL,  .dataType = 0,             .tag = 0,           .readOnly = 0, .notUsed = 0}, .reservedVariableName = {1, 'J',  0,   0,   0,   0,   0,   0} },
/* 11 */  { .header = {.pointerToRegisterData = WP43_NULL,  .dataType = 0,             .tag = 0,           .readOnly = 0, .notUsed = 0}, .reservedVariableName = {1, 'K',  0,   0,   0,   0,   0,   0} },
/* 12 */  { .header = {.pointerToRegisterData = WP43_NULL,  .dataType = dtLongInteger, .tag = liPositive,  .readOnly = 1, .notUsed = 0}, .reservedVariableName = {3, 'A', 'D', 'M',  0,   0,   0,   0} },
/* 13 */  { .header = {.pointerToRegisterData = WP43_NULL,  .dataType = dtLongInteger, .tag = liPositive,  .readOnly = 1, .notUsed = 0}, .reservedVariableName = {5, 'D', '.', 'M', 'A', 'X',  0,   0} },
/* 14 */  { .header = {.pointerToRegisterData = WP43_NULL,  .dataType = dtLongInteger, .tag = liPositive,  .readOnly = 1, .notUsed = 0}, .reservedVariableName = {3, 'I', 'S', 'M',  0,   0,   0,   0} },
/* 15 */  { .header = {.pointerToRegisterData = WP43_NULL,  .dataType = dtLongInteger, .tag = liPositive,  .readOnly = 1, .notUsed = 0}, .reservedVariableName = {6, 'R', 'E', 'A', 'L', 'D', 'F',  0} },
/* 16 */  { .header = {.pointerToRegisterData = WP43_NULL,  .dataType = dtLongInteger, .tag = liPositive,  .readOnly = 1, .notUsed = 0}, .reservedVariableName = {4, '#', 'D', 'E', 'C',  0,   0,   0} },
/* 17 */  { .header = {.pointerToRegisterData = 0,          .dataType = dtReal34,      .tag = amNone,      .readOnly = 0, .notUsed = 0}, .reservedVariableName = {3, 'A', 'C', 'C',  0,   0,   0,   0} },
/* 18 */  { .header = {.pointerToRegisterData = 4,          .dataType = dtReal34,      .tag = amNone,      .readOnly = 0, .notUsed = 0}, .reservedVariableName = {5, 161, 145, 'L', 'i', 'm',  0,   0} },
/* 19 */  { .header = {.pointerToRegisterData = 8,          .dataType = dtReal34,      .tag = amNone,      .readOnly = 0, .notUsed = 0}, .reservedVariableName = {5, 161, 147, 'L', 'i', 'm',  0,   0} },
/* 20 */  { .header = {.pointerToRegisterData = 12,         .dataType = dtReal34,      .tag = amNone,      .readOnly = 0, .notUsed = 0}, .reservedVariableName = {2, 'F', 'V',  0,   0,   0,   0,   0} },
/* 21 */  { .header = {.pointerToRegisterData = 16,         .dataType = dtReal34,      .tag = amNone,      .readOnly = 0, .notUsed = 0}, .reservedVariableName = {4, 'i', '%', '/', 'a',  0,   0,   0} },
/* 22 */  { .header = {.pointerToRegisterData = 20,         .dataType = dtReal34,      .tag = amNone,      .readOnly = 0, .notUsed = 0}, .reservedVariableName = {4, 'N', 'P', 'E', 'R',  0,   0,   0} },
/* 23 */  { .header = {.pointerToRegisterData = 24,         .dataType = dtReal34,      .tag = amNone,      .readOnly = 0, .notUsed = 0}, .reservedVariableName = {5, 'P', 'E', 'R', '/', 'a',  0,   0} },
/* 24 */  { .header = {.pointerToRegisterData = 28,         .dataType = dtReal34,      .tag = amNone,      .readOnly = 0, .notUsed = 0}, .reservedVariableName = {3, 'P', 'M', 'T',  0,   0,   0,   0} },
/* 25 */  { .header = {.pointerToRegisterData = 32,         .dataType = dtReal34,      .tag = amNone,      .readOnly = 0, .notUsed = 0}, .reservedVariableName = {2, 'P', 'V',  0,   0,   0,   0,   0} },
/* 26 */  { .header = {.pointerToRegisterData = 36,         .dataType = dtLongInteger, .tag = liPositive,  .readOnly = 0, .notUsed = 0}, .reservedVariableName = {6, 'G', 'R', 'A', 'M', 'O', 'D',  0} },
};

static inline registerHeader_t *POINTER_TO_LOCAL_REGISTER(calcRegister_t a) {
  return ((registerHeader_t *)(currentLocalRegisters + (a)));
}



uint32_t getRegisterDataType(calcRegister_t regist) {
  if(regist <= LAST_GLOBAL_REGISTER) { // Global register
    return globalRegister[regist].dataType;
  }

  else if(regist <= LAST_LOCAL_REGISTER) { // Local register
    if(currentLocalRegisters != NULL) {
      regist -= FIRST_LOCAL_REGISTER;
      if(regist < currentNumberOfLocalRegisters) {
        return POINTER_TO_LOCAL_REGISTER(regist)->dataType;
      }
      #if defined(PC_BUILD)
        else {
          sprintf(errorMessage, "local register %" PRId16, regist);
          sprintf(errorMessage + ERROR_MESSAGE_LENGTH/2, "Must be from 0 to %" PRIu8, (uint8_t)(currentNumberOfLocalRegisters - 1));
          moreInfoOnError("In function getRegisterDataType:", errorMessage, "is not defined!", errorMessage + ERROR_MESSAGE_LENGTH/2);
        }
      #endif // PC_BUILD
    }
    #if defined(PC_BUILD)
      else {
        moreInfoOnError("In function getRegisterDataType:", "no local registers defined!", "To do so, you can find LocR here:", "[g] [P.FN] [g] [F5]");
      }
    #endif // PC_BUILD
  }

  else if(regist <= LAST_TEMP_REGISTER) { // Saved stack register or temporary register
    return savedStackRegister[regist - FIRST_SAVED_STACK_REGISTER].dataType;
  }

  else if(regist <= LAST_NAMED_VARIABLE) { // Named variable
    if(numberOfNamedVariables > 0) {
      regist -= FIRST_NAMED_VARIABLE;
      if(regist < numberOfNamedVariables) {
        return allNamedVariables[regist].header.dataType;
      }
      #if defined(PC_BUILD)
        else {
          sprintf(errorMessage, "named variable %" PRId16, regist);
          sprintf(errorMessage + ERROR_MESSAGE_LENGTH/2, "Must be from 0 to %" PRIu16, (uint16_t)(numberOfNamedVariables - 1));
          moreInfoOnError("In function getRegisterDataType:", errorMessage, "is not defined!", errorMessage + ERROR_MESSAGE_LENGTH/2);
        }
      #endif // PC_BUILD
    }
    else {
      bugScreen("In function getRegisterDataType: no named variables defined!");
    }
  }

  else if(regist <= LAST_RESERVED_VARIABLE) { // System named variable
    regist -= FIRST_RESERVED_VARIABLE;
    if(regist < 12) { // Lettered register
      return globalRegister[regist + REGISTER_X].dataType;
    }
    else {
      return allReservedVariables[regist].header.dataType;
    }
  }

  else {
    sprintf(errorMessage, "In function getRegisterDataType: regist=%" PRId16 " must be less than %d!", regist, LAST_RESERVED_VARIABLE + 1);
    bugScreen(errorMessage);
  }

  return 31u;
}



dataBlock_t *getRegisterDataPointer(calcRegister_t regist) {
  if(regist <= LAST_GLOBAL_REGISTER) { // Global register
    return TO_PCMEMPTR(globalRegister[regist].pointerToRegisterData);
  }

  else if(regist <= LAST_LOCAL_REGISTER) { // Local register
    if(currentLocalRegisters != NULL) {
      regist -= FIRST_LOCAL_REGISTER;
      if(regist < currentNumberOfLocalRegisters) {
        return TO_PCMEMPTR(POINTER_TO_LOCAL_REGISTER(regist)->pointerToRegisterData);
      }
      #if defined(PC_BUILD)
        else {
          sprintf(errorMessage, "local register %" PRId16, regist);
          sprintf(errorMessage + ERROR_MESSAGE_LENGTH/2, "Must be from 0 to %" PRIu8, (uint8_t)(currentNumberOfLocalRegisters - 1));
          moreInfoOnError("In function getRegisterDataPointer:", errorMessage, "is not defined!", errorMessage + ERROR_MESSAGE_LENGTH/2);
        }
      #endif // PC_BUILD
    }
    #if defined(PC_BUILD)
      else {
        moreInfoOnError("In function getRegisterDataPointer:", "no local registers defined!", "To do so, use LocR", NULL);
      }
    #endif // PC_BUILD
  }

  else if(regist <= LAST_TEMP_REGISTER) { // Saved stack register or temporary register
    return TO_PCMEMPTR(savedStackRegister[regist - FIRST_SAVED_STACK_REGISTER].pointerToRegisterData);
  }

  else if(regist <= LAST_NAMED_VARIABLE) { // Named variable
    if(numberOfNamedVariables > 0) {
      regist -= FIRST_NAMED_VARIABLE;
      if(regist < numberOfNamedVariables) {
        return TO_PCMEMPTR(allNamedVariables[regist].header.pointerToRegisterData);
      }
      #if defined(PC_BUILD)
        else {
          sprintf(errorMessage, "named variable %" PRId16, regist);
          sprintf(errorMessage + ERROR_MESSAGE_LENGTH/2, "Must be from 0 to %" PRIu16, (uint16_t)(numberOfNamedVariables - 1));
          moreInfoOnError("In function getRegisterDataPointer:", errorMessage, "is not defined!", errorMessage + ERROR_MESSAGE_LENGTH/2);
        }
      #endif // PC_BUILD
    }
    else {
      bugScreen("In function getRegisterDataPointer: no named variables defined!");
    }
  }

  else if(regist <= LAST_RESERVED_VARIABLE) { // System named variable
    regist -= FIRST_RESERVED_VARIABLE;
    return TO_PCMEMPTR(allReservedVariables[regist].header.pointerToRegisterData);
  }

  else {
    sprintf(errorMessage, "In function getRegisterDataPointer: regist=%" PRId16 " must be less than %d!", regist, LAST_RESERVED_VARIABLE + 1);
    bugScreen(errorMessage);
  }
  return 0;
}



uint32_t getRegisterTag(calcRegister_t regist) {
  if(regist <= LAST_GLOBAL_REGISTER) { // Global register
    return globalRegister[regist].tag;
  }

  else if(regist <= LAST_LOCAL_REGISTER) { // Local register
    if(currentLocalRegisters != NULL) {
      regist -= FIRST_LOCAL_REGISTER;
      if(regist < currentNumberOfLocalRegisters) {
        return POINTER_TO_LOCAL_REGISTER(regist)->tag;
      }
      #if defined(PC_BUILD)
        else {
          sprintf(errorMessage, "local register %" PRId16, regist);
          sprintf(errorMessage + ERROR_MESSAGE_LENGTH/2, "Must be from 0 to %" PRIu8, (uint8_t)(currentNumberOfLocalRegisters - 1));
          moreInfoOnError("In function getRegisterTag:", errorMessage, "is not defined!", errorMessage + ERROR_MESSAGE_LENGTH/2);
        }
      #endif // PC_BUILD
    }
    #if defined(PC_BUILD)
      else {
       moreInfoOnError("In function getRegisterTag:", "no local registers defined!", "To do so, you can find LocR here:", "[g] [P.FN] [g] [F5]");
      }
    #endif // PC_BUILD
  }

  else if(regist <= LAST_TEMP_REGISTER) { // Saved stack register or temporary register
    return savedStackRegister[regist - FIRST_SAVED_STACK_REGISTER].tag;
  }

  else if(regist <= LAST_NAMED_VARIABLE) { // Named variable
    if(numberOfNamedVariables > 0) {
      regist -= FIRST_NAMED_VARIABLE;
      if(regist < numberOfNamedVariables) {
        return allNamedVariables[regist].header.tag;
      }
      #if defined(PC_BUILD)
        else {
          sprintf(errorMessage, "named variable %" PRId16, regist);
          sprintf(errorMessage + ERROR_MESSAGE_LENGTH/2, "Must be from 0 to %" PRIu16, (uint16_t)(numberOfNamedVariables - 1));
          moreInfoOnError("In function getRegisterTag:", errorMessage, "is not defined!", errorMessage + ERROR_MESSAGE_LENGTH/2);
        }
      #endif // PC_BUILD
    }
    else {
      bugScreen("In function getRegisterTag: no named variables defined!");
    }
  }

  else if(regist <= LAST_RESERVED_VARIABLE) { // System named variable
    regist -= FIRST_RESERVED_VARIABLE;
    return allReservedVariables[regist].header.tag;
  }

  else {
    sprintf(errorMessage, "In function getRegisterTag: regist=%" PRId16 " must be less than %d!", regist, LAST_RESERVED_VARIABLE + 1);
    bugScreen(errorMessage);
  }
  return 0;
}



void setRegisterDataType(calcRegister_t regist, uint16_t dataType, uint32_t tag) {
  if(regist <= LAST_GLOBAL_REGISTER) { // Global register
    globalRegister[regist].dataType = dataType;
    globalRegister[regist].tag = tag;
  }

  else if(regist <= LAST_LOCAL_REGISTER) { // Local register
    if(currentLocalRegisters != NULL) {
      regist -= FIRST_LOCAL_REGISTER;
      if(regist < currentNumberOfLocalRegisters) {
        POINTER_TO_LOCAL_REGISTER(regist)->dataType = dataType;
        POINTER_TO_LOCAL_REGISTER(regist)->tag = tag;
      }
      #if defined(PC_BUILD)
        else {
          sprintf(errorMessage, "local register %" PRId16, regist);
          sprintf(errorMessage + ERROR_MESSAGE_LENGTH/2, "Must be from 0 to %" PRIu8, (uint8_t)(currentNumberOfLocalRegisters - 1));
          moreInfoOnError("In function setRegisterDataType:", errorMessage, "is not defined!", errorMessage + ERROR_MESSAGE_LENGTH/2);
        }
      #endif // PC_BUILD
    }
    #if defined(PC_BUILD)
      else {
       moreInfoOnError("In function setRegisterDataType:", "no local registers defined!", "To do so, you can find LocR here:", "[g] [P.FN] [g] [F5]");
      }
    #endif // PC_BUILD
  }

  else if(regist <= LAST_TEMP_REGISTER) { // Saved stack register or temporary register
    savedStackRegister[regist - FIRST_SAVED_STACK_REGISTER].dataType = dataType;
    savedStackRegister[regist - FIRST_SAVED_STACK_REGISTER].tag = tag;
  }

  else if(regist <= LAST_NAMED_VARIABLE) { // Named variable
    if(numberOfNamedVariables > 0) {
      regist -= FIRST_NAMED_VARIABLE;
      if(regist < numberOfNamedVariables) {
        allNamedVariables[regist].header.dataType = dataType;
        allNamedVariables[regist].header.tag = tag;
      }
      #if defined(PC_BUILD)
        else {
          sprintf(errorMessage, "named variable %" PRId16, regist);
          sprintf(errorMessage + ERROR_MESSAGE_LENGTH/2, "Must be from 0 to %" PRIu16, (uint16_t)(numberOfNamedVariables - 1));
          moreInfoOnError("In function setRegisterDataType:", errorMessage, "is not defined!", errorMessage + ERROR_MESSAGE_LENGTH/2);
        }
      #endif // PC_BUILD
    }
    else {
      bugScreen("In function setRegisterDataType: no named variables defined!");
    }
  }

  else if(regist <= LAST_RESERVED_VARIABLE) { // System named variable
    regist -= FIRST_RESERVED_VARIABLE;
    if(allReservedVariables[regist].header.pointerToRegisterData != WP43_NULL && allReservedVariables[regist].header.readOnly == 0) {
      allNamedVariables[regist].header.dataType = dataType;
      allNamedVariables[regist].header.tag = tag;
    }
  }

  else {
    sprintf(errorMessage, "In function setRegisterDataType: regist=%" PRId16 " must be less than %d!", regist, LAST_RESERVED_VARIABLE + 1);
    bugScreen(errorMessage);
  }
}



void setRegisterDataPointer(calcRegister_t regist, void *memPtr) {
  uint32_t dataPointer = TO_WP43MEMPTR(memPtr);

  if(regist <= LAST_GLOBAL_REGISTER) { // Global register
    globalRegister[regist].pointerToRegisterData = dataPointer;
  }

  else if(regist <= LAST_LOCAL_REGISTER) { // Local register
    if(currentLocalRegisters != NULL) {
      regist -= FIRST_LOCAL_REGISTER;
      if(regist < currentNumberOfLocalRegisters) {
        POINTER_TO_LOCAL_REGISTER(regist)->pointerToRegisterData = dataPointer;
      }
      #if defined(PC_BUILD)
        else {
          sprintf(errorMessage, "local register %" PRId16, regist);
          sprintf(errorMessage + ERROR_MESSAGE_LENGTH/2, "Must be from 0 to %" PRIu8, (uint8_t)(currentNumberOfLocalRegisters - 1));
          moreInfoOnError("In function setRegisterDataPointer:", errorMessage, "is not defined!", errorMessage + ERROR_MESSAGE_LENGTH/2);
        }
      #endif // PC_BUILD
    }
    #if defined(PC_BUILD)
      else {
        moreInfoOnError("In function setRegisterDataPointer:", "no local registers defined!", "To do so, you can find LocR here:", "[g] [P.FN] [g] [F5]");
      }
    #endif // PC_BUILD
  }

  else if(regist <= LAST_TEMP_REGISTER) { // Saved stack register or temporary register
    savedStackRegister[regist - FIRST_SAVED_STACK_REGISTER].pointerToRegisterData = dataPointer;
  }

  else if(regist <= LAST_NAMED_VARIABLE) { // Named variable
    if(numberOfNamedVariables > 0) {
      regist -= FIRST_NAMED_VARIABLE;
      if(regist < numberOfNamedVariables) {
        allNamedVariables[regist].header.pointerToRegisterData = dataPointer;
      }
      #if defined(PC_BUILD)
        else {
          sprintf(errorMessage, "named variable %" PRId16, regist);
          sprintf(errorMessage + ERROR_MESSAGE_LENGTH/2, "Must be from 0 to %" PRIu16, (uint16_t)(numberOfNamedVariables - 1));
          moreInfoOnError("In function setRegisterDataPointer:", errorMessage, "is not defined!", errorMessage + ERROR_MESSAGE_LENGTH/2);
        }
      #endif // PC_BUILD
    }
    #if defined(PC_BUILD)
      else {
        moreInfoOnError("In function setRegisterDataPointer:", "no local registers defined!", NULL, NULL);
      }
    #endif // PC_BUILD
  }

  else if(regist <= LAST_RESERVED_VARIABLE) { // System named variable
  }

  else {
    sprintf(errorMessage, "In function setRegisterDataPointer: regist=%" PRId16 " must be less than %d!", regist, LAST_RESERVED_VARIABLE + 1);
    bugScreen(errorMessage);
  }
}



void setRegisterTag(calcRegister_t regist, uint32_t tag) {
  if(regist <= LAST_GLOBAL_REGISTER) { // Global register
    globalRegister[regist].tag = tag;
  }

  else if(regist <= LAST_LOCAL_REGISTER) { // Local register
    if(currentLocalRegisters != NULL) {
      regist -= FIRST_LOCAL_REGISTER;
      if(regist < currentNumberOfLocalRegisters) {
        POINTER_TO_LOCAL_REGISTER(regist)->tag = tag;
      }
      #if defined(PC_BUILD)
        else {
          sprintf(errorMessage, "local register %" PRId16, regist);
          sprintf(errorMessage + ERROR_MESSAGE_LENGTH/2, "Must be from 0 to %" PRIu8, (uint8_t)(currentNumberOfLocalRegisters - 1));
          moreInfoOnError("In function setRegisterDataInfo:", errorMessage, "is not defined!", errorMessage + ERROR_MESSAGE_LENGTH/2);
        }
      #endif // PC_BUILD
    }
    #if defined(PC_BUILD)
      else {
        moreInfoOnError("In function setRegisterDataInfo:", "no local registers defined!", "To do so, you can find LocR here:", "[g] [P.FN] [g] [F5]");
      }
    #endif // PC_BUILD
  }

  else if(regist <= LAST_TEMP_REGISTER) { // Saved stack register or temporary register
    savedStackRegister[regist - FIRST_SAVED_STACK_REGISTER].tag = tag;
  }

  else if(regist <= LAST_NAMED_VARIABLE) { // Named variable
    if(numberOfNamedVariables > 0) {
      regist -= FIRST_NAMED_VARIABLE;
      if(regist < numberOfNamedVariables) {
        allNamedVariables[regist].header.tag = tag;
      }
      #if defined(PC_BUILD)
        else {
          sprintf(errorMessage, "named variable %" PRId16, regist);
          sprintf(errorMessage + ERROR_MESSAGE_LENGTH/2, "Must be from 0 to %" PRIu16, (uint16_t)(numberOfNamedVariables - 1));
          moreInfoOnError("In function setRegisterDataInfo:", errorMessage, "is not defined!", errorMessage + ERROR_MESSAGE_LENGTH/2);
        }
      #endif // PC_BUILD
    }
    else {
      bugScreen("In function setRegisterDataInfo: no named variables defined!");
    }
  }

  else if(regist <= LAST_RESERVED_VARIABLE) { // System named variable
  }

  else {
    sprintf(errorMessage, "In function setRegisterDataInfo: regist=%" PRId16 " must be less than %d!", regist, LAST_RESERVED_VARIABLE + 1);
    bugScreen(errorMessage);
  }
}



void allocateLocalRegisters(uint16_t numberOfRegistersToAllocate) {
  dataBlock_t *oldSubroutineLevelData = currentSubroutineLevelData;

  if(numberOfRegistersToAllocate > 99) {
    displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "You can allocate up to 99 registers, you requested %" PRIu16, numberOfRegistersToAllocate);
      moreInfoOnError("In function allocateLocalRegisters:", errorMessage, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    return;
  }

  uint16_t r;
  if(currentLocalFlags == NULL) {
    // 1st allocation of local registers in this level of subroutine
    if((currentSubroutineLevelData = reallocWp43(currentSubroutineLevelData, TO_BLOCKS(12), TO_BLOCKS(16 + 4*numberOfRegistersToAllocate)))) {
      currentLocalFlags = currentSubroutineLevelData + 3;
      currentLocalFlags->localFlags = 0;
      currentLocalRegisters = (registerHeader_t *)(currentSubroutineLevelData + 4);
      currentNumberOfLocalFlags = NUMBER_OF_LOCAL_FLAGS;
      currentNumberOfLocalRegisters = numberOfRegistersToAllocate;

      // All the new local registers are real34s initialized to 0.0
      for(r=FIRST_LOCAL_REGISTER; r<FIRST_LOCAL_REGISTER+numberOfRegistersToAllocate; r++) {
        void *newMem = allocWp43(REAL34_SIZE_IN_BLOCKS);
        if(newMem) {
          setRegisterDataType(r, dtReal34, amNone);
          setRegisterDataPointer(r, newMem);
          real34Zero(REGISTER_REAL34_DATA(r));
        }
        else {
          // Not enough memory (!)
          for(uint16_t rr = FIRST_LOCAL_REGISTER; rr < r; rr++) {
            freeRegisterData(FIRST_LOCAL_REGISTER + rr);
          }
          reallocWp43(currentSubroutineLevelData, TO_BLOCKS(16 + 4*numberOfRegistersToAllocate), TO_BLOCKS(12));
          currentLocalFlags = NULL;
          currentLocalRegisters = NULL;
          currentNumberOfLocalRegisters = 0;
          currentNumberOfLocalFlags = NUMBER_OF_LOCAL_FLAGS;
          displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
          return;
        }
      }
    }
    else {
      currentSubroutineLevelData = oldSubroutineLevelData;
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      return;
    }
  }
  else if(numberOfRegistersToAllocate != currentNumberOfLocalRegisters) {
    // The number of allocated local registers changes
    if(numberOfRegistersToAllocate > currentNumberOfLocalRegisters) {
      uint8_t oldNumberOfLocalRegisters = currentNumberOfLocalRegisters;
      if((currentSubroutineLevelData = reallocWp43(currentSubroutineLevelData, TO_BLOCKS(16 + 4*currentNumberOfLocalRegisters), TO_BLOCKS(16 + 4*numberOfRegistersToAllocate)))) {
        currentLocalFlags = currentSubroutineLevelData + 3;
        currentLocalRegisters = (registerHeader_t *)(currentSubroutineLevelData + 4);
        currentNumberOfLocalRegisters = numberOfRegistersToAllocate;

        // All the new local registers are real34s initialized to 0.0
        for(r=FIRST_LOCAL_REGISTER+oldNumberOfLocalRegisters; r<FIRST_LOCAL_REGISTER+numberOfRegistersToAllocate; r++) {
          void *newMem = allocWp43(REAL34_SIZE_IN_BLOCKS);
          if(newMem) {
            setRegisterDataType(r, dtReal34, amNone);
            setRegisterDataPointer(r, newMem);
            real34Zero(REGISTER_REAL34_DATA(r));
          }
          else {
            // Not enough memory (!)
            for(uint16_t rr = FIRST_LOCAL_REGISTER + oldNumberOfLocalRegisters; rr < r; rr++) {
              freeRegisterData(FIRST_LOCAL_REGISTER + rr);
            }
            reallocWp43(currentSubroutineLevelData, TO_BLOCKS(16 + 4*numberOfRegistersToAllocate), TO_BLOCKS(16 + 4*oldNumberOfLocalRegisters));
            currentLocalFlags = currentSubroutineLevelData + 3;
            currentLocalRegisters = (registerHeader_t *)(currentSubroutineLevelData + 4);
            currentNumberOfLocalRegisters = numberOfRegistersToAllocate;
            displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
            return;
          }
        }
      }
      else {
        currentSubroutineLevelData = oldSubroutineLevelData;
        displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        return;
      }
    }
    else {
      // free memory allocated to the data of the deleted local registers
      for(r=numberOfRegistersToAllocate; r<currentNumberOfLocalRegisters; r++) {
        freeRegisterData(FIRST_LOCAL_REGISTER + r);
      }
      freeWp43(currentSubroutineLevelData + 4 + numberOfRegistersToAllocate, TO_BLOCKS(4*(currentNumberOfLocalRegisters - numberOfRegistersToAllocate)));
      currentLocalFlags = currentSubroutineLevelData + 3;
      currentLocalRegisters = (numberOfRegistersToAllocate == 0 ? NULL : (registerHeader_t *)(currentSubroutineLevelData + 4));
      currentNumberOfLocalRegisters = numberOfRegistersToAllocate;
    }
  }
  else {
    return;
  }

  if(currentSubroutineLevel == 0) {
    allSubroutineLevels.ptrToSubroutineLevel0Data = TO_WP43MEMPTR(currentSubroutineLevelData);
  }
  else {
    ((dataBlock_t *)(TO_PCMEMPTR(currentPtrToPreviousLevel)))[2].ptrToNextLevel = TO_WP43MEMPTR(currentSubroutineLevelData);
  }
}



bool validateName(const char *name) {
  if(stringGlyphLength(name)  > 7) {
    return false; // Given name is too long
  }
  else if(stringGlyphLength(name) == 0) {
    return false; // Given name is empty
  }

  // Check for the 1st character
  if(                                          compareChar(name, STD_A                   ) < 0) {
    return false;
  }
  if(compareChar(name, STD_Z          ) > 0 && compareChar(name, STD_a                   ) < 0) {
    return false;
  }
  if(compareChar(name, STD_Z          ) > 0 && compareChar(name, STD_a                   ) < 0) {
    return false;
  }
  if(compareChar(name, STD_z          ) > 0 && compareChar(name, STD_A_GRAVE             ) < 0) {
    return false;
  }
  if(                                          compareChar(name, STD_CROSS               ) ==0) {
    return false;
  }
  if(                                          compareChar(name, STD_DIVIDE              ) ==0) {
    return false;
  }
  if(compareChar(name, STD_z_CARON    ) > 0 && compareChar(name, STD_iota_DIALYTIKA_TONOS) < 0) {
    return false;
  }
  if(compareChar(name, STD_omega_TONOS) > 0 && compareChar(name, STD_SUP_x               ) < 0) {
    return false;
  }
  if(compareChar(name, STD_SUP_x      ) > 0 && compareChar(name, STD_SUB_alpha           ) < 0) {
    return false;
  }
  if(compareChar(name, STD_SUB_mu     ) > 0 && compareChar(name, STD_SUB_h               ) < 0) {
    return false;
  }
  if(compareChar(name, STD_SUB_h      ) > 0 && compareChar(name, STD_SUB_t               ) < 0) {
    return false;
  }
  if(compareChar(name, STD_SUB_t      ) > 0 && compareChar(name, STD_SUB_a               ) < 0) {
    return false;
  }
  if(compareChar(name, STD_SUB_Z      ) > 0                                                   ) {
    return false;
  }

  // Check for the following characters
  for(name += (*name & 0x80) ? 2 : 1; *name != 0; name += (*name & 0x80) ? 2 : 1) {
    switch(*name) {
      case '+':
      case '-':
      case ':':
      case '/':
      case '^':
      case '(':
      case ')':
      case '=':
      case ';':
      case '|':
      case '!':
      case ' ': {
        return false;
      }
      default: {
        if(compareChar(name, STD_CROSS) == 0) {
          return false;
        }
      }
    }
  }

  return true;
}



bool isUniqueName(const char *name) {
  // Built-in items
  for(uint32_t i = 0; i < LAST_ITEM; ++i) {
    switch(indexOfItems[i].status & CAT_STATUS) {
      case CAT_FNCT:
      case CAT_MENU:
      case CAT_CNST:
      case CAT_RVAR:
      case CAT_SYFL: {
        if(compareString(name, indexOfItems[i].itemCatalogName, CMP_NAME) == 0) {
          return false;
        }
      }
    }
  }

  // Variable menus
  if(findNamedVariable(name) != INVALID_VARIABLE) {
    return false;
  }

  // User menus
  for(uint32_t i = 0; i < numberOfUserMenus; ++i) {
    if(compareString(name, userMenus[i].menuName, CMP_NAME) == 0) {
      return false;
    }
  }

  return true;
}



static calcRegister_t _findReservedVariable(const char *variableName) {
  uint8_t len = stringGlyphLength(variableName);

  if(len < 1 || len > 7) {
    return INVALID_VARIABLE;
  }

  for(int i = 0; i < NUMBER_OF_RESERVED_VARIABLES; i++) {
    if(compareString((char *)(allReservedVariables[i].reservedVariableName + 1), variableName, CMP_NAME) == 0) {
      return i + FIRST_RESERVED_VARIABLE;
    }
  }

  return INVALID_VARIABLE;
}



void allocateNamedVariable(const char *variableName, dataType_t dataType, uint16_t fullDataSizeInBlocks) {
  calcRegister_t regist;
  uint8_t len;

  if(stringGlyphLength(variableName) < 1 || stringGlyphLength(variableName) > 7) {
    #if defined(PC_BUILD)
      sprintf(errorMessage, "the name %s", variableName);
      moreInfoOnError("In function allocateNamedVariable:", errorMessage, "is incorrect! The length must be", "from 1 to 7 glyphs!");
    #endif // PC_BUILD
    return;
  }

  if(_findReservedVariable(variableName) != INVALID_VARIABLE) {
    displayCalcErrorMessage(ERROR_INVALID_NAME, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    #if defined(PC_BUILD)
      sprintf(errorMessage, "the name %s", variableName);
      moreInfoOnError("In function allocateNamedVariable:", errorMessage, "clashes with a reserved variable!", NULL);
    #endif // PC_BUILD
    return;
  }

  if(!validateName(variableName)) {
    displayCalcErrorMessage(ERROR_INVALID_NAME, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    #if defined(PC_BUILD)
      sprintf(errorMessage, "the name %s", variableName);
      moreInfoOnError("In function allocateNamedVariable:", errorMessage, "is incorrect! The name does not follow", "the naming convention!");
    #endif // PC_BUILD
    return;
  }

  if(numberOfNamedVariables == 0) { // First named variable
    if((allNamedVariables = allocWp43(TO_BLOCKS(sizeof(namedVariableHeader_t))))) {
      numberOfNamedVariables = 1;

      regist = 0;
    }
    else { // unlikely but possible
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      return;
    }
  }
  else {
    regist = numberOfNamedVariables;
    if(regist == LAST_NAMED_VARIABLE - FIRST_NAMED_VARIABLE + 1) {
      displayCalcErrorMessage(ERROR_TOO_MANY_VARIABLES, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      #if defined(PC_BUILD)
        sprintf(errorMessage, "%d named variables!", LAST_NAMED_VARIABLE - FIRST_NAMED_VARIABLE + 1);
        moreInfoOnError("In function allocateNamedVariable:", "you can allocate up to", errorMessage, NULL);
      #endif // PC_BUILD
      return;
    }

    namedVariableHeader_t *origNamedVariables = allNamedVariables;
    if((allNamedVariables = reallocWp43(allNamedVariables, TO_BLOCKS(sizeof(namedVariableHeader_t) * numberOfNamedVariables), TO_BLOCKS(sizeof(namedVariableHeader_t) * (numberOfNamedVariables + 1))))) {
      numberOfNamedVariables++;
    }
    else {
      allNamedVariables = origNamedVariables;
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      return;
    }
  }

  len = stringByteLength(variableName);
  allNamedVariables[regist].variableName[0] = len;
  // Ensure that we terminate with \0 in the string to make in place comparisons easier
  memset(allNamedVariables[regist].variableName + 1, 0, 15);
  xcopy(allNamedVariables[regist].variableName + 1, variableName, len);

  regist += FIRST_NAMED_VARIABLE;
  setRegisterDataType(regist, dataType, amNone);
  setRegisterDataPointer(regist, allocWp43(fullDataSizeInBlocks));
}



calcRegister_t findNamedVariable(const char *variableName) {
  calcRegister_t regist = INVALID_VARIABLE;
  uint8_t len = stringGlyphLength(variableName);
  if(len < 1 || len > 7) {
    return regist;
  }

  regist = _findReservedVariable(variableName);
  if(regist != INVALID_VARIABLE) {
    return regist;
  }

  for(int i = 0; i < numberOfNamedVariables; i++) {
    if(compareString((char *)(allNamedVariables[i].variableName + 1), variableName, CMP_NAME) == 0) {
      regist = i + FIRST_NAMED_VARIABLE;
      break;
    }
  }
  return regist;
}



calcRegister_t findOrAllocateNamedVariable(const char *variableName) {
  calcRegister_t regist = INVALID_VARIABLE;
  uint8_t len = stringGlyphLength(variableName);
  if(len < 1 || len > 7) {
    return regist;
  }
  regist = findNamedVariable(variableName);
  if(regist == INVALID_VARIABLE && numberOfNamedVariables <= (LAST_NAMED_VARIABLE - FIRST_NAMED_VARIABLE)) {
    if(!isUniqueName(variableName)) {
      displayCalcErrorMessage(ERROR_ENTER_NEW_NAME, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      #if defined(PC_BUILD)
        sprintf(errorMessage, "the name %s", variableName);
        moreInfoOnError("In function allocateNamedVariable:", errorMessage, "is already in use!", NULL);
      #endif // PC_BUILD
      return regist;
    }
    allocateNamedVariable(variableName, dtReal34, REAL34_SIZE_IN_BLOCKS);
    if(lastErrorCode == ERROR_NONE) {
      // New variables are zero by default - although this might be immediately overridden, it might require an
      // initial value, such as when STO+
      regist = FIRST_NAMED_VARIABLE + numberOfNamedVariables - 1;
      real34Zero(REGISTER_REAL34_DATA(regist));
    }
    else {
      // Failed attempt to allocate a new named variable: there is not enough memory or the name is invalid.
      // It is impossible to reach the limitation of number of named variables.
      return INVALID_VARIABLE;
    }
  }
  return regist;
}



void setRegisterMaxDataLength(calcRegister_t regist, uint16_t maxDataLen) {
  if(regist <= LAST_GLOBAL_REGISTER) { // Global register
    ((dataBlock_t *)TO_PCMEMPTR(globalRegister[regist].pointerToRegisterData))->dataMaxLength = maxDataLen;
  }

  else if(regist <= LAST_LOCAL_REGISTER) { // Local register
    if(currentLocalRegisters != NULL) {
      if(regist-FIRST_LOCAL_REGISTER < currentNumberOfLocalRegisters) {
        getRegisterDataPointer(regist)->dataMaxLength = maxDataLen;
      }
      #if defined(PC_BUILD)
        else {
          sprintf(errorMessage, "local register %" PRId16 " is not defined!", (uint16_t)(regist - FIRST_LOCAL_REGISTER));
          sprintf(errorMessage + ERROR_MESSAGE_LENGTH/2, "Must be from 0 to %" PRIu8, (uint8_t)(currentNumberOfLocalRegisters - 1));
          moreInfoOnError("In function setRegisterMaxDataLength:", errorMessage, errorMessage + ERROR_MESSAGE_LENGTH/2, NULL);
        }
      #endif // PC_BUILD
    }
    #if defined(PC_BUILD)
      else {
       moreInfoOnError("In function setRegisterMaxDataLength:", "no local registers defined!", NULL, NULL);
      }
    #endif // PC_BUILD
  }

  else if(regist <= LAST_TEMP_REGISTER) { // Saved stack register or temporary register
    getRegisterDataPointer(regist)->dataMaxLength = maxDataLen;
  }

  else if(regist <= LAST_NAMED_VARIABLE) { // Named variable
    if(numberOfNamedVariables > 0) {
      if(regist - FIRST_NAMED_VARIABLE < numberOfNamedVariables) {
        getRegisterDataPointer(regist)->dataMaxLength = maxDataLen;
      }
      else {
        sprintf(errorMessage, "In function setRegisterMaxDataLength: named variable %" PRId16 " is not defined! Must be from 0 to %" PRIu16, (uint16_t)(regist - FIRST_NAMED_VARIABLE), (uint16_t)(numberOfNamedVariables - 1));
        bugScreen(errorMessage);
      }
    }
    #if defined(PC_BUILD)
      else {
        moreInfoOnError("In function setRegisterMaxDataLength:", "no named variables defined!", NULL, NULL);
      }
    #endif // PC_BUILD
  }

  else if(regist <= LAST_RESERVED_VARIABLE) { // System named variable
    regist -= FIRST_RESERVED_VARIABLE;
    getRegisterDataPointer(regist)->dataMaxLength = maxDataLen;
  }

  else {
    sprintf(errorMessage, "In function setRegisterMaxDataLength: regist=%" PRId16 " must be less than %d!", regist, LAST_RESERVED_VARIABLE + 1);
    bugScreen(errorMessage);
  }
}



uint16_t getRegisterMaxDataLength(calcRegister_t regist) {
  dataBlock_t *db = NULL;

  if(regist <= LAST_GLOBAL_REGISTER) { // Global register
    db = (dataBlock_t *)TO_PCMEMPTR(globalRegister[regist].pointerToRegisterData);
  }

  else if(regist <= LAST_LOCAL_REGISTER) { // Local register
    if(currentLocalRegisters != NULL) {
      if(regist-FIRST_LOCAL_REGISTER < currentNumberOfLocalRegisters) {
        db = (dataBlock_t *)TO_PCMEMPTR(POINTER_TO_LOCAL_REGISTER(regist-FIRST_LOCAL_REGISTER)->pointerToRegisterData);
      }
      else {
        sprintf(errorMessage, "In function getRegisterMaxDataLength: local register %" PRId16 " is not defined! Must be from 0 to %" PRIu8, (uint16_t)(regist - FIRST_LOCAL_REGISTER), (uint8_t)(currentNumberOfLocalRegisters - 1));
        bugScreen(errorMessage);
      }
    }
    #if defined(PC_BUILD)
      else {
        moreInfoOnError("In function getRegisterMaxStringLength:", "no local registers defined!", NULL, NULL);
      }
    #endif // PC_BUILD
  }

  else if(regist <= LAST_TEMP_REGISTER) { // Saved stack register or temporary register
    db = (dataBlock_t *)TO_PCMEMPTR(savedStackRegister[regist - FIRST_SAVED_STACK_REGISTER].pointerToRegisterData);
  }

  else if(regist <= LAST_NAMED_VARIABLE) { // Named variable
    if(numberOfNamedVariables != 0) {
      regist -= FIRST_NAMED_VARIABLE;
      if(regist < numberOfNamedVariables) {
        db = (dataBlock_t *)TO_PCMEMPTR(allNamedVariables[regist].header.pointerToRegisterData);
      }
      else {
        sprintf(errorMessage, "In function getRegisterMaxDataLength: named variable %" PRId16 " is not defined! Must be from 0 to %" PRIu16, regist, (uint16_t)(numberOfNamedVariables - 1));
        bugScreen(errorMessage);
      }
    }
    #if defined(PC_BUILD)
      else {
        moreInfoOnError("In function getRegisterMaxStringLength:", "no named variables defined!", NULL, NULL);
      }
    #endif // PC_BUILD
  }

  else if(regist <= LAST_RESERVED_VARIABLE) { // System named variable
    regist -= FIRST_RESERVED_VARIABLE;
    db = (dataBlock_t *)TO_PCMEMPTR(allReservedVariables[regist].header.pointerToRegisterData);
  }

  else {
    sprintf(errorMessage, "In function getRegisterMaxDataLength: regist=%" PRId16 " must be less than %d!", regist, LAST_RESERVED_VARIABLE + 1);
    bugScreen(errorMessage);
  }

  if(db) {
    if(getRegisterDataType(regist) == dtReal34Matrix) {
      return db->matrixRows * db->matrixColumns * REAL34_SIZE_IN_BLOCKS;
    }
    else if(getRegisterDataType(regist) == dtComplex34Matrix) {
      return db->matrixRows * db->matrixColumns * COMPLEX34_SIZE_IN_BLOCKS;
    }
    else {
      return db->dataMaxLength;
    }
  }
  return 0;
}



uint16_t getRegisterFullSize(calcRegister_t regist) {
  switch(getRegisterDataType(regist)) {
    case dtLongInteger: {
      return getRegisterDataPointer(regist)->dataMaxLength + 1;
    }
    case dtTime: {
      return REAL34_SIZE_IN_BLOCKS;
    }
    case dtDate: {
      return REAL34_SIZE_IN_BLOCKS;
    }
    case dtString: {
      return getRegisterDataPointer(regist)->dataMaxLength + 1;
    }
    case dtReal34Matrix: {
      return TO_BLOCKS((getRegisterDataPointer(regist)->matrixRows * getRegisterDataPointer(regist)->matrixColumns) * sizeof(real34_t)) + 1; break;
    }
    case dtComplex34Matrix: {
      return TO_BLOCKS((getRegisterDataPointer(regist)->matrixRows * getRegisterDataPointer(regist)->matrixColumns) * sizeof(complex34_t)) + 1; break;
    }
    case dtShortInteger: {
      return SHORT_INTEGER_SIZE;
    }
    case dtReal34: {
      return REAL34_SIZE_IN_BLOCKS;
    }
    case dtComplex34: {
      return COMPLEX34_SIZE_IN_BLOCKS;
    }
    case dtConfig: {
      return CONFIG_SIZE;
    }
    default: {
      sprintf(errorMessage, "In function getRegisterFullSize: data type %s is unknown!", getDataTypeName(getRegisterDataType(regist), false, false));
      bugScreen(errorMessage);
      return 0;
    }
  }
}



void clearRegister(calcRegister_t regist) {
  if(getRegisterDataType(regist) == dtReal34) {
    real34Zero(REGISTER_REAL34_DATA(regist));
    setRegisterTag(regist, amNone);
  }
  else{
    reallocateRegister(regist, dtReal34, REAL34_SIZE_IN_BLOCKS, amNone);
    real34Zero(REGISTER_REAL34_DATA(regist));
  }
}



void fnClearRegisters(uint16_t confirmation) {
  if(confirmation == NOT_CONFIRMED) {
    setConfirmationMode(fnClearRegisters);
  }
  else {
    calcRegister_t regist;

    for(regist=0; regist<REGISTER_X; regist++) {
      clearRegister(regist);
    }

    for(regist=0; regist<currentNumberOfLocalRegisters; regist++) {
      clearRegister(FIRST_LOCAL_REGISTER + regist);
    }

    if(!getSystemFlag(FLAG_SSIZE8)) { // Stack size = 4
      for(regist=REGISTER_A; regist<=REGISTER_D; regist++) {
        clearRegister(regist);
      }
    }

    for(regist=REGISTER_I; regist<=REGISTER_K; regist++) {
      clearRegister(regist);
    }
  }
}



void fnGetLocR(uint16_t unusedButMandatoryParameter) {
  longInteger_t locR;

  liftStack();

  longIntegerInit(locR);
  uIntToLongInteger(currentNumberOfLocalRegisters, locR);
  convertLongIntegerToLongIntegerRegister(locR, REGISTER_X);
  longIntegerFree(locR);
}



void adjustResult(calcRegister_t res, bool dropY, bool setCpxRes, calcRegister_t op1, calcRegister_t op2, calcRegister_t op3) {
  uint32_t resultDataType;
  bool     oneArgumentIsComplex = false;

  if(op1 >= 0) {
    oneArgumentIsComplex = oneArgumentIsComplex || getRegisterDataType(op1) == dtComplex34;
  }

  if(op2 >= 0) {
    oneArgumentIsComplex = oneArgumentIsComplex || getRegisterDataType(op2) == dtComplex34;
  }

  if(op3 >= 0) {
    oneArgumentIsComplex = oneArgumentIsComplex || getRegisterDataType(op3) == dtComplex34;
  }

  resultDataType = getRegisterDataType(res);
  if(getSystemFlag(FLAG_SPCRES) == false && lastErrorCode == 0) {
    // D is clear: test infinite values and -0 values
    switch(resultDataType) {
      case dtReal34:
      case dtTime:
      case dtDate: {
        if(real34IsInfinite(REGISTER_REAL34_DATA(res))) {
          displayCalcErrorMessage(real34IsPositive(REGISTER_REAL34_DATA(res)) ? ERROR_OVERFLOW_PLUS_INF : ERROR_OVERFLOW_MINUS_INF , ERR_REGISTER_LINE, res);
        }
        else if(real34IsZero(REGISTER_REAL34_DATA(res))) {
          real34SetPositiveSign(REGISTER_REAL34_DATA(res));
        }
        break;
      }

      case dtComplex34: {
        if(real34IsInfinite(REGISTER_REAL34_DATA(res))) {
          displayCalcErrorMessage(real34IsPositive(REGISTER_REAL34_DATA(res)) ? ERROR_OVERFLOW_PLUS_INF : ERROR_OVERFLOW_MINUS_INF , ERR_REGISTER_LINE, res);
        }
        else if(real34IsZero(REGISTER_REAL34_DATA(res))) {
          real34SetPositiveSign(REGISTER_REAL34_DATA(res));
        }

        if(real34IsInfinite(REGISTER_IMAG34_DATA(res))) {
          displayCalcErrorMessage(real34IsPositive(REGISTER_IMAG34_DATA(res)) ? ERROR_OVERFLOW_PLUS_INF : ERROR_OVERFLOW_MINUS_INF , ERR_REGISTER_LINE, res);
        }
        else if(real34IsZero(REGISTER_IMAG34_DATA(res))) {
          real34SetPositiveSign(REGISTER_IMAG34_DATA(res));
        }
        break;
      }

      #if !defined(TESTSUITE_BUILD)
        case dtReal34Matrix: {
          real34Matrix_t matrix;
          linkToRealMatrixRegister(res, &matrix);
          for(uint32_t i = 0; i < matrix.header.matrixRows * matrix.header.matrixColumns; i++) {
            if(real34IsInfinite(VARIABLE_REAL34_DATA(&matrix.matrixElements[i]))) {
              displayCalcErrorMessage(real34IsPositive(VARIABLE_REAL34_DATA(&matrix.matrixElements[i])) ? ERROR_OVERFLOW_PLUS_INF : ERROR_OVERFLOW_MINUS_INF , ERR_REGISTER_LINE, res);
            }
            else if(real34IsZero(VARIABLE_REAL34_DATA(&matrix.matrixElements[i]))) {
              real34SetPositiveSign(VARIABLE_REAL34_DATA(&matrix.matrixElements[i]));
            }
          }
          break;
        }

        case dtComplex34Matrix: {
          complex34Matrix_t matrix;
          linkToComplexMatrixRegister(res, &matrix);
          for(uint32_t i = 0; i < matrix.header.matrixRows * matrix.header.matrixColumns; i++) {
            if(real34IsInfinite(VARIABLE_REAL34_DATA(&matrix.matrixElements[i]))) {
              displayCalcErrorMessage(real34IsPositive(VARIABLE_REAL34_DATA(&matrix.matrixElements[i])) ? ERROR_OVERFLOW_PLUS_INF : ERROR_OVERFLOW_MINUS_INF , ERR_REGISTER_LINE, res);
            }
            else if(real34IsZero(VARIABLE_REAL34_DATA(&matrix.matrixElements[i]))) {
              real34SetPositiveSign(VARIABLE_REAL34_DATA(&matrix.matrixElements[i]));
            }

            if(real34IsInfinite(VARIABLE_IMAG34_DATA(&matrix.matrixElements[i]))) {
              displayCalcErrorMessage(real34IsPositive(VARIABLE_IMAG34_DATA(&matrix.matrixElements[i])) ? ERROR_OVERFLOW_PLUS_INF : ERROR_OVERFLOW_MINUS_INF , ERR_REGISTER_LINE, res);
            }
            else if(real34IsZero(VARIABLE_IMAG34_DATA(&matrix.matrixElements[i]))) {
              real34SetPositiveSign(VARIABLE_IMAG34_DATA(&matrix.matrixElements[i]));
            }
          }
          break;
        }
      #endif // !TESTSUITE_BUILD

      default: {
        break;
      }
    }
  }

  if(resultDataType == dtTime) {
    checkTimeRange(REGISTER_REAL34_DATA(res));
  }
  if(resultDataType == dtDate) {
    checkDateRange(REGISTER_REAL34_DATA(res));
  }

  if(lastErrorCode != 0) {
    #if defined(TESTSUITE_BUILD)
      #if defined(DEBUGUNDO)
        printf(">>> undo from adjustResult\n");
      #endif // DEBUGUNDO
      undo();
    #endif //TESTSUITE_BUILD
    return;
  }

  if(setCpxRes && oneArgumentIsComplex && resultDataType != dtString) {
    fnSetFlag(FLAG_CPXRES);
  }

  // Round the register value
  switch(resultDataType) {
    real_t tmp;

    case dtReal34: {
      if(significantDigits == 0 || significantDigits >= 34) {
        break;
      }

      real34ToReal(REGISTER_REAL34_DATA(res), &tmp);
      convertRealToReal34ResultRegister(&tmp, res);
      break;
    }

    case dtComplex34: {
      if(significantDigits == 0 || significantDigits >= 34) {
        break;
      }

      real34ToReal(REGISTER_REAL34_DATA(res), &tmp);
      convertRealToReal34ResultRegister(&tmp, res);
      real34ToReal(REGISTER_IMAG34_DATA(res), &tmp);
      convertRealToImag34ResultRegister(&tmp, res);
      break;
    }

    #if !defined(TESTSUITE_BUILD)
      case dtReal34Matrix: {
        if(significantDigits == 0 || significantDigits >= 34) {
          break;
        }

        rsdRema(significantDigits);
        break;
      }

      case dtComplex34Matrix: {
        if(significantDigits == 0 || significantDigits >= 34) {
          break;
        }

        rsdCxma(significantDigits);
        break;
      }
    #endif // !TESTSUITE_BUILD

    default: {
      break;
    }
  }

  if(dropY) {
    fnDropY(NOPARAM);
  }
}



void copySourceRegisterToDestRegister(calcRegister_t sourceRegister, calcRegister_t destRegister) {
  if(destRegister >= RESERVED_VARIABLE_X && destRegister <= RESERVED_VARIABLE_K) {
    destRegister = destRegister - RESERVED_VARIABLE_X + REGISTER_X;
  }

  if(sourceRegister >= RESERVED_VARIABLE_X && sourceRegister <= RESERVED_VARIABLE_K) {
    sourceRegister = sourceRegister - RESERVED_VARIABLE_X + REGISTER_X;
  }
  else if(sourceRegister == RESERVED_VARIABLE_ADM) {
    longInteger_t longIntVar;
    longIntegerInit(longIntVar);
    switch(currentAngularMode) {
      case amDMS: {
        intToLongInteger(1, longIntVar);
        break;
      }
      case amRadian: {
        intToLongInteger(2, longIntVar);
        break;
      }
      case amMultPi: {
        intToLongInteger(3, longIntVar);
        break;
      }
      case amGrad: {
        intToLongInteger(4, longIntVar);
        break;
      }
      default: {
        intToLongInteger(0, longIntVar);
        break;
      }
    }
    convertLongIntegerToLongIntegerRegister(longIntVar, destRegister);
    longIntegerFree(longIntVar);
    return;
  }
  else if(sourceRegister == RESERVED_VARIABLE_DENMAX) {
    longInteger_t longIntVar;
    longIntegerInit(longIntVar);
    uIntToLongInteger(denMax, longIntVar);
    convertLongIntegerToLongIntegerRegister(longIntVar, destRegister);
    longIntegerFree(longIntVar);
    return;
  }
  else if(sourceRegister == RESERVED_VARIABLE_ISM) {
    longInteger_t longIntVar;
    longIntegerInit(longIntVar);
    uIntToLongInteger((shortIntegerMode==SIM_2COMPL ? 2 : (shortIntegerMode==SIM_1COMPL ? 1 : (shortIntegerMode==SIM_UNSIGN ? 0 : -1))), longIntVar);
    convertLongIntegerToLongIntegerRegister(longIntVar, destRegister);
    longIntegerFree(longIntVar);
    return;
  }
  else if(sourceRegister == RESERVED_VARIABLE_REALDF) {
    longInteger_t longIntVar;
    longIntegerInit(longIntVar);
    uIntToLongInteger(displayFormat, longIntVar);
    convertLongIntegerToLongIntegerRegister(longIntVar, destRegister);
    longIntegerFree(longIntVar);
    return;
  }
  else if(sourceRegister == RESERVED_VARIABLE_NDEC) {
    longInteger_t longIntVar;
    longIntegerInit(longIntVar);
    uIntToLongInteger(displayFormatDigits, longIntVar);
    convertLongIntegerToLongIntegerRegister(longIntVar, destRegister);
    longIntegerFree(longIntVar);
    return;
  }

  if(   getRegisterDataType(destRegister) != getRegisterDataType(sourceRegister)
     || getRegisterFullSize(destRegister) != getRegisterFullSize(sourceRegister)) {
    uint32_t sizeInBlocks;

    switch(getRegisterDataType(sourceRegister)) {
      case dtLongInteger: {
        sizeInBlocks = getRegisterDataPointer(sourceRegister)->dataMaxLength;
        break;
      }
      case dtTime: {
        sizeInBlocks = REAL34_SIZE_IN_BLOCKS;
        break;
      }
      case dtDate: {
        sizeInBlocks = REAL34_SIZE_IN_BLOCKS;
        break;
      }
      case dtString: {
        sizeInBlocks = getRegisterDataPointer(sourceRegister)->dataMaxLength;
        break;
      }
      case dtReal34Matrix: {
        sizeInBlocks = TO_BLOCKS((getRegisterDataPointer(sourceRegister)->matrixRows * getRegisterDataPointer(sourceRegister)->matrixColumns) * sizeof(real34_t));
        break;
      }
      case dtComplex34Matrix: {
        sizeInBlocks = TO_BLOCKS((getRegisterDataPointer(sourceRegister)->matrixRows * getRegisterDataPointer(sourceRegister)->matrixColumns) * sizeof(complex34_t));
        break;
      }
      case dtShortInteger: {
        sizeInBlocks = SHORT_INTEGER_SIZE;
        break;
      }
      case dtReal34: {
        sizeInBlocks = REAL34_SIZE_IN_BLOCKS;
        break;
      }
      case dtComplex34: {
        sizeInBlocks = COMPLEX34_SIZE_IN_BLOCKS;
        break;
      }
      case dtConfig: {
        sizeInBlocks = CONFIG_SIZE;
        break;
      }

      default: {
        sprintf(errorMessage, "In function copySourceRegisterToDestRegister: data type %s is unknown!", getDataTypeName(getRegisterDataType(sourceRegister), false, false));
        bugScreen(errorMessage);
        sizeInBlocks = 0;
      }
    }
    reallocateRegister(destRegister, getRegisterDataType(sourceRegister), sizeInBlocks, amNone);
    if(lastErrorCode == ERROR_RAM_FULL) {
      return;
    }
  }

  switch(getRegisterDataType(sourceRegister)) {
    case dtReal34Matrix: {
      xcopy(REGISTER_REAL34_MATRIX_DBLOCK(destRegister), REGISTER_REAL34_MATRIX_DBLOCK(sourceRegister), sizeof(dataBlock_t));
      xcopy(REGISTER_REAL34_MATRIX_M_ELEMENTS(destRegister), REGISTER_REAL34_MATRIX_M_ELEMENTS(sourceRegister),
      getRegisterDataPointer(sourceRegister)->matrixRows * getRegisterDataPointer(sourceRegister)->matrixColumns * TO_BYTES(REAL34_SIZE_IN_BLOCKS));
      break;
    }
    case dtComplex34Matrix: {
      xcopy(REGISTER_COMPLEX34_MATRIX_DBLOCK(destRegister), REGISTER_COMPLEX34_MATRIX_DBLOCK(sourceRegister), sizeof(dataBlock_t));
      xcopy(REGISTER_COMPLEX34_MATRIX_M_ELEMENTS(destRegister), REGISTER_COMPLEX34_MATRIX_M_ELEMENTS(sourceRegister),
      getRegisterDataPointer(sourceRegister)->matrixRows * getRegisterDataPointer(sourceRegister)->matrixColumns * TO_BYTES(COMPLEX34_SIZE_IN_BLOCKS));
      break;
    }
    default: {
      xcopy(REGISTER_DATA(destRegister), REGISTER_DATA(sourceRegister), TO_BYTES(getRegisterFullSize(sourceRegister)));
    }
  }
  setRegisterTag(destRegister, getRegisterTag(sourceRegister));
}



int16_t indirectAddressing(calcRegister_t regist, uint16_t parameterType, int16_t minValue, int16_t maxValue) {
  int16_t value;
  bool    isValidAlpha = false;
  printf("parameterType %u\n", parameterType); fflush(stdout);
  printf("currentNumberOfLocalFlags %u\n", currentNumberOfLocalFlags); fflush(stdout);

  switch(parameterType) {
    case INDPM_REGISTER: {
      // Temorarily assign the maximum value to the maximum register
      // We need to do better range checking later
      maxValue = FIRST_NAMED_VARIABLE + numberOfNamedVariables - 1;
      break;
    }
    case INDPM_FLAG: {
      maxValue = NUMBER_OF_GLOBAL_FLAGS + currentNumberOfLocalFlags - 1;
      break;
    }
  }

  if(regist >= FIRST_LOCAL_REGISTER + currentNumberOfLocalRegisters &&
     (regist < FIRST_NAMED_VARIABLE ||
        regist >= FIRST_NAMED_VARIABLE + numberOfNamedVariables)) {
    if(!getSystemFlag(FLAG_IGN1ER)) {
      displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
    }
    #if defined(PC_BUILD)
      sprintf(errorMessage, "local indirection register .%02d", regist - FIRST_LOCAL_REGISTER);
      moreInfoOnError("In function indirectAddressing:", errorMessage, "is not defined!", getSystemFlag(FLAG_IGN1ER) ? "ignored since IGN1ER was set" : NULL);
    #endif // PC_BUILD
    clearSystemFlag(FLAG_IGN1ER);
    return FAILED_INDIRECTION;
  }

  else if(getRegisterDataType(regist) == dtReal34) {
    real34_t maxValue34;

    int32ToReal34(maxValue, &maxValue34);
    if(real34CompareLessThan(REGISTER_REAL34_DATA(regist), const34_0) || real34CompareGreaterEqual(REGISTER_REAL34_DATA(regist), &maxValue34)) {
      if(!getSystemFlag(FLAG_IGN1ER)) {
        displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
      }
      #if defined(PC_BUILD)
        real34ToString(REGISTER_REAL34_DATA(regist), errorMessage);
        sprintf(tmpString, "register %" PRId16 " = %s:", regist, errorMessage);
        moreInfoOnError("In function indirectAddressing:", tmpString, "this value is negative or too big!", getSystemFlag(FLAG_IGN1ER) ? "ignored since IGN1ER was set" : NULL);
      #endif // PC_BUILD
      clearSystemFlag(FLAG_IGN1ER);
      return FAILED_INDIRECTION;
    }
    value = real34ToInt32(REGISTER_REAL34_DATA(regist));
  }

  else if(getRegisterDataType(regist) == dtLongInteger) {
    longInteger_t lgInt;

    convertLongIntegerRegisterToLongInteger(regist, lgInt);
    if(longIntegerIsNegative(lgInt) || longIntegerCompareUInt(lgInt, maxValue) > 0) {
      if(!getSystemFlag(FLAG_IGN1ER)) {
        displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
      }
      #if defined(PC_BUILD)
        longIntegerToAllocatedString(lgInt, errorMessage, ERROR_MESSAGE_LENGTH);
        sprintf(tmpString, "register %" PRId16 " = %s:", regist, errorMessage);
        moreInfoOnError("In function indirectAddressing:", tmpString, "this value is negative or too big!", getSystemFlag(FLAG_IGN1ER) ? "ignored since IGN1ER was set" : NULL);
      #endif // PC_BUILD
      longIntegerFree(lgInt);
      clearSystemFlag(FLAG_IGN1ER);
      return FAILED_INDIRECTION;
    }
    value = longIntegerToUInt(lgInt);
    longIntegerFree(lgInt);
  }

  else if(getRegisterDataType(regist) == dtShortInteger) {
    uint64_t val;
    int16_t sign;

    convertShortIntegerRegisterToUInt64(regist, &sign, &val);
    if(sign == 1 || val > 180) {
      if(!getSystemFlag(FLAG_IGN1ER)) {
        displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
      }
      #if defined(PC_BUILD)
        shortIntegerToDisplayString(regist, errorMessage, false);
        sprintf(tmpString, "register %" PRId16 " = %s:", regist, errorMessage);
        moreInfoOnError("In function indirectAddressing:", tmpString, "this value is negative or too big!", getSystemFlag(FLAG_IGN1ER) ? "ignored since IGN1ER was set" : NULL);
      #endif // PC_BUILD
      clearSystemFlag(FLAG_IGN1ER);
      return FAILED_INDIRECTION;
    }
    value = val;
  }

  else if(getRegisterDataType(regist) == dtString && parameterType == INDPM_REGISTER) {
    value = findNamedVariable(REGISTER_STRING_DATA(regist));
    isValidAlpha = true;
    if(value == INVALID_VARIABLE) {
      if(!getSystemFlag(FLAG_IGN1ER)) {
        displayCalcErrorMessage(ERROR_UNDEF_SOURCE_VAR, ERR_REGISTER_LINE, REGISTER_X);
      }
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        sprintf(errorMessage, "string '%s' is not a named variable", REGISTER_STRING_DATA(regist));
        moreInfoOnError("In function indirectAddressing:", errorMessage, getSystemFlag(FLAG_IGN1ER) ? "ignored since IGN1ER was set" : NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
      clearSystemFlag(FLAG_IGN1ER);
      return FAILED_INDIRECTION;
    }
  }

  else {
    if(!getSystemFlag(FLAG_IGN1ER)) {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    }
    #if defined(PC_BUILD)
      sprintf(errorMessage, "register %" PRId16 " is %s:", regist, getRegisterDataTypeName(regist, true, false));
      moreInfoOnError("In function indirectAddressing:", errorMessage, "not suited for indirect addressing!", getSystemFlag(FLAG_IGN1ER) ? "ignored since IGN1ER was set" : NULL);
    #endif // PC_BUILD
    clearSystemFlag(FLAG_IGN1ER);
    return FAILED_INDIRECTION;
  }

  if(minValue <= value && (value <= maxValue || isValidAlpha)) {
    return value;
  }
  else {
    if(!getSystemFlag(FLAG_IGN1ER)) {
      displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
    }
    #if defined(PC_BUILD)
      sprintf(errorMessage, "value = %d! Should be from %d to %d.", value, minValue, maxValue);
      moreInfoOnError("In function indirectAddressing:", errorMessage, getSystemFlag(FLAG_IGN1ER) ? "ignored since IGN1ER was set" : NULL, NULL);
    #endif // PC_BUILD
    clearSystemFlag(FLAG_IGN1ER);
    return FAILED_INDIRECTION;
  }
}



#if defined(TESTSUITE_BUILD)
  void printRegisterToString(calcRegister_t regist, char *registerContent) {
    char str[1000];

    if(getRegisterDataType(regist) == dtReal34) {
      real34ToString(REGISTER_REAL34_DATA(regist), str);
      sprintf(registerContent, "real34 %s %s", str, getAngularModeName(getRegisterAngularMode(regist)));
    }

    else if(getRegisterDataType(regist) == dtComplex34) {
      real34ToString(REGISTER_REAL34_DATA(regist), str);
      sprintf(registerContent, "complex34 %s ", str);

      real34ToString(REGISTER_IMAG34_DATA(regist), str);
      if(real34IsNegative(REGISTER_IMAG34_DATA(regist))) {
        strcat(registerContent, "- ix");
        strcat(registerContent, str + 1);
      }
      else {
        strcat(registerContent, "+ ix");
        strcat(registerContent, str);
      }
    }

    else if(getRegisterDataType(regist) == dtString) {
      stringToUtf8(REGISTER_STRING_DATA(regist), (uint8_t *)str);
      sprintf(registerContent, "string (%" PRIu32 " bytes) |%s|", TO_BYTES(getRegisterMaxDataLength(regist)), str);
    }

    else if(getRegisterDataType(regist) == dtShortInteger) {
      uint64_t value = *(REGISTER_SHORT_INTEGER_DATA(regist));
      sprintf(registerContent, "short integer %08x-%08x (base %u)", (unsigned int)(value>>32), (unsigned int)(value&0xffffffff), getRegisterTag(regist));
    }

    else if(getRegisterDataType(regist) == dtConfig) {
      strcpy(registerContent, "Configuration data");
    }

    else if(getRegisterDataType(regist) == dtLongInteger) {
      longInteger_t lgInt;
      char lgIntStr[3000];

      convertLongIntegerRegisterToLongInteger(regist, lgInt);
      longIntegerToAllocatedString(lgInt, lgIntStr, sizeof(lgIntStr));
      longIntegerFree(lgInt);
      sprintf(registerContent, "long integer (%" PRIu32 " bytes) %s", TO_BYTES(getRegisterMaxDataLength(regist)), lgIntStr);
    }

    else if(getRegisterDataType(regist) == dtTime) {
      real34ToString(REGISTER_REAL34_DATA(regist), str);
      sprintf(registerContent, "time %s", str);
    }

    else if(getRegisterDataType(regist) == dtDate) {
      real34ToString(REGISTER_REAL34_DATA(regist), str);
      sprintf(registerContent, "date %s", str);
    }

    else {
      sprintf(registerContent, "In printRegisterToString: data type %s not supported", getRegisterDataTypeName(regist ,false, false));
    }
  }
#endif // TESTSUITE_BUILD



#if !defined(DMCP_BUILD)
  void printRegisterToConsole(calcRegister_t regist, const char *before, const char *after) {
    char str[3000];

    printf("%s", before);

    if(getRegisterDataType(regist) == dtReal34) {
      real34ToString(REGISTER_REAL34_DATA(regist), str);
      printf("real34 %s %s", str, getAngularModeName(getRegisterAngularMode(regist)));
    }

    else if(getRegisterDataType(regist) == dtComplex34) {
      real34ToString(REGISTER_REAL34_DATA(regist), str);
      printf("complex34 %s ", str);

      real34ToString(REGISTER_IMAG34_DATA(regist), str);
      if(real34IsNegative(REGISTER_IMAG34_DATA(regist))) {
        printf("- ix%s", str + 1);
      }
      else {
        printf("+ ix%s", str);
      }
    }

    else if(getRegisterDataType(regist) == dtString) {
      stringToUtf8(REGISTER_STRING_DATA(regist), (uint8_t *)str);
      printf("string (%" PRIu64 " + %" PRIu32 " bytes) |%s|", (uint64_t)sizeof(dataBlock_t), TO_BYTES(getRegisterMaxDataLength(regist)), str);
    }

    else if(getRegisterDataType(regist) == dtShortInteger) {
      uint64_t value = *(REGISTER_SHORT_INTEGER_DATA(regist));
      printf("short integer %08x-%08x (base %" PRIu32 ")", (unsigned int)(value>>32), (unsigned int)(value&0xffffffff), getRegisterTag(regist));
    }

    else if(getRegisterDataType(regist) == dtConfig) {
      printf("Configuration data");
    }

    else if(getRegisterDataType(regist) == dtLongInteger) {
      longInteger_t lgInt;

      convertLongIntegerRegisterToLongInteger(regist, lgInt);
      longIntegerToAllocatedString(lgInt, str, sizeof(str));
      longIntegerFree(lgInt);
      printf("long integer (%" PRIu64 " + %" PRIu32 " bytes) %s", (uint64_t)sizeof(dataBlock_t), TO_BYTES(getRegisterMaxDataLength(regist)), str);
    }

    else if(getRegisterDataType(regist) == dtTime) {
      real34ToString(REGISTER_REAL34_DATA(regist), str);
      printf("time %s", str);
    }

    else if(getRegisterDataType(regist) == dtDate) {
      real34ToString(REGISTER_REAL34_DATA(regist), str);
      printf("date %s", str);
    }

    else if(getRegisterDataType(regist) == dtReal34Matrix) {
      uint16_t r, c;
      real34Matrix_t mat;
      linkToRealMatrixRegister(regist, &mat);
      for(r = 0; r < mat.header.matrixRows; ++r) {
        printf("Matrix Row %3i: ",r);
        for(c = 0; c < mat.header.matrixColumns; ++c) {
          real34ToString(&mat.matrixElements[r * mat.header.matrixColumns + c], str);
          printf("%s ", str);
        }
        printf("\n");
      }
    }


    else {
      sprintf(errorMessage, "In printRegisterToConsole: data type %s not supported", getRegisterDataTypeName(regist ,false, false));
      bugScreen(errorMessage);
    }

    printf("%s", after);
  }



  void printReal34ToConsole(const real34_t *value, const char *before, const char *after) {
    char str[100];

    real34ToString(value, str);
    printf("%sreal34 %s%s", before, str, after);
  }



  void printRealToConsole(const real_t *value, const char *before, const char *after) {
    char str[1000];

    realToString(value, str);
    printf("%sreal%" PRId32 " %s%s", before, value->digits, str, after);

    /*int32_t i, exponent, last;

    if(realIsNaN(value)) {
      printf("NaN");
      return;
    }

    if(realIsNegative(value)) {
      printf("-");
    }

    if(realIsInfinite(value)) {
      printf("infinite");
      return;
    }

    if(realIsZero(value)) {
      printf("0");
      return;
    }

    if(value->digits % DECDPUN) {
      i = value->digits/DECDPUN;
    }
    else {
      i = value->digits/DECDPUN - 1;
    }

    while(value->lsu[i] == 0) i--;
    printf("%" PRIu16, value->lsu[i--]);

    exponent = value->exponent;
    last = 0;
    while(exponent <= -DECDPUN && value->lsu[last] == 0) {
      last++;
      exponent += DECDPUN;
    }

    for(; i>=last; i--) {
      printf(" %03" PRIu16, value->lsu[i]);
    }

    if(exponent != 0) {
      printf(" e %" PRId32, exponent);
    }*/
  }



  void printComplex34ToConsole(const complex34_t *value, const char *before, const char *after) {
    char str[100];

    real34ToString((real34_t *)value, str);
    printf("%scomplex34 %s + ", before, str);
    real34ToString((real34_t *)value + 1, str);
    printf("%si%s", str, after);
  }



  void printRegisterDescriptorToConsole(calcRegister_t regist) {
    registerHeader_t registerHeader;

    registerHeader.descriptor = 0xFFFFFFFF;

    if(regist <= LAST_GLOBAL_REGISTER) { // Global register
      registerHeader = globalRegister[regist];
    }

    else if(regist <= LAST_LOCAL_REGISTER) { // Local register
      if(currentNumberOfLocalRegisters > 0) {
        regist -= FIRST_LOCAL_REGISTER;
        if(regist < currentNumberOfLocalRegisters) {
          registerHeader = *POINTER_TO_LOCAL_REGISTER(regist);
        }
      }
    }

    else if(regist <= LAST_TEMP_REGISTER) { // Saved stack register or temporary register
      registerHeader = savedStackRegister[regist - FIRST_SAVED_STACK_REGISTER];
    }

    else if(regist <= LAST_NAMED_VARIABLE) { // Named variable
      if(numberOfNamedVariables > 0) {
        regist -= FIRST_NAMED_VARIABLE;
        if(regist < numberOfNamedVariables) {
          registerHeader = allNamedVariables[regist].header;
        }
      }
    }

    printf("Header informations of register %d\n", regist);
    printf("    reg ptr   = %u\n", registerHeader.pointerToRegisterData);
    printf("    data type = %u = %s\n", registerHeader.dataType, getDataTypeName(registerHeader.dataType, false, false));
    if(registerHeader.dataType == dtLongInteger || registerHeader.dataType == dtString) {
      printf("    data ptr  = %u\n", registerHeader.pointerToRegisterData + 1);
      printf("    data size = %" PRIu32 "\n", *(uint32_t *)TO_PCMEMPTR(globalRegister[regist].pointerToRegisterData));
    }
    printf("    tag       = %u\n", registerHeader.tag);
  }



  void printLongIntegerToConsole(const longInteger_t value, const char *before, const char *after) {
    char str[3000];

    longIntegerToAllocatedString(value, str, sizeof(str));
    printf("%slong integer (%" PRIu64 " + %" PRIu64 " <%" PRIu64 " reserved> bytes) %s%s", before, (uint64_t)(sizeof(value->_mp_size) + sizeof(value->_mp_d) + sizeof(value->_mp_alloc)), (uint64_t)longIntegerSizeInBytes(value), (uint64_t)(value->_mp_alloc * LIMB_SIZE), str, after);
  }
#endif // !DMCP_BUILD



void reallocateRegister(calcRegister_t regist, uint32_t dataType, uint16_t dataSizeWithoutDataLenBlocks, uint32_t tag) { // dataSize without data length in blocks, this includes the trailing 0 for strings
  uint16_t dataSizeWithDataLenBlocks = dataSizeWithoutDataLenBlocks;

  //printf("reallocateRegister: %d to %s tag=%u (%u bytes excluding maxSize) begin\n", regist, getDataTypeName(dataType, false, false), tag, dataSizeWithoutDataLenBlocks);
  switch(dataType) {
    case dtReal34:
    case dtTime:
    case dtDate: {
      assert(dataSizeWithoutDataLenBlocks == REAL34_SIZE_IN_BLOCKS);
      break;
    }
    case dtComplex34: {
      assert(dataSizeWithoutDataLenBlocks == COMPLEX34_SIZE_IN_BLOCKS);
      break;
    }
    case dtShortInteger: {
      assert(dataSizeWithoutDataLenBlocks == SHORT_INTEGER_SIZE);
      break;
    }
    case dtConfig: {
      assert(dataSizeWithoutDataLenBlocks == CONFIG_SIZE);
      break;
    }
    case dtString:
    case dtReal34Matrix:
    case dtComplex34Matrix: {
      dataSizeWithDataLenBlocks = dataSizeWithoutDataLenBlocks + 1; // +1 for the max length of the string
      break;
    }
    case dtLongInteger: {
      if(dataSizeWithoutDataLenBlocks % LIMB_SIZE != 0) {
        dataSizeWithoutDataLenBlocks = ((dataSizeWithoutDataLenBlocks / TO_BLOCKS(LIMB_SIZE)) + 1) * TO_BLOCKS(LIMB_SIZE);
      }
      dataSizeWithDataLenBlocks = dataSizeWithoutDataLenBlocks + 1; // +1 for the max length of the data
      break;
    }
    default: {}
  }

  if(getRegisterDataType(regist) != dataType || ((getRegisterDataType(regist) == dtString || getRegisterDataType(regist) == dtLongInteger || getRegisterDataType(regist) == dtReal34Matrix || getRegisterDataType(regist) == dtComplex34Matrix) && getRegisterMaxDataLength(regist) != dataSizeWithoutDataLenBlocks)) {
    if(!isMemoryBlockAvailable(dataSizeWithDataLenBlocks)) {
      #if defined(PC_BUILD)
        printf("In function reallocateRegister: required %" PRIu16 " blocks for register #%" PRId16 " but no data blocks with enough size are available!\n", dataSizeWithoutDataLenBlocks, regist); fflush(stdout);
      #endif // PC_BUILD
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      return;
    }
    freeRegisterData(regist);
    setRegisterDataPointer(regist, allocWp43(dataSizeWithDataLenBlocks));
    setRegisterDataType(regist, dataType, tag);
    if(dataType == dtReal34Matrix) {
      REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixRows = 1;
      REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixColumns = dataSizeWithoutDataLenBlocks / REAL34_SIZE_IN_BLOCKS;
    }
    else if(dataType == dtComplex34Matrix) {
      REGISTER_COMPLEX34_MATRIX_DBLOCK(regist)->matrixRows = 1;
      REGISTER_COMPLEX34_MATRIX_DBLOCK(regist)->matrixColumns = dataSizeWithoutDataLenBlocks / COMPLEX34_SIZE_IN_BLOCKS;
    }
    else {
      setRegisterMaxDataLength(regist, dataSizeWithoutDataLenBlocks);
    }
  }
  else {
    setRegisterTag(regist, tag);
  }
  //sprintf(tmpString, "reallocateRegister %d to %s tag=%u (%u bytes including dataLen) done", regist, getDataTypeName(dataType, false, false), tag, dataSizeWithDataLenBlocks);
  //memoryDump(tmpString);
}



void fnToReal(uint16_t unusedButMandatoryParameter) {
  switch(getRegisterDataType(REGISTER_X)) {
    case dtLongInteger: {
      copySourceRegisterToDestRegister(REGISTER_X, REGISTER_L);
      convertLongIntegerRegisterToReal34Register(REGISTER_X, REGISTER_X);
      break;
    }

    case dtShortInteger: {
      copySourceRegisterToDestRegister(REGISTER_X, REGISTER_L);
      convertShortIntegerRegisterToReal34Register(REGISTER_X, REGISTER_X);
      break;
    }

    case dtReal34: {
      copySourceRegisterToDestRegister(REGISTER_X, REGISTER_L);
      setRegisterAngularMode(REGISTER_X, amNone);
      break;
    }

    case dtTime: {
      copySourceRegisterToDestRegister(REGISTER_X, REGISTER_L);
      convertTimeRegisterToReal34Register(REGISTER_X, REGISTER_X);
      break;
    }

    case dtDate: {
      copySourceRegisterToDestRegister(REGISTER_X, REGISTER_L);
      convertDateRegisterToReal34Register(REGISTER_X, REGISTER_X);
      break;
    }

    default: {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        sprintf(errorMessage, "data type %s cannot be converted to a real34!", getRegisterDataTypeName(REGISTER_X, false, false));
        moreInfoOnError("In function fnToReal:", errorMessage, NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
      return;
    }
  }
}



bool saveLastX(void) {
  copySourceRegisterToDestRegister(REGISTER_X, REGISTER_L);
  return lastErrorCode == ERROR_NONE;
}



static uint8_t getRegParam(bool *f, uint16_t *s, uint16_t *n, uint16_t *d) {
  real_t x, p;

  if(getRegisterDataType(REGISTER_X) == dtReal34) {
    *s = *n = 0;
    if(d) {
      *d = 0;
    }
    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);
    if(!realCompareAbsLessThan(&x, const_1000)) {
      return ERROR_OUT_OF_RANGE;
    }

    if(f) {
      *f = realIsNegative(&x);
    }
    if(f == NULL && realIsNegative(&x)) {
      return ERROR_OUT_OF_RANGE;
    }
    realSetPositiveSign(&x);

    realToIntegralValue(&x, &p, DEC_ROUND_DOWN, &ctxtReal39);
    *s = realToInt32(&p);

    realSubtract(&x, &p, &x, &ctxtReal39);
    x.exponent += 2;
    realToIntegralValue(&x, &p, DEC_ROUND_DOWN, &ctxtReal39);
    *n = realToInt32(&p);

    if(d) {
      realSubtract(&x, &p, &x, &ctxtReal39);
      x.exponent += 3;
      realToIntegralValue(&x, &p, DEC_ROUND_DOWN, &ctxtReal39);
      *d = realToInt32(&p);
    }

    if(*s < REGISTER_X) { // global numbered registers
      if(*s + *n >= REGISTER_X) {
        return ERROR_OUT_OF_RANGE;
      }
      else if(*n == 0) {
        *n = REGISTER_X - *s;
      }
    }
    else if(*s < FIRST_LOCAL_REGISTER) { // stack and global lettered registers (XYZT ABCD LIJK)
      if(*s + *n >= FIRST_LOCAL_REGISTER) {
        return ERROR_OUT_OF_RANGE;
      }
      else if(*n == 0) {
        *n = FIRST_LOCAL_REGISTER - *s;
      }
    }
    else if(*s < FIRST_LOCAL_REGISTER + currentNumberOfLocalRegisters) { // local registers
      if(*s + *n >= FIRST_LOCAL_REGISTER + currentNumberOfLocalRegisters) {
        return ERROR_OUT_OF_RANGE;
      }
      else if(f && *f) {
        return ERROR_OUT_OF_RANGE;
      }
      else if(*n == 0) {
        *n = FIRST_LOCAL_REGISTER + currentNumberOfLocalRegisters - *s;
      }
    }
    else {
      return ERROR_OUT_OF_RANGE;
    }

    if(d) {
      if(*d < REGISTER_X) { // global numbered registers
        if(*d + *n >= REGISTER_X) {
          return ERROR_OUT_OF_RANGE;
        }
      }
      else if(*d < FIRST_LOCAL_REGISTER) { // stack and global lettered registers (XYZT ABCD LIJK)
        if(*d + *n >= FIRST_LOCAL_REGISTER) {
          return ERROR_OUT_OF_RANGE;
        }
      }
      else if(*d < FIRST_LOCAL_REGISTER + currentNumberOfLocalRegisters) { // local registers
        if(*d + *n >= FIRST_LOCAL_REGISTER + currentNumberOfLocalRegisters) {
          return ERROR_OUT_OF_RANGE;
        }
      }
      else {
        return ERROR_OUT_OF_RANGE;
      }
    }

    return ERROR_NONE;
  }
  else {
    *s = *n = 0;
    if(d) {
      *d = 0;
    }
    return ERROR_INVALID_DATA_TYPE_FOR_OP;
  }
}



void fnRegClr(uint16_t unusedButMandatoryParameter) {
  uint16_t s, n;

  if((lastErrorCode = getRegParam(NULL, &s, &n, NULL)) == ERROR_NONE) {
    for(int i = s; i < (s + n); ++i) {
      clearRegister(i);
    }
  }
  else {
    displayCalcErrorMessage(lastErrorCode, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
  }
}



static void sortReg(uint16_t range_start, uint16_t range_end) {
  int8_t res;

  if(range_start == range_end) {
    // do nothing
  }
  else if(range_start + 1 == range_end) {
    if(registerCmp(range_start, range_end, &res)) {
      if(res > 0) {
        registerHeader_t savedRegisterHeader = globalRegister[range_start];
        globalRegister[range_start] = globalRegister[range_end];
        globalRegister[range_end] = savedRegisterHeader;
      }
    }
  }
  else {
    const uint16_t range_center = (range_end - range_start) / 2 + range_start;
    uint16_t pos1 = range_start, pos2 = range_center + 1;
    registerHeader_t *sortedReg = allocWp43(TO_BLOCKS(sizeof(registerHeader_t)) * (range_end - range_start + 1));
    if(lastErrorCode == ERROR_RAM_FULL) {
      return; // unlikely
    }

    if(sortedReg) {
      sortReg(range_start,      range_center);
      sortReg(range_center + 1, range_end   );

      for(uint16_t i = 0; i <= (range_end - range_start); ++i) {
        if(registerCmp(pos1, pos2, &res)) {
          if(pos2 > range_end) {
            sortedReg[i] = globalRegister[pos1++];
          }
          else if(pos1 > range_center) {
            sortedReg[i] = globalRegister[pos2++];
          }
          else if(res > 0) {
            sortedReg[i] = globalRegister[pos2++];
          }
          else {
            sortedReg[i] = globalRegister[pos1++];
          }
        }
      }
      for(uint16_t i = 0; i <= (range_end - range_start); ++i) {
        globalRegister[range_start + i] = sortedReg[i];
      }
      freeWp43(sortedReg, TO_BLOCKS(sizeof(registerHeader_t)) * (range_end - range_start + 1));
    }
    else { // unlikely
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }
}



void fnRegSort(uint16_t unusedButMandatoryParameter) {
  uint16_t s, n;

  if((lastErrorCode = getRegParam(NULL, &s, &n, NULL)) == ERROR_NONE) {
    switch(getRegisterDataType(s)) {
      case dtLongInteger:
      case dtShortInteger:
      case dtReal34: {
        for(int i = s + 1; i < (s + n); ++i) {
          if((getRegisterDataType(i) != dtLongInteger) && (getRegisterDataType(i) != dtShortInteger) && (getRegisterDataType(i) != dtReal34)) {
            displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
            break;
          }
        }
        break;
      }
      case dtTime:
      case dtDate:
      case dtString: {
        for(int i = s + 1; i < (s + n); ++i) {
          if(getRegisterDataType(i) != getRegisterDataType(s)) {
            displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
            break;
          }
        }
        break;
      }
    }
    if(lastErrorCode == ERROR_NONE) {
      sortReg(s, s + n - 1);
    }
  }
  else {
    displayCalcErrorMessage(lastErrorCode, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
  }
}



void fnRegCopy(uint16_t unusedButMandatoryParameter) {
  bool     f;
  uint16_t s, n, d;

  if((lastErrorCode = getRegParam(&f, &s, &n, &d)) == ERROR_NONE) {
    if(f) {
      doLoad(LM_REGISTERS_PARTIAL, s, n, d);
    }
    else {
      if(s > d) {
        for(int i = 0; i < n; ++i) {
          copySourceRegisterToDestRegister(s + i, d + i);
          if(lastErrorCode == ERROR_RAM_FULL) {
            return; // abort if not enough memory
          }
        }
      }
      else if(s < d) {
        for(int i = n - 1; i >= 0; --i) {
          copySourceRegisterToDestRegister(s + i, d + i);
          if(lastErrorCode == ERROR_RAM_FULL) {
            return; // abort if not enough memory
          }
        }
      }
    }
  }
  else {
    displayCalcErrorMessage(lastErrorCode, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
  }
}



void fnRegSwap(uint16_t unusedButMandatoryParameter) {
  uint16_t s, n, d;

  if((lastErrorCode = getRegParam(NULL, &s, &n, &d)) == ERROR_NONE) {
    if((d < s + n) && (s < d + n)) { // overlap
      displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
    else {
      for(int i = 0; i < n; ++i) {
        registerHeader_t savedRegisterHeader = globalRegister[s + i];
        globalRegister[s + i] = globalRegister[d + i];
        globalRegister[d + i] = savedRegisterHeader;
      }
    }
  }
  else {
    displayCalcErrorMessage(lastErrorCode, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
  }
}
