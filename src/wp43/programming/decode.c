// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "programming/decode.h"

#include "calcMode.h"
#include "charString.h"
#include "dateTime.h"
#include "display.h"
#include "flags.h"
#include "fonts.h"
#include "items.h"
#include "programming/flash.h"
#include "programming/manage.h"
#include "programming/nextStep.h"
#include "registers.h"
#include <stdbool.h>
#include <string.h>

#include "wp43.h"

TO_QSPI const char shuffleReg[4] = {'x', 'y', 'z', 't'};
TO_QSPI const char supDigit[24] = STD_SUP_0 STD_SUP_1 STD_SUP_2 STD_SUP_3 STD_SUP_4 STD_SUP_5 STD_SUP_6 STD_SUP_7 STD_SUP_8 STD_SUP_9;
TO_QSPI const char baseChars[36] = "??" STD_BASE_1 STD_BASE_2 STD_BASE_3 STD_BASE_4 STD_BASE_5 STD_BASE_6 STD_BASE_7 STD_BASE_8 STD_BASE_9 STD_BASE_10 STD_BASE_11 STD_BASE_12 STD_BASE_13 STD_BASE_14 STD_BASE_15 STD_BASE_16;

#if !defined(DMCP_BUILD)
  void listPrograms(void) {
    uint16_t i, numberOfBytesInStep, stepNumber = 0, programNumber = 0;
    uint8_t *nextStep, *step;

    printf("\nProgram listing");
    step = beginOfProgramMemory;
    while(step) {
      if(step == programList[programNumber].instructionPointer.ram) {
        programNumber++;
        if(programNumber != 1) {
          printf("\n------------------------------------------------------------");
        }
        printf("\nPgm Step   Bytes         OP");
      }

      nextStep = findNextStep_ram(step);
      if(nextStep) {
        numberOfBytesInStep = (uint16_t)(nextStep - step);
        printf("\n%02d  %4d  ", programNumber, ++stepNumber - programList[programNumber - 1].step + 1); fflush(stdout);

        for(i=0; i<numberOfBytesInStep; i++) {
          printf(" %02x", *(step + i)); fflush(stdout);
          if(i == 3 && numberOfBytesInStep > 4) {
            decodeOneStep_ram(step);
            stringToUtf8(tmpString, (uint8_t *)(tmpString + 2000));

            if(!checkOpCodeOfStep(step, ITM_LBL) && !isAtEndOfProgram(step)) { // Not LBL and not END
              printf("   "); fflush(stdout);
            }

            printf("   %s", tmpString + 2000); fflush(stdout);
          }

          if(i%4 == 3 && i != numberOfBytesInStep - 1) {
            printf("\n          "); fflush(stdout);
          }
        }

        if(numberOfBytesInStep <= 4) {
          for(i=1; i<=4 - ((numberOfBytesInStep - 1) % 4); i++) {
            printf("   "); fflush(stdout);
          }
          decodeOneStep_ram(step);
          stringToUtf8(tmpString, (uint8_t *)(tmpString + 2000));

          if(!checkOpCodeOfStep(step, ITM_LBL) && !isAtEndOfProgram(step)) { // Not LBL and not END
            printf("   "); fflush(stdout);
          }

          printf("%s", tmpString + 2000); fflush(stdout);
        }
      }

      step = nextStep;
    }
    printf("\n");
  }



  void listLabelsAndPrograms(void) {
    printf("\nContent of labelList\n");
    printf("num program  step label\n");
    for(int i=0; i<numberOfLabels; i++) {
      printf("%3d%8d%6d ", i, labelList[i].program, labelList[i].step);
      if(labelList[i].program < 0) { // Flash
        readStepInFlashPgmLibrary((uint8_t *)(tmpString + 200), 32, labelList[i].labelPointer.flash);
        if(labelList[i].step < 0) { // Local label
          if(*((uint8_t *)(tmpString + 200)) < 100) {
            printf("%02d\n", *((uint8_t *)(tmpString + 200)));
          }
          else if(*((uint8_t *)(tmpString + 200)) < 105) {
            printf("%c\n", *((uint8_t *)(tmpString + 200)) - 100 + 'A');
          }
        }
        else { // Global label
          xcopy(tmpString + 100, (uint8_t *)(tmpString + 200) + 1, *((uint8_t *)(tmpString + 200)));
          tmpString[100 + *((uint8_t *)(tmpString + 200))] = 0;
          stringToUtf8(tmpString + 100, (uint8_t *)tmpString);
          printf("'%s'\n", tmpString);
        }
      }
      else { // RAM
        if(labelList[i].step < 0) { // Local label
          if(*(labelList[i].labelPointer.ram) < 100) {
            printf("%02d\n", *(labelList[i].labelPointer.ram));
          }
          else if(*(labelList[i].labelPointer.ram) < 105) {
            printf("%c\n", *(labelList[i].labelPointer.ram) - 100 + 'A');
          }
        }
        else { // Global label
          xcopy(tmpString + 100, labelList[i].labelPointer.ram + 1, *(labelList[i].labelPointer.ram));
          tmpString[100 + *(labelList[i].labelPointer.ram)] = 0;
          stringToUtf8(tmpString + 100, (uint8_t *)tmpString);
          printf("'%s'\n", tmpString);
        }
      }
    }

    printf("\nContent of programList\n");
    printf("program  step OP\n");
    for(int i=0; i<numberOfPrograms; i++) {
      if(programList[i].step < 0) { // Flash
        readStepInFlashPgmLibrary((uint8_t *)(tmpString + 1600), 400, programList[i].instructionPointer.flash);
        decodeOneStep_ram((uint8_t *)(tmpString + 1600));
        stringToUtf8(tmpString, (uint8_t *)(tmpString + 2000));
        printf("%7d %5d %s\n", i, programList[i].step, tmpString);
      }
      else {
        decodeOneStep_ram(programList[i].instructionPointer.ram);
        stringToUtf8(tmpString, (uint8_t *)(tmpString + 2000));
        printf("%7d %5d %s\n", i, programList[i].step, tmpString);
      }
    }
  }
