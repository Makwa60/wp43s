// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "ui/tam.h"

#include "apps/bugScreen.h"
#include "bufferize.h"
#include "calcMode.h"
#include "charString.h"
#include "constantPointers.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "hal/gui.h"
#include "items.h"
#include "mathematics/fractionalPart.h"
#include "mathematics/integerPart.h"
#include "mathematics/matrix.h"
#include "programming/lblGtoXeq.h"
#include "programming/manage.h"
#include "programming/nextStep.h"
#include "registers.h"
#include "screen.h"
#include "softmenus.h"
#include "sort.h"
#include <stdbool.h>
#include <string.h>

#include "wp43.h"

tamState_t tam;

void tamReset(void) {
  tam.mode = 0;
  tam.alpha = false;
}

#if !defined(TESTSUITE_BUILD)
  int16_t tamOperation(void) {
    switch(tam.function) {
      case ITM_STO: {
        switch(tam.currentOperation) {
          case ITM_ADD: {
            return ITM_STOADD;
          }
          case ITM_SUB: {
            return ITM_STOSUB;
          }
          case ITM_MULT: {
            return ITM_STOMULT;
          }
          case ITM_DIV: {
            return ITM_STODIV;
          }
          case ITM_Max: {
            return ITM_STOMAX;
          }
          case ITM_Min: {
            return ITM_STOMIN;
          }
          case ITM_Config: {
            return ITM_STOCFG;
          }
          case ITM_Stack: {
            return ITM_STOS;
          }
          case ITM_dddEL: {
            return ITM_STOEL;
          }
          case ITM_dddIJ: {
           return ITM_STOIJ;
          }
          default: {
            return ITM_STO;
          }
        }
      }

      case ITM_RCL: {
        switch(tam.currentOperation) {
          case ITM_ADD: {
            return ITM_RCLADD;
          }
          case ITM_SUB: {
            return ITM_RCLSUB;
          }
          case ITM_MULT: {
            return ITM_RCLMULT;
          }
          case ITM_DIV: {
            return ITM_RCLDIV;
          }
          case ITM_Max: {
            return ITM_RCLMAX;
          }
          case ITM_Min: {
            return ITM_RCLMIN;
          }
          case ITM_Config: {
            return ITM_RCLCFG;
          }
          case ITM_Stack: {
            return ITM_RCLS;
          }
          case ITM_dddEL: {
            return ITM_RCLEL;
          }
          case ITM_dddIJ: {
            return ITM_RCLIJ;
          }
          default: {
            return ITM_RCL;
          }
        }
      }

      case ITM_DELITM: {
        switch(-softmenu[softmenuStack[0].softmenuId].menuItem) {
          case MNU_RAM:
          case MNU_FLASH: {
            return ITM_DELITM_PROG;
          }
          case MNU_MENUS: {
            return ITM_DELITM_MENU;
          }
          default: {
            return ITM_DELITM;
          }
        }
      }

      default: {
        return tam.function;
      }
    }
  }



  static uint8_t _tamMaxDigits(int16_t max) {
    if(tam.function == ITM_GTOP) {
      return (max < 1000 ? 3 : (max < 10000 ? 4 : 5));
    }
    else {
      return (max < 10 ? 1 : (max < 100 ? 2 : (max < 1000 ? 3 : (max < 10000 ? 4 : 5))));
    }
  }



  static void _tamUpdateBuffer() {
    char regists[5];
    char *tbPtr = tamBuffer;
    if(tam.mode == 0) {
      return;
    }

    if(tam.mode == tmKey) {
      tbPtr = stpcpy(tbPtr, "KEY ");
      if(tam.keyInputFinished) {
        if(tam.keyIndirect) {
          tbPtr = stpcpy(tbPtr, STD_RIGHT_ARROW);
        }
        if(tam.keyDot) {
          tbPtr = stpcpy(tbPtr, ".");
        }
        if(tam.keyAlpha) {
          tbPtr = stpcpy(tbPtr, STD_LEFT_SINGLE_QUOTE);
          tbPtr = stpcpy(tbPtr, aimBuffer + AIM_BUFFER_LENGTH / 2);
          tbPtr = stpcpy(tbPtr, STD_RIGHT_SINGLE_QUOTE);
        }
        else {
          int16_t v = tam.key;
          for(int i = 1; i >= 0; i--) {
            tbPtr[i] = '0' + (v % 10);
            v /= 10;
          }
          tbPtr += 2;
        }
        if(tam.function == ITM_KEYX) {
          tbPtr = stpcpy(tbPtr, " XEQ ");
        }
        else {
          tbPtr = stpcpy(tbPtr, " GTO ");
        }
      }
    }
    else {
      tbPtr = stpcpy(tbPtr, indexOfItems[tamOperation()].itemCatalogName);
      tbPtr = stpcpy(tbPtr, " ");
    }

    if(tam.mode == tmShuffle) {
      // Shuffle keeps the source register number for each destination register (X, Y, Z, T) in two bits
      // consecutively, with the 'valid' bit eight above that number
      // E.g. 0000010100001110 would mean that two registers have been entered: T, Z in that order
      regists[4] = 0;
      for(int i=0;i<4;i++) {
        if((tam.value >> (i*2 + 8)) & 1) {
          uint8_t regNum = (tam.value >> (i*2)) & 3;
          regists[i] = (regNum == 3 ? 't' : 'x' + regNum);
        }
        else {
          regists[i] = '_';
        }
      }
      tbPtr = stpcpy(tbPtr, regists);
    }
    else {
      if(tam.indirect) {
        tbPtr = stpcpy(tbPtr, STD_RIGHT_ARROW);
      }
      if(tam.dot) {
        tbPtr = stpcpy(tbPtr, ".");
      }
      if(tam.alpha) {
        tbPtr = stpcpy(tbPtr, STD_LEFT_SINGLE_QUOTE);
        if(aimBuffer[0] == 0) {
          tbPtr = stpcpy(tbPtr, "_");
        }
        else {
          tbPtr = stpcpy(tbPtr, aimBuffer);
          tbPtr = stpcpy(tbPtr, STD_RIGHT_SINGLE_QUOTE);
        }
      }
      else {
        int16_t max = (tam.indirect ? (tam.dot ? (calcMode == cmPem ? 98 : currentNumberOfLocalRegisters) : 99)
          : (tam.dot ? (calcMode == cmPem ? 98 : ((tam.mode == tmFlagR || tam.mode == tmFlagW) ? NUMBER_OF_LOCAL_FLAGS : currentNumberOfLocalRegisters)) : tam.max));
        uint8_t maxDigits = _tamMaxDigits(max);
        uint8_t underscores = maxDigits - tam.digitsSoFar;
        int16_t v = tam.value;
        for(int i = tam.digitsSoFar - 1; i >= 0; i--) {
          tbPtr[i] = '0' + (v % 10);
          v /= 10;
        }
        tbPtr += tam.digitsSoFar;
        for(int i = 0; i < underscores; i++) {
          tbPtr[0] = '_';
          tbPtr++;
        }
      }
    }

    tbPtr[0] = 0;
  }



  static void _tamHandleShuffle(uint16_t item) {
    // Shuffle keeps the source register number for each destination register (X, Y, Z, T) in two bits
    // consecutively, with the 'valid' bit eight above that number
    // E.g. 0000010100001110 would mean that two registers have been entered: T, Z in that order
    switch(item) {
      case ITM_REG_X:
      case ITM_REG_Y:
      case ITM_REG_Z:
      case ITM_REG_T: {
        for(int i=0; i<4; i++) {
          if(!((tam.value >> (2*i + 8)) & 1)) {
            uint16_t mask = 3 << (2*i);
            tam.value |= 1 << (2*i + 8);
            tam.value = (tam.value & ~mask) | (((item-ITM_REG_X) << (2*i)) & mask);
            if(i == 3) {
              if(calcMode == cmPem) {
                addStepInProgram(tamOperation());
              }
              else {
                reallyRunFunction(tamOperation(), tam.value);
              }
              tamLeaveMode();
            }
            break;
          }
        }
        break;
      }
      case ITM_BACKSPACE: {
        // We won't have all four registers at this point as otherwise TAM would already be closed
        for(int i=3; i>=0; i--) {
          if((tam.value >> (2*i + 8)) & 1) {
            tam.value &= ~(1 << (2*i + 8));
            break;
          }
          else if(i == 0) {
            tamLeaveMode();
            scrollPemBackwards();
            break;
          }
        }
        break;
      }
    }
  }



  static void _tamProcessInput(uint16_t item) {
    int16_t  min, max, min2, max2;
    bool     forceTry = false, tryOoR = false;
    bool     valueParameter = (tam.function == ITM_GTOP || tam.function == ITM_BESTF || tam.function == ITM_SKIP || tam.function == ITM_BACK);
    char    *forcedVar = NULL;

    // Shuffle is handled completely differently to everything else
    if(tam.mode == tmShuffle) {
      _tamHandleShuffle(item);
      return;
    }

    min = (tam.dot ? 0 : tam.min);
    max = (tam.dot ? (calcMode == cmPem ? 98 : ((tam.mode == tmFlagR || tam.mode == tmFlagW) ? NUMBER_OF_LOCAL_FLAGS : currentNumberOfLocalRegisters)) : tam.max);
    min2 = (tam.indirect ? 0 : min);
    max2 = (tam.indirect ? (tam.dot ? (calcMode == cmPem ? 98 : currentNumberOfLocalRegisters) : 99) : max);
    if(item == ITM_ENTER || (tam.alpha && stringGlyphLength(aimBuffer) > 6)) {
      forceTry = true;
    }
    else if(item == ITM_BACKSPACE) {
      if(tam.alpha) {
        if(stringByteLength(aimBuffer) != 0) {
          // Delete the last character
          int16_t lg = stringLastGlyph(aimBuffer);
          aimBuffer[lg] = 0;
        }
        else if(tam.mode == tmNewMenu) {
          tamLeaveMode();
          runFunction(ITM_ASSIGN);
        }
        else {
          // backspaces within AIM are handled by addItemToBuffer, so this is if the aimBuffer is already empty
          tam.alpha = false;
          clearSystemFlag(FLAG_ALPHA);
          calcModeUpdateGui();
        }
      }
      else if(tam.digitsSoFar > 0) {
        if(tam.function == ITM_GTOP && tam.digitsSoFar == 3) {
          max2 = tam.max = max(getNumberOfSteps(), 99);
        }
        if(--tam.digitsSoFar != 0) {
          tam.value /= 10;
        }
        else {
          tam.value = 0;
        }
      }
      else if(tam.function == ITM_GTOP) {
        tam.function = ITM_GTO;
        tam.min = indexOfItems[ITM_GTO].tamMinMax >> TAM_MAX_BITS;
        tam.max = indexOfItems[ITM_GTO].tamMinMax & TAM_MAX_MASK;
      }
      else if(tam.dot) {
        tam.dot = false;
      }
      else if(tam.indirect) {
        tam.indirect = false;
        if(tam.mode == tmFlagR || tam.mode == tmFlagW) {
          popSoftmenu();
          showSoftmenu(-MNU_TAMFLAG);
          --numberOfTamMenusToPop;
        }
        else if(tam.mode == tmLabel || (tam.mode == tmKey && tam.keyInputFinished)) {
          popSoftmenu();
          showSoftmenu(-MNU_TAMLABEL);
          --numberOfTamMenusToPop;
        }
      }
      else if(tam.currentOperation != tam.function) {
        tam.currentOperation = tam.function;
      }
      else if(tam.mode == tmKey && tam.keyInputFinished) {
        tam.value            = tam.key / 10;
        tam.alpha            = tam.keyAlpha;
        tam.dot              = tam.keyDot;
        tam.indirect         = tam.keyIndirect;
        tam.keyInputFinished = false;
        xcopy(aimBuffer, aimBuffer + AIM_BUFFER_LENGTH / 2, 16);
        aimBuffer[0]    = 0;
        tam.key         = 0;
        tam.keyAlpha    = false;
        tam.keyDot      = false;
        tam.keyIndirect = false;
        tam.max         = 21;
        tam.min         = 1;
        tam.digitsSoFar = 1;
        popSoftmenu();
        showSoftmenu(-MNU_TAM);
        --numberOfTamMenusToPop;
        if(tam.alpha) {
          setSystemFlag(FLAG_ALPHA);
          calcModeEnter(cmAim);
        }
        calcModeUpdateGui();
      }
      else {
        tamLeaveMode();
        scrollPemBackwards();
      }
      return;
    }
    else if(item == MNU_DYNAMIC) {
      forcedVar = dynmenuGetLabel(dynamicMenuItem);
      if(forcedVar[0] == 0) {
        forcedVar = NULL;
      }
      forceTry = true;
    }
    else if(tam.alpha) {
      // Do nothing if it wasn't enter or backspace as the text input is handled elsewhere
      return;
    }
    else if(item == ITM_alpha) {
      bool allowAlphaMode = false, beginWithLowercase = false;
      allowAlphaMode = allowAlphaMode || (!tam.digitsSoFar && !tam.dot && !valueParameter && (tam.mode == tmStoRcl || tam.mode == tmMDim || tam.mode == tmRegister || tam.mode == tmCmp || tam.function == ITM_MVAR));
      allowAlphaMode = allowAlphaMode || (!tam.digitsSoFar && !tam.dot && tam.indirect);
      beginWithLowercase = allowAlphaMode;
      allowAlphaMode = allowAlphaMode || (!tam.digitsSoFar && !tam.dot && tam.mode == tmLabel);
      allowAlphaMode = allowAlphaMode || (!tam.digitsSoFar && !tam.dot && tam.keyInputFinished && tam.mode == tmKey);
      allowAlphaMode = allowAlphaMode || (!tam.digitsSoFar && (tam.function == ITM_LBL || tam.function == ITM_GTOP));
      if(allowAlphaMode) {
        tam.alpha = true;
        setSystemFlag(FLAG_ALPHA);
        aimBuffer[0] = 0;
        calcModeEnter(cmAim);
        if(beginWithLowercase) {
          alphaCase = AC_LOWER;
        }
      }
      return;
    }
    else if(item==ITM_Max || item==ITM_Min || item==ITM_ADD || item==ITM_SUB || item==ITM_MULT || item==ITM_DIV || item==ITM_Config || item==ITM_Stack || item==ITM_dddEL || item==ITM_dddIJ) { // Operation
      if(!tam.digitsSoFar && !tam.indirect) {
        if(tam.function == ITM_GTO) {
          if(item == ITM_Max) { // UP
            if(currentLocalStepNumber == 1) { // We are on 1st step of current program
              if(currentProgramNumber == 1) { // It's the 1st program in memory
                return;
              }
              else { // It isn't the 1st program in memory
                tam.value = programList[currentProgramNumber - 2].step;
              }
            }
            else { // We aren't on 1st step of current program
              tam.value = programList[currentProgramNumber - 1].step;
            }
            reallyRunFunction(ITM_GTOP, tam.value);
            pemCursorIsZerothStep = true;
            tamLeaveMode();
            hourGlassIconEnabled = false;
            return;
          }

          if(item == ITM_Min) { // DOWN
            if(currentProgramNumber == numberOfPrograms - 1) { // We are in the last program in memory
              return;
            }

            tam.value = programList[currentProgramNumber].step;
            reallyRunFunction(ITM_GTOP, tam.value);
            pemCursorIsZerothStep = true;
            tamLeaveMode();
            hourGlassIconEnabled = false;
            return;
          }
        }
        else if(tam.mode == tmStoRcl && tam.currentOperation != ITM_Config && tam.currentOperation != ITM_Stack) {
          if(item == tam.currentOperation) {
            tam.currentOperation = tam.function;
          }
          else {
            tam.currentOperation = item;
            if(item == ITM_dddEL || item == ITM_dddIJ) {
              switch(calcMode) {
                case cmMim: {
                  mimRunFunction(tamOperation(), NOPARAM);
                  break;
                }
                case cmPem: {
                  addStepInProgram(tamOperation());
                  break;
                }
                default: {
                  reallyRunFunction(tamOperation(), NOPARAM);
                }
              }
              if(tam.mode) {
                tamLeaveMode();
              }
              hourGlassIconEnabled = false;
              return;
            }
          }
        }
      }
      return;
    }
    else if(tam.function == ITM_toINT && item == ITM_REG_I) {
      if(calcMode == cmPem) {
        addStepInProgram(ITM_IP);
      }
      else {
        fnIp(NOPARAM);
      }
      tamLeaveMode();
      return;
    }
    else if(tam.function == ITM_toINT && item == ITM_alpha) {
      if(calcMode == cmPem) {
        addStepInProgram(ITM_FP);
      }
      else {
        fnFp(NOPARAM);
      }
      tamLeaveMode();
      return;
    }
    else if(tam.function == ITM_toINT && item == ITM_REG_D) {
      tam.value = 10;
      forceTry = true;
    }
    else if(tam.function == ITM_toINT && item == ITM_REG_B) {
      tam.value = 2;
      forceTry = true;
    }
    else if(tam.function == ITM_toINT && item == ITM_HEX) {
      tam.value = 16;
      forceTry = true;
    }
    else if((tam.mode == tmLabel || (tam.mode == tmKey && tam.keyInputFinished)) && !tam.indirect && item == ITM_E) {
      tam.value = 100 - 'A' + 'E';
      forceTry = true;
      tryOoR = true;
    }
    else if(REGISTER_X <= indexOfItems[item].param && indexOfItems[item].param <= REGISTER_K && !tam.dot) {
      if(!tam.digitsSoFar && tam.function != ITM_BESTF && (tam.indirect || (tam.mode != tmValue && tam.mode != tmValueChb))) {
        if((tam.mode == tmLabel || (tam.mode == tmKey && tam.keyInputFinished)) && !tam.indirect) {
          switch(indexOfItems[item].param) {
            case REGISTER_A: {
              tam.value = 100 - 'A' + 'A'; forceTry = true; tryOoR = true;
              break;
            }
            case REGISTER_B: {
              tam.value = 100 - 'A' + 'B'; forceTry = true; tryOoR = true;
              break;
            }
            case REGISTER_C: {
              tam.value = 100 - 'A' + 'C'; forceTry = true; tryOoR = true;
              break;
            }
            case REGISTER_D: {
              tam.value = 100 - 'A' + 'D'; forceTry = true; tryOoR = true;
              break;
            }
            case REGISTER_X: {
              tam.alpha = true; aimBuffer[0] = 'X'; aimBuffer[1] = 0; forceTry = true;
              break;
            }
            case REGISTER_Y: {
              tam.alpha = true; aimBuffer[0] = 'Y'; aimBuffer[1] = 0; forceTry = true;
              break;
            }
            case REGISTER_Z: {
              tam.alpha = true; aimBuffer[0] = 'Z'; aimBuffer[1] = 0; forceTry = true;
              break;
            }
            case REGISTER_T: {
              tam.alpha = true; aimBuffer[0] = 'T'; aimBuffer[1] = 0; forceTry = true;
              break;
            }
            case REGISTER_L: {
              tam.alpha = true; aimBuffer[0] = 'L'; aimBuffer[1] = 0; forceTry = true;
              break;
            }
            case REGISTER_I: {
              tam.alpha = true; aimBuffer[0] = 'I'; aimBuffer[1] = 0; forceTry = true;
              break;
            }
            case REGISTER_J: {
              tam.alpha = true; aimBuffer[0] = 'J'; aimBuffer[1] = 0; forceTry = true;
              break;
            }
            case REGISTER_K: {
              tam.alpha = true; aimBuffer[0] = 'K'; aimBuffer[1] = 0; forceTry = true;
              break;
            }
          }
        }
        else {
          tam.value = indexOfItems[item].param;
          forceTry = true;
          // Register letters access registers not accessible via number codes, so we shouldn't look at the tam.max value
          // when determining if this is valid
          tryOoR = true;
        }
      }
    }
    else if(item == ITM_0P || item == ITM_1P) {
      reallocateRegister(TEMP_REGISTER_1, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
      real34Copy(item == ITM_1P ? const34_1 : const34_0, REGISTER_REAL34_DATA(TEMP_REGISTER_1));
      if(!tam.digitsSoFar && tam.function != ITM_BESTF && tam.function != ITM_CNST && tam.mode != tmValue && tam.mode != tmValueChb) {
        tam.value = TEMP_REGISTER_1;
        forceTry = true;
        // Register letters access registers not accessible via number codes, so we shouldn't look at the tam.max value
        // when determining if this is valid
        tryOoR = true;
      }
    }
    else if(ITM_0 <= item && item <= ITM_9) {
      int16_t digit = item - ITM_0;
      uint8_t maxDigits = _tamMaxDigits(max2);
      // If the number is below our minimum, prevent further entry of digits
      if(tam.function == ITM_GTOP && tam.digitsSoFar == 2) {
        max2 = tam.max = getNumberOfSteps();
        maxDigits = _tamMaxDigits(max2);
      }
      if(!tam.alpha && (tam.value*10 + digit) <= max2 && tam.digitsSoFar < maxDigits) {
        if(tam.digitsSoFar != maxDigits - 1 || (tam.value*10 + digit) >= min2) {
          tam.value = tam.value*10 + digit;
          tam.digitsSoFar++;
          if(tam.digitsSoFar == maxDigits) {
            forceTry = true;
          }
        }
      }
      else if(tam.function == ITM_GTOP) {
        max2 = tam.max = max(getNumberOfSteps(), 99);
        maxDigits = _tamMaxDigits(max2);
      }
    }
    else if(item == ITM_PERIOD) {
      if(tam.function == ITM_LBL) {
        return;
      }
      else if(tam.function == ITM_GTOP) {
        tam.value = programList[numberOfPrograms - numberOfProgramsInFlash - 1].step;
        pemCursorIsZerothStep = true;
        reallyRunFunction(ITM_GTOP, tam.value);
        if((*currentStep.ram != 0xff) || (*(currentStep.ram + 1) != 0xff)) {
          currentStep.ram = firstFreeProgramByte;
          insertStepInProgram(ITM_END);
          scanLabelsAndPrograms();
          tam.value = programList[numberOfPrograms - numberOfProgramsInFlash - 1].step;
          reallyRunFunction(ITM_GTOP, tam.value);
        }
        tamLeaveMode();
        hourGlassIconEnabled = false;
        return;
      }
      else if(!tam.alpha && !tam.digitsSoFar && !tam.dot && !valueParameter) {
        if(tam.function == ITM_GTO) {
          tam.function = ITM_GTOP;
          tam.min = 0;
          tam.max = max(getNumberOfSteps(), 99);
        }
        else if(tam.indirect && (currentNumberOfLocalRegisters || calcMode == cmPem)) {
          tam.dot = true;
        }
        else if(tam.mode != tmValue && tam.mode != tmValueChb) {
          if(calcMode == cmPem || ((tam.mode == tmFlagR || tam.mode == tmFlagW) && currentLocalFlags != NULL) || ((tam.mode != tmFlagR && tam.mode != tmFlagW) && currentNumberOfLocalRegisters)) {
            tam.dot = true;
          }
        }
      }
      return;
    }
    else if(item == ITM_INDIRECTION) {
      if(!tam.alpha && !tam.digitsSoFar && !tam.dot && !valueParameter && (indexOfItems[tam.function].status & PTP_STATUS) != PTP_SKIP_BACK && (indexOfItems[tam.function].status & PTP_STATUS) != PTP_DECLARE_LABEL) {
        if(!tam.indirect && (tam.mode == tmFlagR || tam.mode == tmFlagW || tam.mode == tmLabel)) {
          popSoftmenu();
          showSoftmenu(-MNU_TAM);
          --numberOfTamMenusToPop;
        }
        tam.indirect = true;
      }
      return;
    }
    else if(indexOfItems[item].func == fnGetSystemFlag && (tam.mode == tmFlagR || tam.mode == tmFlagW)) {
      // A function key has been pressed that corresponds to a system flag
      tam.value = indexOfItems[item].param;
      tryOoR = true;
      forceTry = true;
    }
    else {
      // Do nothing
      return;
    }

    // All operations that may try and evaluate the function shouldn't return but let execution fall through to here

    if(tam.mode == tmKey && !tam.keyInputFinished) {
      if(tam.alpha || forcedVar || ((tryOoR || (min2 <= tam.value && tam.value <= max2)) && (forceTry || tam.value*10 > max2))) {
        tam.key              = tam.value;
        tam.keyAlpha         = tam.alpha;
        tam.keyDot           = tam.dot;
        tam.keyIndirect      = tam.indirect;
        tam.keyInputFinished = true;
        xcopy(aimBuffer + AIM_BUFFER_LENGTH / 2, aimBuffer, 16);
        aimBuffer[0]    = 0;
        tam.value       = 0;
        tam.alpha       = false;
        tam.dot         = false;
        tam.indirect    = false;
        tam.max         = 99;
        tam.min         = 0;
        tam.digitsSoFar = 0;
        popSoftmenu();
        showSoftmenu(-MNU_TAMLABEL);
        --numberOfTamMenusToPop;
        clearSystemFlag(FLAG_ALPHA);
        calcModeUpdateGui();
      }
      else if(tam.digitsSoFar == 2 && tam.value == 0) {
        tam.digitsSoFar = 1;
      }
    }
    else if(!tam.alpha && !forcedVar) {
      // Check whether it is possible to add any more digits: if not, execute the function
      if((tryOoR || (min2 <= tam.value && tam.value <= max2)) && (forceTry || tam.value*10 > max2)) {
        int16_t value = tam.value;
        bool    run = true;
        if(tam.dot) {
          value += FIRST_LOCAL_REGISTER;
        }
        if(tam.indirect && calcMode != cmPem) {
          value = indirectAddressing(value, (indexOfItems[tamOperation()].param == tmFlagR || indexOfItems[tamOperation()].param == tmFlagW) ? INDPM_FLAG : (tam.mode == tmStoRcl || tam.mode == tmMDim) ? INDPM_REGISTER : INDPM_PARAM, min, max);
          run = (value != FAILED_INDIRECTION);
        }
        if(tam.function == ITM_GTOP) {
          if(tam.digitsSoFar < 3) {
            pemCursorIsZerothStep = false;
            fnGoto(value);
          }
          else {
            pemCursorIsZerothStep = (value == 0);
            if(value == 0) {
              value = 1;
            }
            goToPgmStep(currentProgramNumber, value);
          }
        }
        else if(run) {
          switch(calcMode) {
            case cmMim: {
              mimRunFunction(tamOperation(), value);
              break;
            }
            case cmPem: {
              addStepInProgram(tamOperation());
              break;
            }
            default: {
              reallyRunFunction(tamOperation(), value);
            }
          }
        }
        if(tamOperation() == ITM_M_GOTO_ROW) {
          tamLeaveMode();
          tamEnterMode(ITM_M_GOTO_COLUMN);
        }
        else {
          if(tam.mode) {
            tamLeaveMode();
          }
        }
      }
    }
    else {
      char    *buffer = (forcedVar ? forcedVar : aimBuffer);
      bool     tryAllocate = ((tam.function == ITM_STO || tam.function == ITM_M_DIM || tam.function == ITM_MVAR || tam.function == ITM_INPUT) && !tam.indirect);
      int16_t  value;
      if(tam.mode == tmNewMenu) {
        value = 1;
      }
      else if(tam.function == ITM_XEQ) {
        value = findNamedLabel(buffer);
        if(value == INVALID_VARIABLE) {
          for(int i = 0; i < LAST_ITEM; ++i) {
            if((indexOfItems[i].status & CAT_STATUS) == CAT_FNCT && compareString(buffer, indexOfItems[i].itemCatalogName, CMP_NAME) == 0) {
              if(tam.mode) {
                tamLeaveMode();
              }
              if(calcMode == cmPem) {
                aimBuffer[0] = 0;
                if(!programListEnd) {
                  scrollPemBackwards();
                }
              }
              runFunction(i);
              return;
            }
          }
          if(calcMode != cmPem) {
            if(tam.mode) {
              tamLeaveMode();
            }
            displayCalcErrorMessage(ERROR_FUNCTION_NOT_FOUND, ERR_REGISTER_LINE, REGISTER_X);
            errorMoreInfo("string '%s' is neither a named label nor a function name", buffer);
            return;
          }
        }
      }
      else if(tam.mode == tmLabel || tam.mode == tmSolve || (tam.mode == tmKey && tam.keyInputFinished) || (tam.mode == tmDelItem && (softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_RAM || softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_FLASH))) {
        value = findNamedLabel(buffer);
        if(value == INVALID_VARIABLE && tam.function != ITM_LBL && tam.function != ITM_LBLQ) {
          if(calcMode != cmPem && getSystemFlag(FLAG_IGN1ER)) {
            clearSystemFlag(FLAG_IGN1ER);
            errorMoreInfo("string '%s' is not a named label\nignored since IGN1ER was set", buffer);
          }
          else {
            displayCalcErrorMessage(ERROR_LABEL_NOT_FOUND, ERR_REGISTER_LINE, REGISTER_X);
            errorMoreInfo("string '%s' is not a named label", buffer);
          }
        }
      }
      else if(tam.mode == tmDelItem && softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_MENUS) {
        value = tam.value;
      }
      else if(tryAllocate) {
        value = findOrAllocateNamedVariable(buffer);
      }
      else {
        value = findNamedVariable(buffer);
        if(value == INVALID_VARIABLE) {
          if(calcMode != cmPem && getSystemFlag(FLAG_IGN1ER)) {
            clearSystemFlag(FLAG_IGN1ER);
            errorMoreInfo("string '%s' is not a named variable\nignored since IGN1ER was set", buffer);
          }
          else {
            displayCalcErrorMessage(ERROR_UNDEF_SOURCE_VAR, ERR_REGISTER_LINE, REGISTER_X);
            errorMoreInfo("string '%s' is not a named variable", buffer);
          }
        }
      }
      if(calcMode == cmPem
      #if CLP_WITH_MENU != 0
        && tam.function != ITM_CLP
      #endif /* CLP_WITH_MENU != 0 */
      ) {
        addStepInProgram(tamOperation());
      }
      if(tam.mode != tmNewMenu) {
        aimBuffer[0] = 0;
      }
      if(tam.indirect && value != INVALID_VARIABLE && calcMode != cmPem) {
        value = indirectAddressing(value, (indexOfItems[tam.function].param == tmFlagR || indexOfItems[tam.function].param == tmFlagW) ? INDPM_FLAG : (tam.mode == tmStoRcl || tam.mode == tmMDim) ? INDPM_REGISTER : INDPM_PARAM, min, max);
        if(value == FAILED_INDIRECTION) {
          value = INVALID_VARIABLE;
        }
      }
      if(value != INVALID_VARIABLE || tamOperation() == ITM_LBLQ) {
        if(calcMode == cmMim) {
          mimRunFunction(tamOperation(), value);
        }
        else if(tam.function == ITM_GTOP) {
          goToGlobalStep(labelList[value - FIRST_LABEL].step);
        }
        #if CLP_WITH_MENU != 0
          else if(tam.function == ITM_CLP) {
            reallyRunFunction(ITM_CLP, value);
          }
        #endif /* CLP_WITH_MENU != 0 */
        else if(calcMode == cmPem) {
          // already done
        }
        else {
          reallyRunFunction(tamOperation(), value);
        }
      }
      if(tamOperation() == ITM_M_GOTO_ROW) {
        tamLeaveMode();
        tamEnterMode(ITM_M_GOTO_COLUMN);
      }
      else {
        if(tam.mode) {
          tamLeaveMode();
        }
      }
    }
  }



  void tamEnterMode(int16_t func) {
    tam.mode = func == ITM_ASSIGN ? tmNewMenu : indexOfItems[func].param;
    tam.function = func;
    tam.min = indexOfItems[func].tamMinMax >> TAM_MAX_BITS;
    tam.max = indexOfItems[func].tamMinMax & TAM_MAX_MASK;

    screenUpdatingMode = SCRUPD_AUTO;

    if(tam.max == 16383) { // Only case featuring more than TAM_MAX_BITS bits is GTO.
      tam.max = 32766;
    }

    if(func == ITM_CNST) {
      tam.max = NUMBER_OF_CONSTANTS_39 + NUMBER_OF_CONSTANTS_51 + NUMBER_OF_CONSTANTS_1071 + NUMBER_OF_CONSTANTS_34 - 1;
    }

    if(calcMode == cmNim) {
      closeNim();
    }
    else if(calcMode == cmPem && aimBuffer[0] != 0) {
      if(getSystemFlag(FLAG_ALPHA)) {
        pemCloseAlphaInput();
      }
      else {
        pemCloseNumberInput();
      }
      aimBuffer[0] = 0;
      --currentLocalStepNumber;
      currentStep = findPreviousStep(currentStep);
    }
    else if(calcMode == cmPem) {
      scrollPemForwards();
    }

    if(func == ITM_ASSIGN) {
      aimBuffer[0] = 0;
    }

    tam.alpha = (func == ITM_ASSIGN);
    tam.currentOperation = tam.function;
    tam.digitsSoFar = 0;
    tam.dot = false;
    tam.indirect = false;
    tam.value = 0;

    tam.key = 0;
    tam.keyAlpha = false;
    tam.keyDot = false;
    tam.keyIndirect = false;
    tam.keyInputFinished = false;

    switch(tam.mode) {
      case tmValue:
      case tmValueChb:
      case tmRegister:
      case tmMDim:
      case tmKey: {
        showSoftmenu(-MNU_TAM);
        break;
      }

      case tmCmp: {
        showSoftmenu(-MNU_TAMCMP);
        break;
      }

      case tmFlagR:
      case tmFlagW: {
        showSoftmenu(-MNU_TAMFLAG);
        break;
      }

      case tmStoRcl: {
        showSoftmenu(-MNU_TAMSTORCL);
        break;
      }

      case tmShuffle: {
        showSoftmenu(-MNU_TAMSHUFFLE);
        break;
      }

      case tmLabel: {
        showSoftmenu(-MNU_TAMLABEL);
        break;
      }

      case tmSolve: {
        if(func == ITM_SOLVE && calcMode == cmPem) {
          showSoftmenu(-MNU_TAM);
        }
        else {
          showSoftmenu(-MNU_TAMLABEL);
        }
        break;
      }

      case tmNewMenu: {
        break;
      }

      case tmDelItem: {
        showSoftmenu(-ITM_DELITM);
        break;
      }

      default: {
        sprintf(errorMessage, "In function calcModeTam: %" PRIu16 " is an unexpected value for tam.mode!", (uint16_t)tam.mode);
        bugScreen(errorMessage);
        return;
      }
    }

    numberOfTamMenusToPop = func == ITM_ASSIGN ? 0 : 1;

    _tamUpdateBuffer();

    clearSystemFlag(FLAG_ALPHA);

    #if defined(PC_BUILD) && (SCREEN_800X480 == 0)
      if(tam.mode == tmNewMenu) {
        setSystemFlag(FLAG_ALPHA);
        aimBuffer[0] = 0;
        calcModeEnter(cmAim);
      }
      else {
        calcModeUpdateGui();
      }
    #endif // PC_BUILD && (SCREEN_800X480 == 0)
  }



  void tamLeaveMode(void) {
    if(screenUpdatingMode & (SCRUPD_MANUAL_STACK | SCRUPD_SKIP_STACK_ONE_TIME)) {
      clearTamBuffer();
    }

    tam.alpha = false;
    tam.mode = 0;
    catalog = CATALOG_NONE;
    clearSystemFlag(FLAG_ALPHA);

    while(numberOfTamMenusToPop--) {
      popSoftmenu();
    }

    calcModeUpdateGui();

    if(calcMode == cmPem) {
      hourGlassIconEnabled = false;
    }
  }



  void tamProcessInput(uint16_t item) {
    _tamProcessInput(item);
    _tamUpdateBuffer();
  }
#endif // !TESTSUITE_BUILD
