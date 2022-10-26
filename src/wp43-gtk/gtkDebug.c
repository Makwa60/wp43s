// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "gtkDebug.h"

#include "apps/flagBrowser.h"
#include "apps/fontBrowser.h"
#include "apps/registerBrowser.h"
#include "calcMode.h"
#include "charString.h"
#include "core/memory.h"
#include "debug.h"
#include "display.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "hal/debug.h"
#include "items.h"
#include "registers.h"
#include "ui/cursor.h"
#include "ui/tam.h"
#include <stdbool.h>
#include <string.h>

#include "wp43.h"

#if (DEBUG_PANEL == 1)
  int16_t debugWindow;

  void debugInit(void) {
    debugWindow = DBG_REGISTERS;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(chkHexaString), false);
    debugRefresh();
  }



  static void refreshLAndMemoryStatus(void) {
    #if (DEBUG_REGISTER_L == 1 || SHOW_MEMORY_STATUS == 1)
      char tmpStr[1000];
    #endif // (DEBUG_REGISTER_L == 1 || SHOW_MEMORY_STATUS == 1)
    #if (DEBUG_REGISTER_L == 1)
      char string1[1000], string2[1000], *p;
      uint16_t i;

      strcpy(string1, "L = ");

      if(getRegisterDataType(REGISTER_L) == dtReal34) {
        strcat(string1, "real34 = ");
        formatReal34Debug(string2, (real34_t *)getRegisterDataPointer(REGISTER_L));
        strcat(string2, " ");
        strcat(string2, getAngularModeName(getRegisterAngularMode(REGISTER_L)));
      }

      else if(getRegisterDataType(REGISTER_L) == dtComplex34) {
        strcat(string1, "complex34 = ");
        formatComplex34Debug(string2, (void *)getRegisterDataPointer(REGISTER_L));
      }

      else if(getRegisterDataType(REGISTER_L) == dtString) {
        strcat(string1, "string = ");
        for(i=0, p=REGISTER_STRING_DATA(REGISTER_L); i<=stringByteLength(REGISTER_STRING_DATA(REGISTER_L)); i++, p++) {
          string2[i] = *p;
        }
      }

      else if(getRegisterDataType(REGISTER_L) == dtShortInteger) {
        strcat(string1, "short integer = ");
        shortIntegerToDisplayString(REGISTER_L, string2, false);
        strcat(string2, STD_SPACE_3_PER_EM);
        strcat(string2, getShortIntegerModeName(shortIntegerMode));
      }

      else if(getRegisterDataType(REGISTER_L) == dtLongInteger) {
        strcat(string1, "long integer = ");
        longIntegerRegisterToDisplayString(REGISTER_L, string2, sizeof(string2), SCREEN_WIDTH, 50, STD_SPACE_PUNCTUATION);
      }

      else if(getRegisterDataType(REGISTER_L) == dtTime) {
        strcat(string1, "time = ");
        formatReal34Debug(string2, (real34_t *)getRegisterDataPointer(REGISTER_L));
      }

      else if(getRegisterDataType(REGISTER_L) == dtDate) {
        strcat(string1, "date = ");
        formatReal34Debug(string2, (real34_t *)getRegisterDataPointer(REGISTER_L));
      }

      else if(getRegisterDataType(REGISTER_L) == dtReal34Matrix) {
        sprintf(&string1[strlen(string1)], "real34 %" PRIu16 STD_CROSS "%" PRIu16 " matrix = ", REGISTER_REAL34_MATRIX_DBLOCK(REGISTER_L)->matrixRows, REGISTER_REAL34_MATRIX_DBLOCK(REGISTER_L)->matrixColumns);
        formatReal34Debug(string2, REGISTER_REAL34_MATRIX_M_ELEMENTS(REGISTER_L));
      }

      else if(getRegisterDataType(REGISTER_L) == dtComplex34Matrix) {
        sprintf(&string1[strlen(string1)], "complex34 %" PRIu16 STD_CROSS "%" PRIu16 " matrix = ", REGISTER_COMPLEX34_MATRIX_DBLOCK(REGISTER_L)->matrixRows, REGISTER_COMPLEX34_MATRIX_DBLOCK(REGISTER_L)->matrixColumns);
        formatComplex34Debug(string2, REGISTER_COMPLEX34_MATRIX_M_ELEMENTS(REGISTER_L));
      }

      else if(getRegisterDataType(REGISTER_L) == dtConfig) {
        strcat(string1, "Configuration data");
        string2[0] = 0;
      }

      else {
        sprintf(string2, "data type %s not supported for now!", getRegisterDataTypeName(REGISTER_L, false, false));
      }

      stringToUtf8(string1, (uint8_t *)tmpStr);
      stringToUtf8(string2, (uint8_t *)tmpStr + 500);

      gtk_label_set_label(GTK_LABEL(lblRegisterL1), tmpStr);
      gtk_label_set_label(GTK_LABEL(lblRegisterL2), tmpStr + 500);
      gtk_widget_show(lblRegisterL1);
      gtk_widget_show(lblRegisterL2);
    #endif // (DEBUG_REGISTER_L == 1)
    #if (SHOW_MEMORY_STATUS == 1)
      char string[1000];

      sprintf(string, "%" PRId32 " bytes free (%" PRId32 " region%s), 43 %" PRIu64 " bytes, GMP %" PRIu64 " bytes -> should always be 0", getFreeRamMemory(), numberOfFreeMemoryRegions, numberOfFreeMemoryRegions==1 ? "" : "s", TO_BYTES((uint64_t)wp43MemInBlocks), (uint64_t)gmpMemInBytes);
      stringToUtf8(string, (uint8_t *)tmpStr);
      gtk_label_set_label(GTK_LABEL(lblMemoryStatus), tmpStr);
      gtk_widget_show(lblMemoryStatus);
    #endif // (SHOW_MEMORY_STATUS == 1)
  }



  char *getDisplayFormatName(uint16_t df) {
    switch(df) {
      case dfAll: {
        return "ALL";
      }
      case dfFix: {
        return "FIX";
      }
      case dfSci: {
        return "SCI";
      }
      case dfEng: {
        return "ENG";
      }
      default: {
        return "???";
      }
    }
  }



  char *getTimeFormatName(bool tf) {
    return (tf ? "H24" : "H12");
  }



  char *getBooleanName(bool b) {
    return (b ? "true" : "false");
  }



  char *getRbrModeName(uint16_t mode) {
    switch(mode) {
      case RBR_GLOBAL:  {
        return "RBR_GLOBAL";
      }
      case RBR_LOCAL: {
        return "RBR_LOCAL";
      }
      case RBR_NAMED: {
        return "RBR_NAMED";
      }
      default: {
        return "RBR_???";
      }
    }
  }



  char *getRoundingModeName(uint16_t rm) {
    switch(rm) {
      case rmHalfEven: {
        return "half even";
      }
      case rmHalfUp: {
        return "half up  ";
      }
      case rmHalfDown: {
        return "half down";
      }
      case rmUp: {
        return "up       ";
      }
      case rmDown: {
        return "down     ";
      }
      case rmCeil: {
        return "ceil     ";
      }
      case rmFloor: {
        return "floor    ";
      }
      default: {
        return "???      ";
      }
    }
  }



  char *getCalcModeName(uint16_t cm) {
    switch(cm) {
      case cmNormal: {
        return "normal ";
      }
      case cmAim: {
        return "aim    ";
      }
      case cmNim: {
        return "nim    ";
      }
      case cmPem: {
        return "pem    ";
      }
      case cmAssign: {
        return "assign ";
      }
      case cmPlotStat: {
        return "plot.st";
      }
      case cmErrorMessage: {
        return "err.msg";
      }
      case cmConfirmation: {
        return "confirm";
      }
      case cmMim: {
        return "mim    ";
      }
      case cmEim: {
        return "eim    ";
      }
      case cmTimerApp: {
        return "timer  ";
      }
      case cmGraph: {
        return "plot.gr";
      }
      case cmNoUndo: {
        return "noundo ";
      }
      case cmApp: {
        return "app    ";
      }
      default: {
        return "???    ";
      }
    }
  }



  static char *getTamModeName(uint16_t tmm) {
    switch(tmm) {
      case tmValue: {
        return "Value   ";
      }
      case tmValueChb: {
        return "ValueChb"; // change base (->#)
      }
      case tmRegister: {
        return "Register";
      }
      case tmFlagR: {
        return "FlagR   ";
      }
      case tmFlagW: {
        return "FlagW   ";
      }
      case tmStoRcl: {
        return "StoRcl  ";
      }
      case tmMDim: {
        return "MDim    ";
      }
      case tmShuffle: {
        return "Shuffle ";
      }
      case tmLabel: {
        return "Label   ";
      }
      case tmSolve: {
        return "Solve   ";
      }
      case tmNewMenu: {
        return "NewMenu ";
      }
      case tmKey: {
        return "Key     ";
      }
      case tmIntegrate: {
        return "Integrat";
      }
      case tmCmp: {
        return "Cmp     ";
      }
      default: {
        return "???     ";
      }
    }
  }



  char *getNextCharName(uint16_t nc) {
    switch(nc) {
      case NC_NORMAL: {
        return "normal     ";
      }
      case NC_SUBSCRIPT: {
        return "subscript  ";
      }
      case NC_SUPERSCRIPT: {
        return "superscript";
      }
      default: {
        return "???        ";
      }
    }
  }



  char *getSystemFlagName(uint16_t sf) {
    return indexOfItems[SFL_TDM24 + (sf & 0x3fff)].itemCatalogName;
  }



  char *getComplexUnitName(bool cu) {
    return (cu ? "j" : "i");
  }



  char *getProductSignName(bool ps) {
    return (ps ? "cross" : "dot  ");
  }



  char *getFractionTypeName(bool ft) {
    return (ft ? "proper a b/c" : "improper d/c");
  }



  char *getFractionDenom1ModeName(bool ft) {
    return (ft ? "up to D.MAX   " : "D.MAX or fact");
  }



  char *getFractionDenom2ModeName(bool ft) {
    return (ft ? "D.MAX         " : "fact of D.MAX");
  }



  char *getRadixMarkName(bool rm) {
    return (rm ? "period" :"comma ");
  }



  char *getDisplayOvrName(bool dio) {
    return (dio ? "sciOvr" : "engOvr");
  }



  char *getStackSizeName(bool ss) {
    return (ss ? "8 levels" : "4 levels");
  }



  char *getComplexModeName(bool cm) {
    return (cm ? "rectangular" : "polar      ");
  }



  char *getAlphaCaseName(uint16_t ac) {
    if(ac == AC_LOWER) {
      return "lower";
    }
    if(ac == AC_UPPER) {
      return "upper";
    }

    return "???  ";
  }



  char *getAlphaSelectionMenuName(uint16_t alsm) {
    switch(alsm) {
      case CATALOG_NONE: {
        return "CATALOG_NONE";
      }
      case CATALOG_FCNS: {
        return "CATALOG_FCNS";
      }
      case CATALOG_CNST: {
        return "CATALOG_CNST";
      }
      case CATALOG_MENU: {
        return "CATALOG_MENU";
      }
      case CATALOG_SYFL: {
        return "CATALOG_SYFL";
      }
      case CATALOG_AINT: {
        return "CATALOG_AINT";
      }
      case CATALOG_aint: {
        return "CATALOG_aint";
      }
      case CATALOG_PROG: {
        return "CATALOG_PROG";
      }
      case CATALOG_VAR: {
        return "CATALOG_VAR";
      }
      case CATALOG_MATRS: {
        return "CATALOG_MATRS";
      }
      case CATALOG_STRINGS: {
        return "CATALOG_STRINGS";
      }
      case CATALOG_DATES: {
        return "CATALOG_DATES";
      }
      case CATALOG_TIMES: {
        return "CATALOG_TIMES";
      }
      case CATALOG_ANGLES: {
        return "CATALOG_ANGLES";
      }
      case CATALOG_SINTS: {
        return "CATALOG_SINTS";
      }
      case CATALOG_LINTS: {
        return "CATALOG_LINTS";
      }
      case CATALOG_REALS: {
        return "CATALOG_REALS";
      }
      case CATALOG_CPXS: {
        return "CATALOG_CPXS";
      }
      case CATALOG_MVAR: {
        return "CATALOG_MVAR";
      }
      default: {
        return "CATALOG_????";
      }
    }
  }



  // Fills the row th line of the debug window with the value of register r
  static void debugRegisterValue(calcRegister_t regist, int row) {
    char     string[3000], *p, tmpStr[1000];
    uint16_t i, k, n=0;

    if(FIRST_NAMED_VARIABLE <= regist && regist <= FIRST_NAMED_VARIABLE + numberOfNamedVariables) { // Named variable
      n = allNamedVariables[regist - FIRST_NAMED_VARIABLE].variableName[0];
      for(i=0, p=(char *)(allNamedVariables[regist - FIRST_NAMED_VARIABLE].variableName) + 1; i<n; i++, p++) {
        string[i] = *p;
      }
      string[i] = 0;

      strcat(string, STD_SPACE_3_PER_EM "=" STD_SPACE_3_PER_EM);
      n = stringByteLength(string);
    }

    if(getRegisterDataType(regist) == dtReal34) {
      formatReal34Debug(string + n, (real34_t *)getRegisterDataPointer(regist));
      if(getRegisterAngularMode(regist) != amNone) {
        n = stringByteLength(string);
        strcpy(string + n++, " ");
        strcpy(string + n, getAngularModeName(getRegisterAngularMode(regist)));
      }
    }

    else if(getRegisterDataType(regist) == dtComplex34) {
      formatComplex34Debug(string + n, (void *)getRegisterDataPointer(regist));
    }

    else if(getRegisterDataType(regist) == dtString) {
      if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chkHexaString))) {
        k = stringByteLength(REGISTER_STRING_DATA(regist));
        for(i=0, p=REGISTER_STRING_DATA(regist); i<=k; i++, p++) {
          sprintf(string + n + 4*i, STD_SPACE_3_PER_EM "%02x", *p);
        }
      }
      else {
        for(i=0, p=REGISTER_STRING_DATA(regist); i<=stringByteLength(REGISTER_STRING_DATA(regist)); i++, p++) {
          string[n + i] = *p;
        }
      }
    }

    else if(getRegisterDataType(regist) == dtShortInteger) {
      shortIntegerToDisplayString(regist, string + n, false);
      strcat(string + n, STD_SPACE_3_PER_EM);
      strcat(string + n, getShortIntegerModeName(shortIntegerMode));
    }

    else if(getRegisterDataType(regist) == dtLongInteger) {
      longIntegerRegisterToDisplayString(regist, string + n, sizeof(string) - n, SCREEN_WIDTH, 50, STD_SPACE_PUNCTUATION);
    }

    else if(getRegisterDataType(regist) == dtTime) {
      formatReal34Debug(string + n, (real34_t *)getRegisterDataPointer(regist));
    }

    else if(getRegisterDataType(regist) == dtDate) {
      formatReal34Debug(string + n, (real34_t *)getRegisterDataPointer(regist));
    }

    else if(getRegisterDataType(regist) == dtConfig) {
      sprintf(string + n, "Configuration data");
    }

    else if(getRegisterDataType(regist) == dtReal34Matrix) {
      dataBlock_t* dblock = REGISTER_REAL34_MATRIX_DBLOCK(regist);
      sprintf(string + n, "Real Matrix of Size [%" PRIu16" x %" PRIu16" Matrix]", dblock->matrixRows, dblock->matrixColumns);
    }

    else if(getRegisterDataType(regist) == dtComplex34Matrix) {
      dataBlock_t* dblock = REGISTER_COMPLEX34_MATRIX_DBLOCK(regist);
      sprintf(string + n, "Complex Matrix of Size [%" PRIu16" x %" PRIu16" Matrix]", dblock->matrixRows, dblock->matrixColumns);
    }

    else {
      sprintf(string + n, "data type %s not supported for now!", getRegisterDataTypeName(regist, false, false));
    }

    while(stringWidth(string, &standardFont, true, true) > 961) {
      string[stringLastGlyph(string)] = 0;
    }

    stringToUtf8(string, (uint8_t *)tmpStr);

    gtk_label_set_label(GTK_LABEL(lbl2[row]), tmpStr);
    gtk_widget_show(lbl2[row]);
  }



  void btnBitFieldsClicked(GtkWidget* w, gpointer data) {
    debugWindow = DBG_BIT_FIELDS;
    debugRefresh();
  }



  void btnFlagsClicked(GtkWidget* w, gpointer data) {
    debugWindow = DBG_FLAGS;
    //gtk_label_set_label(GTK_LABEL(lbl1[0]), "Flags:");
    //gtk_widget_show(frmCalc);
    debugRefresh();
  }



  void btnRegistersClicked(GtkWidget* w, gpointer data) {
    debugWindow = DBG_REGISTERS;
    //gtk_label_set_label(GTK_LABEL(lbl1[0]), "Regis Addres   Type  Size Content");
    debugRefresh();
  }



  void btnLocalRegistersClicked(GtkWidget* w, gpointer data) {
    debugWindow = DBG_LOCAL_REGISTERS;
    //gtk_label_set_label(GTK_LABEL(lbl1[0]), "Regis Addres   Type  Size Content");
    debugRefresh();
  }



  void btnStatisticalSumsClicked(GtkWidget* w, gpointer data) {
    debugWindow = DBG_STATISTICAL_SUMS;
    //gtk_label_set_label(GTK_LABEL(lbl1[0]), "Regis Addres   Type  Size Content");
    debugRefresh();
  }



  void btnNamedVariablesClicked(GtkWidget* w, gpointer data) {
    debugWindow = DBG_NAMED_VARIABLES;
    //gtk_label_set_label(GTK_LABEL(lbl1[0]), "Regis Addres   Type  Size Content");
    debugRefresh();
  }



  void btnSavedStackRegistersClicked(GtkWidget* w, gpointer data) {
    debugWindow = DBG_TMP_SAVED_STACK_REGISTERS;
    //gtk_label_set_label(GTK_LABEL(lbl1[0]), "Regis Addres   Type  Size Content");
    debugRefresh();
  }



  void chkHexaStringClicked(GtkWidget* w, gpointer data) {
    debugRefresh();
  }



  void debugRefresh(void) {
    char string[200];
    int row;

    refreshLAndMemoryStatus();

    if(debugWindow == DBG_BIT_FIELDS) {
      for(int i=0; i<DEBUG_LINES; i++) {
        gtk_widget_hide(lbl1[i]);
        gtk_widget_hide(lbl2[i]);
      }

      row = 0;

      if(row < DEBUG_LINES) {
        sprintf(string, "TO_WP43MEMPTR(statisticalSumsPointer)    = %6d",         TO_WP43MEMPTR(statisticalSumsPointer));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "numberOfNamedVariables                    = %6u",         numberOfNamedVariables);
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "FLAG_SSIZE8                               = %s",          getBooleanName(getSystemFlag(FLAG_SSIZE8)));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "FLAG_FRACT                                = %s",          getBooleanName(getSystemFlag(FLAG_FRACT)));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "FLAG_PROPFR                               = %s = %s",     getBooleanName(getSystemFlag(FLAG_PROPFR)), getFractionTypeName(getSystemFlag(FLAG_PROPFR)));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "FLAG_DENANY                               = %s = %s",     getBooleanName(getSystemFlag(FLAG_DENANY)), getFractionDenom1ModeName(getSystemFlag(FLAG_DENANY)));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "FLAG_DENFIX                               = %s = %s",     getBooleanName(getSystemFlag(FLAG_DENFIX)), getFractionDenom2ModeName(getSystemFlag(FLAG_DENFIX)));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "denMax                                    = %6u\n",       denMax);
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "shortIntegerMode                          = %6u = %s",    shortIntegerMode,     getShortIntegerModeName(shortIntegerMode));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "shortIntegerWordSize                      = %6u",         shortIntegerWordSize);
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "displatFormat                             = %6u = %s",    displayFormat,        getDisplayFormatName(displayFormat));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "displatFormatDigits                       = %6u",         displayFormatDigits);
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "timeDisplayFormatDigits                   = %6u",         timeDisplayFormatDigits);
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "groupingGap                               = %6u",         groupingGap);
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "significantDigits                         = %6u",         significantDigits);
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "roundingMode                              = %6u = %s",    roundingMode,         getRoundingModeName(roundingMode));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "currentAngularMode                        = %6u = %s",    currentAngularMode,   getAngularModeName(currentAngularMode));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "FLAG_TDM24 (time format)                  = %s = %s",     getBooleanName(getSystemFlag(FLAG_TDM24)), getTimeFormatName(getSystemFlag(FLAG_TDM24)));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "FLAG_YMD (date format)                    = %s",          getBooleanName(getSystemFlag(FLAG_YMD)));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "FLAG_DMY (date format)                    = %s",          getBooleanName(getSystemFlag(FLAG_DMY)));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "FLAG_MDY (date format)                    = %s",          getBooleanName(getSystemFlag(FLAG_MDY)));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "firstGregorianDay                         = %6u",         firstGregorianDay);
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "FLAG_CPXj                                 = %s = %s",     getBooleanName(getSystemFlag(FLAG_CPXj)), getTimeFormatName(getSystemFlag(FLAG_CPXj)));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "FLAG_LEAD.0                               = %s",          getBooleanName(getSystemFlag(FLAG_CPXj)));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "FLAG_DECIM.                               = %s",          getBooleanName(getSystemFlag(FLAG_DECIMP)));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "FLAG_POLAR (complex mode)                 = %s = %s",     getBooleanName(getSystemFlag(FLAG_POLAR)), getTimeFormatName(getSystemFlag(FLAG_POLAR)));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "alphaCase                                 = %6u = %s",    alphaCase,            getAlphaCaseName(alphaCase));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "hourGlassIconEnabled                      = %6d = %s",    hourGlassIconEnabled, getBooleanName(hourGlassIconEnabled));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "watchIconEnabled                          = %6d = %s",    watchIconEnabled,     getBooleanName(watchIconEnabled));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "FLAG_USER                                 = %s",          getBooleanName(getSystemFlag(FLAG_USER)));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "serialIOIconEnabled                       = %6d = %s",    serialIOIconEnabled,  getBooleanName(serialIOIconEnabled));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "printerIconEnabled                        = %6d = %s",    printerIconEnabled,   getBooleanName(printerIconEnabled));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "FLAG_LOWBAT                               = %s",          getBooleanName(getSystemFlag(FLAG_LOWBAT)));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "FLAG_USB                                  = %s",          getBooleanName(getSystemFlag(FLAG_USB)));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "FLAG_ASLIFT                               = %s",          getBooleanName(getSystemFlag(FLAG_ASLIFT)));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "FLAG_ENDPMT                               = %s",          getBooleanName(getSystemFlag(FLAG_ENDPMT)));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "calcMode                                  = %6u = %s",    calcMode,             getCalcModeName(calcMode));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "nextChar                                  = %6u = %s",    nextChar,             getNextCharName(nextChar));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "TAM mode                                  = %6u = %s",    tam.mode,             getTamModeName(tam.mode));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "shiftF                                    = %6d = %s",    shiftF,               getBooleanName(shiftF));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "shiftG                                    = %6d = %s\n",  shiftG,               getBooleanName(shiftG));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "xCursor                                   = %6u",         xCursor);
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "yCursor                                   = %6u",         yCursor);
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "cursorEnabled                             = %6u = %s",    cursorEnabled,        getBooleanName(cursorEnabled));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "currentFntScr                             = %6u",         currentFntScr);
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "currentRegisterBrowserScreen              = %6d",         currentRegisterBrowserScreen);
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "currentFlgScr                             = %6u",         currentFlgScr);
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "rbrMode                                   = %6u = %s",    rbrMode,              getRbrModeName(rbrMode));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      if(row < DEBUG_LINES) {
        sprintf(string, "showContent                               = %6d = %s",    showContent,          getBooleanName(showContent));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }
    }

    else if(debugWindow == DBG_FLAGS) {
      for(int i=0; i<DEBUG_LINES; i++) {
        gtk_widget_hide(lbl1[i]);
        gtk_widget_hide(lbl2[i]);
      }

      row = 0;

      gtk_label_set_label(GTK_LABEL(lbl1[row]), "Flags:");
      gtk_widget_show(lbl1[row++]);

      gtk_label_set_label(GTK_LABEL(lbl1[row]), "      0 1 2 3 4 5 6 7 8 9");
      gtk_widget_show(lbl1[row++]);

      for(int i=0; i<=10; i++) {
        if(i == 10) {
          row++;
          gtk_label_set_label(GTK_LABEL(lbl1[row]), "      X Y Z T A B C D L I");
          gtk_widget_show(lbl1[row++]);
        }

        sprintf(string, "%4d ", 10*i);

        for(int col=0; col<10; col++) {
          strcat(string, getFlag(10*i+col) ? " 1" : " .");
        }

        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }

      gtk_label_set_label(GTK_LABEL(lbl1[row]), "      J K");
      gtk_widget_show(lbl1[row++]);

      sprintf(string, " 110 ");
      strcat(string, getFlag(110) ? " 1" : " .");
      strcat(string, getFlag(111) ? " 1" : " .");
      gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
      gtk_widget_show(lbl1[row++]);

      row++;
      if(currentLocalRegisters != NULL) {
        gtk_label_set_label(GTK_LABEL(lbl1[row]), "Local flags:");
        gtk_widget_show(lbl1[row++]);

        gtk_label_set_label(GTK_LABEL(lbl1[row]), "      0 1 2 3 4 5 6 7 8 9");
        gtk_widget_show(lbl1[row++]);

        sprintf(string, "   0 ");
        for(int col=0; col<10; col++) {
          strcat(string, getFlag(NUMBER_OF_GLOBAL_FLAGS + col) ? " 1" : " .");
        }
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);

        sprintf(string, "  10 ");
        for(int col=10; col<16; col++) {
         strcat(string, getFlag(NUMBER_OF_GLOBAL_FLAGS + col) ? " 1" : " .");
        }
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row++]);
      }
    }

    else if(debugWindow == DBG_REGISTERS) {
      for(int i=0; i<DEBUG_LINES; i++) {
        gtk_widget_hide(lbl1[i]);
        gtk_widget_hide(lbl2[i]);
      }

      row = 0;
      gtk_label_set_label(GTK_LABEL(lbl1[row]), "Regis Type                  Address    Size");
      gtk_label_set_label(GTK_LABEL(lbl2[row]), "Content");
      gtk_widget_show(lbl1[row]);
      gtk_widget_show(lbl2[row++]);

      for(int i=REGISTER_K; i>=REGISTER_I; i--) {
        sprintf(string, "%3d %c %s %7d %7d", i, i-REGISTER_I+'I', getRegisterDataTypeName(i, false, true), TO_WP43MEMPTR(getRegisterDataPointer(i)), TO_BYTES(getRegisterFullSize(i)));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row]);
        debugRegisterValue(i, row++);
      }

      for(int i=REGISTER_D; i>=REGISTER_A; i--) {
        sprintf(string, "%3d %c %s %7d %7d", i, i-REGISTER_A+'A', getRegisterDataTypeName(i, false, true), TO_WP43MEMPTR(getRegisterDataPointer(i)), TO_BYTES(getRegisterFullSize(i)));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row]);
        debugRegisterValue(i, row++);
      }

      sprintf(string, "103 T %s %7d %7d", getRegisterDataTypeName(REGISTER_T, false, true), TO_WP43MEMPTR(getRegisterDataPointer(REGISTER_T)), TO_BYTES(getRegisterFullSize(REGISTER_T)));
      gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
      gtk_widget_show(lbl1[row]);
      debugRegisterValue(REGISTER_T, row++);

      for(int i=REGISTER_Z; i>=REGISTER_X; i--) {
        sprintf(string, "%3d %c %s %7d %7d", i, i-REGISTER_X+'X', getRegisterDataTypeName(i, false, true), TO_WP43MEMPTR(getRegisterDataPointer(i)), TO_BYTES(getRegisterFullSize(i)));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row]);
        debugRegisterValue(i, row++);
      }

      sprintf(string, "108 L %s %7d %7d", getRegisterDataTypeName(REGISTER_L, false, true), TO_WP43MEMPTR(getRegisterDataPointer(REGISTER_L)), TO_BYTES(getRegisterFullSize(REGISTER_L)));
      gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
      gtk_widget_show(lbl1[row]);
      debugRegisterValue(REGISTER_L, row++);

      row++;
      for(int i=0; i<100; i++) {
        if(row < DEBUG_LINES) {
          sprintf(string, "  %02d  %s %7d %7d", i, getRegisterDataTypeName(i, false, true), TO_WP43MEMPTR(getRegisterDataPointer(i)), TO_BYTES(getRegisterFullSize(i)));
          gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
          gtk_widget_show(lbl1[row]);
          debugRegisterValue(i, row);
          row++;
        }
      }
    }

    else if(debugWindow == DBG_LOCAL_REGISTERS) {
      for(int i=0; i<DEBUG_LINES; i++) {
        gtk_widget_hide(lbl1[i]);
        gtk_widget_hide(lbl2[i]);
      }

      row = 0;
      gtk_label_set_label(GTK_LABEL(lbl1[row]), "Regis Type                  Address    Size");
      sprintf(string, "Content of the %" PRIu8 " local registers", currentNumberOfLocalRegisters);
      gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
      gtk_widget_show(lbl1[row]);
      gtk_widget_show(lbl2[row++]);

      for(uint16_t i=FIRST_LOCAL_REGISTER; i<FIRST_LOCAL_REGISTER + currentNumberOfLocalRegisters; i++) {
        if(row < DEBUG_LINES) {
          sprintf(string, ".%02d   %s %7d %7d", i-FIRST_LOCAL_REGISTER, getRegisterDataTypeName(i, false, true), TO_WP43MEMPTR(getRegisterDataPointer(i)), TO_BYTES(getRegisterFullSize(i)));
          gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
          gtk_widget_show(lbl1[row]);
          debugRegisterValue(i, row++);
        }
      }
    }

    else if(debugWindow == DBG_STATISTICAL_SUMS) {
      for(int i=0; i<DEBUG_LINES; i++) {
        gtk_widget_hide(lbl1[i]);
        gtk_widget_hide(lbl2[i]);
      }

      row = 0;
      gtk_label_set_label(GTK_LABEL(lbl1[row]), "Sum");
      if(statisticalSumsPointer == NULL) {
        strcpy(string, "Content of the statistical sums (NULL)");
      }
      else {
        sprintf(string, "Content of the statistical sums (%d)", TO_WP43MEMPTR(statisticalSumsPointer));
      }
      gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
      gtk_widget_show(lbl1[row]);
      gtk_widget_show(lbl2[row++]);

      if(statisticalSumsPointer != NULL) {
        gtk_label_set_label(GTK_LABEL(lbl1[row]), "n");
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_N);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, STD_SIGMA "(x)");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_X);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, STD_SIGMA "(y)");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_Y);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, STD_SIGMA "(x" STD_SUP_2 ")");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_X2);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, STD_SIGMA "(x" STD_SUP_2 "y)");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_X2Y);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, STD_SIGMA "(y" STD_SUP_2 ")");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_Y2);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, STD_SIGMA "(xy)");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_XY);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, STD_SIGMA "(ln(x) ln(y))");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_lnXlnY);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, STD_SIGMA "(ln(x))");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_lnX);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, STD_SIGMA "(ln" STD_SUP_2 "(x))");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_ln2X);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, STD_SIGMA "(y ln(x))");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_YlnX);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, STD_SIGMA "(ln(y))");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_lnY);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, STD_SIGMA "(ln" STD_SUP_2 "(y))");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_ln2Y);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, STD_SIGMA "(x ln(y))");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_XlnY);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, STD_SIGMA "(x" STD_SUP_2 "ln(y))");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_X2lnY);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, STD_SIGMA "(ln(y)/x)");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_lnYonX);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, STD_SIGMA "(x" STD_SUP_2 "/y)");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_X2onY);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, STD_SIGMA "(1/x)");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_1onX);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, STD_SIGMA "(1/x" STD_SUP_2 ")");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_1onX2);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, STD_SIGMA "(x/y)");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_XonY);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, STD_SIGMA "(1/y)");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_1onY);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, STD_SIGMA "(1/y" STD_SUP_2 ")");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_1onY2);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, STD_SIGMA "(x" STD_SUP_3 ")");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_X3);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, STD_SIGMA "(x" STD_SUP_4 ")");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_X4);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, "Xmin");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_XMIN);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, "Xmax");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_XMAX);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, "Ymin");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_YMIN);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, "Ymax");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, SIGMA_YMAX);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        row++;
        sprintf(string, "STATS %s %7d %7d", getRegisterDataTypeName(findNamedVariable(statMx), false, true), TO_WP43MEMPTR(getRegisterDataPointer(findNamedVariable(statMx))), TO_BYTES(getRegisterFullSize(findNamedVariable(statMx))));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row]);
        debugRegisterValue(findNamedVariable(statMx), row++);

        sprintf(string, "SAVED STATS %s %7d %7d", getRegisterDataTypeName(TEMP_REGISTER_2_SAVED_STATS, false, true), TO_WP43MEMPTR(getRegisterDataPointer(TEMP_REGISTER_2_SAVED_STATS)), TO_BYTES(getRegisterFullSize(TEMP_REGISTER_2_SAVED_STATS)));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row]);
        debugRegisterValue(TEMP_REGISTER_2_SAVED_STATS, row++);

        sprintf(string, "SAVED LastX");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, &SAVED_SIGMA_LASTX);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, "SAVED LastY");
        stringToUtf8(string, (uint8_t *)(string + 50));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string + 50);
        gtk_widget_show(lbl1[row]);
        formatRealDebug(string, &SAVED_SIGMA_LASTY);
        gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
        gtk_widget_show(lbl2[row++]);

        sprintf(string, "SAVED Last Action %65i", SAVED_SIGMA_LAct);
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row]);
      }
    }

    else if(debugWindow == DBG_NAMED_VARIABLES) {
      for(int i=0; i<DEBUG_LINES; i++) {
        gtk_widget_hide(lbl1[i]);
        gtk_widget_hide(lbl2[i]);
      }

      row = 0;
      gtk_label_set_label(GTK_LABEL(lbl1[row]), "Regis Type                  Address    Size");
      sprintf(string, "Content of the %" PRIu16 " named variables", numberOfNamedVariables);
      gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
      gtk_widget_show(lbl1[row]);
      gtk_widget_show(lbl2[row++]);

      for(uint16_t i=FIRST_NAMED_VARIABLE; i<FIRST_NAMED_VARIABLE + numberOfNamedVariables; i++) {
        if(row < DEBUG_LINES) {
          sprintf(string, "%03d   %s %7d %7d", i-FIRST_NAMED_VARIABLE, getRegisterDataTypeName(i, false, true), TO_WP43MEMPTR(getRegisterDataPointer(i)), TO_BYTES(getRegisterFullSize(i)));
          gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
          gtk_widget_show(lbl1[row]);
          debugRegisterValue(i, row++);
        }
      }
    }

    else if(debugWindow == DBG_TMP_SAVED_STACK_REGISTERS) {
      for(int i=0; i<DEBUG_LINES; i++) {
        gtk_widget_hide(lbl1[i]);
        gtk_widget_hide(lbl2[i]);
      }

      row = 0;
      gtk_label_set_label(GTK_LABEL(lbl1[row]), "Regis Type                  Address    Size");
      sprintf(string, "Content of the 8 saved stack registers");
      gtk_label_set_label(GTK_LABEL(lbl2[row]), string);
      gtk_widget_show(lbl1[row]);
      gtk_widget_show(lbl2[row++]);

      for(uint16_t i=SAVED_REGISTER_X; i<=SAVED_REGISTER_L; i++) {
        sprintf(string, "%3d   %s %7d %7d", i - SAVED_REGISTER_X, getRegisterDataTypeName(i, false, true), TO_WP43MEMPTR(getRegisterDataPointer(i)), TO_BYTES(getRegisterFullSize(i)));
        gtk_label_set_label(GTK_LABEL(lbl1[row]), string);
        gtk_widget_show(lbl1[row]);
        debugRegisterValue(i, row++);
      }
    }
  }
#endif // (DEBUG_PANEL == 1)
