// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "debug.h"

#include "apps/flagBrowser.h"
#include "apps/fontBrowser.h"
#include "apps/registerBrowser.h"
#include "calcMode.h"
#include "charString.h"
#include "display.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "items.h"
#include "registers.h"
#include "ui/tam.h"
#include <stdbool.h>
#include <string.h>

#include "wp43.h"

#if !defined(NDEBUG)
  char debuggerString[1000];
#endif

char *getDataTypeName(uint16_t dt, bool article, bool padWithBlanks) {
  if(article && padWithBlanks) {
    switch(dt) {
      case dtLongInteger: {
        return "a long integer       ";
      }
      case dtTime: {
        return "a time               ";
      }
      case dtDate: {
        return "a date               ";
      }
      case dtString: {
        return "a string             ";
      }
      case dtReal34Matrix: {
        return "a real34 matrix      ";
      }
      case dtComplex34Matrix: {
        return "a complex34 matrix   ";
      }
      case dtShortInteger: {
        return "a short integer      ";
      }
      case dtReal34: {
        return "a real34             ";
      }
      case dtComplex34: {
        return "a complex34          ";
      }
      case dtConfig: {
        return "a config data        ";
      }
      //case dtLabel: {
      //  return "a label              ";
      //}
      //case dtSystemInteger: {
      //  return "a system integer     ";
      //}
      //case dtFlags: {
      //  return "a flags              ";
      //}
      //case dtPgmStep: {
      //  return "a pgm step           ";
      //}
      //case dtDirectory: {
      //  return "a directory          ";
      //}
      default: {
        return                              "a ???                ";
      }
    }
  }
  else if(article && !padWithBlanks) {
    switch(dt) {
      case dtLongInteger: {
        return "a long integer";
      }
      case dtTime: {
        return "a time";
      }
      case dtDate: {
        return "a date";
      }
      case dtString: {
        return "a string";
      }
      case dtReal34Matrix: {
        return "a real34 matrix";
      }
      case dtComplex34Matrix: {
        return "a complex34 matrix";
      }
      case dtShortInteger: {
        return "a short integer";
      }
      case dtReal34: {
        return "a real34";
      }
      case dtComplex34: {
        return "a complex34";
      }
      case dtConfig: {
        return "a config data";
      }
      //case dtLabel: {          return "a label";
      //}
      //case dtSystemInteger: {  return "a system integer";
      //}
      //case dtFlags: {          return "a flags";
      //}
      //case dtPgmStep: {        return "a pgm step";
      //}
      //case dtDirectory: {      return "a directory";
      //}
      default: {
        return                              "a ???";
      }
    }
  }
  else if(!article && padWithBlanks) {
    switch(dt) {
      case dtLongInteger: {
        return "long integer         ";
      }
      case dtTime: {
        return "time                 ";
      }
      case dtDate: {
        return "date                 ";
      }
      case dtString: {
        return "string               ";
      }
      case dtReal34Matrix: {
        return "real34 matrix        ";
      }
      case dtComplex34Matrix: {
        return "complex34 matrix     ";
      }
      case dtShortInteger: {
        return "short integer        ";
      }
      case dtReal34: {
        return "real34               ";
      }
      case dtComplex34: {
        return "complex34            ";
      }
      case dtConfig: {
        return "config data          ";
      }
      //case dtLabel: {
      //  return "label                ";
      //}
      //case dtSystemInteger: {
      //  return "system integer       ";
      //}
      //case dtFlags: {
      //  return "flags                ";
      //}
      //case dtPgmStep: {
      //  return "pgm step             ";
      //}
      //case dtDirectory: {
      //  return "directory            ";
      //}
      default: {
        return                              "???                  ";
      }
    }
  }
  else if(!article && !padWithBlanks) {
    switch(dt) {
      case dtLongInteger: {
        return "long integer";
      }
      case dtTime: {
        return "time";
      }
      case dtDate: {
        return "date";
      }
      case dtString: {
        return "string";
      }
      case dtReal34Matrix: {
        return "real34 matrix";
      }
      case dtComplex34Matrix: {
        return "complex34 matrix";
      }
      case dtShortInteger: {
        return "short integer";
      }
      case dtReal34: {
        return "real34";
      }
      case dtComplex34: {
        return "complex34";
      }
      case dtConfig: {
        return "config data";
      }
      //case dtLabel: {
      //  return "label";
      //}
      //case dtSystemInteger: {
      //  return "system integer";
      //}
      //case dtFlags: {
      //  return "flags";
      //}
      //case dtPgmStep: {
      //  return "pgm step";
      //}
      //case dtDirectory: {
      //  return "directory";
      //}
      default: {
        return                              "???";
      }
    }
  }
  else {
    return                              "???";
  }
}