#endif // !DMCP_BUILD



static void getStringLabelOrVariableName(uint8_t *stringAddress) {
  uint8_t stringLength = *(uint8_t *)(stringAddress++);
  xcopy(tmpStringLabelOrVariableName, stringAddress, stringLength);
  tmpStringLabelOrVariableName[stringLength] = 0;
}



static void getIndirectRegister(uint8_t *paramAddress, const char *op) {
  uint8_t opParam = *(uint8_t *)paramAddress;
  if(opParam < REGISTER_X) { // Global register from 00 to 99
    sprintf(tmpString, "%s " STD_RIGHT_ARROW "%02u", op, opParam);
  }
  else if(opParam <= REGISTER_K) { // Lettered register from X to K
    sprintf(tmpString, "%s " STD_RIGHT_ARROW "%s", op, indexOfItems[ITM_REG_X + opParam - REGISTER_X].itemSoftmenuName);
  }
  else if(opParam <= LAST_LOCAL_REGISTER) { // Local register from .00 to .98
    sprintf(tmpString, "%s " STD_RIGHT_ARROW ".%02d", op, opParam - FIRST_LOCAL_REGISTER);
  }
  else {
    sprintf(tmpString, "\nIn function getIndirectRegister: %s " STD_RIGHT_ARROW " %u is not a valid parameter!", op, opParam);
  }
}



static void getIndirectVariable(uint8_t *stringAddress, const char *op) {
  getStringLabelOrVariableName(stringAddress);
  sprintf(tmpString, "%s " STD_RIGHT_ARROW STD_LEFT_SINGLE_QUOTE "%s" STD_RIGHT_SINGLE_QUOTE, op, tmpStringLabelOrVariableName);
}



