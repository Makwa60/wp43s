// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 and C47 Authors

/********************************************//**
 * \file ui/genericEditor.c
 ***********************************************/

#include "ui/genericEditor.h"

#include "apps/bugScreen.h"
#include "bufferize.h"
#include "calcMode.h"
#include "charString.h"
#include "constantPointers.h"
#include "conversionAngles.h"
#include "defines.h"
#include "display.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "fractions.h"
#include "items.h"
#include "registers.h"
#include "stack.h"
#include "tam.h"
#include "programming/decode.h"
#include "programming/manage.h"
#include "programming/nextStep.h"
#include "realType.h"
#include "registerValueConversions.h"
#include "saveRestoreCalcState.h"
#include "ui/cursor.h"
#include "ui/matrixEditor.h"
#include "ui/screen.h"
#include "ui/softmenus.h"
#include <string.h>

#include "wp43.h"


#if !defined(TESTSUITE_BUILD)
  static void getStringLabelOrVariableName(uint8_t *stringAddress) {
    uint8_t stringLength = *(uint8_t *)(stringAddress++);
    xcopy(tmpStringLabelOrVariableName, stringAddress, stringLength);
    tmpStringLabelOrVariableName[stringLength] = 0;
  }
#endif // !TESTSUITE_BUILD

void fractionToString(calcRegister_t regist, char *displayString) {
  int16_t  sign, lessEqualGreater;
  uint64_t intPart, numer, denom;

  fraction(regist, &sign, &intPart, &numer, &denom, &lessEqualGreater);

  if(getSystemFlag(FLAG_PROPFR)) { // a b/c
    sprintf(displayString, "%s%" PRIu64 " %" PRIu64 "/%" PRIu64, (sign == -1 ? "-" : "+"), intPart, numer, denom);
  }

  else { // FT_IMPROPER d/
    sprintf(displayString, "%s%" PRIu64 "/%" PRIu64, (sign == -1 ? "-" : "+"), numer, denom);

  }
}

void shortIntegerToString(calcRegister_t regist, char *displayString) {
  int16_t i, j, k, unit, digit, base;
  uint64_t number, sign;

  base    = getRegisterTag(regist);
  number  = *(REGISTER_SHORT_INTEGER_DATA(regist));

  if(base <= 1 || base >= 17) {
    sprintf(errorMessage, "In function shortIntegerToString: %d is an unexpected value for base!", base);
    bugScreen(errorMessage);
    base = 10;
  }

  //number &= shortIntegerMask;

  if(shortIntegerMode == SIM_UNSIGN || base == 2 || base == 4 || base == 8 || base == 16) {
    sign = 0;
  }
  else {
    sign = number & shortIntegerSignBit;
  }

  if(sign) {
    if(shortIntegerMode == SIM_2COMPL) {
      number |= ~shortIntegerMask;
      number = ~number + 1;
    }
    else if(shortIntegerMode == SIM_1COMPL) {
      number = ~number;
    }
    else if(shortIntegerMode == SIM_SIGNMT) {
      number &= ~shortIntegerSignBit;
    }
    else {
      sprintf(errorMessage, "In function shortIntegerToString: %d is an unexpected value for shortIntegerMode!", shortIntegerMode);
      bugScreen(errorMessage);
    }

    number &= shortIntegerMask;
  }

  i = ERROR_MESSAGE_LENGTH / 2;

  if(number == 0) {
    displayString[i++] = '0';
    digit = 1;
  }
  else {
    digit = 0;
  }

  while(number) {
    digit++;

    unit = number % base;
    number /= base;
    displayString[i++] = digits[unit];
  }

  if(sign) {
    displayString[i++] = '-';
  }
  else {
    displayString[i++] = '+';
  }

  for(k=i-1, j=0; k>=ERROR_MESSAGE_LENGTH / 2; k--, j++) {
    if(displayString[k] == ' ') {
      displayString[j++] = STD_SPACE_PUNCTUATION[0];
      displayString[j]   = STD_SPACE_PUNCTUATION[1];
    }
    else {
      displayString[j] = displayString[k];
    }
  }
  displayString[j] = 0;

  return;
}