char *getRegisterDataTypeName(calcRegister_t regist, bool article, bool padWithBlanks) {
  return getDataTypeName(getRegisterDataType(regist), article, padWithBlanks);
}



char *getRegisterTagName(calcRegister_t regist, bool padWithBlanks) {
  static char base[9];

  switch(getRegisterDataType(regist)) {
    case dtLongInteger: {
      switch(getRegisterTag(regist)) {
        case liZero: {
          return "zero    ";
        }
        case liNegative: {
          return "negative";
        }
        case liPositive: {
          return "positive";
        }
        default: {
          return "???     ";
        }
      }
    }

    case dtReal34: {
      switch(getRegisterTag(regist)) {
        case amRadian: {
          return "radian  ";
        }
        case amMultPi: {
          return "multPi  ";
        }
        case amGrad: {
          return "grad    ";
        }
        case amDegree: {
          return "degree  ";
        }
        case amDMS: {
          return "dms     ";
        }
        case amNone: {
          return "none    ";
        }
        default: {
         return "???     ";
        }
      }
    }

    case dtComplex34:
    case dtString:
    case dtReal34Matrix:
    case dtComplex34Matrix:
    case dtDate:
    case dtTime:
    case dtConfig: {
      switch(getRegisterTag(regist)) {
        case amNone: {
          return "        ";
        }
        default: {
          return "???     ";
        }
      }
    }

    case dtShortInteger: {
      sprintf(base, "base %2" PRIu32 " ", getRegisterTag(regist));
      return base;
    }

    default: {
      return "???     ";
    }
  }
}



// Can be only one bit. ??? if invalid
char *getCurveFitModeName(uint16_t selection) {
  switch(selection & 0x03FF) {
    case CF_LINEAR_FITTING: {
      return "Linear     ";
    }
    case CF_EXPONENTIAL_FITTING: {
      return "Exponential";
    }
    case CF_LOGARITHMIC_FITTING: {
      return "Logarithmic";
    }
    case CF_POWER_FITTING: {
      return "Power      ";
    }
    case CF_ROOT_FITTING: {
      return "Root       ";
    }
    case CF_HYPERBOLIC_FITTING: {
      return "Hyperbolic ";
    }
    case CF_PARABOLIC_FITTING: {
      return "Parabolic  ";
    }
    case CF_CAUCHY_FITTING: {
      return "Cauchy peak";
    }
    case CF_GAUSS_FITTING: {
      return "Gauss peak ";
    }
    case CF_ORTHOGONAL_FITTING: {
      return "Orthogonal ";
    }
    default: {
      return "???        ";
      break;
    }
  }
}



char *eatSpacesEnd(const char * ss) {
  static char tmp_names[20];
  int8_t ix;

  strcpy(tmp_names, ss);
  ix = stringByteLength(ss)-1;
  while(ix > 0) {
    if(ss[ix] == ' ') {
      tmp_names[ix] = 0;
    }
    else {
      break;
    }
    ix--;
  }
  return tmp_names;
}



char *eatSpacesMid(const char * ss) {
  static char tmp_names[20];
  char tt[50];
  int8_t ix = 0, iy = 0;

  strcpy(tt, ss);
  tmp_names[0] = 0;
  while(tt[ix] != 0 && ix < 50) {
    if(tt[ix] != ' ') {
      tmp_names[iy++] = tt[ix];
    }
    ix++;
  }
  tmp_names[iy] = 0;
  tmp_names[iy++] = 0;
  return tmp_names;
}



char *getCurveFitModeNames(uint16_t selection) {
  uint16_t ix;

  errorMessage[0] = 0;
  for(ix = 0; ix < 10; ix++) {
    if(selection & (1 << ix)) {
      strcat(errorMessage, errorMessage[0] == 0 ? "" : " ");
      strcat(errorMessage, eatSpacesEnd(getCurveFitModeName(1 << ix)));
    }
  }
  if(errorMessage[0] == 0) {
    return "???        ";
  }
  return errorMessage;
}



