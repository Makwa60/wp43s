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
#include "programming/manage.h"
#include "programming/nextstep.h"
#include "ui/cursor.h"
#include "ui/matrixEditor.h"
#include "ui/softmenus.h"
#include <string.h>

#include "wp43.h"

void fnEdit (uint16_t unusedParamButMandatory) {
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
    //printf("DDDDDa func[0] [1] = %i %i\n",func, currentStep[1]);
    if(func & 0x80) {
      func &= 0x7f;
      func <<= 8;
      func |= currentStep[1];
    }
    if((func == ITM_LITERAL || func == ITM_REM)) {
      pemAlpha(ITM_EDIT);
    }
    else {
      currentLocalStepNumber++;
      currentStep = findNextStep(currentStep);
    }
  }
}
