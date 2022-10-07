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

/********************************************//**
 * \file flags.c
 ***********************************************/

#include "flags.h"

#include "apps/bugScreen.h"
#include "calcMode.h"
#include "config.h"
#include "error.h"
#include "items.h"
#include "solver/equation.h"
#include "ui/softmenus.h"
#include "ui/tam.h"
#include <string.h>

#include "wp43.h"

void systemFlagAction(uint16_t systemFlag, uint16_t action) {
  switch(systemFlag) {
    case FLAG_ALLENG: {
      switch(action) {
        case 0: {
          globalFlags[FLAG_A/16] &= ~(1u << (FLAG_A%16));
          break;
        }
        case 1: {
          globalFlags[FLAG_A/16] |=   1u << (FLAG_A%16);
          break;
        }
        case 2: {
          globalFlags[FLAG_A/16] ^=   1u << (FLAG_A%16);
          break;
        }
        default: {
        }
      }
      break;
    }

    case FLAG_OVERFLOW: {
      switch(action) {
        case 0: {
          globalFlags[FLAG_B/16] &= ~(1u << (FLAG_B%16));
          break;
        }
        case 1: {
          globalFlags[FLAG_B/16] |=   1u << (FLAG_B%16);
          break;
        }
        case 2: {
          globalFlags[FLAG_B/16] ^=   1u << (FLAG_B%16);
          break;
        }
        default: {
        }
      }
      break;
    }

    case FLAG_CARRY: {
      switch(action) {
        case 0: {
          globalFlags[FLAG_C/16] &= ~(1u << (FLAG_C%16));
          break;
        }
        case 1: {
          globalFlags[FLAG_C/16] |=   1u << (FLAG_C%16);
          break;
        }
        case 2: {
          globalFlags[FLAG_C/16] ^=   1u << (FLAG_C%16);
          break;
        }
        default: {
        }
      }
      break;
    }

    case FLAG_SPCRES: {
      switch(action) {
        case 0: {
          globalFlags[FLAG_D/16] &= ~(1u << (FLAG_D%16));
          break;
        }
        case 1: {
          globalFlags[FLAG_D/16] |=   1u << (FLAG_D%16);
          break;
        }
        case 2: {
          globalFlags[FLAG_D/16] ^=   1u << (FLAG_D%16);
          break;
        }
        default: {
        }
      }
      break;
    }

    case FLAG_CPXRES: {
      switch(action) {
        case 0: {
          globalFlags[FLAG_I/16] &= ~(1u << (FLAG_I%16));
          break;
        }
        case 1: {
          globalFlags[FLAG_I/16] |=   1u << (FLAG_I%16);
          break;
        }
        case 2: {
          globalFlags[FLAG_I/16] ^=   1u << (FLAG_I%16);
          break;
        }
        default: {
        }
      }
      break;
    }

    case FLAG_LEAD0: {
      switch(action) {
        case 0: {
          globalFlags[FLAG_L/16] &= ~(1u << (FLAG_L%16));
          break;
        }
        case 1: {
          globalFlags[FLAG_L/16] |=   1u << (FLAG_L%16);
          break;
        }
        case 2: {
          globalFlags[FLAG_L/16] ^=   1u << (FLAG_L%16);
          break;
        }
        default: {
        }
      }
      break;
    }

    case FLAG_TRACE: {
      switch(action) {
        case 0: {
          globalFlags[FLAG_T/16] &= ~(1u << (FLAG_T%16));
          break;
        }
        case 1: {
          globalFlags[FLAG_T/16] |=   1u << (FLAG_T%16);
          break;
        }
        case 2: {
          globalFlags[FLAG_T/16] ^=   1u << (FLAG_T%16);
          break;
        }
        default: {
        }
      }
      break;
    }

    case FLAG_POLAR: {
      switch(action) {
        case 0: {
          globalFlags[FLAG_X/16] &= ~(1u << (FLAG_X%16));
          break;
        }
        case 1: {
          globalFlags[FLAG_X/16] |=   1u << (FLAG_X%16);
          break;
        }
        case 2: {
          globalFlags[FLAG_X/16] ^=   1u << (FLAG_X%16);
          break;
        }
        default: {
        }
      }
      break;
    }

    default: {
    }
  }
}