//Can be only one bit. ??? if invalid.
char *getCurveFitModeFormula(uint16_t selection) {
  switch(selection & 0x03FF) {
    case CF_LINEAR_FITTING: {
      return "a" STD_SUB_0 STD_SPACE_3_PER_EM "+" STD_SPACE_3_PER_EM "a" STD_SUB_1 "x";
    }
    case CF_EXPONENTIAL_FITTING: {
      return "a" STD_SUB_0 STD_SPACE_3_PER_EM "e^(a" STD_SUB_1 "x)";
    }
    case CF_LOGARITHMIC_FITTING: {
      return "a" STD_SUB_0 STD_SPACE_3_PER_EM "+" STD_SPACE_3_PER_EM "a" STD_SUB_1 "ln(x)";
    }
    case CF_POWER_FITTING: {
      return "a" STD_SUB_0 STD_SPACE_3_PER_EM "x^a" STD_SUB_1 ;
    }
    case CF_ROOT_FITTING: {
      return "a" STD_SUB_0 STD_SPACE_3_PER_EM "a" STD_SUB_1 "^(1/x)";
    }
    case CF_HYPERBOLIC_FITTING: {
      return "(a" STD_SUB_0 STD_SPACE_3_PER_EM "+" STD_SPACE_3_PER_EM "a" STD_SUB_1 "x)" STD_SUP_MINUS_1;
    }
    case CF_PARABOLIC_FITTING: {
      return "a" STD_SUB_0 STD_SPACE_3_PER_EM "+" STD_SPACE_3_PER_EM "a" STD_SUB_1 "x" STD_SPACE_3_PER_EM "+" STD_SPACE_3_PER_EM "a" STD_SUB_2 "x" STD_SUP_2;
    }
    case CF_CAUCHY_FITTING: {
      return STD_LEFT_SQUARE_BRACKET "a" STD_SUB_0 "(x+a" STD_SUB_1 ")" STD_SUP_2 "+a" STD_SUB_2 STD_RIGHT_SQUARE_BRACKET STD_SUP_MINUS_1;
    }
    case CF_GAUSS_FITTING: {
      return "a" STD_SUB_0 "e^" STD_LEFT_SQUARE_BRACKET  "(x-a" STD_SUB_1 ")" STD_SUP_2 "/a" STD_SUB_2 STD_RIGHT_SQUARE_BRACKET;
    }
    case CF_ORTHOGONAL_FITTING: {
      return "a" STD_SUB_0 STD_SPACE_3_PER_EM "+" STD_SPACE_3_PER_EM "a" STD_SUB_1 "x";
    }
    default: {
      return "???        "; break;
    }
  }
}



char *getAngularModeName(angularMode_t angularMode) {
  switch(angularMode) {
    case amRadian: {
      return "radian";
    }
    case amMultPi: {
      return "multPi";
    }
    case amGrad: {
      return "grad  ";
    }
    case amDegree: {
      return "degree";
    }
    case amDMS: {
      return "d.ms  ";
    }
    case amNone: {
      return "none  ";
    }
    default: {
      return "???   ";
    }
  }
}



char *getShortIntegerModeName(uint16_t im) {
  switch(im) {
    case SIM_1COMPL: {
      return "1compl";
    }
    case SIM_2COMPL: {
      return "2compl";
    }
    case SIM_SIGNMT: {
      return "signmt";
    }
    case SIM_UNSIGN: {
      return "unsign";
    }
    default: {
      return "???   ";
    }
  }
}



