// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "apps/flagBrowser.h"

#include "apps/apps.h"
#include "calcMode.h"
#include "charString.h"
#include "config.h"
#include "core/memory.h"
#include "display.h"
#include "flags.h"
#include "fonts.h"
#include "items.h"
#include "registers.h"
#include "ui/screen.h"
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "wp43.h"

flagScreen_t flagScreen;

#if !defined(TESTSUITE_BUILD)
  TO_QSPI const char flagLetter[] = "XYZTABCDLIJK";

  static void oneSystemFlag(uint16_t systemFlag, const char *systemFlagNamename, int16_t *line, bool *firstSystemFlag) {
    if(getSystemFlag(systemFlag)) {
      if(stringWidth(tmpString + CHARS_PER_LINE * *line, &standardFont, true, true) + stringWidth(systemFlagNamename, &standardFont, true, false) <= SCREEN_WIDTH - 1 - 8) { // SPACE is 8 pixel wide
        if(!*firstSystemFlag) {
          strcat(tmpString + CHARS_PER_LINE * *line, " ");
        }
        else {
          *firstSystemFlag = false;
        }
        strcat(tmpString + CHARS_PER_LINE * *line, systemFlagNamename);
      }
      else {
        xcopy(tmpString + CHARS_PER_LINE * ++(*line), systemFlagNamename, strlen(systemFlagNamename) + 1);
      }
    }
  }



  void flagBrowserInit(void) {
    flagScreen = fsUninitialized;
  }



  static bool _flagBrowserKeyHandler(int16_t item) {
    if(item == ITM_UP || item == ITM_DOWN) {
      if(flagScreen == fsScreen1) {
        flagScreen = fsScreen2;
      }
      else if(flagScreen == fsScreen2) {
        flagScreen = fsScreen1;
      }
      return true;
    }
    return false;
  }



  static void _flagBrowserDraw(void) {
    int16_t line = 0;
    bool firstFlag;

    hourGlassIconEnabled = false;

    char flagNumber[4];

    // Free memory
    sprintf(tmpString + CHARS_PER_LINE * line++, "%" PRIu32 " bytes free in RAM, %" PRIu32 " in flash.", getFreeRamMemory(), getFreeFlash());

    // Global flags
    sprintf(tmpString + CHARS_PER_LINE * line++, "Global user flags set:");

    tmpString[CHARS_PER_LINE * line] = 0;
    firstFlag = true;
    for(int f=0; f<NUMBER_OF_GLOBAL_FLAGS; f++) {
      if(getFlag(f)) {
        if(f < 10) {
          flagNumber[0] = '0' + f;
          flagNumber[1] = 0;
        }
        else if(f < 100) {
          flagNumber[0] = '0' + f/10;
          flagNumber[1] = '0' + f%10;
          flagNumber[2] = 0;
        }
        else {
          flagNumber[0] = flagLetter[f-100];
          flagNumber[1] = 0;
        }

        if(stringWidth(tmpString + CHARS_PER_LINE * line, &standardFont, true, true) + stringWidth(flagNumber, &standardFont, true, false) <= SCREEN_WIDTH - 1 - 8) { // SPACE is 8 pixel wide
          if(!firstFlag) {
            strcat(tmpString + CHARS_PER_LINE * line, " ");
          }
          else {
            firstFlag = false;
          }
          strcat(tmpString + CHARS_PER_LINE * line, flagNumber);
        }
        else {
          xcopy(tmpString + CHARS_PER_LINE * ++line, flagNumber, 4);
        }
      }
    }

    if(currentLocalFlags == NULL) {
      sprintf(tmpString + CHARS_PER_LINE * ++line, "No local flags and registers are allocated.");
    }
    else {
      if(currentLocalRegisters == NULL) {
        sprintf(tmpString + CHARS_PER_LINE * ++line, "No local registers are allocated.");
      }
      else {
        // Local registers
        if(currentNumberOfLocalRegisters == 1) {
          strcpy(tmpString + CHARS_PER_LINE * ++line, "1 local register is allocated.");
        }
        else {
          sprintf(tmpString + CHARS_PER_LINE * ++line, "%" PRIu8 " local registers are allocated.", currentNumberOfLocalRegisters);
        }
      }

      // Local flags
      tmpString[CHARS_PER_LINE * ++line] = 0;
      firstFlag = true;
      for(int f=0; f<NUMBER_OF_LOCAL_FLAGS; f++) {
        if(getFlag(NUMBER_OF_GLOBAL_FLAGS + f)) {
          if(f < 10) {
            flagNumber[0] = '0' + f;
            flagNumber[1] = 0;
          }
          else {
            flagNumber[0] = '0' + f/10;
            flagNumber[1] = '0' + f%10;
            flagNumber[2] = 0;
          }

          if(stringWidth(tmpString + CHARS_PER_LINE * line, &standardFont, true, true) + stringWidth(flagNumber, &standardFont, true, false) <= SCREEN_WIDTH - 1 - 8) { // SPACE is 8 pixel wide
            if(!firstFlag) {
              strcat(tmpString + CHARS_PER_LINE * line, " ");
            }
            else {
              firstFlag = false;
            }
            strcat(tmpString + CHARS_PER_LINE * line, flagNumber);
          }
          else {
            xcopy(tmpString + CHARS_PER_LINE * ++line, flagNumber, 4);
          }
        }
      }
    }

    // Empty line
    tmpString[CHARS_PER_LINE * ++line] = 0;

    // Rounding mode
    strcpy(tmpString + CHARS_PER_LINE * ++line, "RM=");
    switch(roundingMode) {
      case rmHalfEven: {
        strcat(tmpString + CHARS_PER_LINE * line, STD_ONE_HALF "E");
        break;
      }

      case rmHalfUp: {
        strcat(tmpString + CHARS_PER_LINE * line, STD_ONE_HALF STD_UP_ARROW);
        break;
      }

      case rmHalfDown: {
        strcat(tmpString + CHARS_PER_LINE * line, STD_ONE_HALF STD_DOWN_ARROW);
        break;
      }

      case rmUp: {
        strcat(tmpString + CHARS_PER_LINE * line, STD_LEFT_ARROW "0" STD_RIGHT_ARROW);
        break;
      }

      case rmDown: {
        strcat(tmpString + CHARS_PER_LINE * line, STD_RIGHT_ARROW "0" STD_LEFT_ARROW);
        break;
      }

      case rmCeil: {
        strcat(tmpString + CHARS_PER_LINE * line, STD_LEFT_CEILING "x" STD_RIGHT_CEILING);
        break;
      }

      case rmFloor: {
        strcat(tmpString + CHARS_PER_LINE * line, STD_LEFT_FLOOR "x" STD_RIGHT_FLOOR);
        break;
      }

      default: {
        strcat(tmpString + CHARS_PER_LINE * line, "???");
      }
    }

    // Significant digits
    strcat(tmpString + CHARS_PER_LINE * line, "  SDIGS=");
    uint8_t sd = (significantDigits == 0 ? 34 : significantDigits);
    if(sd < 10) {
      flagNumber[0] = '0' + sd;
      flagNumber[1] = 0;
    }
    else {
      flagNumber[0] = '0' + sd/10;
      flagNumber[1] = '0' + sd%10;
      flagNumber[2] = 0;
    }
    strcat(tmpString + CHARS_PER_LINE * line, flagNumber);

    // ULP of X
    switch(getRegisterDataType(REGISTER_X)) {
      case dtLongInteger:
      case dtShortInteger: {
        strcat(tmpString + CHARS_PER_LINE * line, "  ULP of reg X = 1");
        break;
      }

      case dtReal34: {
        if(real34IsInfinite(REGISTER_REAL34_DATA(REGISTER_X))) {
          strcat(tmpString + CHARS_PER_LINE * line, "  ULP of reg X = " STD_INFINITY);
        }
        else {
          real34_t x34;
          real34NextPlus(REGISTER_REAL34_DATA(REGISTER_X), &x34);
          if(real34IsInfinite(&x34)) {
            real34NextMinus(REGISTER_REAL34_DATA(REGISTER_X), &x34);
            real34Subtract(REGISTER_REAL34_DATA(REGISTER_X), &x34, &x34);
          }
          else {
            real34Subtract(&x34, REGISTER_REAL34_DATA(REGISTER_X), &x34);
          }
          strcat(tmpString + CHARS_PER_LINE * line, "  ULP of reg X = 10");
          supNumberToDisplayString(real34GetExponent(&x34), tmpString + CHARS_PER_LINE * line + strlen(tmpString + CHARS_PER_LINE * line), NULL, false, NULL);
        }
        break;
      }

      default: {
      }
    }

    // System flags
    firstFlag = true;
    tmpString[CHARS_PER_LINE * ++line] = 0;
    oneSystemFlag(FLAG_ALLENG,  "ALLENG",  &line, &firstFlag);
    oneSystemFlag(FLAG_ALPIN,   "ALP.IN",  &line, &firstFlag);
    oneSystemFlag(FLAG_AUTOFF,  "AUTOFF",  &line, &firstFlag);
    oneSystemFlag(FLAG_AUTXEQ,  "AUTXEQ",  &line, &firstFlag);
    oneSystemFlag(FLAG_CPXj,    "CPXj",    &line, &firstFlag);
    oneSystemFlag(FLAG_ENDPMT,  "ENDPMT",  &line, &firstFlag);
    oneSystemFlag(FLAG_FRACT,   "FRACT",   &line, &firstFlag);
    oneSystemFlag(FLAG_GROW,    "GROW",    &line, &firstFlag);
    oneSystemFlag(FLAG_IGN1ER,  "IGN1ER",  &line, &firstFlag);
    oneSystemFlag(FLAG_INTING,  "INTING",  &line, &firstFlag);
    oneSystemFlag(FLAG_LEAD0,   "LEAD.0",  &line, &firstFlag);
    oneSystemFlag(FLAG_NUMIN,   "NUM.IN",  &line, &firstFlag);
    oneSystemFlag(FLAG_PRTACT,  "PRTACT",  &line, &firstFlag);
    oneSystemFlag(FLAG_QUIET,   "QUIET",   &line, &firstFlag);
    oneSystemFlag(FLAG_SLOW,    "SLOW",    &line, &firstFlag);
    oneSystemFlag(FLAG_SOLVING, "SOLVING", &line, &firstFlag);
    oneSystemFlag(FLAG_SPCRES,  "SPCRES",  &line, &firstFlag);
    oneSystemFlag(FLAG_SSIZE8,  "SSIZE8",  &line, &firstFlag);
    oneSystemFlag(FLAG_TRACE,   "TRACE",   &line, &firstFlag);
    oneSystemFlag(FLAG_USB,     "USB",     &line, &firstFlag);
    oneSystemFlag(FLAG_VMDISP,  "VMDISP",  &line, &firstFlag);
    line++;

    if(flagScreen == fsUninitialized) {
      if(line <= 9) {
        flagScreen = fsOnlyOneScreen;
      }
      else {
        flagScreen = fsScreen1;
      }
    }

    if(flagScreen == fsScreen1 || flagScreen == fsOnlyOneScreen) {
      for(int f=0; f<min(9, line); f++) {
        showString(tmpString + CHARS_PER_LINE * f, &standardFont, 1, 22*f + 43, vmNormal, true, false);
      }
    }
    if(flagScreen == fsScreen2) {
      for(int f=9; f<line; f++) {
        showString(tmpString + CHARS_PER_LINE * f, &standardFont, 1, 22*(f-9) + 43, vmNormal, true, false);
      }
    }
  }



  void fnFlagBrowser(uint16_t unusedButMandatoryParameter) {
    hourGlassIconEnabled = false;
    if(calcMode == cmAim) {
      hideCursor();
      cursorEnabled = false;
    }
    clearSystemFlag(FLAG_ALPHA);
    flagScreen = fsUninitialized;
    appsEnter(glFlagFontBrowser, _flagBrowserKeyHandler, _flagBrowserDraw);
  }
#endif // !TESTSUITE_BUILD
