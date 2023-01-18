// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "assign.h"

#include "calcMode.h"
#include "charString.h"
#include "core/memory.h"
#include "defines.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "hal/gui.h"
#include "items.h"
#include "programming/flash.h"
#include "programming/manage.h"
#include "registers.h"
#include "sort.h"
#include "ui/bufferize.h"
#include "ui/screen.h"
#include "ui/softmenus.h"
#include "ui/tam.h"
#include <stdbool.h>
#include <string.h>

#include "wp43.h"

//This variable is to store in flash memory
TO_QSPI const calcKey_t kbd_std[37] = {
//keyId primary          fShifted         gShifted         keyLblAim        primaryAim         fShiftedAim            gShiftedAim    primaryTam
 {21,   ITM_1ONX,        ITM_TGLFRT,     -MNU_UNITCONV,    ITM_NULL,        ITM_A,            -MNU_ALPHAINTL,         ITM_ALPHA,     ITM_REG_A    },
 {22,  -MNU_EXP,         ITM_toINT,      -MNU_BITS,        ITM_NUMBER_SIGN, ITM_B,             ITM_NUMBER_SIGN,       ITM_BETA,      ITM_REG_B    },
 {23,  -MNU_TRI,         ITM_DMS,        -MNU_ANGLECONV,   ITM_NULL,        ITM_C,             ITM_LEFT_PARENTHESIS,  ITM_GAMMA,     ITM_REG_C    },
 {24,   ITM_LN,          ITM_dotD,        ITM_LOG10,       ITM_NULL,        ITM_D,             ITM_RIGHT_PARENTHESIS, ITM_DELTA,     ITM_REG_D    }, // if f or g are changed: adapt the function btnClicked section if(calcMode == cmNim) in keyboard.c. Case D for decimal base
 {25,   ITM_EXP,         ITM_toHMS,       ITM_10x,         ITM_ALOG_SYMBOL, ITM_E,             ITM_ALOG_SYMBOL,       ITM_EPSILON,   ITM_E        },
 {26,   ITM_SQUAREROOTX, ITM_AIM,        -MNU_ALPHAFN,     ITM_ROOT_SIGN,   ITM_F,             ITM_ROOT_SIGN,         ITM_PHI,       ITM_alpha    },

 {31,   ITM_STO,         ITM_ASSIGN,      ITM_SAVE,        ITM_NULL,        ITM_G,             ITM_ASSIGN,            ITM_GAMMA,     ITM_NULL     },
 {32,   ITM_RCL,         ITM_RBR,         ITM_VIEW,        ITM_NULL,        ITM_H,             ITM_RBR,               ITM_CHI,       ITM_HEX      }, // if f or g are changed: adapt the function btnClicked section if(calcMode == cmNim) in keyboard.c. Case H for hexadecimal base
 {33,   ITM_Rdown,       ITM_Rup,        -MNU_CPX,         ITM_NULL,        ITM_I,             ITM_DOWN_ARROW,        ITM_IOTA,      ITM_REG_I    },
 {34,   ITM_CC,          ITM_MAGNITUDE,   ITM_ARG,         ITM_NULL,        ITM_J,             ITM_VERTICAL_BAR,      ITM_ETA,       ITM_REG_J    },
 {35,   ITM_SHIFTf,      ITM_NULL,        ITM_SNAP,        ITM_SHIFTf,      ITM_SHIFTf,        ITM_NULL,              ITM_SNAP,      ITM_SHIFTf   },
 {36,   ITM_SHIFTg,      ITM_USERMODE,    ITM_NULL,        ITM_SHIFTg,      ITM_SHIFTg,        ITM_USERMODE,          ITM_NULL,      ITM_SHIFTg   },

 {41,   ITM_ENTER,       ITM_FILL,        ITM_DROP,        ITM_ENTER,       ITM_ENTER,         ITM_NULL,              ITM_NULL,      ITM_ENTER    },
 {42,   ITM_XexY,        ITM_Xex,        -MNU_STK,         ITM_ex,          ITM_K,             ITM_ex,                ITM_KAPPA,     ITM_REG_K    },
 {43,   ITM_CHS,         ITM_DELTAPC,    -MNU_FIN,         ITM_PLUS_MINUS,  ITM_L,             ITM_PLUS_MINUS,        ITM_LAMBDA,    ITM_REG_L    },
 {44,   ITM_EXPONENT,    ITM_DSP,        -MNU_DISP,        ITM_NULL,        ITM_M,             ITM_UP_ARROW,          ITM_MU,        ITM_NULL     },
 {45,   ITM_BACKSPACE,   ITM_UNDO,       -MNU_CLR,         ITM_BACKSPACE,   ITM_BACKSPACE,     ITM_UNDO,             -MNU_CLR,       ITM_BACKSPACE},

 {51,   ITM_DIV,         ITM_PARALLEL,    ITM_MOD,         ITM_SLASH,       ITM_N,             ITM_SLASH,             ITM_NU,        ITM_DIV      },
 {52,   ITM_7,           ITM_NULL,        ITM_NULL,        ITM_7,           ITM_O,             ITM_7,                 ITM_OMEGA,     ITM_7        },
 {53,   ITM_8,           ITM_NULL,       -MNU_MODE,        ITM_8,           ITM_P,             ITM_8,                 ITM_PI,        ITM_8        },
 {54,   ITM_9,           ITM_LBL,         ITM_RTN,         ITM_9,           ITM_Q,             ITM_9,                 ITM_OMICRON,   ITM_9        },
 {55,   ITM_XEQ,         ITM_GTO,        -MNU_FLAGS,       ITM_NULL,        ITM_NULL,          ITM_NULL,             -MNU_FLAGS,     ITM_NULL     },

 {61,   ITM_MULT,        ITM_XFACT,      -MNU_PROB,        ITM_CROSS,       ITM_R,             ITM_PROD_SIGN,         ITM_RHO,       ITM_MULT     },
 {62,   ITM_4,          -MNU_SUMS,       -MNU_STAT,        ITM_4,           ITM_S,             ITM_4,                 ITM_SIGMA,     ITM_4        },
 {63,   ITM_5,           ITM_toREC,       ITM_toPOL,       ITM_5,           ITM_T,             ITM_5,                 ITM_TAU,       ITM_5        },
 {64,   ITM_6,           ITM_TIMER,      -MNU_CLK,         ITM_6,           ITM_U,             ITM_6,                 ITM_THETA,     ITM_6        },
 {65,   ITM_UP,          ITM_BST,         ITM_SF,          ITM_UP,          ITM_UP,            ITM_BST,               ITM_SF,        ITM_UP       },

 {71,   ITM_SUB,        -MNU_INTS,       -MNU_PARTS,       ITM_MINUS,       ITM_V,             ITM_MINUS,            -MNU_ALPHAMATH, ITM_SUB      },
 {72,   ITM_1,          -MNU_ADV,        -MNU_EQN,         ITM_1,           ITM_W,             ITM_1,                 ITM_PSI,       ITM_1        },
 {73,   ITM_2,          -MNU_MATX,       -MNU_XFN,         ITM_2,           ITM_X,             ITM_2,                 ITM_XI,        ITM_2        },
 {74,   ITM_3,           ITM_CONSTpi,    -MNU_CONST,       ITM_3,           ITM_Y,             ITM_3,                 ITM_UPSILON,   ITM_3        },
 {75,   ITM_DOWN,        ITM_SST,         ITM_CF,          ITM_DOWN,        ITM_DOWN,          ITM_SST,               ITM_CF,        ITM_DOWN     },

 {81,   ITM_ADD,        -MNU_LOOP,       -MNU_TEST,        ITM_PLUS,        ITM_Z,             ITM_PLUS,              ITM_ZETA,      ITM_ADD      },
 {82,   ITM_0,          -MNU_IO,         -MNU_PRINT,       ITM_0,           ITM_QUESTION_MARK, ITM_0,                 ITM_PRINTER,   ITM_0        },
 {83,   ITM_PERIOD,      ITM_SHOW,       -MNU_INFO,        ITM_PERIOD,      ITM_COMMA,         ITM_PERIOD,           -MNU_ALPHADOT,  ITM_PERIOD   },
 {84,   ITM_RS,          ITM_PR,         -MNU_PFN,         ITM_NULL,        ITM_SPACE,         ITM_NULL,              ITM_NULL,      ITM_NULL     },
 {85,   ITM_EXIT,       -MNU_CATALOG,     ITM_OFF,         ITM_EXIT,        ITM_EXIT,         -MNU_CATALOG,           ITM_OFF,       ITM_EXIT     }
//keyId primary          fShifted         gShifted         keyLblAim        primaryAim         fShiftedAim            gShiftedAim    primaryTam
};



