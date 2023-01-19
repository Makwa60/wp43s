// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "ui/keyboard.h"

#include "apps/apps.h"
#include "apps/bugScreen.h"
#include "apps/flagBrowser.h"
#include "apps/fontBrowser.h"
#include "apps/registerBrowser.h"
#include "apps/timerApp.h"
#include "assign.h"
#include "calcMode.h"
#include "charString.h"
#include "config.h"
#include "constants.h"
#include "core/memory.h"
#include "debug.h"
#include "error.h"
#include "flags.h"
#include "hal/gui.h"
#include "hal/lcd.h"
#include "hal/system.h"
#include "hal/timer.h"
#include "items.h"
#include "mathematics/matrix.h"
#include "plotstat.h"
#include "programming/manage.h"
#include "programming/nextStep.h"
#include "programming/programmableMenu.h"
#include "recall.h"
#include "registers.h"
#include "solver/equation.h"
#include "solver/graph.h"
#include "sort.h"
#include "stack.h"
#include "stats.h"
#include "ui/bufferize.h"
#include "ui/cursor.h"
#include "ui/screen.h"
#include "ui/softmenus.h"
#include "ui/tam.h"
#if (REAL34_WIDTH_TEST == 1)
  #include "registerValueConversions.h"
#endif // (REAL34_WIDTH_TEST == 1)
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "wp43.h"

keyCode_t lastKeyCode;
bool      _kbCheckForInterrupt = false;
bool      _kbSeenInterrupt     = false;

