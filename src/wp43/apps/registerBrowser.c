// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "apps/registerBrowser.h"

#include "apps/apps.h"
#include "apps/bugScreen.h"
#include "calcMode.h"
#include "charString.h"
#include "debug.h"
#include "display.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "hal/lcd.h"
#include "items.h"
#include "recall.h"
#include "registers.h"
#include "ui/screen.h"
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "wp43.h"

bool    showContent;
uint8_t rbrMode;
int16_t currentRegisterBrowserScreen;

#if !defined(TESTSUITE_BUILD)
  void registerBrowserInit(void) {
    currentRegisterBrowserScreen = 9999;
  }



  static void _showRegisterInRbr(calcRegister_t regist, int16_t registerNameWidth) {
    switch(getRegisterDataType(regist)) {
      case dtReal34: {
        if(showContent) {
          real34ToDisplayString(REGISTER_REAL34_DATA(regist), getRegisterAngularMode(regist), tmpString, &standardFont, SCREEN_WIDTH - 1 - registerNameWidth, 34, false, STD_SPACE_4_PER_EM, false);
        }
        else {
          sprintf(tmpString, "%d bytes", (int16_t)TO_BYTES(TO_BLOCKS(REAL34_SIZE_IN_BYTES)));
        }
        break;
      }

      case dtComplex34: {
        if(showContent) {
          complex34ToDisplayString(REGISTER_COMPLEX34_DATA(regist), tmpString, &standardFont, SCREEN_WIDTH - 1 - registerNameWidth, 34, false, STD_SPACE_4_PER_EM, false);
        }
        else {
          sprintf(tmpString, "%d bytes", (int16_t)TO_BYTES(TO_BLOCKS(COMPLEX34_SIZE_IN_BYTES)));
        }
        break;
      }

      case dtLongInteger: {
        if(showContent) {
          if(regist >= FIRST_RESERVED_VARIABLE) {
            copySourceRegisterToDestRegister(regist, TEMP_REGISTER_1);
            longIntegerRegisterToDisplayString(TEMP_REGISTER_1, tmpString, TMP_STR_LENGTH, SCREEN_WIDTH - 1 - registerNameWidth, 50, STD_SPACE_4_PER_EM);
          }
          else if(getRegisterLongIntegerSign(regist) == liNegative) {
            longIntegerRegisterToDisplayString(regist, tmpString, TMP_STR_LENGTH, SCREEN_WIDTH - 1 - registerNameWidth, 50, STD_SPACE_4_PER_EM);
          }
          else {
            longIntegerRegisterToDisplayString(regist, tmpString, TMP_STR_LENGTH, SCREEN_WIDTH - 9 - registerNameWidth, 50, STD_SPACE_4_PER_EM);
          }
        }
        else {
          if(regist >= FIRST_RESERVED_VARIABLE) {
            sprintf(tmpString, "4 bytes");
          }
          else {
            sprintf(tmpString, "%" PRIu32 " bits " STD_CORRESPONDS_TO " 4+%" PRIu32 " bytes", (uint32_t)TO_BYTES(getRegisterMaxDataLength(regist)) * 8, (uint32_t)TO_BYTES(getRegisterMaxDataLength(regist)));
          }
        }
        break;
      }

      case dtShortInteger: {
        if(showContent) {
          shortIntegerToDisplayString(regist, tmpString, false);
        }
        else {
          strcpy(tmpString, "64 bits " STD_CORRESPONDS_TO " 8 bytes");
        }
        break;
      }

      case dtString: {
        if(showContent) {
          strcpy(tmpString, STD_LEFT_SINGLE_QUOTE);
          strncat(tmpString, REGISTER_STRING_DATA(regist), stringByteLength(REGISTER_STRING_DATA(regist)) + 1);
          strcat(tmpString, STD_RIGHT_SINGLE_QUOTE);
          if(stringWidth(tmpString, &standardFont, false, true) >= SCREEN_WIDTH - 12 - registerNameWidth) { // 12 is the width of STD_ELLIPSIS
            *(stringAfterPixels(tmpString, &standardFont, SCREEN_WIDTH - 12 - registerNameWidth, false, true)) = 0;
            strcat(tmpString + stringByteLength(tmpString), STD_ELLIPSIS);
          }
        }
        else {
          sprintf(tmpString, "%" PRIu32 " character%s " STD_CORRESPONDS_TO " 4+%" PRIu32 " bytes", (uint32_t)stringGlyphLength(REGISTER_STRING_DATA(regist)), stringGlyphLength(REGISTER_STRING_DATA(regist))==1 ? "" : "s", (uint32_t)TO_BYTES(getRegisterMaxDataLength(regist)));
        }
        break;
      }

      case dtTime: {
        if(showContent) {
          timeToDisplayString(regist, tmpString, true);
        }
        else {
          sprintf(tmpString, "%d bytes", (int16_t)TO_BYTES(TO_BLOCKS(REAL34_SIZE_IN_BYTES)));
        }
        break;
      }

      case dtDate: {
        if(showContent) {
          dateToDisplayString(regist, tmpString);
        }
        else {
          sprintf(tmpString, "%d bytes", (int16_t)TO_BYTES(TO_BLOCKS(REAL34_SIZE_IN_BYTES)));
        }
        break;
      }

      case dtReal34Matrix: {
        if(showContent) {
          real34MatrixToDisplayString(regist, tmpString);
        }
        else {
          dataBlock_t* dblock = REGISTER_REAL34_MATRIX_DBLOCK(regist);
          sprintf(tmpString, "%" PRIu16 " element%s " STD_CORRESPONDS_TO " 4+%" PRIu32 " bytes", (uint16_t)(dblock->matrixRows * dblock->matrixColumns), (dblock->matrixRows * dblock->matrixColumns)==1 ? "" : "s", (uint32_t)TO_BYTES(dblock->matrixRows * dblock->matrixColumns * TO_BLOCKS(REAL34_SIZE_IN_BYTES)));
        }
        break;
      }

      case dtComplex34Matrix: {
        if(showContent) {
          complex34MatrixToDisplayString(regist, tmpString);
        }
        else {
          dataBlock_t* dblock = REGISTER_COMPLEX34_MATRIX_DBLOCK(regist);
          sprintf(tmpString, "%" PRIu16 " element%s " STD_CORRESPONDS_TO " 4+%" PRIu32 " bytes", (uint16_t)(dblock->matrixRows * dblock->matrixColumns), (dblock->matrixRows * dblock->matrixColumns)==1 ? "" : "s", (uint32_t)TO_BYTES(dblock->matrixRows * dblock->matrixColumns * TO_BLOCKS(COMPLEX34_SIZE_IN_BYTES)));
        }
        break;
      }

      case dtConfig: {
        if(showContent) {
          strcpy(tmpString, "Configuration data");
        }
        else {
          sprintf(tmpString, "%d bytes", (int16_t)CONFIG_SIZE_IN_BYTES);
        }
        break;
      }

      default: {
        sprintf(tmpString, "Data type %s: to be coded", getDataTypeName(getRegisterDataType(regist), false, true));
      }
    }
  }



  static bool _registerBrowserKeyHandler(int16_t item) {
    if(ITM_0 <= item && item <= ITM_9) {
      if(rbr1stDigit) {
        rbr1stDigit = false;
        rbrRegister = item - ITM_0;
      }
      else {
        rbr1stDigit = true;
        rbrRegister = rbrRegister*10 + item - ITM_0;

        switch(rbrMode) {
          case RBR_GLOBAL: {
            currentRegisterBrowserScreen = rbrRegister;
            break;
          }
          case RBR_LOCAL: {
            rbrRegister = (rbrRegister >= currentNumberOfLocalRegisters ? 0 : rbrRegister);
            currentRegisterBrowserScreen = FIRST_LOCAL_REGISTER + rbrRegister;
            break;
          }
          case RBR_NAMED: {
            rbrMode = RBR_GLOBAL;
            currentRegisterBrowserScreen = rbrRegister;
            break;
          }
        }
      }
      return true;
    }
    switch(item) {
      case ITM_UP: {
        rbr1stDigit = true;
        if(rbrMode == RBR_GLOBAL) {
          currentRegisterBrowserScreen = modulo(currentRegisterBrowserScreen + 1, FIRST_LOCAL_REGISTER);
        }
        else if(rbrMode == RBR_LOCAL) {
          currentRegisterBrowserScreen = modulo(currentRegisterBrowserScreen - FIRST_LOCAL_REGISTER + 1, currentNumberOfLocalRegisters) + FIRST_LOCAL_REGISTER;
        }
        else if(rbrMode == RBR_NAMED) {
          currentRegisterBrowserScreen = modulo(currentRegisterBrowserScreen - FIRST_NAMED_VARIABLE + 1, numberOfNamedVariables + LAST_RESERVED_VARIABLE - FIRST_RESERVED_VARIABLE - 11) + FIRST_NAMED_VARIABLE;
        }
        else {
          sprintf(errorMessage, "In function fnKeyUp: unexpected case while processing key UP! %" PRIu8 " is an unexpected value for rbrMode.", rbrMode);
          bugScreen(errorMessage);
        }
        break;
      }
      case ITM_DOWN: {
        rbr1stDigit = true;
        if(rbrMode == RBR_GLOBAL) {
          currentRegisterBrowserScreen = modulo(currentRegisterBrowserScreen - 1, FIRST_LOCAL_REGISTER);
        }
        else if(rbrMode == RBR_LOCAL) {
          currentRegisterBrowserScreen = modulo(currentRegisterBrowserScreen - FIRST_LOCAL_REGISTER - 1, currentNumberOfLocalRegisters) + FIRST_LOCAL_REGISTER;
        }
        else if(rbrMode == RBR_NAMED) {
          currentRegisterBrowserScreen = modulo(currentRegisterBrowserScreen - FIRST_NAMED_VARIABLE - 1, numberOfNamedVariables + LAST_RESERVED_VARIABLE - FIRST_RESERVED_VARIABLE - 11) + FIRST_NAMED_VARIABLE;
        }
        else {
          sprintf(errorMessage, "In function fnKeyDown: unexpected case while processing key DOWN! %" PRIu8 " is an unexpected value for rbrMode.", rbrMode);
          bugScreen(errorMessage);
        }
        break;
      }
      case ITM_PERIOD: {
        rbr1stDigit = true;
        if(rbrMode == RBR_GLOBAL) {
          if(currentNumberOfLocalRegisters != 0) {
            rbrMode = RBR_LOCAL;
            currentRegisterBrowserScreen = FIRST_LOCAL_REGISTER;
          }
          else {
            rbrMode = RBR_NAMED;
            currentRegisterBrowserScreen = FIRST_NAMED_VARIABLE;
          }
        }
        else if(rbrMode == RBR_LOCAL) {
          rbrMode = RBR_NAMED;
          currentRegisterBrowserScreen = FIRST_NAMED_VARIABLE;
        }
        else if(rbrMode == RBR_NAMED) {
          rbrMode = RBR_GLOBAL;
          currentRegisterBrowserScreen = REGISTER_X;
        }
        break;
      }
      case ITM_RS: {
        rbr1stDigit = true;
        showContent = !showContent;
        break;
      }
      case ITM_RCL: {
        rbr1stDigit = true;
        if(rbrMode == RBR_GLOBAL || rbrMode == RBR_LOCAL) {
          fnRecall(currentRegisterBrowserScreen);
          setSystemFlag(FLAG_ASLIFT);
          calcModeLeave();
        }
        else if(rbrMode == RBR_NAMED) {
          if(currentRegisterBrowserScreen >= FIRST_NAMED_VARIABLE + numberOfNamedVariables) { // Reserved variables
            currentRegisterBrowserScreen -= FIRST_NAMED_VARIABLE + numberOfNamedVariables;
            currentRegisterBrowserScreen += FIRST_RESERVED_VARIABLE + 12;
          }
          fnRecall(currentRegisterBrowserScreen);
          setSystemFlag(FLAG_ASLIFT);
          calcModeLeave();
        }
        break;
      }
      default: {
        for(int i = 0; i < 37; ++i) {
          const calcKey_t *key = getSystemFlag(FLAG_USER) ? &kbd_usr[i] : &kbd_std[i];
          if(key->primary == item) {
            switch(key->primaryAim) {
              case ITM_A:
              case ITM_B:
              case ITM_C:
              case ITM_D: {
                rbrMode = RBR_GLOBAL;
                currentRegisterBrowserScreen = key->primaryAim - ITM_A + REGISTER_A;
                return true;
              }
              case ITM_L: {
                rbrMode = RBR_GLOBAL;
                currentRegisterBrowserScreen = REGISTER_L;
                return true;
              }
              case ITM_I:
              case ITM_J:
              case ITM_K: {
                rbrMode = RBR_GLOBAL;
                currentRegisterBrowserScreen = key->primaryAim - ITM_I + REGISTER_I;
                return true;
              }
            }
          }
        }
        return false;
      }
    }
    return true;
  }



  static void _registerBrowserDraw(void) {
    int16_t registerNameWidth;

    hourGlassIconEnabled = false;

    if(currentRegisterBrowserScreen == 9999) { // Init
      currentRegisterBrowserScreen = REGISTER_X;
      rbrMode = RBR_GLOBAL;
      showContent = true;
      rbr1stDigit = true;
    }

    if(rbrMode == RBR_GLOBAL) { // Global registers
      for(int16_t row=0; row<10; row++) {
        calcRegister_t regist = (currentRegisterBrowserScreen + row) % FIRST_LOCAL_REGISTER;
        switch(regist) {
          case REGISTER_X: {
            strcpy(tmpString, "X:");
            break;
          }
          case REGISTER_Y: {
            strcpy(tmpString, "Y:");
            break;
          }
          case REGISTER_Z: {
            strcpy(tmpString, "Z:");
            break;
          }
          case REGISTER_T: {
            strcpy(tmpString, "T:");
            break;
          }
          case REGISTER_A: {
            strcpy(tmpString, "A:");
            break;
          }
          case REGISTER_B: {
            strcpy(tmpString, "B:");
            break;
          }
          case REGISTER_C: {
            strcpy(tmpString, "C:");
            break;
          }
          case REGISTER_D: {
            strcpy(tmpString, "D:");
            break;
          }
          case REGISTER_L: {
            strcpy(tmpString, "L:");
            break;
          }
          case REGISTER_I: {
            strcpy(tmpString, "I:");
            break;
          }
          case REGISTER_J: {
            strcpy(tmpString, "J:");
            break;
          }
          case REGISTER_K: {
            strcpy(tmpString, "K:");
            break;
          }
          default: {
            sprintf(tmpString, "R%02d:", regist);
          }
        }

        // register name or number
        registerNameWidth = showString(tmpString, &standardFont, 1, 219 - 22 * row, vmNormal, false, true);

        if(   (regist <  REGISTER_X && regist % 5 == 4)
           || (regist >= REGISTER_X && regist % 4 == 3)) {
          lcd_fill_rect(0, 218 - 22 * row, SCREEN_WIDTH, 1, LCD_EMPTY_VALUE);
        }

        _showRegisterInRbr(regist, registerNameWidth);

        showString(tmpString, &standardFont, SCREEN_WIDTH - stringWidth(tmpString, &standardFont, false, true) - 1, 219-22*row, vmNormal, false, true);
      }
    }

    else if(rbrMode == RBR_LOCAL) { // Local registers
      if(currentNumberOfLocalRegisters != 0) { // Local registers are allocated
        for(int16_t row=0; row<10; row++) {
          calcRegister_t regist = currentRegisterBrowserScreen + row;
          if(regist < FIRST_LOCAL_REGISTER + currentNumberOfLocalRegisters) {
            sprintf(tmpString, "R.%02d:", regist - FIRST_LOCAL_REGISTER);

            // register number
            registerNameWidth = showString(tmpString, &standardFont, 1, 219 - 22 * row, vmNormal, true, true);

            if(regist % 5 == 1) {
              lcd_fill_rect(0, 218 - 22 * row, SCREEN_WIDTH, 1, LCD_EMPTY_VALUE);
            }

            _showRegisterInRbr(regist, registerNameWidth);

            showString(tmpString, &standardFont, SCREEN_WIDTH - stringWidth(tmpString, &standardFont, false, true), 219 - 22 * row, vmNormal, false, true);
          }
        }
      }
      else { // no local register allocated
        rbrMode = RBR_GLOBAL;
        _registerBrowserDraw();
      }
    }

    else if(rbrMode == RBR_NAMED) { // Named variables
      for(int16_t row=0; row<10; row++) {
        calcRegister_t regist = currentRegisterBrowserScreen + row;
        if(regist < FIRST_NAMED_VARIABLE + numberOfNamedVariables) { // Named variables
          sprintf(tmpString, "%s:", (char *)allNamedVariables[regist - FIRST_NAMED_VARIABLE].variableName + 1);

          // variable name
          registerNameWidth = showString(tmpString, &standardFont, 1, 219 - 22 * row, vmNormal, true, true);

          if((regist % 5 == 1) || (regist == FIRST_NAMED_VARIABLE + numberOfNamedVariables - 1)) {
            lcd_fill_rect(0, 218 - 22 * row, SCREEN_WIDTH, 1, LCD_EMPTY_VALUE);
          }

          _showRegisterInRbr(regist, registerNameWidth);

          showString(tmpString, &standardFont, SCREEN_WIDTH - stringWidth(tmpString, &standardFont, false, true), 219 - 22 * row, vmNormal, false, true);
        }
        else { // Reserved variables
          if(regist < FIRST_RESERVED_VARIABLE) {
            regist -= FIRST_NAMED_VARIABLE + numberOfNamedVariables;
            regist += FIRST_RESERVED_VARIABLE + 12;
          }

          if(regist <= LAST_RESERVED_VARIABLE) { // Named variables
            sprintf(tmpString, "%s:", (char *)allReservedVariables[regist - FIRST_RESERVED_VARIABLE].reservedVariableName + 1);

            // variable name
            registerNameWidth = showString(tmpString, &standardFont, 1, 219 - 22 * row, vmNormal, true, true);

            if(regist % 5 == 1) {
              lcd_fill_rect(0, 218 - 22 * row, SCREEN_WIDTH, 1, LCD_EMPTY_VALUE);
            }

            _showRegisterInRbr(regist, registerNameWidth);

            showString(tmpString, &standardFont, SCREEN_WIDTH - stringWidth(tmpString, &standardFont, false, true), 219 - 22 * row, vmNormal, false, true);
          }
        }
      }
    }
  }



  void fnRegisterBrowser(uint16_t unusedButMandatoryParameter) {
    currentApp = appRegisterBrowser;
    appsEnter(glRegisterBrowser, _registerBrowserKeyHandler, _registerBrowserDraw);
  }
#endif // !TESTSUITE_BUILD