void fnAssign(uint16_t mode) {
  if(mode) {
    createMenu(aimBuffer);
    aimBuffer[0] = 0;
  }
  else {
    calcModeEnter(cmAssign);
    itemToBeAssigned = 0;
    updateAssignTamBuffer();
  }
}



void fnDeleteMenu(uint16_t id) {
  if(id >= numberOfUserMenus) {
    displayCalcErrorMessage(ERROR_CANNOT_DELETE_PREDEF_ITEM, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
  }
  else {
    if(numberOfUserMenus == 1) {
      freeWp43(userMenus, sizeof(userMenu_t));
      userMenus = NULL;
      numberOfUserMenus = 0;
    }
    else if(numberOfUserMenus > 1) {
      if(id < numberOfUserMenus - 1) {
        xcopy(userMenus + id, userMenus + id + 1, sizeof(userMenu_t) * (numberOfUserMenus - id - 1));
      }
      freeWp43(userMenus + numberOfUserMenus - 1, sizeof(userMenu_t));
      --numberOfUserMenus;
    }
  }
  #if !defined(TESTSUITE_BUILD)
    showSoftmenu(-MNU_DYNAMIC);
    popSoftmenu();
  #endif // !TESTSUITE_BUILD
}



void updateAssignTamBuffer(void) {
  char *tbPtr = tamBuffer;
  tbPtr = stpcpy(tbPtr, "ASSIGN ");

  if(itemToBeAssigned == 0) {
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
      tbPtr = stpcpy(tbPtr, "_");
    }
  }
  else if(itemToBeAssigned == ASSIGN_CLEAR) {
    tbPtr = stpcpy(tbPtr, "NULL");
  }
  else if(itemToBeAssigned >= ASSIGN_LABELS) {
    if(labelList[itemToBeAssigned - ASSIGN_LABELS].program > 0) { // RAM
      uint8_t *lblPtr = labelList[itemToBeAssigned - ASSIGN_LABELS].labelPointer.ram;
      uint32_t count = *(lblPtr++);
      for(uint32_t i = 0; i < count; ++i) {
        *(tbPtr++) = *(lblPtr++);
      }
    }
    else { // Flash
      uint8_t *lblPtr = (uint8_t *)(tmpString + TMP_STR_LENGTH - 16);
      readStepInFlashPgmLibrary(lblPtr, 16, labelList[itemToBeAssigned - ASSIGN_LABELS].labelPointer.flash);
      uint32_t count = *(lblPtr++);
      for(uint32_t i = 0; i < count; ++i) {
        *(tbPtr++) = *(lblPtr++);
      }
    }
  }
  else if(itemToBeAssigned >= ASSIGN_RESERVED_VARIABLES) {
    tbPtr = stpcpy(tbPtr, (char *)allReservedVariables[itemToBeAssigned - ASSIGN_RESERVED_VARIABLES].reservedVariableName + 1);
  }
  else if(itemToBeAssigned >= ASSIGN_NAMED_VARIABLES) {
    tbPtr = stpcpy(tbPtr, (char *)allNamedVariables[itemToBeAssigned - ASSIGN_NAMED_VARIABLES].variableName + 1);
  }
  else if(itemToBeAssigned <= ASSIGN_USER_MENU) {
    tbPtr = stpcpy(tbPtr, userMenus[-(itemToBeAssigned - ASSIGN_USER_MENU)].menuName);
  }
  else if(itemToBeAssigned < 0) {
    tbPtr = stpcpy(tbPtr, indexOfItems[-itemToBeAssigned].itemCatalogName);
  }
  else if(indexOfItems[itemToBeAssigned].itemCatalogName[0] == 0) {
    tbPtr = stpcpy(tbPtr, indexOfItems[itemToBeAssigned].itemSoftmenuName);
  }
  else {
    tbPtr = stpcpy(tbPtr, indexOfItems[itemToBeAssigned].itemCatalogName);
  }

  tbPtr = stpcpy(tbPtr, " ");
  if(itemToBeAssigned != 0 && tam.alpha) {
    tbPtr = stpcpy(tbPtr, STD_LEFT_SINGLE_QUOTE);
    if(aimBuffer[0] == 0) {
      tbPtr = stpcpy(tbPtr, "_");
    }
    else {
      tbPtr = stpcpy(tbPtr, aimBuffer);
      tbPtr = stpcpy(tbPtr, STD_RIGHT_SINGLE_QUOTE);
    }

  }
  else if(itemToBeAssigned != 0 && shiftF) {
    tbPtr = stpcpy(tbPtr, STD_SUP_f STD_CURSOR);
  }
  else if(itemToBeAssigned != 0 && shiftG) {
    tbPtr = stpcpy(tbPtr, STD_SUP_g STD_CURSOR);
  }
  else {
    tbPtr = stpcpy(tbPtr, "_");
  }
}