static void decodeOp(uint8_t *paramAddress, const char *op, uint16_t paramMode, uint16_t tamMax) {
  uint8_t opParam = *(uint8_t *)(paramAddress++);

  switch(paramMode) {
    case PARAM_DECLARE_LABEL: {
      if(opParam <= 99) { // Local label from 00 to 99
        sprintf(tmpString, "%s %02u", op, opParam);
      }
      else if(opParam <= 104) { // Local label from A to E
        sprintf(tmpString, "%s %c", op, 'A' + (opParam - 100));
      }
      else if(opParam == STRING_LABEL_VARIABLE) {
        getStringLabelOrVariableName(paramAddress);
        sprintf(tmpString, "%s " STD_LEFT_SINGLE_QUOTE "%s" STD_RIGHT_SINGLE_QUOTE, op, tmpStringLabelOrVariableName);
      }
      else {
        sprintf(tmpString, "\nIn function decodeOp case PARAM_DECLARE_LABEL: opParam %u is not a valid label!\n", opParam);
      }
      break;
    }

    case PARAM_LABEL: {
      if(opParam <= 99) { // Local label from 00 to 99
        sprintf(tmpString, "%s %02u", op, opParam);
      }
      else if(opParam <= 104) { // Local label from A to E
        sprintf(tmpString, "%s %c", op, 'A' + (opParam - 100));
      }
      else if(opParam == STRING_LABEL_VARIABLE) {
        getStringLabelOrVariableName(paramAddress);
        sprintf(tmpString, "%s " STD_LEFT_SINGLE_QUOTE "%s" STD_RIGHT_SINGLE_QUOTE, op, tmpStringLabelOrVariableName);
      }
      else if(opParam == INDIRECT_REGISTER) {
        getIndirectRegister(paramAddress, op);
      }
      else if(opParam == INDIRECT_VARIABLE) {
        getIndirectVariable(paramAddress, op);
      }
      else {
        sprintf(tmpString, "\nIn function decodeOp: case PARAM_LABEL, %s  %u is not a valid parameter!", op, opParam);
      }
      break;
    }

    case PARAM_REGISTER: {
      if(opParam < REGISTER_X) { // Global register from 00 to 99
        sprintf(tmpString, "%s %02u", op, opParam);
      }
      else if(opParam <= REGISTER_K) { // Lettered register from X to K
        sprintf(tmpString, "%s %s", op, indexOfItems[ITM_REG_X + opParam - REGISTER_X].itemSoftmenuName);
      }
      else if(opParam <= LAST_LOCAL_REGISTER) { // Local register from .00 to .98
        sprintf(tmpString, "%s .%02d", op, opParam - FIRST_LOCAL_REGISTER);
      }
      else if(opParam == STRING_LABEL_VARIABLE) {
        getStringLabelOrVariableName(paramAddress);
        sprintf(tmpString, "%s " STD_LEFT_SINGLE_QUOTE "%s" STD_RIGHT_SINGLE_QUOTE, op, tmpStringLabelOrVariableName);
      }
      else if(opParam == INDIRECT_REGISTER) {
        getIndirectRegister(paramAddress, op);
      }
      else if(opParam == INDIRECT_VARIABLE) {
        getIndirectVariable(paramAddress, op);
      }
      else {
        sprintf(tmpString, "\nIn function decodeOp: case PARAM_REGISTER, %s  %u is not a valid parameter!", op, opParam);
      }
      break;
    }

    case PARAM_FLAG: {
      if(opParam < REGISTER_X) { // Global flag from 00 to 99
        sprintf(tmpString, "%s %02u", op, opParam);
      }
      else if(opParam <= REGISTER_K) { // Lettered flag from X to K
        sprintf(tmpString, "%s %s", op, indexOfItems[ITM_REG_X + opParam - REGISTER_X].itemSoftmenuName);
      }
      else if(opParam <= LAST_LOCAL_FLAG) { // Local flag from .00 to .15 (or .31)
        sprintf(tmpString, "%s .%02d", op, opParam - FIRST_LOCAL_FLAG);
      }
      else if(FIRST_LOCAL_FLAG + NUMBER_OF_LOCAL_FLAGS <= opParam && opParam < FIRST_LOCAL_FLAG + NUMBER_OF_LOCAL_FLAGS + NUMBER_OF_SYSTEM_FLAGS) { // Local register from .00 to .15 (or .31)
        sprintf(tmpString, "%s .%02d", op, opParam - FIRST_LOCAL_FLAG);
      }
      else if(opParam == SYSTEM_FLAG_NUMBER) {
        sprintf(tmpString, "%s " STD_LEFT_SINGLE_QUOTE "%s" STD_RIGHT_SINGLE_QUOTE, op, indexOfItems[*paramAddress + SFL_TDM24].itemSoftmenuName);
      }
      else if(opParam == INDIRECT_REGISTER) {
        getIndirectRegister(paramAddress, op);
      }
      else if(opParam == INDIRECT_VARIABLE) {
        getIndirectVariable(paramAddress, op);
      }
      else {
        sprintf(tmpString, "\nIn function decodeOp: case PARAM_FLAG, %s  %u is not a valid parameter!", op, opParam);
      }
      break;
    }

    case PARAM_NUMBER_8: {
      if(opParam <= tamMax) { // Value from 0 to 99
        if(tamMax <= 9) {
          sprintf(tmpString, "%s %u", op, opParam);
        }
        else if(tamMax <= 99) {
          sprintf(tmpString, "%s %02u", op, opParam);
        }
        else {
          sprintf(tmpString, "%s %03u", op, opParam);
        }
      }
      else if(opParam == INDIRECT_REGISTER) {
        getIndirectRegister(paramAddress, op);
      }
      else if(opParam == INDIRECT_VARIABLE) {
        getIndirectVariable(paramAddress, op);
      }
      else {
        sprintf(tmpString, "\nIn function decodeOp: case PARAM_NUMBER, %s  %u is not a valid parameter!", op, opParam);
      }
      break;
    }

    case PARAM_NUMBER_16: {
      sprintf(tmpString, "%s %u", op, opParam + 256 * *(paramAddress));
      break;
    }

    case PARAM_COMPARE: {
      if(opParam < REGISTER_X) { // Global register from 00 to 99
        sprintf(tmpString, "%s %02u", op, opParam);
      }
      else if(opParam <= REGISTER_K) { // Lettered register from X to K
        sprintf(tmpString, "%s %s", op, indexOfItems[ITM_REG_X + opParam - REGISTER_X].itemSoftmenuName);
      }
      else if(opParam <= LAST_LOCAL_REGISTER) { // Local register from .00 to .98
        sprintf(tmpString, "%s .%02d", op, opParam - FIRST_LOCAL_REGISTER);
      }
      else if(opParam == STRING_LABEL_VARIABLE) {
        getStringLabelOrVariableName(paramAddress);
        sprintf(tmpString, "%s " STD_LEFT_SINGLE_QUOTE "%s" STD_RIGHT_SINGLE_QUOTE, op, tmpStringLabelOrVariableName);
      }
      else if(opParam == VALUE_0) {
        sprintf(tmpString, "%s 0.", op);
      }
      else if(opParam == VALUE_1) {
        sprintf(tmpString, "%s 1.", op);
      }
      else if(opParam == INDIRECT_REGISTER) {
        getIndirectRegister(paramAddress, op);
      }
      else if(opParam == INDIRECT_VARIABLE) {
        getIndirectVariable(paramAddress, op);
      }
      else {
        sprintf(tmpString, "\nIn function decodeOp: case PARAM_COMPARE, %s  %u is not a valid parameter!", op, opParam);
      }
      break;
    }

    case PARAM_KEYG_KEYX: {
      uint8_t *secondParam = findKey2ndParam_ram(paramAddress - 3);
      decodeOp(secondParam + 1, indexOfItems[*secondParam].itemCatalogName, PARAM_LABEL, indexOfItems[*secondParam].tamMinMax & TAM_MAX_MASK);
      xcopy(tmpString + TMP_STR_LENGTH / 2, tmpString, stringByteLength(tmpString) + 1);
      decodeOp(paramAddress - 1, op, PARAM_NUMBER_8, 21);
      tmpString[stringByteLength(tmpString) + 1] = 0;
      tmpString[stringByteLength(tmpString)    ] = ' ';
      xcopy(tmpString + stringByteLength(tmpString), tmpString + TMP_STR_LENGTH / 2, stringByteLength(tmpString + TMP_STR_LENGTH / 2) + 1);
      break;
    }

    case PARAM_SKIP_BACK: {
      sprintf(tmpString, "%s %03u", op, opParam);
      break;
    }

    case PARAM_SHUFFLE: {
      sprintf(tmpString, "%s %c%c%c%c", op, shuffleReg[ opParam & 0x03      ],
                                            shuffleReg[(opParam & 0x0c) >> 2],
                                            shuffleReg[(opParam & 0x30) >> 4],
                                            shuffleReg[(opParam & 0xc0) >> 6]);
      break;
    }

    default: {
      sprintf(tmpString, "\nIn function decodeOp: paramMode %u is not valid!\n", paramMode);
    }
  }
}



