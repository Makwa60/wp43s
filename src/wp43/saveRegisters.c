// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "saveRegisters.h"

#include "charString.h"
#include "defines.h"
#include "display.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "hal/io.h"
#include "longIntegerType.h"
#include "realType.h"
#include "registers.h"
#include "registerValueConversions.h"
#include "wp43.h"

#define CSV_STR "\""
#define CSV_TAB "\t"
#define CSV_NEWLINE "\n"

// Imported from C43
#if !defined(TESTSUITE_BUILD)
  static void angularUnitToString(angularMode_t angularMode, char *string) {
    switch(angularMode) {
      case amRadian: {
        strcpy(string, "r");
        break;
      }
      case amMultPi: {
        strcpy(string, STD_pi);
        break;
      }
      case amGrad: {
        strcpy(string, "g");
        break;
      }
      case amDegree: {
        strcpy(string, STD_DEGREE);
        break;
      }
      case amDMS: {
        strcpy(string, "d.ms");
        break;
      }
      case amNone: {
        break;
      }
      default: {
        strcpy(string, "?");
      }
    }
  }

  void copyRegisterToClipboardString(calcRegister_t regist, char *clipboardString) {
    longInteger_t lgInt;
    int16_t base, sign, n;
    uint64_t shortInt;
    char string[30000];

    switch(getRegisterDataType(regist)) {
      case dtLongInteger: {
        convertLongIntegerRegisterToLongInteger(regist, lgInt);
        longIntegerToAllocatedString(lgInt, string, 30000);
        longIntegerFree(lgInt);
        break;
      }

      case dtTime: {
        timeToDisplayString(regist, string, false);
        break;
      }

      case dtDate: {
        dateToDisplayString(regist, string);
        break;
      }

      case dtString: {
        xcopy(string, REGISTER_STRING_DATA(regist), stringByteLength(REGISTER_STRING_DATA(regist)) + 1);
        break;
      }

      case dtReal34Matrix: {
        dataBlock_t* dblock = REGISTER_REAL34_MATRIX_DBLOCK(regist);
        real34_t *real34 = REGISTER_REAL34_MATRIX_M_ELEMENTS(regist);
        real34_t reduced;
        int rows, columns, len;

        rows = dblock->matrixRows;
        columns = dblock->matrixColumns;
        sprintf(string, "%dx%d", rows, columns);

        for(int i=0; i<rows*columns; i++) {
          strcat(string, LINEBREAK);
          len = strlen(string);

          real34Reduce(real34++, &reduced);
          real34ToString(&reduced, string + len);

          if(strchr(string + len, '.') == NULL && strchr(string + len, 'E') == NULL) {
            strcat(string + len, ".");
          }
        }
        break;
      }

      case dtComplex34Matrix: {
        dataBlock_t* dblock = REGISTER_COMPLEX34_MATRIX_DBLOCK(regist);
        complex34_t *complex34 = REGISTER_COMPLEX34_MATRIX_M_ELEMENTS(regist);
        real34_t reduced;
        int rows, columns, len;

        rows = dblock->matrixRows;
        columns = dblock->matrixColumns;
        sprintf(string, "%dx%d", rows, columns);

        for(int i=0; i<rows*columns; i++, complex34++) {
          strcat(string, LINEBREAK);
          len = strlen(string);

          // Real part
          real34Reduce((real34_t *)complex34, &reduced);
          real34ToString(&reduced, string + len);
          if(strchr(string + len, '.') == NULL && strchr(string + len, 'E') == NULL) {
            strcat(string + len, ".");
          }
          len = strlen(string);

          // Imaginary part
          real34Reduce(((real34_t *)complex34) + 1, &reduced);
          if(real34IsNegative(&reduced)) {
            sprintf(string + len, " - %sx", COMPLEX_UNIT);
            len += 5;
            real34SetPositiveSign(&reduced);
            real34ToString(&reduced, string + len);
          }
          else {
            sprintf(string + len + strlen(string + len), " + %sx", COMPLEX_UNIT);
            len += 5;
            real34ToString(&reduced, string + len);
          }
          if(strchr(string + len, '.') == NULL && strchr(string + len, 'E') == NULL) {
            strcat(string + len, ".");
          }
        }
        break;
      }

      case dtShortInteger: {
        convertShortIntegerRegisterToUInt64(regist, &sign, &shortInt);
        base = getRegisterShortIntegerBase(regist);

        n = ERROR_MESSAGE_LENGTH - 100;
        sprintf(errorMessage + n--, "#%d (word size = %u)", base, shortIntegerWordSize);

        if(shortInt == 0) {
          errorMessage[n--] = '0';
        }
        else {
          while(shortInt != 0) {
            errorMessage[n--] = digits[shortInt % base];
            shortInt /= base;
          }
          if(sign) {
            errorMessage[n--] = '-';
          }
        }
        n++;

        strcpy(string, errorMessage + n);
        break;
      }

      case dtReal34: {
        real34_t reduced;

        real34Reduce(REGISTER_REAL34_DATA(regist), &reduced);
        real34ToString(&reduced, string);
        if(strchr(string, '.') == NULL && strchr(string, 'E') == NULL) {
          strcat(string, ".");
        }
        angularUnitToString(getRegisterAngularMode(regist), string + strlen(string));
        break;
      }

      case dtComplex34: {
        real34_t reduced;
        int len;
        char tmpStr[100];

        // Real part
        real34Reduce(REGISTER_REAL34_DATA(regist), &reduced);
        real34ToString(&reduced, tmpStr);
        if(strchr(tmpStr, '.') == NULL && strchr(tmpStr, 'E') == NULL) {
          strcat(tmpStr, ".");
        }
        len = strlen(tmpStr);

        // Imaginary part
        real34Reduce(REGISTER_IMAG34_DATA(regist), &reduced);
        if(real34IsNegative(&reduced)) {
          sprintf(string, "%s - %sx", tmpStr, COMPLEX_UNIT);
          len += 5;
          real34SetPositiveSign(&reduced);
          real34ToString(&reduced, string + len);
        }
        else {
          sprintf(string, "%s + %sx", tmpStr, COMPLEX_UNIT);
          len += 5;
          real34ToString(&reduced, string + len);
        }
        if(strchr(string + len, '.') == NULL && strchr(string + len, 'E') == NULL) {
          strcat(string + len, ".");
        }

        break;
      }

      case dtConfig: {
        xcopy(string, "Configuration data", 19);
        break;
      }

      default: {
        sprintf(string, "In function copyRegisterXToClipboard, the data type %" PRIu32 " is unknown! Please try to reproduce and submit a bug.", getRegisterDataType(regist));
      }
    }

    stringToUtf8(string, (uint8_t *)clipboardString);
  }

  static void stackregister_csv_out(int16_t reg_b, int16_t reg_e) {
    int16_t ix = reg_b;
    while (ix <= reg_e) {
      tmpString[0]=0;
      if((ix>=REGISTER_X)&&(ix<=REGISTER_K)) {
        sprintf(tmpString, "%s%c%s%s", CSV_STR, "XYZTABCDLIJK"[ix-REGISTER_X], CSV_STR, CSV_TAB);
      } else
      if((ix>=0)&&(ix<=99)) {
        sprintf(tmpString, "%sR%02d%s%s", CSV_STR, ix, CSV_STR, CSV_TAB);
      } else
      if((ix>=FIRST_LOCAL_REGISTER)&&(ix<=LAST_LOCAL_REGISTER)) {
        sprintf(tmpString, "%sU%02d%s%s", CSV_STR, ix-FIRST_LOCAL_REGISTER, CSV_STR, CSV_TAB);
      }

      copyRegisterToClipboardString((calcRegister_t)ix, tmpString + strlen(tmpString));
      strcat(tmpString + strlen(tmpString), CSV_NEWLINE);

      ioFileWrite(tmpString, strlen(tmpString));

      ++ix;
    }

  }