/* Never used
void debugNIM(void) {
  switch(nimNumberPart) {
    case NP_EMPTY: {
      printf("nimNumberPart = NP_EMPTY               \n");
    }
    case NP_INT_10: {
      printf("nimNumberPart = NP_INT_10              \n");
    }
    case NP_INT_16: {
      printf("nimNumberPart = NP_INT_16              \n");
    }
    case NP_INT_BASE: {
      printf("nimNumberPart = NP_INT_BASE            \n");
    }
    case NP_REAL_FLOAT_PART: {
      printf("nimNumberPart = NP_REAL_FLOAT_PART     \n");
    }
    case NP_REAL_EXPONENT: {
      printf("nimNumberPart = NP_REAL_EXPONENT       \n");
    }
    case NP_FRACTION_DENOMINATOR: {
      printf("nimNumberPart = NP_FRACTION_DENOMINATOR\n");
    }
    case NP_COMPLEX_INT_PART: {
      printf("nimNumberPart = NP_COMPLEX_INT_PART    \n");
    }
    case NP_COMPLEX_FLOAT_PART: {
      printf("nimNumberPart = NP_COMPLEX_FLOAT_PART  \n");
    }
    case NP_COMPLEX_EXPONENT: {
      printf("nimNumberPart = NP_COMPLEX_EXPONENT    \n");
    }
    default: {
      printf("nimNumberPart = NP_???                 \n");
    }
  }
}
*/



#if (DEBUG_PANEL == 1) || (DEBUG_REGISTER_L == 1)
  char *debuggerReal34(real34_t *real34) {
    formatReal34Debug(debuggerString, real34);
    return debuggerString;
  }



  void formatReal34Debug(char *str, real34_t *real34) {
    uint8_t ch, coef, digit;
    uint8_t bcd[DECQUAD_Pmax];
    int32_t sign, exponent;

    if(real34IsInfinite(real34) || real34IsNaN(real34)) {
      real34ToString(real34, str);
      return;
    }

    if(real34IsZero(real34)) {
      if(real34IsPositive(real34)) {
        strcpy(str, "+0.000000000000000000000000000000000e+0");
      }
      else {
        strcpy(str, "-0.000000000000000000000000000000000e+0");
      }
      return;
    }

    sign = real34GetCoefficient(real34, bcd);
    exponent = real34GetExponent(real34);
    if(sign) {
      str[0] = '-';
    }
    else {
      str[0] = '+';
    }

    coef = 0;
    while(coef<34 && bcd[coef] == 0) {
      coef++;
    }

    str[1] = '0' + bcd[coef++];
    digit = 1;
    str[2] = '.';

    ch = 3;
    while(coef<34) {
      str[ch++] = '0' + bcd[coef++];
      digit++;
      exponent++;
    }

    while(digit<34) {
      str[ch++] = '0';
      digit++;
    }

    sprintf(str+ch, "e%+d", exponent);
  }



  void formatRealDebug(char *str, real_t *real) {
    if(realIsZero(real)) {
      xcopy(str, "0.", 3);
    }
    else {
      realToString(real, str);
    }
  }



  void formatComplex34Debug(char *str, void *addr) {
    formatReal34Debug(str     , addr             );
    formatReal34Debug(str + 64, (real34_t *)((dataBlock_t *)addr + REAL34_SIZE_IN_BLOCKS));

    strcat(str, " ");
    xcopy(strchr(str, '\0'), str + 64, strlen(str + 64) + 1);
    strcat(str, "i");
  }
#endif // DEBUG_PANEL == 1 || DEBUG_REGISTER_L == 1



#if defined(PC_BUILD)
  void dumpScreenToConsole(void) {
    int x, y;

    for(y = 0; y < SCREEN_HEIGHT; y++) {
      for(x = 0; x < SCREEN_WIDTH; x++) {
        if(*(screenData + y*screenStride + x) == ON_PIXEL) {
          printf("#");
        }
        else {
          printf(" ");
        }
      }
      printf("\n");
    }
  }
#endif // PC_BUILD