static void _decodeNumeral(char *startPtr, const char *srcStartPtr, bool isLongInt, char **updatedTgtPtr, const char **updatedSrcPtr) {
  int32_t digit;
  char *strPtr = startPtr;
  const char *srcStr = srcStartPtr;

  if(*srcStr == '-') {
    ++srcStr;
  }
  for(digit = 0; ((*srcStr >= '0' && *srcStr <= '9') || (*srcStr >= 'A' && *srcStr <= 'F')); ++digit, ++srcStr) {
  }
  srcStr = srcStartPtr;

  if(*srcStr == '-') {
    *(strPtr++) = *(srcStr++);
  }
  while((*srcStr >= '0' && *srcStr <= '9') || (*srcStr >= 'A' && *srcStr <= 'F') || *srcStr == '.' || *srcStr == ',') {
    if(digit == 0) {
      *(strPtr++) = RADIX34_MARK_CHAR;
      ++srcStr;
    }
    else {
      if(groupingGap > 0 && digit < -1 && (abs(digit) % groupingGap) == 1) {
        *(strPtr++) = STD_SPACE_PUNCTUATION[0];
        *(strPtr++) = STD_SPACE_PUNCTUATION[1];
      }
      *(strPtr++) = *(srcStr++);
      if(groupingGap > 0 && digit > 1 && (digit % groupingGap) == 1) {
        *(strPtr++) = STD_SPACE_PUNCTUATION[0];
        *(strPtr++) = STD_SPACE_PUNCTUATION[1];
      }
    }
    --digit;
  }
  if(digit == 0 && !isLongInt) {
    *(strPtr++) = RADIX34_MARK_CHAR;
  }

  if(*srcStr == 'e') {
    ++srcStr;
    *(strPtr++) = PRODUCT_SIGN[0];
    *(strPtr++) = PRODUCT_SIGN[1];
    *(strPtr++) = STD_SUB_10[0];
    *(strPtr++) = STD_SUB_10[1];
    if(*srcStr == '-') {
      *(strPtr++) = STD_SUP_MINUS[0];
      *(strPtr++) = STD_SUP_MINUS[1];
      ++srcStr;
    }
    else if(*srcStr == '+') {
      ++srcStr;
    }
    while(*srcStr >= '0' && *srcStr <= '9') {
      *(strPtr++) = supDigit[0 + (*srcStr - '0') * 2];
      *(strPtr++) = supDigit[1 + (*srcStr - '0') * 2];
      ++srcStr;
    }
  }
  else if(*srcStr == '#') { // input not yet closed
    *(strPtr++) = *(srcStr++);
    while(*srcStr >= '0' && *srcStr <= '9') {
      *(strPtr++) = *(srcStr++);
    }
  }

  *strPtr = 0;
  if(updatedTgtPtr) {
    *updatedTgtPtr = strPtr;
  }
  if(updatedSrcPtr) {
    *updatedSrcPtr = srcStr;
  }
}



