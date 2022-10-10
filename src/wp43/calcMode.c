// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "calcMode.h"

#include "apps/apps.h"
#include "apps/timerApp.h"
#include "debug.h"
#include "error.h"
#include "flags.h"
#include "hal/gui.h"
#include "items.h"
#include "matrix.h"
#include "registers.h"
#include "saveRestoreCalcState.h"
#include "stack.h"
#include "ui/bufferize.h"
#include "ui/keyboard.h"
#include "ui/screen.h"
#include <string.h>
#include <unistd.h>

#include "wp43.h"

calcMode_t calcMode;

#if !defined(TESTSUITE_BUILD)
  #if !defined(DMCP_BUILD)
    static guiLayout_t previousLayout = MAX_GUI_LAYOUTS;

    void calcModeUpdateGui() {
      guiLayout_t newLayout = previousLayout;
      // Ideally this should be more fully specified
      switch(calcMode) {
        case cmApp:
          newLayout = appsGetLayout();
          break;
        default:
          if(tam.mode && !tam.alpha) {
            newLayout = glTam;
          }
          else if(catalog && calcMode != cmPem && catalog != CATALOG_MVAR) {
            newLayout = glAim;
          }
          else if(calcMode == cmAim || (tam.mode && tam.alpha) || getSystemFlag(FLAG_ALPHA)) {
            newLayout = glAim;
          }
          else if(calcMode == cmNormal || calcMode == cmPem || calcMode == cmMim || calcMode == cmAssign) {
            newLayout = glNormal;
          }
          else if(calcMode == cmTimerApp) {
            newLayout = glTimerApp;
          }
      }
      if(newLayout != previousLayout) {
        guiSetLayout(newLayout);
        previousLayout = newLayout;
      }
    }
  #endif // !DMCP_BUILD



  void fnOff(uint16_t unsuedParamButMandatory) {
    shiftF = false;
    shiftG = false;

    timerAppStop();

    #if defined(PC_BUILD)
      if(matrixIndex != INVALID_VARIABLE) {
        if(getRegisterDataType(matrixIndex) == dtReal34Matrix) {
          if(openMatrixMIMPointer.realMatrix.matrixElements) {
            realMatrixFree(&openMatrixMIMPointer.realMatrix);
          }
        }
        else if(getRegisterDataType(matrixIndex) == dtComplex34Matrix) {
          if(openMatrixMIMPointer.complexMatrix.matrixElements) {
            complexMatrixFree(&openMatrixMIMPointer.complexMatrix);
          }
        }
      }
      saveCalc();
      gtk_main_quit();
    #endif // PC_BUILD

    #if defined(DMCP_BUILD)
      SET_ST(STAT_PGM_END);
    #endif // DMCP_BUILD
  }



  void calcModeNormal(void) {
    calcMode = cmNormal;

    if(softmenuStack[0].softmenuId == 1) { // MyAlpha
      softmenuStack[0].softmenuId = 0; // MyMenu
    }

    clearSystemFlag(FLAG_ALPHA);
    hideCursor();
    cursorEnabled = false;
  }



  void calcModeAim(void) {
    alphaCase = AC_UPPER;
    nextChar = NC_NORMAL;

    if(!tam.mode && calcMode != cmAssign) {
      calcMode = cmAim;
      liftStack();

      clearRegisterLine(AIM_REGISTER_LINE, true, true);
      xCursor = 1;
      yCursor = Y_POSITION_OF_AIM_LINE + 6;
      cursorFont = &standardFont;
      cursorEnabled = true;
    }

    if(softmenuStack[0].softmenuId == 0) { // MyMenu
      softmenuStack[0].softmenuId = 1; // MyAlpha
    }

    setSystemFlag(FLAG_ALPHA);
  }



  void calcModeNim(void) {
    #if defined(DEBUGUNDO)
      printf(">>> saveForUndo from gui: calcModeNim\n");
    #endif // DEBUGUNDO
    saveForUndo();
    if(lastErrorCode == ERROR_RAM_FULL) {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function calcModeNim:", "there is not enough memory to save for undo!", NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
      return;
    }

    clearSystemFlag(FLAG_ALPHA);
    if(calcMode != cmPem && calcMode != cmMim) {
      calcMode = cmNim;

      liftStack();
      real34Zero(REGISTER_REAL34_DATA(REGISTER_X));
    }

    aimBuffer[0] = 0;
    hexDigits = 0;

    if(calcMode != cmPem) {
      clearRegisterLine(NIM_REGISTER_LINE, true, true);
      xCursor = 1;
      yCursor = Y_POSITION_OF_NIM_LINE;
      cursorEnabled = true;
      cursorFont = &numericFont;
    }
  }



  void calcModeEnter(calcMode_t newMode) {
    switch(newMode) {
      case cmNormal:
        calcModeNormal();
        break;
      case cmAim:
        calcModeAim();
        break;
      case cmNim:
        calcModeNim();
        break;
      case cmAssign:
      case cmTimerApp:
      case cmApp:
        previousCalcMode = calcMode;
        calcMode = newMode;
        break;
      default:
        break;
    }
    calcModeUpdateGui();
  }



  void calcModeLeave(void) {
    switch(calcMode) {
      case cmAssign:
      case cmTimerApp:
      case cmApp:
        calcMode = previousCalcMode;
        calcModeUpdateGui();
        break;
      default:
        break;
    }
  }



  void enterAsmModeIfMenuIsACatalog(int16_t id) {
    switch(-id) {
      case MNU_FCNS: {
        catalog = CATALOG_FCNS;
        break;
      }
      case MNU_CONST: {
        catalog = CATALOG_CNST;
        break;
      }
      case MNU_MENUS: {
        catalog = CATALOG_MENU;
        break;
      }
      case MNU_SYSFL: {
        catalog = CATALOG_SYFL;
        break;
      }
      case MNU_ALPHAINTL: {
        catalog = CATALOG_AINT;
        break;
      }
      case MNU_ALPHAintl: {
        catalog = CATALOG_aint;
        break;
      }
      case MNU_PROG:
      case MNU_RAM:
      case MNU_FLASH: {
        catalog = CATALOG_PROG;
        break;
      }
      case MNU_VAR: {
        catalog = CATALOG_VAR;
        break;
      }
      case MNU_MATRS: {
        catalog = CATALOG_MATRS;
        break;
      }
      case MNU_STRINGS: {
        catalog = CATALOG_STRINGS;
        break;
      }
      case MNU_DATES: {
        catalog = CATALOG_DATES;
        break;
      }
      case MNU_TIMES: {
        catalog = CATALOG_TIMES;
        break;
      }
      case MNU_ANGLES: {
        catalog = CATALOG_ANGLES;
        break;
      }
      case MNU_SINTS: {
        catalog = CATALOG_SINTS;
        break;
      }
      case MNU_LINTS: {
        catalog = CATALOG_LINTS;
        break;
      }
      case MNU_REALS: {
        catalog = CATALOG_REALS;
        break;
      }
      case MNU_CPXS: {
        catalog = CATALOG_CPXS;
        break;
      }
      case MNU_Solver:
      case MNU_Sf:
      case MNU_1STDERIV:
      case MNU_2NDDERIV:
      case MNU_MVAR: {
        catalog = CATALOG_MVAR;
        break;
      }
      default: {
        catalog = CATALOG_NONE;
      }
    }

    if(catalog) {
      if(calcMode == cmNim) {
        closeNim();
      }

      if(calcMode != cmPem || !getSystemFlag(FLAG_ALPHA)) {
        alphaCase = AC_UPPER;
        nextChar = NC_NORMAL;

        clearSystemFlag(FLAG_ALPHA);
        resetAlphaSelectionBuffer();

        if(catalog != CATALOG_MVAR) {
          calcModeUpdateGui();
        }
      }
    }
  }



  void leaveAsmMode(void) {
    catalog = CATALOG_NONE;
    calcModeUpdateGui();
  }
#endif // !TESTSUITE_BUILD
