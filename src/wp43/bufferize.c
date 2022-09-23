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

#include "bufferize.h"

#include "calcMode.h"
#include "charString.h"
#include "constantPointers.h"
#include "constants.h"
#include "conversionAngles.h"
#include "dateTime.h"
#include "debug.h"
#include "display.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "hal/time.h"
#include "items.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/toRect.h"
#include "mathematics/wp34s.h"
#include "matrix.h"
#include "programming/manage.h"
#include "registers.h"
#include "registerValueConversions.h"
#include "saveRestoreCalcState.h"
#include "screen.h"
#include "softmenus.h"
#include "sort.h"
#include "stack.h"
#include "ui/tam.h"
#include <string.h>

#include "wp43.h"

#if !defined(TESTSUITE_BUILD)
  void fnAim(uint16_t unusedButMandatoryParameter) {
    shiftF = false;
    shiftG = false;
    aimBuffer[0] = 0;
    calcModeEnter(cmAim);
    if(programRunStop != PGM_RUNNING) {
      entryStatus |= 0x01;
    }
  }



  uint16_t convertItemToSubOrSup(uint16_t item, int16_t subOrSup) {
    nextChar = NC_NORMAL;

    if(subOrSup == NC_SUBSCRIPT) {
      switch(item) {
        case ITM_alpha:    return ITM_SUB_alpha;
        case ITM_delta:    return ITM_SUB_delta;
        case ITM_mu:       return ITM_SUB_mu;
        case ITM_SUN:      return ITM_SUB_SUN;
        case ITM_h:        return ITM_SUB_h;
        case ITM_t:        return ITM_SUB_t;
        case ITM_INFINITY: return ITM_SUB_INFINITY;
        case ITM_s:        return ITM_SUB_s;
        case ITM_PLUS:     return ITM_SUB_PLUS;
        case ITM_MINUS:    return ITM_SUB_MINUS;
        case ITM_0:        return ITM_SUB_0;
        case ITM_1:        return ITM_SUB_1;
        case ITM_2:        return ITM_SUB_2;
        case ITM_3:        return ITM_SUB_3;
        case ITM_4:        return ITM_SUB_4;
        case ITM_5:        return ITM_SUB_5;
        case ITM_6:        return ITM_SUB_6;
        case ITM_7:        return ITM_SUB_7;
        case ITM_8:        return ITM_SUB_8;
        case ITM_9:        return ITM_SUB_9;
        case ITM_a:        return ITM_SUB_a;
        case ITM_b:        return ITM_SUB_b;
        case ITM_c:        return ITM_SUB_c;
        case ITM_d:        return ITM_SUB_d;
        case ITM_e:        return ITM_SUB_e;
        case ITM_i:        return ITM_SUB_i;
        case ITM_j:        return ITM_SUB_j;
        case ITM_k:        return ITM_SUB_k;
        case ITM_l:        return ITM_SUB_l;
        case ITM_m:        return ITM_SUB_m;
        case ITM_n:        return ITM_SUB_n;
        case ITM_o:        return ITM_SUB_o;
        case ITM_p:        return ITM_SUB_p;
        case ITM_q:        return ITM_SUB_q;
        case ITM_u:        return ITM_SUB_u;
        case ITM_v:        return ITM_SUB_v;
        case ITM_w:        return ITM_SUB_w;
        case ITM_x:        return ITM_SUB_x;
        case ITM_y:        return ITM_SUB_y;
        case ITM_z:        return ITM_SUB_z;
        case ITM_A:        return ITM_SUB_A;
        case ITM_B:        return ITM_SUB_B;
        case ITM_C:        return ITM_SUB_C;
        case ITM_D:        return ITM_SUB_D;
        case ITM_E:        return ITM_SUB_E;
        case ITM_F:        return ITM_SUB_F;
        case ITM_G:        return ITM_SUB_G;
        case ITM_H:        return ITM_SUB_H;
        case ITM_I:        return ITM_SUB_I;
        case ITM_J:        return ITM_SUB_J;
        case ITM_K:        return ITM_SUB_K;
        case ITM_L:        return ITM_SUB_L;
        case ITM_M:        return ITM_SUB_M;
        case ITM_N:        return ITM_SUB_N;
        case ITM_O:        return ITM_SUB_O;
        case ITM_P:        return ITM_SUB_P;
        case ITM_Q:        return ITM_SUB_Q;
        case ITM_R:        return ITM_SUB_R;
        case ITM_S:        return ITM_SUB_S;
        case ITM_T:        return ITM_SUB_T;
        case ITM_U:        return ITM_SUB_U;
        case ITM_V:        return ITM_SUB_V;
        case ITM_W:        return ITM_SUB_W;
        case ITM_X:        return ITM_SUB_X;
        case ITM_Y:        return ITM_SUB_Y;
        case ITM_Z:        return ITM_SUB_Z;
        default:           return item;
      }
    }
    else if(subOrSup == NC_SUPERSCRIPT) {
      switch(item) {
        case ITM_a:        return ITM_SUP_a;
        case ITM_x:        return ITM_SUP_x;
        case ITM_INFINITY: return ITM_SUP_INFINITY;
        case ITM_PLUS:     return ITM_SUP_PLUS;
        case ITM_MINUS:    return ITM_SUP_MINUS;
        case ITM_0:        return ITM_SUP_0;
        case ITM_1:        return ITM_SUP_1;
        case ITM_2:        return ITM_SUP_2;
        case ITM_3:        return ITM_SUP_3;
        case ITM_4:        return ITM_SUP_4;
        case ITM_5:        return ITM_SUP_5;
        case ITM_6:        return ITM_SUP_6;
        case ITM_7:        return ITM_SUP_7;
        case ITM_8:        return ITM_SUP_8;
        case ITM_9:        return ITM_SUP_9;
        case ITM_f:        return ITM_SUP_f;
        case ITM_g:        return ITM_SUP_g;
        case ITM_h:        return ITM_SUP_h;
        case ITM_r:        return ITM_SUP_r;
        case ITM_T:        return ITM_SUP_T;
        default:           return item;
      }
    }

    return item;
  }



  int32_t findFirstItem(const char *twoLetters) {
    int16_t menuId = softmenuStack[0].softmenuId;

    if(menuId < NUMBER_OF_DYNAMIC_SOFTMENUS) { // Dynamic softmenu
      uint8_t *firstString, *middleString;
      int16_t first, middle, last;

      first = 0;
      firstString = dynamicSoftmenu[menuId].menuContent;

      last = dynamicSoftmenu[menuId].numItems - 1;

      middle = first + (last - first) / 2;
      middleString = getNthString(dynamicSoftmenu[menuId].menuContent, middle);

      while(first + 1 < last) {
        if(compareString(twoLetters, (char *)middleString, CMP_CLEANED_STRING_ONLY) <= 0) {
          last = middle;
        }
        else {
          first = middle;
          firstString = middleString;
        }

        middle = first + (last - first) / 2;
        middleString = getNthString(dynamicSoftmenu[menuId].menuContent, middle);
      }

      if(compareString(twoLetters, (char *)firstString, CMP_CLEANED_STRING_ONLY) <= 0) {
        return first;
      }
      else {
        return last;
      }
    }

    else { // Static softmenu
      const int16_t *first, *middle, *last;
      first = softmenu[menuId].softkeyItem;
      last = first + softmenu[menuId].numItems - 1;
      while(*last == ITM_NULL) {
        last--;
      }

      middle = first + (last - first) / 2;
      //const int16_t *f = softmenu[menuId].softkeyItem;
      //printf("\n----------------------------------\nfirst  = %3" PRIu64 "   %3d\n", (int64_t)(first - f), *first);
      //printf("middle = %3" PRIu64 "   %3d\n", (int64_t)(middle - f), *middle);
      //printf("last   = %3" PRIu64 "   %3d\n", (int64_t)(last - f), *last);
      while(first + 1 < last) {
        if(compareString(twoLetters, indexOfItems[abs(*middle)].itemCatalogName, CMP_CLEANED_STRING_ONLY) <= 0) {
          last = middle;
        }
        else {
          first = middle;
        }

        middle = first + (last - first) / 2;
      //printf("\nfirst  = %3" PRIu64 "   %3d\n", (int64_t)(first - f), *first);
      //printf("middle = %3" PRIu64 "   %3d\n", (int64_t)(middle - f), *middle);
      //printf("last   = %3" PRIu64 "   %3d\n", (int64_t)(last - f), *last);
      }

      if(compareString(twoLetters, indexOfItems[abs(*first)].itemCatalogName, CMP_CLEANED_STRING_ONLY) <= 0) {
        return first - softmenu[menuId].softkeyItem;
      }
      else {
        return last - softmenu[menuId].softkeyItem;
      }
    }
  }


  void resetAlphaSelectionBuffer(void) {
    lgCatalogSelection = 0;
    alphaSelectionTimer = 0;
    asmBuffer[0] = 0;
    fnKeyInCatalog = 0;
  }



  void addItemToBuffer(uint16_t item) {
    if(item == NOPARAM) {
      displayBugScreen("In function addItemToBuffer: item should not be NOPARAM=7654!");
    }
    else {
      screenUpdatingMode &= ~(SCRUPD_MANUAL_STACK | SCRUPD_MANUAL_SHIFT_STATUS);
      currentSolverStatus &= ~SOLVER_STATUS_READY_TO_EXECUTE;
      if(calcMode == cmNormal && fnKeyInCatalog && isAlphabeticSoftmenu()) {
        fnAim(NOPARAM);
      }
      if((fnKeyInCatalog || !catalog || catalog == CATALOG_MVAR) && (((calcMode == cmAim || calcMode == cmEim) && !tam.mode) || tam.alpha)) {
        item = convertItemToSubOrSup(item, nextChar);
        if(stringByteLength(aimBuffer) + stringByteLength(indexOfItems[item].itemSoftmenuName) >= AIM_BUFFER_LENGTH) { /// TODO this error should never happen but who knows!
          sprintf(errorMessage, "In function addItemToBuffer: the AIM input buffer is full! %d bytes for now", AIM_BUFFER_LENGTH);
          displayBugScreen(errorMessage);
        }
        else if(calcMode == cmEim) {
          const char *addChar = item == ITM_PAIR_OF_PARENTHESES ? "()" :
                                item == ITM_VERTICAL_BAR        ? "||" :
                                item == ITM_ROOT_SIGN           ? STD_SQUARE_ROOT "()" :
                                item == ITM_ALOG_SYMBOL         ? "e" STD_SUB_E "^()" :
                                indexOfItems[item].itemSoftmenuName;
          char *aimCursorPos = aimBuffer;
          char *aimBottomPos = aimBuffer + stringByteLength(aimBuffer);
          uint32_t itemLen = stringByteLength(addChar);
          for(uint32_t i = 0; i < xCursor; ++i) {
            aimCursorPos += (*aimCursorPos & 0x80) ? 2 : 1;
          }
          for(; aimBottomPos >= aimCursorPos; --aimBottomPos) {
            *(aimBottomPos + itemLen) = *aimBottomPos;
          }
          xcopy(aimCursorPos, addChar, itemLen);
          switch(item) {
            case ITM_ALOG_SYMBOL: {
              xCursor += 4;
              break;
            }
            case ITM_ROOT_SIGN: {
              xCursor += 2;
              break;
            }
            case ITM_PAIR_OF_PARENTHESES:
            case ITM_VERTICAL_BAR: {
              xCursor += 1;
              break;
            }
            default: {
              xCursor += stringGlyphLength(indexOfItems[item].itemSoftmenuName);
            }
          }
        }
        else {
          xcopy(aimBuffer + stringByteLength(aimBuffer), indexOfItems[item].itemSoftmenuName, stringByteLength(indexOfItems[item].itemSoftmenuName) + 1);
        }
      }

      if(catalog && catalog != CATALOG_MVAR && !fnKeyInCatalog) {
        if(item == ITM_BACKSPACE) {
          calcModeEnter(cmNormal);
          return;
        }

        // NOP if not a single character input for search
        // or if we already have two characters in the search buffer
        else if(stringGlyphLength(indexOfItems[item].itemSoftmenuName) == 1 &&
                (lgCatalogSelection < ((asmBuffer[0] & 0x80) ? 3 : 2)) &&
                currentSoftmenuScrolls()) {
          int32_t pos = lgCatalogSelection++;
          if(asmBuffer[pos] != 0) {
            pos++;
          }

          asmBuffer[pos++] = indexOfItems[item].itemSoftmenuName[0];
          if(indexOfItems[item].itemSoftmenuName[0] & 0x80) { // 2 bytes
            asmBuffer[pos++] = indexOfItems[item].itemSoftmenuName[1];
          }
          asmBuffer[pos] = 0;

          softmenuStack[0].firstItem = findFirstItem(asmBuffer);
          setCatalogLastPos();
          alphaSelectionTimer = timeUptimeMs();
        }
      }

      else if(tam.mode) {
        tamProcessInput(item);
      }

      else if(calcMode == cmNim) {
        addItemToNimBuffer(item);
      }

      //Probably wrong place for this function?! Should Arrow be processed in buffercize.c in this case? //Switch statement better.
      else if(calcMode == cmMim) {
        if(temporaryInformation == TI_SHOW_REGISTER) {
          temporaryInformation = TI_NO_INFO;
        }

        if(item == ITM_RIGHT_ARROW) {
          mimEnter(true);
          setJRegisterAsInt(true, getJRegisterAsInt(true) + 1);
          refreshScreen();
        }
        else if(item == ITM_LEFT_ARROW) {
          mimEnter(true);
          setJRegisterAsInt(true, getJRegisterAsInt(true) - 1);
          refreshScreen();
        }
        else if(item == ITM_UP_ARROW) {
          mimEnter(true);
          setIRegisterAsInt(true, getIRegisterAsInt(true) - 1);
          refreshScreen();
        }
        else if(item == ITM_DOWN_ARROW) {
          mimEnter(true);
          setIRegisterAsInt(true, getIRegisterAsInt(true) + 1);
          refreshScreen();
        }

        if((int16_t)item < 0) {
          showSoftmenu(item);
          refreshScreen();
          return;
        }

        switch(item) {
          case ITM_EXPONENT:
          case ITM_PERIOD:
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
          case ITM_CHS: // +/-
          case ITM_CONSTpi: {
            mimAddNumber(item);
            break;
          }

          case ITM_STO:
          case ITM_STOADD:
          case ITM_STOSUB:
          case ITM_STOMULT:
          case ITM_STODIV:
          case ITM_STOMAX:
          case ITM_STOMIN:
          case ITM_RCL:
          case ITM_RCLADD:
          case ITM_RCLSUB:
          case ITM_RCLMULT:
          case ITM_RCLDIV:
          case ITM_RCLMAX:
          case ITM_RCLMIN:
          case ITM_CF:
          case ITM_SF:
          case ITM_FF:
          case ITM_CNST:
          case ITM_ALL:
          case ITM_ENG:
          case ITM_FIX:
          case ITM_DSP:
          case ITM_SCI:
          case ITM_SDL:
          case ITM_SDR:
          case ITM_RDP:
          case ITM_RM:
          case ITM_RSD:
          case ITM_DEG:
          case ITM_GRAD:
          case ITM_MULPI:
          case ITM_RAD: {
            lastErrorCode = ERROR_NONE;
            mimEnter(true);
            runFunction(item);
            break;
          }

          case ITM_SQUARE:
          case ITM_CUBE:
          case ITM_SQUAREROOTX:
          case ITM_CUBEROOT:
          case ITM_2X:
          case ITM_EXP:
          case ITM_ROUND:
          case ITM_10x:
          case ITM_LOG2:
          case ITM_LN:
          case ITM_LOG10:
          case ITM_1ONX:
          case ITM_cos:
          case ITM_cosh:
          case ITM_sin:
          case ITM_sinh:
          case ITM_tan:
          case ITM_tanh:
          case ITM_arccos:
          case ITM_arcosh:
          case ITM_arcsin:
          case ITM_arsinh:
          case ITM_arctan:
          case ITM_artanh:
          case ITM_CEIL:
          case ITM_FLOOR:
          case ITM_DEC:
          case ITM_INC:
          case ITM_IP:
          case ITM_FP:
          case ITM_MAGNITUDE:
          case ITM_NEXTP:
          case ITM_XFACT:
          case ITM_toDEG:
          case ITM_toDMS:
          case ITM_toGRAD:
          case ITM_toMULpi:
          case ITM_toRAD:
          case ITM_DtoR:
          case ITM_RtoD:
          case ITM_LOGICALNOT:
          case CST_01:
          case CST_02:
          case CST_03:
          case CST_04:
          case CST_05:
          case CST_06:
          case CST_07:
          case CST_08:
          case CST_09:
          case CST_10:
          case CST_11:
          case CST_12:
          case CST_13:
          case CST_14:
          case CST_15:
          case CST_16:
          case CST_17:
          case CST_18:
          case CST_19:
          case CST_20:
          case CST_21:
          case CST_22:
          case CST_23:
          case CST_24:
          case CST_25:
          case CST_26:
          case CST_27:
          case CST_28:
          case CST_29:
          case CST_30:
          case CST_31:
          case CST_32:
          case CST_33:
          case CST_34:
          case CST_35:
          case CST_36:
          case CST_37:
          case CST_38:
          case CST_39:
          case CST_40:
          case CST_41:
          case CST_42:
          case CST_43:
          case CST_44:
          case CST_45:
          case CST_46:
          case CST_47:
          case CST_48:
          case CST_49:
          case CST_50:
          case CST_51:
          case CST_52:
          case CST_53:
          case CST_54:
          case CST_55:
          case CST_56:
          case CST_57:
          case CST_58:
          case CST_59:
          case CST_60:
          case CST_61:
          case CST_62:
          case CST_63:
          case CST_64:
          case CST_65:
          case CST_66:
          case CST_67:
          case CST_68:
          case CST_69:
          case CST_70:
          case CST_71:
          case CST_72:
          case CST_73:
          case CST_74:
          case CST_75:
          case CST_76:
          case CST_77:
          case CST_78:
          case CST_79:
          case ITM_CtoF:
          case ITM_FtoC:
          case ITM_DBtoPR:
          case ITM_DBtoPRb:
          case ITM_DBtoPRc:
          case ITM_DBtoFR:
          case ITM_DBtoFRb:
          case ITM_DBtoFRc:
          case ITM_PRtoDB:
          case ITM_PRtoDBb:
          case ITM_PRtoDBc:
          case ITM_FRtoDB:
          case ITM_FRtoDBb:
          case ITM_FRtoDBc:
          case ITM_ACtoHA:
          case ITM_ACtoHAb:
          case ITM_HAtoAC:
          case ITM_HAtoACb:
          case ITM_ACUStoHA:
          case ITM_ACUStoHAb:
          case ITM_HAtoACUS:
          case ITM_HAtoACUSb:
          case ITM_PAtoATM:
          case ITM_ATMtoPA:
          case ITM_AUtoM:
          case ITM_MtoAU:
          case ITM_BARtoPA:
          case ITM_PAtoBAR:
          case ITM_BTUtoJ:
          case ITM_JtoBTU:
          case ITM_CALtoJ:
          case ITM_JtoCAL:
          case ITM_LBFFTtoNM:
          case ITM_LBFFTtoNMb:
          case ITM_NMtoLBFFT:
          case ITM_NMtoLBFFTb:
          case ITM_CWTtoKG:
          case ITM_KGtoCWT:
          case ITM_FTtoM:
          case ITM_MtoFT:
          case ITM_FTUStoM:
          case ITM_FTUStoMb:
          case ITM_FTUStoMc:
          case ITM_MtoFTUS:
          case ITM_MtoFTUSb:
          case ITM_MtoFTUSc:
          case ITM_FZUKtoML:
          case ITM_FZUKtoMLb:
          case ITM_MLtoFZUK:
          case ITM_MLtoFZUKb:
          case ITM_FZUStoML:
          case ITM_FZUStoMLb:
          case ITM_MLtoFZUS:
          case ITM_MLtoFZUSb:
          case ITM_GLUKtoL:
          case ITM_LtoGLUK:
          case ITM_GLUStoL:
          case ITM_LtoGLUS:
          case ITM_HPEtoW:
          case ITM_WtoHPE:
          case ITM_HPMtoW:
          case ITM_WtoHPM:
          case ITM_HPUKtoW:
          case ITM_WtoHPUK:
          case ITM_INCHHGtoPA:
          case ITM_INCHHGtoPAb:
          case ITM_PAtoINCHHG:
          case ITM_PAtoINCHHGb:
          case ITM_INCHtoMM:
          case ITM_MMtoINCH:
          case ITM_WHtoJ:
          case ITM_JtoWH:
          case ITM_KGtoLBS:
          case ITM_LBStoKG:
          case ITM_GtoOZ:
          case ITM_OZtoG:
          case ITM_KGtoSCW:
          case ITM_KGtoSCWb:
          case ITM_SCWtoKG:
          case ITM_SCWtoKGb:
          case ITM_KGtoSTO:
          case ITM_KGtoSTOb:
          case ITM_STOtoKG:
          case ITM_STOtoKGb:
          case ITM_KGtoST:
          case ITM_KGtoSTb:
          case ITM_KGtoSTc:
          case ITM_STtoKG:
          case ITM_STtoKGb:
          case ITM_STtoKGc:
          case ITM_KGtoTON:
          case ITM_KGtoLIANG:
          case ITM_KGtoLIANGb:
          case ITM_TONtoKG:
          case ITM_LIANGtoKG:
          case ITM_LIANGtoKGb:
          case ITM_GtoTRZ:
          case ITM_GtoTRZb:
          case ITM_TRZtoG:
          case ITM_TRZtoGb:
          case ITM_LBFtoN:
          case ITM_NtoLBF:
          case ITM_LYtoM:
          case ITM_MtoLY:
          case ITM_MMHGtoPA:
          case ITM_MMHGtoPAb:
          case ITM_PAtoMMHG:
          case ITM_PAtoMMHGb:
          case ITM_MItoKM:
          case ITM_KMtoMI:
          case ITM_KMtoNMI:
          case ITM_NMItoKM:
          case ITM_MtoPC:
          case ITM_PCtoM:
          case ITM_MMtoPOINT:
          case ITM_MMtoPOINTb:
          case ITM_POINTtoMM:
          case ITM_POINTtoMMb:
          case ITM_MtoYD:
          case ITM_YDtoM:
          case ITM_PSItoPA:
          case ITM_PAtoPSI:
          case ITM_PAtoTOR:
          case ITM_PAtoTORb:
          case ITM_TORtoPA:
          case ITM_TORtoPAb:
          case ITM_StoYEAR:
          case ITM_YEARtoS:
          case ITM_CARATtoG:
          case ITM_CARATtoGb:
          case ITM_JINtoKG:
          case ITM_GtoCARAT:
          case ITM_GtoCARATb:
          case ITM_KGtoJIN:
          case ITM_QTtoL:
          case ITM_LtoQT:
          case ITM_FATHOMtoM:
          case ITM_FATHOMtoMb:
          case ITM_MtoFATHOM:
          case ITM_MtoFATHOMb:
          case ITM_BARRELtoM3:
          case ITM_BARRELtoM3b:
          case ITM_M3toBARREL:
          case ITM_M3toBARRELb:
          case ITM_ATMtoPAb:
          case ITM_PAtoATMb:
          case ITM_HECTAREtoM2:
          case ITM_M2toHECTARE:
          case ITM_MUtoM2:
          case ITM_M2toMU:
          case ITM_LItoM:
          case ITM_MtoLI:
          case ITM_CHItoM:
          case ITM_MtoCHI:
          case ITM_YINtoM:
          case ITM_MtoYIN:
          case ITM_CUNtoM:
          case ITM_MtoCUN:
          case ITM_ZHANGtoM:
          case ITM_ZHANGtoMb:
          case ITM_MtoZHANG:
          case ITM_MtoZHANGb:
          case ITM_FENtoM:
          case ITM_MtoFEN:
          case ITM_MILEtoM:
          case ITM_MtoMILE:
          case ITM_NMItoM:
          case ITM_MtoNMI:
          case ITM_NSIGMA:
          case ITM_SIGMAx:
          case ITM_SIGMAy:
          case ITM_SIGMAx2:
          case ITM_SIGMAx2y:
          case ITM_SIGMAy2:
          case ITM_SIGMAxy:
          case ITM_SIGMAlnxy:
          case ITM_SIGMAlnx:
          case ITM_SIGMAln2x:
          case ITM_SIGMAylnx:
          case ITM_SIGMAlny:
          case ITM_SIGMAln2y:
          case ITM_SIGMAxlny:
          case ITM_SIGMAx2lny:
          case ITM_SIGMAlnyonx:
          case ITM_SIGMAx2ony:
          case ITM_SIGMA1onx:
          case ITM_SIGMA1onx2:
          case ITM_SIGMAxony:
          case ITM_SIGMA1ony:
          case ITM_SIGMA1ony2:
          case ITM_SIGMAx3:
          case ITM_SIGMAx4:
          case ITM_ABS:
          case ITM_BATT:
          case ITM_BN:
          case ITM_BNS:
          case ITM_CONJ:
          case ITM_CORR:
          case ITM_COV:
          case ITM_BESTFQ:
          case ITM_DAY:
          case ITM_DEGto:
          case ITM_DMSto:
          case ITM_DtoJ:
          case ITM_ERF:
          case ITM_ERFC:
          case ITM_EXPT:
          case ITM_FIB:
          case ITM_FLASH:
          case ITM_GD:
          case ITM_GDM1:
          case ITM_GRADto:
          case ITM_IM:
          case ITM_JtoD:
          case ITM_LASTX:
          case ITM_LNGAMMA:
          case ITM_LocRQ:
          case ITM_MANT:
          case ITM_MEM:
          case ITM_MONTH:
          case ITM_sincpi:
          case ITM_RADto:
          case ITM_RAN:
          case ITM_RE:
          case ITM_REexIM:
          case ITM_RMQ:
          case ITM_EX1:
          case ITM_ROUNDI:
          case ITM_SDIGS:
          case ITM_SIGN:
          case ITM_ISM:
          case ITM_SMW:
          case ITM_SSIZE:
          case ITM_LN1X:
          case ITM_SW:
          case ITM_SXY:
          case ITM_TICKS:
          case ITM_UNITV:
          case ITM_WDAY:
          case ITM_WM:
          case ITM_WP:
          case ITM_WM1:
          case ITM_XW:
          case ITM_YEAR:
          case ITM_GAMMAX:
          case ITM_zetaX:
          case ITM_STDDEVPOP:
          case ITM_M1X:
          case ITM_toHMS:
          case ITM_MPItoR:
          case ITM_RtoMPI:
          case ITM_toREAL:
          case ITM_DtoDMS:
          case ITM_MULPIto:
          case ITM_Kk:
          case ITM_Ek: {
            mimRunFunction(item, indexOfItems[item].param);
            break;
          }

          case ITM_ANGLE: {
            mimRunFunction(ITM_ARG, indexOfItems[ITM_ARG].param);
            break;
          }

          case ITM_SHOW: {
            mimEnter(true);
            temporaryInformation = TI_SHOW_REGISTER;
            break;
          }

          case ITM_OFF: {
            runFunction(ITM_OFF);
            break;
          }
        }
      }

      else if(calcMode != cmAim && calcMode != cmEim && calcMode != cmAssign && (item >= ITM_A && item <= ITM_F)) {
        // We are not in NIM, but should enter NIM - this should be handled here
        // unlike digits 0 to 9 which are handled by processKeyAction
        addItemToNimBuffer(item);
      }

      else if(calcMode != cmAim && calcMode != cmEim) {
        funcOK = false;
        return;
      }

      funcOK = true;
    }
  }



  void addItemToNimBuffer(int16_t item) {
    int16_t lastChar, index;
    uint8_t savedNimNumberPart;
    bool_t done;
    char *strBase;

    screenUpdatingMode &= ~(SCRUPD_MANUAL_STACK | SCRUPD_MANUAL_SHIFT_STATUS);
    currentSolverStatus &= ~SOLVER_STATUS_READY_TO_EXECUTE;

    if(calcMode == cmNormal) {
      switch(item) {
        case ITM_EXPONENT: {
          calcModeEnter(cmNim);
          aimBuffer[0] = '+';
          aimBuffer[1] = '1';
          aimBuffer[2] = '.';
          aimBuffer[3] = 0;
          nimNumberPart = NP_REAL_FLOAT_PART;
          lastIntegerBase = 0;
          break;
        }

        case ITM_PERIOD: {
          calcModeEnter(cmNim);
          aimBuffer[0] = '+';
          aimBuffer[1] = '0';
          aimBuffer[2] = 0;
          nimNumberPart = NP_INT_10;
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
        case ITM_9:
        case ITM_A:
        case ITM_B:
        case ITM_C:
        case ITM_D:
        case ITM_E:
        case ITM_F: {
          calcModeEnter(cmNim);
          aimBuffer[0] = '+';
          aimBuffer[1] = 0;
          nimNumberPart = NP_EMPTY;
          break;
        }

        default: {
          sprintf(errorMessage, "In function addItemToNimBuffer: %d is an unexpected item value when initializing NIM!", item);
          displayBugScreen(errorMessage);
          return;
        }
      }

      if(programRunStop != PGM_RUNNING) {
        entryStatus |= 0x01;
      }

      //debugNIM();
    }

    else if(calcMode == cmNim) {
      screenUpdatingMode |= SCRUPD_SKIP_STACK_ONE_TIME;
    }

    done = false;

    switch(item) {
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
        done = true;

        switch(nimNumberPart) {
          case NP_INT_10: {
            if(item == ITM_0) {
              if(aimBuffer[1] != '0') {
                strcat(aimBuffer, "0");
              }
            }
            else {
              if(aimBuffer[1] == '0') {
                aimBuffer[1] = 0;
              }

              strcat(aimBuffer, indexOfItems[item].itemSoftmenuName);
            }
            break;
          }

          case NP_REAL_EXPONENT: {
            if(item == ITM_0) {
              if(aimBuffer[exponentSignLocation + 1] == '0') {
                aimBuffer[strlen(aimBuffer) - 1] = 0;
              }

              if(aimBuffer[exponentSignLocation + 1] != 0 || aimBuffer[exponentSignLocation] == '+') {
                strcat(aimBuffer, "0");
              }

              if(stringToInt16(aimBuffer + exponentSignLocation) > exponentLimit || stringToInt16(aimBuffer + exponentSignLocation) < -exponentLimit) {
                aimBuffer[strlen(aimBuffer) - 1] = 0;
              }
            }
            else {
              if(aimBuffer[exponentSignLocation + 1] == '0') {
                aimBuffer[strlen(aimBuffer) - 1] = 0;
              }

              strcat(aimBuffer, indexOfItems[item].itemSoftmenuName);

              if(stringToInt16(aimBuffer + exponentSignLocation) > exponentLimit || stringToInt16(aimBuffer + exponentSignLocation) < -exponentLimit) {
                aimBuffer[strlen(aimBuffer) - 1] = 0;
              }
            }
            break;
          }

          case NP_FRACTION_DENOMINATOR: {
            if(item == ITM_0) {
              strcat(aimBuffer, "0");

              if(aimBuffer[denominatorLocation] == '0') {
                aimBuffer[denominatorLocation] = 0;
              }

              if(stringToInt32(aimBuffer + denominatorLocation) > 9999) {
                aimBuffer[strlen(aimBuffer) - 1] = 0;
              }
            }
            else {
              strcat(aimBuffer, indexOfItems[item].itemSoftmenuName);

              if(stringToInt32(aimBuffer + denominatorLocation) > 9999) {
                aimBuffer[strlen(aimBuffer) - 1] = 0;
              }
            }
            break;
          }

          case NP_COMPLEX_INT_PART: {
            if(item == ITM_0) {
              if(aimBuffer[imaginaryMantissaSignLocation + 2] != '0') {
                strcat(aimBuffer, "0");
              }
            }
            else {
              if(aimBuffer[imaginaryMantissaSignLocation + 2] == '0') {
                aimBuffer[imaginaryMantissaSignLocation + 2] = 0;
              }

              strcat(aimBuffer, indexOfItems[item].itemSoftmenuName);
            }
            break;
          }

          case NP_COMPLEX_EXPONENT: {
            if(item == ITM_0) {
              if(aimBuffer[imaginaryExponentSignLocation + 1] == '0') {
                aimBuffer[strlen(aimBuffer) - 1] = 0;
              }

              if(aimBuffer[imaginaryExponentSignLocation + 1] != 0 || aimBuffer[imaginaryExponentSignLocation] == '+') {
                strcat(aimBuffer, "0");
              }

              if(stringToInt16(aimBuffer + imaginaryExponentSignLocation) > exponentLimit || stringToInt16(aimBuffer + imaginaryExponentSignLocation) < -exponentLimit) {
                aimBuffer[strlen(aimBuffer) - 1] = 0;
              }
            }
            else {
              if(aimBuffer[imaginaryExponentSignLocation + 1] == '0') {
                aimBuffer[strlen(aimBuffer) - 1] = 0;
              }

              strcat(aimBuffer, indexOfItems[item].itemSoftmenuName);

              if(stringToInt16(aimBuffer + imaginaryExponentSignLocation) > exponentLimit || stringToInt16(aimBuffer + imaginaryExponentSignLocation) < -exponentLimit) {
                aimBuffer[strlen(aimBuffer) - 1] = 0;
              }
            }
            break;
          }

          case NP_INT_BASE: {
            strcat(aimBuffer, indexOfItems[item].itemSoftmenuName);
            strBase = strchr(aimBuffer, '#') + 1;
            if(atoi(strBase) > 16) {
              strBase[1] = 0;
            }
            else if(atoi(strBase) >= 2) {
              goto addItemToNimBuffer_exit;
            }
            break;
          }

          default: {
            if(nimNumberPart == NP_EMPTY) {
              nimNumberPart = NP_INT_10;
              //debugNIM();
            }

            strcat(aimBuffer, indexOfItems[item].itemSoftmenuName);
          }
        }
        break;
      }

      case ITM_A:
      case ITM_B:
      case ITM_C:
      case ITM_D:
      case ITM_E:
      case ITM_F: {
        done = true;

        if(nimNumberPart == NP_EMPTY || nimNumberPart == NP_INT_10 || nimNumberPart == NP_INT_16) {
          if(aimBuffer[1] == '0') {
            aimBuffer[1] = 0;
          }

          strcat(aimBuffer, indexOfItems[item].itemSoftmenuName);
          hexDigits++;

          nimNumberPart = NP_INT_16;
          //debugNIM();
        }
        break;
      }

      case ITM_PERIOD: {
        done = true;

        if(aimBuffer[strlen(aimBuffer)-1] == 'i') {
          strcat(aimBuffer, "0");
        }

        lastIntegerBase = 0;

        switch(nimNumberPart) {
          case NP_INT_10: {
            strcat(aimBuffer, ".");

            nimNumberPart = NP_REAL_FLOAT_PART;
            //debugNIM();
            break;
          }

          case NP_REAL_FLOAT_PART: {
            if(aimBuffer[strlen(aimBuffer) - 1] == '.') {
              strcat(aimBuffer, "0");
            }

            for(uint16_t i=0; i<strlen(aimBuffer); i++) {
              if(aimBuffer[i] == '.') {
                aimBuffer[i] = ' ';
                break;
              }
            }
            strcat(aimBuffer, "/");

            denominatorLocation = strlen(aimBuffer);
            nimNumberPart = NP_FRACTION_DENOMINATOR;
            //debugNIM();
            break;
          }

          case NP_COMPLEX_INT_PART: {
            strcat(aimBuffer, ".");

            nimNumberPart = NP_COMPLEX_FLOAT_PART;
            //debugNIM();
            break;
          }

          default: {
          }
        }
        break;
      }

      case ITM_EXPONENT: {
        done = true;

        if(aimBuffer[strlen(aimBuffer)-1] == 'i') {
          strcat(aimBuffer, "1");
        }

        lastIntegerBase = 0;

        switch(nimNumberPart) {
          case NP_INT_10: {
            strcat(aimBuffer, "."); // no break here
            #if !defined(OSX)
              __attribute__ ((fallthrough));
            #endif // !OSX
          }
          case NP_REAL_FLOAT_PART: {
            strcat(aimBuffer, "e+");
            exponentSignLocation = strlen(aimBuffer) - 1;

            nimNumberPart = NP_REAL_EXPONENT;
            //debugNIM();
            break;
          }
          case NP_COMPLEX_INT_PART: {
            strcat(aimBuffer, "."); // no break here
            #if !defined(OSX)
              __attribute__ ((fallthrough));
            #endif // !OSX
          }
          case NP_COMPLEX_FLOAT_PART: {
            strcat(aimBuffer, "e+");
            imaginaryExponentSignLocation = strlen(aimBuffer) - 1;

            nimNumberPart = NP_COMPLEX_EXPONENT;
            //debugNIM();
            break;
          }
          default: {
          }
        }
        break;
      }

      case ITM_toINT: { // #
        done = true;

        lastIntegerBase = 0;

        if(nimNumberPart == NP_INT_10 || nimNumberPart == NP_INT_16) {
          strcat(aimBuffer, "#");

          nimNumberPart = NP_INT_BASE;
          //debugNIM();
        }
        break;
      }

      case ITM_CHS: { // +/-
        done = true;

        switch(nimNumberPart) {
          case NP_INT_10:
          case NP_INT_16:
          case NP_INT_BASE:
          case NP_REAL_FLOAT_PART:
          case NP_FRACTION_DENOMINATOR: {
            if(aimBuffer[0] == '+') {
              aimBuffer[0] = '-';
            }
            else {
              aimBuffer[0] = '+';
            }
            break;
          }

          case NP_REAL_EXPONENT: {
            if(aimBuffer[exponentSignLocation] == '+') {
              aimBuffer[exponentSignLocation] = '-';
              if(aimBuffer[exponentSignLocation + 1] == '0') {
                aimBuffer[strlen(aimBuffer) - 1] = 0;
              }
            }
            else {
              aimBuffer[exponentSignLocation] = '+';
            }
            break;
          }

          case NP_COMPLEX_INT_PART:
          case NP_COMPLEX_FLOAT_PART: {
            if(aimBuffer[imaginaryMantissaSignLocation] == '+') {
              aimBuffer[imaginaryMantissaSignLocation] = '-';
            }
            else {
              aimBuffer[imaginaryMantissaSignLocation] = '+';
            }
            break;
          }

          case NP_COMPLEX_EXPONENT: {
            if(aimBuffer[imaginaryExponentSignLocation] == '+') {
              aimBuffer[imaginaryExponentSignLocation] = '-';
              if(aimBuffer[imaginaryExponentSignLocation + 1] == '0') {
                aimBuffer[strlen(aimBuffer) - 1] = 0;
              }
            }
            else {
              aimBuffer[imaginaryExponentSignLocation] = '+';
            }
            break;
          }

          default: {
          }
        }
        break;
      }

      case ITM_CC: {
        lastChar = strlen(aimBuffer) - 1;

        done = true;

        lastIntegerBase = 0;

        switch(nimNumberPart) {
         case NP_REAL_EXPONENT: {
            if((aimBuffer[lastChar] == '+' || aimBuffer[lastChar] == '-') && aimBuffer[lastChar - 1] == 'e') {
              aimBuffer[lastChar - 1] = 0;
            }
            else if(aimBuffer[lastChar] == 'e') {
              aimBuffer[lastChar] = 0;
            }
            else {
              imaginaryMantissaSignLocation = strlen(aimBuffer);
              strcat(aimBuffer, "+i");

              nimNumberPart = NP_COMPLEX_INT_PART;
              //debugNIM();
            }
            break;
         }

          case NP_INT_10: {
            strcat(aimBuffer, "."); // no break here
            #if !defined(OSX)
              __attribute__ ((fallthrough));
            #endif // !OSX
          }

          case NP_REAL_FLOAT_PART: {
            imaginaryMantissaSignLocation = strlen(aimBuffer);
            strcat(aimBuffer, "+i");

            nimNumberPart = NP_COMPLEX_INT_PART;
            //debugNIM();
            break;
          }
          default: {
          }
        }
        break;
      }

      case ITM_CONSTpi: {
        if(nimNumberPart == NP_COMPLEX_INT_PART && aimBuffer[strlen(aimBuffer) - 1] == 'i') {
          done = true;
          strcat(aimBuffer, "3.141592653589793238462643383279503");
          reallyRunFunction(ITM_EXIT, NOPARAM);
        }
        break;
      }

      case ITM_BACKSPACE: {
        lastChar = strlen(aimBuffer) - 1;

        done = true;

        switch(nimNumberPart) {
          case NP_INT_10: {
            break;
          }

          case NP_INT_16: {
            if(aimBuffer[lastChar] >= 'A') {
              hexDigits--;
            }

            if(hexDigits == 0) {
              nimNumberPart = NP_INT_10;
              //debugNIM();
            }
            break;
          }

          case NP_INT_BASE: {
            if(aimBuffer[lastChar] == '#') {
              if(hexDigits > 0) {
                nimNumberPart = NP_INT_16;
              }
              else {
                nimNumberPart = NP_INT_10;
              }
              //debugNIM();
            }
            break;
          }

          case NP_REAL_FLOAT_PART: {
            if(aimBuffer[lastChar] == '.') {
              nimNumberPart = NP_INT_10;
              //debugNIM();
            }
            break;
          }

          case NP_REAL_EXPONENT: {
            if(aimBuffer[lastChar] == '+' || aimBuffer[lastChar] == '-') {
              aimBuffer[lastChar--] = 0;
            }

            if(aimBuffer[lastChar] == 'e') {
              nimNumberPart = NP_INT_10;
              for(int16_t i=0; i<lastChar; i++) {
                if(aimBuffer[i] == '.') {
                  nimNumberPart = NP_REAL_FLOAT_PART;
                  break;
                }
              }
              //debugNIM();
            }
            break;
          }

          case NP_FRACTION_DENOMINATOR: {
            if(aimBuffer[lastChar] == '/') {
              nimNumberPart = NP_REAL_FLOAT_PART;
              for(int16_t i=0; i<lastChar; i++) {
                if(aimBuffer[i] == ' ') {
                  aimBuffer[i] = '.';
                  break;
                }
              }
              //debugNIM();
            }
            break;
          }

          case NP_COMPLEX_INT_PART: {
            if(aimBuffer[lastChar] == 'i') {
              nimNumberPart = NP_INT_10;
              for(int16_t i=0; i<lastChar; i++) {
                if(aimBuffer[i] == 'e') {
                  nimNumberPart = NP_REAL_EXPONENT;
                  break;
                }
                if(aimBuffer[i] == '.') {
                  nimNumberPart = NP_REAL_FLOAT_PART;
                }
              }
              //debugNIM();
              lastChar--;
            }
            break;
          }

          case NP_COMPLEX_FLOAT_PART: {
            if(aimBuffer[lastChar] == '.') {
              nimNumberPart = NP_COMPLEX_INT_PART;
              //debugNIM();
            }
            break;
          }

          case NP_COMPLEX_EXPONENT: {
            if(aimBuffer[lastChar] == '+' || aimBuffer[lastChar] == '-') {
              aimBuffer[lastChar--] = 0;
            }

            if(aimBuffer[lastChar] == 'e') {
              nimNumberPart = NP_COMPLEX_INT_PART;
              for(int16_t i=imaginaryMantissaSignLocation+2; i<lastChar; i++) {
                if(aimBuffer[i] == '.') {
                  nimNumberPart = NP_COMPLEX_FLOAT_PART;
                  break;
                }
              }
              //debugNIM();
            }
            break;
          }

          default: {
          }
        }

        aimBuffer[lastChar--] = 0;

        if((calcMode != cmMim) && (lastChar == -1 || (lastChar == 0 && aimBuffer[0] == '+'))) {
          screenUpdatingMode &= ~SCRUPD_SKIP_STACK_ONE_TIME;
          calcModeEnter(cmNormal);
          #if defined(DEBUGUNDO)
            printf(">>> undo from addItemToNimBuffer\n");
          #endif // DEBUGUNDO
          undo();
        }
        break;
      }

      case ITM_EXIT: {
        addItemToNimBuffer_exit:
        done = true;
        screenUpdatingMode &= ~SCRUPD_SKIP_STACK_ONE_TIME;
        closeNim();
        if(calcMode != cmNim && lastErrorCode == 0) {
          setSystemFlag(FLAG_ASLIFT);
          if(item == ITM_EXIT) {
            #if defined(DEBUGUNDO)
              printf(">>> saveForUndo from bufferizeA:");
            #endif // DEBUGUNDO
            saveForUndo();
            if(lastErrorCode == ERROR_RAM_FULL) {
              lastErrorCode = 0;
              temporaryInformation = TI_UNDO_DISABLED;
              #if (EXTRA_INFO_ON_CALC_ERROR == 1)
                moreInfoOnError("In function addItemToNimBuffer:", "there is not enough memory to save for undo!", NULL, NULL);
              #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
            }
          }
          return;
        }
        if(item == ITM_EXIT) {
          #if defined(DEBUGUNDO)
            printf(">>> saveForUndo from bufferizeB:");
          #endif // DEBUGUNDO
          saveForUndo();
          if(lastErrorCode == ERROR_RAM_FULL) {
            lastErrorCode = 0;
            temporaryInformation = TI_UNDO_DISABLED;
            #if (EXTRA_INFO_ON_CALC_ERROR == 1)
              moreInfoOnError("In function addItemToNimBuffer:", "there is not enough memory to save for undo!", NULL, NULL);
            #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
          }
        }
        break;
      }

      case ITM_YX: { // B for binary base
        if(nimNumberPart == NP_INT_BASE && aimBuffer[strlen(aimBuffer) - 1] == '#') {
          strcat(aimBuffer, "2");
          goto addItemToNimBuffer_exit;
        }
        break;
      }

      case ITM_LN: { // D for decimal base
        if(nimNumberPart == NP_INT_BASE && aimBuffer[strlen(aimBuffer) - 1] == '#') {
          strcat(aimBuffer, "10");
          goto addItemToNimBuffer_exit;
        }
        break;
      }

      case ITM_RCL: { // H for hexadecimal base
        if(nimNumberPart == NP_INT_BASE && aimBuffer[strlen(aimBuffer) - 1] == '#') {
          strcat(aimBuffer, "16");
          goto addItemToNimBuffer_exit;
        }
        break;
      }

      case ITM_DMS: {
        if(nimNumberPart == NP_INT_10 || nimNumberPart == NP_REAL_FLOAT_PART) {
          done = true;

          screenUpdatingMode &= ~SCRUPD_SKIP_STACK_ONE_TIME;
          closeNim();
          if(calcMode != cmNim && lastErrorCode == 0) {
            if(getRegisterDataType(REGISTER_X) == dtLongInteger) {
              convertLongIntegerRegisterToReal34Register(REGISTER_X, REGISTER_X);
            }

            real34FromDmsToDeg(REGISTER_REAL34_DATA(REGISTER_X), REGISTER_REAL34_DATA(REGISTER_X));
            setRegisterAngularMode(REGISTER_X, amDMS);

            setSystemFlag(FLAG_ASLIFT);
            return;
          }
        }
        break;
    }

      case ITM_dotD: {
        if(nimNumberPart == NP_REAL_FLOAT_PART) {
          done = true;

          screenUpdatingMode &= ~SCRUPD_SKIP_STACK_ONE_TIME;
          closeNim();
          if(calcMode != cmNim && lastErrorCode == 0) {
            convertReal34RegisterToDateRegister(REGISTER_X, REGISTER_X);
            checkDateRange(REGISTER_REAL34_DATA(REGISTER_X));

            if(lastErrorCode == 0) {
              setSystemFlag(FLAG_ASLIFT);
            }
            else {
              #if defined(DEBUGUNDO)
                printf(">>> undo from addItemToNimBufferB\n");
              #endif // DEBUGUNDO
              undo();
            }
            return;
          }
        }
        break;
    }

      case ITM_toHMS: {
        if(nimNumberPart == NP_INT_10 || nimNumberPart == NP_REAL_FLOAT_PART || nimNumberPart == NP_REAL_EXPONENT) {
          done = true;

          screenUpdatingMode &= ~SCRUPD_SKIP_STACK_ONE_TIME;
          closeNim();
          if(calcMode != cmNim && lastErrorCode == 0) {
            if(getRegisterDataType(REGISTER_X) == dtLongInteger) {
              convertLongIntegerRegisterToReal34Register(REGISTER_X, REGISTER_X);
            }

            hmmssInRegisterToSeconds(REGISTER_X);
            if(lastErrorCode == 0) {
              setSystemFlag(FLAG_ASLIFT);
            }
            else {
              #if defined(DEBUGUNDO)
                printf(">>> undo from addItemToNimBufferC\n");
              #endif // DEBUGUNDO
              undo();
            }
            return;
          }
        }
        break;
      }

      default: {
        keyActionProcessed = false;
      }
    }

    if(done) {
      //Convert nimBuffer to display string

      strcpy(nimBufferDisplay, STD_SPACE_HAIR);

      switch(nimNumberPart) {
        case NP_INT_10:     // +12345
        case NP_INT_16:     // +123AB
        case NP_INT_BASE: { // +123AB#16
          nimBufferToDisplayBuffer(aimBuffer, nimBufferDisplay + 2);
          break;
        }

        case NP_REAL_FLOAT_PART: { // +12345.6789
          nimBufferToDisplayBuffer(aimBuffer, nimBufferDisplay + 2);
          break;
        }

        case NP_REAL_EXPONENT: { // +12345.678e+3
          nimBufferToDisplayBuffer(aimBuffer, nimBufferDisplay + 2);

          exponentToDisplayString(stringToInt32(aimBuffer + exponentSignLocation), nimBufferDisplay + stringByteLength(nimBufferDisplay), NULL, true, STD_SPACE_PUNCTUATION);
          if(aimBuffer[exponentSignLocation + 1] == 0 && aimBuffer[exponentSignLocation] == '-') {
            strcat(nimBufferDisplay, STD_SUP_MINUS);
          }
          else if(aimBuffer[exponentSignLocation + 1] == '0' && aimBuffer[exponentSignLocation] == '+') {
            strcat(nimBufferDisplay, STD_SUP_0);
          }
          break;
        }

        case NP_FRACTION_DENOMINATOR: { // +123 12/7
          nimBufferToDisplayBuffer(aimBuffer, nimBufferDisplay + 2);
          strcat(nimBufferDisplay, STD_SPACE_4_PER_EM);

          for(index=2; aimBuffer[index]!=' '; index++) {
          }
          supNumberToDisplayString(stringToInt32(aimBuffer + index + 1), nimBufferDisplay + stringByteLength(nimBufferDisplay), NULL, true, STD_SPACE_PUNCTUATION);

          strcat(nimBufferDisplay, "/");

          for(; aimBuffer[index]!='/'; index++) {
          }
          if(aimBuffer[++index] != 0) {
            subNumberToDisplayString(stringToInt32(aimBuffer + index), nimBufferDisplay + stringByteLength(nimBufferDisplay), NULL);
          }
          break;
        }

        case NP_COMPLEX_INT_PART:   // +1.2+i15
        case NP_COMPLEX_FLOAT_PART: // +1.2+i15.69
        case NP_COMPLEX_EXPONENT: { // +1.2+i15.69e2
          // Real part
          savedNimNumberPart = nimNumberPart;

          for(index=2; index<imaginaryMantissaSignLocation && aimBuffer[index] != '.'; index++) {
          }
          if(index < imaginaryMantissaSignLocation) { // There is a decimal part in the real part
            nimNumberPart = NP_REAL_FLOAT_PART;
          }

          for(index=2; index<imaginaryMantissaSignLocation && aimBuffer[index] != 'e'; index++) {
          }
          if(index < imaginaryMantissaSignLocation) { // There is an exposant in the real part
            nimNumberPart = NP_REAL_EXPONENT;
          }

          nimBufferToDisplayBuffer(aimBuffer, nimBufferDisplay + 2);

          if(nimNumberPart == NP_REAL_EXPONENT) {
            exponentToDisplayString(stringToInt32(aimBuffer + exponentSignLocation), nimBufferDisplay + stringByteLength(nimBufferDisplay), NULL, true, STD_SPACE_PUNCTUATION);
            if(aimBuffer[exponentSignLocation + 1] == 0 && aimBuffer[exponentSignLocation] == '-') {
              strcat(nimBufferDisplay, STD_SUP_MINUS);
            }
            else if(aimBuffer[exponentSignLocation + 1] == '0' && aimBuffer[exponentSignLocation] == '+') {
              strcat(nimBufferDisplay, STD_SUP_0);
            }
          }

          nimNumberPart = savedNimNumberPart;

          // Complex "separator"
          if(getSystemFlag(FLAG_POLAR)) { // polar mode
            strcat(nimBufferDisplay, STD_SPACE_4_PER_EM STD_MEASURED_ANGLE STD_SPACE_4_PER_EM);
            if(aimBuffer[imaginaryMantissaSignLocation] == '-') {
              strcat(nimBufferDisplay, "-");
            }
          }
          else { // rectangular mode
            if(strncmp(nimBufferDisplay + stringByteLength(nimBufferDisplay) - 2, STD_SPACE_HAIR, 2) != 0) {
              strcat(nimBufferDisplay, STD_SPACE_HAIR);
            }

            if(aimBuffer[imaginaryMantissaSignLocation] == '-') {
              strcat(nimBufferDisplay, "-");
            }
            else {
              strcat(nimBufferDisplay, "+");
            }
            strcat(nimBufferDisplay, COMPLEX_UNIT);
            strcat(nimBufferDisplay, PRODUCT_SIGN);
          }

          // Imaginary part
          if(aimBuffer[imaginaryMantissaSignLocation+2] != 0) {
            savedNimNumberPart = nimNumberPart;

            for(index=imaginaryMantissaSignLocation+1; index<(int16_t)strlen(aimBuffer) && aimBuffer[index] != '.'; index++) {
            }
            if(index < (int16_t)strlen(aimBuffer)) { // There is a decimal part in the real part
              nimNumberPart = NP_REAL_FLOAT_PART;
            }

            for(index=imaginaryMantissaSignLocation+1; index<(int16_t)strlen(aimBuffer) && aimBuffer[index] != 'e'; index++) {
            }
            if(index < (int16_t)strlen(aimBuffer)) { // There is an exposant in the real part
              nimNumberPart = NP_REAL_EXPONENT;
            }

            nimBufferToDisplayBuffer(aimBuffer + imaginaryMantissaSignLocation + 1, nimBufferDisplay + stringByteLength(nimBufferDisplay));

            if(nimNumberPart == NP_REAL_EXPONENT) {
              exponentToDisplayString(stringToInt32(aimBuffer + imaginaryExponentSignLocation), nimBufferDisplay + stringByteLength(nimBufferDisplay), NULL, true, STD_SPACE_PUNCTUATION);
              if(aimBuffer[imaginaryExponentSignLocation + 1] == 0 && aimBuffer[imaginaryExponentSignLocation] == '-') {
                strcat(nimBufferDisplay, STD_SUP_MINUS);
              }
              else if(aimBuffer[imaginaryExponentSignLocation + 1] == '0' && aimBuffer[imaginaryExponentSignLocation] == '+') {
                strcat(nimBufferDisplay, STD_SUP_0);
              }
            }

            nimNumberPart = savedNimNumberPart;
          }
          break;
        }

        default: {
          sprintf(errorMessage, "In function addItemToNimBuffer: %d is an unexpected nimNumberPart value while converting buffer to display!", nimNumberPart);
          displayBugScreen(errorMessage);
        }
      }

      if(!getSystemFlag(FLAG_DECIMP)) {
        for(index=stringByteLength(nimBufferDisplay) - 1; index>0; index--) {
          if(nimBufferDisplay[index] == '.') {
            nimBufferDisplay[index] = ',';
          }
        }
      }
    }

    else {
      if(item != -MNU_INTS && item != -MNU_BITS) {
        screenUpdatingMode &= ~SCRUPD_SKIP_STACK_ONE_TIME;
        closeNim();
      }
      if(calcMode != cmNim) {
        if(item == ITM_CONSTpi || (item >= 0 && indexOfItems[item].func == fnConstant)) {
          setSystemFlag(FLAG_ASLIFT);
        }

        if(lastErrorCode == 0) {
          showFunctionName(item, 1000); // 1000ms = 1s
        }
      }
    }
  }



  static int16_t insertGapIP(char *displayBuffer, int16_t numDigits, int16_t nth) {
    if(groupingGap == 0) {
      return 0; // no gap when none is required!
    }
    if(numDigits <= groupingGap) {
      return 0; // there are less than groupingGap digits
    }
    if(nth + 1 == numDigits) {
      return 0; // no gap after the last digit
    }

    if((numDigits - nth) % groupingGap == 1 || groupingGap == 1) {
      strcpy(displayBuffer, STD_SPACE_PUNCTUATION);
      return 2;
    }

    return 0;
  }

  static int16_t insertGapFP(char *displayBuffer, int16_t numDigits, int16_t nth) {
    if(groupingGap == 0) {
      return 0; // no gap when none is required!
    }
    if(numDigits <= groupingGap) {
      return 0; // there are less than groupingGap digits
    }
    if(nth + 1 == numDigits) {
      return 0; // no gap after the last digit
    }

    if(nth % groupingGap == groupingGap - 1) {
      strcpy(displayBuffer, STD_SPACE_PUNCTUATION);
      return 2;
    }

    return 0;
  }



  void nimBufferToDisplayBuffer(const char *buffer, char *displayBuffer) {
    int16_t numDigits, source, dest;

    if(*buffer == '-') {
      *(displayBuffer++) = '-';
    }
    buffer++;

    for(numDigits=0; buffer[numDigits]!=0 && buffer[numDigits]!='e' && buffer[numDigits]!='.' && buffer[numDigits]!=' ' && buffer[numDigits]!='#' && buffer[numDigits]!='+' && buffer[numDigits]!='-'; numDigits++) {
    }

    for(source=0, dest=0; source<numDigits; source++) {
      displayBuffer[dest++] = buffer[source];
      dest += insertGapIP(displayBuffer + dest, numDigits, source);
    }

    displayBuffer[dest] = 0;

    if(nimNumberPart == NP_REAL_FLOAT_PART || nimNumberPart == NP_REAL_EXPONENT) {
      displayBuffer[dest++] = '.';

      buffer += source + 1;

      for(numDigits=0; buffer[numDigits]!=0 && buffer[numDigits]!='e' && buffer[numDigits]!='+' && buffer[numDigits]!='-'; numDigits++) {
      }

      for(source=0; source<numDigits; source++) {
        displayBuffer[dest++] = buffer[source];
        dest += insertGapFP(displayBuffer + dest, numDigits, source);
      }

      displayBuffer[dest] = 0;
    }

    else if(nimNumberPart == NP_INT_BASE) {
      strcpy(displayBuffer + dest, buffer + numDigits);
    }
  }



  void closeNimWithFraction(real34_t *dest) {
    int16_t i, posSpace, posSlash, lg;
    int32_t integer, numer, denom;
    real34_t temp;

    // Set Fraction mode
    if(!getSystemFlag(FLAG_FRACT)) {
      setSystemFlag(FLAG_FRACT);
    }

    lg = strlen(aimBuffer);

    posSpace = 0;
    for(i=2; i<lg; i++) {
      if(aimBuffer[i] == ' ') {
        posSpace = i;
        break;
      }
    }

    for(i=1; i<posSpace; i++) {
      if(aimBuffer[i]<'0' || aimBuffer[i]>'9') { // This should never happen
        displayCalcErrorMessage(ERROR_BAD_INPUT, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        #if (EXTRA_INFO_ON_CALC_ERROR == 1)
          moreInfoOnError("In function parseNimString:", "there is a non numeric character in the integer part of the fraction!", NULL, NULL);
        #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
        return;
      }
    }

    posSlash = 0;
    for(i=posSpace+2; i<lg; i++) {
      if(aimBuffer[i] == '/') {
        posSlash = i;
        break;
      }
    }

    for(i=posSpace+1; i<posSlash; i++) {
      if(aimBuffer[i]<'0' || aimBuffer[i]>'9') { // This should never happen
       displayCalcErrorMessage(ERROR_BAD_INPUT, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
       #if (EXTRA_INFO_ON_CALC_ERROR == 1)
         moreInfoOnError("In function parseNimString:", "there is a non numeric character in the numerator part of the fraction!", NULL, NULL);
       #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
       return;
      }
    }

    for(i=posSlash+1; i<lg; i++) {
      if(aimBuffer[i]<'0' || aimBuffer[i]>'9') {
        displayCalcErrorMessage(ERROR_BAD_INPUT, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        #if (EXTRA_INFO_ON_CALC_ERROR == 1)
          moreInfoOnError("In function parseNimString:", "there is a non numeric character in the denominator part of the fraction!", NULL, NULL);
        #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
        return;
      }
    }

    aimBuffer[posSpace] = 0;
    aimBuffer[posSlash] = 0;
    integer = stringToInt32(aimBuffer + 1);
    numer   = stringToInt32(aimBuffer + posSpace + 1);
    denom   = stringToInt32(aimBuffer + posSlash + 1);

    if(denom == 0 && !getSystemFlag(FLAG_SPCRES)) {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        moreInfoOnError("In function parseNimString:", "the denominator of the fraction should not be 0!", "Unless D flag (Danger) is set.", NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
      return;
    }

    int32ToReal34(numer, dest);
    int32ToReal34(denom, &temp);
    real34Divide(dest, &temp, dest);
    int32ToReal34(integer, &temp);
    real34Add(dest, &temp, dest);
    if(aimBuffer[0] == '-') {
      real34SetNegativeSign(dest);
    }
  }

  void closeNimWithComplex(real34_t *dest_r, real34_t *dest_i) {
    int16_t imaginarySign;

    if(aimBuffer[imaginaryMantissaSignLocation] == '+') {
      imaginarySign = 1;
    }
    else {
      imaginarySign = -1;
    }
    aimBuffer[imaginaryMantissaSignLocation] = 0;

    stringToReal34(aimBuffer, dest_r);

    stringToReal34(aimBuffer + imaginaryMantissaSignLocation + 2, dest_i);
    if(imaginarySign == -1) {
      real34SetNegativeSign(dest_i);
    }

    if(getSystemFlag(FLAG_POLAR)) { // polar mode
      if(real34CompareEqual(dest_r, const34_0)) {
        real34Zero(dest_i);
      }
      else {
        real_t magnitude, theta;

        real34ToReal(dest_r, &magnitude);
        real34ToReal(dest_i, &theta);
        convertAngleFromTo(&theta, currentAngularMode, amRadian, &ctxtReal39);
        if(realCompareLessThan(&magnitude, const_0)) {
          realSetPositiveSign(&magnitude);
          realAdd(&theta, const_pi, &theta, &ctxtReal39);
          WP34S_Mod(&theta, const1071_2pi, &theta, &ctxtReal39);
        }
        realPolarToRectangular(&magnitude, &theta, &magnitude, &theta, &ctxtReal39); // theta in radian
        realToReal34(&magnitude, dest_r);
        realToReal34(&theta,     dest_i);
      }
    }
    fnSetFlag(FLAG_CPXRES);
  }

  void closeNim(void) {
    setSystemFlag(FLAG_ASLIFT);
    if((nimNumberPart == NP_INT_10 || nimNumberPart == NP_INT_16) && lastIntegerBase != 0) {
      strcat(aimBuffer, "#0");
      int16_t basePos = strlen(aimBuffer) - 1;
      if(lastIntegerBase <= 9) {
        aimBuffer[basePos] += lastIntegerBase;
      }
      else {
        aimBuffer[basePos++] = '1';
        aimBuffer[basePos] = '0';
        aimBuffer[basePos + 1] = 0;
        aimBuffer[basePos] += lastIntegerBase - 10;
      }

      nimNumberPart = NP_INT_BASE;
    }
    else {
      lastIntegerBase = 0;
    }

    int16_t lastChar = strlen(aimBuffer) - 1;

    if(calcMode == cmPem) {
      pemCloseNumberInput();
      return;
    }

    if(nimNumberPart != NP_INT_16) { // We need a # and a base
      if(nimNumberPart != NP_INT_BASE || aimBuffer[lastChar] != '#') { // We need a base
        if((nimNumberPart == NP_COMPLEX_EXPONENT || nimNumberPart == NP_REAL_EXPONENT) && (aimBuffer[lastChar] == '+' || aimBuffer[lastChar] == '-') && aimBuffer[lastChar - 1] == 'e') {
          aimBuffer[--lastChar] = 0;
          lastChar--;
        }
        else if(nimNumberPart == NP_REAL_EXPONENT && aimBuffer[lastChar] == 'e') {
          aimBuffer[lastChar--] = 0;
        }

        if(nimNumberPart == NP_COMPLEX_INT_PART && (aimBuffer[lastChar] == 'i' || aimBuffer[lastChar-1]*256 + aimBuffer[lastChar]*256 == 0xa221)) { // complex i or measured angle
          aimBuffer[++lastChar] = '0';                    //JM CHANGED FROM "1" to '0'. DEFAULTING TO 0+0xi WHEN ABORTING CC ENTRY.
          aimBuffer[lastChar + 1] = 0;
        }

        if((aimBuffer[0] != '-' || aimBuffer[1] != 0) && (aimBuffer[lastChar] != '-')) { // The buffer is not just the minus sign AND The last char of the buffer is not the minus sign
          calcModeEnter(cmNormal);

          if(nimNumberPart == NP_INT_10) {
            longInteger_t lgInt;

            longIntegerInit(lgInt);
            stringToLongInteger(aimBuffer + (aimBuffer[0] == '+' ? 1 : 0), 10, lgInt);
            convertLongIntegerToLongIntegerRegister(lgInt, REGISTER_X);
            longIntegerFree(lgInt);
          }
          else if(nimNumberPart == NP_INT_BASE) {
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Any change in this part must be reported in the function strToShortInteger from file testSuite.c after the line: else if(nimNumberPart == NP_INT_BASE) {
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            longInteger_t minVal, value, maxVal;
            int16_t posHash, i, lg;
            int32_t base;

            lg = strlen(aimBuffer);
            posHash = 0;
            for(i=1; i<lg; i++) {
              if(aimBuffer[i] == '#') {
                posHash = i;
                break;
              }
            }

            for(i=posHash+1; i<lg; i++) {
              if(aimBuffer[i]<'0' || aimBuffer[i]>'9') {
                // This should never happen
                displayCalcErrorMessage(ERROR_INVALID_INTEGER_INPUT, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
                #if (EXTRA_INFO_ON_CALC_ERROR == 1)
                  moreInfoOnError("In function closeNIM:", "there is a non numeric character in the base of the integer!", NULL, NULL);
                #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
                return;
              }
            }

            base = stringToInt32(aimBuffer + posHash + 1);
            if(base < 2 || base > 16) {
              displayCalcErrorMessage(ERROR_INVALID_INTEGER_INPUT, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
              #if (EXTRA_INFO_ON_CALC_ERROR == 1)
               moreInfoOnError("In function closeNIM:", "the base of the integer must be from 2 to 16!", NULL, NULL);
              #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
              return;
            }

            for(i=aimBuffer[0] == '-' ? 1 : 0; i<posHash; i++) {
              if((aimBuffer[i] > '9' ? aimBuffer[i] - 'A' + 10 : aimBuffer[i] - '0') >= base) {
                displayCalcErrorMessage(ERROR_INVALID_INTEGER_INPUT, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
                #if (EXTRA_INFO_ON_CALC_ERROR == 1)
                  sprintf(errorMessage, "digit %c is not allowed in base %d!", aimBuffer[i], base);
                  moreInfoOnError("In function closeNIM:", errorMessage, NULL, NULL);
                #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

                #if defined(DEBUGUNDO)
                  printf(">>> undo from addItemToNimBufferD\n");
                #endif // DEBUGUNDO
                undo();
                return;
              }
            }

            longIntegerInit(value);
            aimBuffer[posHash] = 0;
            stringToLongInteger(aimBuffer + (aimBuffer[0] == '+' ? 1 : 0), base, value);

            // maxVal = 2^shortIntegerWordSize
            longIntegerInit(maxVal);
            if(shortIntegerWordSize >= 1 && shortIntegerWordSize <= 64) {
              longInteger2Pow(shortIntegerWordSize, maxVal);
            }
            else {
              sprintf(errorMessage, "In function closeNIM: %d is an unexpected value for shortIntegerWordSize!", shortIntegerWordSize);
              displayBugScreen(errorMessage);
              longIntegerFree(maxVal);
              longIntegerFree(value);
              return;
            }

            // minVal = -maxVal/2
            longIntegerInit(minVal);
            longIntegerSetZero(minVal); // Mandatory! Else segmentation fault at next instruction
            longIntegerDivideUInt(maxVal, 2, minVal); // minVal = maxVal / 2
            longIntegerSetNegativeSign(minVal); // minVal = -minVal

            if((base != 2) && (base != 4) && (base != 8) && (base != 16) && (shortIntegerMode != SIM_UNSIGN)) {
              longIntegerDivideUInt(maxVal, 2, maxVal); // maxVal /= 2
            }

            longIntegerSubtractUInt(maxVal, 1, maxVal); // maxVal--

            if(shortIntegerMode == SIM_UNSIGN) {
              longIntegerSetZero(minVal); // minVal = 0
            }

            if(shortIntegerMode == SIM_1COMPL || shortIntegerMode == SIM_SIGNMT) {
              longIntegerAddUInt(minVal, 1, minVal); // minVal++
            }

            if(longIntegerCompare(value, minVal) < 0 || longIntegerCompare(value, maxVal) > 0) {
              displayCalcErrorMessage(ERROR_WORD_SIZE_TOO_SMALL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
              #if (EXTRA_INFO_ON_CALC_ERROR == 1)
                char strMin[22], strMax[22];
                longIntegerToAllocatedString(minVal, strMin, sizeof(strMin));
                longIntegerToAllocatedString(maxVal, strMax, sizeof(strMax));
                sprintf(errorMessage, "For word size of %d bit%s and integer mode %s,", shortIntegerWordSize, shortIntegerWordSize>1 ? "s" : "", getShortIntegerModeName(shortIntegerMode));
                sprintf(errorMessage + ERROR_MESSAGE_LENGTH/2, "the entered number must be from %s to %s!", strMin, strMax);
                moreInfoOnError("In function closeNIM:", errorMessage, errorMessage + ERROR_MESSAGE_LENGTH/2, NULL);
              #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
              longIntegerFree(maxVal);
              longIntegerFree(minVal);
              longIntegerFree(value);
              return;
            }

            reallocateRegister(REGISTER_X, dtShortInteger, SHORT_INTEGER_SIZE, base);

            char strValue[22];
            longIntegerToAllocatedString(value, strValue, sizeof(strValue));

            uint64_t val = strtoull(strValue + (longIntegerIsNegative(value) ? 1 : 0), NULL, 10); // when value is negative: discard the minus sign

            if(shortIntegerMode == SIM_UNSIGN) {
            }
            else if(shortIntegerMode == SIM_2COMPL) {
              if(longIntegerIsNegative(value)) {
                val = (~val + 1) & shortIntegerMask;
              }
            }
            else if(shortIntegerMode == SIM_1COMPL) {
              if(longIntegerIsNegative(value)) {
                val = ~val & shortIntegerMask;
              }
            }
            else if(shortIntegerMode == SIM_SIGNMT) {
              if(longIntegerIsNegative(value)) {
                val = (val & shortIntegerMask) | shortIntegerSignBit;
              }
            }
            else {
              sprintf(errorMessage, "In function closeNIM: %d is an unexpected value for shortIntegerMode!", shortIntegerMode);
              displayBugScreen(errorMessage);
              *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) = 0;
            }

            *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) = val;
            lastIntegerBase = base;

            longIntegerFree(maxVal);
            longIntegerFree(minVal);
            longIntegerFree(value);
          }
          else if(nimNumberPart == NP_REAL_FLOAT_PART || nimNumberPart == NP_REAL_EXPONENT) {
            reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE, amNone);
            stringToReal34(aimBuffer, REGISTER_REAL34_DATA(REGISTER_X));
          }
          else if(nimNumberPart == NP_FRACTION_DENOMINATOR) {
            reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE, amNone);
            closeNimWithFraction(REGISTER_REAL34_DATA(REGISTER_X));
          }
          else if(nimNumberPart == NP_COMPLEX_INT_PART || nimNumberPart == NP_COMPLEX_FLOAT_PART || nimNumberPart == NP_COMPLEX_EXPONENT) {
            reallocateRegister(REGISTER_X, dtComplex34, COMPLEX34_SIZE, amNone);
            closeNimWithComplex(REGISTER_REAL34_DATA(REGISTER_X), REGISTER_IMAG34_DATA(REGISTER_X));
          }
          else {
            sprintf(errorMessage, "In function closeNIM: %d is an unexpected nimNumberPart value!", nimNumberPart);
            displayBugScreen(errorMessage);
          }
        }
      }
    }
  }

  void closeAim(void) {
    calcModeEnter(cmNormal);
    popSoftmenu();

    if(aimBuffer[0] == 0) {
      #if defined(DEBUGUNDO)
        printf(">>> undo from closeAim\n");
      #endif // DEBUGUNDO
      undo();
    }
    else {
      int16_t len = stringByteLength(aimBuffer) + 1;

      reallocateRegister(REGISTER_X, dtString, TO_BLOCKS(len), amNone);

      xcopy(REGISTER_STRING_DATA(REGISTER_X), aimBuffer, len);
      aimBuffer[0] = 0;

      setSystemFlag(FLAG_ASLIFT);
    }
  }
#endif // !TESTSUITE_BUILD