#if !defined(TESTSUITE_BUILD)
static void _real34ToNim(const real34_t *real34, char *nimInput, char *nimDisplay) {
// nimInput   : used to fill aimBuffer
// nimDisplay : used to fill nimBufferDisplay
  uint16_t i;
  uint8_t groupingGapOld = groupingGap;

  groupingGap = 0;
  real34ToDisplayString(real34, amNone, tmpString, &standardFont, SCREEN_WIDTH, NUMBER_OF_DISPLAY_DIGITS, true, STD_SPACE_PUNCTUATION, true);
  groupingGap = groupingGapOld;
  //printf("**[DL]** tmpString %s\n",tmpString);fflush(stdout);

  bool noDisplayExponent = true;
  for(i = 0; i < strlen(tmpString); i++) {
    if((tmpString[i] == STD_SUB_10[0]) && (tmpString[i+1] == STD_SUB_10[1])) {
      noDisplayExponent = false;
    }
  }
  groupingGap = 0;
  real34ToString(real34, nimDisplay);
  groupingGap = groupingGapOld;
  //printf("**[DL]** nimBufferDisplay %s\n",nimBufferDisplay);fflush(stdout);
  bool dotFound = false;
  if(noDisplayExponent) {                                // if no exponent in display string but exponent in real34ToString, use the display string
    for(i = 0; i < strlen(nimDisplay); i++) {
      if((nimDisplay[i] == 'e') || (nimDisplay[i] == 'E')) {
        strcpy(nimDisplay, tmpString + (tmpString[0] == '-'? 0 : 1));
        break;
      }
      if(nimDisplay[i] == '.') {
        dotFound = true;
      }
    }
    if(dotFound) {
      for(i = strlen(nimDisplay)-1; i > 0; i--) {
        if((nimDisplay[i] == '0')) {
          nimDisplay[i] = 0;              // remove trailing zeros
        }
        else {
          break;
        }
      }
    }
  }
  if(real34IsPositive(real34)) {
    nimInput[0] = '+';
    strcpy(nimInput + 1, nimDisplay);
  }
  else {
    strcpy(nimInput, nimDisplay);
  }
  //printf("**[DL]** nimInput %s\n",nimInput);fflush(stdout);
  bool exponentFound = false;
  dotFound = false;
  for(i = 0; i < strlen(nimInput); i++) {
    if(nimInput[i] == 'E') {
      nimInput[i] = 'e';
      dotFound = true;
      exponentFound = true;
      exponentSignLocation = i + 1;
      nimNumberPart = NP_REAL_EXPONENT;
    }
    if(nimInput[i] == '.') {
      dotFound = true;
      nimNumberPart = NP_REAL_FLOAT_PART;
    }
  }
  if(!dotFound) {
    nimInput[i] = '.';
    nimNumberPart = NP_REAL_FLOAT_PART;
  }
  strcpy(nimDisplay, STD_SPACE_HAIR);
  //printf("**[DL]** nimNumberPart %d\n",nimNumberPart);fflush(stdout);
  nimBufferToDisplayBuffer(nimInput, nimDisplay + 2);
  if(exponentFound) {
    exponentToDisplayString(stringToInt32(nimInput + exponentSignLocation), nimDisplay + stringByteLength(nimDisplay), NULL, true, STD_SPACE_PUNCTUATION);
    if(nimInput[exponentSignLocation + 1] == 0 && nimInput[exponentSignLocation] == '-') {
      strcat(nimDisplay, STD_SUP_MINUS);
    }
    else if(nimInput[exponentSignLocation + 1] == '0' && nimInput[exponentSignLocation] == '+') {
      strcat(nimDisplay, STD_SUP_0);
    }
  }
}
#endif // !TESTSUITE_BUILD



