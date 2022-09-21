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

#include "keyboard.h"

#include "assign.h"
#include "bufferize.h"
#include "calcMode.h"
#include "charString.h"
#include "config.h"
#include "constants.h"
#include "debug.h"
#include "error.h"
#include "flags.h"
#include "hal/gui.h"
#include "items.h"
#include "matrix.h"
#include "memory.h"
#include "plotstat.h"
#include "programming/manage.h"
#include "programming/nextStep.h"
#include "programming/programmableMenu.h"
#include "recall.h"
#include "registers.h"
#include "screen.h"
#include "softmenus.h"
#include "solver/equation.h"
#include "solver/graph.h"
#include "sort.h"
#include "stack.h"
#include "stats.h"
#include "timer.h"
#include "ui/tam.h"
#if (REAL34_WIDTH_TEST == 1)
  #include "registerValueConversions.h"
#endif // (REAL34_WIDTH_TEST == 1)
#include <string.h>

#include "wp43.h"

#if !defined(TESTSUITE_BUILD)
  int16_t determineFunctionKeyItem(const char *data) {
    int16_t item = ITM_NOP;

    dynamicMenuItem = -1;

    int16_t itemShift = (shiftF ? 6 : (shiftG ? 12 : 0));
    int16_t fn = *(data) - '0';
    const softmenu_t *sm;
    int16_t row, menuId = softmenuStack[0].softmenuId;
    int16_t firstItem = softmenuStack[0].firstItem;

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
    switch(-softmenu[menuId].menuItem) {
      case MNU_MyMenu: {
        dynamicMenuItem = firstItem + itemShift + (fn - 1);
        item = userMenuItems[dynamicMenuItem].item;
        break;
      }

      case MNU_MyAlpha: {
        dynamicMenuItem = firstItem + itemShift + (fn - 1);
        item = userAlphaItems[dynamicMenuItem].item;
        break;
      }

      case MNU_DYNAMIC: {
        dynamicMenuItem = firstItem + itemShift + (fn - 1);
        item = userMenus[currentUserMenu].menuItem[dynamicMenuItem].item;
        break;
      }

      case MNU_PROG: {
        dynamicMenuItem = firstItem + itemShift + (fn - 1);
        if(tam.function == ITM_GTOP) {
          item = (dynamicMenuItem >= dynamicSoftmenu[menuId].numItems ? ITM_NOP : ITM_GTOP);
        }
        else {
          item = (dynamicMenuItem >= dynamicSoftmenu[menuId].numItems ? ITM_NOP : MNU_DYNAMIC);
        }
        break;
      }

      case MNU_VAR: {
        dynamicMenuItem = firstItem + itemShift + (fn - 1);
        item = (dynamicMenuItem >= dynamicSoftmenu[menuId].numItems ? ITM_NOP : MNU_DYNAMIC);
        break;
      }

      case MNU_MVAR: {
        dynamicMenuItem = firstItem + itemShift + (fn - 1);
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
        dynamicMenuItem = firstItem + itemShift + (fn - 1);
        item = (dynamicMenuItem >= dynamicSoftmenu[menuId].numItems ? ITM_NOP : ITM_RCL);
        break;
      }

      case MNU_RAM:
      case MNU_FLASH: {
        dynamicMenuItem = firstItem + itemShift + (fn - 1);
        item = (dynamicMenuItem >= dynamicSoftmenu[menuId].numItems ? ITM_NOP : ITM_XEQ);
        break;
      }

      case MNU_MENUS: {
        dynamicMenuItem = firstItem + itemShift + (fn - 1);
        item = ITM_NOP;
        if(dynamicMenuItem < dynamicSoftmenu[menuId].numItems) {
          for(uint32_t i = 0; softmenu[i].menuItem < 0; ++i) {
            if(compareString((char *)getNthString(dynamicSoftmenu[menuId].menuContent, dynamicMenuItem), indexOfItems[-softmenu[i].menuItem].itemCatalogName, CMP_NAME) == 0) {
              item = softmenu[i].menuItem;
            }
          }
          for(uint32_t i = 0; i < numberOfUserMenus; ++i) {
            if(compareString((char *)getNthString(dynamicSoftmenu[menuId].menuContent, dynamicMenuItem), userMenus[i].menuName, CMP_NAME) == 0) {
              item = -MNU_DYNAMIC;
              currentUserMenu = i;
            }
          }
        }
        break;
      }

      case ITM_MENU: {
        dynamicMenuItem = firstItem + itemShift + (fn - 1);
        item = ITM_MENU;
        break;
      }

      case MNU_EQN: {
        if(numberOfFormulae == 0 && (firstItem + itemShift + (fn - 1)) > 0) {
          break;
        }
        /* fallthrough */
      }

      default: {
        sm = &softmenu[menuId];
        row = min(3, (sm->numItems + modulo(firstItem - sm->numItems, 6))/6 - firstItem/6) - 1;
        if(itemShift/6 <= row && firstItem + itemShift + (fn - 1) < sm->numItems) {
          item = (sm->softkeyItem)[firstItem + itemShift + (fn - 1)] % 10000;

          if(item == ITM_PROD_SIGN) {
            item = (getSystemFlag(FLAG_MULTx) ? ITM_DOT : ITM_CROSS);
          }
        }
      }
    }
  #pragma GCC diagnostic pop

    if(calcMode == CM_ASSIGN && item != ITM_NOP && item != ITM_NULL) {
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
            displayBugScreen("In function determineFunctionKeyItem: nonexistent menu specified!");
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



  #if defined(PC_BUILD)
    void btnFnClicked(char *data) {
      GdkEvent mouseButton;
      mouseButton.button.button = 1;

      btnFnPressed(&mouseButton, data);
      btnFnReleased(data);
    }
  #endif // PC_BUILD

  #if defined(DMCP_BUILD)
    void btnFnClicked(char *data) {
      btnFnPressed(data);
      btnFnReleased(data);
    }
  #endif // DMCP_BUILD


  void execAutoRepeat(uint16_t key) {
    #if defined(DMCP_BUILD)
      char charKey[6];
      bool_t f = shiftF;
      bool_t g = shiftG;
      uint8_t origScreenUpdatingMode = screenUpdatingMode;
      sprintf(charKey, "%02d", key -1);

      fnTimerStart(TO_AUTO_REPEAT, key, KEY_AUTOREPEAT_PERIOD);

      btnClicked(charKey);
      screenUpdatingMode = origScreenUpdatingMode;
      //btnPressed(charKey);
      shiftF = f;
      shiftG = g;
      refreshLcd();
      lcd_refresh_dma();
    #endif // DMCP_BUILD
  }


  static void _closeCatalog(void) {
    bool_t inCatalog = false;
    for(int i = 0; i < SOFTMENU_STACK_SIZE; ++i) {
      if(softmenu[softmenuStack[i].softmenuId].menuItem == -MNU_CATALOG) {
        inCatalog = true;
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
      addItemToBuffer(item + 26);
      keyActionProcessed = true;
    }

    else if(alphaCase == AC_LOWER && (ITM_ALPHA <= item && item <= ITM_OMEGA)) {
      addItemToBuffer(item + 36);
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

    else if(indexOfItems[item].func == addItemToBuffer) {
      addItemToBuffer(item);
      keyActionProcessed = true;
    }

    if(keyActionProcessed) {
      refreshScreen();
    }
  }


  uint8_t asnKey[4] = {0, 0, 0, 0};


  #if defined(PC_BUILD)
    void btnFnPressed(GdkEvent *event, char *data) {
      if(event->type == GDK_DOUBLE_BUTTON_PRESS || event->type == GDK_TRIPLE_BUTTON_PRESS) { // return unprocessed for double or triple click
        return;
      }
      if(event->button.button == 2) { // Middle click
        shiftF = true;
        shiftG = false;
      }
      if(event->button.button == 3) { // Right click
        shiftF = false;
        shiftG = true;
      }
  #endif // PC_BUILD
  #if defined(DMCP_BUILD)
    void btnFnPressed(char *data) {
  #endif // DMCP_BUILD
    asnKey[0] = ((uint8_t *)data)[0];
    asnKey[1] = 0;

    if(programRunStop == PGM_RUNNING || programRunStop == PGM_PAUSED) {
      setLastKeyCode((*((char *)data) - '0') + 37);
    }
    else {
      lastKeyCode = 0;
    }

    if(programRunStop == PGM_PAUSED) {
      programRunStop = PGM_KEY_PRESSED_WHILE_PAUSED;
      return;
    }
    if(tam.mode == TM_KEY && !tam.keyInputFinished) {
      // not processed here
      return;
    }
    if(calcMode == CM_ASSIGN && itemToBeAssigned != 0 && !(tam.alpha && tam.mode != TM_NEWMENU)) {
      int16_t item = determineFunctionKeyItem((char *)data);

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
      _closeCatalog();
    }
    else if(calcMode != CM_REGISTER_BROWSER && calcMode != CM_FLAG_BROWSER && calcMode != CM_FONT_BROWSER) {
      int16_t item = determineFunctionKeyItem((char *)data);

      if(shiftF || shiftG) {
        screenUpdatingMode &= ~SCRUPD_MANUAL_SHIFT_STATUS;
        clearShiftState();
      }

      shiftF = false;
      shiftG = false;
      if(item != ITM_NOP && item != ITM_NULL) {
        lastErrorCode = 0;

      if(calcMode != CM_ASSIGN && indexOfItems[item].func == addItemToBuffer) {
        // If we are in the catalog then a normal key press should affect the Alpha Selection Buffer to choose
        // an item from the catalog, but a function key press should put the item in the AIM (or TAM) buffer
        // Use this variable to distinguish between the two
        if(calcMode == CM_PEM && !tam.mode) {
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
        if(calcMode == CM_EIM && !tam.mode) {
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



  static bool_t _assignToMenu(uint8_t *data) {
    switch(-softmenu[softmenuStack[0].softmenuId].menuItem) {
      case MNU_MyMenu: {
        assignToMyMenu((*data - '1') + (shiftG ? 12 : shiftF ? 6 : 0));
        calcMode = previousCalcMode;
        shiftF = shiftG = false;
        _closeCatalog();
        refreshScreen();
        screenUpdatingMode &= ~SCRUPD_ONE_TIME_FLAGS;
        return true;
      }
      case MNU_MyAlpha: {
        assignToMyAlpha((*data - '1') + (shiftG ? 12 : shiftF ? 6 : 0));
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
          #if defined(PC_BUILD)
            moreInfoOnError("In function btnFnReleased:", "cannot assign submenu", indexOfItems[-itemToBeAssigned].itemCatalogName, "in user-created menu.");
          #endif // PC_BUILD
        }
        else {
          assignToUserMenu((*data - '1') + (shiftG ? 12 : shiftF ? 6 : 0));
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
        #if defined(PC_BUILD)
          moreInfoOnError("In function btnFnReleased:", "the menu", indexOfItems[-softmenu[softmenuStack[0].softmenuId].menuItem].itemCatalogName, "is write-protected.");
        #endif // PC_BUILD
        calcMode = previousCalcMode;
        shiftF = shiftG = false;
        _closeCatalog();
        refreshScreen();
        screenUpdatingMode &= ~SCRUPD_ONE_TIME_FLAGS;
        return true;
      }
    }
  }

  void btnFnReleased(char *data) {
    if(programRunStop == PGM_KEY_PRESSED_WHILE_PAUSED) {
      programRunStop = PGM_RESUMING;
      screenUpdatingMode &= ~SCRUPD_ONE_TIME_FLAGS;
      return;
    }
    if(calcMode != CM_REGISTER_BROWSER && calcMode != CM_FLAG_BROWSER && calcMode != CM_FONT_BROWSER) {
      if(tam.mode == TM_KEY && !tam.keyInputFinished) {
        if(tam.digitsSoFar == 0) {
          switch(data[0]) {
            case '1': {
              tamProcessInput(shiftG ? ITM_1 :                  ITM_0);
              tamProcessInput(shiftG ? ITM_3 : shiftF ? ITM_7 : ITM_1);
              break;
            }
            case '2': {
              tamProcessInput(shiftG ? ITM_1 :                  ITM_0);
              tamProcessInput(shiftG ? ITM_4 : shiftF ? ITM_8 : ITM_2);
              break;
            }
            case '3': {
              tamProcessInput(shiftG ? ITM_1 :                  ITM_0);
              tamProcessInput(shiftG ? ITM_5 : shiftF ? ITM_9 : ITM_3);
              break;
            }
            case '4': {
              tamProcessInput(     (shiftG || shiftF) ? ITM_1 : ITM_0);
              tamProcessInput(shiftG ? ITM_6 : shiftF ? ITM_0 : ITM_4);
              break;
            }
            case '5': {
              tamProcessInput(     (shiftG || shiftF) ? ITM_1 : ITM_0);
              tamProcessInput(shiftG ? ITM_7 : shiftF ? ITM_1 : ITM_5);
              break;
            }
            case '6': {
              tamProcessInput(     (shiftG || shiftF) ? ITM_1 : ITM_0);
              tamProcessInput(shiftG ? ITM_8 : shiftF ? ITM_2 : ITM_6);
              break;
            }
          }
          shiftF = shiftG = false;
          refreshScreen();
        }
        screenUpdatingMode &= ~SCRUPD_ONE_TIME_FLAGS;
        return;
      }

      if(calcMode == CM_ASSIGN && itemToBeAssigned != 0 && !(tam.alpha && tam.mode != TM_NEWMENU)) {
        if(_assignToMenu((uint8_t *)data)) {
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

        if(calcMode != CM_CONFIRMATION) {
          lastErrorCode = 0;

          if(calcMode != CM_PEM && item == -MNU_Sfdx) {
            tamEnterMode(MNU_Sfdx);
            refreshScreen();
            screenUpdatingMode &= ~SCRUPD_ONE_TIME_FLAGS;
            return;
          }
          else if(calcMode != CM_PEM && item == ITM_INTEGRAL) {
            reallyRunFunction(item, currentSolverVariable);
            refreshScreen();
            screenUpdatingMode &= ~SCRUPD_ONE_TIME_FLAGS;
            return;
          }
          else if(item < 0) { // softmenu
            if(calcMode == CM_ASSIGN && itemToBeAssigned == 0 && softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_MENUS) {
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
          if(tam.mode && catalog && (tam.digitsSoFar || tam.function == ITM_BESTF || tam.function == ITM_CNST || (!tam.indirect && (tam.mode == TM_VALUE || tam.mode == TM_VALUE_CHB || (tam.mode == TM_KEY && !tam.keyInputFinished))))) {
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
          else if(calcMode == CM_PEM && catalog && catalog != CATALOG_MVAR) { // TODO: is that correct
            if(indexOfItems[item].func == fnGetSystemFlag && (tam.mode == TM_FLAGR || tam.mode == TM_FLAGW) && !tam.indirect) {
              tam.value = (indexOfItems[item].param & 0xff);
              tam.alpha = true;
              addStepInProgram(tamOperation());
              tamLeaveMode();
              hourGlassIconEnabled = false;
            }
            else if(tam.mode) {
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
          if(tam.mode && catalog && (tam.digitsSoFar || tam.function == ITM_BESTF || tam.function == ITM_CNST || (!tam.indirect && (tam.mode == TM_VALUE || tam.mode == TM_VALUE_CHB)))) {
            // disabled
          }
          else if(tam.mode && (!tam.alpha || isAlphabeticSoftmenu())) {
            addItemToBuffer(item);
          }
          else if((calcMode == CM_NORMAL || calcMode == CM_AIM) && isAlphabeticSoftmenu()) {
            if(calcMode == CM_NORMAL) {
              fnAim(NOPARAM);
            }
            addItemToBuffer(item);
          }
          else if(calcMode == CM_EIM && catalog && catalog != CATALOG_MVAR) {
            addItemToBuffer(item);
            while(softmenu[softmenuStack[0].softmenuId].menuItem != -MNU_EQ_EDIT) {
              popSoftmenu();
            }
          }
          else if((calcMode == CM_NORMAL || calcMode == CM_NIM) && (ITM_0<=item && item<=ITM_F) && (!catalog || catalog == CATALOG_MVAR)) {
            addItemToNimBuffer(item);
          }
          else if(calcMode == CM_MIM && softmenu[softmenuStack[0].softmenuId].menuItem != -MNU_M_EDIT) {
            addItemToBuffer(item);
          }
          else if(item > 0) { // function
            if(calcMode == CM_NIM && item != ITM_CC) {
              closeNim();
              if(calcMode != CM_NIM) {
                if(indexOfItems[item].func == fnConstant) {
                  setSystemFlag(FLAG_ASLIFT);
                }
              }
            }
            if(calcMode == CM_AIM && !isAlphabeticSoftmenu()) {
              closeAim();
            }
            if(tam.alpha && calcMode != CM_ASSIGN && tam.mode != TM_NEWMENU) {
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
              if(calcMode == CM_ASSIGN && itemToBeAssigned == 0 && item != ITM_NOP) {
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
              else if(calcMode == CM_ASSIGN && tam.alpha && tam.mode != TM_NEWMENU && item != ITM_NOP) {
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



  int16_t determineItem(const char *data) {
    int16_t result;
    const calcKey_t *key;

    dynamicMenuItem = -1;
    key = getSystemFlag(FLAG_USER) ? (kbd_usr + (*data - '0')*10 + *(data+1) - '0') : (kbd_std + (*data - '0')*10 + *(data+1) - '0');

    // Shift f pressed and shift g not active
    if(key->primary == ITM_SHIFTf && !shiftG && (calcMode == CM_NORMAL || calcMode == CM_AIM || calcMode == CM_NIM || calcMode == CM_MIM || calcMode == CM_EIM || calcMode == CM_PEM || calcMode == CM_PLOT_STAT || calcMode == CM_GRAPH || calcMode == CM_ASSIGN)) {
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
    else if(key->primary == ITM_SHIFTg && !shiftF && (calcMode == CM_NORMAL || calcMode == CM_AIM || calcMode == CM_NIM || calcMode == CM_MIM || calcMode == CM_EIM || calcMode == CM_PEM || calcMode == CM_PLOT_STAT || calcMode == CM_GRAPH || calcMode == CM_ASSIGN)) {
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

    if(calcMode == CM_AIM || (catalog && catalog != CATALOG_MVAR && calcMode != CM_NIM) || calcMode == CM_EIM || tam.alpha || (calcMode == CM_ASSIGN && (previousCalcMode == CM_AIM || previousCalcMode == CM_EIM)) || (calcMode == CM_PEM && getSystemFlag(FLAG_ALPHA))) {
      result = shiftF ? key->fShiftedAim :
               shiftG ? key->gShiftedAim :
                        key->primaryAim;

    }
    else if(tam.mode) {
      result = key->primaryTam; // No shifted function in TAM
    }
    else if(calcMode == CM_NORMAL || calcMode == CM_NIM || calcMode == CM_MIM || calcMode == CM_FONT_BROWSER || calcMode == CM_FLAG_BROWSER || calcMode == CM_REGISTER_BROWSER || calcMode == CM_BUG_ON_SCREEN || calcMode == CM_CONFIRMATION || calcMode == CM_PEM || calcMode == CM_PLOT_STAT || calcMode == CM_GRAPH || calcMode == CM_ASSIGN || calcMode == CM_TIMER) {
      result = shiftF ? key->fShifted :
               shiftG ? key->gShifted :
                        key->primary;
    }
    else {
      displayBugScreen("In function determineItem: item was not determined!");
      result = 0;
    }

    if(result == ITM_PROD_SIGN) {
      result = (getSystemFlag(FLAG_MULTx) ? ITM_CROSS : ITM_DOT);
    }

    if(shiftF || shiftG) {
      screenUpdatingMode &= ~SCRUPD_MANUAL_SHIFT_STATUS;
      clearShiftState();
    }

    shiftF = false;
    shiftG = false;

    if(calcMode == CM_ASSIGN && itemToBeAssigned != 0 && (result == ITM_NOP || result == ITM_NULL)) {
      result = ITM_LBL;
    }

    return result;
  }



  #if defined(PC_BUILD)
    void btnClicked(char *data) {
      GdkEvent mouseButton;
      mouseButton.button.button = 1;
      mouseButton.type = 0;

      btnPressed(&mouseButton, data);
      btnReleased(data);
  }
  #endif // PC_BUILD

  #if defined(DMCP_BUILD)
    void btnClicked(char *data) {
      btnPressed(data);
      btnReleased(data);
    }
  #endif // DMCP_BUILD

  #if defined(PC_BUILD)
    char key[3] = {0, 0, 0};
  #endif

  #if defined(PC_BUILD)
    void btnPressed(GdkEvent *event, char *data) {
  #elif defined(DMCP_BUILD)
    void btnPressed(char *data) {
  #endif
    int keyCode = (*data - '0')*10 + *(data + 1) - '0';

    asnKey[0] = ((uint8_t *)data)[0];
    asnKey[1] = ((uint8_t *)data)[1];
    asnKey[2] = 0;

    if(programRunStop == PGM_RUNNING || programRunStop == PGM_PAUSED) {
      #if defined(PC_BUILD)
        setLastKeyCode(keyCode + 1);
      #elif defined(DMCP_BUILD)
        lastKeyCode = keyCode;
      #endif
    }
    else {
      lastKeyCode = 0;
    }

    #if defined(PC_BUILD)
      if(event->type == GDK_DOUBLE_BUTTON_PRESS || event->type == GDK_TRIPLE_BUTTON_PRESS) { // return unprocessed for double or triple click
        return;
      }
      if(event->button.button == 2) { // Middle click
        shiftF = true;
        shiftG = false;
      }
      if(event->button.button == 3) { // Right click
        shiftF = false;
        shiftG = true;
      }
    #endif

    bool_t f = shiftF;
    bool_t g = shiftG;
    #if defined(DMCP_BUILD)
      //if(keyAutoRepeat) {
      //  //beep(880, 50);
      //  item = previousItem;
      //}
      //else {
    #endif
    int16_t item = determineItem(data);
    #if defined(DMCP_BUILD)
      //  previousItem = item;
      //}
    #endif

    #if defined(PC_BUILD)
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
    #elif defined(DMCP_BUILD)
      if(calcMode == CM_PEM && (item == ITM_SST || item == ITM_BST)) {
        shiftF = f;
        shiftG = g;
      }
    #endif

    if(getSystemFlag(FLAG_USER)) {
      int keyStateCode = (getSystemFlag(FLAG_ALPHA) ? 3 : 0) + (g ? 2 : f ? 1 : 0);
      char *funcParam = (char *)getNthString((uint8_t *)userKeyLabel, keyCode * 6 + keyStateCode);
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
    if(calcMode == CM_ASSIGN && itemToBeAssigned != 0 && tamBuffer[0] == 0) {
      shiftF = f;
      shiftG = g;
    }
  }



  void btnReleased(char *data) {
    int16_t item;

    if(programRunStop == PGM_KEY_PRESSED_WHILE_PAUSED) {
      programRunStop = PGM_RESUMING;
      screenUpdatingMode &= ~SCRUPD_ONE_TIME_FLAGS;
      return;
    }

    if(calcMode == CM_ASSIGN && itemToBeAssigned != 0 && tamBuffer[0] == 0) {
      assignToKey(data);
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
        int keyCode = (*data - '0')*10 + *(data + 1) - '0';
        int keyStateCode = (getSystemFlag(FLAG_ALPHA) ? 3 : 0) + (shiftG ? 2 : shiftF ? 1 : 0);
        char *funcParam = (char *)getNthString((uint8_t *)userKeyLabel, keyCode * 6 + keyStateCode);

        #if defined(PC_BUILD)
          if(item == ITM_RS || item == ITM_XEQ) {
            key[0] = 0;
          }
        #endif // PC_BUILD

        if(item != ITM_NOP && tam.alpha && indexOfItems[item].func != addItemToBuffer) {
          // We are in TAM mode so need to cancel first (equivalent to EXIT)
          tamLeaveMode();
        }
        if(item == ITM_RCL && getSystemFlag(FLAG_USER) && funcParam[0] != 0) {
          calcRegister_t var = findNamedVariable(funcParam);
          if(var != INVALID_VARIABLE) {
            reallyRunFunction(item, var);
          }
          else {
            displayCalcErrorMessage(ERROR_UNDEF_SOURCE_VAR, ERR_REGISTER_LINE, REGISTER_X);
            #if (EXTRA_INFO_ON_CALC_ERROR == 1)
              sprintf(errorMessage, "string '%s' is not a named variable", funcParam);
              moreInfoOnError("In function btnReleased:", errorMessage, NULL, NULL);
            #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
          }
        }
        else if(item == ITM_XEQ && getSystemFlag(FLAG_USER) && funcParam[0] != 0) {
          calcRegister_t label = findNamedLabel(funcParam);
          if(label != INVALID_VARIABLE) {
            reallyRunFunction(item, label);
          }
          else {
            displayCalcErrorMessage(ERROR_LABEL_NOT_FOUND, ERR_REGISTER_LINE, REGISTER_X);
            #if (EXTRA_INFO_ON_CALC_ERROR == 1)
              sprintf(errorMessage, "string '%s' is not a named label", funcParam);
              moreInfoOnError("In function btnReleased:", errorMessage, NULL, NULL);
            #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
          }
        }
        else {
          runFunction(item);
        }
      }
    }
    //#if defined(DMCP_BUILD)
    //  else if(keyAutoRepeat) {
    //    btnPressed(data);
    //  }
    //#endif // DMCP_BUILD
    if(fnTimerGetStatus(TO_AUTO_REPEAT) != TMR_RUNNING) {
      refreshScreen();
    }
    screenUpdatingMode &= ~SCRUPD_ONE_TIME_FLAGS;
  }



  void leavePem(void) {
    if(freeProgramBytes >= 4) { // Push the programs to the end of RAM
      uint32_t newProgramSize = (uint32_t)((uint8_t *)(ram + RAM_SIZE) - beginOfProgramMemory) - (freeProgramBytes & 0xfffc);
      uint16_t localStepNumber = currentLocalStepNumber;
      uint16_t programNumber = currentProgramNumber;
      uint16_t fdLocalStepNumber = firstDisplayedLocalStepNumber;
      bool_t inRam = (programList[currentProgramNumber - 1].step > 0);
      if(inRam) {
        currentStep.ram           += (freeProgramBytes & 0xfffc);
        firstDisplayedStep.ram    += (freeProgramBytes & 0xfffc);
        beginOfCurrentProgram.ram += (freeProgramBytes & 0xfffc);
        endOfCurrentProgram.ram   += (freeProgramBytes & 0xfffc);
      }
      freeProgramBytes &= 0x03;
      resizeProgramMemory(TO_BLOCKS(newProgramSize));
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
        if(currentSoftmenuScrolls() || calcMode != CM_NORMAL || temporaryInformation != TI_NO_INFO) {
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
        if(currentSoftmenuScrolls() || calcMode != CM_NORMAL || temporaryInformation != TI_NO_INFO) {
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
        if(calcMode == CM_ASSIGN) {
          if(itemToBeAssigned == 0) {
            itemToBeAssigned = item;
          }
          else {
            tamBuffer[0] = 0;
          }
          keyActionProcessed = true;
        }
        else if(calcMode == CM_REGISTER_BROWSER || calcMode == CM_FLAG_BROWSER || calcMode == CM_FONT_BROWSER || calcMode == CM_TIMER) {
          keyActionProcessed = true;
        }
        else if(calcMode == CM_PEM && item == ITM_dotD && aimBuffer[0] == 0) {
          addStepInProgram(ITM_toREAL);
          keyActionProcessed = true;
        }
        break;
      }

      case ITM_ENTER: {
        if(calcMode == CM_ASSIGN) {
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
            if(tam.alpha && tam.mode != TM_NEWMENU) {
              assignLeaveAlpha();
              assignGetName2();
            }
            else if(tam.alpha) {
              tamBuffer[0] = 0;
            }
          }
          keyActionProcessed = true;
        }
        else if(calcMode == CM_REGISTER_BROWSER || calcMode == CM_FLAG_BROWSER || calcMode == CM_FONT_BROWSER) {
          keyActionProcessed = true;
        }
        else if(tam.mode) {
          tamProcessInput(ITM_ENTER);
          keyActionProcessed = true;
        }
        break;
      }

      default: {
        if(calcMode == CM_ASSIGN && itemToBeAssigned != 0 && item == ITM_USERMODE) {
          while(softmenuStack[0].softmenuId > 1) {
            popSoftmenu();
          }
          if(previousCalcMode == CM_AIM) {
            softmenuStack[0].softmenuId = 1;
            calcModeAimGui();
          }
          else {
            leaveAsmMode();
          }
          keyActionProcessed = true;
        }
        else if(calcMode == CM_ASSIGN && itemToBeAssigned == 0 && item == ITM_USERMODE) {
          tamEnterMode(ITM_ASSIGN);
          calcMode = previousCalcMode;
          keyActionProcessed = true;
        }
        else if(calcMode == CM_ASSIGN && item == ITM_AIM) {
          assignEnterAlpha();
          keyActionProcessed = true;
        }
        else if((calcMode != CM_PEM || !getSystemFlag(FLAG_ALPHA)) && catalog && catalog != CATALOG_MVAR) {
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
        else if(tam.mode) {
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
            case CM_NORMAL: {
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

            case CM_AIM: {
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

            case CM_EIM: {
              processAimInput(item);
              break;
            }

            case CM_NIM: {
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

            case CM_MIM: {
              addItemToBuffer(item);
              keyActionProcessed = true;
              break;
            }

            case CM_REGISTER_BROWSER: {
              if(item == ITM_PERIOD) {
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
              }
              else if(item == ITM_RS) {
                rbr1stDigit = true;
                showContent = !showContent;
              }
              else if(item == ITM_RCL) {
                rbr1stDigit = true;
                if(rbrMode == RBR_GLOBAL || rbrMode == RBR_LOCAL) {
                  calcMode = previousCalcMode;
                  fnRecall(currentRegisterBrowserScreen);
                  setSystemFlag(FLAG_ASLIFT);
                }
                else if(rbrMode == RBR_NAMED) {
                }
              }
              else if(ITM_0 <= item && item <= ITM_9 && (rbrMode == RBR_GLOBAL || rbrMode == RBR_LOCAL)) {
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
              }

              keyActionProcessed = true;
              break;
            }

            case CM_FLAG_BROWSER:
            case CM_FONT_BROWSER:
            case CM_ERROR_MESSAGE:
            case CM_BUG_ON_SCREEN: {
              keyActionProcessed = true;
              break;
            }

            case CM_GRAPH:
            case CM_PLOT_STAT: {
              break;
            }

            case CM_CONFIRMATION: {
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

            case CM_PEM: {
              if(item == ITM_PR) {
                leavePem();
                calcModeNormal();
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

            case CM_ASSIGN: {
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
                if(tam.alpha && tam.mode != TM_NEWMENU) {
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

            case CM_TIMER: {
              printf("ITEM: %d\n", item);
              switch(item) {
                case ITM_RS: {
                  fnStartStopTimerApp();
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
                  fnDigitKeyTimerApp(item - ITM_0);
                  break;
                }
                case ITM_PERIOD: {
                  fnDotTimerApp();
                  break;
                }
                case ITM_ADD: {
                  fnPlusTimerApp();
                  break;
                }
                case ITM_RCL: {
                  runFunction(ITM_TIMER_RCL);
                  break;
                }
              }
              keyActionProcessed = true;
              break;
            }

            default: {
              sprintf(errorMessage, "In function processKeyAction: %" PRIu8 " is an unexpected value while processing calcMode!", calcMode);
              displayBugScreen(errorMessage);
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
      case CM_NORMAL: {
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

      case CM_AIM: {
        calcModeNormal();
        popSoftmenu();

        if(aimBuffer[0] == 0) {
          #if defined(DEBUGUNDO)
            printf(">>> undo from fnKeyEnter\n");
          #endif // DEBUGUNDO
          undo();
        }
        else {
          int16_t len = stringByteLength(aimBuffer) + 1;

          reallocateRegister(REGISTER_X, dtString, TO_BLOCKS(len), amNone);
          xcopy(REGISTER_STRING_DATA(REGISTER_X), aimBuffer, len);

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

      case CM_MIM: {
        mimEnter(false);
        break;
      }

      case CM_NIM: {
        closeNim();

        if(calcMode != CM_NIM && lastErrorCode == 0) {
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

      case CM_EIM: {
        if(aimBuffer[0] != 0) {
          setEquation(currentFormula, aimBuffer);
        }
        if(softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_EQ_EDIT) {
          calcModeNormal();
          if(allFormulae[currentFormula].pointerToFormulaData == WP43_NULL) {
            deleteEquation(currentFormula);
          }
        }
        popSoftmenu();
        break;
      }

      case CM_REGISTER_BROWSER:
      case CM_FLAG_BROWSER:
      case CM_FONT_BROWSER:
      case CM_ERROR_MESSAGE:
      case CM_BUG_ON_SCREEN:
      case CM_PLOT_STAT:
      case CM_GRAPH: {
        break;
      }

      case CM_TIMER: {
        fnEnterTimerApp();
        break;
      }

      case CM_CONFIRMATION: {
        calcMode = previousCalcMode;
        confirmedFunction(CONFIRMED);
        break;
      }

      default: {
        sprintf(errorMessage, "In function fnKeyEnter: unexpected calcMode value (%" PRIu8 ") while processing key ENTER!", calcMode);
        displayBugScreen(errorMessage);
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
    if(tam.mode == TM_KEY && !tam.keyInputFinished) {
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
      case CM_REGISTER_BROWSER:
      case CM_FLAG_BROWSER:
      case CM_FONT_BROWSER:
      case CM_CONFIRMATION:
      case CM_ERROR_MESSAGE:
      case CM_BUG_ON_SCREEN: {
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
            if(tam.mode) {
              numberOfTamMenusToPop--;
            }
          }
          return;
        }
      }
    }

    if(tam.mode) {
      if(numberOfTamMenusToPop > 1) {
        popSoftmenu();
        numberOfTamMenusToPop--;
      }
      else {
        if(calcMode == CM_PEM) {
          aimBuffer[0] = 0;
        }
        tamLeaveMode();
        if(calcMode == CM_PEM) {
          scrollPemBackwards();
        }
      }
      return;
    }

    switch(calcMode) {
      case CM_REGISTER_BROWSER:
      case CM_FLAG_BROWSER:
      case CM_FONT_BROWSER:
      case CM_CONFIRMATION:
      case CM_ERROR_MESSAGE:
      case CM_BUG_ON_SCREEN: {
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
      case CM_NORMAL: {
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

      case CM_AIM: {
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

      case CM_NIM: {
        addItemToNimBuffer(ITM_EXIT);
        updateMatrixHeightCache();
        break;
      }

      case CM_MIM: {
        if(softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_M_EDIT) {
          mimEnter(true);
          if(matrixIndex == findNamedVariable(statMx)) {
            calcSigma(0);
          }
          mimFinalize();
          calcModeNormal();
          updateMatrixHeightCache();
        }
        screenUpdatingMode = SCRUPD_AUTO;
        popSoftmenu(); // close softmenu dedicated for the MIM
        break;
      }

      case CM_PEM: {
        if(getSystemFlag(FLAG_ALPHA) && aimBuffer[0] == 0 && !tam.mode) {
          pemAlpha(ITM_BACKSPACE);
          fnBst(NOPARAM); // Set the PGM pointer to the original position
          break;
        }
        if(aimBuffer[0] != 0 && !tam.mode) {
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
        calcModeNormal();
        #if defined(DEBUGUNDO)
          printf(">>> saveForUndo from fnKeyExitB\n");
        #endif // DEBUGUNDO
        saveForUndo();
        if(lastErrorCode == ERROR_RAM_FULL) {
          goto undo_disabled;
        }
        break;
      }

      case CM_EIM: {
        if(lastErrorCode != 0) {
          lastErrorCode = 0;
        }
        else {
          if(softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_EQ_EDIT) {
            calcModeNormal();
            if(allFormulae[currentFormula].pointerToFormulaData == WP43_NULL) {
              deleteEquation(currentFormula);
            }
          }
          popSoftmenu();
        }
        break;
      }

      case CM_REGISTER_BROWSER:
      case CM_FLAG_BROWSER:
      case CM_FONT_BROWSER: {
        rbr1stDigit = true;
        calcMode = previousCalcMode;
        break;
      }

      case CM_TIMER: {
        screenUpdatingMode = SCRUPD_AUTO;
        if(lastErrorCode != 0) {
          lastErrorCode = 0;
        }
        else {
          fnLeaveTimerApp();
        }
        break;
      }

      case CM_BUG_ON_SCREEN: {
        calcMode = previousCalcMode;
        break;
      }

      case CM_GRAPH:
      case CM_PLOT_STAT: {
        //Temporary - TODO JM
        restoreStats();
        printf(">>> ####@@@@ D %i\n",calcMode);
        if(lastPlotMode == H_PLOT && calcMode == CM_PLOT_STAT) {
          popSoftmenu();
        }
        //TODO
        lastPlotMode = PLOT_NOTHING;
        plotSelection = 0;
        calcMode = CM_NORMAL;
        #if defined(DEBUGUNDO)
          printf(">>> Undo from fnKeyExit\n");
        #endif // DEBUGUNDO
        fnUndo(NOPARAM);
        fnClDrawMx();
        popSoftmenu();
        break;
      }

      case CM_CONFIRMATION: {
        calcMode = previousCalcMode;
        temporaryInformation = TI_NO_INFO;
        if(programRunStop == PGM_WAITING) {
          programRunStop = PGM_STOPPED;
        }
        break;
      }

      case CM_ASSIGN: {
        if((softmenuStack[0].softmenuId <= 1 && softmenuStack[1].softmenuId <= 1) || (previousCalcMode == CM_EIM && softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_EQ_EDIT)) { // MyMenu or MyAlpha is displayed
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
        sprintf(errorMessage, "In function fnKeyExit: unexpected calcMode value (%" PRIu8 ") while processing key EXIT!", calcMode);
        displayBugScreen(errorMessage);
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
      case CM_NORMAL: {
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
          #if (EXTRA_INFO_ON_CALC_ERROR == 1)
            sprintf(errorMessage, "You cannot use CC with %s in X and %s in Y!", getDataTypeName(getRegisterDataType(REGISTER_X), true, false), getDataTypeName(getRegisterDataType(REGISTER_Y), true, false));
            moreInfoOnError("In function fnKeyCC:", errorMessage, NULL, NULL);
          #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
        }
        break;
      }

      case CM_NIM: {
        addItemToNimBuffer(ITM_CC);
        break;
      }

      case CM_MIM: {
        mimAddNumber(ITM_CC);
        break;
      }

      case CM_PEM: {
        if(aimBuffer[0] != 0 && !getSystemFlag(FLAG_ALPHA)) {
          pemAddNumber(ITM_CC);
        }
        break;
      }

      case CM_EIM:
      case CM_REGISTER_BROWSER:
      case CM_FLAG_BROWSER:
      case CM_FONT_BROWSER:
      case CM_PLOT_STAT:
      case CM_TIMER:
      case CM_GRAPH: {
        break;
      }

      default: {
        sprintf(errorMessage, "In function fnKeyCC: unexpected calcMode value (%" PRIu8 ") while processing key CC!", calcMode);
        displayBugScreen(errorMessage);
      }
    }
  #endif // !TESTSUITE_BUILD
}



void fnKeyBackspace(uint16_t unusedButMandatoryParameter) {
  #if !defined(TESTSUITE_BUILD)
    uint16_t lg;
    uint8_t *nextStep;

    if(tam.mode) {
      tamProcessInput(ITM_BACKSPACE);
      return;
    }

    switch(calcMode) {
      case CM_NORMAL: {
        if(lastErrorCode != 0) {
          lastErrorCode = 0;
        }
        else {
          runFunction(ITM_CLX);
        }
        break;
      }

      case CM_AIM: {
        if(catalog && catalog != CATALOG_MVAR) {
          if(stringByteLength(aimBuffer) > 0) {
            lg = stringLastGlyph(aimBuffer);
            aimBuffer[lg] = 0;
            xCursor = showString(aimBuffer, &standardFont, 1, Y_POSITION_OF_AIM_LINE + 6, vmNormal, true, true);
          }
        }
        else if(stringByteLength(aimBuffer) > 0) {
          lg = stringLastGlyph(aimBuffer);
          aimBuffer[lg] = 0;
        }
        break;
      }

      case CM_NIM: {
        addItemToNimBuffer(ITM_BACKSPACE);
        break;
      }

      case CM_MIM: {
        mimAddNumber(ITM_BACKSPACE);
        break;
      }

      case CM_EIM: {
        if(xCursor > 0) {
          char *srcPos = aimBuffer;
          char *dstPos = aimBuffer;
          char *lstPos = aimBuffer + stringNextGlyph(aimBuffer, stringLastGlyph(aimBuffer));
          --xCursor;
          for(uint32_t i = 0; i < xCursor; ++i) {
            dstPos += (*dstPos & 0x80) ? 2 : 1;
          }
          srcPos = dstPos + ((*dstPos & 0x80) ? 2 : 1);
          for(; srcPos <= lstPos;) {
            *(dstPos++) = *(srcPos++);
          }
        }
        break;
      }

      //case CM_ASM_OVER_NORMAL: {
      //  addItemToBuffer(ITM_BACKSPACE);
      //  break;
      //}

      case CM_REGISTER_BROWSER:
      case CM_FLAG_BROWSER:
      case CM_FONT_BROWSER: {
        calcMode = previousCalcMode;
        break;
      }

      case CM_BUG_ON_SCREEN:
      case CM_PLOT_STAT:
      case CM_GRAPH: {
        calcMode = previousCalcMode;
        break;
      }

      case CM_CONFIRMATION: {
        calcMode = previousCalcMode;
        temporaryInformation = TI_NO_INFO;
        if(programRunStop == PGM_WAITING) {
          programRunStop = PGM_STOPPED;
        }
        break;
      }

      case CM_PEM: {
        if(programList[currentProgramNumber - 1].step < 0) {
          // attempt to modify a program in the flash memory
          displayCalcErrorMessage(ERROR_FLASH_MEMORY_WRITE_PROTECTED, ERR_REGISTER_LINE, REGISTER_X);
          return;
        }
        if(getSystemFlag(FLAG_ALPHA)) {
          pemAlpha(ITM_BACKSPACE);
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

      case CM_ASSIGN: {
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
            if(asnKey[1] != 0) {
              assignToKey((char *)asnKey);
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

      case CM_TIMER: {
        if(lastErrorCode != 0) {
          lastErrorCode = 0;
        }
        else {
          fnBackspaceTimerApp();
        }
        break;
      }

      default: {
        sprintf(errorMessage, "In function fnKeyBackspace: unexpected calcMode value (%" PRIu8 ") while processing key BACKSPACE!", calcMode);
        displayBugScreen(errorMessage);
      }
    }
  #endif // !TESTSUITE_BUILD
}



void fnKeyUp(uint16_t unusedButMandatoryParameter) {
  #if !defined(TESTSUITE_BUILD)
    if(tam.mode == TM_KEY && !tam.keyInputFinished) {
      if(tam.digitsSoFar == 0) {
        tamProcessInput(ITM_1);
        tamProcessInput(ITM_9);
        shiftF = shiftG = false;
        refreshScreen();
      }
      return;
    }
    if(tam.mode && !catalog) {
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
      case CM_NORMAL:
      case CM_AIM:
      case CM_NIM:
      case CM_EIM:
      case CM_PLOT_STAT:
      case CM_GRAPH: {
        resetAlphaSelectionBuffer();
        if(currentSoftmenuScrolls()) {
          menuUp();
        }
        else if((calcMode == CM_NORMAL || calcMode == CM_NIM) && (numberOfFormulae < 2 || softmenu[softmenuStack[0].softmenuId].menuItem != -MNU_EQN)) {
          screenUpdatingMode = SCRUPD_AUTO;
          if(calcMode == CM_NIM) {
            closeNim();
          }
          fnBst(NOPARAM);
          #if defined(DMCP_BUILD)
            lcd_refresh();
          #else // !DMCP_BUILD
            refreshLcd();
          #endif // DMCP_BUILD
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

      case CM_REGISTER_BROWSER: {
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
          displayBugScreen(errorMessage);
        }
        break;
      }

      case CM_FLAG_BROWSER: {
        currentFlgScr = 3 - currentFlgScr;
        break;
      }

      case CM_FONT_BROWSER: {
        if(currentFntScr >= 2) {
          currentFntScr--;
        }
        break;
      }

      case CM_PEM: {
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

      case CM_MIM: {
        if(currentSoftmenuScrolls()) {
          menuUp();
        }
        break;
      }

      case CM_ASSIGN: {
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
          if(asnKey[1] != 0) {
            assignToKey((char *)asnKey);
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

      case CM_TIMER: {
        fnUpTimerApp();
        break;
      }

      default: {
        sprintf(errorMessage, "In function fnKeyUp: unexpected calcMode value (%" PRIu8 ") while processing key UP!", calcMode);
        displayBugScreen(errorMessage);
      }
    }
  #endif // !TESTSUITE_BUILD
}



void fnKeyDown(uint16_t unusedButMandatoryParameter) {
  #if !defined(TESTSUITE_BUILD)
    if(tam.mode == TM_KEY && !tam.keyInputFinished) {
      if(tam.digitsSoFar == 0) {
        tamProcessInput(ITM_2);
        tamProcessInput(ITM_0);
        shiftF = shiftG = false;
        refreshScreen();
      }
      return;
    }
    if(tam.mode && !catalog) {
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
      case CM_NORMAL:
      case CM_AIM:
      case CM_NIM:
      case CM_EIM:
      case CM_PLOT_STAT:
      case CM_GRAPH: {
        resetAlphaSelectionBuffer();
        if(currentSoftmenuScrolls()) {
          menuDown();
        }
        else if((calcMode == CM_NORMAL || calcMode == CM_NIM) && (numberOfFormulae < 2 || softmenu[softmenuStack[0].softmenuId].menuItem != -MNU_EQN)) {
          screenUpdatingMode = SCRUPD_AUTO;
          if(calcMode == CM_NIM) {
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

      case CM_REGISTER_BROWSER: {
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
          displayBugScreen(errorMessage);
        }
        break;
      }

      case CM_FLAG_BROWSER: {
        currentFlgScr = 3 - currentFlgScr;
        break;
      }

      case CM_FONT_BROWSER: {
        if(currentFntScr < numScreensNumericFont + numScreensStandardFont) {
          currentFntScr++;
        }
        break;
      }

      case CM_PEM: {
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

      case CM_MIM: {
        if(currentSoftmenuScrolls()) {
          menuDown();
        }
        break;
      }

      case CM_ASSIGN: {
        if(currentSoftmenuScrolls()) {
          menuDown();
        }
        else if(tam.alpha && (itemToBeAssigned == 0 || tam.mode == TM_NEWMENU) && alphaCase == AC_UPPER) {
          alphaCase = AC_LOWER;
        }
        else if(tam.alpha && itemToBeAssigned == 0 && aimBuffer[0] == 0) {
          assignLeaveAlpha();
          itemToBeAssigned = ITM_DOWN;
        }
        else if(tam.alpha && aimBuffer[0] == 0) {
          assignLeaveAlpha();
          if(asnKey[1] != 0) {
            assignToKey((char *)asnKey);
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

      case CM_TIMER: {
        fnDownTimerApp();
        break;
      }

      default: {
        sprintf(errorMessage, "In function fnKeyDown: unexpected calcMode value (%" PRIu8 ") while processing key DOWN!", calcMode);
        displayBugScreen(errorMessage);
      }
    }
  #endif // !TESTSUITE_BUILD
}



void fnKeyDotD(uint16_t unusedButMandatoryParameter) {
  #if !defined(TESTSUITE_BUILD)
    switch(calcMode) {
      case CM_NORMAL: {
        if(getSystemFlag(FLAG_FRACT)) {
          clearSystemFlag(FLAG_FRACT);
        }
        else {
          runFunction(ITM_toREAL);
        }
        break;
      }

      case CM_NIM: {
        addItemToNimBuffer(ITM_dotD);
        break;
      }

      case CM_REGISTER_BROWSER:
      case CM_FLAG_BROWSER:
      case CM_FONT_BROWSER:
      case CM_PLOT_STAT:
      case CM_GRAPH:
      case CM_MIM:
      case CM_EIM:
      case CM_TIMER: {
        break;
      }

      default: {
        sprintf(errorMessage, "In function fnKeyDotD: unexpected calcMode value (%" PRIu8 ") while processing key .d!", calcMode);
        displayBugScreen(errorMessage);
      }
    }
  #endif // !TESTSUITE_BUILD
}



void fnKeyAngle(uint16_t unusedButMandatoryParameter) {
  #if !defined(TESTSUITE_BUILD)
    switch(calcMode) {
      case CM_NORMAL: {
        if(getRegisterDataType(REGISTER_X) == dtReal34Matrix) {
          runFunction(ITM_VANGLE);
        }
        else {
          runFunction(ITM_ARG);
        }
        break;
      }

      case CM_NIM: {
        addItemToNimBuffer(ITM_ANGLE);
        break;
      }

      case CM_REGISTER_BROWSER:
      case CM_FLAG_BROWSER:
      case CM_FONT_BROWSER:
      case CM_PLOT_STAT:
      case CM_GRAPH:
      case CM_MIM:
      case CM_EIM:
      case CM_TIMER: {
        break;
      }

      case CM_ASSIGN: {
        if(itemToBeAssigned == 0) {
          itemToBeAssigned = ITM_ANGLE;
        }
        break;
      }

      default: {
        sprintf(errorMessage, "In function fnKeyAngle: unexpected calcMode value (%" PRIu8 ") while processing key .d!", calcMode);
        displayBugScreen(errorMessage);
      }
    }
  #endif // !TESTSUITE_BUILD
}



void setLastKeyCode(int key) {
  if(1 <= key && key <= 43) {
    if(key <=  6) {
      lastKeyCode = key      + 20;
    }
    else if(key <= 12) {
      lastKeyCode = key -  6 + 30;
    }
    else if(key <= 17) {
      lastKeyCode = key - 12 + 40;
    }
    else if(key <= 22) {
      lastKeyCode = key - 17 + 50;
    }
    else if(key <= 27) {
      lastKeyCode = key - 22 + 60;
    }
    else if(key <= 32) {
      lastKeyCode = key - 27 + 70;
    }
    else if(key <= 37) {
      lastKeyCode = key - 32 + 80;
    }
    else if(key <= 43) {
      lastKeyCode = key - 37 + 10; // function keys
    }
  }
}