void synchronizeLetteredFlags(void) {
  if(getSystemFlag(FLAG_ALLENG)) {
    setSystemFlag(FLAG_ALLENG)
  }
  else {
    clearSystemFlag(FLAG_ALLENG)
  }
  if(getSystemFlag(FLAG_OVERFLOW)) {
    setSystemFlag(FLAG_OVERFLOW)
  }
  else {
    clearSystemFlag(FLAG_OVERFLOW)
  }
  if(getSystemFlag(FLAG_CARRY)) {
    setSystemFlag(FLAG_CARRY)
  }
  else {
    clearSystemFlag(FLAG_CARRY)
  }
  if(getSystemFlag(FLAG_SPCRES)) {
    setSystemFlag(FLAG_SPCRES)
  }
  else {
    clearSystemFlag(FLAG_SPCRES)
  }
  if(getSystemFlag(FLAG_CPXRES)) {
    setSystemFlag(FLAG_CPXRES)
  }
  else {
    clearSystemFlag(FLAG_CPXRES)
  }
  if(getSystemFlag(FLAG_LEAD0)) {
    setSystemFlag(FLAG_LEAD0)
  }
  else {
    clearSystemFlag(FLAG_LEAD0)
  }
  if(getSystemFlag(FLAG_TRACE)) {
    setSystemFlag(FLAG_TRACE)
  }
  else {
    clearSystemFlag(FLAG_TRACE)
  }
  if(getSystemFlag(FLAG_POLAR)) {
    setSystemFlag(FLAG_POLAR)
  }
  else {
    clearSystemFlag(FLAG_POLAR)
  }
}



#if !defined(TESTSUITE_BUILD)
static void _setAlpha(void) {
  if(calcMode != cmEim) {
    calcModeEnter(cmAim);
  }
}

static void _clearAlpha(void) {
  if(calcMode == cmEim) {
    if(softmenu[softmenuStack[0].softmenuId].menuItem == -MNU_EQ_EDIT) {
      calcModeEnter(cmNormal);
      if(allFormulae[currentFormula].pointerToFormulaData == WP43_NULL) {
        deleteEquation(currentFormula);
      }
    }
    popSoftmenu();
  }
  else {
    calcModeEnter(cmNormal);
  }
}
#endif // TESTSUITE_BUILD



/********************************************//**
 * \brief Returns the status of a flag
 *
 * \param[in] flag uint16_t
 * \return bool_t
 ***********************************************/
bool_t getFlag(uint16_t flag) {
  if(flag & 0x8000) { // System flag
    return getSystemFlag(flag);
  }
  else if(flag < NUMBER_OF_GLOBAL_FLAGS) { // Global flag
    return (globalFlags[flag/16] & (1u << (flag%16))) != 0;
  }
  else { // Local flag
    if(currentLocalFlags != NULL) {
      flag -= NUMBER_OF_GLOBAL_FLAGS;
      if(flag < NUMBER_OF_LOCAL_FLAGS) {
        return (currentLocalFlags->localFlags & (1u << (uint32_t)flag)) != 0;
      }
      else {
        sprintf(errorMessage, "In function getFlag: local flag %" PRIu16 " is not defined! Must be from 0 to %d.", flag, NUMBER_OF_LOCAL_FLAGS - 1);
        bugScreen(errorMessage);
      }
    }
    #if defined(PC_BUILD)
      else {
        moreInfoOnError("In function getFlag:", "no local flags defined!", "To do so, you can find LocR here:", "[g] [P.FN] [g] [F5]");
      }
    #endif // PC_BUILD
  }
  return false;
 }



/********************************************//**
 * \brief Returns the status of a system flag
 *
 * \param[in] systemFlag uint16_t
 * \return void
 ***********************************************/
void fnGetSystemFlag(uint16_t systemFlag) {
  if(getSystemFlag(systemFlag)) {
    temporaryInformation = TI_TRUE;
  }
  else {
    temporaryInformation = TI_FALSE;
  }
}



/********************************************//**
 * \brief Sets a flag
 *
 * \param[in] flag uint16_t
 * \return void
 ***********************************************/
