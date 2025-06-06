// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 and C47 Authors

/********************************************//**
 * \file ui/genericEditor.c
 ***********************************************/

#include "ui/genericEditor.h"

#include "calcMode.h"
#include "charString.h"
#include "defines.h"
#include "flags.h"
#include "items.h"
#include "registers.h"
#include "stack.h"
#include "tam.h"
#include "programming/manage.h"
#include "programming/nextStep.h"
#include "ui/cursor.h"
#include "ui/matrixEditor.h"
#include "ui/softmenus.h"
#include <string.h>

#include "wp43.h"

void fnEdit (uint16_t unusedParamButMandatory) {

#if !defined(TESTSUITE_BUILD)
  if(calcMode == cmNormal) {
    switch(getRegisterDataType(REGISTER_X)) {
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
    if((func == ITM_LITERAL || func == ITM_REM)) {
      pemAlpha(ITM_EDIT);
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

          if(opParam == INDIRECT_REGISTER)  {
            tam.indirect = true;
            tam.max = 99;
            maxDigits = 2;
            opParam = opParam2;
            popSoftmenu();
            showSoftmenu(-MNU_TAM);
            --numberOfTamMenusToPop;
          }
          else if(opParam == INDIRECT_VARIABLE)  {
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
           else if(paramMode == PARAM_SHUFFLE) {     // Stack registers shuffle
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