void fnEdit (uint16_t unusedParamButMandatory) {
#if !defined(TESTSUITE_BUILD)
  int16_t  index;
  if(calcMode == cmNormal) {
    switch(getRegisterDataType(REGISTER_X)) {

      case dtLongInteger: {
        #define NIM_BUFFER_EXTENDED_LENGTH    1400      // provision for very long integers (up to 1000 digits + separators)
        memset(nimBufferDisplay, 0, NIM_BUFFER_EXTENDED_LENGTH);
        longInteger_t lgInt;
        convertLongIntegerRegisterToLongInteger(REGISTER_X, lgInt);
        longIntegerToAllocatedString(lgInt, nimBufferDisplay, NIM_BUFFER_EXTENDED_LENGTH);
        if(longIntegerIsPositiveOrZero(lgInt)) {
          aimBuffer[0] = '+';
          strcpy(aimBuffer + 1, nimBufferDisplay);
        }
        else {
          strcpy(aimBuffer, nimBufferDisplay);
        }
        longIntegerFree(lgInt);
        if(groupingGap > 0) {
          int16_t len = strlen(nimBufferDisplay);
          for(int16_t i=len - groupingGap; i>0; i-=groupingGap) {
            if(i != 1 || nimBufferDisplay[0] != '-') {
              xcopy(nimBufferDisplay + i + 2, nimBufferDisplay + i, len - i + 1);
              nimBufferDisplay[i] = *(STD_SPACE_PUNCTUATION);
              nimBufferDisplay[i + 1] = *(STD_SPACE_PUNCTUATION + 1);
              len += 2;
            }
          }
        }

        // Test if long inter number display string will fit on two lines in standard font, if not do nothing (cannot edit)
        if(stringWidth(nimBufferDisplay, &standardFont, true, true) <= (SCREEN_WIDTH - 8) * 2) { // 8 is the standard font cursor width
          clearSystemFlag(FLAG_ALPHA);
          calcMode = cmNim;
          real34Zero(REGISTER_REAL34_DATA(REGISTER_X));
          hexDigits = 0;
          nimNumberPart = NP_INT_10;
          clearRegisterLine(NIM_REGISTER_LINE, true, true);
          cursorShow(false, 1, Y_POSITION_OF_NIM_LINE);
        }
        else {
          memset(nimBufferDisplay, 0, NIM_BUFFER_EXTENDED_LENGTH);
          aimBuffer[0] = 0;
          nimBufferDisplay[0] = 0;
        }
        break;
      }

      case dtReal34: {
        uint8_t groupingGapOld = groupingGap;
        angularMode_t xangularMode = getRegisterAngularMode(REGISTER_X);

        memset(aimBuffer, 0, AIM_BUFFER_LENGTH);
        memset(nimBufferDisplay, 0, NIM_BUFFER_LENGTH);

        if(xangularMode == amMultPi) {
          real_t multPi;

          real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &multPi);
          realDivide(&multPi, const_pi, &multPi, &ctxtReal39);
          realToReal34(&multPi, REGISTER_REAL34_DATA(REGISTER_X));
        }
        else if(xangularMode == amDMS) {
          real34FromDegToDms(REGISTER_REAL34_DATA(REGISTER_X), REGISTER_REAL34_DATA(REGISTER_X));
        }

        if (getSystemFlag(FLAG_FRACT)) {
          groupingGap = 0;
          fractionToString(REGISTER_X, aimBuffer);
          groupingGap = groupingGapOld;
          nimNumberPart = NP_FRACTION_DENOMINATOR;
          strcpy(nimBufferDisplay, STD_SPACE_HAIR);
          nimBufferToDisplayBuffer(aimBuffer, nimBufferDisplay + 2);
          strcat(nimBufferDisplay, STD_SPACE_4_PER_EM);

          for(index=2; aimBuffer[index]!=' '; index++) {
          }
          supNumberToDisplayString(stringToInt32(aimBuffer + index + 1), nimBufferDisplay + stringByteLength(nimBufferDisplay), NULL, true, STD_SPACE_PUNCTUATION);

          strcat(nimBufferDisplay, "/");

          for(; aimBuffer[index]!='/'; index++) {
          }
          if(aimBuffer[++index] != 0) {
            subNumberToDisplayString(stringToInt32(aimBuffer + index), nimBufferDisplay + stringByteLength(nimBufferDisplay), NULL);
          }
        }
        else {
          _real34ToNim(REGISTER_REAL34_DATA(REGISTER_X), aimBuffer, nimBufferDisplay);
        }
        //printf("**[DL]** dtReal34 aimBuffer %s nimBufferDisplay %s\n",aimBuffer,nimBufferDisplay);fflush(stdout);

        clearSystemFlag(FLAG_ALPHA);
        calcMode = cmNim;
        //printf("**[DL]** AngularMode %d\n",getRegisterAngularMode(REGISTER_X));fflush(stdout);
        real34Zero(REGISTER_REAL34_DATA(REGISTER_X));
        //printf("**[DL]** AngularMode %d\n",getRegisterAngularMode(REGISTER_X));fflush(stdout);
        hexDigits = 0;
        clearRegisterLine(NIM_REGISTER_LINE, true, true);
        cursorShow(false, 1, Y_POSITION_OF_NIM_LINE);
        break;
      }

      case dtComplex34: {
        uint16_t i, j;
        uint16_t imaginaryDisplayStart;
        int16_t realExponentSignLocation;

        memset(aimBuffer, 0, AIM_BUFFER_LENGTH);
        memset(nimBufferDisplay, 0, NIM_BUFFER_LENGTH);

        _real34ToNim(REGISTER_REAL34_DATA(REGISTER_X), aimBuffer, nimBufferDisplay);
        realExponentSignLocation = exponentSignLocation;
        imaginaryMantissaSignLocation = strlen(aimBuffer);

        if(strncmp(nimBufferDisplay + stringByteLength(nimBufferDisplay) - 2, STD_SPACE_HAIR, 2) != 0) {
          strcat(nimBufferDisplay, STD_SPACE_HAIR);
        }
        if(real34IsPositive(REGISTER_IMAG34_DATA(REGISTER_X))) {
          strcat(aimBuffer, "+");
          strcat(nimBufferDisplay, "+");
        }
        else {
          strcat(aimBuffer, "-");
          strcat(nimBufferDisplay, "-");
        }
        strcat(nimBufferDisplay, COMPLEX_UNIT);
        strcat(nimBufferDisplay, PRODUCT_SIGN);
        imaginaryDisplayStart = strlen(nimBufferDisplay);
        _real34ToNim(REGISTER_IMAG34_DATA(REGISTER_X), aimBuffer + strlen(aimBuffer), nimBufferDisplay + strlen(nimBufferDisplay));
        aimBuffer[imaginaryMantissaSignLocation + 1] = 'i';

        // Remove SPACE HAIR and - sign in front of the imaginary part
        j = (nimBufferDisplay[imaginaryDisplayStart + 2] == '-' ? 3 : 2);
        for(i = imaginaryDisplayStart; i < strlen(nimBufferDisplay); i++) {
          nimBufferDisplay[i] = nimBufferDisplay[i+j];
        }

        nimNumberPart = NP_COMPLEX_FLOAT_PART;
        for(i = imaginaryMantissaSignLocation; i < strlen(aimBuffer); i++) {
          if(aimBuffer[i] == 'e') {
            imaginaryExponentSignLocation = i + 1;
            nimNumberPart = NP_COMPLEX_EXPONENT;
          }
        }

        exponentSignLocation = realExponentSignLocation;
        clearSystemFlag(FLAG_ALPHA);
        calcMode = cmNim;
        //real34Zero(REGISTER_REAL34_DATA(REGISTER_X));
        hexDigits = 0;
        clearRegisterLine(NIM_REGISTER_LINE, true, true);
        cursorShow(false, 1, Y_POSITION_OF_NIM_LINE);
        break;
      }

      case dtTime: {
        break;
      }

      case dtDate: {
        break;
      }

      case dtString: {
        setSystemFlag(FLAG_ASLIFT);
        if(stringByteLength(REGISTER_STRING_DATA(REGISTER_X)) < AIM_BUFFER_LENGTH) {
          strcpy(aimBuffer, REGISTER_STRING_DATA(REGISTER_X));
          alphaCursor = (uint32_t)stringGlyphLength(aimBuffer);
          fnDrop(NOPARAM);
          shiftF = false;
          shiftG = false;
          #if !defined(TESTSUITE_BUILD)
            calcModeAim(); // Alpha Input Mode
            showSoftmenu(-MNU_MyAlpha);
          #endif // !TESTSUITE_BUILD
        }
        break;
      }

      case dtReal34Matrix:
      case dtComplex34Matrix: {
        fnEditMatrix(NOPARAM);
        break;
      }

      case dtShortInteger: {
        uint16_t i;
        uint8_t groupingGapOld = groupingGap;

        memset(aimBuffer, 0, AIM_BUFFER_LENGTH);
        memset(nimBufferDisplay, 0, NIM_BUFFER_LENGTH);

        lastIntegerBase  = getRegisterTag(REGISTER_X);
        nimNumberPart = (lastIntegerBase <= 10 ? NP_INT_10 : NP_INT_16);

        groupingGap = 0;
        shortIntegerToString(REGISTER_X, aimBuffer);
        groupingGap = groupingGapOld;

        hexDigits   = 0;
        for(i = 0; i < strlen(aimBuffer); i++) {
          if((aimBuffer[i] >= 'A') && (aimBuffer[i] <= 'F')) {
            hexDigits++;
          }
        }

        strcpy(nimBufferDisplay, STD_SPACE_HAIR);
        nimBufferToDisplayBuffer(aimBuffer, nimBufferDisplay + 2);

        //printf("**[DL]** dtShortInteger aimBuffer %s nimBufferDisplay %s\n",aimBuffer,nimBufferDisplay);fflush(stdout);

        clearSystemFlag(FLAG_ALPHA);
        calcMode = cmNim;
        real34Zero(REGISTER_REAL34_DATA(REGISTER_X));
        clearRegisterLine(NIM_REGISTER_LINE, true, true);
        cursorShow(false, 1, Y_POSITION_OF_NIM_LINE);
        break;
      }

      // case dtConfig: Not relevant for EDIT
      default : {
      }
    }
  }
  else if(calcMode == cmPem) {
    --currentLocalStepNumber;
    currentStep = findPreviousStep(currentStep);
    int16_t func = currentStep[0];
    uint8_t opParam  = currentStep[1];
    uint8_t opParam2 = currentStep[2];
    uint8_t opParam3 = currentStep[3];
    if(func & 0x80) {
      func &= 0x7f;
      func <<= 8;
      func |= currentStep[1];
      opParam  = currentStep[2];
      opParam2 = currentStep[3];
      opParam3 = currentStep[4];
    }
    //printf("**[DL]** fnEdit cmPem func %d opParam %d opParam2 %d\n",func,opParam,opParam2);fflush(stdout);
    if((func == ITM_LITERAL || func == ITM_REM)) {
      memset(aimBuffer, 0, AIM_BUFFER_LENGTH);

      if(opParam == STRING_LABEL_VARIABLE) {
        pemAlpha(ITM_EDIT);
      }
      else if((opParam == BINARY_SHORT_INTEGER) || (opParam == STRING_SHORT_INTEGER) || (opParam == STRING_LONG_INTEGER) ||
              (opParam == BINARY_REAL34)        || (opParam == STRING_REAL34) ||
              (opParam == BINARY_COMPLEX34)     || (opParam == STRING_COMPLEX34)) {
        char *tempBuffer = errorMessage + 1500;
        bool chsNeeded = false;

        if((opParam == STRING_REAL34)|| (opParam == STRING_COMPLEX34))  {
          getStringLabelOrVariableName(&currentStep[2]);
          strcpy(tempBuffer, tmpStringLabelOrVariableName);
        }
        else {
          uint8_t groupingGapOld = groupingGap;
          groupingGap = 0;
          decodeOneStep(currentStep);
          groupingGap = groupingGapOld;
          strcpy(tempBuffer, tmpString);
        }
        //printf("**[DL]** fnEdit tempBuffer %s\n",tempBuffer);fflush(stdout);
        lastIntegerBase = (opParam == BINARY_SHORT_INTEGER ? opParam2: opParam == STRING_SHORT_INTEGER ? opParam2: 0);
        deleteStepsFromTo(currentStep, findNextStep(currentStep));

        uint16_t i;
        for(i = 0; i < strlen(tempBuffer); i++) {
          //printf("**[DL]** fnEdit tempBuffer[i] %02x aimBuffer %s\n",tempBuffer[i],aimBuffer);fflush(stdout);
          switch ((uint8_t) tempBuffer[i]) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
              pemAddNumber(ITM_0 + tempBuffer[i] - '0', false);
              break;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
              pemAddNumber(ITM_A + tempBuffer[i] - 'A', false);
              break;
            case '.':
              pemAddNumber(ITM_PERIOD, false);
              break;
            case '+':
              if(chsNeeded)  pemAddNumber(ITM_CHS, false);  // '-' was already encountered, let's first negate the real part
              chsNeeded = false;
              break;
            case '-':
              if(chsNeeded) pemAddNumber(ITM_CHS, false);  // second time '-' is encountered, let's first negate the real part
              chsNeeded = true;
              break;
            case 'e':
              if(chsNeeded) pemAddNumber(ITM_CHS, false);           // change mantissa sign before entering exponent
              chsNeeded = false;
              pemAddNumber(ITM_EXPONENT, false);
              break;
            case 'i':
              pemAddNumber(ITM_CC, false);
              break;
            case 0x80:
              i++;
              if((tempBuffer[i] == STD_CROSS[1]) && (nimNumberPart != NP_COMPLEX_INT_PART)) {
                i += 2; // Skip next character (STD_BASE_10)
                if(chsNeeded) pemAddNumber(ITM_CHS, false);         // change mantissa sign before entering exponent
                chsNeeded = false;
                pemAddNumber(ITM_EXPONENT, false);
              }
              break;
            case 0xa1:
              i++;
              if((tempBuffer[i] >= STD_SUP_0[1]) && (tempBuffer[i] <= STD_SUP_9[1])) {
                pemAddNumber(ITM_0 + tempBuffer[i] - STD_SUP_0[1], false);
              }
              else if((tempBuffer[i] == STD_SUP_MINUS[1])) {
                chsNeeded = true;
              }
              else if((tempBuffer[i] == STD_IMAGINARY_i[1])) {
                //printf("**[DL]** fnEdit pemAddNumber ITM_CC aimBuffer %s\n",aimBuffer);fflush(stdout);
                pemAddNumber(ITM_CC, false);
              }
              //printf("**[DL]** fnEdit pemAddNumber %02x aimBuffer %s\n",tempBuffer[i],aimBuffer);fflush(stdout);
              break;
            default:
              //printf("**[DL]** fnEdit tempBuffer[i] %02X\n",tempBuffer[i]);fflush(stdout);
              break;
          }
          lastIntegerBase = (opParam == BINARY_SHORT_INTEGER ? opParam2: opParam == STRING_SHORT_INTEGER ? opParam2: 0);
        }
        if(chsNeeded) pemAddNumber(ITM_CHS, false);
        pemAddNumber(ITM_NOP, true);    // to insert the resulting number in program
        //printf("**[DL]** fnEdit aimBuffer %s\n",aimBuffer);fflush(stdout);
      }
      else {
        currentLocalStepNumber++;
        currentStep = findNextStep(currentStep);
      }
    }
    else {
      uint16_t paramMode = (indexOfItems[func].status & PTP_STATUS) >> 9;
      switch (paramMode) {
        case PARAM_DECLARE_LABEL:
        case PARAM_LABEL:
        case PARAM_REGISTER:
        case PARAM_FLAG:
        case PARAM_NUMBER_8:
        case PARAM_NUMBER_16:            // Used only for "BestF"
        case PARAM_COMPARE:
        case PARAM_SKIP_BACK:
        case PARAM_NUMBER_8_16:          // Used only for "CNST"
        case PARAM_VARIABLE:
        case PARAM_SHUFFLE: {            // Used only for "<>"
          tamEnterMode(func);

          uint8_t maxDigits = tam.max < 10 ? 1 : (tam.max < 100 ? 2 : (tam.max < 1000 ? 3 : (tam.max < 10000 ? 4 : 5)));

          if((opParam == INDIRECT_REGISTER) && (func != ITM_BESTF_NO_IND))  {
            tam.indirect = true;
            tam.max = 99;
            maxDigits = 2;
            opParam = opParam2;
            popSoftmenu();
            showSoftmenu(-MNU_TAM);
            --numberOfTamMenusToPop;
          }
          else if((opParam == INDIRECT_VARIABLE) && (func != ITM_BESTF_NO_IND))   {
            tam.indirect = true;
            opParam = STRING_LABEL_VARIABLE;
            popSoftmenu();
            showSoftmenu(-MNU_TAM);
            --numberOfTamMenusToPop;
          }

          if ((paramMode == PARAM_FLAG) && opParam == SYSTEM_FLAG_NUMBER) {                 // System flag
            tam.digitsSoFar = 0;
            tam.value = 0;
          }
          else if(opParam == STRING_LABEL_VARIABLE) {      // Variable name
            tam.digitsSoFar = 0;
            tam.value = 0;
          }
          else if ((paramMode == PARAM_COMPARE) && ((opParam == VALUE_0) ||(opParam == VALUE_1)))  {  // Comparison to 0 or 1
            tam.digitsSoFar = 0;
            tam.value = 0;
          }
          else if((paramMode == PARAM_FLAG) && opParam > LAST_GLOBAL_FLAG) {                // Local flag
            tam.dot = true;
            tam.digitsSoFar = maxDigits - 1;
            tam.value = (opParam - FIRST_LOCAL_FLAG) / 10;
          }
          else if(((paramMode == PARAM_REGISTER) || (paramMode == PARAM_COMPARE) || tam.indirect) && opParam > LAST_GLOBAL_REGISTER) {    // Local register
            tam.dot = true;
            tam.digitsSoFar = maxDigits - 1;
            tam.value = (opParam - FIRST_LOCAL_REGISTER) / 10;
          }
          else if(((paramMode == PARAM_REGISTER) || (paramMode == PARAM_FLAG) || (paramMode == PARAM_COMPARE)|| tam.indirect) && opParam >= REGISTER_X) {    // Lettered flag or register from X to K
            tam.digitsSoFar = 0;
            tam.value = 0;
          }
          else if(((paramMode == PARAM_DECLARE_LABEL) ||(paramMode == PARAM_LABEL)) && opParam >= 100) {    // Local label from A to E or Label name
            tam.digitsSoFar = 0;
            tam.value = 0;
          }
           else if((paramMode == PARAM_NUMBER_16) && !tam.indirect) {     // BestF parameter
            tam.digitsSoFar =  maxDigits - 1;
            if(func == ITM_BESTF_NO_IND) {  // original BestF without indirection support (little endian parameter)
              tam.value = ((opParam2 << 8) + opParam) / 10;
            }
            else {                        // new Bestf with indirection support (big endian parameter)
              tam.value = ((opParam << 8) + opParam2) / 10;
            }
            //tam.value = (opParam & 0X3F) + 0X1500;     // remove last shuffled register
          }
          else if(paramMode == PARAM_SHUFFLE) {       // Stack registers shuffle
            tam.digitsSoFar = 3;
            tam.value = (opParam & 0X3F) + 0X1500;    // remove last shuffled register
          }
          else if ((paramMode == PARAM_NUMBER_8_16) && opParam == CNST_BEYOND_250) {         // Constant from 250 to 499
            tam.digitsSoFar = maxDigits - 1;
            tam.value = (opParam2 / 10) + 25;
          }
          else {                                    // Number, numbered register 0-99, local label 0-99
            tam.digitsSoFar =  maxDigits - 1;
            tam.value = opParam / 10;
          }
          deleteStepsFromTo(currentStep, findNextStep(currentStep));
          scrollPemBackwards();
          tamProcessInput(func);
          break;
        }


        case PARAM_KEYG_KEYX: {                            // Key Goto or Key eXecute
          func = (opParam2 == ITM_GTO ? ITM_KEYG : ITM_KEYX);
          deleteStepsFromTo(currentStep, findNextStep(currentStep));
          runFunction(func);
          tamProcessInput(ITM_0 + opParam/10);
          tamProcessInput(ITM_0 + (opParam % 10));
          if((opParam3 == INDIRECT_REGISTER) || (opParam3 == INDIRECT_VARIABLE)) {
            tamProcessInput(ITM_INDIRECTION);
          }
          scrollPemBackwards();
          break;
        }

        default: {
          currentLocalStepNumber++;
          currentStep = findNextStep(currentStep);
        }
      }
    }
  }
#endif // !TESTSUITE_BUILD
}