static void decodeLiteral(uint8_t *literalAddress) {
  switch(*(uint8_t *)(literalAddress++)) {
    case BINARY_SHORT_INTEGER: {
      reallocateRegister(TEMP_REGISTER_1, dtShortInteger, SHORT_INTEGER_SIZE_IN_BYTES, *(uint8_t *)(literalAddress++));
      xcopy(REGISTER_DATA(TEMP_REGISTER_1), literalAddress, SHORT_INTEGER_SIZE_IN_BYTES);
      shortIntegerToDisplayString(TEMP_REGISTER_1, tmpString, false);
      break;
    }

    //case BINARY_LONG_INTEGER: {
    //  break;
    //}

    case BINARY_REAL34: {
      real34ToDisplayString((real34_t *)literalAddress, amNone, tmpString, &standardFont, 9999, 34, false, STD_SPACE_PUNCTUATION, false);
      break;
    }

    case BINARY_COMPLEX34: {
      complex34_t complexLiteral;
      xcopy(VARIABLE_REAL34_DATA(&complexLiteral), literalAddress     , 16);
      xcopy(VARIABLE_IMAG34_DATA(&complexLiteral), literalAddress + 16, 16);
      complex34ToDisplayString(&complexLiteral, tmpString, &standardFont, 9999, 34, false, STD_SPACE_PUNCTUATION, false);
      break;
    }

    //case BINARY_DATE: {
    //  break;
    //}

    //case BINARY_TIME: {
    //  break;
    //}

    case STRING_SHORT_INTEGER: {
      int32_t digit;
      uint8_t gap = groupingGap;
      char *dispStringPtr = tmpString;
      char *sourceStringPtr = tmpStringLabelOrVariableName;
      uint8_t base = (uint8_t)(*literalAddress);
      getStringLabelOrVariableName(literalAddress + 1);

      if(groupingGap > 0) {
        if(base == 2) {
          gap = 4;
        }
        else if(base == 4 || base == 8 || base == 16) {
          gap = 2;
        }
      }

      if(*sourceStringPtr == '-') {
        ++sourceStringPtr;
      }
      for(digit = 0; (*sourceStringPtr >= '0' && *sourceStringPtr <= '9') || (*sourceStringPtr >= 'A' && *sourceStringPtr <= 'F'); ++digit, ++sourceStringPtr) {
      }
      sourceStringPtr = tmpStringLabelOrVariableName;

      if(*sourceStringPtr == '-') {
        *(dispStringPtr++) = *(sourceStringPtr++);
      }
      while((*sourceStringPtr >= '0' && *sourceStringPtr <= '9') || (*sourceStringPtr >= 'A' && *sourceStringPtr <= 'F')) {
        *(dispStringPtr++) = *(sourceStringPtr++);
        if(gap > 0 && digit > 1 && (digit % gap) == 1) {
          *(dispStringPtr++) = STD_SPACE_PUNCTUATION[0];
          *(dispStringPtr++) = STD_SPACE_PUNCTUATION[1];
        }
        --digit;
      }
      *(dispStringPtr++) = baseChars[base * 2    ];
      *(dispStringPtr++) = baseChars[base * 2 + 1];
      *dispStringPtr = 0;
      break;
    }

    case STRING_LONG_INTEGER: {
      getStringLabelOrVariableName(literalAddress);
      _decodeNumeral(tmpString, tmpStringLabelOrVariableName, true, NULL, NULL);
      break;
    }

    case STRING_REAL34: {
      getStringLabelOrVariableName(literalAddress);
      _decodeNumeral(tmpString, tmpStringLabelOrVariableName, false, NULL, NULL);
      break;
    }

    case STRING_COMPLEX34:  {
      char *dispStringPtr = tmpString;
      char *sourceStringPtr = tmpStringLabelOrVariableName;
      getStringLabelOrVariableName(literalAddress);
      _decodeNumeral(dispStringPtr, sourceStringPtr, false, &dispStringPtr, (const char **)&sourceStringPtr);
      if(*sourceStringPtr == 'i' || *sourceStringPtr == 'j') {
        *(dispStringPtr++) = '+';
        *(dispStringPtr++) = '+';
        *(dispStringPtr++) = COMPLEX_UNIT[0];
        ++sourceStringPtr;
      }
      else if(*sourceStringPtr == '+' || *sourceStringPtr == '-') {
        *(dispStringPtr++) = *(sourceStringPtr++);
        *(dispStringPtr++) = COMPLEX_UNIT[0];
        ++sourceStringPtr;
      }
      *(dispStringPtr++) = PRODUCT_SIGN[0];
      *(dispStringPtr++) = PRODUCT_SIGN[1];
      _decodeNumeral(dispStringPtr, sourceStringPtr, calcMode == cmPem && aimBuffer[0] != 0 && (currentStep.ram + 2 == literalAddress), NULL, NULL);
      break;
    }

    case STRING_LABEL_VARIABLE: {
      getStringLabelOrVariableName(literalAddress);
      sprintf(tmpString, STD_LEFT_SINGLE_QUOTE "%s" STD_RIGHT_SINGLE_QUOTE, tmpStringLabelOrVariableName);
      break;
    }

    case STRING_DATE: {
      getStringLabelOrVariableName(literalAddress);
      reallocateRegister(TEMP_REGISTER_1, dtDate, REAL34_SIZE_IN_BYTES, amNone);
      stringToReal34(tmpStringLabelOrVariableName, REGISTER_REAL34_DATA(TEMP_REGISTER_1));
      julianDayToInternalDate(REGISTER_REAL34_DATA(TEMP_REGISTER_1), REGISTER_REAL34_DATA(TEMP_REGISTER_1));
      dateToDisplayString(TEMP_REGISTER_1, tmpString);
      break;
    }

    case STRING_TIME: {
      char *timeStringPtr = tmpString;
      char *sourceStringPtr = tmpStringLabelOrVariableName;
      getStringLabelOrVariableName(literalAddress);
      for(; *sourceStringPtr != '.' && *sourceStringPtr != 0; ++sourceStringPtr) {
        *(timeStringPtr++) = *sourceStringPtr;
      }
      if(*sourceStringPtr == '.') {
        ++sourceStringPtr;
      }
      *(timeStringPtr++) = ':';
      if(*sourceStringPtr != 0) {
        *(timeStringPtr++) = *(sourceStringPtr++);
      }
      else {
        *(timeStringPtr++) = '0';
      }
      if(*sourceStringPtr != 0) {
        *(timeStringPtr++) = *(sourceStringPtr++);
      }
      else {*(timeStringPtr++) = '0';
      }
      *(timeStringPtr++) = ':';
      if(*sourceStringPtr != 0) {
        *(timeStringPtr++) = *(sourceStringPtr++);
      }
      else {
        *(timeStringPtr++) = '0';
      }
      if(*sourceStringPtr != 0) {
        *(timeStringPtr++) = *(sourceStringPtr++);
      }
      else {
        *(timeStringPtr++) = '0';
      }
      if(*sourceStringPtr != 0) {
        *(timeStringPtr++) = '.';
      }
      for(; *sourceStringPtr != 0; ++sourceStringPtr) {
        *(timeStringPtr++) = *sourceStringPtr;
      }
      *(timeStringPtr++) = 0;
      break;
    }

    case STRING_ANGLE_DMS: {
      char *angleStringPtr = tmpString;
      char *sourceStringPtr = tmpStringLabelOrVariableName;
      getStringLabelOrVariableName(literalAddress);
      for(; *sourceStringPtr != '.' && *sourceStringPtr != 0; ++sourceStringPtr) {
        *(angleStringPtr++) = *sourceStringPtr;
      }
      if(*sourceStringPtr == '.') {
        ++sourceStringPtr;
      }
      *(angleStringPtr++) = STD_DEGREE[0];
      *(angleStringPtr++) = STD_DEGREE[1];
      if(*sourceStringPtr != 0) {
        *(angleStringPtr++) = *(sourceStringPtr++);
      }
      else {
        *(angleStringPtr++) = '0';
      }
      if(*sourceStringPtr != 0) {
        *(angleStringPtr++) = *(sourceStringPtr++);
      }
      else {
        *(angleStringPtr++) = '0';
      }
      *(angleStringPtr++) = '\'';
      if(*sourceStringPtr != 0) {
        *(angleStringPtr++) = *(sourceStringPtr++);
      }
      else {
        *(angleStringPtr++) = '0';
      }
      if(*sourceStringPtr != 0) {
        *(angleStringPtr++) = *(sourceStringPtr++);
      }
      else {
        *(angleStringPtr++) = '0';
      }
      if(*sourceStringPtr != 0) {
        *(angleStringPtr++) = '.';
      }
      for(; *sourceStringPtr != 0; ++sourceStringPtr) {
        *(angleStringPtr++) = *sourceStringPtr;
      }
      *(angleStringPtr++) = '"';
      *(angleStringPtr++) = 0;
      break;
    }

    default: {
      #if !defined(DMCP_BUILD)
        printf("\nERROR: %u is not an acceptable parameter for ITM_LITERAL!\n", *(uint8_t *)(literalAddress - 1));
      #endif // !DMCP_BUILD
    }
  }
}