static void _assignItem(userMenuItem_t *menuItem) {
  const uint8_t *lblPtr = NULL;
  uint32_t l = 0;
  if(itemToBeAssigned == ASSIGN_CLEAR) {
    menuItem->item            = ITM_NULL;
    menuItem->argumentName[0] = 0;
  }
  else if(itemToBeAssigned >= ASSIGN_LABELS) {
    if(labelList[itemToBeAssigned - ASSIGN_LABELS].program > 0) {
      lblPtr                  = labelList[itemToBeAssigned - ASSIGN_LABELS].labelPointer.ram;
      menuItem->item          = ITM_XEQ;
    }
    else {
      readStepInFlashPgmLibrary((uint8_t *)(tmpString + 200), 32, labelList[itemToBeAssigned - ASSIGN_LABELS].labelPointer.flash);
      lblPtr                  = (uint8_t *)(tmpString + 200);
      menuItem->item          = ITM_XEQ;
    }
  }
  else if(itemToBeAssigned >= ASSIGN_RESERVED_VARIABLES) {
    lblPtr                    = allReservedVariables[itemToBeAssigned - ASSIGN_RESERVED_VARIABLES].reservedVariableName;
    menuItem->item            = ITM_RCL;
  }
  else if(itemToBeAssigned >= ASSIGN_NAMED_VARIABLES) {
    lblPtr                    = allNamedVariables[itemToBeAssigned - ASSIGN_NAMED_VARIABLES].variableName;
    menuItem->item            = ITM_RCL;
  }
  else if(itemToBeAssigned <= ASSIGN_USER_MENU) {
    lblPtr                    = (uint8_t *)userMenus[-(itemToBeAssigned - ASSIGN_USER_MENU)].menuName;
    menuItem->item            = -MNU_DYNAMIC;
    xcopy(menuItem->argumentName, (char *)lblPtr, stringByteLength((char *)lblPtr));
    lblPtr                    = NULL;
  }
  else {
    menuItem->item            = itemToBeAssigned;
    menuItem->argumentName[0] = 0;
  }
  if(lblPtr) {
    l = (uint32_t)(*(lblPtr++));
    xcopy(menuItem->argumentName, (char *)lblPtr, l);
    menuItem->argumentName[l] = 0;
  }
}