#if defined(PC_BUILD ) || defined(TESTSUITE_BUILD)
  void testRegisters(const char *text) {
    calcRegister_t i;
    bool situationIsBad;

    situationIsBad = false;
    for(i=0; i<FIRST_LOCAL_REGISTER; i++) {
      if(TO_WP43MEMPTR(getRegisterDataPointer(i)) >= RAM_SIZE) {
        situationIsBad = true;
        break;
      }

      if(getRegisterDataType(i) == dtString || getRegisterDataType(i) == dtLongInteger) {
        if(getRegisterMaxDataLength(i) >= RAM_SIZE) {
          situationIsBad = true;
          break;
        }
      }
    }

    if(situationIsBad) {
      printf("\nsituation is bad %s\n", text);
      for(i=0; i<FIRST_LOCAL_REGISTER; i++) {
        if(TO_WP43MEMPTR(getRegisterDataPointer(i)) >= RAM_SIZE) {
          printf("register %d    ptr=%d\n", i, TO_WP43MEMPTR(getRegisterDataPointer(i)));
        }

        if(getRegisterDataType(i) == dtString || getRegisterDataType(i) == dtLongInteger) {
          if(getRegisterMaxDataLength(i) >= RAM_SIZE) {
            printf("register %d    ptr=%d     dataLen=%d (%d %d)\n", i, TO_WP43MEMPTR(getRegisterDataPointer(i)), TO_BYTES(getRegisterMaxDataLength(i)), TO_BYTES(getRegisterMaxDataLength(i)), TO_BYTES(getRegisterMaxDataLength(i)+1));
          }
        }
      }
      exit(0);
    }
  }



  void memoryDump2(const char *text) {
    int32_t i;
    uint32_t dataType;
    calcRegister_t regist;

    //if(debug) {
    //  debugCounter++;
      printf("\n\n%s\nTotal memory = %d bytes = %d blocks\n", text, TO_BYTES(RAM_SIZE), RAM_SIZE);
      printf("Free blocks (%" PRId32 "):\n", numberOfFreeMemoryRegions);

      for(i=0; i<numberOfFreeMemoryRegions; i++) {
        printf("  %2" PRId32 " starting at %5" PRIu16 ": %5" PRIu16 " blocks = %6" PRIu32 " bytes\n", i, freeMemoryRegions[i].address, freeMemoryRegions[i].sizeInBlocks, TO_BYTES((uint32_t)freeMemoryRegions[i].sizeInBlocks));
      }

      printf("Reg  Num DescrAddr DataType                    DataInfo    DataPtr FullDataLen Content\n");

      regist = REGISTER_X;
      dataType = globalRegister[regist].dataType;
      printf(" X  %4d           %2u=%s %5u=%s %5u   %5d       ", regist, dataType, getDataTypeName(dataType, false, true), globalRegister[regist].tag, getRegisterTagName(regist, true), globalRegister[regist].pointerToRegisterData, TO_BYTES(getRegisterFullSize(regist)));
      printRegisterToConsole(regist, "", "\n");

      regist = REGISTER_Y;
      dataType = globalRegister[regist].dataType;
      printf(" Y  %4d           %2u=%s %5u=%s %5u   %5d       ", regist, dataType, getDataTypeName(dataType, false, true), globalRegister[regist].tag, getRegisterTagName(regist, true), globalRegister[regist].pointerToRegisterData, TO_BYTES(getRegisterFullSize(regist)));
      printRegisterToConsole(regist, "", "\n");

      regist = REGISTER_Z;
      dataType = globalRegister[regist].dataType;
      printf(" Z  %4d           %2u=%s %5u=%s %5u   %5d       ", regist, dataType, getDataTypeName(dataType, false, true), globalRegister[regist].tag, getRegisterTagName(regist, true), globalRegister[regist].pointerToRegisterData, TO_BYTES(getRegisterFullSize(regist)));
      printRegisterToConsole(regist, "", "\n");

      regist = REGISTER_T;
      dataType = globalRegister[regist].dataType;
      printf(" T  %4d           %2u=%s %5u=%s %5u   %5d       ", regist, dataType, getDataTypeName(dataType, false, true), globalRegister[regist].tag, getRegisterTagName(regist, true), globalRegister[regist].pointerToRegisterData, TO_BYTES(getRegisterFullSize(regist)));
      printRegisterToConsole(regist, "", "\n");

      printf("----------------------------------------------------------------------------------------------------------------------------------------\n");

      regist = REGISTER_A;
      dataType = globalRegister[regist].dataType;
      printf(" A  %4d           %2u=%s %5u=%s %5u   %5d       ", regist, dataType, getDataTypeName(dataType, false, true), globalRegister[regist].tag, getRegisterTagName(regist, true), globalRegister[regist].pointerToRegisterData, TO_BYTES(getRegisterFullSize(regist)));
      printRegisterToConsole(regist, "", "\n");

      regist = REGISTER_B;
      dataType = globalRegister[regist].dataType;
      printf(" B  %4d           %2u=%s %5u=%s %5u   %5d       ", regist, dataType, getDataTypeName(dataType, false, true), globalRegister[regist].tag, getRegisterTagName(regist, true), globalRegister[regist].pointerToRegisterData, TO_BYTES(getRegisterFullSize(regist)));
      printRegisterToConsole(regist, "", "\n");

      regist = REGISTER_C;
      dataType = globalRegister[regist].dataType;
      printf(" C  %4d           %2u=%s %5u=%s %5u   %5d       ", regist, dataType, getDataTypeName(dataType, false, true), globalRegister[regist].tag, getRegisterTagName(regist, true), globalRegister[regist].pointerToRegisterData, TO_BYTES(getRegisterFullSize(regist)));
      printRegisterToConsole(regist, "", "\n");

      regist = REGISTER_D;
      dataType = globalRegister[regist].dataType;
      printf(" D  %4d           %2u=%s %5u=%s %5u   %5d       ", regist, dataType, getDataTypeName(dataType, false, true), globalRegister[regist].tag, getRegisterTagName(regist, true), globalRegister[regist].pointerToRegisterData, TO_BYTES(getRegisterFullSize(regist)));
      printRegisterToConsole(regist, "", "\n");

      printf("----------------------------------------------------------------------------------------------------------------------------------------\n");

      regist = REGISTER_L;
      dataType = globalRegister[regist].dataType;
      printf(" L  %4d           %2u=%s %5u=%s %5u   %5d       ", regist, dataType, getDataTypeName(dataType, false, true), globalRegister[regist].tag, getRegisterTagName(regist, true), globalRegister[regist].pointerToRegisterData, TO_BYTES(getRegisterFullSize(regist)));
      printRegisterToConsole(regist, "", "\n");

      regist = REGISTER_I;
      dataType = globalRegister[regist].dataType;
      printf(" I  %4d           %2u=%s %5u=%s %5u   %5d       ", regist, dataType, getDataTypeName(dataType, false, true), globalRegister[regist].tag, getRegisterTagName(regist, true), globalRegister[regist].pointerToRegisterData, TO_BYTES(getRegisterFullSize(regist)));
      printRegisterToConsole(regist, "", "\n");

      regist = REGISTER_J;
      dataType = globalRegister[regist].dataType;
      printf(" J  %4d           %2u=%s %5u=%s %5u   %5d       ", regist, dataType, getDataTypeName(dataType, false, true), globalRegister[regist].tag, getRegisterTagName(regist, true), globalRegister[regist].pointerToRegisterData, TO_BYTES(getRegisterFullSize(regist)));
      printRegisterToConsole(regist, "", "\n");

      regist = REGISTER_K;
      dataType = globalRegister[regist].dataType;
      printf(" K  %4d           %2u=%s %5u=%s %5u   %5d       ", regist, dataType, getDataTypeName(dataType, false, true), globalRegister[regist].tag, getRegisterTagName(regist, true), globalRegister[regist].pointerToRegisterData, TO_BYTES(getRegisterFullSize(regist)));
      printRegisterToConsole(regist, "", "\n");

      printf("----------------------------------------------------------------------------------------------------------------------------------------\n");

      regist = SAVED_REGISTER_X;
      dataType = savedStackRegister[regist - SAVED_REGISTER_X].dataType;
      printf("SX  %4d           %2u=%s %5u=%s %5u   %5d       ", regist, dataType, getDataTypeName(dataType, false, true), savedStackRegister[regist - SAVED_REGISTER_X].tag, getRegisterTagName(regist, true), savedStackRegister[regist - SAVED_REGISTER_X].pointerToRegisterData, TO_BYTES(getRegisterFullSize(regist)));
      printRegisterToConsole(regist, "", "\n");

      regist = SAVED_REGISTER_Y;
      dataType = savedStackRegister[regist - SAVED_REGISTER_X].dataType;
      printf("SY  %4d           %2u=%s %5u=%s %5u   %5d       ", regist, dataType, getDataTypeName(dataType, false, true), savedStackRegister[regist - SAVED_REGISTER_X].tag, getRegisterTagName(regist, true), savedStackRegister[regist - SAVED_REGISTER_X].pointerToRegisterData, TO_BYTES(getRegisterFullSize(regist)));
      printRegisterToConsole(regist, "", "\n");

      regist = SAVED_REGISTER_Z;
      dataType = savedStackRegister[regist - SAVED_REGISTER_X].dataType;
      printf("SZ  %4d           %2u=%s %5u=%s %5u   %5d       ", regist, dataType, getDataTypeName(dataType, false, true), savedStackRegister[regist - SAVED_REGISTER_X].tag, getRegisterTagName(regist, true), savedStackRegister[regist - SAVED_REGISTER_X].pointerToRegisterData, TO_BYTES(getRegisterFullSize(regist)));
      printRegisterToConsole(regist, "", "\n");

      regist = SAVED_REGISTER_T;
      dataType = savedStackRegister[regist - SAVED_REGISTER_X].dataType;
      printf("ST  %4d           %2u=%s %5u=%s %5u   %5d       ", regist, dataType, getDataTypeName(dataType, false, true), savedStackRegister[regist - SAVED_REGISTER_X].tag, getRegisterTagName(regist, true), savedStackRegister[regist - SAVED_REGISTER_X].pointerToRegisterData, TO_BYTES(getRegisterFullSize(regist)));
      printRegisterToConsole(regist, "", "\n");

      printf("----------------------------------------------------------------------------------------------------------------------------------------\n");

      regist = SAVED_REGISTER_A;
      dataType = savedStackRegister[regist - SAVED_REGISTER_X].dataType;
      printf("SA  %4d           %2u=%s %5u=%s %5u   %5d       ", regist, dataType, getDataTypeName(dataType, false, true), savedStackRegister[regist - SAVED_REGISTER_X].tag, getRegisterTagName(regist, true), savedStackRegister[regist - SAVED_REGISTER_X].pointerToRegisterData, TO_BYTES(getRegisterFullSize(regist)));
      printRegisterToConsole(regist, "", "\n");

      regist = SAVED_REGISTER_B;
      dataType = savedStackRegister[regist - SAVED_REGISTER_X].dataType;
      printf("SB  %4d           %2u=%s %5u=%s %5u   %5d       ", regist, dataType, getDataTypeName(dataType, false, true), savedStackRegister[regist - SAVED_REGISTER_X].tag, getRegisterTagName(regist, true), savedStackRegister[regist - SAVED_REGISTER_X].pointerToRegisterData, TO_BYTES(getRegisterFullSize(regist)));
      printRegisterToConsole(regist, "", "\n");

      regist = SAVED_REGISTER_C;
      dataType = savedStackRegister[regist - SAVED_REGISTER_X].dataType;
      printf("SC  %4d           %2u=%s %5u=%s %5u   %5d       ", regist, dataType, getDataTypeName(dataType, false, true), savedStackRegister[regist - SAVED_REGISTER_X].tag, getRegisterTagName(regist, true), savedStackRegister[regist - SAVED_REGISTER_X].pointerToRegisterData, TO_BYTES(getRegisterFullSize(regist)));
      printRegisterToConsole(regist, "", "\n");

      regist = SAVED_REGISTER_D;
      dataType = savedStackRegister[regist - SAVED_REGISTER_X].dataType;
      printf("SD  %4d           %2u=%s %5u=%s %5u   %5d       ", regist, dataType, getDataTypeName(dataType, false, true), savedStackRegister[regist - SAVED_REGISTER_X].tag, getRegisterTagName(regist, true), savedStackRegister[regist - SAVED_REGISTER_X].pointerToRegisterData, TO_BYTES(getRegisterFullSize(regist)));
      printRegisterToConsole(regist, "", "\n");

      regist = SAVED_REGISTER_L;
      dataType = savedStackRegister[regist - SAVED_REGISTER_X].dataType;
      printf("SL  %4d           %2u=%s %5u=%s %5u   %5d       ", regist, dataType, getDataTypeName(dataType, false, true), savedStackRegister[regist - SAVED_REGISTER_X].tag, getRegisterTagName(regist, true), savedStackRegister[regist - SAVED_REGISTER_X].pointerToRegisterData, TO_BYTES(getRegisterFullSize(regist)));
      printRegisterToConsole(regist, "", "\n");
  //  }
  }
#endif // PC_BUILD || TESTSUITE_BUILD
