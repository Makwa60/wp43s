// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "error.h"

#include "apps/bugScreen.h"
#include "calcMode.h"
#include "charString.h"
#include "constantPointers.h"
#include "debug.h"
#include "flags.h"
#include "fonts.h"
#include "hal/lcd.h"
#include "mathematics/comparisonReals.h"
#include "realType.h"
#include "registers.h"
#include "registerValueConversions.h"
#include "ui/screen.h"
#include <stdarg.h>
#include <string.h>

#include "wp43.h"

errorCode_t     lastErrorCode;
calcRegister_t  errorMessageRegisterLine;
char           *errorMessage;

TO_QSPI const char *errorMessages[NUMBER_OF_ERROR_CODES] = {
/*  0 */  "No error",
/*  1 */  "An argument exceeds the function domain",
/*  2 */  "Bad time or date input",
/*  3 */  "Undefined op-code",
/*  4 */  "Overflow at +" STD_INFINITY,
/*  5 */  "Overflow at -" STD_INFINITY,
/*  6 */  "No such label found",
/*  7 */  "No such function",
/*  8 */  "Out of range",
/*  9 */  "Illegal digit in integer input for this base",
/* 10 */  "Input is too long",
/* 11 */  "RAM is full",
/* 12 */  "Stack clash",
/* 13 */  "Operation is undefined in this mode",
/* 14 */  "Word size is too small",
/* 15 */  "Too few data points for this statistic",
/* 16 */  "Distribution parameter out of valid range",
/* 17 */  "I/O error",
/* 18 */  "Invalid or corrupted data",
/* 19 */  "Flash memory is write protected",
/* 20 */  "No root found",
/* 21 */  "Matrix mismatch",
/* 22 */  "Singular matrix",
/* 23 */  "Flash memory is full",
/* 24 */  "Invalid input data type for this operation",
/* 25 */  "",
/* 26 */  "Please enter a NEW name",
/* 27 */  "Cannot delete a predefined item",
/* 28 */  "No statistic data present",
/* 29 */  "Item to be coded",
/* 30 */  "Function to be coded for that data type",
/* 31 */  "Input data types do not match",
/* 32 */  "This system flag is write protected",
/* 33 */  "Output would exceed 196 characters",
/* 34 */  "This does not work with an empty string",
/* 35 */  "No backup data found",
/* 36 */  "Undefined source variable",
/* 37 */  "This variable is write protected",
/* 38 */  "No matrix indexed",
/* 39 */  "Not enough memory for such a matrix",
/* 40 */  "No errors for selected model",
/* 41 */  "Large " STD_DELTA " and opposite signs, may be a pole",
/* 42 */  "Solver reached local extremum, no root",
/* 43 */  STD_GREATER_EQUAL "1 initial guess lies out of the domain",
/* 44 */  "The function value look constant",
/* 45 */  "Syntax error in this equation",
/* 46 */  "This equation formula is too complex",
/* 47 */  "This item cannot be assigned here",
/* 48 */  "Invalid name",
/* 49 */  "Too many variables",
/* 50 */  "Non-programmable command, please remove",
/* 51 */  "No global label in this program",
/* 52 */  "Bad input", // This error is not in ReM and cannot occur (theoretically).
};



#if defined(PC_BUILD)
  void moreInfoOnError(const char *m1, const char *m2, const char *m3, const char *m4) {
    uint8_t utf8m1[2000], utf8m2[2000], utf8m3[2000], utf8m4[2000];

    if(m1 == NULL) {
      stringToUtf8("No error message available!", utf8m1);
      printf("\n%s\n", utf8m1);
    }
    else if(m2 == NULL) {
      stringToUtf8(m1, utf8m1);
      printf("\n%s\n\n", utf8m1);
    }
    else if(m3 == NULL) {
      stringToUtf8(m1, utf8m1);
      stringToUtf8(m2, utf8m2);
      printf("\n%s\n%s\n\n", utf8m1, utf8m2);
    }
    else if(m4 == NULL) {
      stringToUtf8(m1, utf8m1);
      stringToUtf8(m2, utf8m2);
      stringToUtf8(m3, utf8m3);
      printf("\n%s\n%s\n%s\n\n", utf8m1, utf8m2, utf8m3);
    }
    else {
      stringToUtf8(m1, utf8m1);
      stringToUtf8(m2, utf8m2);
      stringToUtf8(m3, utf8m3);
      stringToUtf8(m4, utf8m4);
      printf("\n%s\n%s\n%s\n%s\n\n", utf8m1, utf8m2, utf8m3, utf8m4);
    }
  }