void assignToMyMenu(uint16_t position) {
  if(position < 18) {
    _assignItem(&userMenuItems[position]);
  }
  cachedDynamicMenu = 0;
  refreshScreen();
}

void assignToMyAlpha(uint16_t position) {
  if(position < 18) {
    _assignItem(&userAlphaItems[position]);
  }
  cachedDynamicMenu = 0;
  refreshScreen();
}

void assignToUserMenu(uint16_t position) {
  if(position < 18) {
    _assignItem(&userMenus[currentUserMenu].menuItem[position]);
  }
  cachedDynamicMenu = 0;
  refreshScreen();
}

static int _typeOfFunction(int16_t func) {
  switch(func) {
    case ITM_NULL: {
      return 0;
    }

    case ITM_EXIT:
    case ITM_ENTER:
    case ITM_UP:
    case ITM_DOWN:
    case ITM_BACKSPACE: {
      return 1;
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
    case ITM_9:
    case ITM_PERIOD:
    case ITM_ADD:
    case ITM_SUB:
    case ITM_MULT:
    case ITM_DIV: {
      return 2;
    }

    case ITM_A:
    case ITM_B:
    case ITM_C:
    case ITM_D:
    case ITM_E:
    case ITM_H:
    case ITM_I:
    case ITM_J:
    case ITM_K:
    case ITM_L: {
      return 3;
    }

    default: {
      return 4;
    }
  }
}

void assignToKey(keyCode_t keyCode) {
  calcKey_t *key = kbd_usr + keyCode - 1;
  userMenuItem_t tmpMenuItem;
  int keyStateCode = ((previousCalcMode) == cmAim ? 3 : 0) + (shiftG ? 2 : shiftF ? 1 : 0);
  const calcKey_t *stdKey = kbd_std + keyCode - 1;

  _assignItem(&tmpMenuItem);
  switch(_typeOfFunction(tmpMenuItem.item)) {
    case 0: {
      switch(keyStateCode) {
        case 5: {
          key->gShiftedAim = stdKey->gShiftedAim;
          break;
        }
        case 4: {
          key->fShiftedAim = stdKey->fShiftedAim;
          break;
        }
        case 3: {
          key->primaryAim  = stdKey->primaryAim;
          break;
        }
        case 2: {
          key->gShifted    = stdKey->gShifted;
          break;
        }
        case 1: {
          key->fShifted    = stdKey->fShifted;
          break;
        }
        case 0: {
          key->primary     = stdKey->primary;
          key->primaryTam  = stdKey->primaryTam;
        }
      }
      break;
    }

    case 1: {
      switch(keyStateCode) {
        case 5:
        case 2: {
          key->gShiftedAim = key->gShifted                   = tmpMenuItem.item;
          break;
        }
        case 4:
        case 1: {
          key->fShiftedAim = key->fShifted                   = tmpMenuItem.item;
          break;
        }
        case 3:
        case 0: {
          key->primaryAim  = key->primary  = key->primaryTam = tmpMenuItem.item;
          break;
        }
      }
      break;
    }

    case 2: {
      switch(keyStateCode) {
        case 5: {
           key->gShiftedAim = tmpMenuItem.item;
           break;
        }
        case 4: {
           key->fShiftedAim = tmpMenuItem.item;
           switch(tmpMenuItem.item) {
             case ITM_PLUS: {
               key->primary = ITM_ADD;
               break;
             }
             case ITM_MINUS: {
               key->primary = ITM_SUB;
               break;
             }
             case ITM_CROSS:
             case ITM_DOT:
             case ITM_PROD_SIGN: {
               key->primary = ITM_MULT;
               break;
             }
             case ITM_SLASH: {
               key->primary = ITM_DIV;
               break;
             }
             default: {
               key->primary = tmpMenuItem.item;
             }
           }
           break;
        }
        case 3: {
          key->primaryAim = tmpMenuItem.item;
          break;
        }
        case 2: {
          key->gShifted = tmpMenuItem.item;
          break;
        }
        case 1: {
          key->fShifted = tmpMenuItem.item;
          break;
        }
        case 0: {
          key->primary     = key->primaryTam = tmpMenuItem.item;
          switch(tmpMenuItem.item) {
            case ITM_ADD: {
              key->fShiftedAim = ITM_PLUS;
              break;
            }
            case ITM_SUB: {
              key->fShiftedAim = ITM_MINUS;
              break;
            }
            case ITM_MULT: {
              key->fShiftedAim = ITM_PROD_SIGN;
              break;
            }
            case ITM_DIV: {
              key->fShiftedAim = ITM_SLASH;
              break;
            }
            default: {
              key->fShiftedAim = tmpMenuItem.item;
            }
          }
        }
      }
      break;
    }

    case 3: {
      switch(keyStateCode) {
        case 5: {
          key->gShiftedAim = tmpMenuItem.item;
          break;
        }
        case 4: {
          key->fShiftedAim = tmpMenuItem.item;
          break;
        }
        case 3: {
          key->primaryAim  = tmpMenuItem.item;
          switch(tmpMenuItem.item) {
            case ITM_A: {
              key->primaryTam = ITM_REG_A;
              break;
            }
            case ITM_B: {
              key->primaryTam = ITM_REG_B;
              break;
            }
            case ITM_C: {
              key->primaryTam = ITM_REG_C;
              break;
            }
            case ITM_D: {
              key->primaryTam = ITM_REG_D;
              break;
            }
            case ITM_E: {
              key->primaryTam = ITM_E;
              break;
            }
            case ITM_H: {
              key->primaryTam = ITM_HEX;
              break;
            }
            case ITM_I: {
              key->primaryTam = ITM_REG_I;
              break;
            }
            case ITM_J: {
              key->primaryTam = ITM_REG_J;
              break;
            }
            case ITM_K: {
              key->primaryTam = ITM_REG_K;
              break;
            }
            case ITM_L: {
              key->primaryTam = ITM_REG_L;
              break;
            }
          }
          break;
        }
        case 2: {
          key->gShifted    = tmpMenuItem.item;
          break;
        }
        case 1: {
          key->fShifted    = tmpMenuItem.item;
          break;
        }
        case 0: {
          key->primary     = tmpMenuItem.item;
        }
      }
      break;
    }

    default: {
      switch(keyStateCode) {
        case 5: {
          key->gShiftedAim = tmpMenuItem.item;
          break;
        }
        case 4: {
          key->fShiftedAim = tmpMenuItem.item;
          break;
        }
        case 3: {
          key->primaryAim  = tmpMenuItem.item;
          if(_typeOfFunction(key->primary) != 2) {
            key->primaryTam  = ITM_NULL;
          }
          break;
        }
        case 2: {
          key->gShifted = tmpMenuItem.item;
          break;
        }
        case 1: {
          key->fShifted = tmpMenuItem.item;
          break;
        }
        case 0: {
          key->primary = tmpMenuItem.item;
          if(_typeOfFunction(key->primary) != 3) {
            key->primaryTam  = ITM_NULL;
          }
        }
      }
    }
  }
  if(keyCode == kcSqrt) { // alpha
    key->primaryTam  = stdKey->primaryTam;
  }

  setUserKeyArgument((keyCode - 1) * 6 + keyStateCode, tmpMenuItem.argumentName);
}



void setUserKeyArgument(uint16_t position, const char *name) {
  char *userKeyLabelPtr1 = (char *)getNthString((uint8_t *)userKeyLabel, position);
  char *userKeyLabelPtr2 = (char *)getNthString((uint8_t *)userKeyLabel, position + 1);
  char *userKeyLabelPtr3 = (char *)getNthString((uint8_t *)userKeyLabel, 37 * 6);
  uint16_t newUserKeyLabelSize = userKeyLabelSize - stringByteLength(userKeyLabelPtr1) + stringByteLength(name);
  char *newUserKeyLabel = allocWp43(newUserKeyLabelSize);
  char *newUserKeyLabelPtr = newUserKeyLabel;

  xcopy(newUserKeyLabelPtr, userKeyLabel, (int)(userKeyLabelPtr1 - userKeyLabel));
  newUserKeyLabelPtr += (int)(userKeyLabelPtr1 - userKeyLabel);
  xcopy(newUserKeyLabelPtr, name, stringByteLength(name));
  newUserKeyLabelPtr += stringByteLength(name);
  *(newUserKeyLabelPtr++) = 0;
  xcopy(newUserKeyLabelPtr, userKeyLabelPtr2, (int)(userKeyLabelPtr3 - userKeyLabelPtr2));
  newUserKeyLabelPtr += (int)(userKeyLabelPtr3 - userKeyLabelPtr2);
  *(newUserKeyLabelPtr++) = 0;

  freeWp43(userKeyLabel, userKeyLabelSize);
  userKeyLabel = newUserKeyLabel;
  userKeyLabelSize = newUserKeyLabelSize;
}



void createMenu(const char *name) {
  if(validateName(name)) {
    if(isUniqueName(name)) {
      if(numberOfUserMenus == 0) {
        userMenus = allocWp43(sizeof(userMenu_t));
      }
      else {
        userMenus = reallocWp43(userMenus, sizeof(userMenu_t) * numberOfUserMenus, sizeof(userMenu_t) * (numberOfUserMenus + 1));
      }
      memset(userMenus + numberOfUserMenus, 0, sizeof(userMenu_t));
      xcopy(userMenus[numberOfUserMenus].menuName, name, stringByteLength(name));
      ++numberOfUserMenus;
    }
    else {
      displayCalcErrorMessage(ERROR_ENTER_NEW_NAME, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("the name '%s' is already in use!", name);
    }
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_NAME, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("the menu '%s' does not follow the naming convention", name);
  }
}



void assignEnterAlpha(void) {
#if !defined(TESTSUITE_BUILD)
  tam.alpha = true;
  setSystemFlag(FLAG_ALPHA);
  aimBuffer[0] = 0;
  calcModeEnter(cmAim);
  numberOfTamMenusToPop = 0;
#endif // !TESTSUITE_BUILD
}

void assignLeaveAlpha(void) {
#if !defined(TESTSUITE_BUILD)
  tam.alpha = false;
  clearSystemFlag(FLAG_ALPHA);
  while(numberOfTamMenusToPop--) {
    popSoftmenu();
  }
  if(softmenuStack[0].softmenuId == 1) { // MyAlpha
    softmenuStack[0].softmenuId = 0; // MyMenu
  }
  calcModeUpdateGui();
#endif // !TESTSUITE_BUILD
}


void assignGetName1(void) {
  if(compareString(aimBuffer, "ENTER", CMP_NAME) == 0) {
    itemToBeAssigned = ITM_ENTER;
  }
  else if(compareString(aimBuffer, "EXIT", CMP_NAME) == 0) {
    itemToBeAssigned = ITM_EXIT;
  }
  /*else if(compareString(aimBuffer, "USER", CMP_NAME) == 0) {
    itemToBeAssigned = ITM_USERMODE;
  }
  else if(compareString(aimBuffer, STD_alpha, CMP_NAME) == 0) {
    itemToBeAssigned = ITM_AIM;
  }
  else if(compareString(aimBuffer, "f", CMP_NAME) == 0) {
    itemToBeAssigned = ITM_SHIFTf;
  }
  else if(compareString(aimBuffer, "g", CMP_NAME) == 0) {
    itemToBeAssigned = ITM_SHIFTg;
  }*/
  else if(aimBuffer[0] == 0 && alphaCase == AC_LOWER) {
    itemToBeAssigned = ITM_DOWN;
  }
  else if(aimBuffer[0] == 0) {
    itemToBeAssigned = ITM_NULL;
  }
  else {
    itemToBeAssigned = ASSIGN_CLEAR;

    // user-defined menus
    for(int i = 0; i < numberOfUserMenus; ++i) {
      if(compareString(aimBuffer, userMenus[i].menuName, CMP_NAME) == 0) {
        itemToBeAssigned = ASSIGN_USER_MENU - i;
        break;
      }
    }

    // preset menus
    if(itemToBeAssigned == ASSIGN_CLEAR) {
      for(int i = 0; softmenu[i].menuItem != 0; ++i) {
        if(compareString(aimBuffer, indexOfItems[-softmenu[i].menuItem].itemCatalogName, CMP_NAME) == 0) {
          itemToBeAssigned = softmenu[i].menuItem;
          break;
        }
      }
    }

    // programs
    if(itemToBeAssigned == ASSIGN_CLEAR) {
      itemToBeAssigned = findNamedLabel(aimBuffer);
      if(itemToBeAssigned == INVALID_VARIABLE) {
        itemToBeAssigned = ASSIGN_CLEAR;
      }
      else {
        itemToBeAssigned = itemToBeAssigned - FIRST_LABEL + ASSIGN_LABELS;
      }
    }

    // functions
    if(itemToBeAssigned == ASSIGN_CLEAR) {
      for(int i = 0; i < LAST_ITEM; ++i) {
        if((indexOfItems[i].status & CAT_STATUS) == CAT_FNCT && compareString(aimBuffer, indexOfItems[i].itemCatalogName, CMP_NAME) == 0) {
          itemToBeAssigned = i;
          break;
        }
      }
    }
  }
}

static bool _assignToKey(int16_t keyFunc) {
  int keyStateCode = (previousCalcMode) == cmAim ? 3 : 0;

  for(int i = 0; i < 3; ++i) {
    for(int j = 0; j < 37; ++j) {
      const calcKey_t *key = (getSystemFlag(FLAG_USER) ? kbd_usr : kbd_std) + j;
      int16_t kf = 0;
      switch(keyStateCode + i) {
        case 5: {
          kf = key->gShiftedAim;
          break;
        }
        case 4: {
          kf = key->fShiftedAim;
          break;
        }
        case 3: {
          kf = key->primaryAim;
          break;
        }
        case 2: {
          kf = key->gShifted;
          break;
        }
        case 1: {
          kf = key->fShifted;
          break;
        }
        case 0: {
          kf = key->primary;
          break;
        }
      }
      if(keyFunc == kf && (!getSystemFlag(FLAG_USER) || getNthString((uint8_t *)userKeyLabel, j * 6 + keyStateCode + i) == 0)) {
        keyCode_t kc = j + 1;
        shiftF = (i == 1);
        shiftG = (i == 2);
        assignToKey(kc);
        return true;
      }
    }
  }
  return false;
}

void assignGetName2(void) {
  bool result = false;
  if(compareString(aimBuffer, "ENTER", CMP_NAME) == 0) {
    result = _assignToKey(ITM_ENTER);
  }
  else if(compareString(aimBuffer, "EXIT", CMP_NAME) == 0) {
    result = _assignToKey(ITM_EXIT);
  }
  else if(compareString(aimBuffer, "USER", CMP_NAME) == 0) {
    result = _assignToKey(ITM_USERMODE);
  }
  /*else if(compareString(aimBuffer, STD_alpha, CMP_NAME) == 0) {
    result = _assignToKey(ITM_AIM);
  }
  else if(compareString(aimBuffer, "f", CMP_NAME) == 0) {
    result = _assignToKey(ITM_SHIFTf);
  }
  else if(compareString(aimBuffer, "g", CMP_NAME) == 0) {
    result = _assignToKey(ITM_SHIFTg);
  }*/
  else if(compareString(aimBuffer, "CATALOG", CMP_NAME) == 0) {
    result = _assignToKey(-MNU_CATALOG);
  }
  else if(aimBuffer[0] == 0 && alphaCase == AC_LOWER) {
    result = _assignToKey(ITM_DOWN);
  }

  calcModeLeave();
  shiftF = shiftG = false;
  refreshScreen();

  if(!result) {
    displayCalcErrorMessage(ERROR_CANNOT_ASSIGN_HERE, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    errorMoreInfo("'%s' is an invalid name", aimBuffer);
  }
}