#if !defined(TESTSUITE_BUILD)
  static bool inAutoRepeat = false;

  int16_t determineFunctionKeyItem(keyCode_t keyCode) {
    int16_t item = ITM_NOP;

    dynamicMenuItem = -1;

    int16_t itemShift = (shiftF ? 6 : (shiftG ? 12 : 0));
    int16_t fn = keyCode - kcF1;
    const softmenu_t *sm;
    int16_t row, menuId = softmenuStack[0].softmenuId;
    int16_t firstItem = softmenuStack[0].firstItem;

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
    switch(-softmenu[menuId].menuItem) {
      case MNU_MyMenu: {
        dynamicMenuItem = firstItem + itemShift + fn;
        item = userMenuItems[dynamicMenuItem].item;
        break;
      }

      case MNU_MyAlpha: {
        dynamicMenuItem = firstItem + itemShift + fn;
        item = userAlphaItems[dynamicMenuItem].item;
        break;
      }

      case MNU_DYNAMIC: {
        dynamicMenuItem = firstItem + itemShift + fn;
        item = userMenus[currentUserMenu].menuItem[dynamicMenuItem].item;
        break;
      }

      case MNU_PROG: {
        dynamicMenuItem = firstItem + itemShift + fn;
        if(tam.function == ITM_GTOP) {
          item = (dynamicMenuItem >= dynamicSoftmenu[menuId].numItems ? ITM_NOP : ITM_GTOP);
        }
        else {
          item = (dynamicMenuItem >= dynamicSoftmenu[menuId].numItems ? ITM_NOP : MNU_DYNAMIC);
        }
        break;
      }

      case MNU_VAR: {
        dynamicMenuItem = firstItem + itemShift + fn;
        item = (dynamicMenuItem >= dynamicSoftmenu[menuId].numItems ? ITM_NOP : MNU_DYNAMIC);
        break;
      }

      case MNU_MVAR: {
        dynamicMenuItem = firstItem + itemShift + fn;
        if(currentMvarLabel != INVALID_VARIABLE) {
          item = (dynamicMenuItem >= dynamicSoftmenu[menuId].numItems ? ITM_NOP : ITM_SOLVE_VAR);
        }
        else if((currentSolverStatus & SOLVER_STATUS_USES_FORMULA) && (currentSolverStatus & SOLVER_STATUS_INTERACTIVE) && ((currentSolverStatus & SOLVER_STATUS_EQUATION_MODE) == SOLVER_STATUS_EQUATION_SOLVER) && dynamicMenuItem == 5) {
          item = ITM_CALC;
        }
        else if((currentSolverStatus & SOLVER_STATUS_USES_FORMULA) && (currentSolverStatus & SOLVER_STATUS_INTERACTIVE) && ((currentSolverStatus & SOLVER_STATUS_EQUATION_MODE) == SOLVER_STATUS_EQUATION_SOLVER) && dynamicMenuItem == 4) {
          item = ITM_DRAW;
        }
        else if((currentSolverStatus & SOLVER_STATUS_USES_FORMULA) && (currentSolverStatus & SOLVER_STATUS_INTERACTIVE) && ((currentSolverStatus & SOLVER_STATUS_EQUATION_MODE) == SOLVER_STATUS_EQUATION_SOLVER) && dynamicMenuItem == 3) {
          item = ITM_CPXSLV;
        }
        else if((currentSolverStatus & SOLVER_STATUS_USES_FORMULA) && (currentSolverStatus & SOLVER_STATUS_INTERACTIVE) && *getNthString(dynamicSoftmenu[softmenuStack[0].softmenuId].menuContent, dynamicMenuItem) == 0) {
          item = ITM_NOP;
        }
        else if((currentSolverStatus & SOLVER_STATUS_USES_FORMULA) && (currentSolverStatus & SOLVER_STATUS_INTERACTIVE) && ((currentSolverStatus & SOLVER_STATUS_EQUATION_MODE) == SOLVER_STATUS_EQUATION_1ST_DERIVATIVE) && dynamicMenuItem == 5) {
          item = ITM_FPHERE;
        }
        else if((currentSolverStatus & SOLVER_STATUS_USES_FORMULA) && (currentSolverStatus & SOLVER_STATUS_INTERACTIVE) && ((currentSolverStatus & SOLVER_STATUS_EQUATION_MODE) == SOLVER_STATUS_EQUATION_1ST_DERIVATIVE) && dynamicMenuItem == 4) {
          item = ITM_DRAW;
        }
        else if((currentSolverStatus & SOLVER_STATUS_USES_FORMULA) && (currentSolverStatus & SOLVER_STATUS_INTERACTIVE) && ((currentSolverStatus & SOLVER_STATUS_EQUATION_MODE) == SOLVER_STATUS_EQUATION_2ND_DERIVATIVE) && dynamicMenuItem == 5) {
          item = ITM_FPPHERE;
        }
        else if((currentSolverStatus & SOLVER_STATUS_USES_FORMULA) && (currentSolverStatus & SOLVER_STATUS_INTERACTIVE) && ((currentSolverStatus & SOLVER_STATUS_EQUATION_MODE) == SOLVER_STATUS_EQUATION_2ND_DERIVATIVE) && dynamicMenuItem == 4) {
          item = ITM_DRAW;
        }
        else if(dynamicMenuItem >= dynamicSoftmenu[menuId].numItems) {
          item = ITM_NOP;
        }
        else if(!(currentSolverStatus & SOLVER_STATUS_INTERACTIVE)) {
          item = MNU_DYNAMIC;
        }
        else if( ((currentSolverStatus & SOLVER_STATUS_EQUATION_MODE) == SOLVER_STATUS_EQUATION_INTEGRATE) && dynamicMenuItem == 2) {
          item = ITM_DRAW;
        }
        else if((currentSolverStatus & SOLVER_STATUS_EQUATION_MODE) == SOLVER_STATUS_EQUATION_INTEGRATE) {
          item = ITM_Sfdx_VAR;
        }
        else {
          item = ITM_SOLVE_VAR;
        }
        break;
      }

      case MNU_MATRS:
      case MNU_STRINGS:
      case MNU_DATES:
      case MNU_TIMES:
      case MNU_ANGLES:
      case MNU_SINTS:
      case MNU_LINTS:
      case MNU_REALS:
      case MNU_CPXS: {
        dynamicMenuItem = firstItem + itemShift + fn;
        item = (dynamicMenuItem >= dynamicSoftmenu[menuId].numItems ? ITM_NOP : (tamIsActive() && tam.mode == tmDelItem) ? MNU_DYNAMIC : ITM_RCL);
        break;
      }

      case MNU_RAM:
      case MNU_FLASH: {
        dynamicMenuItem = firstItem + itemShift + fn;
        item = (dynamicMenuItem >= dynamicSoftmenu[menuId].numItems ? ITM_NOP : (tamIsActive() && tam.mode == tmDelItem) ? MNU_DYNAMIC : ITM_XEQ);
        break;
      }

      case MNU_MENUS: {
        dynamicMenuItem = firstItem + itemShift + fn;
        item = ITM_NOP;
        if(dynamicMenuItem < dynamicSoftmenu[menuId].numItems) {
          for(uint32_t i = 0; softmenu[i].menuItem < 0; ++i) {
            if(compareString((char *)getNthString(dynamicSoftmenu[menuId].menuContent, dynamicMenuItem), indexOfItems[-softmenu[i].menuItem].itemCatalogName, CMP_NAME) == 0) {
              if(tamIsActive() && tam.mode == tmDelItem) {
                item = MNU_DYNAMIC;
                tam.value = numberOfUserMenus;
              }
              else {
                item = softmenu[i].menuItem;
              }
            }
          }
          for(uint32_t i = 0; i < numberOfUserMenus; ++i) {
            if(compareString((char *)getNthString(dynamicSoftmenu[menuId].menuContent, dynamicMenuItem), userMenus[i].menuName, CMP_NAME) == 0) {
              if(tamIsActive() && tam.mode == tmDelItem) {
                item = MNU_DYNAMIC;
                tam.value = i;
              }
              else {
                item = -MNU_DYNAMIC;
                currentUserMenu = i;
              }
            }
          }
        }
        break;
      }

      case ITM_MENU: {
        dynamicMenuItem = firstItem + itemShift + fn;
        item = ITM_MENU;
        break;
      }

      case MNU_EQN: {
        if(numberOfFormulae == 0 && (firstItem + itemShift + fn) > 0) {
          break;
        }
        /* fallthrough */
      }

      default: {
        sm = &softmenu[menuId];
        row = min(3, (sm->numItems + modulo(firstItem - sm->numItems, 6))/6 - firstItem/6) - 1;
        if(itemShift/6 <= row && firstItem + itemShift + fn < sm->numItems) {
          item = (sm->softkeyItem)[firstItem + itemShift + fn] % 10000;

          if(item == ITM_PROD_SIGN) {
            item = (getSystemFlag(FLAG_MULTx) ? ITM_DOT : ITM_CROSS);
          }
        }
      }
    }
  #pragma GCC diagnostic pop

    if(calcMode == cmAssign && item != ITM_NOP && item != ITM_NULL) {
      switch(-softmenu[menuId].menuItem) {
        case MNU_PROG:
        case MNU_RAM:
        case MNU_FLASH: {
          return findNamedLabel((char *)getNthString(dynamicSoftmenu[menuId].menuContent, dynamicMenuItem)) - FIRST_LABEL + ASSIGN_LABELS;
        }
        case MNU_VAR:
        case MNU_MATRS:
        case MNU_STRINGS:
        case MNU_DATES:
        case MNU_TIMES:
        case MNU_ANGLES:
        case MNU_SINTS:
        case MNU_LINTS:
        case MNU_REALS:
        case MNU_CPXS: {
          return findNamedVariable((char *)getNthString(dynamicSoftmenu[menuId].menuContent, dynamicMenuItem)) - FIRST_NAMED_VARIABLE + ASSIGN_NAMED_VARIABLES;
        }
        case MNU_MENUS: {
          if(item == -MNU_DYNAMIC) {
            for(int32_t i = 0; i < numberOfUserMenus; ++i) {
              if(compareString((char *)getNthString(dynamicSoftmenu[menuId].menuContent, dynamicMenuItem), userMenus[i].menuName, CMP_NAME) == 0) {
                return ASSIGN_USER_MENU - i;
              }
            }
            bugScreen("In function determineFunctionKeyItem: nonexistent menu specified!");
            return item;
          }
          else {
            return item;
          }
        }
        default: {
          return item;
        }
      }
    }
    else {
      return item;
    }
  }



  void cbAutoRepeat(uint16_t key) {
    uint8_t origScreenUpdatingMode = screenUpdatingMode;

    timerStart(tidAutoRepeat, key, KEY_AUTOREPEAT_PERIOD);

    inAutoRepeat = true;
    btnClicked(key);
    inAutoRepeat = false;
    screenUpdatingMode = origScreenUpdatingMode;
  }



  static void _closeCatalog(void) {
    bool inCatalog = false;
    for(int i = 0; i < SOFTMENU_STACK_SIZE; ++i) {
      if(softmenu[softmenuStack[i].softmenuId].menuItem == -MNU_CATALOG) {
        inCatalog = true;
        break;
      }
      else if(softmenu[softmenuStack[i].softmenuId].menuItem == -MNU_MENUS) {
        break;
      }
    }
    if(inCatalog || softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_CONST) {
      switch(-softmenu[softmenuStack[0].softmenuId].menuItem) {
        case MNU_TAM:
        case MNU_TAMCMP:
        case MNU_TAMSTORCL:
        case MNU_TAMFLAG:
        case MNU_TAMSHUFFLE:
        case MNU_TAMLABEL: {
          // TAM menus are processed elsewhere
          break;
        }
        default: {
          leaveAsmMode();
          popSoftmenu();
        }
      }
    }
  }



  static void processAimInput(int16_t item) {
    if(alphaCase == AC_LOWER && (ITM_A <= item && item <= ITM_Z)) {
      addItemToBuffer(item + (ITM_a - ITM_A));
      keyActionProcessed = true;
    }

    else if(alphaCase == AC_LOWER && (ITM_ALPHA <= item && item <= ITM_OMEGA)) {
      addItemToBuffer(item + (ITM_alpha - ITM_ALPHA));
      keyActionProcessed = true;
    }

    else if(item == ITM_DOWN_ARROW) {
      nextChar = NC_SUBSCRIPT;
      keyActionProcessed = true;
    }

    else if(item == ITM_UP_ARROW) {
      nextChar = NC_SUPERSCRIPT;
      keyActionProcessed = true;
    }

    else if(item >= 0 && indexOfItems[item].func == addItemToBuffer) {
      addItemToBuffer(item);
      keyActionProcessed = true;
    }

    if(keyActionProcessed) {
      refreshScreen();
    }
  }



  static keyCode_t asnKey = 0;

  void btnFnPressed(keyCode_t keyCode) {
    asnKey = keyCode;

    if(programRunStop == PGM_RUNNING || programRunStop == PGM_PAUSED) {
      lastKeyCode = keyCode;
    }
    else {
      lastKeyCode = kcNoKey;
    }

    if(programRunStop == PGM_PAUSED) {
      programRunStop = PGM_KEY_PRESSED_WHILE_PAUSED;
      return;
    }
    if(tamIsWaitingKey()) {
      // not processed here
      return;
    }
    if(calcMode == cmAssign && itemToBeAssigned != 0 && !(tam.alpha && tam.mode != tmNewMenu)) {
      int16_t item = determineFunctionKeyItem(keyCode);

      #pragma GCC diagnostic push
      #pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
      switch(-softmenu[softmenuStack[0].softmenuId].menuItem) {
        case MNU_MENUS: {
          if(item <= ASSIGN_USER_MENU) {
            currentUserMenu = ASSIGN_USER_MENU - item;
            item = -MNU_DYNAMIC;
          }
          /* fallthrough */
        }
        case MNU_CATALOG:
        case MNU_CHARS:
        case MNU_PROGS:
        case MNU_VARS: {
          #if (FN_KEY_TIMEOUT_TO_NOP == 1)
            showFunctionName(item, 1000); // 1000ms = 1s
          #else // (FN_KEY_TIMEOUT_TO_NOP == 0)
            showFunctionNameItem = item;
          #endif // (FN_KEY_TIMEOUT_TO_NOP == 1)
          break;
        }
        default: {
          updateAssignTamBuffer();
        }
      }
      #pragma GCC diagnostic pop
      //_closeCatalog();
    }
    else if(calcMode != cmApp) {
      int16_t item = determineFunctionKeyItem(keyCode);

      if(shiftF || shiftG) {
        screenUpdatingMode &= ~SCRUPD_MANUAL_SHIFT_STATUS;
        clearShiftState();
      }

      shiftF = false;
      shiftG = false;
      if(item != ITM_NOP && item != ITM_NULL) {
        lastErrorCode = 0;

        if(calcMode != cmAssign && item >= 0 && indexOfItems[item].func == addItemToBuffer) {
          // If we are in the catalog then a normal key press should affect the Alpha Selection Buffer to choose
          // an item from the catalog, but a function key press should put the item in the AIM (or TAM) buffer
          // Use this variable to distinguish between the two
          if(calcMode == cmPem && !tamIsActive()) {
            if(getSystemFlag(FLAG_ALPHA)) {
              pemAlpha(item);
            }
            else {
              addStepInProgram(item);
            }
            hourGlassIconEnabled = false;
          }
          else {
            fnKeyInCatalog = 1;
            addItemToBuffer(item);
            fnKeyInCatalog = 0;
          }
          if(calcMode == cmEim && !tamIsActive()) {
            while(softmenu[softmenuStack[0].softmenuId].menuItem != -MNU_EQ_EDIT) {
              popSoftmenu();
            }
          }
          _closeCatalog();
          refreshScreen();
        }

        else {
          #if (FN_KEY_TIMEOUT_TO_NOP == 1)
            showFunctionName(item, 1000); // 1000ms = 1s
          #else // (FN_KEY_TIMEOUT_TO_NOP == 0)
            showFunctionNameItem = item;
          #endif // (FN_KEY_TIMEOUT_TO_NOP == 1)
        }
      }
      else {
        showFunctionNameItem = ITM_NOP;
      }
    }
  }



  static bool _assignToMenu(keyCode_t keyCode) {
    switch(-softmenu[softmenuStack[0].softmenuId].menuItem) {
      case MNU_MyMenu: {
        assignToMyMenu((keyCode - kcF1) + (shiftG ? 12 : shiftF ? 6 : 0));
        calcMode = previousCalcMode;
        shiftF = shiftG = false;
        _closeCatalog();
        refreshScreen();
        screenUpdatingMode &= ~SCRUPD_ONE_TIME_FLAGS;
        return true;
      }
      case MNU_MyAlpha: {
        assignToMyAlpha((keyCode - kcF1) + (shiftG ? 12 : shiftF ? 6 : 0));
        calcMode = previousCalcMode;
        shiftF = shiftG = false;
        _closeCatalog();
        refreshScreen();
        screenUpdatingMode &= ~SCRUPD_ONE_TIME_FLAGS;
        return true;
      }
      case MNU_DYNAMIC: {
        if(itemToBeAssigned < 0) {
          displayCalcErrorMessage(ERROR_CANNOT_ASSIGN_HERE, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
          errorMoreInfo("cannot assign submenu '%s' in user-created menu", indexOfItems[-itemToBeAssigned].itemCatalogName);
        }
        else {
          assignToUserMenu((keyCode - kcF1) + (shiftG ? 12 : shiftF ? 6 : 0));
        }
        calcMode = previousCalcMode;
        shiftF = shiftG = false;
        _closeCatalog();
        refreshScreen();
        screenUpdatingMode &= ~SCRUPD_ONE_TIME_FLAGS;
        return true;
      }
      case MNU_CATALOG:
      case MNU_CHARS:
      case MNU_PROGS:
      case MNU_VARS:
      case MNU_MENUS: {
        screenUpdatingMode &= ~SCRUPD_ONE_TIME_FLAGS;
        return false;
      }
      default: {
        displayCalcErrorMessage(ERROR_CANNOT_ASSIGN_HERE, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        errorMoreInfo("the menu '%s' is write-protected", indexOfItems[-softmenu[softmenuStack[0].softmenuId].menuItem].itemCatalogName);
        calcMode = previousCalcMode;
        shiftF = shiftG = false;
        _closeCatalog();
        refreshScreen();
        screenUpdatingMode &= ~SCRUPD_ONE_TIME_FLAGS;
        return true;
      }
    }
  }



  void btnFnReleased(keyCode_t keyCode) {
    if(programRunStop == PGM_KEY_PRESSED_WHILE_PAUSED) {
      programRunStop = PGM_RESUMING;
      screenUpdatingMode &= ~SCRUPD_ONE_TIME_FLAGS;
      return;
    }
    if(calcMode != cmApp) {
      if(tamIsWaitingKey()) {
        if(tam.digitsSoFar == 0) {
          switch(keyCode) {
            case kcF1: {
              tamProcessInput(shiftG ? ITM_1 :                  ITM_0);
              tamProcessInput(shiftG ? ITM_3 : shiftF ? ITM_7 : ITM_1);
              break;
            }
            case kcF2: {
              tamProcessInput(shiftG ? ITM_1 :                  ITM_0);
              tamProcessInput(shiftG ? ITM_4 : shiftF ? ITM_8 : ITM_2);
              break;
            }
            case kcF3: {
              tamProcessInput(shiftG ? ITM_1 :                  ITM_0);
              tamProcessInput(shiftG ? ITM_5 : shiftF ? ITM_9 : ITM_3);
              break;
            }
            case kcF4: {
              tamProcessInput(     (shiftG || shiftF) ? ITM_1 : ITM_0);
              tamProcessInput(shiftG ? ITM_6 : shiftF ? ITM_0 : ITM_4);
              break;
            }
            case kcF5: {
              tamProcessInput(     (shiftG || shiftF) ? ITM_1 : ITM_0);
              tamProcessInput(shiftG ? ITM_7 : shiftF ? ITM_1 : ITM_5);
              break;
            }
            case kcF6: {
              tamProcessInput(     (shiftG || shiftF) ? ITM_1 : ITM_0);
              tamProcessInput(shiftG ? ITM_8 : shiftF ? ITM_2 : ITM_6);
              break;
            }
            default: {
              // This should be impossible because this is a function key
              assert(false);
              break;
            }
          }
          shiftF = shiftG = false;
          refreshScreen();
        }
        screenUpdatingMode &= ~SCRUPD_ONE_TIME_FLAGS;
        return;
      }

      if(calcMode == cmAssign && itemToBeAssigned != 0 && !(tam.alpha && tam.mode != tmNewMenu)) {
        if(_assignToMenu(keyCode)) {
          return;
        }
      }
      if(showFunctionNameItem != 0) {
        int16_t item = showFunctionNameItem;
        #if (FN_KEY_TIMEOUT_TO_NOP == 1)
          hideFunctionName();
        #else // FN_KEY_TIMEOUT_TO_NOP != 1
          showFunctionNameItem = 0;
        #endif // FN_KEY_TIMEOUT_TO_NOP == 1

        if(calcMode != cmConfirmation) {
          lastErrorCode = 0;

          if(calcMode != cmPem && item == -MNU_Sfdx) {
            tamEnterMode(MNU_Sfdx);
            refreshScreen();
            screenUpdatingMode &= ~SCRUPD_ONE_TIME_FLAGS;
            return;
          }
          else if(calcMode != cmPem && item == ITM_INTEGRAL) {
            reallyRunFunction(item, currentSolverVariable);
            refreshScreen();
            screenUpdatingMode &= ~SCRUPD_ONE_TIME_FLAGS;
            return;
          }
          else if(item < 0) { // softmenu
            if(calcMode == cmAssign && itemToBeAssigned == 0 && softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_MENUS) {
              itemToBeAssigned = item;
            }
            else {
              showSoftmenu(item);
              if((item == -MNU_Solver || item == -MNU_Sf || item == -MNU_1STDERIV || item == -MNU_2NDDERIV) && lastErrorCode != 0) {
                popSoftmenu();
                currentSolverStatus &= ~SOLVER_STATUS_INTERACTIVE;
                currentSolverStatus &= ~SOLVER_STATUS_EQUATION_MODE;
              }
            }
            refreshScreen();
            screenUpdatingMode &= ~SCRUPD_ONE_TIME_FLAGS;
            return;
          }
          if(tamIsActive() && catalog && (tam.digitsSoFar || tam.function == ITM_BESTF || tam.function == ITM_CNST || (!tam.indirect && (tam.mode == tmValue || tam.mode == tmValueChb || tamIsWaitingKey())))) {
            // disabled
          }
          else if(tam.function == ITM_GTOP && catalog == CATALOG_PROG) {
            runFunction(item);
            tamLeaveMode();
            hourGlassIconEnabled = false;
            _closeCatalog();
            refreshScreen();
            screenUpdatingMode &= ~SCRUPD_ONE_TIME_FLAGS;
            return;
          }
          else if(calcMode == cmPem && catalog && catalog != CATALOG_MVAR
          #if CLP_WITH_MENU != 0
            && (!tam.mode || tam.function != ITM_CLP)
          #endif /* CLP_WITH_MENU != 0 */
          ) { // TODO: is that correct
            if(indexOfItems[item].func == fnGetSystemFlag && (tam.mode == tmFlagR || tam.mode == tmFlagW) && !tam.indirect) {
              tam.value = (indexOfItems[item].param & 0xff);
              tam.alpha = true;
              addStepInProgram(tamOperation());
              tamLeaveMode();
              hourGlassIconEnabled = false;
            }
            else if(tamIsActive()) {
              const char *itmLabel = dynmenuGetLabel(dynamicMenuItem);
              uint16_t nameLength = stringByteLength(itmLabel);
              xcopy(aimBuffer, itmLabel, nameLength + 1);
              tam.alpha = true;
              addStepInProgram(tamOperation());
              tamLeaveMode();
              hourGlassIconEnabled = false;
            }
            else {
              runFunction(item);
            }
            _closeCatalog();
            refreshScreen();
            screenUpdatingMode &= ~SCRUPD_ONE_TIME_FLAGS;
            return;
          }

          // If we are in the catalog then a normal key press should affect the Alpha Selection Buffer to choose
          // an item from the catalog, but a function key press should put the item in the AIM (or TAM) buffer
          // Use this variable to distinguish between the two
          fnKeyInCatalog = 1;
          if(tamIsActive() && catalog && (tam.digitsSoFar || tam.function == ITM_BESTF || tam.function == ITM_CNST || (!tam.indirect && (tam.mode == tmValue || tam.mode == tmValueChb)))) {
            // disabled
          }
          else if(tamIsActive() && (!tam.alpha || isAlphabeticSoftmenu())) {
            addItemToBuffer(item);
          }
          else if((calcMode == cmNormal || calcMode == cmAim) && isAlphabeticSoftmenu()) {
            if(calcMode == cmNormal) {
              fnAim(NOPARAM);
            }
            addItemToBuffer(item);
          }
          else if(calcMode == cmEim && catalog && catalog != CATALOG_MVAR) {
            addItemToBuffer(item);
            while(softmenu[softmenuStack[0].softmenuId].menuItem != -MNU_EQ_EDIT) {
              popSoftmenu();
            }
          }
          else if((calcMode == cmNormal || calcMode == cmNim) && (ITM_0<=item && item<=ITM_F) && (!catalog || catalog == CATALOG_MVAR)) {
            addItemToNimBuffer(item);
          }
          else if(calcMode == cmMim && softmenu[softmenuStack[0].softmenuId].menuItem != -MNU_M_EDIT) {
            addItemToBuffer(item);
          }
          else if(item > 0) { // function
            if(calcMode == cmNim && item != ITM_CC) {
              closeNim();
              if(calcMode != cmNim) {
                if(indexOfItems[item].func == fnConstant) {
                  setSystemFlag(FLAG_ASLIFT);
                }
              }
            }
            if(calcMode == cmAim && !isAlphabeticSoftmenu()) {
              closeAim();
            }
            if(tam.alpha && calcMode != cmAssign && tam.mode != tmNewMenu) {
              tamLeaveMode();
            }

            if(lastErrorCode == 0) {
              if(temporaryInformation == TI_VIEW_REGISTER) {
                temporaryInformation = TI_NO_INFO;
                updateMatrixHeightCache();
              }
              else {
                temporaryInformation = TI_NO_INFO;
              }
              if(programRunStop == PGM_WAITING) {
                programRunStop = PGM_STOPPED;
              }
              if(calcMode == cmAssign && itemToBeAssigned == 0 && item != ITM_NOP) {
                if(tam.alpha) {
                  processAimInput(item);
                  if(stringGlyphLength(aimBuffer) > 6) {
                    assignLeaveAlpha();
                    assignGetName1();
                  }
                }
                else if(item == ITM_AIM) { // in case α is already assigned
                  assignEnterAlpha();
                  keyActionProcessed = true;
                }
                else {
                  itemToBeAssigned = item;
                }
              }
              else if(calcMode == cmAssign && tam.alpha && tam.mode != tmNewMenu && item != ITM_NOP) {
                processAimInput(item);
                if(stringGlyphLength(aimBuffer) > 6) {
                  assignLeaveAlpha();
                  assignGetName2();
                }
              }
              else {
                runFunction(item);
              }
            }
          }
          _closeCatalog();
          fnKeyInCatalog = 0;
        }
      }

      refreshScreen();
      screenUpdatingMode &= ~SCRUPD_ONE_TIME_FLAGS;
    }
  }



  int16_t determineItem(keyCode_t keyCode) {
    int16_t result;
    const calcKey_t *key;

    dynamicMenuItem = -1;
    key = getSystemFlag(FLAG_USER) ? &kbd_usr[keyCode - 1] : &kbd_std[keyCode - 1];

    // App mode doesn't support shift keys
    if(calcMode == cmApp) {
      return key->primary;
    }

    // Shift f pressed and shift g not active
    if(key->primary == ITM_SHIFTf && !shiftG && (calcMode == cmNormal || calcMode == cmAim || calcMode == cmNim || calcMode == cmMim || calcMode == cmEim || calcMode == cmPem || calcMode == cmPlotStat || calcMode == cmGraph || calcMode == cmAssign)) {
      if(temporaryInformation == TI_VIEW_REGISTER) {
        temporaryInformation = TI_NO_INFO;
        updateMatrixHeightCache();
      }
      else {
        temporaryInformation = TI_NO_INFO;
      }
      if(programRunStop == PGM_WAITING) {
        programRunStop = PGM_STOPPED;
      }
      lastErrorCode = 0;
      shiftF = !shiftF;
      screenUpdatingMode &= ~SCRUPD_MANUAL_SHIFT_STATUS;
      return ITM_NOP;
    }

    // Shift g pressed and shift f not active
    else if(key->primary == ITM_SHIFTg && !shiftF && (calcMode == cmNormal || calcMode == cmAim || calcMode == cmNim || calcMode == cmMim || calcMode == cmEim || calcMode == cmPem || calcMode == cmPlotStat || calcMode == cmGraph || calcMode == cmAssign)) {
      if(temporaryInformation == TI_VIEW_REGISTER) {
        temporaryInformation = TI_NO_INFO;
        updateMatrixHeightCache();
      }
      else {
        temporaryInformation = TI_NO_INFO;
      }
      if(programRunStop == PGM_WAITING) {
        programRunStop = PGM_STOPPED;
      }
      lastErrorCode = 0;
      shiftG = !shiftG;
      screenUpdatingMode &= ~SCRUPD_MANUAL_SHIFT_STATUS;
      return ITM_NOP;
    }

    if(calcMode == cmAim || (catalog && catalog != CATALOG_MVAR && calcMode != cmNim) || calcMode == cmEim || tam.alpha || (calcMode == cmAssign && (previousCalcMode == cmAim || previousCalcMode == cmEim)) || (calcMode == cmPem && getSystemFlag(FLAG_ALPHA))) {
      result = shiftF ? key->fShiftedAim :
               shiftG ? key->gShiftedAim :
                        key->primaryAim;

    }
    else if(tamIsActive()) {
      result = key->primaryTam; // No shifted function in TAM
    }
    else if(calcMode == cmNormal || calcMode == cmNim || calcMode == cmMim || calcMode == cmConfirmation || calcMode == cmPem || calcMode == cmPlotStat || calcMode == cmGraph || calcMode == cmAssign || calcMode == cmTimerApp) {
      result = shiftF ? key->fShifted :
               shiftG ? key->gShifted :
                        key->primary;
    }
    else {
      bugScreen("In function determineItem: item was not determined!");
      result = 0;
    }

    if(result == ITM_PROD_SIGN) {
      result = (getSystemFlag(FLAG_MULTx) ? ITM_CROSS : ITM_DOT);
    }

    if((shiftF || shiftG) && result != ITM_SNAP) {
      screenUpdatingMode &= ~SCRUPD_MANUAL_SHIFT_STATUS;
      clearShiftState();
    }

    shiftF = false;
    shiftG = false;

    if(calcMode == cmAssign && itemToBeAssigned != 0 && (result == ITM_NOP || result == ITM_NULL)) {
      result = ITM_LBL;
    }

    return result;
  }



  void btnClicked(keyCode_t keyCode) {
    btnPressed(keyCode);
    btnReleased(keyCode);
  }



  void btnPressed(keyCode_t keyCode) {
    if(_kbCheckForInterrupt) {
      if(keyCode == kcExit) {
        _kbSeenInterrupt = true;
      }
      return;
    }
    if(keyCode >= kcF1) {
      btnFnPressed(keyCode);
      return;
    }
    asnKey = keyCode;

    if(programRunStop == PGM_RUNNING || programRunStop == PGM_PAUSED) {
      lastKeyCode = keyCode;
    }
    else {
      lastKeyCode = kcNoKey;
    }

    bool f = shiftF;
    bool g = shiftG;
    int16_t item = determineItem(keyCode);

    if(item == ITM_UP || item == ITM_DOWN || ((item == ITM_SST || item == ITM_BST) && calcMode == cmPem)) {
      if(currentSoftmenuScrolls() || (calcMode != cmNormal && calcMode != cmNim && calcMode != cmAim)) {
        if(!inAutoRepeat) {
          timerStart(tidAutoRepeat, keyCode, KEY_AUTOREPEAT_FIRST_PERIOD);
        }
        shiftF = f;
        shiftG = g;
      }
    }

    if(programRunStop == PGM_RUNNING || programRunStop == PGM_PAUSED) {
      if((item == ITM_RS || item == ITM_EXIT) && !getSystemFlag(FLAG_INTING) && !getSystemFlag(FLAG_SOLVING)) {
        programRunStop = PGM_WAITING;
        showFunctionNameItem = 0;
      }
      else if(programRunStop == PGM_PAUSED) {
        programRunStop = PGM_KEY_PRESSED_WHILE_PAUSED;
      }
      return;
    }

    if(getSystemFlag(FLAG_USER)) {
      int keyStateCode = (getSystemFlag(FLAG_ALPHA) ? 3 : 0) + (g ? 2 : f ? 1 : 0);
      char *funcParam = (char *)getNthString((uint8_t *)userKeyLabel, (keyCode - 1) * 6 + keyStateCode);
      xcopy(tmpString, funcParam, stringByteLength(funcParam) + 1);
    }
    else {
      *tmpString = 0;
    }

    showFunctionNameItem = 0;
    if(item != ITM_NOP && item != ITM_NULL) {
      processKeyAction(item);
      if(!keyActionProcessed) {
        showFunctionName(item, 1000); // 1000ms = 1s
      }
    }
    if(calcMode == cmAssign && itemToBeAssigned != 0 && tamBuffer[0] == 0) {
      shiftF = f;
      shiftG = g;
    }
  }



  void btnReleased(keyCode_t keyCode) {
    if(_kbCheckForInterrupt) {
      return;
    }
    if(keyCode >= kcF1) {
      btnFnReleased(keyCode);
      return;
    }
    int16_t item;

    if(programRunStop == PGM_KEY_PRESSED_WHILE_PAUSED) {
      programRunStop = PGM_RESUMING;
      screenUpdatingMode &= ~SCRUPD_ONE_TIME_FLAGS;
      return;
    }

    if(calcMode == cmAssign && itemToBeAssigned != 0 && tamBuffer[0] == 0) {
      assignToKey(keyCode);
      calcMode = previousCalcMode;
      shiftF = shiftG = false;
      refreshScreen();
    }
    else if(showFunctionNameItem != 0) {
      item = showFunctionNameItem;
      hideFunctionName();
      if(item < 0) {
        showSoftmenu(item);
      }
      else {
        int keyStateCode = (getSystemFlag(FLAG_ALPHA) ? 3 : 0) + (shiftG ? 2 : shiftF ? 1 : 0);
        char *funcParam = (char *)getNthString((uint8_t *)userKeyLabel, (keyCode - 1) * 6 + keyStateCode);

        if(item != ITM_NOP && tam.alpha && indexOfItems[item].func != addItemToBuffer) {
          // We are in TAM mode so need to cancel first (equivalent to EXIT)
          tamLeaveMode();
        }
        if(item == ITM_RCL && getSystemFlag(FLAG_USER) && funcParam[0] != 0) {
          calcRegister_t var = findNamedVariable(funcParam);
          if(var != INVALID_VARIABLE) {
            reallyRunFunction(item, var);
          }
          else if(getSystemFlag(FLAG_IGN1ER)) {
            clearSystemFlag(FLAG_IGN1ER);
            errorMoreInfo("string '%s' is not a named variable\nignored since IGN1ER was set", funcParam);
          }
          else {
            displayCalcErrorMessage(ERROR_UNDEF_SOURCE_VAR, ERR_REGISTER_LINE, REGISTER_X);
            errorMoreInfo("string '%s' is not a named variable", funcParam);
          }
        }
        else if(item == ITM_XEQ && getSystemFlag(FLAG_USER) && funcParam[0] != 0) {
          calcRegister_t label = findNamedLabel(funcParam);
          if(label != INVALID_VARIABLE) {
            reallyRunFunction(item, label);
          }
          else if(getSystemFlag(FLAG_IGN1ER)) {
            clearSystemFlag(FLAG_IGN1ER);
            errorMoreInfo("string '%s' is not a named label\nignored since IGN1ER was set", funcParam);
          }
          else {
            displayCalcErrorMessage(ERROR_LABEL_NOT_FOUND, ERR_REGISTER_LINE, REGISTER_X);
            errorMoreInfo("string '%s' is not a named label", funcParam);
          }
        }
        else {
          runFunction(item);
        }
      }
    }
    if(!inAutoRepeat && timerIsRunning(tidAutoRepeat)) {
      timerStop(tidAutoRepeat);
      shiftF = shiftG = false;
    }
    if(!timerIsRunning(tidAutoRepeat)) {
      refreshScreen();
    }
    screenUpdatingMode &= ~SCRUPD_ONE_TIME_FLAGS;
  }



  void leavePem(void) {
      const uint16_t mask = ~((1 << BITS_TO_SHIFT) - 1); // 0xfffc for 4 byte blocks, 0xfff8 for 8 byte blocks, ...
    if(freeProgramBytes >= TO_BYTES(1)) { // Push the programs to the end of RAM
      uint32_t newProgramSize = (uint32_t)((uint8_t *)(ram + RAM_SIZE_IN_BLOCKS) - beginOfProgramMemory) - (freeProgramBytes & mask);
      uint16_t localStepNumber = currentLocalStepNumber;
      uint16_t programNumber = currentProgramNumber;
      uint16_t fdLocalStepNumber = firstDisplayedLocalStepNumber;
      bool     inRam = (programList[currentProgramNumber - 1].step > 0);
      if(inRam) {
        currentStep.ram           += (freeProgramBytes & mask);
        firstDisplayedStep.ram    += (freeProgramBytes & mask);
        beginOfCurrentProgram.ram += (freeProgramBytes & mask);
        endOfCurrentProgram.ram   += (freeProgramBytes & mask);
      }
      freeProgramBytes &= 0x03;
      resizeProgramMemory(newProgramSize);
      scanLabelsAndPrograms();
      if(inRam) {
        currentLocalStepNumber = localStepNumber;
        currentProgramNumber = programNumber;
        firstDisplayedLocalStepNumber = fdLocalStepNumber;
        defineCurrentStep();
        defineFirstDisplayedStep();
        defineCurrentProgramFromCurrentStep();
      }
    }
  }



  void processKeyAction(int16_t item) {
    keyActionProcessed = false;

    if(lastErrorCode != 0 && item != ITM_EXIT && item != ITM_BACKSPACE) {
      lastErrorCode = 0;
    }

    if(temporaryInformation == TI_VIEW_REGISTER) {
      temporaryInformation = TI_NO_INFO;
      updateMatrixHeightCache();
      if(item == ITM_UP || item == ITM_DOWN || item == ITM_EXIT) {
        temporaryInformation = TI_VIEW_REGISTER;
      }
    }
    else if(item != ITM_UP && item != ITM_DOWN && item != ITM_EXIT) {
      temporaryInformation = TI_NO_INFO;
    }
    if(programRunStop == PGM_WAITING) {
      programRunStop = PGM_STOPPED;
    }

    if(calcMode == cmApp) {
      appsHandleKey(item);
      keyActionProcessed = true;
      return;
    }

    #if (REAL34_WIDTH_TEST == 1)
      longInteger_t lgInt;
      longIntegerInit(lgInt);
    #endif // (REAL34_WIDTH_TEST == 1)

    switch(item) {
      case ITM_BACKSPACE: {
        fnKeyBackspace(NOPARAM);
        keyActionProcessed = true;
        break;
      }

      case ITM_UP: {
        fnKeyUp(NOPARAM);
        if(currentSoftmenuScrolls() || calcMode != cmNormal || temporaryInformation != TI_NO_INFO) {
          refreshScreen();
        }
        temporaryInformation = TI_NO_INFO;
        keyActionProcessed = true;
        #if (REAL34_WIDTH_TEST == 1)
          if(++largeur > SCREEN_WIDTH) {
            largeur--;
          }
          uIntToLongInteger(largeur, lgInt);
          convertLongIntegerToLongIntegerRegister(lgInt, REGISTER_Z);
        #endif // (REAL34_WIDTH_TEST == 1)
        break;
      }

      case ITM_DOWN: {
        fnKeyDown(NOPARAM);
        if(currentSoftmenuScrolls() || calcMode != cmNormal || temporaryInformation != TI_NO_INFO) {
          refreshScreen();
        }
        temporaryInformation = TI_NO_INFO;
        keyActionProcessed = true;
        #if (REAL34_WIDTH_TEST == 1)
          if(--largeur < 20) {
            largeur++;
          }
          uIntToLongInteger(largeur, lgInt);
          convertLongIntegerToLongIntegerRegister(lgInt, REGISTER_Z);
        #endif // (REAL34_WIDTH_TEST == 1)
        break;
      }

      case ITM_EXIT: {
        fnKeyExit(NOPARAM);
        if(temporaryInformation != TI_NO_INFO) {
          refreshScreen();
        }
        temporaryInformation = TI_NO_INFO;
        keyActionProcessed = true;
        break;
      }

      case ITM_CC:
      case ITM_dotD: {
        if(calcMode == cmAssign) {
          if(itemToBeAssigned == 0) {
            itemToBeAssigned = item;
          }
          else {
            tamBuffer[0] = 0;
          }
          keyActionProcessed = true;
        }
        else if(calcMode == cmTimerApp) {
          keyActionProcessed = true;
        }
        else if(calcMode == cmPem && item == ITM_dotD && aimBuffer[0] == 0) {
          addStepInProgram(ITM_toREAL);
          keyActionProcessed = true;
        }
        break;
      }

      case ITM_ENTER: {
        if(calcMode == cmAssign) {
          if(itemToBeAssigned == 0) {
            if(tam.alpha) {
              assignLeaveAlpha();
              assignGetName1();
            }
            else {
              itemToBeAssigned = ASSIGN_CLEAR;
            }
          }
          else {
            if(tam.alpha && tam.mode != tmNewMenu) {
              assignLeaveAlpha();
              assignGetName2();
            }
            else if(tam.alpha) {
              tamBuffer[0] = 0;
            }
          }
          keyActionProcessed = true;
        }
        else if(tamIsActive()) {
          tamProcessInput(ITM_ENTER);
          keyActionProcessed = true;
        }
        break;
      }

      default: {
        if(calcMode == cmAssign && itemToBeAssigned != 0 && item == ITM_USERMODE) {
          while(softmenuStack[0].softmenuId > 1) {
            popSoftmenu();
          }
          if(previousCalcMode == cmAim) {
            softmenuStack[0].softmenuId = 1;
            calcModeUpdateGui();
          }
          else {
            leaveAsmMode();
          }
          keyActionProcessed = true;
        }
        else if(calcMode == cmAssign && itemToBeAssigned == 0 && item == ITM_USERMODE) {
          tamEnterMode(ITM_ASSIGN);
          calcMode = previousCalcMode;
          keyActionProcessed = true;
        }
        else if(calcMode == cmAssign && item == ITM_AIM) {
          assignEnterAlpha();
          keyActionProcessed = true;
        }
        else if(calcMode == cmAssign && itemToBeAssigned != 0 && item == -MNU_CATALOG) {
          showSoftmenu(-MNU_CATALOG);
          keyActionProcessed = true;
        }
        else if((calcMode != cmPem || !getSystemFlag(FLAG_ALPHA)) && catalog && catalog != CATALOG_MVAR) {
          if(ITM_A <= item && item <= ITM_Z && alphaCase == AC_LOWER) {
            addItemToBuffer(item + 26);
            keyActionProcessed = true;
          }

          else if(ITM_ALPHA <= item && item <= ITM_OMEGA && alphaCase == AC_LOWER) {
            addItemToBuffer(item + 36);
            keyActionProcessed = true;
          }

          else if(item == ITM_DOWN_ARROW || item == ITM_UP_ARROW) {
            addItemToBuffer(item);
            keyActionProcessed = true;
          }
          break;
        }
        else if(tamIsActive()) {
          if(tam.alpha) {
            processAimInput(item);
          }
          else {
            addItemToBuffer(item);
            keyActionProcessed = true;
          }
          break;
        }
        else {
          switch(calcMode) {
            case cmNormal: {
              if(item == ITM_EXPONENT || item == ITM_PERIOD || (ITM_0 <= item && item <= ITM_9)) {
                addItemToNimBuffer(item);
                keyActionProcessed = true;
              }
              // Following commands do not timeout to NOP
              else if(item == ITM_UNDO || item == ITM_BST || item == ITM_SST || item == ITM_PR || item == ITM_AIM) {
                runFunction(item);
                keyActionProcessed = true;
              }
              break;
            }

            case cmAim: {
              if(item == ITM_BST || item == ITM_SST) {
                closeAim();
                runFunction(item);
                keyActionProcessed = true;
              }
              else {
                processAimInput(item);
              }
              break;
            }

            case cmEim: {
              processAimInput(item);
              break;
            }

            case cmNim: {
              if(item == ITM_BST || item == ITM_SST) {
                closeNim();
                runFunction(item);
                keyActionProcessed = true;
              }
              else {
                keyActionProcessed = true;
                addItemToNimBuffer(item);
              }
              break;
            }

            case cmMim: {
              addItemToBuffer(item);
              keyActionProcessed = true;
              break;
            }

            case cmErrorMessage: {
              keyActionProcessed = true;
              break;
            }

            case cmGraph:
            case cmPlotStat: {
              if(item == ITM_SNAP) {
                runFunction(item);
                keyActionProcessed = true;
              }
              break;
            }

            case cmConfirmation: {
              if(item == ITM_3 || item == ITM_XEQ || item == ITM_ENTER) { // Yes or XEQ or ENTER
                calcMode = previousCalcMode;
                confirmedFunction(CONFIRMED);
              }

              else if(item == ITM_DIV || item == ITM_EXIT) { // No or EXIT
                calcMode = previousCalcMode;
              }

              else {
                temporaryInformation = TI_ARE_YOU_SURE;
              }

              keyActionProcessed = true;
              break;
            }

            case cmPem: {
              if(item == ITM_PR) {
                leavePem();
                calcModeEnter(cmNormal);
                keyActionProcessed = true;
              }
              else if(item == ITM_OFF) {
                fnOff(NOPARAM);
                keyActionProcessed = true;
              }
              else if(item == ITM_SST) {
                fnSst(NOPARAM);
                keyActionProcessed = true;
                refreshScreen();
              }
              else if(item == ITM_BST) {
                fnBst(NOPARAM);
                keyActionProcessed = true;
                refreshScreen();
              }
              else if(aimBuffer[0] != 0 && !getSystemFlag(FLAG_ALPHA) && (item == ITM_toINT || (nimNumberPart == NP_INT_BASE && item == ITM_RCL))) {
                pemAddNumber(item);
                keyActionProcessed = true;
                if(item == ITM_RCL) {
                  currentStep = findPreviousStep(currentStep);
                  --currentLocalStepNumber;
                  if(!programListEnd) {
                    scrollPemBackwards();
                  }
                }
              }
              else if(item == ITM_RS) {
                addStepInProgram(ITM_STOP);
                keyActionProcessed = true;
              }
              break;
            }

            case cmAssign: {
              if(item > 0 && itemToBeAssigned == 0) {
                if(tam.alpha) {
                  processAimInput(item);
                  if(stringGlyphLength(aimBuffer) > 6) {
                    assignLeaveAlpha();
                    assignGetName1();
                  }
                }
                else {
                  itemToBeAssigned = item;
                }
                keyActionProcessed = true;
              }
              else if(item != 0 && itemToBeAssigned != 0) {
                if(tam.alpha && tam.mode != tmNewMenu) {
                  if(item > 0) {
                    processAimInput(item);
                    if(stringGlyphLength(aimBuffer) > 6) {
                      assignLeaveAlpha();
                      assignGetName2();
                    }
                    keyActionProcessed = true;
                  }
                }
                else {
                  switch(item) {
                    case ITM_ENTER:
                    case ITM_SHIFTf:
                    case ITM_SHIFTg:
                    case ITM_USERMODE:
                    case ITM_EXIT:
                    case ITM_BACKSPACE: {
                      break;
                    }
                    default: {
                      tamBuffer[0] = 0;
                      keyActionProcessed = true;
                    }
                  }
                }
              }
              break;
            }

            case cmTimerApp: {
              switch(item) {
                case ITM_RS: {
                  timerAppStartStop();
                  break;
                }
                case ITM_0:
                case ITM_1:
                case ITM_2:
                case ITM_3:
                case ITM_4:
                case ITM_5:
                case ITM_6:
                case ITM_7:
                case ITM_8:
                case ITM_9: {
                  timerAppDigitKey(item - ITM_0);
                  break;
                }
                case ITM_PERIOD: {
                  timerAppDot();
                  break;
                }
                case ITM_ADD: {
                  timerAppPlus();
                  break;
                }
                case ITM_RCL: {
                  runFunction(ITM_TIMER_RCL);
                  break;
                }
                case ITM_SHIFTf: {
                  if(!tamIsActive()) {
                    fnScreenDump(NOPARAM);
                  }
                  break;
                }
              }
              keyActionProcessed = true;
              break;
            }

            default: {
              sprintf(errorMessage, "In function processKeyAction: %" PRIu8 " is an unexpected value while processing calcMode!", (uint8_t)calcMode);
              bugScreen(errorMessage);
            }
          }
        }
      }
    }
    #if (REAL34_WIDTH_TEST == 1)
      longIntegerFree(lgInt);
    #endif // (REAL34_WIDTH_TEST == 1)
  }



  static void menuUp(void) {
    int16_t menuId = softmenuStack[0].softmenuId;
    int16_t sm = softmenu[menuId].menuItem;

    screenUpdatingMode &= ~SCRUPD_MANUAL_MENU;
    if(temporaryInformation == TI_NO_INFO && lastErrorCode == ERROR_NONE) {
      screenUpdatingMode |= SCRUPD_SKIP_STACK_ONE_TIME;
    }

    if((sm == -MNU_alpha_omega || sm == -MNU_ALPHAintl) && alphaCase == AC_LOWER) {
      alphaCase = AC_UPPER;
      softmenuStack[0].softmenuId--; // Switch to the upper case menu
    }
    else if((sm == -MNU_ALPHADOT || sm == -MNU_ALPHAMATH) && alphaCase == AC_LOWER) {
      alphaCase = AC_UPPER;
    }
    else {
      int16_t itemShift = (catalog == CATALOG_NONE ? 18 : 6);

      if((softmenuStack[0].firstItem + itemShift) < (menuId < NUMBER_OF_DYNAMIC_SOFTMENUS ? dynamicSoftmenu[menuId].numItems : softmenu[menuId].numItems)) {
        softmenuStack[0].firstItem += itemShift;
      }
      else {
        softmenuStack[0].firstItem = 0;
      }

      setCatalogLastPos();
    }
  }



  static void menuDown(void) {
    int16_t menuId = softmenuStack[0].softmenuId;
    int16_t sm = softmenu[menuId].menuItem;

    screenUpdatingMode &= ~SCRUPD_MANUAL_MENU;
    if(temporaryInformation == TI_NO_INFO && lastErrorCode == ERROR_NONE) {
      screenUpdatingMode |= SCRUPD_SKIP_STACK_ONE_TIME;
    }

    if((sm == -MNU_ALPHA_OMEGA || sm == -MNU_ALPHAINTL) && alphaCase == AC_UPPER) {
      alphaCase = AC_LOWER;
      softmenuStack[0].softmenuId++; // Switch to the lower case menu
    }
    else if((sm == -MNU_ALPHADOT || sm == -MNU_ALPHAMATH) && alphaCase == AC_UPPER) {
      alphaCase = AC_LOWER;
    }
    else {
      int16_t itemShift = (catalog == CATALOG_NONE ? 18 : 6);

      if((softmenuStack[0].firstItem - itemShift) >= 0) {
        softmenuStack[0].firstItem -= itemShift;
      }
      else if((softmenuStack[0].firstItem - itemShift) >= -5) {
        softmenuStack[0].firstItem = 0;
      }
      else {
        if(menuId < NUMBER_OF_DYNAMIC_SOFTMENUS) {
          softmenuStack[0].firstItem = ((dynamicSoftmenu[menuId].numItems - 1)/6) / (itemShift/6) * itemShift;
        }
        else {
          softmenuStack[0].firstItem = ((       softmenu[menuId].numItems - 1)/6) / (itemShift/6) * itemShift;
        }
      }
      setCatalogLastPos();
    }
  }
#endif // !TESTSUITE_BUILD



void fnKeyEnter(uint16_t unusedButMandatoryParameter) {
  #if !defined(TESTSUITE_BUILD)
    switch(calcMode) {
      case cmNormal: {
        setSystemFlag(FLAG_ASLIFT);
        #if defined(DEBUGUNDO)
          printf(">>> saveForUndo from fnKeyEnterA\n");
        #endif // DEBUGUNDO
        saveForUndo();
        if(lastErrorCode == ERROR_RAM_FULL) {
          goto undo_disabled;
        }

        liftStack();
        if(lastErrorCode == ERROR_RAM_FULL) {
          goto ram_full;
        }
        copySourceRegisterToDestRegister(REGISTER_Y, REGISTER_X);
        if(lastErrorCode == ERROR_RAM_FULL) {
          goto ram_full;
        }

        clearSystemFlag(FLAG_ASLIFT);
        break;
      }

      case cmAim: {
        calcModeEnter(cmNormal);
        popSoftmenu();

        if(aimBuffer[0] == 0) {
          #if defined(DEBUGUNDO)
            printf(">>> undo from fnKeyEnter\n");
          #endif // DEBUGUNDO
          undo();
        }
        else {
          int16_t lenInBytes = stringByteLength(aimBuffer) + 1;

          reallocateRegister(REGISTER_X, dtString, lenInBytes, amNone);
          xcopy(REGISTER_STRING_DATA(REGISTER_X), aimBuffer, lenInBytes);

          setSystemFlag(FLAG_ASLIFT);
          #if defined(DEBUGUNDO)
            printf(">>> saveForUndo from fnKeyEnterB\n");
          #endif // DEBUGUNDO
          saveForUndo();
          if(lastErrorCode == ERROR_RAM_FULL) {
            goto undo_disabled;
          }
          liftStack();
          if(lastErrorCode == ERROR_RAM_FULL) {
            goto ram_full;
          }
          clearSystemFlag(FLAG_ASLIFT);

          copySourceRegisterToDestRegister(REGISTER_Y, REGISTER_X);
          if(lastErrorCode == ERROR_RAM_FULL) {
            goto ram_full;
          }
          aimBuffer[0] = 0;
        }
        break;
      }

      case cmMim: {
        mimEnter(false);
        break;
      }

      case cmNim: {
        closeNim();

        if(calcMode != cmNim && lastErrorCode == 0) {
          setSystemFlag(FLAG_ASLIFT);
          #if defined(DEBUGUNDO)
            printf(">>> saveForUndo from fnKeyEnterC\n");
          #endif // DEBUGUNDO
          saveForUndo();
          if(lastErrorCode == ERROR_RAM_FULL) {
            goto undo_disabled;
          }
          liftStack();
          if(lastErrorCode == ERROR_RAM_FULL) {
            goto ram_full;
          }
          clearSystemFlag(FLAG_ASLIFT);
          copySourceRegisterToDestRegister(REGISTER_Y, REGISTER_X);
          if(lastErrorCode == ERROR_RAM_FULL) {
            goto ram_full;
          }
        }
        break;
      }

      case cmEim: {
        if(aimBuffer[0] != 0) {
          setEquation(currentFormula, aimBuffer);
        }
        if(softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_EQ_EDIT) {
          calcModeEnter(cmNormal);
          if(allFormulae[currentFormula].pointerToFormulaData == WP43_NULL) {
            deleteEquation(currentFormula);
          }
        }
        popSoftmenu();
        break;
      }

      case cmErrorMessage:
      case cmPlotStat:
      case cmGraph: {
        break;
      }

      case cmTimerApp: {
        timerAppEnter();
        break;
      }

      case cmConfirmation: {
        calcMode = previousCalcMode;
        confirmedFunction(CONFIRMED);
        break;
      }

      default: {
        sprintf(errorMessage, "In function fnKeyEnter: unexpected calcMode value (%" PRIu8 ") while processing key ENTER!", (uint8_t)calcMode);
        bugScreen(errorMessage);
      }
    }
    return;

undo_disabled:
    temporaryInformation = TI_UNDO_DISABLED;
    return;

ram_full:
    displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    #if defined(DEBUGUNDO)
      printf(">>> Undo from fnKeyEnterD\n");
    #endif // DEBUGUNDO
    fnUndo(NOPARAM);
    return;
  #endif // !TESTSUITE_BUILD
}



void fnKeyExit(uint16_t unusedButMandatoryParameter) {
  #if !defined(TESTSUITE_BUILD)
    if(tamIsWaitingKey()) {
      if(tam.digitsSoFar == 0) {
        tamProcessInput(ITM_2);
        tamProcessInput(ITM_1);
        shiftF = shiftG = false;
        refreshScreen();
      }
      return;
    }
    if(lastErrorCode == 0 && softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_MVAR) {
      currentSolverStatus &= ~SOLVER_STATUS_INTERACTIVE;
    }

    switch(calcMode) {
      case cmConfirmation:
      case cmErrorMessage: {
        // Browser or message should be closed first
        break;
      }

      default: {
        if(catalog) {
          if(lastErrorCode != 0) {
            lastErrorCode = 0;
          }
          else {
            leaveAsmMode();
            popSoftmenu();
            if(tamIsActive()) {
              numberOfTamMenusToPop--;
            }
          }
          return;
        }
      }
    }

    if(tamIsActive()) {
      if(numberOfTamMenusToPop > 1) {
        popSoftmenu();
        numberOfTamMenusToPop--;
      }
      else {
        if(calcMode == cmPem) {
          aimBuffer[0] = 0;
        }
        tamLeaveMode();
        if(calcMode == cmPem) {
          scrollPemBackwards();
        }
      }
      return;
    }

    switch(calcMode) {
      case cmConfirmation:
      case cmErrorMessage: {
        // Browser or message should be closed first
        break;
      }

      default: {
        if(softmenu[softmenuStack[0].softmenuId].menuItem == -ITM_MENU) {
          dynamicMenuItem = 20;
          fnProgrammableMenu(NOPARAM);
          return;
        }
      }
    }

    switch(calcMode) {
      case cmNormal: {
        if(temporaryInformation == TI_SHOW_REGISTER || temporaryInformation == TI_VIEW_REGISTER) {
          temporaryInformation = TI_NO_INFO;
        }
        else if(lastErrorCode != 0) {
          lastErrorCode = 0;
        }
        else {
          if(softmenuStack[0].softmenuId <= 1) { // MyMenu or MyAlpha is displayed
            currentInputVariable = INVALID_VARIABLE;
          }
          else {
            popSoftmenu();
          }
          screenUpdatingMode &= ~SCRUPD_MANUAL_MENU;
          if(temporaryInformation == TI_NO_INFO) {
            screenUpdatingMode |= SCRUPD_SKIP_STACK_ONE_TIME;
          }
        }
        break;
      }

      case cmAim: {
        if(softmenuStack[0].softmenuId <= 1) { // MyMenu or MyAlpha is displayed
          closeAim();
          #if defined(DEBUGUNDO)
            printf(">>> saveForUndo from fnKeyExitA\n");
          #endif  // DEBUGUNDO
          updateMatrixHeightCache();
          saveForUndo();
          if(lastErrorCode == ERROR_RAM_FULL) {
            goto undo_disabled;
          }
        }
        else {
          popSoftmenu();
        }
        break;
      }

      case cmNim: {
        addItemToNimBuffer(ITM_EXIT);
        updateMatrixHeightCache();
        break;
      }

      case cmMim: {
        if(lastErrorCode != 0) {
          lastErrorCode = 0;
        }
        else if(temporaryInformation == TI_SHOW_REGISTER) {
          temporaryInformation = TI_NO_INFO;
        }
        else {
          if(softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_M_EDIT) {
            mimEnter(true);
            if(matrixIndex == findNamedVariable(statMx)) {
              calcSigma(0);
            }
            mimFinalize();
            calcModeEnter(cmNormal);
            updateMatrixHeightCache();
          }
          screenUpdatingMode = SCRUPD_AUTO;
          popSoftmenu(); // close softmenu dedicated for the MIM
        }
        break;
      }

      case cmPem: {
        if(lastErrorCode != 0) {
          lastErrorCode = 0;
          break;
        }
        if(getSystemFlag(FLAG_ALPHA) && aimBuffer[0] == 0 && !tamIsActive()) {
          pemAlpha(ITM_BACKSPACE);
          fnBst(NOPARAM); // Set the PGM pointer to the original position
          break;
        }
        if(aimBuffer[0] != 0 && !tamIsActive()) {
          if(getSystemFlag(FLAG_ALPHA)) {
            pemCloseAlphaInput();
          }
          else {
            pemCloseNumberInput();
          }
          aimBuffer[0] = 0;
          fnBst(NOPARAM); // Set the PGM pointer to the original position
          break;
        }
        if(softmenuStack[0].softmenuId > 1) { // not MyMenu and not MyAlpha
          popSoftmenu();
          break;
        }

        aimBuffer[0] = 0;
        leavePem();
        calcModeEnter(cmNormal);
        #if defined(DEBUGUNDO)
          printf(">>> saveForUndo from fnKeyExitB\n");
        #endif // DEBUGUNDO
        saveForUndo();
        if(lastErrorCode == ERROR_RAM_FULL) {
          goto undo_disabled;
        }
        break;
      }

      case cmEim: {
        if(lastErrorCode != 0) {
          lastErrorCode = 0;
        }
        else {
          if(softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_EQ_EDIT) {
            calcModeEnter(cmNormal);
            if(allFormulae[currentFormula].pointerToFormulaData == WP43_NULL) {
              deleteEquation(currentFormula);
            }
          }
          popSoftmenu();
        }
        break;
      }

      case cmTimerApp: {
        screenUpdatingMode = SCRUPD_AUTO;
        if(lastErrorCode != 0) {
          lastErrorCode = 0;
        }
        else {
          timerAppLeave();
        }
        break;
      }

      case cmGraph:
      case cmPlotStat: {
        if(calcMode == cmPlotStat) {
          for(int16_t ii = 0; ii < 3; ii++) {
            if( (softmenuStack[0].softmenuId > 1) && !((-softmenu[softmenuStack[0].softmenuId].menuItem == MNU_HIST) || (-softmenu[softmenuStack[0].softmenuId].menuItem == MNU_STAT))) {
              popSoftmenu();
            }
          }
        } else {
          popSoftmenu();
        }
        lastPlotMode = PLOT_NOTHING;
        plotSelection = 0;
        calcMode = cmNormal;
        #if defined(DEBUGUNDO)
          printf(">>> Undo from fnKeyExit\n");
        #endif // DEBUGUNDO
        fnUndo(NOPARAM);
        fnClDrawMx();
        restoreStats();
        break;
      }

      case cmConfirmation: {
        calcMode = previousCalcMode;
        temporaryInformation = TI_NO_INFO;
        if(programRunStop == PGM_WAITING) {
          programRunStop = PGM_STOPPED;
        }
        break;
      }

      case cmAssign: {
        if((softmenuStack[0].softmenuId <= 1 && softmenuStack[1].softmenuId <= 1) || (previousCalcMode == cmEim && softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_EQ_EDIT)) { // MyMenu or MyAlpha is displayed
          calcMode = previousCalcMode;
          if(tam.alpha) {
            assignLeaveAlpha();
          }
        }
        else {
          popSoftmenu();
        }
        break;
      }

      default: {
        sprintf(errorMessage, "In function fnKeyExit: unexpected calcMode value (%" PRIu8 ") while processing key EXIT!", (uint8_t)calcMode);
        bugScreen(errorMessage);
      }
    }
    return;

undo_disabled:
    temporaryInformation = TI_UNDO_DISABLED;
    return;
  #endif // !TESTSUITE_BUILD
}



void fnKeyCC(uint16_t unusedButMandatoryParameter) {
  #if !defined(TESTSUITE_BUILD)
    uint32_t dataTypeX;
    uint32_t dataTypeY;

    switch(calcMode) {
      case cmNormal: {
        dataTypeX = getRegisterDataType(REGISTER_X);
        dataTypeY = getRegisterDataType(REGISTER_Y);

        if(   (dataTypeX == dtReal34 || dataTypeX == dtLongInteger)
           && (dataTypeY == dtReal34 || dataTypeY == dtLongInteger)) {
          runFunction(ITM_REtoCX);
        }
        else if(dataTypeX == dtComplex34) {
          runFunction(ITM_CXtoRE);
        }
        else if(dataTypeX == dtReal34Matrix && dataTypeY == dtReal34Matrix) {
          runFunction(ITM_REtoCX);
        }
        else if(dataTypeX == dtComplex34Matrix) {
          runFunction(ITM_CXtoRE);
        }
        else {
          displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X); // Invalid input data type for this operation
          errorMoreInfo("You cannot use CC with %s in X and %s in Y!", getDataTypeName(getRegisterDataType(REGISTER_X), true, false), getDataTypeName(getRegisterDataType(REGISTER_Y), true, false));
        }
        break;
      }

      case cmNim: {
        addItemToNimBuffer(ITM_CC);
        break;
      }

      case cmMim: {
        mimAddNumber(ITM_CC);
        break;
      }

      case cmPem: {
        if(aimBuffer[0] != 0 && !getSystemFlag(FLAG_ALPHA)) {
          pemAddNumber(ITM_CC);
        }
        break;
      }

      case cmEim:
      case cmPlotStat:
      case cmTimerApp:
      case cmGraph: {
        break;
      }

      default: {
        sprintf(errorMessage, "In function fnKeyCC: unexpected calcMode value (%" PRIu8 ") while processing key CC!", (uint8_t)calcMode);
        bugScreen(errorMessage);
      }
    }
  #endif // !TESTSUITE_BUILD
}



void fnKeyBackspace(uint16_t unusedButMandatoryParameter) {
  #if !defined(TESTSUITE_BUILD)
    uint16_t lg;
    uint8_t *nextStep;

    if(tamIsActive()) {
      tamProcessInput(ITM_BACKSPACE);
      return;
    }

    switch(calcMode) {
      case cmNormal: {
        if(lastErrorCode != 0) {
          lastErrorCode = 0;
        }
        else {
          runFunction(ITM_CLX);
        }
        break;
      }

      case cmAim: {
        if(catalog && catalog != CATALOG_MVAR) {
          if(stringByteLength(aimBuffer) > 0) {
            lg = stringLastGlyph(aimBuffer);
            aimBuffer[lg] = 0;
            uint32_t xCursor = showString(aimBuffer, &standardFont, 1, Y_POSITION_OF_AIM_LINE + 6, vmNormal, true, true);
            cursorShow(true, xCursor, Y_POSITION_OF_AIM_LINE + 6);
          }
        }
        else if(stringByteLength(aimBuffer) > 0) {
          lg = stringLastGlyph(aimBuffer);
          aimBuffer[lg] = 0;
        }
        if(aimBuffer[0] == 0) {
          closeAim();
          updateMatrixHeightCache();
        }
        break;
      }

      case cmNim: {
        addItemToNimBuffer(ITM_BACKSPACE);
        break;
      }

      case cmMim: {
        if(lastErrorCode != 0) {
          lastErrorCode = 0;
        }
        else {
          mimAddNumber(ITM_BACKSPACE);
        }
        break;
      }

      case cmEim: {
        if(equationEditorCursor > 0) {
          char *srcPos = aimBuffer;
          char *dstPos = aimBuffer;
          char *lstPos = aimBuffer + stringNextGlyph(aimBuffer, stringLastGlyph(aimBuffer));
          --equationEditorCursor;
          for(uint32_t i = 0; i < equationEditorCursor; ++i) {
            dstPos += (*dstPos & 0x80) ? 2 : 1;
          }
          srcPos = dstPos + ((*dstPos & 0x80) ? 2 : 1);
          for(; srcPos <= lstPos;) {
            *(dstPos++) = *(srcPos++);
          }
        }
        break;
      }

      //case cmAsm_over_normal: {
      //  addItemToBuffer(ITM_BACKSPACE);
      //  break;
      //}

      case cmPlotStat:
      case cmGraph: {
        if(calcMode == cmPlotStat) {
          for(int16_t ii = 0; ii < 3; ii++) {
            if( (softmenuStack[0].softmenuId > 1) && !((-softmenu[softmenuStack[0].softmenuId].menuItem == MNU_HIST) || (-softmenu[softmenuStack[0].softmenuId].menuItem == MNU_STAT))) {
              popSoftmenu();
            }
          }
        } else {
          popSoftmenu();
        }
        lastPlotMode = PLOT_NOTHING;
        plotSelection = 0;
        calcMode = cmNormal;
        #if defined(DEBUGUNDO)
          printf(">>> Undo from fnKeyExit\n");
        #endif // DEBUGUNDO
        fnUndo(NOPARAM);
        fnClDrawMx();
        restoreStats();
        break;
      }

      case cmConfirmation: {
        calcMode = previousCalcMode;
        temporaryInformation = TI_NO_INFO;
        if(programRunStop == PGM_WAITING) {
          programRunStop = PGM_STOPPED;
        }
        break;
      }

      case cmPem: {
        if(lastErrorCode != 0) {
          lastErrorCode = 0;
          return;
        }
        if(programList[currentProgramNumber - 1].step < 0) {
          // attempt to modify a program in the flash memory
          displayCalcErrorMessage(ERROR_FLASH_MEMORY_WRITE_PROTECTED, ERR_REGISTER_LINE, REGISTER_X);
          return;
        }
        if(getSystemFlag(FLAG_ALPHA)) {
          pemAlpha(ITM_BACKSPACE);
          if(aimBuffer[0] == 0 && getSystemFlag(FLAG_ALPHA)) {
            // close if no characters left
            pemAlpha(ITM_BACKSPACE);
          }
          if(aimBuffer[0] == 0 && !getSystemFlag(FLAG_ALPHA)) {
            if(currentLocalStepNumber > 1) {
              --currentLocalStepNumber;
              defineCurrentStep();
              if(!programListEnd) {
                scrollPemBackwards();
              }
            }
            else {
              pemCursorIsZerothStep = true;
            }
          }
        }
        else if(aimBuffer[0] == 0) {
          if(currentLocalStepNumber > 1) {
            pemCursorIsZerothStep = false;
          }
          if(!pemCursorIsZerothStep) {
            nextStep = findNextStep_ram(currentStep.ram);
            if(*currentStep.ram != 255 || *(currentStep.ram + 1) != 255) { // Not the last END
              deleteStepsFromTo(currentStep.ram, nextStep);
            }
            if(currentLocalStepNumber > 1) {
              --currentLocalStepNumber;
              defineCurrentStep();
            }
            else {
              pemCursorIsZerothStep = true;
            }
            scrollPemBackwards();
          }
        }
        else {
          pemAddNumber(ITM_BACKSPACE);
          if(aimBuffer[0] == 0 && currentLocalStepNumber > 1) {
            currentStep = findPreviousStep(currentStep);
            --currentLocalStepNumber;
            if(!programListEnd) {
              scrollPemBackwards();
            }
          }
        }
        break;
      }

      case cmAssign: {
        if(itemToBeAssigned == 0) {
          if(!tam.alpha) {
            calcMode = previousCalcMode;
          }
          else if(stringByteLength(aimBuffer) != 0) {
            // Delete the last character
            int16_t lg = stringLastGlyph(aimBuffer);
            aimBuffer[lg] = 0;
          }
          else {
            assignLeaveAlpha();
            itemToBeAssigned = ITM_BACKSPACE;
          }
        }
        else {
          if(!tam.alpha) {
            itemToBeAssigned = 0;
          }
          else if(stringByteLength(aimBuffer) != 0) {
            // Delete the last character
            int16_t lg = stringLastGlyph(aimBuffer);
            aimBuffer[lg] = 0;
          }
          else {
            assignLeaveAlpha();
            if(asnKey < kcF1) {
              assignToKey(asnKey);
            }
            else {
              _assignToMenu(asnKey);
            }
            calcMode = previousCalcMode;
            shiftF = shiftG = false;
            refreshScreen();
          }
        }
        break;
      }

      case cmTimerApp: {
        if(lastErrorCode != 0) {
          lastErrorCode = 0;
        }
        else {
          timerAppBackspace();
        }
        break;
      }

      default: {
        sprintf(errorMessage, "In function fnKeyBackspace: unexpected calcMode value (%" PRIu8 ") while processing key BACKSPACE!", (uint8_t)calcMode);
        bugScreen(errorMessage);
      }
    }
  #endif // !TESTSUITE_BUILD
}



void fnKeyUp(uint16_t unusedButMandatoryParameter) {
  #if !defined(TESTSUITE_BUILD)
    if(tamIsWaitingKey()) {
      if(tam.digitsSoFar == 0) {
        tamProcessInput(ITM_1);
        tamProcessInput(ITM_9);
        shiftF = shiftG = false;
        refreshScreen();
      }
      return;
    }
    if(tamIsActive() && !catalog) {
      if(tam.alpha) {
        resetAlphaSelectionBuffer();
        if(currentSoftmenuScrolls()) {
          menuUp();
        }
        else {
          alphaCase = AC_UPPER;
        }
      }
      else {
        addItemToBuffer(ITM_Max);
      }
      return;
    }

    if(softmenu[softmenuStack[0].softmenuId].menuItem == -ITM_MENU) {
      dynamicMenuItem = 18;
      fnProgrammableMenu(NOPARAM);
      return;
    }

    switch(calcMode) {
      case cmNormal:
      case cmAim:
      case cmNim:
      case cmEim:
      case cmPlotStat:
      case cmGraph: {
        resetAlphaSelectionBuffer();
        if(currentSoftmenuScrolls()) {
          menuUp();
        }
        else if((calcMode == cmNormal || calcMode == cmNim) && (numberOfFormulae < 2 || softmenu[softmenuStack[0].softmenuId].menuItem != -MNU_EQN)) {
          screenUpdatingMode = SCRUPD_AUTO;
          if(calcMode == cmNim) {
            closeNim();
          }
          fnBst(NOPARAM);
          lcd_refresh();
        }
        if(softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_PLOT_LR){
          strcpy(plotStatMx, "STATS");
          fnPlotStat(PLOT_NXT);
        }
        else if(softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_EQN) {
          if(currentFormula == 0) {
            currentFormula = numberOfFormulae;
          }
          --currentFormula;
          screenUpdatingMode &= ~SCRUPD_MANUAL_MENU;
        }
        else {
          alphaCase = AC_UPPER;
        }
        break;
      }

      case cmPem: {
        resetAlphaSelectionBuffer();
        if(getSystemFlag(FLAG_ALPHA) && alphaCase == AC_LOWER) {
          alphaCase = AC_UPPER;
          if(softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_alpha_omega || softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_ALPHAintl) {
            softmenuStack[0].softmenuId--; // Switch to the upper case menu
          }
        }
        else if(currentSoftmenuScrolls()) {
          menuUp();
        }
        else if(!getSystemFlag(FLAG_ALPHA)) {
          fnBst(NOPARAM);
        }
        break;
      }

      case cmMim: {
        if(currentSoftmenuScrolls()) {
          menuUp();
        }
        break;
      }

      case cmAssign: {
        if(currentSoftmenuScrolls()) {
          menuUp();
        }
        else if(tam.alpha && alphaCase == AC_LOWER) {
          alphaCase = AC_UPPER;
        }
        else if(tam.alpha && itemToBeAssigned == 0 && aimBuffer[0] == 0) {
          assignLeaveAlpha();
          itemToBeAssigned = ITM_UP;
        }
        else if(tam.alpha && aimBuffer[0] == 0) {
          assignLeaveAlpha();
          if(asnKey < kcF1) {
            assignToKey(asnKey);
          }
          else {
            _assignToMenu(asnKey);
          }
          calcMode = previousCalcMode;
          shiftF = shiftG = false;
          refreshScreen();
        }
        break;
      }

      case cmTimerApp: {
        resetAlphaSelectionBuffer();
        if(currentSoftmenuScrolls()) {
          menuUp();
        }
        else if(!tamIsActive()) {
          timerAppUp();
        }
        break;
      }

      default: {
        sprintf(errorMessage, "In function fnKeyUp: unexpected calcMode value (%" PRIu8 ") while processing key UP!", (uint8_t)calcMode);
        bugScreen(errorMessage);
      }
    }
  #endif // !TESTSUITE_BUILD
}



void fnKeyDown(uint16_t unusedButMandatoryParameter) {
  #if !defined(TESTSUITE_BUILD)
    if(tamIsWaitingKey()) {
      if(tam.digitsSoFar == 0) {
        tamProcessInput(ITM_2);
        tamProcessInput(ITM_0);
        shiftF = shiftG = false;
        refreshScreen();
      }
      return;
    }
    if(tamIsActive() && !catalog) {
      if(tam.alpha) {
        resetAlphaSelectionBuffer();
        if(currentSoftmenuScrolls()) {
          menuDown();
        }
        else {
          alphaCase = AC_LOWER;
        }
      }
      else {
        addItemToBuffer(ITM_Min);
      }
      return;
    }

    if(softmenu[softmenuStack[0].softmenuId].menuItem == -ITM_MENU) {
      dynamicMenuItem = 19;
      fnProgrammableMenu(NOPARAM);
      return;
    }

    switch(calcMode) {
      case cmNormal:
      case cmAim:
      case cmNim:
      case cmEim:
      case cmPlotStat:
      case cmGraph: {
        resetAlphaSelectionBuffer();
        if(currentSoftmenuScrolls()) {
          menuDown();
        }
        else if((calcMode == cmNormal || calcMode == cmNim) && (numberOfFormulae < 2 || softmenu[softmenuStack[0].softmenuId].menuItem != -MNU_EQN)) {
          screenUpdatingMode = SCRUPD_AUTO;
          if(calcMode == cmNim) {
            closeNim();
          }
          fnSst(NOPARAM);
        }
        if(softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_PLOT_LR) {
          strcpy(plotStatMx, "STATS");
          fnPlotStat(PLOT_REV); //REVERSE
        }
        else if(softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_EQN) {
          ++currentFormula;
          if(currentFormula == numberOfFormulae) {
            currentFormula = 0;
          }
          screenUpdatingMode &= ~SCRUPD_MANUAL_MENU;
        }
        else {
          alphaCase = AC_LOWER;
        }
        break;
      }

      case cmPem: {
        resetAlphaSelectionBuffer();
        if(getSystemFlag(FLAG_ALPHA) && alphaCase == AC_UPPER) {
          alphaCase = AC_LOWER;
          if(softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_ALPHA_OMEGA || softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_ALPHAINTL) {
            softmenuStack[0].softmenuId++; // Switch to the lower case menu
          }
        }
        else if(currentSoftmenuScrolls()) {
          menuDown();
        }
        else if(!getSystemFlag(FLAG_ALPHA)) {
          fnSst(NOPARAM);
        }
        break;
      }

      case cmMim: {
        if(currentSoftmenuScrolls()) {
          menuDown();
        }
        break;
      }

      case cmAssign: {
        if(currentSoftmenuScrolls()) {
          menuDown();
        }
        else if(tam.alpha && (itemToBeAssigned == 0 || tam.mode == tmNewMenu) && alphaCase == AC_UPPER) {
          alphaCase = AC_LOWER;
        }
        else if(tam.alpha && itemToBeAssigned == 0 && aimBuffer[0] == 0) {
          assignLeaveAlpha();
          itemToBeAssigned = ITM_DOWN;
        }
        else if(tam.alpha && aimBuffer[0] == 0) {
          assignLeaveAlpha();
          if(asnKey < kcF1) {
            assignToKey(asnKey);
          }
          else {
            _assignToMenu(asnKey);
          }
          calcMode = previousCalcMode;
          shiftF = shiftG = false;
          refreshScreen();
        }
        break;
      }

      case cmTimerApp: {
        resetAlphaSelectionBuffer();
        if(currentSoftmenuScrolls()) {
          menuDown();
        }
        else if(!tamIsActive()) {
          timerAppDown();
        }
        break;
      }

      default: {
        sprintf(errorMessage, "In function fnKeyDown: unexpected calcMode value (%" PRIu8 ") while processing key DOWN!", (uint8_t)calcMode);
        bugScreen(errorMessage);
      }
    }
  #endif // !TESTSUITE_BUILD
}



void fnKeyDotD(uint16_t unusedButMandatoryParameter) {
  #if !defined(TESTSUITE_BUILD)
    switch(calcMode) {
      case cmNormal: {
        if(getSystemFlag(FLAG_FRACT)) {
          clearSystemFlag(FLAG_FRACT);
        }
        else {
          runFunction(ITM_toREAL);
        }
        break;
      }

      case cmNim: {
        addItemToNimBuffer(ITM_dotD);
        break;
      }

      case cmPlotStat:
      case cmGraph:
      case cmMim:
      case cmEim:
      case cmTimerApp: {
        break;
      }

      default: {
        sprintf(errorMessage, "In function fnKeyDotD: unexpected calcMode value (%" PRIu8 ") while processing key .d!", (uint8_t)calcMode);
        bugScreen(errorMessage);
      }
    }
  #endif // !TESTSUITE_BUILD
}



keyCode_t kbKeyCodeFromRowColumn(uint8_t rowColumn) {
  if(rowColumn < 11) {
    return kcNoKey;
  }
  else if(rowColumn < 17) {
    return rowColumn - 10 + 37;
  }
  else if(rowColumn < 21) {
    return kcNoKey;
  }
  else if(rowColumn < 27) {
    return rowColumn - 20 +  0;
  }
  else if(rowColumn < 31) {
    return kcNoKey;
  }
  else if(rowColumn < 37) {
    return rowColumn - 30 +  6;
  }
  else if(rowColumn < 41) {
    return kcNoKey;
  }
  else if(rowColumn < 46) {
    return rowColumn - 40 + 12;
  }
  else if(rowColumn < 51) {
    return kcNoKey;
  }
  else if(rowColumn < 56) {
    return rowColumn - 50 + 17;
  }
  else if(rowColumn < 61) {
    return kcNoKey;
  }
  else if(rowColumn < 66) {
    return rowColumn - 60 + 22;
  }
  else if(rowColumn < 71) {
    return kcNoKey;
  }
  else if(rowColumn < 76) {
    return rowColumn - 70 + 27;
  }
  else if(rowColumn < 81) {
    return kcNoKey;
  }
  else if(rowColumn < 86) {
    return rowColumn - 80 + 32;
  }
  return kcNoKey;
}



uint8_t kbRowColumnFromKeyCode(keyCode_t keyCode) {
  if(keyCode == kcNoKey) {
    return 0;
  }
  else if(keyCode <= 6) {
    return keyCode      + 20;
  }
  else if(keyCode <= 12) {
    return keyCode -  6 + 30;
  }
  else if(keyCode <= 17) {
    return keyCode - 12 + 40;
  }
  else if(keyCode <= 22) {
    return keyCode - 17 + 50;
  }
  else if(keyCode <= 27) {
    return keyCode - 22 + 60;
  }
  else if(keyCode <= 32) {
    return keyCode - 27 + 70;
  }
  else if(keyCode <= 37) {
    return keyCode - 32 + 80;
  }
  else if(keyCode <= 43) {
    return keyCode - 37 + 10; // function keys
  }
  return 0;
}



bool kbCheckForInterrupt(void) {
  _kbCheckForInterrupt = true;
  _kbSeenInterrupt     = false;
  systemProcessEvents();
  _kbCheckForInterrupt = false;
  return _kbSeenInterrupt;
}