#endif // PC_BUILD



#if (EXTRA_INFO_ON_CALC_ERROR == 1)
  void errorMoreInfoForFunc(const char *funcName, const char *format, ...) {
    char errorMsgString[2000];
    uint8_t errorMsgUtf8[2000];
    printf("\nIn function %s:\n", funcName);
    va_list args;
    va_start(args, format);
    vsprintf(errorMsgString, format, args);
    va_end(args);
    stringToUtf8(errorMsgString, errorMsgUtf8);
    printf("%s", errorMsgUtf8);
    printf("\n\n");
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR == 1)



void fnRaiseError(uint16_t errorCode) {
  displayCalcErrorMessage((errorCode_t)errorCode, ERR_REGISTER_LINE, REGISTER_X);
}



void fnErrorMessage(uint16_t unusedButMandatoryParameter) {
  real34_t r, maxErr;
  uInt32ToReal34(NUMBER_OF_ERROR_CODES, &maxErr);

  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
  switch(getRegisterDataType(REGISTER_X)) {
    case dtLongInteger: {
      convertLongIntegerRegisterToReal34(REGISTER_X, &r);
      break;
    }

    case dtReal34: {
      if(getRegisterAngularMode(REGISTER_X) == amNone) {
        real34Copy(REGISTER_REAL34_DATA(REGISTER_X), &r);
        break;
      }
      /* fallthrough */
    }

    default: {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("data type %s cannot be used for this function!", getRegisterDataTypeName(REGISTER_X, false, false));
      return;
    }
  }
  #pragma GCC diagnostic pop

  if(real34CompareLessEqual(const34_1, &r) && real34CompareLessThan(&r, &maxErr)) {
    displayCalcErrorMessage((errorCode_t)real34ToUInt32(&r), ERR_REGISTER_LINE, REGISTER_X);
  }
  else {
    displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("the argument is not less than %u or is negative!", NUMBER_OF_ERROR_CODES);
  }
}



void displayCalcErrorMessage(errorCode_t errorCode, calcRegister_t errMessageRegisterLine, calcRegister_t errRegisterLine) {
  if(errorCode >= NUMBER_OF_ERROR_CODES || errorCode == 0) {
    sprintf(errorMessage, "In function displayCalcErrorMessage: %d is an unexpected value for errorCode!", errorCode);
    bugScreen(errorMessage);
  }

  else if(errMessageRegisterLine > REGISTER_T || errMessageRegisterLine < REGISTER_X) {
    sprintf(errorMessage, "In function displayCalcErrorMessage: %d is an unexpected value for errMessageRegisterLine! Must be from 100 (register X) to 103 (register T)", errMessageRegisterLine);
    bugScreen(errorMessage);
  }

  else if(errRegisterLine > REGISTER_T || errRegisterLine < REGISTER_X) {
    sprintf(errorMessage, "In function displayCalcErrorMessage: %d is an unexpected value for errRegisterLine! Must be from 100 (register X) to 103 (register T)", errRegisterLine);
    bugScreen(errorMessage);
  }

  else {
    lastErrorCode            = errorCode;
    errorMessageRegisterLine = errMessageRegisterLine;
  }
}



#if (EXTRA_INFO_ON_CALC_ERROR != 1)
  void typeError(void) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
  }
#endif // (EXTRA_INFO_ON_CALC_ERROR != 1)
