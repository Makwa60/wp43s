  /* This file is part of 43S.
 *
 * 43S is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * 43S is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 43S.  If not, see <http://www.gnu.org/licenses/>.
 */

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
#include <string.h>

#include "wp43.h"

bool_t  showContent;
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
          sprintf(tmpString, "%d bytes", (int16_t)TO_BYTES(REAL34_SIZE));
        }
        break;
      }

      case dtComplex34: {
        if(showContent) {
          complex34ToDisplayString(REGISTER_COMPLEX34_DATA(regist), tmpString, &standardFont, SCREEN_WIDTH - 1 - registerNameWidth, 34, false, STD_SPACE_4_PER_EM, false);
        }
        else {
          sprintf(tmpString, "%d bytes", (int16_t)TO_BYTES(COMPLEX34_SIZE));
        }
        break;
      }

      case dtLongInteger: {
        if(showContent) {
          if(getRegisterLongIntegerSign(regist) == liNegative) {
            longIntegerRegisterToDisplayString(regist, tmpString, TMP_STR_LENGTH, SCREEN_WIDTH - 1 - registerNameWidth, 50, STD_SPACE_4_PER_EM);
          }
          else {
            longIntegerRegisterToDisplayString(regist, tmpString, TMP_STR_LENGTH, SCREEN_WIDTH - 9 - registerNameWidth, 50, STD_SPACE_4_PER_EM);
          }
        }
        else {
          sprintf(tmpString, "%" PRIu32 " bits " STD_CORRESPONDS_TO " 4+%" PRIu32 " bytes", (uint32_t)TO_BYTES(getRegisterMaxDataLength(regist)) * 8, (uint32_t)TO_BYTES(getRegisterMaxDataLength(regist)));
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
            tmpString[stringLastGlyph(tmpString)] = 0;
            while(stringWidth(tmpString, &standardFont, false, true) >= SCREEN_WIDTH - 12 - registerNameWidth) { // 12 is the width of STD_ELLIPSIS
              tmpString[stringLastGlyph(tmpString)] = 0;
            }
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
          sprintf(tmpString, "%d bytes", (int16_t)TO_BYTES(REAL34_SIZE));
        }
        break;
      }

      case dtDate: {
        if(showContent) {
          dateToDisplayString(regist, tmpString);
        }
        else {
          sprintf(tmpString, "%d bytes", (int16_t)TO_BYTES(REAL34_SIZE));
        }
        break;
      }

      case dtReal34Matrix: {
        if(showContent) {
          real34MatrixToDisplayString(regist, tmpString);
        }
        else {
          dataBlock_t* dblock = REGISTER_REAL34_MATRIX_DBLOCK(regist);
          sprintf(tmpString, "%" PRIu16 " element%s " STD_CORRESPONDS_TO " 4+%" PRIu32 " bytes", (uint16_t)(dblock->matrixRows * dblock->matrixColumns), (dblock->matrixRows * dblock->matrixColumns)==1 ? "" : "s", (uint32_t)TO_BYTES(dblock->matrixRows * dblock->matrixColumns * REAL34_SIZE));
        }
        break;
      }

      case dtComplex34Matrix: {
        if(showContent) {
          complex34MatrixToDisplayString(regist, tmpString);
        }
        else {
          dataBlock_t* dblock = REGISTER_COMPLEX34_MATRIX_DBLOCK(regist);
          sprintf(tmpString, "%" PRIu16 " element%s " STD_CORRESPONDS_TO " 4+%" PRIu32 " bytes", (uint16_t)(dblock->matrixRows * dblock->matrixColumns), (dblock->matrixRows * dblock->matrixColumns)==1 ? "" : "s", (uint32_t)TO_BYTES(dblock->matrixRows * dblock->matrixColumns * COMPLEX34_SIZE));
        }
        break;
      }

      case dtConfig: {
        if(showContent) {
          strcpy(tmpString, "Configuration data");
        }
        else {
          sprintf(tmpString, "%d bytes", (int16_t)TO_BYTES(CONFIG_SIZE));
        }
        break;
      }

      default: {
        sprintf(tmpString, "Data type %s: to be coded", getDataTypeName(getRegisterDataType(regist), false, true));
      }
    }
  }



  static bool_t _registerBrowserKeyHandler(int16_t item) {
    if(ITM_0 <= item && item <= ITM_9 && (rbrMode == RBR_GLOBAL || rbrMode == RBR_LOCAL)) {
      if(rbr1stDigit) {
        rbr1stDigit = false;
        rbrRegister = item - ITM_0;
      }
      else {
        rbr1stDigit = true;
        rbrRegister = rbrRegister*10 + item - ITM_0;

        if(rbrMode == RBR_GLOBAL) {
          currentRegisterBrowserScreen = rbrRegister;
        }
        else {
          rbrRegister = (rbrRegister >= currentNumberOfLocalRegisters ? 0 : rbrRegister);
          currentRegisterBrowserScreen = FIRST_LOCAL_REGISTER + rbrRegister;
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
          currentRegisterBrowserScreen = modulo(currentRegisterBrowserScreen - FIRST_NAMED_VARIABLE + 1, numberOfNamedVariables) + FIRST_NAMED_VARIABLE;
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
          currentRegisterBrowserScreen = modulo(currentRegisterBrowserScreen - 1000 - 1, numberOfNamedVariables) + 1000;
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
          if(currentLocalRegisters != NULL) {
            rbrMode = RBR_LOCAL;
            currentRegisterBrowserScreen = FIRST_LOCAL_REGISTER;
          }
          else if(numberOfNamedVariables > 0) {
            rbrMode = RBR_NAMED;
            currentRegisterBrowserScreen = FIRST_NAMED_VARIABLE;
          }
        }
        else if(rbrMode == RBR_LOCAL) {
          if(numberOfNamedVariables > 0) {
            rbrMode = RBR_NAMED;
            currentRegisterBrowserScreen = FIRST_NAMED_VARIABLE;
          }
          else {
            rbrMode = RBR_GLOBAL;
            currentRegisterBrowserScreen = REGISTER_X;
          }
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
        }
        break;
      }
      default:
        return false;
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

            if(   (regist <  REGISTER_X && regist % 5 == 4)
               || (regist >= REGISTER_X && regist % 4 == 3)) {
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
  }



  void fnRegisterBrowser(uint16_t unusedButMandatoryParameter) {
    hourGlassIconEnabled = false;

    if(calcMode == cmAim) {
      hideCursor();
      cursorEnabled = false;
    }

    clearSystemFlag(FLAG_ALPHA);

    appsEnter(glRegisterBrowser, _registerBrowserKeyHandler, _registerBrowserDraw);
  }
#endif // !TESTSUITE_BUILD