#endif // !defined(TESTSUITE_BUILD)

// Imported from C43
void fnDumpRegsToFile(uint16_t unusedButMandatoryParameter) {
#ifndef TESTSUITE_BUILD
  if(!ioFileOpen(ioPathRegDump, ioModeWrite)) {
    printf("Cannot save register contents in file!\n");
    exit(0);
  }

  //switch(option) {
  //case 0: {                   //All registers
    stackregister_csv_out(REGISTER_X, REGISTER_D);
    stackregister_csv_out(REGISTER_L, REGISTER_K);
    stackregister_csv_out(0, 99);
    //stackregister_csv_out(FIRST_LOCAL_REGISTER, FIRST_LOCAL_REGISTER + currentNumberOfLocalRegisters);
  //}
  //break;

  //case 1: {                   //Stack only
  //  stackregister_csv_out(REGISTER_X, REGISTER_D);
  //}
  //break;

  //case 2: {                   //Global Registers
  //  stackregister_csv_out(0, 99);
  //}
  //break;

  //case 3: {                   //USER Registers
  //  stackregister_csv_out(FIRST_LOCAL_REGISTER, FIRST_LOCAL_REGISTER + currentNumberOfLocalRegisters);
  //}
  //break;

  //default:
  //  break;
  //}

  ioFileClose();
#endif //TESTSUITE_BUILD
}