void decodeOneStep(pgmPtr_t step) {
  if(currentProgramNumber > (numberOfPrograms - numberOfProgramsInFlash)) { // Flash
    readStepInFlashPgmLibrary((uint8_t *)(tmpString + 1600), 400, step.flash);
    decodeOneStep_ram((uint8_t *)(tmpString + 1600));
  }
  else {
    decodeOneStep_ram(step.ram);
  }
}



void decodeOneStep_ram(uint8_t *step) {
  uint16_t op = *(step++);
  if(op & 0x80) {
    op &= 0x7f;
    op <<= 8;
    op |= *(step++);
  }

  if(op == 0x7fff) { // .END.
    xcopy(tmpString, ".END.", 6);
  }
  else {
    switch(indexOfItems[op].status & PTP_STATUS) {
      case PTP_NONE: {
        sprintf(tmpString, "%s%s", (CST_01 <= op && op <= CST_79) ? "# " : "", indexOfItems[op].itemCatalogName);
        break;
      }

      case PTP_DISABLED: {
        printf("\nERROR in decodeOneStep: instruction %u is not programmable!\n", op);
        break;
      }

      case PTP_LITERAL: {
        decodeLiteral(step);
        break;
      }

      default: {
        decodeOp(step, (op == ITM_INTEGRAL) ? STD_INTEGRAL "fd" : indexOfItems[op].itemCatalogName, (indexOfItems[op].status & PTP_STATUS) >> 9, indexOfItems[op].tamMinMax & TAM_MAX_MASK);
      }
    }
  }
}