void fnSetFlag(uint16_t flag) {
  if(flag & 0x8000) { // System flag
    if(isSystemFlagWriteProtected(flag)) {
      temporaryInformation = TI_NO_INFO;
      if(programRunStop == PGM_WAITING) {
        programRunStop = PGM_STOPPED;
      }
      displayCalcErrorMessage(ERROR_WRITE_PROTECTED_SYSTEM_FLAG, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        sprintf(errorMessage, "protected system flag (%" PRIu16 ")!", (uint16_t)(flag & 0x3fff));
        moreInfoOnError("In function fnSetFlag:", "Tying to set a write", errorMessage, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
      return;
    }
    #if !defined(TESTSUITE_BUILD)
      else if(flag == FLAG_ALPHA) {
        tamLeaveMode();
        _setAlpha();
      }
    #endif // !TESTSUITE_BUILD
    else {
      setSystemFlag(flag);
    }
  }
  else if(flag < NUMBER_OF_GLOBAL_FLAGS) { // Global flag
    switch(flag) {
      case FLAG_A: {
        setSystemFlag(FLAG_ALLENG);
        break;
      }
      case FLAG_B: {
        setSystemFlag(FLAG_OVERFLOW);
        break;
      }
      case FLAG_C: {
        setSystemFlag(FLAG_CARRY);
        break;
      }
      case FLAG_D: {
        setSystemFlag(FLAG_SPCRES);
        break;
      }
      case FLAG_I: {
        setSystemFlag(FLAG_CPXRES);
        break;
      }
      case FLAG_L: {
        setSystemFlag(FLAG_LEAD0);
        break;
      }
      case FLAG_T: {
        setSystemFlag(FLAG_TRACE);
        break;
      }
      case FLAG_X: {
        setSystemFlag(FLAG_POLAR);
        break;
      }
      default: {
        globalFlags[flag/16] |= 1u << (flag%16);
      }
    }
  }
  else { // Local flag
    if(currentLocalFlags != NULL) {
      flag -= NUMBER_OF_GLOBAL_FLAGS;
      if(flag < NUMBER_OF_LOCAL_FLAGS) {
        currentLocalFlags->localFlags |=  (1u << (uint32_t)flag);
      }
      else {
        sprintf(errorMessage, "In function fnSetFlag: local flag %" PRIu16 " is not defined! Must be from 0 to %d.", flag, NUMBER_OF_LOCAL_FLAGS - 1);
        bugScreen(errorMessage);
      }
    }
    #if defined(PC_BUILD)
      else {
        moreInfoOnError("In function fnSetFlag:", "no local flags defined!", "To do so, you can find LocR here:", "[g] [P.FN] [g] [F5]");
      }
    #endif // PC_BUILD
  }
}



/********************************************//**
 * \brief Clears a flag
 *
 * \param[in] flags uint16_t
 * \return void
 ***********************************************/
void fnClearFlag(uint16_t flag) {
  if(flag & 0x8000) { // System flag
    if(isSystemFlagWriteProtected(flag)) {
      temporaryInformation = TI_NO_INFO;
      if(programRunStop == PGM_WAITING) {
        programRunStop = PGM_STOPPED;
      }
      displayCalcErrorMessage(ERROR_WRITE_PROTECTED_SYSTEM_FLAG, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        sprintf(errorMessage, "protected system flag (%" PRIu16 ")!", (uint16_t)(flag & 0x3fff));
        moreInfoOnError("In function fnClearFlag:", "Tying to clear a write", errorMessage, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
      return;
    }
    #if !defined(TESTSUITE_BUILD)
      else if(flag == FLAG_ALPHA) {
        tamLeaveMode();
        _clearAlpha();
      }
    #endif // !TESTSUITE_BUILD
    else {
      clearSystemFlag(flag);
    }
  }
  else if(flag < NUMBER_OF_GLOBAL_FLAGS) { // Global flag
    switch(flag) {
      case FLAG_A: {
        clearSystemFlag(FLAG_ALLENG);
        break;
      }
      case FLAG_B: {
        clearSystemFlag(FLAG_OVERFLOW);
        break;
      }
      case FLAG_C: {
        clearSystemFlag(FLAG_CARRY);
        break;
      }
      case FLAG_D: {
        clearSystemFlag(FLAG_SPCRES);
        break;
      }
      case FLAG_I: {
        clearSystemFlag(FLAG_CPXRES);
        break;
      }
      case FLAG_L: {
        clearSystemFlag(FLAG_LEAD0);
        break;
      }
      case FLAG_T: {
        clearSystemFlag(FLAG_TRACE);
        break;
      }
      case FLAG_X: {
        clearSystemFlag(FLAG_POLAR);
        break;
      }
      default: {
        globalFlags[flag/16] &= ~(1u << (flag%16));
      }
    }
  }
  else { // Local flag
    if(currentLocalFlags != NULL) {
      flag -= NUMBER_OF_GLOBAL_FLAGS;
      if(flag < NUMBER_OF_LOCAL_FLAGS) {
        currentLocalFlags->localFlags &= ~(1u << (uint32_t)flag);
      }
      else {
        sprintf(errorMessage, "In function fnClearFlag: local flag %" PRIu16 " is not defined! Must be from 0 to %d.", flag, NUMBER_OF_LOCAL_FLAGS - 1);
        bugScreen(errorMessage);
      }
    }
    #if defined(PC_BUILD)
      else {
       moreInfoOnError("In function fnClearFlag:", "no local flags defined!", "To do so, you can find LocR here:", "[g] [P.FN] [g] [F5]");
      }
    #endif // PC_BUILD
  }
}



/********************************************//**
 * \brief Flips a flag
 *
 * \param[in] flags uint16_t
 * \return void
 ***********************************************/
void fnFlipFlag(uint16_t flag) {
  if(flag & 0x8000) { // System flag
    if(isSystemFlagWriteProtected(flag)) {
      temporaryInformation = TI_NO_INFO;
      if(programRunStop == PGM_WAITING) {
        programRunStop = PGM_STOPPED;
      }
      displayCalcErrorMessage(ERROR_WRITE_PROTECTED_SYSTEM_FLAG, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        sprintf(errorMessage, "protected system flag (%" PRIu16 ")!", (uint16_t)(flag & 0x3fff));
        moreInfoOnError("In function fnFlipFlag:", "Tying to flip a write", errorMessage, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
      return;
    }
    #if !defined(TESTSUITE_BUILD)
      else if(flag == FLAG_ALPHA) {
        tamLeaveMode();
        if(getSystemFlag(FLAG_ALPHA)) {
          _clearAlpha();
        }
        else {
          _setAlpha();
        }
      }
    #endif // !TESTSUITE_BUILD
    else {
      flipSystemFlag(flag);
    }
  }
  else if(flag < NUMBER_OF_GLOBAL_FLAGS) { // Global flag
    switch(flag) {
      case FLAG_A: {
        flipSystemFlag(FLAG_ALLENG);
        break;
      }
      case FLAG_B: {
        flipSystemFlag(FLAG_OVERFLOW);
        break;
      }
      case FLAG_C: {
        flipSystemFlag(FLAG_CARRY);
        break;
      }
      case FLAG_D: {
        flipSystemFlag(FLAG_SPCRES);
        break;
      }
      case FLAG_I: {
        flipSystemFlag(FLAG_CPXRES);
        break;
      }
      case FLAG_L: {
        flipSystemFlag(FLAG_LEAD0);
        break;
      }
      case FLAG_T: {
        flipSystemFlag(FLAG_TRACE);
        break;
      }
      case FLAG_X: {
        flipSystemFlag(FLAG_POLAR);
        break;
      }
      default: {
        globalFlags[flag/16] ^=  1u << (flag%16);
      }
    }
  }
  else { // Local flag
    if(currentLocalFlags != NULL) {
      flag -= NUMBER_OF_GLOBAL_FLAGS;
      if(flag < NUMBER_OF_LOCAL_FLAGS) {
        currentLocalFlags->localFlags ^=  (1u << (uint32_t)flag);
      }
      else {
        sprintf(errorMessage, "In function fnFlipFlag: local flag %" PRIu16 " is not defined! Must be from 0 to %d.", flag, NUMBER_OF_LOCAL_FLAGS - 1);
        bugScreen(errorMessage);
      }
    }
    #if defined(PC_BUILD)
      else {
        moreInfoOnError("In function fnFlipFlag:", "no local flags defined!", "To do so, you can find LocR here:", "[g] [P.FN] [g] [F5]");
      }
    #endif // PC_BUILD
  }
}



/********************************************//**
 * \brief Clear all global from 00 to 99 and the local flags
 *
 * \param[in] flags uint16_t
 * \return void
 ***********************************************/
void fnClFAll(uint16_t confirmation) {
  if(confirmation == NOT_CONFIRMED) {
    setConfirmationMode(fnClAll);
  }
  else {
    // Leave lettered flags as they are
    memset(globalFlags, 0, sizeof(globalFlags) - sizeof(uint16_t)); // Clear flags from 00 to 95
    globalFlags[6] &= 0xfff0; // Clear flags from 96 to 99

    if(currentLocalFlags != NULL) {
      currentLocalFlags->localFlags = 0;
    }
  }
}



void fnIsFlagClear(uint16_t flag) {
  temporaryInformation = (getFlag(flag) ? TI_FALSE : TI_TRUE);
}



void fnIsFlagClearClear(uint16_t flag) {
  temporaryInformation = (getFlag(flag) ? TI_FALSE : TI_TRUE);
  fnClearFlag(flag);
}



void fnIsFlagClearSet(uint16_t flag) {
  temporaryInformation = (getFlag(flag) ? TI_FALSE : TI_TRUE);
  fnSetFlag(flag);
}



void fnIsFlagClearFlip(uint16_t flag) {
  temporaryInformation = (getFlag(flag) ? TI_FALSE : TI_TRUE);
  fnFlipFlag(flag);
}



void fnIsFlagSet(uint16_t flag) {
  temporaryInformation = (getFlag(flag) ? TI_TRUE : TI_FALSE);
}




void fnIsFlagSetClear(uint16_t flag) {
  temporaryInformation = (getFlag(flag) ? TI_TRUE : TI_FALSE);
  fnClearFlag(flag);
}




void fnIsFlagSetSet(uint16_t flag) {
  temporaryInformation = (getFlag(flag) ? TI_TRUE : TI_FALSE);
  fnSetFlag(flag);
}




void fnIsFlagSetFlip(uint16_t flag) {
  temporaryInformation = (getFlag(flag) ? TI_TRUE : TI_FALSE);
  fnFlipFlag(flag);
}
