// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "matrix.h"

#include "charString.h"
#include "constantPointers.h"
#include "conversionAngles.h"
#include "core/memory.h"
#include "debug.h"
#include "display.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "items.h"
#include "longIntegerType.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/cubeRoot.h"
#include "mathematics/division.h"
#include "mathematics/magnitude.h"
#include "mathematics/multiplication.h"
#include "mathematics/slvq.h"
#include "mathematics/toPolar.h"
#include "mathematics/toRect.h"
#include "mathematics/squareRoot.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"
#include "stack.h"
#include "store.h"
#include "ui/matrixEditor.h"
#include "ui/softmenus.h"

#include "wp43.h"

#if !defined(TESTSUITE_BUILD)
  static bool getArg(calcRegister_t regist, real_t *arg) {
    if(getRegisterDataType(regist) == dtLongInteger) {
      convertLongIntegerRegisterToReal(regist, arg, &ctxtReal39);
    }
    else if(getRegisterDataType(regist) == dtReal34) {
      real34ToReal(REGISTER_REAL34_DATA(regist), arg);
      realToIntegralValue(arg, arg, DEC_ROUND_DOWN, &ctxtReal39);
    }
    else {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("cannot accept %s as the argument", getRegisterDataTypeName(regist, true, false));
      return false;
    }
    return true;
  }

  static bool getDimensionArg(uint32_t *rows, uint32_t *cols) {
    longInteger_t tmp_lgInt;

    //Get Size from REGISTER_X and REGISTER_Y
    if(((getRegisterDataType(REGISTER_X) != dtLongInteger) && (getRegisterDataType(REGISTER_X) != dtReal34)) ||
      ((getRegisterDataType(REGISTER_Y) != dtLongInteger) && (getRegisterDataType(REGISTER_Y) != dtReal34))) {
        displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        errorMoreInfo("invalid data type %s and %s", getRegisterDataTypeName(REGISTER_Y, true, false), getRegisterDataTypeName(REGISTER_X, true, false));
        return false;
    }

    if(getRegisterDataType(REGISTER_X) == dtLongInteger) {
      convertLongIntegerRegisterToLongInteger(REGISTER_X, tmp_lgInt);
    }
    else { // dtReal34
      convertReal34ToLongInteger(REGISTER_REAL34_DATA(REGISTER_X), tmp_lgInt, DEC_ROUND_DOWN);
    }
    if(longIntegerIsNegativeOrZero(tmp_lgInt)) {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        char strbuf[32];
        longIntegerToAllocatedString(tmp_lgInt, strbuf, 32);
        errorMoreInfo("invalid number of columns %s", strbuf);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
      return false;
    }
    *cols = longIntegerToUInt(tmp_lgInt);

    if(getRegisterDataType(REGISTER_Y) == dtLongInteger) {
      convertLongIntegerRegisterToLongInteger(REGISTER_Y, tmp_lgInt);
    }
    else { // dtReal34
      convertReal34ToLongInteger(REGISTER_REAL34_DATA(REGISTER_Y), tmp_lgInt, DEC_ROUND_DOWN);
    }
    if(longIntegerIsNegativeOrZero(tmp_lgInt)) {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        char strbuf[32];
        longIntegerToAllocatedString(tmp_lgInt, strbuf, 32);
        errorMoreInfo("invalid number of rows %s", strbuf);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
      return false;
    }
    *rows = longIntegerToUInt(tmp_lgInt);

    longIntegerFree(tmp_lgInt);
    return true;
  }



  static bool swapRowsReal(real34Matrix_t *matrix) {
    real_t ry, rx, rrows;
    uint16_t a, b;

    int32ToReal(matrix->header.matrixRows, &rrows);

    if((!getArg(REGISTER_Y, &ry)) || (!getArg(REGISTER_X, &rx))) {
      return false;
    }

    a = realToInt32(&ry);
    b = realToInt32(&rx);
    if(realIsPositive(&rx) && realIsPositive(&ry) && realCompareLessEqual(&rx, &rrows) && realCompareLessEqual(&ry, &rrows)) {
      if(!realCompareEqual(&ry, &rx)) {
        realMatrixSwapRows(matrix, matrix, a - 1, b - 1);
      }
    }
    else {
      displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("rows %" PRIu16 " and/or %" PRIu16 " out of range", a, b);
      return false;
    }

    return true;
  }

  static bool swapRowsComplex(complex34Matrix_t *matrix) {
    real_t ry, rx, rrows;
    uint16_t a, b;

    int32ToReal(matrix->header.matrixRows, &rrows);

    if((!getArg(REGISTER_Y, &ry)) || (!getArg(REGISTER_X, &rx))) {
      return false;
    }

    a = realToInt32(&ry);
    b = realToInt32(&rx);
    if(realIsPositive(&rx) && realIsPositive(&ry) && realCompareLessEqual(&rx, &rrows) && realCompareLessEqual(&ry, &rrows)) {
      if(!realCompareEqual(&ry, &rx)) {
        complexMatrixSwapRows(matrix, matrix, a - 1, b - 1);
      }
    }
    else {
      displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("rows %" PRIu16 " and/or %" PRIu16 " out of range", a, b);
      return false;
    }

    return true;
  }



  static bool getMatrixReal(real34Matrix_t *matrix) {
    real_t ry, rx, rrows, rcols;
    uint16_t a, b, r, c;

    const int16_t i = getIRegisterAsInt(true);
    const int16_t j = getJRegisterAsInt(true);

    int32ToReal(matrix->header.matrixRows    - i, &rrows);
    int32ToReal(matrix->header.matrixColumns - j, &rcols);

    if((!getArg(REGISTER_Y, &ry)) || (!getArg(REGISTER_X, &rx))) {
      return false;
    }

    a = realToInt32(&ry);
    b = realToInt32(&rx);
    if(realIsPositive(&rx) && realIsPositive(&ry) && realCompareLessEqual(&rx, &rcols) && realCompareLessEqual(&ry, &rrows)) {
      real34Matrix_t mat;
      fnDropY(NOPARAM);
      if(lastErrorCode == ERROR_NONE) {
        if(initMatrixRegister(REGISTER_X, a, b, false)) {
          linkToRealMatrixRegister(REGISTER_X, &mat);
          for(r = 0; r < a; ++r) {
            for(c = 0; c < b; ++c) {
              real34Copy(&matrix->matrixElements[(r + i) * matrix->header.matrixColumns + c + j], &mat.matrixElements[r * b + c]);
            }
          }
        }
        else {
          displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
          return false;
        }
      }
    }
    else {
      displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("%" PRIu16 " " STD_CROSS " %" PRIu16 " out of range", a, b);
      return false;
    }

    return false;
  }

  static bool getMatrixComplex(complex34Matrix_t *matrix) {
    real_t ry, rx, rrows, rcols;
    uint16_t a, b, r, c;

    const int16_t i = getIRegisterAsInt(true);
    const int16_t j = getJRegisterAsInt(true);

    int32ToReal(matrix->header.matrixRows    - i, &rrows);
    int32ToReal(matrix->header.matrixColumns - j, &rcols);

    if((!getArg(REGISTER_Y, &ry)) || (!getArg(REGISTER_X, &rx))) {
      return false;
    }

    a = realToInt32(&ry);
    b = realToInt32(&rx);
    if(realIsPositive(&rx) && realIsPositive(&ry) && realCompareLessEqual(&rx, &rcols) && realCompareLessEqual(&ry, &rrows)) {
      complex34Matrix_t mat;
      fnDropY(NOPARAM);
      if(lastErrorCode == ERROR_NONE) {
        if(initMatrixRegister(REGISTER_X, a, b, true)) {
          linkToComplexMatrixRegister(REGISTER_X, &mat);
          for(r = 0; r < a; ++r) {
            for(c = 0; c < b; ++c) {
              complex34Copy(&matrix->matrixElements[(r + i) * matrix->header.matrixColumns + c + j], &mat.matrixElements[r * b + c]);
            }
          }
        }
        else {
          displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        }
      }
    }
    else {
      displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("%" PRIu16 " " STD_CROSS " %" PRIu16 " out of range", a, b);
      return false;
    }

    return false;
  }



  static bool putMatrixReal(real34Matrix_t *matrix) {
    uint16_t r, c;
    real34Matrix_t mat;

    const int16_t i = getIRegisterAsInt(true);
    const int16_t j = getJRegisterAsInt(true);

    if(getRegisterDataType(REGISTER_X) != dtReal34Matrix) {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("%s is not a real matrix", getRegisterDataTypeName(REGISTER_X, true, false));
      return false;
    }

    linkToRealMatrixRegister(REGISTER_X, &mat);
    if((mat.header.matrixRows + i) <= matrix->header.matrixRows && (mat.header.matrixColumns + j) <= matrix->header.matrixColumns) {
      for(r = 0; r < mat.header.matrixRows; ++r) {
        for(c = 0; c < mat.header.matrixColumns; ++c) {
          real34Copy(&mat.matrixElements[r * mat.header.matrixColumns + c], &matrix->matrixElements[(r + i) * matrix->header.matrixColumns + c + j]);
        }
      }
    }
    else {
      displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("%" PRIu16 " " STD_CROSS " %" PRIu16 " out of range", mat.header.matrixRows, mat.header.matrixColumns);
      return false;
    }

    return true;
  }

  static bool putMatrixComplex(complex34Matrix_t *matrix) {
    uint16_t r, c;
    complex34Matrix_t mat;

    const int16_t i = getIRegisterAsInt(true);
    const int16_t j = getJRegisterAsInt(true);

    if(getRegisterDataType(REGISTER_X) != dtComplex34Matrix) {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("%s is not a complex matrix", getRegisterDataTypeName(REGISTER_X, true, false));
      return false;
    }

    linkToComplexMatrixRegister(REGISTER_X, &mat);
    if((mat.header.matrixRows + i) <= matrix->header.matrixRows && (mat.header.matrixColumns + j) <= matrix->header.matrixColumns) {
      for(r = 0; r < mat.header.matrixRows; ++r) {
        for(c = 0; c < mat.header.matrixColumns; ++c) {
          complex34Copy(&mat.matrixElements[r * mat.header.matrixColumns + c], &matrix->matrixElements[(r + i) * matrix->header.matrixColumns + c + j]);
        }
      }
    }
    else {
      displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("%" PRIu16 " " STD_CROSS " %" PRIu16 " out of range", mat.header.matrixRows, mat.header.matrixColumns);
      return false;
    }

    return true;
  }
#endif // !TESTSUITE_BUILD


void fnNewMatrix(uint16_t unusedParamButMandatory) {
  #if !defined(TESTSUITE_BUILD)
    uint32_t rows, cols;

    if(!getDimensionArg(&rows, &cols)) {
      return;
    }

    if(!saveLastX()) {
      return;
    }

    //Initialize Memory for Matrix
    if(initMatrixRegister(REGISTER_X, rows, cols, false)) {
      setSystemFlag(FLAG_ASLIFT);
    }
    else {
      displayCalcErrorMessage(ERROR_NOT_ENOUGH_MEMORY_FOR_NEW_MATRIX, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("Not enough memory for a %" PRIu32 STD_CROSS "%" PRIu32 " matrix", rows, cols);
      return;
    }

    adjustResult(REGISTER_X, true, false, REGISTER_X, REGISTER_Y, -1);
  #endif // !TESTSUITE_BUILD
}


bool saveStatsMatrix(void) {
  #if !defined(TESTSUITE_BUILD)
    uint32_t rows, cols;
    calcRegister_t regStats = findNamedVariable("STATS");

    #if defined(DEBUGUNDO)
      printf(">>> saveStatsMatrix\n");
      printRegisterToConsole(regStats,"...STATS:\n","\n");
      printRegisterToConsole(TEMP_REGISTER_2_SAVED_STATS,"...pre-save: TEMP_REGISTER_2_SAVED_STATS:\n","\n");
    #endif // DEBUGUNDO

    if(regStats != INVALID_VARIABLE) {
      if(getRegisterDataType(regStats) == dtReal34Matrix) {
        rows = REGISTER_DATA(regStats)->matrixRows;
        cols = REGISTER_DATA(regStats)->matrixColumns;

        //Initialize Memory for Matrix
        if(initMatrixRegister(TEMP_REGISTER_2_SAVED_STATS, rows, cols, false)) {
          copySourceRegisterToDestRegister(regStats, TEMP_REGISTER_2_SAVED_STATS);
          #if defined(DEBUGUNDO)
            printf(">>>saveStatsMatrix:    backing up STATS matrix containing %i rows and %i columns\n",rows, cols);
            regStats = findNamedVariable("STATS");
            printRegisterToConsole(regStats,"...STATS:\n","\n");
            printRegisterToConsole(TEMP_REGISTER_2_SAVED_STATS,"post save: ...TEMP_REGISTER_2_SAVED_STATS:\n","\n");
          #endif // DEBUGUNDO
          return true; //backed up
        }
        else {
          displayCalcErrorMessage(ERROR_NOT_ENOUGH_MEMORY_FOR_NEW_MATRIX, ERR_REGISTER_LINE, REGISTER_X);
          errorMoreInfo("Not enough memory for STATS undo matrix: rows=%i, cols=%i", rows, cols);
          return false;
        }
      }
      else {
        return true; //nothing to backup
      }
    }
    else {
      return true; //nothing to backup
    }
  #else // TESTSUITE_BUILD
    return true;
  #endif // !TESTSUITE_BUILD
}


bool recallStatsMatrix(void) {
  #if !defined(TESTSUITE_BUILD)
    #if defined(DEBUGUNDO)
      printf(">>> recallStatsMatrix ...");
      printRegisterToConsole(TEMP_REGISTER_2_SAVED_STATS,"From: recallStatsMatrix: TEMP_REGISTER_2_SAVED_STATS\n","");
    #endif // DEBUGUNDO
    uint32_t rows, cols;

    calcRegister_t regStats = TEMP_REGISTER_2_SAVED_STATS;

    if(getRegisterDataType(regStats) == dtReal34Matrix) {
      rows = REGISTER_DATA(regStats)->matrixRows;
      cols = REGISTER_DATA(regStats)->matrixColumns;
      if(cols == 2 && rows >= 1) {
        regStats = findNamedVariable("STATS");
        if(regStats == INVALID_VARIABLE) {
          allocateNamedVariable("STATS", dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES));
          regStats = findNamedVariable("STATS");
        }
        #if defined(DEBUGUNDO)
          printf("Clearing STATS\n");
        #endif // DEBUGUNDO
        clearRegister(regStats);

        //Initialize Memory for Matrix
        if(initMatrixRegister(regStats, rows, cols, false)) {
          #if defined(DEBUGUNDO)
            printf(">>>    restoring STATS matrix containing %i rows and %i columns\n",rows, cols);
          #endif // DEBUGUNDO
          copySourceRegisterToDestRegister(TEMP_REGISTER_2_SAVED_STATS, regStats);
          clearRegister(TEMP_REGISTER_2_SAVED_STATS);
          return true; //restored
        }
        else {
          displayCalcErrorMessage(ERROR_NOT_ENOUGH_MEMORY_FOR_NEW_MATRIX, ERR_REGISTER_LINE, REGISTER_X);
          errorMoreInfo("Not enough memory to undo STATS undo matrix: rows=%i, cols=%i", rows, cols);
          return false; //not enough memory
        }
      }
      else {
        return false; //TEMP_REGISTER_2_SAVED_STATS is a non-STATS dimensioned register
      }
    }
    else {
      return false; //TEMP_REGISTER_2_SAVED_STATS is no real 34 matrix
    }
  #else // TESTSUITE_BUILD
    return true; //no error
  #endif // !TESTSUITE_BUILD
}


void fnSetMatrixDimensions(uint16_t regist) {
  #if !defined(TESTSUITE_BUILD)
    uint32_t y, x;

    if(!getDimensionArg(&y, &x)) {
    }
    else if(redimMatrixRegister(regist, y, x)) {
    }
    else {
      displayCalcErrorMessage(ERROR_NOT_ENOUGH_MEMORY_FOR_NEW_MATRIX, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("Not enough memory for a %" PRIu32 STD_CROSS "%" PRIu32 " matrix", y, x);
      return;
    }
  #endif // !TESTSUITE_BUILD
}


void fnGetMatrixDimensions(uint16_t unusedButMandatoryParameter) {
  #if !defined(TESTSUITE_BUILD)
    if(!saveLastX()) {
      return;
    }

    if(getRegisterDataType(REGISTER_X) == dtReal34Matrix || getRegisterDataType(REGISTER_X) == dtComplex34Matrix) {
      const uint16_t rows = REGISTER_DATA(REGISTER_X)->matrixRows;
      const uint16_t cols = REGISTER_DATA(REGISTER_X)->matrixColumns;
      longInteger_t li;

      reallocateRegister(REGISTER_X, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
      liftStack();
      reallocateRegister(REGISTER_Y, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);

      longIntegerInit(li);
      uIntToLongInteger(rows, li);
      convertLongIntegerToLongIntegerRegister(li, REGISTER_Y);
      uIntToLongInteger(cols, li);
      convertLongIntegerToLongIntegerRegister(li, REGISTER_X);
      longIntegerFree(li);
    }
    else {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      errorMoreInfo("DataType %" PRIu32 " is not a matrix", getRegisterDataType(REGISTER_X));
    }

    adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
  #endif // !TESTSUITE_BUILD
}


void fnTranspose(uint16_t unusedButMandatoryParameter) {
  #if !defined(TESTSUITE_BUILD)
    if(!saveLastX()) {
      return;
    }

    if(getRegisterDataType(REGISTER_X) == dtReal34Matrix) {
      real34Matrix_t x;

      linkToRealMatrixRegister(REGISTER_X, &x);
      transposeRealMatrix(&x, &x);
      REGISTER_REAL34_MATRIX_DBLOCK(REGISTER_X)->matrixRows    = x.header.matrixRows;
      REGISTER_REAL34_MATRIX_DBLOCK(REGISTER_X)->matrixColumns = x.header.matrixColumns;
    }
    else if(getRegisterDataType(REGISTER_X) == dtComplex34Matrix) {
      complex34Matrix_t x;

      linkToComplexMatrixRegister(REGISTER_X, &x);
      transposeComplexMatrix(&x, &x);
      REGISTER_REAL34_MATRIX_DBLOCK(REGISTER_X)->matrixRows    = x.header.matrixRows;
      REGISTER_REAL34_MATRIX_DBLOCK(REGISTER_X)->matrixColumns = x.header.matrixColumns;
    }
    else {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      errorMoreInfo("DataType %" PRIu32 " is not a matrix", getRegisterDataType(REGISTER_X));
    }

    adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
  #endif // !TESTSUITE_BUILD
}


void fnLuDecomposition(uint16_t unusedParamButMandatory) {
  #if !defined(TESTSUITE_BUILD)
    if(!saveLastX()) {
      return;
    }

    if(getRegisterDataType(REGISTER_X) == dtReal34Matrix) {
      real34Matrix_t x, l, u;
      uint16_t *p, i, j;

      convertReal34MatrixRegisterToReal34Matrix(REGISTER_X, &x);

      if(x.header.matrixRows != x.header.matrixColumns) {
        displayCalcErrorMessage(ERROR_MATRIX_MISMATCH, ERR_REGISTER_LINE, REGISTER_X);
        errorMoreInfo("not a square matrix (%d" STD_CROSS "%d)", x.header.matrixRows, x.header.matrixColumns);
      }
      else {
        if((p = allocWp43(x.header.matrixRows * 8))) {
          WP34S_LU_decomposition(&x, &l, p);
          if(l.matrixElements) {
            copyRealMatrix(&l, &u);
            if(u.matrixElements) {
              for(i = 0; i < l.header.matrixRows; ++i) {
                for(j = i; j < l.header.matrixColumns; ++j) {
                  real34Copy(i == j ? const34_1 : const34_0, &l.matrixElements[i * l.header.matrixColumns + j]);
                }
              }
              for(i = 1; i < u.header.matrixRows; ++i) {
                for(j = 0; j < i; ++j) {
                  real34Copy(const34_0, &u.matrixElements[i * u.header.matrixColumns + j]);
                }
              }
              realMatrixFree(&x);
              realMatrixIdentity(&x, l.header.matrixColumns);
              for(uint16_t i = 0; i < l.header.matrixColumns; ++i) {
                realMatrixSwapRows(&x, &x, i, p[i]);
              }
              transposeRealMatrix(&x, &x);
              liftStack();
              liftStack();
              convertReal34MatrixToReal34MatrixRegister(&x, REGISTER_Z);
              if(lastErrorCode == ERROR_NONE) {
                convertReal34MatrixToReal34MatrixRegister(&l, REGISTER_Y);
                if(lastErrorCode == ERROR_NONE) {
                  convertReal34MatrixToReal34MatrixRegister(&u, REGISTER_X);
                }
              }
              setSystemFlag(FLAG_ASLIFT);
              realMatrixFree(&u);
            }
            realMatrixFree(&l);
          }
          else {
            displayCalcErrorMessage(ERROR_SINGULAR_MATRIX, ERR_REGISTER_LINE, REGISTER_X);
            errorMoreInfo("attempt to LU-decompose a singular matrix");
          }
          freeWp43(p, x.header.matrixRows * 8);
        }
        else {
          displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        }
      }

      realMatrixFree(&x);
    }
    else if(getRegisterDataType(REGISTER_X) == dtComplex34Matrix) {
      complex34Matrix_t x, l, u;
      real34Matrix_t pivot;
      uint16_t *p, i, j;

      convertComplex34MatrixRegisterToComplex34Matrix(REGISTER_X, &x);

      if(x.header.matrixRows != x.header.matrixColumns) {
        displayCalcErrorMessage(ERROR_MATRIX_MISMATCH, ERR_REGISTER_LINE, REGISTER_X);
        errorMoreInfo("not a square matrix (%d" STD_CROSS "%d)", x.header.matrixRows, x.header.matrixColumns);
      }
      else {
        if((p = allocWp43(x.header.matrixRows * 8))) {
          complex_LU_decomposition(&x, &l, p);
          if(l.matrixElements) {
            copyComplexMatrix(&l, &u);
            if(u.matrixElements) {
              for(i = 0; i < l.header.matrixRows; ++i) {
                for(j = i; j < l.header.matrixColumns; ++j) {
                  real34Copy(i == j ? const34_1 : const34_0, VARIABLE_REAL34_DATA(&l.matrixElements[i * l.header.matrixColumns + j]));
                  real34Copy(                     const34_0, VARIABLE_IMAG34_DATA(&l.matrixElements[i * l.header.matrixColumns + j]));
                }
              }
              for(i = 1; i < u.header.matrixRows; ++i) {
                for(j = 0; j < i; ++j) {
                  real34Copy(const34_0, VARIABLE_REAL34_DATA(&u.matrixElements[i * u.header.matrixColumns + j]));
                  real34Copy(const34_0, VARIABLE_IMAG34_DATA(&u.matrixElements[i * u.header.matrixColumns + j]));
                }
              }
              realMatrixIdentity(&pivot, l.header.matrixColumns);
              if(lastErrorCode == ERROR_NONE) {
                for(uint16_t i = 0; i < l.header.matrixColumns; ++i) {
                  realMatrixSwapRows(&pivot, &pivot, i, p[i]);
                }
                transposeRealMatrix(&pivot, &pivot);
                liftStack();
                liftStack();
                setSystemFlag(FLAG_ASLIFT);
                convertReal34MatrixToReal34MatrixRegister(&pivot, REGISTER_Z);
                if(lastErrorCode == ERROR_NONE) {
                  convertComplex34MatrixToComplex34MatrixRegister(&l, REGISTER_Y);
                  if(lastErrorCode == ERROR_NONE) {
                    convertComplex34MatrixToComplex34MatrixRegister(&u, REGISTER_X);
                  }
                }
                realMatrixFree(&pivot);
              }
              else {
                displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
              }
              complexMatrixFree(&u);
            }
            else {
              displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
            }
            complexMatrixFree(&l);
          }
          else {
            displayCalcErrorMessage(ERROR_SINGULAR_MATRIX, ERR_REGISTER_LINE, REGISTER_X);
            errorMoreInfo("attempt to LU-decompose a singular matrix");
          }
          freeWp43(p, x.header.matrixRows * 8);
        }
        else {
          displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        }
      }

      complexMatrixFree(&x);
    }
    else {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      errorMoreInfo("DataType %" PRIu32 " is not a matrix", getRegisterDataType(REGISTER_X));
    }

    adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
  #endif // !TESTSUITE_BUILD
}


void fnDeterminant(uint16_t unusedParamButMandatory) {
  #if !defined(TESTSUITE_BUILD)
    if(!saveLastX()) {
      return;
    }

    if(getRegisterDataType(REGISTER_X) == dtReal34Matrix) {
      real34Matrix_t x;
      real34_t res;

      linkToRealMatrixRegister(REGISTER_X, &x);

      if(x.header.matrixRows != x.header.matrixColumns) {
        displayCalcErrorMessage(ERROR_MATRIX_MISMATCH, ERR_REGISTER_LINE, REGISTER_X);
        errorMoreInfo("not a square matrix (%d" STD_CROSS "%d)", x.header.matrixRows, x.header.matrixColumns);
      }
      else {
        detRealMatrix(&x, &res);
        if(lastErrorCode == ERROR_NONE) {
          reallocateRegister(REGISTER_X, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
          real34Copy(&res, REGISTER_REAL34_DATA(REGISTER_X));
        }
      }
    }
    else if(getRegisterDataType(REGISTER_X) == dtComplex34Matrix) {
      complex34Matrix_t x;
      real34_t res_r, res_i;

      linkToComplexMatrixRegister(REGISTER_X, &x);

      if(x.header.matrixRows != x.header.matrixColumns) {
        displayCalcErrorMessage(ERROR_MATRIX_MISMATCH, ERR_REGISTER_LINE, REGISTER_X);
        errorMoreInfo("not a square matrix (%d" STD_CROSS "%d)", x.header.matrixRows, x.header.matrixColumns);
      }
      else {
        detComplexMatrix(&x, &res_r, &res_i);
        if(lastErrorCode == ERROR_NONE) {
          reallocateRegister(REGISTER_X, dtComplex34, TO_BLOCKS(COMPLEX34_SIZE_IN_BYTES), amNone);
          real34Copy(&res_r, REGISTER_REAL34_DATA(REGISTER_X));
          real34Copy(&res_i, REGISTER_IMAG34_DATA(REGISTER_X));
        }
      }
    }
    else {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      errorMoreInfo("DataType %" PRIu32 " is not a matrix", getRegisterDataType(REGISTER_X));
    }

    adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
  #endif // !TESTSUITE_BUILD
}


void fnInvertMatrix(uint16_t unusedParamButMandatory) {
  #if !defined(TESTSUITE_BUILD)
    if(!saveLastX()) {
      return;
    }

    if(getRegisterDataType(REGISTER_X) == dtReal34Matrix) {
      real34Matrix_t x, res;

      linkToRealMatrixRegister(REGISTER_X, &x);

      if(x.header.matrixRows != x.header.matrixColumns) {
        displayCalcErrorMessage(ERROR_MATRIX_MISMATCH, ERR_REGISTER_LINE, REGISTER_X);
        errorMoreInfo("not a square matrix (%d" STD_CROSS "%d)", x.header.matrixRows, x.header.matrixColumns);
      }
      else {
        invertRealMatrix(&x, &res);
        if(lastErrorCode == ERROR_NONE) {
          if(res.matrixElements) {
            convertReal34MatrixToReal34MatrixRegister(&res, REGISTER_X);
            realMatrixFree(&res);
            setSystemFlag(FLAG_ASLIFT);
          }
          else {
            displayCalcErrorMessage(ERROR_SINGULAR_MATRIX, ERR_REGISTER_LINE, REGISTER_X);
            errorMoreInfo("attempt to invert a singular matrix");
          }
        }
        else {
          temporaryInformation = TI_NO_INFO;
          if(programRunStop == PGM_WAITING) {
            programRunStop = PGM_STOPPED;
          }
        }
      }
    }
    else if(getRegisterDataType(REGISTER_X) == dtComplex34Matrix) {
      complex34Matrix_t x, res;

      linkToComplexMatrixRegister(REGISTER_X, &x);

      if(x.header.matrixRows != x.header.matrixColumns) {
        displayCalcErrorMessage(ERROR_MATRIX_MISMATCH, ERR_REGISTER_LINE, REGISTER_X);
        errorMoreInfo("not a square matrix (%d" STD_CROSS "%d)", x.header.matrixRows, x.header.matrixColumns);
      }
      else {
        invertComplexMatrix(&x, &res);
        if(lastErrorCode == ERROR_NONE) {
          if(res.matrixElements) {
            convertComplex34MatrixToComplex34MatrixRegister(&res, REGISTER_X);
            complexMatrixFree(&res);
            setSystemFlag(FLAG_ASLIFT);
          }
          else {
            displayCalcErrorMessage(ERROR_SINGULAR_MATRIX, ERR_REGISTER_LINE, REGISTER_X);
            errorMoreInfo("attempt to invert a singular matrix");
          }
        }
        else {
          temporaryInformation = TI_NO_INFO;
          if(programRunStop == PGM_WAITING) {
            programRunStop = PGM_STOPPED;
          }
        }
      }
    }
    else {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      errorMoreInfo("DataType %" PRIu32 " is not a matrix", getRegisterDataType(REGISTER_X));
    }

    adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
  #endif // !TESTSUITE_BUILD
}


void fnEuclideanNorm(uint16_t unusedParamButMandatory) {
  #if !defined(TESTSUITE_BUILD)
    if(!saveLastX()) {
      return;
    }

    if(getRegisterDataType(REGISTER_X) == dtReal34Matrix) {
      real34Matrix_t matrix;
      real34_t sum;

      linkToRealMatrixRegister(REGISTER_X, &matrix);

      euclideanNormRealMatrix(&matrix, &sum);

      // `matrix` invalidates here
      reallocateRegister(REGISTER_X, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
      real34Copy(&sum, REGISTER_REAL34_DATA(REGISTER_X));
    }
    else if(getRegisterDataType(REGISTER_X) == dtComplex34Matrix) {
      complex34Matrix_t matrix;
      real34_t sum;

      linkToComplexMatrixRegister(REGISTER_X, &matrix);

      euclideanNormComplexMatrix(&matrix, &sum);

      // `matrix` invalidates here
      reallocateRegister(REGISTER_X, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
      real34Copy(&sum, REGISTER_REAL34_DATA(REGISTER_X));
    }
    else {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      errorMoreInfo("DataType %" PRIu32 " is not a matrix", getRegisterDataType(REGISTER_X));
    }

    adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
  #endif // !TESTSUITE_BUILD
}


void fnRowSum(uint16_t unusedParamButMandatory) {
  #if !defined(TESTSUITE_BUILD)
    if(!saveLastX()) {
      return;
    }

    if(getRegisterDataType(REGISTER_X) == dtReal34Matrix) {
      real34Matrix_t x, res;
      real_t sum, elem;
      linkToRealMatrixRegister(REGISTER_X, &x);

      if(realMatrixInit(&res, x.header.matrixRows, 1)) {
        for(uint16_t i = 0; i < x.header.matrixRows; ++i) {
          realZero(&sum);
          for(uint16_t j = 0; j < x.header.matrixColumns; ++j) {
            real34ToReal(&x.matrixElements[i * x.header.matrixColumns + j], &elem);
            realAdd(&sum, &elem, &sum, &ctxtReal39);
          }
          realToReal34(&sum, &res.matrixElements[i]);
        }

        convertReal34MatrixToReal34MatrixRegister(&res, REGISTER_X);
        realMatrixFree(&res);
      }
      else {
        displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      }
    }
    else if(getRegisterDataType(REGISTER_X) == dtComplex34Matrix) {
      complex34Matrix_t x, res;
      real_t sumr, sumi, elem;
      linkToComplexMatrixRegister(REGISTER_X, &x);

      if(complexMatrixInit(&res, x.header.matrixRows, 1)) {
        for(uint16_t i = 0; i < x.header.matrixRows; ++i) {
          realZero(&sumr); realZero(&sumi);
          for(uint16_t j = 0; j < x.header.matrixColumns; ++j) {
            real34ToReal(VARIABLE_REAL34_DATA(&x.matrixElements[i * x.header.matrixColumns + j]), &elem);
            realAdd(&sumr, &elem, &sumr, &ctxtReal39);
            real34ToReal(VARIABLE_IMAG34_DATA(&x.matrixElements[i * x.header.matrixColumns + j]), &elem);
            realAdd(&sumi, &elem, &sumi, &ctxtReal39);
          }
          realToReal34(&sumr, VARIABLE_REAL34_DATA(&res.matrixElements[i]));
          realToReal34(&sumi, VARIABLE_IMAG34_DATA(&res.matrixElements[i]));
        }

        convertComplex34MatrixToComplex34MatrixRegister(&res, REGISTER_X);
        complexMatrixFree(&res);
      }
      else {
        displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      }
    }
    else {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      errorMoreInfo("DataType %" PRIu32 " is not a matrix", getRegisterDataType(REGISTER_X));
    }

    adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
  #endif // !TESTSUITE_BUILD
}


void fnRowNorm(uint16_t unusedParamButMandatory) {
  #if !defined(TESTSUITE_BUILD)
    if(!saveLastX()) {
      return;
    }

    if(getRegisterDataType(REGISTER_X) == dtReal34Matrix) {
      real34Matrix_t x;
      real_t norm, sum, elem;
      linkToRealMatrixRegister(REGISTER_X, &x);

      realZero(&norm);
      for(uint16_t i = 0; i < x.header.matrixRows; ++i) {
        realZero(&sum);
        for(uint16_t j = 0; j < x.header.matrixColumns; ++j) {
          real34ToReal(&x.matrixElements[i * x.header.matrixColumns + j], &elem);
          realSetPositiveSign(&elem);
          realAdd(&sum, &elem, &sum, &ctxtReal39);
        }
        if(realCompareGreaterThan(&sum, &norm)) {
          realCopy(&sum, &norm);
        }
      }

      reallocateRegister(REGISTER_X, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
      convertRealToReal34ResultRegister(&norm, REGISTER_X);
    }
    else if(getRegisterDataType(REGISTER_X) == dtComplex34Matrix) {
      complex34Matrix_t x;
      real_t norm, sum, elem, imag;
      linkToComplexMatrixRegister(REGISTER_X, &x);

      realZero(&norm);
      for(uint16_t i = 0; i < x.header.matrixRows; ++i) {
        realZero(&sum);
        for(uint16_t j = 0; j < x.header.matrixColumns; ++j) {
          real34ToReal(VARIABLE_REAL34_DATA(&x.matrixElements[i * x.header.matrixColumns + j]), &elem);
          real34ToReal(VARIABLE_IMAG34_DATA(&x.matrixElements[i * x.header.matrixColumns + j]), &imag);
          complexMagnitude(&elem, &imag, &elem, &ctxtReal39);
          realAdd(&sum, &elem, &sum, &ctxtReal39);
        }
        if(realCompareGreaterThan(&sum, &norm)) {
          realCopy(&sum, &norm);
        }
      }

      reallocateRegister(REGISTER_X, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
      convertRealToReal34ResultRegister(&norm, REGISTER_X);
    }
    else {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      errorMoreInfo("DataType %" PRIu32 " is not a matrix", getRegisterDataType(REGISTER_X));
    }

    adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
  #endif // !TESTSUITE_BUILD
}


void fnVectorAngle(uint16_t unusedParamButMandatory) {
  #if !defined(TESTSUITE_BUILD)
    if(!saveLastX()) {
      return;
    }

    if(getRegisterDataType(REGISTER_X) == dtReal34Matrix && getRegisterDataType(REGISTER_Y) == dtReal34Matrix) {
      real34Matrix_t x, y;
      real34_t res;
      linkToRealMatrixRegister(REGISTER_X, &x);
      linkToRealMatrixRegister(REGISTER_Y, &y);

      if((realVectorSize(&y) < 2) || (realVectorSize(&x) < 2) || (realVectorSize(&y) > 3) || (realVectorSize(&x) > 3) || (realVectorSize(&y) != realVectorSize(&x))) {
        displayCalcErrorMessage(ERROR_MATRIX_MISMATCH, ERR_REGISTER_LINE, REGISTER_X);
        errorMoreInfo("invalid numbers of elements of %d" STD_CROSS "%d-matrix to %d" STD_CROSS "%d-matrix",
            x.header.matrixRows, x.header.matrixColumns,
            y.header.matrixRows, y.header.matrixColumns);
      }
      else {
        vectorAngle(&y, &x, &res);
        convertAngle34FromTo(&res, amRadian, currentAngularMode);
        reallocateRegister(REGISTER_X, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), currentAngularMode);
        real34Copy(&res, REGISTER_REAL34_DATA(REGISTER_X));
      }
    }
    else {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      errorMoreInfo("DataType %" PRIu32 " is not a real matrix", getRegisterDataType(REGISTER_X));
    }

    adjustResult(REGISTER_X, true, true, REGISTER_X, REGISTER_Y, -1);
  #endif // !TESTSUITE_BUILD
}


void fnIndexMatrix(uint16_t regist) {
  #if !defined(TESTSUITE_BUILD)
    if((getRegisterDataType(regist) == dtReal34Matrix) || (getRegisterDataType(regist) == dtComplex34Matrix)) {
      matrixIndex = regist;
      setIRegisterAsInt(false, 1);
      setJRegisterAsInt(false, 1);
    }
    else {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      errorMoreInfo("DataType %" PRIu32 " is not a matrix", getRegisterDataType(regist));
    }
#endif // !TESTSUITE_BUILD
}


void fnGetMatrix(uint16_t unusedParamButMandatory) {
  #if !defined(TESTSUITE_BUILD)
    callByIndexedMatrix(getMatrixReal, getMatrixComplex);
  #endif // !TESTSUITE_BUILD
}


void fnPutMatrix(uint16_t unusedParamButMandatory) {
  #if !defined(TESTSUITE_BUILD)
    callByIndexedMatrix(putMatrixReal, putMatrixComplex);
  #endif // !TESTSUITE_BUILD
}


void fnSwapRows(uint16_t unusedParamButMandatory) {
  #if !defined(TESTSUITE_BUILD)
    callByIndexedMatrix(swapRowsReal, swapRowsComplex);
  #endif // !TESTSUITE_BUILD
}


void fnSimultaneousLinearEquation(uint16_t numberOfUnknowns) {
  #if !defined(TESTSUITE_BUILD)
    if(allocateNamedMatrix("Mat_A", numberOfUnknowns, numberOfUnknowns) != INVALID_VARIABLE) {
      if(allocateNamedMatrix("Mat_B", numberOfUnknowns, 1) != INVALID_VARIABLE) {
        if(allocateNamedMatrix("Mat_X", numberOfUnknowns, 1) != INVALID_VARIABLE) {
          showSoftmenu(-MNU_SIMQ);
          showSoftmenu(-MNU_TAM);
          numberOfTamMenusToPop = 1;
        }
      }
    }
  #endif // !TESTSUITE_BUILD
}


void fnEditLinearEquationMatrixA(uint16_t unusedParamButMandatory) {
  #if !defined(TESTSUITE_BUILD)
    fnEditMatrix(findNamedVariable("Mat_A"));
  #endif // !TESTSUITE_BUILD
}


void fnEditLinearEquationMatrixB(uint16_t unusedParamButMandatory) {
  #if !defined(TESTSUITE_BUILD)
    fnEditMatrix(findNamedVariable("Mat_B"));
  #endif // !TESTSUITE_BUILD
}


void fnEditLinearEquationMatrixX(uint16_t unusedParamButMandatory) {
  #if !defined(TESTSUITE_BUILD)
    if(findNamedVariable("Mat_A") == INVALID_VARIABLE || findNamedVariable("Mat_B") == INVALID_VARIABLE || findNamedVariable("Mat_X") == INVALID_VARIABLE) {
      displayCalcErrorMessage(ERROR_UNDEF_SOURCE_VAR, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("At least one of Mat_A, Mat_B or Mat_X is missing");
    }
    else if(getRegisterDataType(findNamedVariable("Mat_A")) == dtReal34Matrix && getRegisterDataType(findNamedVariable("Mat_B")) == dtReal34Matrix) {
      real34Matrix_t a, b, x;
      linkToRealMatrixRegister(findNamedVariable("Mat_A"), &a);
      linkToRealMatrixRegister(findNamedVariable("Mat_B"), &b);
      real_matrix_linear_eqn(&a, &b, &x);
      if(x.matrixElements) {
        convertReal34MatrixToReal34MatrixRegister(&x, findNamedVariable("Mat_X"));
        realMatrixFree(&x);
      }
    }
    else if(getRegisterDataType(findNamedVariable("Mat_A")) == dtReal34Matrix && getRegisterDataType(findNamedVariable("Mat_B")) == dtComplex34Matrix) {
      complex34Matrix_t a, b, x;
      convertReal34MatrixRegisterToComplex34Matrix(findNamedVariable("Mat_A"), &a);
      linkToComplexMatrixRegister(findNamedVariable("Mat_B"), &b);
      complex_matrix_linear_eqn(&a, &b, &x);
      if(x.matrixElements) {
        convertComplex34MatrixToComplex34MatrixRegister(&x, findNamedVariable("Mat_X"));
        complexMatrixFree(&x);
      }
      complexMatrixFree(&a);
    }
    else if(getRegisterDataType(findNamedVariable("Mat_A")) == dtComplex34Matrix && getRegisterDataType(findNamedVariable("Mat_B")) == dtReal34Matrix) {
      complex34Matrix_t a, b, x;
      linkToComplexMatrixRegister(findNamedVariable("Mat_A"), &a);
      convertReal34MatrixRegisterToComplex34Matrix(findNamedVariable("Mat_B"), &b);
      complex_matrix_linear_eqn(&a, &b, &x);
      if(x.matrixElements) {
        convertComplex34MatrixToComplex34MatrixRegister(&x, findNamedVariable("Mat_X"));
        complexMatrixFree(&x);
      }
      complexMatrixFree(&b);
    }
    else if(getRegisterDataType(findNamedVariable("Mat_A")) == dtComplex34Matrix && getRegisterDataType(findNamedVariable("Mat_B")) == dtComplex34Matrix) {
      complex34Matrix_t a, b, x;
      linkToComplexMatrixRegister(findNamedVariable("Mat_A"), &a);
      linkToComplexMatrixRegister(findNamedVariable("Mat_B"), &b);
      complex_matrix_linear_eqn(&a, &b, &x);
      if(x.matrixElements) {
        convertComplex34MatrixToComplex34MatrixRegister(&x, findNamedVariable("Mat_X"));
        complexMatrixFree(&x);
      }
    }
    if(lastErrorCode == ERROR_NONE) {
      liftStack();
      copySourceRegisterToDestRegister(findNamedVariable("Mat_X"), REGISTER_X);
      popSoftmenu();
    }
  #endif // !TESTSUITE_BUILD
}


void fnQrDecomposition(uint16_t unusedParamButMandatory) {
  #if !defined(TESTSUITE_BUILD)
    if(!saveLastX()) {
      return;
    }

    if(getRegisterDataType(REGISTER_X) == dtReal34Matrix) {
      real34Matrix_t x, q, r;

      linkToRealMatrixRegister(REGISTER_X, &x);

      if(x.header.matrixRows != x.header.matrixColumns) {
        displayCalcErrorMessage(ERROR_MATRIX_MISMATCH, ERR_REGISTER_LINE, REGISTER_X);
        errorMoreInfo("not a square matrix (%d" STD_CROSS "%d)", x.header.matrixRows, x.header.matrixColumns);
      }
      else {
        real_QR_decomposition(&x, &q, &r);
        setSystemFlag(FLAG_ASLIFT);
        liftStack();
        convertReal34MatrixToReal34MatrixRegister(&q, REGISTER_Y);
        convertReal34MatrixToReal34MatrixRegister(&r, REGISTER_X);
        realMatrixFree(&q);
        realMatrixFree(&r);
      }
    }
    else if(getRegisterDataType(REGISTER_X) == dtComplex34Matrix) {
      complex34Matrix_t x, q, r;

      linkToComplexMatrixRegister(REGISTER_X, &x);

      if(x.header.matrixRows != x.header.matrixColumns) {
        displayCalcErrorMessage(ERROR_MATRIX_MISMATCH, ERR_REGISTER_LINE, REGISTER_X);
        errorMoreInfo("not a square matrix (%d" STD_CROSS "%d)", x.header.matrixRows, x.header.matrixColumns);
      }
      else {
        complex_QR_decomposition(&x, &q, &r);
        setSystemFlag(FLAG_ASLIFT);
        liftStack();
        convertComplex34MatrixToComplex34MatrixRegister(&q, REGISTER_Y);
        convertComplex34MatrixToComplex34MatrixRegister(&r, REGISTER_X);
        complexMatrixFree(&q);
        complexMatrixFree(&r);
      }
    }
    else {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      errorMoreInfo("DataType %" PRIu32 " is not a matrix", getRegisterDataType(REGISTER_X));
    }

    adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
  #endif // !TESTSUITE_BUILD
}


void fnEigenvalues(uint16_t unusedParamButMandatory) {
  #if !defined(TESTSUITE_BUILD)
    if(getRegisterDataType(REGISTER_X) == dtReal34Matrix) {
      real34Matrix_t x, res, ires;

      linkToRealMatrixRegister(REGISTER_X, &x);

      if(x.header.matrixRows != x.header.matrixColumns && x.header.matrixRows >= 2) {
        displayCalcErrorMessage(ERROR_MATRIX_MISMATCH, ERR_REGISTER_LINE, REGISTER_X);
        errorMoreInfo("rectangular or single-element matrix or (%d" STD_CROSS "%d)", x.header.matrixRows, x.header.matrixColumns);
      }
      else {
        setSystemFlag(FLAG_ASLIFT);
        liftStack();
        linkToRealMatrixRegister(REGISTER_Y, &x);
        ires.header.matrixRows = ires.header.matrixColumns = 0;
        ires.matrixElements = NULL;
        realEigenvalues(&x, &res, &ires);
        if(lastErrorCode == ERROR_NONE) {
          if(ires.matrixElements) {
            complex34Matrix_t cres;
            if(complexMatrixInit(&cres, res.header.matrixRows, res.header.matrixColumns)) {
              for(uint32_t i = 0; i < x.header.matrixRows * x.header.matrixColumns; i++) {
                real34Copy(&res.matrixElements[i],  VARIABLE_REAL34_DATA(&cres.matrixElements[i]));
                real34Copy(&ires.matrixElements[i], VARIABLE_IMAG34_DATA(&cres.matrixElements[i]));
              }
              convertComplex34MatrixToComplex34MatrixRegister(&cres, REGISTER_X);
              realMatrixFree(&ires);
              complexMatrixFree(&cres);
            }
            else {
              displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
            }
          }
          else {
            convertReal34MatrixToReal34MatrixRegister(&res, REGISTER_X);
          }
          realMatrixFree(&res);
        }
      }
    }
    else if(getRegisterDataType(REGISTER_X) == dtComplex34Matrix) {
      complex34Matrix_t x, res;

      linkToComplexMatrixRegister(REGISTER_X, &x);

      if(x.header.matrixRows != x.header.matrixColumns && x.header.matrixRows >= 2) {
        displayCalcErrorMessage(ERROR_MATRIX_MISMATCH, ERR_REGISTER_LINE, REGISTER_X);
        errorMoreInfo("rectangular or single-element matrix or (%d" STD_CROSS "%d)", x.header.matrixRows, x.header.matrixColumns);
      }
      else {
        setSystemFlag(FLAG_ASLIFT);
        liftStack();
        complexEigenvalues(&x, &res);
        convertComplex34MatrixToComplex34MatrixRegister(&res, REGISTER_X);
        complexMatrixFree(&res);
      }
    }
    else {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      errorMoreInfo("DataType %" PRIu32 " is not a matrix", getRegisterDataType(REGISTER_X));
    }

    adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
  #endif // !TESTSUITE_BUILD
}


void fnEigenvectors(uint16_t unusedParamButMandatory) {
  #if !defined(TESTSUITE_BUILD)
    if(getRegisterDataType(REGISTER_X) == dtReal34Matrix) {
      real34Matrix_t x, res, ires;

      linkToRealMatrixRegister(REGISTER_X, &x);

      if(x.header.matrixRows != x.header.matrixColumns && x.header.matrixRows >= 2) {
        displayCalcErrorMessage(ERROR_MATRIX_MISMATCH, ERR_REGISTER_LINE, REGISTER_X);
        errorMoreInfo("rectangular or single-element matrix or (%d" STD_CROSS "%d)", x.header.matrixRows, x.header.matrixColumns);
      }
      else {
        setSystemFlag(FLAG_ASLIFT);
        liftStack();
        ires.header.matrixRows = ires.header.matrixColumns = 0;
        ires.matrixElements = NULL;
        realEigenvectors(&x, &res, &ires);
        if(ires.matrixElements) {
          complex34Matrix_t cres;
          if(complexMatrixInit(&cres, res.header.matrixRows, res.header.matrixColumns)) {
            for(uint32_t i = 0; i < x.header.matrixRows * x.header.matrixColumns; i++) {
              real34Copy(&res.matrixElements[i],  VARIABLE_REAL34_DATA(&cres.matrixElements[i]));
              real34Copy(&ires.matrixElements[i], VARIABLE_IMAG34_DATA(&cres.matrixElements[i]));
            }
            convertComplex34MatrixToComplex34MatrixRegister(&cres, REGISTER_X);
            realMatrixFree(&ires);
            complexMatrixFree(&cres);
          }
          else {
            displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
          }
        }
        else {
          convertReal34MatrixToReal34MatrixRegister(&res, REGISTER_X);
        }
        realMatrixFree(&res);
      }
    }
    else if(getRegisterDataType(REGISTER_X) == dtComplex34Matrix) {
      complex34Matrix_t x, res;

      linkToComplexMatrixRegister(REGISTER_X, &x);

      if(x.header.matrixRows != x.header.matrixColumns && x.header.matrixRows >= 2) {
        displayCalcErrorMessage(ERROR_MATRIX_MISMATCH, ERR_REGISTER_LINE, REGISTER_X);
        errorMoreInfo("rectangular or single-element matrix or (%d" STD_CROSS "%d)", x.header.matrixRows, x.header.matrixColumns);
      }
      else {
        setSystemFlag(FLAG_ASLIFT);
        liftStack();
        complexEigenvectors(&x, &res);
        convertComplex34MatrixToComplex34MatrixRegister(&res, REGISTER_X);
        complexMatrixFree(&res);
      }
    }
    else {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      errorMoreInfo("DataType %" PRIu32 " is not a matrix", getRegisterDataType(REGISTER_X));
    }

    adjustResult(REGISTER_X, false, true, REGISTER_X, -1, -1);
#endif // !TESTSUITE_BUILD
}


#if !defined(TESTSUITE_BUILD)
  bool realMatrixInit(real34Matrix_t *matrix, uint16_t rows, uint16_t cols) {
    //Allocate Memory for Matrix
    const size_t neededSizeInBytes = (rows * cols) * REAL34_SIZE_IN_BYTES;
    if(!isMemoryBlockAvailable(neededSizeInBytes)) {
      matrix->header.matrixColumns = matrix->header.matrixRows = 0;
      matrix->matrixElements = NULL;
      return false;
    }
    matrix->matrixElements = allocWp43(neededSizeInBytes);

    matrix->header.matrixColumns = cols;
    matrix->header.matrixRows = rows;

    //Initialize with 0.
    for(uint32_t i = 0; i < rows * cols; i++) {
      real34Copy(const34_0, &matrix->matrixElements[i]);
    }
    return true;
  }


  void realMatrixFree(real34Matrix_t *matrix) {
    uint16_t cols = matrix->header.matrixColumns;
    uint16_t rows = matrix->header.matrixRows;

    freeWp43(matrix->matrixElements, (rows * cols) * REAL34_SIZE_IN_BYTES);
    matrix->matrixElements = NULL;
    matrix->header.matrixRows = matrix->header.matrixColumns = 0;
  }


  void realMatrixIdentity(real34Matrix_t *matrix, uint16_t size) {
    if(realMatrixInit(matrix, size, size)) {
      for(uint16_t i = 0; i < size; ++i) {
        real34Copy(const34_1, &matrix->matrixElements[i * size + i]);
      }
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  void realMatrixRedim(real34Matrix_t *matrix, uint16_t rows, uint16_t cols) {
    real34Matrix_t newMatrix;
    uint32_t elements;

    if(realMatrixInit(&newMatrix, rows, cols)) {
      elements = matrix->header.matrixRows * matrix->header.matrixColumns;
      if(elements > rows * cols) {
        elements = rows * cols;
      }
      for(uint32_t i = 0; i < elements; ++i) {
        real34Copy(&matrix->matrixElements[i], &newMatrix.matrixElements[i]);
      }
      realMatrixFree(matrix);
      matrix->header.matrixRows = newMatrix.header.matrixRows;
      matrix->header.matrixColumns = newMatrix.header.matrixColumns;
      matrix->matrixElements = newMatrix.matrixElements;
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  bool complexMatrixInit(complex34Matrix_t *matrix, uint16_t rows, uint16_t cols) {
    //Allocate Memory for Matrix
    const size_t neededSizeInBytes = (rows * cols) * COMPLEX34_SIZE_IN_BYTES;
    if(!isMemoryBlockAvailable(neededSizeInBytes)) {
      matrix->header.matrixColumns = matrix->header.matrixRows = 0;
      matrix->matrixElements = NULL;
      return false;
    }
    matrix->matrixElements = allocWp43(neededSizeInBytes);

    matrix->header.matrixColumns = cols;
    matrix->header.matrixRows = rows;

    //Initialize with 0.
    for(uint32_t i = 0; i < rows * cols; i++) {
      real34Copy(const34_0, VARIABLE_REAL34_DATA(&matrix->matrixElements[i]));
      real34Copy(const34_0, VARIABLE_IMAG34_DATA(&matrix->matrixElements[i]));
    }
    return true;
  }


  void complexMatrixFree(complex34Matrix_t *matrix) {
    uint16_t cols = matrix->header.matrixColumns;
    uint16_t rows = matrix->header.matrixRows;

    freeWp43(matrix->matrixElements, (rows * cols) * COMPLEX34_SIZE_IN_BYTES);
    matrix->matrixElements = NULL;
    matrix->header.matrixRows = matrix->header.matrixColumns = 0;
  }


  void complexMatrixIdentity(complex34Matrix_t *matrix, uint16_t size) {
    if(complexMatrixInit(matrix, size, size)) {
      for(uint16_t i = 0; i < size; ++i) {
        real34Copy(const34_1, VARIABLE_REAL34_DATA(&matrix->matrixElements[i * size + i]));
        real34Copy(const34_0, VARIABLE_IMAG34_DATA(&matrix->matrixElements[i * size + i]));
      }
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  void complexMatrixRedim(complex34Matrix_t *matrix, uint16_t rows, uint16_t cols) {
    complex34Matrix_t newMatrix;
    uint32_t elements;

    if(complexMatrixInit(&newMatrix, rows, cols)) {
      elements = matrix->header.matrixRows * matrix->header.matrixColumns;
      if(elements > rows * cols) {
        elements = rows * cols;
      }
      for(uint32_t i = 0; i < elements; ++i) {
        complex34Copy(&matrix->matrixElements[i], &newMatrix.matrixElements[i]);
      }
      complexMatrixFree(matrix);
      matrix->header.matrixRows = newMatrix.header.matrixRows;
      matrix->header.matrixColumns = newMatrix.header.matrixColumns;
      matrix->matrixElements = newMatrix.matrixElements;
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }

  /*
  void storeMatrixToXRegister(real34Matrix_t *matrix) {
    setSystemFlag(FLAG_ASLIFT);
    liftStack();
    clearSystemFlag(FLAG_ASLIFT);

    //WORKING//
    //openMatrixMIMPointer = matrix;
    // END WORKING//

    convertReal34MatrixToReal34MatrixRegister(matrix, REGISTER_X);
  }
  */

  void getMatrixFromRegister(calcRegister_t regist) {
    if(getRegisterDataType(regist) == dtReal34Matrix) {
      real34Matrix_t matrix;

      if(openMatrixMIMPointer.realMatrix.matrixElements) {
        realMatrixFree(&openMatrixMIMPointer.realMatrix);
      }
      convertReal34MatrixRegisterToReal34Matrix(regist, &matrix);

      openMatrixMIMPointer.realMatrix = matrix;
    }
    else if(getRegisterDataType(regist) == dtComplex34Matrix) {
      complex34Matrix_t matrix;

      if(openMatrixMIMPointer.complexMatrix.matrixElements) {
        complexMatrixFree(&openMatrixMIMPointer.complexMatrix);
      }
      convertComplex34MatrixRegisterToComplex34Matrix(regist, &matrix);

      openMatrixMIMPointer.complexMatrix = matrix;
    }
    else {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      errorMoreInfo("DataType %" PRIu32 " is not dataType dtRealMatrix", getRegisterDataType(regist));
      return;
    }
  }


  bool initMatrixRegister(calcRegister_t regist, uint16_t rows, uint16_t cols, bool complex) {
    const size_t neededSize = (rows * cols) * (complex ? TO_BLOCKS(COMPLEX34_SIZE_IN_BYTES) : TO_BLOCKS(REAL34_SIZE_IN_BYTES));
    reallocateRegister(regist, complex ? dtComplex34Matrix : dtReal34Matrix, neededSize, amNone);
    if(regist == INVALID_VARIABLE) {
      return false;
    }
    else if(lastErrorCode == ERROR_NONE) {
      // REGISTER_COMPLEX34_MATRIX_DBLOCK is same as REGISTER_REAL34_MATRIX_DBLOCK
      REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixRows    = rows;
      REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixColumns = cols;
      if(complex) {
        for(uint16_t i = 0; i < rows * cols; ++i) {
          real34Zero(VARIABLE_REAL34_DATA(REGISTER_COMPLEX34_MATRIX_M_ELEMENTS(regist) + i));
          real34Zero(VARIABLE_REAL34_DATA(REGISTER_COMPLEX34_MATRIX_M_ELEMENTS(regist) + i));
        }
      }
      else {
        for(uint16_t i = 0; i < rows * cols; ++i) {
          real34Zero(REGISTER_REAL34_MATRIX_M_ELEMENTS(regist) + i);
        }
      }
      return true;
    }
    else {
      return false;
    }
  }


  bool redimMatrixRegister(calcRegister_t regist, uint16_t rows, uint16_t cols) {
    const uint16_t origRows = REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixRows, origCols = REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixColumns;
    if(regist == INVALID_VARIABLE) {
      return false;
    }
    else if(getRegisterDataType(regist) == dtReal34Matrix) {
      const size_t oldSizeInBytes = (origRows * origCols) * REAL34_SIZE_IN_BYTES;
      const size_t newSizeInBytes = (rows     * cols    ) * REAL34_SIZE_IN_BYTES;
      if(oldSizeInBytes >= newSizeInBytes) {
        if(oldSizeInBytes > newSizeInBytes) {
          freeWp43(REGISTER_REAL34_MATRIX_M_ELEMENTS(regist) + rows * cols, oldSizeInBytes - newSizeInBytes);
        }
        REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixRows    = rows;
        REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixColumns = cols;
        return true;
      }
      else {
        real34Matrix_t newMatrix;
        convertReal34MatrixRegisterToReal34Matrix(regist, &newMatrix);
        if(lastErrorCode == ERROR_NONE) {
          reallocateRegister(regist, dtReal34Matrix, TO_BLOCKS(newSizeInBytes), amNone);
          if(lastErrorCode == ERROR_NONE) {
            REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixRows    = rows;
            REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixColumns = cols;
            for(uint16_t i = 0; i < origRows * origCols; ++i) {
              real34Copy(newMatrix.matrixElements + i, REGISTER_REAL34_MATRIX_M_ELEMENTS(regist) + i);
            }
            for(uint16_t i = origRows * origCols; i < rows * cols; ++i) {
              real34Zero(REGISTER_REAL34_MATRIX_M_ELEMENTS(regist) + i);
            }
            realMatrixFree(&newMatrix);
            return true;
          }
          else {
            realMatrixFree(&newMatrix);
            return false;
          }
        }
        else {
          return false;
        }
      }
    }
    else if(getRegisterDataType(regist) == dtComplex34Matrix) {
      const size_t oldSizeInBytes = (origRows * origCols) * TO_BLOCKS(COMPLEX34_SIZE_IN_BYTES);
      const size_t newSizeInBytes = (rows     * cols    ) * TO_BLOCKS(COMPLEX34_SIZE_IN_BYTES);
      if(oldSizeInBytes >= newSizeInBytes) {
        if(oldSizeInBytes > newSizeInBytes) {
          freeWp43(REGISTER_COMPLEX34_MATRIX_M_ELEMENTS(regist) + rows * cols, oldSizeInBytes - newSizeInBytes);
        }
        REGISTER_COMPLEX34_MATRIX_DBLOCK(regist)->matrixRows    = rows;
        REGISTER_COMPLEX34_MATRIX_DBLOCK(regist)->matrixColumns = cols;
        return true;
      }
      else {
        complex34Matrix_t newMatrix;
        convertComplex34MatrixRegisterToComplex34Matrix(regist, &newMatrix);
        if(lastErrorCode == ERROR_NONE) {
          reallocateRegister(regist, dtComplex34Matrix, TO_BLOCKS(newSizeInBytes), amNone);
          if(lastErrorCode == ERROR_NONE) {
            REGISTER_COMPLEX34_MATRIX_DBLOCK(regist)->matrixRows    = rows;
            REGISTER_COMPLEX34_MATRIX_DBLOCK(regist)->matrixColumns = cols;
            for(uint16_t i = 0; i < origRows * origCols; ++i) {
              complex34Copy(newMatrix.matrixElements + i, REGISTER_COMPLEX34_MATRIX_M_ELEMENTS(regist) + i);
            }
            for(uint16_t i = origRows * origCols; i < rows * cols; ++i) {
              real34Zero(VARIABLE_REAL34_DATA(REGISTER_REAL34_MATRIX_M_ELEMENTS(regist) + i));
              real34Zero(VARIABLE_IMAG34_DATA(REGISTER_REAL34_MATRIX_M_ELEMENTS(regist) + i));
            }
            complexMatrixFree(&newMatrix);
            return true;
          }
          else {
            complexMatrixFree(&newMatrix);
            return false;
          }
        }
        else {
          return false;
        }
      }
    }
    else {
      return initMatrixRegister(regist, rows, cols, false);
    }
  }

  calcRegister_t allocateNamedMatrix(const char *name, uint16_t rows, uint16_t cols) {
    const calcRegister_t regist = findOrAllocateNamedVariable(name);
    if(regist == INVALID_VARIABLE) {
      return INVALID_VARIABLE;
    }
    else if(redimMatrixRegister(regist, rows, cols)) {
      return regist;
    }
    else {
      return INVALID_VARIABLE;
    }
  }

  bool appendRowAtMatrixRegister(calcRegister_t regist) {
    const uint16_t rows = REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixRows, cols = REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixColumns;
    if(regist == INVALID_VARIABLE) {
      return false;
    }
    else if(getRegisterDataType(regist) == dtReal34Matrix || getRegisterDataType(regist) == dtComplex34Matrix) {
      return redimMatrixRegister(regist, rows + 1, cols);
    }
    else {
      return false;
    }
  }


  /* Duplicate */
  void copyRealMatrix(const real34Matrix_t *matrix, real34Matrix_t *res) {
    const uint16_t rows = matrix->header.matrixRows;
    const uint16_t cols = matrix->header.matrixColumns;
    int32_t i;

    if(realMatrixInit(res, rows, cols)) {
      for(i = 0; i < cols * rows; ++i) {
        real34Copy(&matrix->matrixElements[i], &res->matrixElements[i]);
      }
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }

  void copyComplexMatrix(const complex34Matrix_t *matrix, complex34Matrix_t *res) {
    const uint16_t rows = matrix->header.matrixRows;
    const uint16_t cols = matrix->header.matrixColumns;
    int32_t i;

    if(complexMatrixInit(res, rows, cols)) {
      for(i = 0; i < cols * rows; ++i) {
        complex34Copy(&matrix->matrixElements[i], &res->matrixElements[i]);
      }
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }
#endif // !TESTSUITE_BUILD


/* Link to real matrix register (data not copied) */
void linkToRealMatrixRegister(calcRegister_t regist, real34Matrix_t *linkedMatrix) {
  linkedMatrix->header.matrixRows    = REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixRows;
  linkedMatrix->header.matrixColumns = REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixColumns;
  linkedMatrix->matrixElements       = REGISTER_REAL34_MATRIX_M_ELEMENTS(regist);
}


/* Link to complex matrix register (data not copied) */
void linkToComplexMatrixRegister(calcRegister_t regist, complex34Matrix_t *linkedMatrix) {
  linkedMatrix->header.matrixRows    = REGISTER_COMPLEX34_MATRIX_DBLOCK(regist)->matrixRows;
  linkedMatrix->header.matrixColumns = REGISTER_COMPLEX34_MATRIX_DBLOCK(regist)->matrixColumns;
  linkedMatrix->matrixElements       = REGISTER_COMPLEX34_MATRIX_M_ELEMENTS(regist);
}


#if !defined(TESTSUITE_BUILD)
  /* Insert a row */
  void insRowRealMatrix(real34Matrix_t *matrix, uint16_t beforeRowNo) {
    const uint16_t rows = matrix->header.matrixRows;
    const uint16_t cols = matrix->header.matrixColumns;
    int32_t i;
    real34Matrix_t newMat;

    if(realMatrixInit(&newMat, rows + 1, cols)) {
      for(i = 0; i < beforeRowNo * cols; ++i) {
        real34Copy(&matrix->matrixElements[i], &newMat.matrixElements[i]);
      }
      for(i = 0; i < cols; ++i) {
        real34Copy(const34_0, &newMat.matrixElements[beforeRowNo * cols + i]);
      }
      for(i = beforeRowNo * cols; i < cols * rows; ++i) {
        real34Copy(&matrix->matrixElements[i], &newMat.matrixElements[i + cols]);
      }

      realMatrixFree(matrix);
      matrix->header.matrixRows    = newMat.header.matrixRows;
      matrix->header.matrixColumns = newMat.header.matrixColumns;
      matrix->matrixElements       = newMat.matrixElements;
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  void insRowComplexMatrix(complex34Matrix_t *matrix, uint16_t beforeRowNo) {
    const uint16_t rows = matrix->header.matrixRows;
    const uint16_t cols = matrix->header.matrixColumns;
    int32_t i;
    complex34Matrix_t newMat;

    if(complexMatrixInit(&newMat, rows + 1, cols)) {
      for(i = 0; i < beforeRowNo * cols; ++i) {
        complex34Copy(&matrix->matrixElements[i], &newMat.matrixElements[i]);
      }
      for(i = 0; i < cols; ++i) {
        real34Copy(const34_0, VARIABLE_REAL34_DATA(&newMat.matrixElements[beforeRowNo * cols + i]));
        real34Copy(const34_0, VARIABLE_IMAG34_DATA(&newMat.matrixElements[beforeRowNo * cols + i]));
      }
      for(i = beforeRowNo * cols; i < cols * rows; ++i) {
        complex34Copy(&matrix->matrixElements[i], &newMat.matrixElements[i + cols]);
      }

      complexMatrixFree(matrix);
      matrix->header.matrixRows    = newMat.header.matrixRows;
      matrix->header.matrixColumns = newMat.header.matrixColumns;
      matrix->matrixElements       = newMat.matrixElements;
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  /* Delete a row */
  void delRowRealMatrix(real34Matrix_t *matrix, uint16_t beforeRowNo) {
    const uint16_t rows = matrix->header.matrixRows;
    const uint16_t cols = matrix->header.matrixColumns;
    int32_t i;
    real34Matrix_t newMat;

    if(realMatrixInit(&newMat, rows - 1, cols)) {
      for(i = 0; i < beforeRowNo * cols; ++i) {
        real34Copy(&matrix->matrixElements[i], &newMat.matrixElements[i]);
      }
      for(i = (beforeRowNo + 1) * cols; i < cols * rows; ++i) {
        real34Copy(&matrix->matrixElements[i], &newMat.matrixElements[i - cols]);
      }

      realMatrixFree(matrix);
      matrix->header.matrixRows    = newMat.header.matrixRows;
      matrix->header.matrixColumns = newMat.header.matrixColumns;
      matrix->matrixElements       = newMat.matrixElements;
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  void delRowComplexMatrix(complex34Matrix_t *matrix, uint16_t beforeRowNo) {
    const uint16_t rows = matrix->header.matrixRows;
    const uint16_t cols = matrix->header.matrixColumns;
    int32_t i;
    complex34Matrix_t newMat;

    if(complexMatrixInit(&newMat, rows - 1, cols)) {
      for(i = 0; i < beforeRowNo * cols; ++i) {
        complex34Copy(&matrix->matrixElements[i], &newMat.matrixElements[i]);
      }
      for(i = (beforeRowNo + 1) * cols; i < cols * rows; ++i) {
        complex34Copy(&matrix->matrixElements[i], &newMat.matrixElements[i - cols]);
      }

      complexMatrixFree(matrix);
      matrix->header.matrixRows    = newMat.header.matrixRows;
      matrix->header.matrixColumns = newMat.header.matrixColumns;
      matrix->matrixElements       = newMat.matrixElements;
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  /* Transpose */
  void transposeRealMatrix(const real34Matrix_t *matrix, real34Matrix_t *res) {
    const uint16_t rows = matrix->header.matrixRows;
    const uint16_t cols = matrix->header.matrixColumns;
    int32_t i, j;

    if(matrix != res) {
      if(realMatrixInit(res, cols, rows)) {
        for(i = 0; i < rows; ++i) {
          for(j = 0; j < cols; ++j) {
            real34Copy(&matrix->matrixElements[i * cols + j], &res->matrixElements[j * rows + i]);
          }
        }
      }
      else {
        displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      }
    }
    else {
      real34Matrix_t tmp;
      copyRealMatrix(matrix, &tmp);
      if(tmp.matrixElements) {
        for(i = 0; i < rows; ++i) {
          for(j = 0; j < cols; ++j) {
            real34Copy(&tmp.matrixElements[i * cols + j], &res->matrixElements[j * rows + i]);
          }
        }
        realMatrixFree(&tmp);
        res->header.matrixRows    = cols;
        res->header.matrixColumns = rows;
      }
      else {
        displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      }
    }
  }


  void transposeComplexMatrix(const complex34Matrix_t *matrix, complex34Matrix_t *res) {
    const uint16_t rows = matrix->header.matrixRows;
    const uint16_t cols = matrix->header.matrixColumns;
    int32_t i, j;

    if(matrix != res) {
      if(complexMatrixInit(res, cols, rows)) {
        for(i = 0; i < rows; ++i) {
          for(j = 0; j < cols; ++j) {
            complex34Copy(&matrix->matrixElements[i * cols + j], &res->matrixElements[j * rows + i]);
          }
        }
      }
      else {
        displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      }
    }
    else {
      complex34Matrix_t tmp;
      copyComplexMatrix(matrix, &tmp);
      if(tmp.matrixElements) {
        for(i = 0; i < rows; ++i) {
          for(j = 0; j < cols; ++j) {
            complex34Copy(&tmp.matrixElements[i * cols + j], &res->matrixElements[j * rows + i]);
          }
        }
        complexMatrixFree(&tmp);
        res->header.matrixRows    = cols;
        res->header.matrixColumns = rows;
      }
      else {
        displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      }
    }
  }


  /* Addition and subtraction */
  static void addSubRealMatrices(const real34Matrix_t *y, const real34Matrix_t *x, bool subtraction, real34Matrix_t *res) {
    const uint16_t rows = y->header.matrixRows;
    const uint16_t cols = y->header.matrixColumns;
    int32_t i;

    if((y->header.matrixColumns != x->header.matrixColumns) || (y->header.matrixRows != x->header.matrixRows)) {
      res->matrixElements = NULL; // Matrix mismatch
      res->header.matrixRows = res->header.matrixColumns = 0;
      return;
    }

    if((y != res) && (x != res)) {
      if(!realMatrixInit(res, rows, cols)) {
        displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        return;
      }
    }
    for(i = 0; i < cols * rows; ++i) {
      if(subtraction) {
        real34Subtract(&y->matrixElements[i], &x->matrixElements[i], &res->matrixElements[i]);
      }
      else {
        real34Add(&y->matrixElements[i], &x->matrixElements[i], &res->matrixElements[i]);
      }
    }
  }


  void addRealMatrices(const real34Matrix_t *y, const real34Matrix_t *x, real34Matrix_t *res) {
    addSubRealMatrices(y, x, false, res);
  }


  void subtractRealMatrices(const real34Matrix_t *y, const real34Matrix_t *x, real34Matrix_t *res) {
    addSubRealMatrices(y, x, true, res);
  }


  static void addSubComplexMatrices(const complex34Matrix_t *y, const complex34Matrix_t *x, bool subtraction, complex34Matrix_t *res) {
    const uint16_t rows = y->header.matrixRows;
    const uint16_t cols = y->header.matrixColumns;
    int32_t i;

    if((y->header.matrixColumns != x->header.matrixColumns) || (y->header.matrixRows != x->header.matrixRows)) {
      res->matrixElements = NULL; // Matrix mismatch
      res->header.matrixRows = res->header.matrixColumns = 0;
      return;
    }

    if((y == res) || (x == res) || complexMatrixInit(res, rows, cols)) {
      for(i = 0; i < cols * rows; ++i) {
        if(subtraction) {
          real34Subtract(VARIABLE_REAL34_DATA(&y->matrixElements[i]), VARIABLE_REAL34_DATA(&x->matrixElements[i]), VARIABLE_REAL34_DATA(&res->matrixElements[i]));
          real34Subtract(VARIABLE_IMAG34_DATA(&y->matrixElements[i]), VARIABLE_IMAG34_DATA(&x->matrixElements[i]), VARIABLE_IMAG34_DATA(&res->matrixElements[i]));
        }
        else {
          real34Add(VARIABLE_REAL34_DATA(&y->matrixElements[i]), VARIABLE_REAL34_DATA(&x->matrixElements[i]), VARIABLE_REAL34_DATA(&res->matrixElements[i]));
          real34Add(VARIABLE_IMAG34_DATA(&y->matrixElements[i]), VARIABLE_IMAG34_DATA(&x->matrixElements[i]), VARIABLE_IMAG34_DATA(&res->matrixElements[i]));
        }
      }
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  void addComplexMatrices(const complex34Matrix_t *y, const complex34Matrix_t *x, complex34Matrix_t *res) {
    addSubComplexMatrices(y, x, false, res);
  }


  void subtractComplexMatrices(const complex34Matrix_t *y, const complex34Matrix_t *x, complex34Matrix_t *res) {
    addSubComplexMatrices(y, x, true, res);
  }


  /* Multiplication */
  void multiplyRealMatrix(const real34Matrix_t *matrix, const real34_t *x, real34Matrix_t *res) {
    const uint16_t rows = matrix->header.matrixRows;
    const uint16_t cols = matrix->header.matrixColumns;
    int32_t i;

    if(matrix == res || realMatrixInit(res, rows, cols)) {
      for(i = 0; i < cols * rows; ++i) {
        real34Multiply(&matrix->matrixElements[i], x, &res->matrixElements[i]);
      }
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  void _multiplyRealMatrix(const real34Matrix_t *matrix, const real_t *x, real34Matrix_t *res, realContext_t *realContext) {
    const uint16_t rows = matrix->header.matrixRows;
    const uint16_t cols = matrix->header.matrixColumns;
    int32_t i;
    real_t y;

    if(matrix == res || realMatrixInit(res, rows, cols)) {
      for(i = 0; i < cols * rows; ++i) {
        real34ToReal(&matrix->matrixElements[i], &y);
        realMultiply(&y, x, &y, realContext);
        realToReal34(&y, &res->matrixElements[i]);
      }
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  void multiplyRealMatrices(const real34Matrix_t *y, const real34Matrix_t *x, real34Matrix_t *res) {
    const uint16_t rows = y->header.matrixRows;
    const uint16_t iter = y->header.matrixColumns;
    const uint16_t cols = x->header.matrixColumns;
    int32_t i, j, k;
    real_t sum, prod, p, q;

    if(y->header.matrixColumns != x->header.matrixRows) {
      res->matrixElements = NULL; // Matrix mismatch
      res->header.matrixRows = res->header.matrixColumns = 0;
      return;
    }

    if(realMatrixInit(res, rows, cols)) {
      for(i = 0; i < rows; ++i) {
        for(j = 0; j < cols; ++j) {
          realCopy(const_0, &sum);
          realCopy(const_0, &prod);
          for(k = 0; k < iter; ++k) {
            real34ToReal(&y->matrixElements[i * iter + k], &p);
            real34ToReal(&x->matrixElements[k * cols + j], &q);
            realMultiply(&p, &q, &prod, &ctxtReal39);
            realAdd(&sum, &prod, &sum, &ctxtReal39);
          }
          realToReal34(&sum, &res->matrixElements[i * cols + j]);
        }
      }
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  void multiplyComplexMatrix(const complex34Matrix_t *matrix, const real34_t *xr, const real34_t *xi, complex34Matrix_t *res) {
    real_t _xr, _xi;

    real34ToReal(xr, &_xr); real34ToReal(xi, &_xi);
    _multiplyComplexMatrix(matrix, &_xr, &_xi, res, &ctxtReal39);
  }


  void _multiplyComplexMatrix(const complex34Matrix_t *matrix, const real_t *xr, const real_t *xi, complex34Matrix_t *res, realContext_t *realContext) {
    const uint16_t rows = matrix->header.matrixRows;
    const uint16_t cols = matrix->header.matrixColumns;
    int32_t i;
    real_t yr, yi;

    if(matrix == res || complexMatrixInit(res, rows, cols)) {
      for(i = 0; i < cols * rows; ++i) {
        real34ToReal(VARIABLE_REAL34_DATA(&matrix->matrixElements[i]), &yr);
        real34ToReal(VARIABLE_IMAG34_DATA(&matrix->matrixElements[i]), &yi);
        mulComplexComplex(&yr, &yi, xr, xi, &yr, &yi, &ctxtReal39);
        realToReal34(&yr, VARIABLE_REAL34_DATA(&res->matrixElements[i]));
        realToReal34(&yi, VARIABLE_IMAG34_DATA(&res->matrixElements[i]));
      }
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  static void mulCpxMat(const real_t *y, const real_t *x, uint16_t sizeY, uint16_t sizeYX, uint16_t sizeX, real_t *res, realContext_t *realContext) {
    int32_t i, j, k;
    real_t *sumr, prodr;
    real_t *sumi, prodi;

    for(i = 0; i < sizeY; ++i) {
      for(j = 0; j < sizeX; ++j) {
        sumr = res + (i * sizeX + j) * 2;
        sumi = sumr + 1;
        realCopy(const_0, sumr);   realCopy(const_0, sumi);
        realCopy(const_0, &prodr); realCopy(const_0, &prodi);
        for(k = 0; k < sizeYX; ++k) {
          if(realIsZero(y + (i * sizeYX + k) * 2 + 1) && realIsZero(x + (k * sizeX + j) * 2 + 1)) {
            realFMA(y + (i * sizeYX + k) * 2, x + (k * sizeX + j) * 2, sumr, sumr, realContext);
          }
          else {
            mulComplexComplex(y + (i * sizeYX + k) * 2, y + (i * sizeYX + k) * 2 + 1,
                              x + (k * sizeX  + j) * 2, x + (k * sizeX  + j) * 2 + 1,
                              &prodr, &prodi, realContext);
            realAdd(sumr, &prodr, sumr, realContext);
            realAdd(sumi, &prodi, sumi, realContext);
          }
        }
      }
    }
  }


  void multiplyComplexMatrices(const complex34Matrix_t *y, const complex34Matrix_t *x, complex34Matrix_t *res) {
    const uint16_t rows = y->header.matrixRows;
    const uint16_t iter = y->header.matrixColumns;
    const uint16_t cols = x->header.matrixColumns;
    int32_t i, j, k;
    real_t sumr, prodr, pr, qr;
    real_t sumi, prodi, pi, qi;

    if(y->header.matrixColumns != x->header.matrixRows) {
      res->matrixElements = NULL; // Matrix mismatch
      res->header.matrixRows = res->header.matrixColumns = 0;
      return;
    }

    if(complexMatrixInit(res, rows, cols)) {
      for(i = 0; i < rows; ++i) {
        for(j = 0; j < cols; ++j) {
          realCopy(const_0, &sumr);  realCopy(const_0, &sumi);
          realCopy(const_0, &prodr); realCopy(const_0, &prodi);
          for(k = 0; k < iter; ++k) {
            real34ToReal(VARIABLE_REAL34_DATA(&y->matrixElements[i * iter + k]), &pr);
            real34ToReal(VARIABLE_IMAG34_DATA(&y->matrixElements[i * iter + k]), &pi);
            real34ToReal(VARIABLE_REAL34_DATA(&x->matrixElements[k * cols + j]), &qr);
            real34ToReal(VARIABLE_IMAG34_DATA(&x->matrixElements[k * cols + j]), &qi);
            mulComplexComplex(&pr, &pi, &qr, &qi, &prodr, &prodi, &ctxtReal39);
            realAdd(&sumr, &prodr, &sumr, &ctxtReal39);
            realAdd(&sumi, &prodi, &sumi, &ctxtReal39);
          }
          realToReal34(&sumr, VARIABLE_REAL34_DATA(&res->matrixElements[i * cols + j]));
          realToReal34(&sumi, VARIABLE_IMAG34_DATA(&res->matrixElements[i * cols + j]));
        }
      }
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  /* Euclidean (Frobenius) norm */
  static void _euclideanNormRealMatrix(const real34Matrix_t *matrix, real_t *res, realContext_t *realContext) {
    real_t elem;

    realZero(res);
    for(int i = 0; i < matrix->header.matrixRows * matrix->header.matrixColumns; ++i) {
      real34ToReal(&matrix->matrixElements[i], &elem);
      realFMA(&elem, &elem, res, res, realContext);
    }
    realSquareRoot(res, res, realContext);
  }


  void euclideanNormRealMatrix(const real34Matrix_t *matrix, real34_t *res) {
    real_t sum;

    _euclideanNormRealMatrix(matrix, &sum, &ctxtReal39);
    realToReal34(&sum, res);
  }


  void euclideanNormComplexMatrix(const complex34Matrix_t *matrix, real34_t *res) {
    real_t elem, sum;

    realZero(&sum);
    for(int i = 0; i < matrix->header.matrixRows * matrix->header.matrixColumns; ++i) {
      real34ToReal(VARIABLE_REAL34_DATA(&matrix->matrixElements[i]), &elem);
      realFMA(&elem, &elem, &sum, &sum, &ctxtReal39);
      real34ToReal(VARIABLE_IMAG34_DATA(&matrix->matrixElements[i]), &elem);
      realFMA(&elem, &elem, &sum, &sum, &ctxtReal39);
    }
    realSquareRoot(&sum, &sum, &ctxtReal39);
    realToReal34(&sum, res);
  }


  /* Vectors */
  uint16_t realVectorSize(const real34Matrix_t *matrix) {
    if((matrix->header.matrixColumns != 1) && (matrix->header.matrixRows != 1)) {
      return 0;
    }
    else {
      return matrix->header.matrixColumns * matrix->header.matrixRows;
    }
  }


  static void _dotRealVectors(const real34Matrix_t *y, const real34Matrix_t *x, real_t *res, realContext_t *realContext) {
    const uint16_t elements = realVectorSize(y);
    int32_t i;
    real_t sum, p, q;

    realCopy(const_0, &sum);
    for(i = 0; i < elements; ++i) {
      real34ToReal(&y->matrixElements[i], &p);
      real34ToReal(&x->matrixElements[i], &q);
      realFMA(&p, &q, &sum, &sum, realContext);
    }
    realCopy(&sum, res);
  }


  void dotRealVectors(const real34Matrix_t *y, const real34Matrix_t *x, real34_t *res) {
    real_t p;

    if((realVectorSize(y) == 0) || (realVectorSize(x) == 0) || (realVectorSize(y) != realVectorSize(x))) {
      realToReal34(const_NaN, res); // Not a vector or mismatched
      return;
    }

    _dotRealVectors(y, x, &p, &ctxtReal39);
    realToReal34(&p, res);
  }


  void crossRealVectors(const real34Matrix_t *y, const real34Matrix_t *x, real34Matrix_t *res) {
    const uint16_t elementsY = realVectorSize(y);
    const uint16_t elementsX = realVectorSize(x);
    real_t a1, a2, a3, b1, b2, b3, p, q;

    if((elementsY == 0) || (elementsX == 0) || (elementsY > 3) || (elementsX > 3)) {
      return; // Not a vector or mismatched
    }

    real34ToReal(                 &y->matrixElements[0]            , &a1);
    real34ToReal(elementsY >= 2 ? &y->matrixElements[1] : const34_0, &a2);
    real34ToReal(elementsY >= 3 ? &y->matrixElements[2] : const34_0, &a3);

    real34ToReal(                 &x->matrixElements[0]            , &b1);
    real34ToReal(elementsX >= 2 ? &x->matrixElements[1] : const34_0, &b2);
    real34ToReal(elementsX >= 3 ? &x->matrixElements[2] : const34_0, &b3);

    if(realMatrixInit(res, 1, 3)) {
      realMultiply(&a2, &b3, &p, &ctxtReal39); realMultiply(&a3, &b2, &q, &ctxtReal39);
      realSubtract(&p, &q, &p, &ctxtReal39); realToReal34(&p, &res->matrixElements[0]);

      realMultiply(&a3, &b1, &p, &ctxtReal39); realMultiply(&a1, &b3, &q, &ctxtReal39);
      realSubtract(&p, &q, &p, &ctxtReal39); realToReal34(&p, &res->matrixElements[1]);

      realMultiply(&a1, &b2, &p, &ctxtReal39); realMultiply(&a2, &b1, &q, &ctxtReal39);
      realSubtract(&p, &q, &p, &ctxtReal39); realToReal34(&p, &res->matrixElements[2]);
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  uint16_t complexVectorSize(const complex34Matrix_t *matrix) {
    return realVectorSize((const real34Matrix_t *)matrix);
  }


  void dotComplexVectors(const complex34Matrix_t *y, const complex34Matrix_t *x, real34_t *res_r, real34_t *res_i) {
    const uint16_t elements = complexVectorSize(y);
    int32_t i;
    real_t sumr, prodr, pr, qr;
    real_t sumi, prodi, pi, qi;

    if((complexVectorSize(y) == 0) || (complexVectorSize(x) == 0) || (complexVectorSize(y) != complexVectorSize(x))) {
      realToReal34(const_NaN, res_r); realToReal34(const_NaN, res_i); // Not a vector or mismatched
      return;
    }

    realCopy(const_0, &sumr);  realCopy(const_0, &sumi);
    realCopy(const_0, &prodr); realCopy(const_0, &prodi);
    for(i = 0; i < elements; ++i) {
      real34ToReal(VARIABLE_REAL34_DATA(&y->matrixElements[i]), &pr); real34ToReal(VARIABLE_IMAG34_DATA(&y->matrixElements[i]), &pi);
      real34ToReal(VARIABLE_REAL34_DATA(&x->matrixElements[i]), &qr); real34ToReal(VARIABLE_IMAG34_DATA(&x->matrixElements[i]), &qi);
      mulComplexComplex(&pr, &pi, &qr, &qi, &prodr, &prodi, &ctxtReal39);
      realAdd(&sumr, &prodr, &sumr, &ctxtReal39);
      realAdd(&sumi, &prodi, &sumi, &ctxtReal39);
    }
    realToReal34(&sumr, res_r);
    realToReal34(&sumi, res_i);
  }


  void crossComplexVectors(const complex34Matrix_t *y, const complex34Matrix_t *x, complex34Matrix_t *res) {
    const uint16_t elementsY = complexVectorSize(y);
    const uint16_t elementsX = complexVectorSize(x);
    real_t a1r, a2r, a3r, b1r, b2r, b3r, pr, qr;
    real_t a1i, a2i, a3i, b1i, b2i, b3i, pi, qi;

    if((elementsY == 0) || (elementsX == 0) || (elementsY > 3) || (elementsX > 3)) {
      return; // Not a vector or mismatched
    }

    real34ToReal(                 VARIABLE_REAL34_DATA(&y->matrixElements[0])            , &a1r);
    real34ToReal(                 VARIABLE_IMAG34_DATA(&y->matrixElements[0])            , &a1i);
    real34ToReal(elementsY >= 2 ? VARIABLE_REAL34_DATA(&y->matrixElements[1]) : const34_0, &a2r);
    real34ToReal(elementsY >= 2 ? VARIABLE_IMAG34_DATA(&y->matrixElements[1]) : const34_0, &a2i);
    real34ToReal(elementsY >= 3 ? VARIABLE_REAL34_DATA(&y->matrixElements[2]) : const34_0, &a3r);
    real34ToReal(elementsY >= 3 ? VARIABLE_IMAG34_DATA(&y->matrixElements[2]) : const34_0, &a3i);

    real34ToReal(                 VARIABLE_REAL34_DATA(&x->matrixElements[0])            , &b1r);
    real34ToReal(                 VARIABLE_IMAG34_DATA(&x->matrixElements[0])            , &b1i);
    real34ToReal(elementsX >= 2 ? VARIABLE_REAL34_DATA(&x->matrixElements[1]) : const34_0, &b2r);
    real34ToReal(elementsX >= 2 ? VARIABLE_IMAG34_DATA(&x->matrixElements[1]) : const34_0, &b2i);
    real34ToReal(elementsX >= 3 ? VARIABLE_REAL34_DATA(&x->matrixElements[2]) : const34_0, &b3r);
    real34ToReal(elementsX >= 3 ? VARIABLE_IMAG34_DATA(&x->matrixElements[2]) : const34_0, &b3i);

    if(complexMatrixInit(res, 1, 3)) {
      mulComplexComplex(&a2r, &a2i, &b3r, &b3i, &pr, &pi, &ctxtReal39);
      mulComplexComplex(&a3r, &a3i, &b2r, &b2i, &qr, &qi, &ctxtReal39);
      realSubtract(&pr, &qr, &pr, &ctxtReal39), realSubtract(&pi, &qi, &pi, &ctxtReal39);
      realToReal34(&pr, VARIABLE_REAL34_DATA(&res->matrixElements[0]));
      realToReal34(&pi, VARIABLE_IMAG34_DATA(&res->matrixElements[0]));

      mulComplexComplex(&a3r, &a3i, &b1r, &b1i, &pr, &pi, &ctxtReal39);
      mulComplexComplex(&a1r, &a1i, &b3r, &b3i, &qr, &qi, &ctxtReal39);
      realSubtract(&pr, &qr, &pr, &ctxtReal39), realSubtract(&pi, &qi, &pi, &ctxtReal39);
      realToReal34(&pr, VARIABLE_REAL34_DATA(&res->matrixElements[1]));
      realToReal34(&pi, VARIABLE_IMAG34_DATA(&res->matrixElements[1]));

      mulComplexComplex(&a1r, &a1i, &b2r, &b2i, &pr, &pi, &ctxtReal39);
      mulComplexComplex(&a2r, &a2i, &b1r, &b1i, &qr, &qi, &ctxtReal39);
      realSubtract(&pr, &qr, &pr, &ctxtReal39), realSubtract(&pi, &qi, &pi, &ctxtReal39);
      realToReal34(&pr, VARIABLE_REAL34_DATA(&res->matrixElements[2]));
      realToReal34(&pi, VARIABLE_IMAG34_DATA(&res->matrixElements[2]));
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  void vectorAngle(const real34Matrix_t *y, const real34Matrix_t *x, real34_t *radians) {
    const uint16_t elements = realVectorSize(y);
    real_t a, b;

    if((elements == 0) || (realVectorSize(x) == 0) || (elements != realVectorSize(x))) {
      realToReal34(const_NaN, radians); // Not a vector or mismatched
      return;
    }

    if(elements == 2 || elements == 3) {
      _dotRealVectors(y, x, &a, &ctxtReal39);
      _euclideanNormRealMatrix(y, &b, &ctxtReal39);
      realDivide(&a, &b, &a, &ctxtReal39);
      _euclideanNormRealMatrix(x, &b, &ctxtReal39);
      realDivide(&a, &b, &a, &ctxtReal39);
      WP34S_Acos(&a, &a, &ctxtReal39);
      realToReal34(&a, radians);
    }
    else {
      realToReal34(const_NaN, radians);
    }
  }


  /* LU decomposition routine borrowed from WP 34s */
  void WP34S_LU_decomposition(const real34Matrix_t *matrix, real34Matrix_t *lu, uint16_t *p) {
    int i, j, k;
    int pvt;
    real_t max, t, u;
    real_t *tmpMat;

    const uint16_t m = matrix->header.matrixRows;
    const uint16_t n = matrix->header.matrixColumns;

    if(matrix->header.matrixRows != matrix->header.matrixColumns) {
      if(matrix != lu) {
        lu->matrixElements = NULL; // Matrix is not square
        lu->header.matrixRows = lu->header.matrixColumns = 0;
      }
      return;
    }

    if((tmpMat = allocWp43(m * n * REAL_SIZE_IN_BYTES))) {
      if(matrix != lu) {
        copyRealMatrix(matrix, lu);
      }

      if(lu->matrixElements) {
        for(i = 0; i < n; i++) {
          for(j = 0; j < n; j++) {
            real34ToReal(&lu->matrixElements[i * n + j], &tmpMat[i * n + j]);
          }
        }

        for(k = 0; k < n; k++) {
          /* Find the pivot row */
          pvt = k;
          realCopy(&tmpMat[k * n + k], &u);
          realCopyAbs(&u, &max);
          for(j = k + 1; j < n; j++) {
            realCopy(&tmpMat[j * n + k], &t);
            realCopyAbs(&t, &u);
            if(realCompareGreaterThan(&u, &max)) {
              realCopy(&u, &max);
              pvt = j;
            }
          }
          if(p != NULL) {
            *p++ = pvt;
          }

          /* pivot if required */
          if(pvt != k) {
            for(i = 0; i < n; i++) {
              realCopy(&tmpMat[k   * n + i], &t                  );
              realCopy(&tmpMat[pvt * n + i], &tmpMat[k   * n + i]);
              realCopy(&t,                   &tmpMat[pvt * n + i]);
            }
          }

          /* Check for singular */
          realCopy(&tmpMat[k * n + k], &t);
          if(realIsZero(&t)) {
            realMatrixFree(lu);
            return;
          }

          /* Find the lower triangular elements for column k */
          for(i = k + 1; i < n; i++) {
            realCopy(&tmpMat[k * n + k], &t);
            realCopy(&tmpMat[i * n + k], &u);
            realDivide(&u, &t, &max, &ctxtReal39);
            realCopy(&max, &tmpMat[i * n + k]);
          }
          /* Update the upper triangular elements */
          for(i = k + 1; i < n; i++) {
            for(j = k + 1; j < n; j++) {
              realCopy(&tmpMat[i * n + k], &t);
              realCopy(&tmpMat[k * n + j], &u);
              realMultiply(&t, &u, &max, &ctxtReal39);
              realCopy(&tmpMat[i * n + j], &t);
              realSubtract(&t, &max, &u, &ctxtReal39);
              realDivide(&u, &t, &max, &ctxtReal39); // condition number
              if(realCompareAbsLessThan(&max, const_1e_37)) {
                realZero(&tmpMat[i * n + j]); // prevent ill-conditionedness (likely singular)
              }
              else {
                realCopy(&u, &tmpMat[i * n + j]);
              }
            }
          }
        }

        for(i = 0; i < n; i++) {
          for(j = 0; j < n; j++) {
            realToReal34(&tmpMat[i * n + j], &lu->matrixElements[i * n + j]);
          }
        }
      }
      else {
        displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      }

      freeWp43(tmpMat, m * n * REAL_SIZE_IN_BYTES);
    }
    else {
      if(matrix != lu) {
        lu->matrixElements = NULL;
        lu->header.matrixRows = lu->header.matrixColumns = 0;
      }
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  static bool luCpxMat(real_t *tmpMat, uint16_t size, uint16_t *p, realContext_t *realContext) {
    int i, j, k;
    int pvt;
    real_t max, t, u, v;

    const uint16_t n = size;

    for(k = 0; k < n; k++) {
      /* Find the pivot row */
      pvt = k;
      complexMagnitude(&tmpMat[(k * n + k) * 2], &tmpMat[(k * n + k) * 2 + 1], &max, realContext);
      for(j = k + 1; j < n; j++) {
        complexMagnitude(&tmpMat[(j * n + k) * 2], &tmpMat[(j * n + k) * 2 + 1], &u, realContext);
        if(realCompareGreaterThan(&u, &max)) {
          realCopy(&u, &max);
          pvt = j;
        }
      }
      if(p != NULL) {
        *p++ = pvt;
      }

      /* pivot if required */
      if(pvt != k) {
        for(i = 0; i < n; i++) {
          realCopy(&tmpMat[(k   * n + i) * 2    ], &t                            );
          realCopy(&tmpMat[(pvt * n + i) * 2    ], &tmpMat[(k   * n + i) * 2    ]);
          realCopy(&t,                             &tmpMat[(pvt * n + i) * 2    ]);
          realCopy(&tmpMat[(k   * n + i) * 2 + 1], &t                            );
          realCopy(&tmpMat[(pvt * n + i) * 2 + 1], &tmpMat[(k   * n + i) * 2 + 1]);
          realCopy(&t,                             &tmpMat[(pvt * n + i) * 2 + 1]);
        }
      }

      /* Check for singular */
      if(realIsZero(&tmpMat[(k * n + k) * 2]) && realIsZero(&tmpMat[(k * n + k) * 2 + 1])) {
        return false;
      }

      /* Find the lower triangular elements for column k */
      for(i = k + 1; i < n; i++) {
        divComplexComplex(&tmpMat[(i * n + k) * 2], &tmpMat[(i * n + k) * 2 + 1], &tmpMat[(k * n + k) * 2], &tmpMat[(k * n + k) * 2 + 1], &t, &u, realContext);
        realCopy(&t, &tmpMat[(i * n + k) * 2    ]);
        realCopy(&u, &tmpMat[(i * n + k) * 2 + 1]);
      }
      /* Update the upper triangular elements */
      for(i = k + 1; i < n; i++) {
        for(j = k + 1; j < n; j++) {
          mulComplexComplex(&tmpMat[(i * n + k) * 2], &tmpMat[(i * n + k) * 2 + 1], &tmpMat[(k * n + j) * 2], &tmpMat[(k * n + j) * 2 + 1], &t, &u, realContext);
          realCopy(&tmpMat[(i * n + j) * 2], &v), realCopy(&tmpMat[(i * n + j) * 2 + 1], &max);
          realSubtract(&v,   &t, &tmpMat[(i * n + j) * 2    ], realContext);
          realSubtract(&max, &u, &tmpMat[(i * n + j) * 2 + 1], realContext);
          realDivide(&tmpMat[(i * n + j) * 2    ], &v,   &t, &ctxtReal39); // condition number
          realDivide(&tmpMat[(i * n + j) * 2 + 1], &max, &u, &ctxtReal39);
          if(realCompareAbsLessThan(&t, const_1e_37)) {
            realZero(&tmpMat[(i * n + j) * 2    ]); // prevent ill-conditionedness
          }
          if(realCompareAbsLessThan(&u, const_1e_37)) {
            realZero(&tmpMat[(i * n + j) * 2 + 1]);
          }
        }
      }
    }

    return true;
  }


  void complex_LU_decomposition(const complex34Matrix_t *matrix, complex34Matrix_t *lu, uint16_t *p) {
    int i, j;
    real_t *tmpMat;

    const uint16_t m = matrix->header.matrixRows;
    const uint16_t n = matrix->header.matrixColumns;

    if(matrix->header.matrixRows != matrix->header.matrixColumns) {
      if(matrix != lu) {
        lu->matrixElements = NULL; // Matrix is not square
        lu->header.matrixRows = lu->header.matrixColumns = 0;
      }
      return;
    }

    if((tmpMat = allocWp43(m * n * REAL_SIZE_IN_BYTES * 2))) {
      if(matrix != lu) {
        copyComplexMatrix(matrix, lu);
      }

      if(lu->matrixElements) {
        for(i = 0; i < n; i++) {
          for(j = 0; j < n; j++) {
            real34ToReal(VARIABLE_REAL34_DATA(&lu->matrixElements[i * n + j]), &tmpMat[(i * n + j) * 2    ]);
            real34ToReal(VARIABLE_IMAG34_DATA(&lu->matrixElements[i * n + j]), &tmpMat[(i * n + j) * 2 + 1]);
          }
        }

        if(luCpxMat(tmpMat, n, p, &ctxtReal39)) {
          for(i = 0; i < n; i++) {
            for(j = 0; j < n; j++) {
              realToReal34(&tmpMat[(i * n + j) * 2    ], VARIABLE_REAL34_DATA(&lu->matrixElements[i * n + j]));
              realToReal34(&tmpMat[(i * n + j) * 2 + 1], VARIABLE_IMAG34_DATA(&lu->matrixElements[i * n + j]));
            }
          }
        }
      }
      else {
        displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      }

      freeWp43(tmpMat, m * n * REAL_SIZE_IN_BYTES * 2);
    }
    else {
      if(matrix != lu) {
        lu->matrixElements = NULL; // Matrix is not square
        lu->header.matrixRows = lu->header.matrixColumns = 0;
      }
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  /* Swap 2 rows */
  void realMatrixSwapRows(const real34Matrix_t *matrix, real34Matrix_t *res, uint16_t a, uint16_t b) {
    const uint16_t rows = matrix->header.matrixRows;
    const uint16_t cols = matrix->header.matrixColumns;
    uint16_t i;
    real34_t t;

    if(matrix != res) {
      copyRealMatrix(matrix, res);
    }
    if(res->matrixElements) {
      if((a < rows) && (b < rows) && (a != b)) {
        for(i = 0; i < cols; i++) {
          real34Copy(&res->matrixElements[a * cols + i], &t);
          real34Copy(&res->matrixElements[b * cols + i], &res->matrixElements[a * cols + i]);
          real34Copy(&t,                                 &res->matrixElements[b * cols + i]);
        }
      }
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  void complexMatrixSwapRows(const complex34Matrix_t *matrix, complex34Matrix_t *res, uint16_t a, uint16_t b) {
    const uint16_t rows = matrix->header.matrixRows;
    const uint16_t cols = matrix->header.matrixColumns;
    uint16_t i;
    real34_t t;

    if(matrix != res) {
      copyComplexMatrix(matrix, res);
    }
    if(res->matrixElements) {
      if((a < rows) && (b < rows) && (a != b)) {
        for(i = 0; i < cols; i++) {
          real34Copy(VARIABLE_REAL34_DATA(&res->matrixElements[a * cols + i]), &t);
          real34Copy(VARIABLE_REAL34_DATA(&res->matrixElements[b * cols + i]), VARIABLE_REAL34_DATA(&res->matrixElements[a * cols + i]));
          real34Copy(&t,                                                       VARIABLE_REAL34_DATA(&res->matrixElements[b * cols + i]));
          real34Copy(VARIABLE_IMAG34_DATA(&res->matrixElements[a * cols + i]), &t);
          real34Copy(VARIABLE_IMAG34_DATA(&res->matrixElements[b * cols + i]), VARIABLE_IMAG34_DATA(&res->matrixElements[a * cols + i]));
          real34Copy(&t,                                                       VARIABLE_IMAG34_DATA(&res->matrixElements[b * cols + i]));
        }
      }
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  /* Determinant */
  static void detCpxMat(const real_t *matrix, uint16_t size, real_t *res_r, real_t *res_i, realContext_t *realContext) {
    uint16_t *p;
    real_t *lu;
    real_t tr, ti;

    if((lu = allocWp43(size * size * REAL_SIZE_IN_BYTES * 2))) {
      xcopy(lu, matrix, TO_BYTES(size * size * TO_BLOCKS(REAL_SIZE_IN_BYTES) * 2));
      if((p = allocWp43(size * 2))) {
        realCopy(const_1, &tr), realCopy(const_0, &ti);
        if(luCpxMat(lu, size, p, realContext)) {
          for(uint16_t i = 0; i < size; ++i) {
            mulComplexComplex(&tr, &ti, &lu[(i * size + i) * 2], &lu[(i * size + i) * 2 + 1], &tr, &ti, realContext);
          }
          for(uint16_t i = 0; i < size; ++i) {
            if(p[i] != i) {
              realChangeSign(&tr); realChangeSign(&ti);
            }
          }
          realCopy(&tr, res_r); realCopy(&ti, res_i);
        }
        else { // singular
          real34Copy(const34_0, res_r); real34Copy(const34_0, res_i);
        }

        freeWp43(p, size * 2);
      }
      else {
        displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        realCopy(const_NaN, res_r), realCopy(const_NaN, res_i);
      }

      freeWp43(lu, size * size * REAL_SIZE_IN_BYTES * 2);
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      realCopy(const_NaN, res_r), realCopy(const_NaN, res_i);
    }
  }


  void detRealMatrix(const real34Matrix_t *matrix, real34_t *res) {
    const uint16_t n = matrix->header.matrixColumns;
    real_t *lu;
    real_t tr, ti;

    if(matrix->header.matrixRows != n) {
      realToReal34(const_NaN, res);
      return;
    }

    if((lu = allocWp43(n * n * REAL_SIZE_IN_BYTES * 2))) {
      for(int i = 0; i < n * n; ++i) {
        real34ToReal(&matrix->matrixElements[i], &lu[i * 2]);
        realZero(&lu[i * 2 + 1]);
      }
      detCpxMat(lu, n, &tr, &ti, &ctxtReal51);
      freeWp43(lu, n * n * REAL_SIZE_IN_BYTES * 2);
      realToReal34(&tr, res);
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      realToReal34(const_NaN, res);
    }
  }


  void detComplexMatrix(const complex34Matrix_t *matrix, real34_t *res_r, real34_t *res_i) {
    const uint16_t n = matrix->header.matrixColumns;
    real_t *lu;
    real_t tr, ti;

    if(matrix->header.matrixRows != n) {
      realToReal34(const_NaN, res_r);
      realToReal34(const_NaN, res_i);
      return;
    }

    if((lu = allocWp43(n * n * REAL_SIZE_IN_BYTES * 2))) {
      for(int i = 0; i < n * n; ++i) {
        real34ToReal(VARIABLE_REAL34_DATA(&matrix->matrixElements[i]), &lu[i * 2    ]);
        real34ToReal(VARIABLE_IMAG34_DATA(&matrix->matrixElements[i]), &lu[i * 2 + 1]);
      }
      detCpxMat(lu, n, &tr, &ti, &ctxtReal51);
      freeWp43(lu, n * n * REAL_SIZE_IN_BYTES * 2);
      realToReal34(&tr, res_r);
      realToReal34(&ti, res_i);
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      realToReal34(const_NaN, res_r);
      realToReal34(const_NaN, res_i);
    }
  }


  /* Solve the linear equation Ax = b.
   * We do this by utilising the LU decomposition passed in in A and solving
   * the linear equation Ly = b for y, where L is the lower diagonal triangular
   * matrix with unity along the diagonal.  Then we solve the linear system
   * Ux = y, where U is the upper triangular matrix.
   */
  static void complex_matrix_pivoting_solve(const real_t *LU, uint16_t n, real_t *b, uint16_t *pivot, real_t *x, realContext_t *realContext) {
    uint16_t i, k;
    real_t rr, ri, tr, ti;

    /* Solve the first linear equation Ly = b */
    for(k = 0; k < n; k++) {
      if(k != pivot[k]) {
        real_t swap;
        realCopy(&b[k * 2], &swap);
        realCopy(&b[pivot[k] * 2], &b[k * 2]);
        realCopy(&swap, &b[pivot[k] * 2]);
        realCopy(&b[k * 2 + 1], &swap);
        realCopy(&b[pivot[k] * 2 + 1], &b[k * 2 + 1]);
        realCopy(&swap, &b[pivot[k] * 2 + 1]);
      }
      realCopy(&b[k * 2    ], x + (k * 2    ));
      realCopy(&b[k * 2 + 1], x + (k * 2 + 1));
      for(i = 0; i < k; i++) {
        realCopy(LU + (k * n + i) * 2,     &rr);
        realCopy(LU + (k * n + i) * 2 + 1, &ri);
        mulComplexComplex(&rr, &ri, x + i * 2, x + (i * 2 + 1), &tr, &ti, realContext);
        realSubtract(x + (k * 2    ), &tr, x + (k * 2    ), realContext);
        realSubtract(x + (k * 2 + 1), &ti, x + (k * 2 + 1), realContext);
      }
    }

    /* Solve the second linear equation Ux = y */
    for(k = n; k > 0; k--) {
      --k;
      for(i = k + 1; i < n; i++) {
        realCopy(LU + (k * n + i) * 2,     &rr);
        realCopy(LU + (k * n + i) * 2 + 1, &ri);
        mulComplexComplex(&rr, &ri, x + i * 2, x + (i * 2 + 1), &tr, &ti, realContext);
        realSubtract(x + (k * 2    ), &tr, x + (k * 2    ), realContext);
        realSubtract(x + (k * 2 + 1), &ti, x + (k * 2 + 1), realContext);
      }
      realCopy(LU + (k * n + k) * 2,     &rr);
      realCopy(LU + (k * n + k) * 2 + 1, &ri);
      divComplexComplex(x + (k * 2), x + (k * 2 + 1), &rr, &ri, x + (k * 2), x + (k * 2 + 1), realContext);
      ++k;
    }
  }


  /* Invert a matrix
   * Do this by calculating the LU decomposition and solving lots of systems
   * of linear equations.
   */
  static bool invCpxMat(real_t *matrix, uint16_t n, realContext_t *realContext) {
    real_t *x;
    real_t *lu;
    uint16_t *pivots;
    uint16_t i, j;
    real_t *b;

    if((lu = allocWp43(n * n * REAL_SIZE_IN_BYTES * 2))) {
      if((pivots = allocWp43(n * 2))) {
        for(i = 0; i < n * n * 2; i++) {
          realCopy(matrix + i, lu + i);
        }
        if(!luCpxMat(lu, n, pivots, realContext)) {
          freeWp43(lu, n * n * REAL_SIZE_IN_BYTES * 2);
          freeWp43(pivots, n * 2);
          return false;
        }

        {
          real_t maxVal, minVal;
          real_t p, q;
          realCopy(lu,     &p);
          realCopy(lu + 1, &q);
          complexMagnitude(&p, &q, &p, realContext);
          realCopy(&p, &maxVal);
          realCopy(&p, &minVal);
          for(i = 1; i < n; ++i) {
            realCopy(lu + (i * n + i) * 2,     &p);
            realCopy(lu + (i * n + i) * 2 + 1, &q);
            complexMagnitude(&p, &q, &p, realContext);
            if(realCompareLessThan(&p, &minVal)) {
              realCopy(&p, &minVal);
            }
            if(realCompareGreaterThan(&p, &maxVal)) {
              realCopy(&p, &maxVal);
            }
          }
          WP34S_Log10(&maxVal, &p, realContext);
          WP34S_Log10(&minVal, &q, realContext);
          realSubtract(&p, &q, &p, realContext);
          int32ToReal(33 - displayFormatDigits, &q);
          if(realCompareLessEqual(&q, &p)) {
            temporaryInformation = TI_INACCURATE;
          }
        }

        if((x = allocWp43(n * REAL_SIZE_IN_BYTES * 2))) {
          if((b = allocWp43(n * REAL_SIZE_IN_BYTES * 2))) {
            for(i = 0; i < n; i++) {
              for(j = 0; j < n; j++) {
                realCopy((i == j) ? const_1 : const_0, &b[j * 2    ]);
                realCopy(                     const_0, &b[j * 2 + 1]);
              }
              complex_matrix_pivoting_solve(lu, n, b, pivots, x, realContext);
              for(j = 0; j < n; j++) {
                realCopy(x + j * 2,     matrix + (j * n + i) * 2    );
                realCopy(x + j * 2 + 1, matrix + (j * n + i) * 2 + 1);
              }
            }
            freeWp43(b, n * REAL_SIZE_IN_BYTES * 2);
          }
          else {
            displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
          }
          freeWp43(x, n * REAL_SIZE_IN_BYTES * 2);
        }
        else {
          displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        }
        freeWp43(pivots, n * 2);
      }
      else {
        displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      }
      freeWp43(lu, n * n * REAL_SIZE_IN_BYTES * 2);
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
    return lastErrorCode == ERROR_NONE;
  }


  void invertRealMatrix(const real34Matrix_t *matrix, real34Matrix_t *res) {
    const uint16_t n = matrix->header.matrixColumns;
    real_t *tmpMat;
    uint16_t i, j;

    if(matrix->header.matrixRows != matrix->header.matrixColumns) {
      if(matrix != res) {
        res->matrixElements = NULL; // Matrix is not square
        res->header.matrixRows = res->header.matrixColumns = 0;
      }
      displayCalcErrorMessage(ERROR_MATRIX_MISMATCH, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      return;
    }

    if((tmpMat = allocWp43(n * n * REAL_SIZE_IN_BYTES * 2))) {
      for(i = 0; i < n; i++) {
        for(j = 0; j < n; j++) {
          real34ToReal(&matrix->matrixElements[i * n + j], &tmpMat[(i * n + j) * 2]);
          realZero(&tmpMat[(i * n + j) * 2 + 1]);
        }
      }

      if(invCpxMat(tmpMat, n, &ctxtReal39)) {
        if(matrix != res) {
          copyRealMatrix(matrix, res);
        }
        if(res->matrixElements) {
          for(i = 0; i < n; i++) {
            for(j = 0; j < n; j++) {
              realToReal34(&tmpMat[(i * n + j) * 2], &res->matrixElements[i * n + j]);
            }
          }
        }
        else {
          displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        }
      }
      else { // singular matrix
        if(matrix != res) {
          res->matrixElements = NULL;
          res->header.matrixRows = res->header.matrixColumns = 0;
        }
      }

      freeWp43(tmpMat, n * n * REAL_SIZE_IN_BYTES * 2);
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  void invertComplexMatrix(const complex34Matrix_t *matrix, complex34Matrix_t *res) {
    const uint16_t n = matrix->header.matrixColumns;
    real_t *tmpMat;
    uint16_t i, j;

    if(matrix->header.matrixRows != matrix->header.matrixColumns) {
      if(matrix != res) {
        res->matrixElements = NULL; // Matrix is not square
        res->header.matrixRows = res->header.matrixColumns = 0;
      }
      displayCalcErrorMessage(ERROR_MATRIX_MISMATCH, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      return;
    }

    if((tmpMat = allocWp43(n * n * REAL_SIZE_IN_BYTES * 2))) {
      for(i = 0; i < n; i++) {
        for(j = 0; j < n; j++) {
          real34ToReal(VARIABLE_REAL34_DATA(&matrix->matrixElements[i * n + j]), &tmpMat[(i * n + j) * 2    ]);
          real34ToReal(VARIABLE_IMAG34_DATA(&matrix->matrixElements[i * n + j]), &tmpMat[(i * n + j) * 2 + 1]);
        }
      }

      if(invCpxMat(tmpMat, n, &ctxtReal39)) {
        if(matrix != res) {
          copyComplexMatrix(matrix, res);
        }
        if(res->matrixElements) {
          for(i = 0; i < n; i++) {
            for(j = 0; j < n; j++) {
              realToReal34(&tmpMat[(i * n + j) * 2    ], VARIABLE_REAL34_DATA(&res->matrixElements[i * n + j]));
              realToReal34(&tmpMat[(i * n + j) * 2 + 1], VARIABLE_IMAG34_DATA(&res->matrixElements[i * n + j]));
            }
          }
        }
        else {
          displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        }
      }
      else { // singular matrix
        if(matrix != res) {
          res->matrixElements = NULL;
          res->header.matrixRows = res->header.matrixColumns = 0;
        }
      }

      freeWp43(tmpMat, n * n * REAL_SIZE_IN_BYTES * 2);
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  /* Division */
  void divideRealMatrix(const real34Matrix_t *matrix, const real34_t *x, real34Matrix_t *res) {
    const uint16_t rows = matrix->header.matrixRows;
    const uint16_t cols = matrix->header.matrixColumns;
    int32_t i;

    if(matrix == res || realMatrixInit(res, rows, cols)) {
      for(i = 0; i < cols * rows; ++i) {
        real34Divide(&matrix->matrixElements[i], x, &res->matrixElements[i]);
      }
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  void divideByRealMatrix(const real34_t *y, const real34Matrix_t *matrix, real34Matrix_t *res) {
    const uint16_t rows = matrix->header.matrixRows;
    const uint16_t cols = matrix->header.matrixColumns;
    int32_t i;

    if(matrix == res || realMatrixInit(res, rows, cols)) {
      for(i = 0; i < cols * rows; ++i) {
        real34Divide(y, &matrix->matrixElements[i], &res->matrixElements[i]);
      }
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  void _divideRealMatrix(const real34Matrix_t *matrix, const real_t *x, real34Matrix_t *res, realContext_t *realContext) {
    const uint16_t rows = matrix->header.matrixRows;
    const uint16_t cols = matrix->header.matrixColumns;
    int32_t i;
    real_t y;

    if(matrix == res || realMatrixInit(res, rows, cols)) {
      for(i = 0; i < cols * rows; ++i) {
        real34ToReal(&matrix->matrixElements[i], &y);
        realDivide(&y, x, &y, realContext);
        realToReal34(&y, &res->matrixElements[i]);
      }
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  void _divideByRealMatrix(const real_t *y, const real34Matrix_t *matrix, real34Matrix_t *res, realContext_t *realContext) {
    const uint16_t rows = matrix->header.matrixRows;
    const uint16_t cols = matrix->header.matrixColumns;
    int32_t i;
    real_t x;

    if(matrix == res || realMatrixInit(res, rows, cols)) {
      for(i = 0; i < cols * rows; ++i) {
        real34ToReal(&matrix->matrixElements[i], &x);
        realDivide(y, &x, &x, realContext);
        realToReal34(&x, &res->matrixElements[i]);
      }
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  void divideRealMatrices(const real34Matrix_t *y, const real34Matrix_t *x, real34Matrix_t *res) {
    const uint16_t sizeY = y->header.matrixRows;
    const uint16_t size  = x->header.matrixRows;
    real_t *yy, *xx, *rr;

    if(y->header.matrixColumns != x->header.matrixRows || x->header.matrixRows != x->header.matrixColumns) {
      res->matrixElements = NULL; // Matrix mismatch
      res->header.matrixRows = res->header.matrixColumns = 0;
      return;
    }

    if((yy = allocWp43(sizeY * size * REAL_SIZE_IN_BYTES * 2))) {
      if((xx = allocWp43(size * size * REAL_SIZE_IN_BYTES * 2))) {
        if((rr = allocWp43(sizeY * size * REAL_SIZE_IN_BYTES * 2))) {
          for(int i = 0; i < size * size; ++i) {
            real34ToReal(&x->matrixElements[i], &xx[i * 2]);
            realZero(&xx[i * 2 + 1]);
          }
          if(invCpxMat(xx, size, &ctxtReal39)) {
            for(int i = 0; i < size * size; ++i) {
              real34ToReal(&y->matrixElements[i], &yy[i * 2]);
              realZero(&xx[i * 2 + 1]);
            }
            mulCpxMat(yy, xx, sizeY, size, size, rr, &ctxtReal39);

            if(realMatrixInit(res, sizeY, size)) {
              for(int i = 0; i < sizeY * size; ++i) {
                realToReal34(&rr[i * 2], &res->matrixElements[i]);
              }
            }
            else {
              if(y != res && x != res) {
                res->matrixElements = NULL;
                res->header.matrixRows = res->header.matrixColumns = 0;
              }
              displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
            }
          }
          else { // singular matrix
            if(y != res && x != res) {
              res->matrixElements = NULL;
              res->header.matrixRows = res->header.matrixColumns = 0;
            }
          }

          freeWp43(rr, sizeY * size * REAL_SIZE_IN_BYTES * 2);
        }
        else {
          if(y != res && x != res) {
            res->matrixElements = NULL;
            res->header.matrixRows = res->header.matrixColumns = 0;
          }
          displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        }
        freeWp43(xx, size * size * REAL_SIZE_IN_BYTES * 2);
      }
      else {
        if(y != res && x != res) {
          res->matrixElements = NULL;
          res->header.matrixRows = res->header.matrixColumns = 0;
        }
        displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      }
      freeWp43(yy, sizeY * size * REAL_SIZE_IN_BYTES * 2);
    }
    else {
      if(y != res && x != res) {
        res->matrixElements = NULL;
        res->header.matrixRows = res->header.matrixColumns = 0;
      }
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }



  void divideComplexMatrix(const complex34Matrix_t *matrix, const real34_t *xr, const real34_t *xi, complex34Matrix_t *res) {
    real_t _xr, _xi;

    real34ToReal(xr, &_xr); real34ToReal(xi, &_xi);
    _divideComplexMatrix(matrix, &_xr, &_xi, res, &ctxtReal39);
  }


  void divideByComplexMatrix(const real34_t *yr, const real34_t *yi, const complex34Matrix_t *matrix, complex34Matrix_t *res) {
    real_t _yr, _yi;

    real34ToReal(yr, &_yr); real34ToReal(yi, &_yi);
    _divideByComplexMatrix(&_yr, &_yi, matrix, res, &ctxtReal39);
  }


  void _divideComplexMatrix(const complex34Matrix_t *matrix, const real_t *xr, const real_t *xi, complex34Matrix_t *res, realContext_t *realContext) {
    const uint16_t rows = matrix->header.matrixRows;
    const uint16_t cols = matrix->header.matrixColumns;
    int32_t i;
    real_t yr, yi;

    if(matrix == res || complexMatrixInit(res, rows, cols)) {
      for(i = 0; i < cols * rows; ++i) {
        real34ToReal(VARIABLE_REAL34_DATA(&matrix->matrixElements[i]), &yr);
        real34ToReal(VARIABLE_IMAG34_DATA(&matrix->matrixElements[i]), &yi);
        divComplexComplex(&yr, &yi, xr, xi, &yr, &yi, realContext);
        realToReal34(&yr, VARIABLE_REAL34_DATA(&res->matrixElements[i]));
        realToReal34(&yi, VARIABLE_IMAG34_DATA(&res->matrixElements[i]));
      }
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  void _divideByComplexMatrix(const real_t *yr, const real_t *yi, const complex34Matrix_t *matrix, complex34Matrix_t *res, realContext_t *realContext) {
    const uint16_t rows = matrix->header.matrixRows;
    const uint16_t cols = matrix->header.matrixColumns;
    int32_t i;
    real_t xr, xi;

    if(matrix == res || complexMatrixInit(res, rows, cols)) {
      for(i = 0; i < cols * rows; ++i) {
        real34ToReal(VARIABLE_REAL34_DATA(&matrix->matrixElements[i]), &xr);
        real34ToReal(VARIABLE_IMAG34_DATA(&matrix->matrixElements[i]), &xi);
        divComplexComplex(yr, yi, &xr, &xi, &xr, &xi, realContext);
        realToReal34(&xr, VARIABLE_REAL34_DATA(&res->matrixElements[i]));
        realToReal34(&xi, VARIABLE_IMAG34_DATA(&res->matrixElements[i]));
      }
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  void divideComplexMatrices(const complex34Matrix_t *y, const complex34Matrix_t *x, complex34Matrix_t *res) {
    const uint16_t sizeY = y->header.matrixRows;
    const uint16_t size  = x->header.matrixRows;
    real_t *yy, *xx, *rr;

    if(y->header.matrixColumns != x->header.matrixRows || x->header.matrixRows != x->header.matrixColumns) {
      res->matrixElements = NULL; // Matrix mismatch
      res->header.matrixRows = res->header.matrixColumns = 0;
      return;
    }

    if((yy = allocWp43(sizeY * size * REAL_SIZE_IN_BYTES * 2))) {
      if((xx = allocWp43(size * size * REAL_SIZE_IN_BYTES * 2))) {
        if((rr = allocWp43(sizeY * size * REAL_SIZE_IN_BYTES * 2))) {
          for(int i = 0; i < size * size; ++i) {
            real34ToReal(VARIABLE_REAL34_DATA(&x->matrixElements[i]), &xx[i * 2    ]);
            real34ToReal(VARIABLE_IMAG34_DATA(&x->matrixElements[i]), &xx[i * 2 + 1]);
          }
          if(invCpxMat(xx, size, &ctxtReal39)) {
            for(int i = 0; i < size * size; ++i) {
              real34ToReal(VARIABLE_REAL34_DATA(&y->matrixElements[i]), &yy[i * 2    ]);
              real34ToReal(VARIABLE_IMAG34_DATA(&y->matrixElements[i]), &yy[i * 2 + 1]);
            }
            mulCpxMat(yy, xx, sizeY, size, size, rr, &ctxtReal39);

            if(complexMatrixInit(res, sizeY, size)) {
              for(int i = 0; i < sizeY * size; ++i) {
                realToReal34(&rr[i * 2    ], VARIABLE_REAL34_DATA(&res->matrixElements[i]));
                realToReal34(&rr[i * 2 + 1], VARIABLE_IMAG34_DATA(&res->matrixElements[i]));
              }
            }
            else {
              if(y != res && x != res) {
                res->matrixElements = NULL;
                res->header.matrixRows = res->header.matrixColumns = 0;
              }
              displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
            }
          }
          else { // Singular matrix
            if(y != res && x != res) {
              res->matrixElements = NULL;
              res->header.matrixRows = res->header.matrixColumns = 0;
            }
          }

          freeWp43(rr, sizeY * size * REAL_SIZE_IN_BYTES * 2);
        }
        else {
          if(y != res && x != res) {
            res->matrixElements = NULL;
            res->header.matrixRows = res->header.matrixColumns = 0;
          }
          displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        }
        freeWp43(xx, size * size * REAL_SIZE_IN_BYTES * 2);
      }
      else {
        if(y != res && x != res) {
          res->matrixElements = NULL;
          res->header.matrixRows = res->header.matrixColumns = 0;
        }
        displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      }
      freeWp43(yy, sizeY * size * REAL_SIZE_IN_BYTES * 2);
    }
    else {
      if(y != res && x != res) {
        res->matrixElements = NULL;
        res->header.matrixRows = res->header.matrixColumns = 0;
      }
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  // Solve a system of linear equations Ac = b
  static void cpxLinearEqn(const real_t *a, const real_t *b, real_t *r, uint16_t size, realContext_t *realContext) {
    real_t *inv_a;

    if((inv_a = allocWp43(size * size * REAL_SIZE_IN_BYTES * 2))) {
      xcopy(inv_a, a, TO_BYTES(size * size * TO_BLOCKS(REAL_SIZE_IN_BYTES) * 2));
      if(invCpxMat(inv_a, size, realContext)) {
        mulCpxMat(inv_a, b, size, size, 1, r, realContext);
      }
      else if(lastErrorCode != ERROR_RAM_FULL) {
        displayCalcErrorMessage(ERROR_SINGULAR_MATRIX, ERR_REGISTER_LINE, REGISTER_X);
        errorMoreInfo("attempt to invert a singular matrix");
      }
      freeWp43(inv_a, size * size * REAL_SIZE_IN_BYTES * 2);
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  void real_matrix_linear_eqn(const real34Matrix_t *a, const real34Matrix_t *b, real34Matrix_t *r) {
    const uint16_t size = a->header.matrixColumns;
    real_t *aa, *bb, *rr;

    if(size != a->header.matrixRows) {
      displayCalcErrorMessage(ERROR_MATRIX_MISMATCH, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("not a square matrix (%d" STD_CROSS "%d)", a->header.matrixRows, a->header.matrixColumns);
      return;
    }
    if(b->header.matrixRows != size || b->header.matrixColumns != 1) {
      displayCalcErrorMessage(ERROR_MATRIX_MISMATCH, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("not a column vector or size mismatch (%d" STD_CROSS "%d)", b->header.matrixRows, b->header.matrixColumns);
      return;
    }

    if((aa = allocWp43(size * size * REAL_SIZE_IN_BYTES * 2))) {
      if((bb = allocWp43(size * REAL_SIZE_IN_BYTES * 2))) {
        if((rr = allocWp43(size * REAL_SIZE_IN_BYTES * 2))) {
          for(int i = 0; i < size * size; ++i) {
            real34ToReal(&a->matrixElements[i], &aa[i * 2]);
            realZero(&aa[i * 2 + 1]);
          }
          for(int i = 0; i < size; ++i) {
            real34ToReal(&b->matrixElements[i], &bb[i * 2]);
            realZero(&bb[i * 2 + 1]);
          }
          cpxLinearEqn(aa, bb, rr, size, &ctxtReal51);
          if(lastErrorCode == ERROR_NONE) {
            if(realMatrixInit(r, size, 1)) {
              for(int i = 0; i < size; ++i) {
                realToReal34(&rr[i * 2], &r->matrixElements[i]);
              }
            }
            else {
              if(a != r && b != r) {
                r->matrixElements = NULL;
                r->header.matrixRows = r->header.matrixColumns = 0;
              }
              displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
            }
          }
          else { // singular
            if(a != r && b != r) {
              r->matrixElements = NULL;
              r->header.matrixRows = r->header.matrixColumns = 0;
            }
          }
          freeWp43(rr, size * REAL_SIZE_IN_BYTES * 2);
        }
        else {
          if(a != r && b != r) {
            r->matrixElements = NULL;
            r->header.matrixRows = r->header.matrixColumns = 0;
          }
          displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        }
        freeWp43(bb, size * REAL_SIZE_IN_BYTES * 2);
      }
      else {
        if(a != r && b != r) {
          r->matrixElements = NULL;
          r->header.matrixRows = r->header.matrixColumns = 0;
        }
        displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      }
      freeWp43(aa, size * size * REAL_SIZE_IN_BYTES * 2);
    }
    else {
      if(a != r && b != r) {
        r->matrixElements = NULL;
        r->header.matrixRows = r->header.matrixColumns = 0;
      }
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


  void complex_matrix_linear_eqn(const complex34Matrix_t *a, const complex34Matrix_t *b, complex34Matrix_t *r) {
    const uint16_t size = a->header.matrixColumns;
    real_t *aa, *bb, *rr;

    if(size != a->header.matrixRows) {
      displayCalcErrorMessage(ERROR_MATRIX_MISMATCH, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("not a square matrix (%d" STD_CROSS "%d)", a->header.matrixRows, a->header.matrixColumns);
      return;
    }
    if(b->header.matrixRows != size || b->header.matrixColumns != 1) {
      displayCalcErrorMessage(ERROR_MATRIX_MISMATCH, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("not a column vector or size mismatch (%d" STD_CROSS "%d)", b->header.matrixRows, b->header.matrixColumns);
      return;
    }

    if((aa = allocWp43(size * size * REAL_SIZE_IN_BYTES * 2))) {
      if((bb = allocWp43(size * REAL_SIZE_IN_BYTES * 2))) {
        if((rr = allocWp43(size * REAL_SIZE_IN_BYTES * 2))) {
          for(int i = 0; i < size * size; ++i) {
            real34ToReal(VARIABLE_REAL34_DATA(&a->matrixElements[i]), &aa[i * 2    ]);
            real34ToReal(VARIABLE_IMAG34_DATA(&a->matrixElements[i]), &aa[i * 2 + 1]);
          }
          for(int i = 0; i < size; ++i) {
            real34ToReal(VARIABLE_REAL34_DATA(&b->matrixElements[i]), &bb[i * 2    ]);
            real34ToReal(VARIABLE_IMAG34_DATA(&b->matrixElements[i]), &bb[i * 2 + 1]);
          }
          cpxLinearEqn(aa, bb, rr, size, &ctxtReal51);
          if(lastErrorCode == ERROR_NONE) {
            if(complexMatrixInit(r, size, 1)) {
              for(int i = 0; i < size; ++i) {
                realToReal34(&rr[i * 2    ], VARIABLE_REAL34_DATA(&r->matrixElements[i]));
                realToReal34(&rr[i * 2 + 1], VARIABLE_IMAG34_DATA(&r->matrixElements[i]));
              }
            }
            else {
              if(a != r && b != r) {
                r->matrixElements = NULL;
                r->header.matrixRows = r->header.matrixColumns = 0;
              }
              displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
            }
          }
          else { // singular
            if(a != r && b != r) {
              r->matrixElements = NULL;
              r->header.matrixRows = r->header.matrixColumns = 0;
            }
          }
          freeWp43(rr, size * REAL_SIZE_IN_BYTES * 2);
        }
        else {
          if(a != r && b != r) {
            r->matrixElements = NULL;
            r->header.matrixRows = r->header.matrixColumns = 0;
          }
          displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        }
        freeWp43(bb, size * REAL_SIZE_IN_BYTES * 2);
      }
      else {
        if(a != r && b != r) {
          r->matrixElements = NULL;
          r->header.matrixRows = r->header.matrixColumns = 0;
        }
        displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      }
      freeWp43(aa, size * size * REAL_SIZE_IN_BYTES * 2);
    }
    else {
      if(a != r && b != r) {
        r->matrixElements = NULL;
        r->header.matrixRows = r->header.matrixColumns = 0;
      }
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }


/* Routines for calculating eigenpairs */
  static void adjCpxMat(const real_t *x, uint16_t size, real_t *res) {
    int32_t i, j;
    for(i = 0; i < size; ++i) {
      for(j = 0; j < size; ++j) {
        realCopy(x + (i * size + j) * 2,     res + (j * size + i) * 2    );
        realCopy(x + (i * size + j) * 2 + 1, res + (j * size + i) * 2 + 1);
        realChangeSign(res + (j * size + i) * 2 + 1);
      }
    }
  }


  static void QR_decomposition_householder(const real_t *mat, uint16_t size, real_t *q, real_t *r, realContext_t *realContext) {
    uint32_t i, j, k;

    real_t *bulk;
    real_t *matq, *matr;

    real_t *v, *qq, *qt, *newMat, sum, m, t;

    // Allocate
    if((bulk = allocWp43((size * size * 5 + size) * REAL_SIZE_IN_BYTES * 2))) {
      matr = bulk;
      matq = bulk + (size * size * 2);

      qq     = bulk + 2 * (size * size * 2);
      qt     = bulk + 3 * (size * size * 2);
      newMat = bulk + 4 * (size * size * 2);
      v      = bulk + 5 * (size * size * 2);

      // Copy mat to matr
      for(i = 0; i < size * size; i++) {
        realCopy(mat + i * 2,     matr + i * 2    );
        realCopy(mat + i * 2 + 1, matr + i * 2 + 1);
      }

      // Initialize Q
      for(i = 0; i < size * size; i++) {
        realCopy(const_0, matq + i * 2    );
        realCopy(const_0, matq + i * 2 + 1);
      }
      for(i = 0; i < size; i++) {
        realCopy(const_1, matq + (i * size + i) * 2);
      }

      // Calculate
      for(j = 0; j < (size - 1u); ++j) {
        // Column vector of submatrix
        realCopy(const_0, &sum);
        for(i = 0; i < (size - j); i++) {
          realCopy(matr + ((i + j) * size + j) * 2,     v + i * 2    );
          realCopy(matr + ((i + j) * size + j) * 2 + 1, v + i * 2 + 1);
          realFMA(v + i * 2,     v + i * 2,     &sum, &sum, realContext);
          realFMA(v + i * 2 + 1, v + i * 2 + 1, &sum, &sum, realContext);
        }
        realSquareRoot(&sum, &sum, realContext);

        // Calculate u = x - alpha e1
        if(realIsZero(v + 1)) {
          realSubtract(v, &sum, v, realContext);
        }
        else {
          realRectangularToPolar(v, v + 1, &m, &t, realContext);
          realPolarToRectangular(&sum, &t, &m, &t, realContext);
          realSubtract(v, &m, v, realContext);
          realAdd(v + 1, &t, v + 1, realContext);
        }

        // Euclidean norm
        realCopy(const_0, &sum);
        for(i = 0; i < (size - j); i++) {
          realFMA(v + i * 2,     v + i * 2,     &sum, &sum, realContext);
          realFMA(v + i * 2 + 1, v + i * 2 + 1, &sum, &sum, realContext);
        }
        realSquareRoot(&sum, &sum, realContext);

        // Calculate v = u / ||u||
        for(i = 0; i < (size - j); i++) {
          if(realIsZero(&sum)) {
            realCopy(v + i * 2,     &m);
            realCopy(v + i * 2 + 1, &t);
          }
          else if(realIsZero(v + i * 2 + 1)) {
            realDivide(v + i * 2, &sum, &m, realContext);
            realCopy(const_0, &t);
          }
          else {
            divComplexComplex(v + i * 2, v + i * 2 + 1, &sum, const_0, &m, &t, realContext);
          }
          realCopy(&m, v + i * 2    );
          realCopy(&t, v + i * 2 + 1);
        }

        // Initialize Q
        for(i = 0; i < size * size; i++) {
          realCopy(const_0, qq + i * 2    );
          realCopy(const_0, qq + i * 2 + 1);
        }
        for(i = 0; i < size; i++) {
          realCopy(const_1, qq + (i * size + i) * 2);
        }

        // Q -= 2vv*
        for(i = 0; i < (size - j); i++) {
          for(k = 0; k < (size - j); k++) {
            const uint32_t qe = (i + j) * size + k + j;
            realSubtract(const_0, (v + k * 2 + 1), &sum, realContext);
            if(realIsZero(v + i * 2 + 1) && realIsZero(&sum)) {
              realMultiply(v + i * 2, v + k * 2, &m, realContext);
              realCopy(const_0, &t);
            }
            else {
              mulComplexComplex((v + i * 2), (v + i * 2 + 1), (v + k * 2), &sum, &m, &t, realContext);
            }
            realMultiply(&m, const_2, &m, realContext);
            realMultiply(&t, const_2, &t, realContext);
            realSubtract(qq + qe * 2 ,    &m, qq + qe * 2,     realContext);
            realSubtract(qq + qe * 2 + 1, &t, qq + qe * 2 + 1, realContext);
          }
        }

        // Calculate R
        mulCpxMat(qq, matr, size, size, size, newMat, realContext);
        for(i = 0; i < size * size; i++) {
          realCopy(newMat + i * 2,     matr + i * 2    );
          realCopy(newMat + i * 2 + 1, matr + i * 2 + 1);
        }

        // Calculate Q
        adjCpxMat(qq, size, qt);
        mulCpxMat(matq, qt, size, size, size, newMat, realContext);
        for(i = 0; i < size * size; i++) {
          realCopy(newMat + i * 2,     matq + i * 2    );
          realCopy(newMat + i * 2 + 1, matq + i * 2 + 1);
        }
      }

      // Make sure R is triangular
      for(j = 0; j < (size - 1u); j++) {
        for(i = j + 1; i < size; i++) {
          realCopy(const_0, matr + (i * size + j) * 2    );
          realCopy(const_0, matr + (i * size + j) * 2 + 1);
        }
      }

      // Copy results
      for(i = 0; i < size * size; i++) {
        realCopy(matq + i * 2,     q + i * 2    );
        realCopy(matq + i * 2 + 1, q + i * 2 + 1);
        realCopy(matr + i * 2,     r + i * 2    );
        realCopy(matr + i * 2 + 1, r + i * 2 + 1);
      }

      // Cleanup
      freeWp43(bulk, (size * size * 5 + size) * REAL_SIZE_IN_BYTES * 2);
    }
    else {
      displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    }
  }
#endif // !TESTSUITE_BUILD


void real_QR_decomposition(const real34Matrix_t *matrix, real34Matrix_t *q, real34Matrix_t *r) {
  #if !defined(TESTSUITE_BUILD)
    if(matrix->header.matrixRows == matrix->header.matrixColumns) {
      real_t *mat, *matq, *matr;
      uint32_t i;

      // Allocate
      if((mat = allocWp43(matrix->header.matrixRows * matrix->header.matrixColumns * REAL_SIZE_IN_BYTES * 2 * 3))) {
        matq = mat + matrix->header.matrixRows * matrix->header.matrixColumns * 2;
        matr = mat + matrix->header.matrixRows * matrix->header.matrixColumns * 2 * 2;

        // Convert real34 to real
        for(i = 0; i < matrix->header.matrixRows * matrix->header.matrixColumns; i++) {
          real34ToReal(&matrix->matrixElements[i], mat + i * 2);
          realCopy(const_0, mat + i * 2 + 1);
        }

        // Calculate
        QR_decomposition_householder(mat, matrix->header.matrixRows, matq, matr, &ctxtReal39);
        if(lastErrorCode == ERROR_NONE) {
          // Write back
          if(realMatrixInit(q, matrix->header.matrixRows, matrix->header.matrixRows)) {
            for(i = 0; i < matrix->header.matrixRows * matrix->header.matrixRows; i++) {
              realToReal34(matq + i * 2, &q->matrixElements[i]);
            }
            if(realMatrixInit(r, matrix->header.matrixRows, matrix->header.matrixRows)) {
              for(i = 0; i < matrix->header.matrixRows * matrix->header.matrixRows; i++) {
                realToReal34(matr + i * 2, &r->matrixElements[i]);
              }
            }
            else {
              displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
            }
          }
          else {
            displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
          }
        }
        else {
          displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        }

        // Cleanup
        freeWp43(mat, matrix->header.matrixRows * matrix->header.matrixColumns * REAL_SIZE_IN_BYTES * 2 * 3);
      }
      else {
        displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      }
    }
  #endif // !TESTSUITE_BUILD
}


void complex_QR_decomposition(const complex34Matrix_t *matrix, complex34Matrix_t *q, complex34Matrix_t *r) {
  #if !defined(TESTSUITE_BUILD)
    if(matrix->header.matrixRows == matrix->header.matrixColumns) {
      real_t *mat, *matq, *matr;
      uint32_t i;

      // Allocate
      if((mat = allocWp43(matrix->header.matrixRows * matrix->header.matrixColumns * REAL_SIZE_IN_BYTES * 2 * 3))) {
        matq = mat + matrix->header.matrixRows * matrix->header.matrixColumns * 2;
        matr = mat + matrix->header.matrixRows * matrix->header.matrixColumns * 2 * 2;

        for(i = 0; i < matrix->header.matrixRows * matrix->header.matrixColumns; i++) {
          real34ToReal(VARIABLE_REAL34_DATA(&matrix->matrixElements[i]), mat + i * 2    );
          real34ToReal(VARIABLE_IMAG34_DATA(&matrix->matrixElements[i]), mat + i * 2 + 1);
        }

        // Calculate
        QR_decomposition_householder(mat, matrix->header.matrixRows, matq, matr, &ctxtReal39);

        // Write back
        if(complexMatrixInit(q, matrix->header.matrixRows, matrix->header.matrixRows)) {
          for(i = 0; i < matrix->header.matrixRows * matrix->header.matrixColumns; i++) {
            realToReal34(matq + i * 2,     VARIABLE_REAL34_DATA(&q->matrixElements[i]));
            realToReal34(matq + i * 2 + 1, VARIABLE_IMAG34_DATA(&q->matrixElements[i]));
          }
          if(complexMatrixInit(r, matrix->header.matrixRows, matrix->header.matrixRows)) {
            for(i = 0; i < matrix->header.matrixRows * matrix->header.matrixColumns; i++) {
              realToReal34(matr + i * 2,     VARIABLE_REAL34_DATA(&r->matrixElements[i]));
              realToReal34(matr + i * 2 + 1, VARIABLE_IMAG34_DATA(&r->matrixElements[i]));
            }
          }
          else {
            displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
          }
        }
        else {
          displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        }

        // Cleanup
        freeWp43(mat, matrix->header.matrixRows * matrix->header.matrixColumns * REAL_SIZE_IN_BYTES * 2 * 3);
      }
      else {
        displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      }
    }
  #endif // !TESTSUITE_BUILD
}


#if !defined(TESTSUITE_BUILD)
  static void calculateEigenvalues22(const real_t *mat, uint16_t size, real_t *t1r, real_t *t1i, real_t *t2r, real_t *t2i, realContext_t *realContext) {
    // Calculate eigenvalues of 2x2 bottom right submatrix
    // Characteristic equation of A = [[a b] [c d]] : t^2 - trace(A) t +      det(A) = 0
    //                                                t^2 -  (a + d) t + (a d - b c) = 0
    //                                            t = ((a + d) ± √(a^2 + 2 a d + d^2 - 4 (a d - b c))) / 2
    //                                                ((a + d) ± √(a^2         + d^2 - 2 a d + 4 b c)) / 2
    const real_t *ar, *ai, *br, *bi, *cr, *ci, *dr, *di;
    real_t trR, trI, detR, detI, discrR, discrI;

    ar = mat + ((size - 2) * size + (size - 2)) * 2; ai = ar + 1;
    br = mat + ((size - 2) * size + (size - 1)) * 2; bi = br + 1;
    cr = mat + ((size - 1) * size + (size - 2)) * 2; ci = cr + 1;
    dr = mat + ((size - 1) * size + (size - 1)) * 2; di = dr + 1;

    // determinant
    if(realIsZero(ai) && realIsZero(bi) && realIsZero(ci) && realIsZero(di)) {
      realMultiply(ar, dr, &detR, realContext);
      realChangeSign(&detR);
      realFMA(br, cr, &detR, &detR, realContext);
      realChangeSign(&detR);
      realZero(&detI);
    }
    else {
      mulComplexComplex(ar, ai, dr, di, &detR, &detI, realContext);
      mulComplexComplex(br, bi, cr, ci, &trR,  &trI,  realContext);
      realSubtract(&detR, &trR, &detR, realContext);
      realSubtract(&detI, &trI, &detI, realContext);
    }

    // trace
    realAdd(ar, dr, &trR, realContext);
    realAdd(ai, di, &trI, realContext);

    solveQuadraticEquation(const_1, const_0, &trR, &trI, &detR, &detI, &discrR, &discrI, t1r, t1i, t2r, t2i, realContext);
  }


  static bool _checkConditionNumberOfAddSub(const real_t *operand1, const real_t *operand2, const real_t *res, realContext_t *realContext) {
    real_t conditionNumber1, conditionNumber2;
    real_t *conditionNumber = &conditionNumber1;

    if(realIsZero(res)) {
      return false;
    }
    else {
      realDivide(res, operand1, &conditionNumber1, realContext); realSetPositiveSign(&conditionNumber1);
      realDivide(res, operand2, &conditionNumber2, realContext); realSetPositiveSign(&conditionNumber2);
      if(realIsZero(operand1)) {
        conditionNumber = &conditionNumber2;
      }
      else if(realIsZero(operand2)) {
        conditionNumber = &conditionNumber1;
      }
      else if(realCompareGreaterThan(&conditionNumber1, &conditionNumber2)) {
        conditionNumber = &conditionNumber2;
      }
      else {
        conditionNumber = &conditionNumber1;
      }
      return realCompareLessThan(conditionNumber, const_1e_37);
    }
  }
  static void _realCheckedAdd(const real_t *operand1, const real_t *operand2, real_t *res, realContext_t *realContext) {
    real_t r;
    realAdd(operand1, operand2, &r, realContext);
    if(_checkConditionNumberOfAddSub(operand1, operand2, &r, realContext)) {
      realZero(res);
    }
    else {
      realCopy(&r, res);
    }
  }
  static void _realCheckedSubtract(const real_t *operand1, const real_t *operand2, real_t *res, realContext_t *realContext) {
    real_t r;
    realSubtract(operand1, operand2, &r, realContext);
    if(_checkConditionNumberOfAddSub(operand1, operand2, &r, realContext)) {
      realZero(res);
    }
    else {
      realCopy(&r, res);
    }
  }

  static void solveCubicEquation(const real_t *c2Real, const real_t *c2Imag, const real_t *c1Real, const real_t *c1Imag, const real_t *c0Real, const real_t *c0Imag, real_t *rReal, real_t *rImag, real_t *x1Real, real_t *x1Imag, real_t *x2Real, real_t *x2Imag, real_t *x3Real, real_t *x3Imag, realContext_t *realContext) {
    // x^3 + b x^2 + c x + d = 0
    // Abramowitz & Stegun §3.8.2
    real_t qr, qi, rr, ri, s1r, s1i, s2r, s2i, ar, ai;

    // q = (c - b^2 / 3) / 3
    mulComplexComplex(c2Real, c2Imag, c2Real, c2Imag, &qr, &qi, realContext);
    realDivide(&qr, const_3, &qr, realContext), realDivide(&qi, const_3, &qi, realContext);
    realSubtract(c1Real, &qr, &qr, realContext), realSubtract(c1Imag, &qi, &qi, realContext);
    realDivide(&qr, const_3, &qr, realContext), realDivide(&qi, const_3, &qi, realContext);

    // r = (b c - 3 d) / 6 - b^3 / 27
    mulComplexComplex(c2Real, c2Imag, c1Real, c1Imag, &rr, &ri, realContext);
    realMultiply(c0Real, const_3, &ar, realContext), realMultiply(c0Imag, const_3, &ai, realContext);
    realSubtract(&rr, &ar, &rr, realContext), realSubtract(&ri, &ai, &ri, realContext);
    realDivide(&rr, const_6, &rr, realContext), realDivide(&ri, const_6, &ri, realContext);
    mulComplexComplex(c2Real, c2Imag, c2Real, c2Imag, &ar, &ai, realContext);
    mulComplexComplex(&ar, &ai, c2Real, c2Imag, &ar, &ai, realContext);
    realDivide(&ar, const_27, &ar, realContext), realDivide(&ai, const_27, &ai, realContext);
    realSubtract(&rr, &ar, &rr, realContext), realSubtract(&ri, &ai, &ri, realContext);

    // q^3 + r^2
    mulComplexComplex(&qr, &qi, &qr, &qi, rReal, rImag, realContext);
    mulComplexComplex(rReal, rImag, &qr, &qi, rReal, rImag, realContext);
    mulComplexComplex(&rr, &ri, &rr, &ri, &ar, &ai, realContext);
    realAdd(rReal, &ar, rReal, realContext), realAdd(rImag, &ai, rImag, realContext);

    // s1, s2 = cbrt(r ± sqrt(q^3 + r^2))
    sqrtComplex(rReal, rImag, &s1r, &s1i, realContext);
    realSubtract(&rr, &s1r, &s2r, realContext), realSubtract(&ri, &s1i, &s2i, realContext);
    realAdd(&rr, &s1r, &s1r, realContext), realAdd(&ri, &s1i, &s1i, realContext);
    curtComplex(&s1r, &s1i, &s1r, &s1i, realContext);
    curtComplex(&s2r, &s2i, &s2r, &s2i, realContext);

    // reusing q, r for (s1 ± s2)
    realAdd(&s1r, &s2r, &qr, realContext), realAdd(&s1i, &s2i, &qi, realContext);
    realSubtract(&s1r, &s2r, &rr, realContext), realSubtract(&s1i, &s2i, &ri, realContext);
    mulComplexComplex(&rr, &ri, const_0, const_root3on2, &rr, &ri, realContext);

    // roots
    realDivide(c2Real, const_3, x2Real, realContext), realDivide(c2Imag, const_3, x2Imag, realContext);
    realSubtract(&qr, x2Real, x1Real, realContext), realSubtract(&qi, x2Imag, x1Imag, realContext);
    realMultiply(&qr, const_1on2, x3Real, realContext), realMultiply(&qi, const_1on2, x3Imag, realContext);
    realAdd(x3Real, x2Real, x3Real, realContext), realAdd(x3Imag, x2Imag, x3Imag, realContext);
    realChangeSign(x3Real); realChangeSign(x3Imag);
    _realCheckedAdd(x3Real, &rr, x2Real, realContext), _realCheckedAdd(x3Imag, &ri, x2Imag, realContext);
    _realCheckedSubtract(x3Real, &rr, x3Real, realContext), _realCheckedSubtract(x3Imag, &ri, x3Imag, realContext);
  }

  static void calculateEigenvalues33(const real_t *mat, uint16_t size, real_t *t1r, real_t *t1i, real_t *t2r, real_t *t2i, real_t *t3r, real_t *t3i, realContext_t *realContext) {
    // Calculate eigenvalues of 3x3 (bottom right sub-)matrix
    // Characteristic equation of A = [[a b c] [d e f] [g h k]] : t^3 -    trace(A) t^2 +                                   c t -                                        det(A) = 0
    //                                                            t^3 - (a + e + k) t^2 + (a e - b d + a k - c g + e k - f h) t - a e k - b f g - c d h + c e g + b d k + a f h = 0

    const real_t *mr[9], *mi[9];
    real_t br, bi, cr, ci, dr, di, discrR, discrI;
    {
      real_t aekr, aeki, bfgr, bfgi, cdhr, cdhi, cegr, cegi, bdkr, bdki, afhr, afhi;

      mr[0] = mat + ((size - 3) * size + (size - 3)) * 2; mr[1] = mr[0] + 2; mr[2] = mr[1] + 2;
      mr[3] = mat + ((size - 2) * size + (size - 3)) * 2; mr[4] = mr[3] + 2; mr[5] = mr[4] + 2;
      mr[6] = mat + ((size - 1) * size + (size - 3)) * 2; mr[7] = mr[6] + 2; mr[8] = mr[7] + 2;
      for(int i = 0; i < 9; ++i) {
        mi[i] = mr[i] + 1;
      }

      // quadratic coefficient: trace
      realAdd(mr[0], mr[4], &br, realContext); realAdd(mi[0], mi[4], &bi, realContext);
      realAdd(&br, mr[8], &br, realContext); realAdd(&bi, mi[8], &bi, realContext);
      realChangeSign(&br); realChangeSign(&bi);

      // linear coefficient: sum of determinant of principal minors
      mulComplexComplex(mr[0], mi[0], mr[4], mi[4], &aekr, &aeki, realContext);
      mulComplexComplex(mr[1], mi[1], mr[3], mi[3], &bdkr, &bdki, realContext);
      mulComplexComplex(mr[0], mi[0], mr[8], mi[8], &cdhr, &cdhi, realContext);
      mulComplexComplex(mr[2], mi[2], mr[6], mi[6], &cegr, &cegi, realContext);
      mulComplexComplex(mr[4], mi[4], mr[8], mi[8], &bfgr, &bfgi, realContext);
      mulComplexComplex(mr[5], mi[5], mr[7], mi[7], &afhr, &afhi, realContext);
      realAdd(&aekr, &cdhr, &cr, realContext); realAdd(&aeki, &cdhi, &ci, realContext);
      realAdd(&cr, &bfgr, &cr, realContext); realAdd(&ci, &bfgi, &ci, realContext);
      realSubtract(&cr, &bdkr, &cr, realContext); realSubtract(&ci, &bdki, &ci, realContext);
      realSubtract(&cr, &cegr, &cr, realContext); realSubtract(&ci, &cegi, &ci, realContext);
      realSubtract(&cr, &afhr, &cr, realContext); realSubtract(&ci, &afhi, &ci, realContext);

      // constant term: determinant
      mulComplexComplex(&aekr, &aeki, mr[8], mi[8], &aekr, &aeki, realContext);
      mulComplexComplex(mr[1], mi[1], mr[5], mi[5], &bfgr, &bfgi, realContext);
      mulComplexComplex(&bfgr, &bfgi, mr[6], mi[6], &bfgr, &bfgi, realContext);
      mulComplexComplex(mr[2], mi[2], mr[3], mi[3], &cdhr, &cdhi, realContext);
      mulComplexComplex(&cdhr, &cdhi, mr[7], mi[7], &cdhr, &cdhi, realContext);
      mulComplexComplex(&cegr, &cegi, mr[4], mi[4], &cegr, &cegi, realContext);
      mulComplexComplex(&bdkr, &bdki, mr[8], mi[8], &bdkr, &bdki, realContext);
      mulComplexComplex(&afhr, &afhi, mr[0], mi[0], &afhr, &afhi, realContext);
      realAdd(&aekr, &bfgr, &dr, realContext); realAdd(&aeki, &bfgi, &di, realContext);
      realAdd(&dr, &cdhr, &dr, realContext); realAdd(&di, &cdhi, &di, realContext);
      realSubtract(&dr, &cegr, &dr, realContext); realSubtract(&di, &cegi, &di, realContext);
      realSubtract(&dr, &bdkr, &dr, realContext); realSubtract(&di, &bdki, &di, realContext);
      realSubtract(&dr, &afhr, &dr, realContext); realSubtract(&di, &afhi, &di, realContext);
      realChangeSign(&dr); realChangeSign(&di);
    }

    solveCubicEquation(&br, &bi, &cr, &ci, &dr, &di, &discrR, &discrI, t1r, t1i, t2r, t2i, t3r, t3i, realContext);
  }


  static void calculateQrShift(const real_t *mat, uint16_t size, real_t *re, real_t *im, realContext_t *realContext) {
    real_t t1r, t1i, t2r, t2i;
    real_t tmp, tmpR, tmpI;
    const real_t *dr, *di;

    dr = mat + ((size - 1) * size + (size - 1)) * 2; di = dr + 1;

    calculateEigenvalues22(mat, size, &t1r, &t1i, &t2r, &t2i, realContext);

    // Choose shift parameter
    realSubtract(&t1r, dr, &tmpR, realContext), realSubtract(&t1i, di, &tmpI, realContext);
    complexMagnitude(&tmpR, &tmpI, &tmpR, realContext);
    realSubtract(&t2r, dr, &tmp, realContext), realSubtract(&t2i, di, &tmpI, realContext);
    complexMagnitude(&tmp, &tmpI, &tmp, realContext);

    if(realCompareEqual(&t1r, &t2r) && realCompareEqual(&t1i, &t2i)) {
      realCopy(const_0, re); realCopy(const_0, im); // disable shift
    }
    else if(realCompareLessThan(&tmpR, &tmp)) {
      realCopy(&t1r, re); realCopy(&t1i, im);
    }
    else {
      realCopy(&t2r, re); realCopy(&t2i, im);
    }
  }


  static void sortEigenvalues(real_t *eig, uint16_t size, uint16_t begin_a, uint16_t begin_b, uint16_t end_b, realContext_t *realContext) {
    const uint16_t end_a = begin_b - 1;

    if(size < 2) { // ... trivial
      return;
    }
    else if(begin_a == end_b) { // ... trivial
      return;
    }
    else if(size == 2) { // simply compare
      complexMagnitude(eig,     eig + 1, eig + 2, realContext);
      complexMagnitude(eig + 6, eig + 7, eig + 4, realContext);
      if(realCompareLessThan(eig + 2, eig + 4)) {
        realCopy(eig,     eig + 2); realCopy(eig + 1, eig + 3);
        realCopy(eig + 6, eig    ); realCopy(eig + 7, eig + 1);
        realCopy(eig + 2, eig + 6); realCopy(eig + 1, eig + 7);
      }
    }
    else {
      uint16_t a = begin_a, b = begin_b;
      sortEigenvalues(eig, size, begin_a, (begin_a + end_a + 2) / 2, end_a, realContext);
      sortEigenvalues(eig, size, begin_b, (begin_b + end_b + 2) / 2, end_b, realContext);
      for(uint16_t i = begin_a; i <= end_b; i++) {
        complexMagnitude(eig + (i * size + i) * 2, eig + (i * size + i) * 2 + 1, eig + (i * size + (i + 1) % size) * 2, realContext);
      }
      for(uint16_t i = begin_a; i <= end_b; i++) {
        if(a > end_a) {
          realCopy(eig + (b * size + b) * 2,     eig + (i * size + (i + 2) % size) * 2    );
          realCopy(eig + (b * size + b) * 2 + 1, eig + (i * size + (i + 2) % size) * 2 + 1);
          ++b;
        }
        else if(b > end_b) {
          realCopy(eig + (a * size + a) * 2,     eig + (i * size + (i + 2) % size) * 2    );
          realCopy(eig + (a * size + a) * 2 + 1, eig + (i * size + (i + 2) % size) * 2 + 1);
          ++a;
        }
        else if(realCompareLessThan(eig + (a * size + (a + 1) % size) * 2, eig + (b * size + (b + 1) % size) * 2)) {
          realCopy(eig + (b * size + b) * 2,     eig + (i * size + (i + 2) % size) * 2    );
          realCopy(eig + (b * size + b) * 2 + 1, eig + (i * size + (i + 2) % size) * 2 + 1);
          ++b;
        }
        else {
          realCopy(eig + (a * size + a) * 2,     eig + (i * size + (i + 2) % size) * 2    );
          realCopy(eig + (a * size + a) * 2 + 1, eig + (i * size + (i + 2) % size) * 2 + 1);
          ++a;
        }
      }
      for(uint16_t i = begin_a; i <= end_b; i++) {
        realCopy(eig + (i * size + (i + 2) % size) * 2,     eig + (i * size + i) * 2    );
        realCopy(eig + (i * size + (i + 2) % size) * 2 + 1, eig + (i * size + i) * 2 + 1);
      }
    }
  }


  static void calculateEigenvalues(real_t *a, real_t *q, real_t *r, real_t *eig, uint16_t size, bool shifted, bool reducedSignificantDigits, realContext_t *realContext) {
    real_t   shiftRe, shiftIm;
    uint16_t i, j;
    bool     converged;

    if(size == 2) {
      calculateEigenvalues22(a, size, eig, eig + 1, eig + 6, eig + 7, realContext);
      sortEigenvalues(eig, size, 0, (size + 1) / 2, size - 1, realContext);
    }
    else if(size == 3) {
      calculateEigenvalues33(a, size, eig, eig + 1, eig + 8, eig + 9, eig + 16, eig + 17, realContext);
      sortEigenvalues(eig, size, 0, (size + 1) / 2, size - 1, realContext);
    }
    else {
      real_t tol, maxM, minM, tmpM;
      if(reducedSignificantDigits) {
        if(significantDigits == 0 || significantDigits >= 34) {
          realCopy(const_1e_37, &tol);
        }
        else {
          realCopy(const_1, &tol);
          tol.exponent -= (significantDigits + 3);
        }
      }
      else {
        realCopy(const_1e_37, &tol);
      }

      while(true) {
        if(shifted) {
          calculateQrShift(a, size, &shiftRe, &shiftIm, realContext);
          if((realIsZero(&shiftRe) && realIsZero(&shiftIm)) || realIsSpecial(&shiftRe) || realIsSpecial(&shiftIm)) {
            shifted = false;
          }
          else {
            for(i = 0; i < size; i++) {
              realSubtract(a + (i * size + i) * 2,     &shiftRe, a + (i * size + i) * 2,     realContext);
              realSubtract(a + (i * size + i) * 2 + 1, &shiftIm, a + (i * size + i) * 2 + 1, realContext);
            }
          }
        }
        QR_decomposition_householder(a, size, q, r, realContext);
        mulCpxMat(r, q, size, size, size, eig, realContext);
        if(shifted) {
          for(i = 0; i < size; i++) {
            realAdd(a   + (i * size + i) * 2,     &shiftRe, a   + (i * size + i) * 2,     realContext);
            realAdd(a   + (i * size + i) * 2 + 1, &shiftIm, a   + (i * size + i) * 2 + 1, realContext);
            realAdd(eig + (i * size + i) * 2,     &shiftRe, eig + (i * size + i) * 2,     realContext);
            realAdd(eig + (i * size + i) * 2 + 1, &shiftIm, eig + (i * size + i) * 2 + 1, realContext);
          }
        }

        converged = true;
        for(i = 0; i < size; i++) {
          if(realIsNaN(eig + i * 2) || realIsNaN(eig + i * 2 + 1)) {
            for(j = 0; j < size * size * 2; j++) {
              realCopy(a + j, eig + j);
            }
            converged = true;
            break;
          }
          else if(!WP34S_RelativeError(a + (i * size + i) * 2, eig + (i * size + i) * 2, &tol, realContext) || !WP34S_RelativeError(a + (i * size + i) * 2 + 1, eig + (i * size + i) * 2 + 1, &tol, realContext)) {
            converged = false;
          }
        }
        if(converged) {
          break;
        }
        else {
          for(i = 0; i < size * size * 2; i++) {
            realCopy(eig + i, a + i);
          }
        }
      }
      shifted = false;

      // check for condition number of the diagonal elements
      // at least one of eigenvalues is 0 if and only if the given matrix is singular
      sortEigenvalues(eig, size, 0, (size + 1) / 2, size - 1, realContext);
      complexMagnitude(eig, eig + 1, &maxM, realContext);
      for(i = 0; i < size; i++) {
        complexMagnitude(eig + (i * size + i) * 2, eig + (i * size + i) * 2 + 1, &tmpM, realContext);
        if(!realIsZero(&tmpM) && !realIsZero(&maxM) && realCompareLessThan(&tmpM, &tol)) { // ill-conditioned: possibly singular
          realMultiply(&maxM, &tol, &minM, realContext);
          for(i = 1; i < size; i++) {
            complexMagnitude(eig + (i * size + i) * 2, eig + (i * size + i) * 2 + 1, &tmpM, realContext);
            if(realCompareLessThan(&tmpM, &minM)) {
              realZero(eig + (i * size + i) * 2    );
              realZero(eig + (i * size + i) * 2 + 1);
            }
          }
        }
      }
    }
  }


  static void calculateEigenvectors(const any34Matrix_t *matrix, bool isComplex, real_t *a, real_t *q, real_t *r, real_t *eig, realContext_t *realContext) {
    // Call after the eigenvalues are calculated!
    const uint16_t size = matrix->header.matrixRows;
    uint16_t       i, j, k;
    real_t         *v = NULL;

    if(matrix->header.matrixRows == matrix->header.matrixColumns) {
      for(i = 0; i < size * size * 2; i++) {
        realZero(r + i);
      }
      for(k = 0; k < size; k++) {
        // Round to 34 digits
        realAdd(eig + (k * size + k) * 2,     const_0, eig + (k * size + k) * 2,     &ctxtReal34);
        realAdd(eig + (k * size + k) * 2 + 1, const_0, eig + (k * size + k) * 2 + 1, &ctxtReal34);
      }

      for(k = 0; k < size; k++) {
        if((v = allocWp43(size * REAL_SIZE_IN_BYTES * 2))) {
          for(i = 0; i < size * size; i++) {
            for(j = 0; j < size * 2; j++) {
              realCopy(const_NaN, v + j);
            }

            // Restore the original matrix
            if(isComplex) {
              for(j = 0; j < size * size; j++) {
                real34ToReal(VARIABLE_REAL34_DATA(&matrix->complexMatrix.matrixElements[j]), a + j * 2    );
                real34ToReal(VARIABLE_IMAG34_DATA(&matrix->complexMatrix.matrixElements[j]), a + j * 2 + 1);
              }
            }
            else {
              for(j = 0; j < size * size; j++) {
                real34ToReal(&matrix->realMatrix.matrixElements[j], a + j * 2);
                realZero(a + j * 2 + 1);
              }
            }

            // Subtract an eigenvalue
            for(j = 0; j < size; j++) {
              realSubtract(a + (j * size + j) * 2,     eig + (k * size + k) * 2,     a + (j * size + j) * 2,     realContext);
              realSubtract(a + (j * size + j) * 2 + 1, eig + (k * size + k) * 2 + 1, a + (j * size + j) * 2 + 1, realContext);
              realCopy(const_0, q + j * 2    );
              realCopy(const_0, q + j * 2 + 1);
            }

            // Make the equation matrices
            for(j = 0; j < size; j++) {
              realCopy(j ? const_0 : const_1, a + ((k + i / size) * size + (i + j) % size) * 2    );
              realCopy(    const_0,           a + ((k + i / size) * size + (i + j) % size) * 2 + 1);
            }
            realCopy(const_1, q + (k + i / size) * 2);

            // Solve linear equations from the submatrix
            lastErrorCode = ERROR_NONE;
            cpxLinearEqn(a, q, v, size, realContext);
            if(lastErrorCode != ERROR_SINGULAR_MATRIX) {
              break;
            }
          }
          if(lastErrorCode == ERROR_SINGULAR_MATRIX) {
            for(i = 0; i < size; i++) {
              realCopy(const_0, v + i * 2    );
              realCopy(const_0, v + i * 2 + 1);
            }
            lastErrorCode = ERROR_NONE;
          }
          for(i = 0; i < size; i++) {
            realCopy(v + i * 2,     r + (i * size + k) * 2    );
            realCopy(v + i * 2 + 1, r + (i * size + k) * 2 + 1);
          }

          freeWp43(v, size * REAL_SIZE_IN_BYTES * 2);
          v = NULL;
        }
        else {
          displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
          for(i = 0; i < size; i++) {
            realCopy(const_NaN, r + (i * size + k) * 2    );
            realCopy(const_NaN, r + (i * size + k) * 2 + 1);
          }
        }
      }
    }
  }
#endif // !TESTSUITE_BUILD


void realEigenvalues(const real34Matrix_t *matrix, real34Matrix_t *res, real34Matrix_t *ires) {
  #if !defined(TESTSUITE_BUILD)
    const uint16_t  size = matrix->header.matrixRows;
    real_t         *bulk, *a, *q, *r, *eig;
    uint16_t        i;
    bool            isComplex;
    bool            shifted = true;

    if(matrix->header.matrixRows == matrix->header.matrixColumns) {
      if((bulk = allocWp43(size * size * REAL_SIZE_IN_BYTES * 2 * 4))) {
        a   = bulk;
        q   = bulk + size * size * 2;
        r   = bulk + size * size * 2 * 2;
        eig = bulk + size * size * 2 * 3;

        // Convert real34 to real
        for(i = 0; i < size * size; i++) {
          real34ToReal(&matrix->matrixElements[i], a + i * 2);
          realZero(a + i * 2 + 1);
        }

        // Calculate
        calculateEigenvalues(a, q, r, eig, size, shifted, true, &ctxtReal75);
        shifted = false;

        // Check imaginary part (mutually conjugate complex roots are possible in real quadratic equations)
        isComplex = false;
        for(i = 0; i < size; i++) {
          if(!realIsZero(eig + (i * size + i) * 2 + 1)) {
            isComplex = true;
            break;
          }
        }

        // Write back
        if(matrix == res || realMatrixInit(res, size, size)) {
          for(i = 0; i < size; i++) {
            realToReal34(eig + (i * size + i) * 2, &res->matrixElements[i * size + i]);
          }
          if(isComplex && (ires != NULL)) {
            if(matrix == ires || res == ires || realMatrixInit(ires, size, size)) {
              for(i = 0; i < size; i++) {
                realToReal34(eig + (i * size + i) * 2 + 1, &ires->matrixElements[i * size + i]);
              }
            }
            else {
              displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
            }
          }
        }
        else {
          displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        }

        freeWp43(bulk, size * size * REAL_SIZE_IN_BYTES * 2 * 4);
      }
      else {
        displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      }
    }
  #endif // !TESTSUITE_BUILD
}


void complexEigenvalues(const complex34Matrix_t *matrix, complex34Matrix_t *res) {
  #if !defined(TESTSUITE_BUILD)
    const uint16_t  size = matrix->header.matrixRows;
    real_t         *bulk, *a, *q, *r, *eig;
    uint16_t        i;
    bool            shifted = true;

    if(matrix->header.matrixRows == matrix->header.matrixColumns) {
      if((bulk = allocWp43(size * size * REAL_SIZE_IN_BYTES * 2 * 4))) {
        a   = bulk;
        q   = bulk + size * size * 2;
        r   = bulk + size * size * 2 * 2;
        eig = bulk + size * size * 2 * 3;

        // Convert real34 to real
        for(i = 0; i < size * size; i++) {
          real34ToReal(VARIABLE_REAL34_DATA(&matrix->matrixElements[i]), a + i * 2    );
          real34ToReal(VARIABLE_IMAG34_DATA(&matrix->matrixElements[i]), a + i * 2 + 1);
        }

        // Calculate
        calculateEigenvalues(a, q, r, eig, size, shifted, true, &ctxtReal75);
        shifted = false;

        // Write back
        if(matrix == res || complexMatrixInit(res, size, size)) {
          for(i = 0; i < size; i++) {
            realToReal34(eig + (i * size + i) * 2,     VARIABLE_REAL34_DATA(&res->matrixElements[i * size + i]));
            realToReal34(eig + (i * size + i) * 2 + 1, VARIABLE_IMAG34_DATA(&res->matrixElements[i * size + i]));
          }
        }
        else {
          displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        }

        freeWp43(bulk, size * size * REAL_SIZE_IN_BYTES * 2 * 4);
      }
      else {
        displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      }
    }
  #endif // !TESTSUITE_BUILD
}


void realEigenvectors(const real34Matrix_t *matrix, real34Matrix_t *res, real34Matrix_t *ires) {
  #if !defined(TESTSUITE_BUILD)
    const uint16_t  size = matrix->header.matrixRows;
    real_t         *bulk, *a, *q, *r, *eig;
    uint16_t        i, j;
    bool            isComplex;
    bool            shifted = true;

    if(matrix->header.matrixRows == matrix->header.matrixColumns) {
      if((bulk = allocWp43(size * size * REAL_SIZE_IN_BYTES * 2 * 4))) {
        a   = bulk;
        q   = bulk + size * size * 2;
        r   = bulk + size * size * 2 * 2;
        eig = bulk + size * size * 2 * 3;

        // Convert real34 to real
        for(i = 0; i < size * size; i++) {
          real34ToReal(&matrix->matrixElements[i], a + i * 2);
          realZero(a + i * 2 + 1);
        }

        // Calculate eigenvalues
        calculateEigenvalues(a, q, r, eig, size, shifted, false, &ctxtReal75);
        shifted = false;
        calculateEigenvectors((any34Matrix_t *)matrix, false, a, q, r, eig, &ctxtReal75);

        // Check imaginary part (mutually conjugate complex roots are possible in real quadratic equations)
        isComplex = false;
        for(i = 0; i < size * size; i++) {
          if(!realIsZero(r + i * 2 + 1)) {
            isComplex = true;
            break;
          }
        }

        // Normalize
        for(j = 0; j < size; j++) {
          real_t sum;
          realZero(&sum);
          for(i = 0; i < size; i++) {
            realFMA(r + (i * size + j) * 2,     r + (i * size + j) * 2,     &sum, &sum, &ctxtReal75);
            realFMA(r + (i * size + j) * 2 + 1, r + (i * size + j) * 2 + 1, &sum, &sum, &ctxtReal75);
          }
          realSquareRoot(&sum, &sum, &ctxtReal75);
          if(!realIsZero(&sum) && !realIsSpecial(&sum)) {
            for(i = 0; i < size; i++) {
              realDivide(r + (i * size + j) * 2,     &sum, r + (i * size + j) * 2,     &ctxtReal75);
              realDivide(r + (i * size + j) * 2 + 1, &sum, r + (i * size + j) * 2 + 1, &ctxtReal75);
            }
          }
        }

        // Write back
        if(matrix == res || realMatrixInit(res, size, size)) {
          for(i = 0; i < size * size; i++) {
            realToReal34(r + i * 2, &res->matrixElements[i]);
          }
          if(isComplex && (ires != NULL)) {
            if(matrix == ires || res == ires || realMatrixInit(ires, size, size)) {
              for(i = 0; i < size * size; i++) {
                realToReal34(r + i * 2 + 1, &ires->matrixElements[i]);
              }
            }
            else {
              displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
            }
          }
        }
        else {
          displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        }

        freeWp43(bulk, size * size * REAL_SIZE_IN_BYTES * 2 * 4);
      }
      else {
        displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      }
    }
  #endif // !TESTSUITE_BUILD
}


void complexEigenvectors(const complex34Matrix_t *matrix, complex34Matrix_t *res) {
  #if !defined(TESTSUITE_BUILD)
    const uint16_t  size = matrix->header.matrixRows;
    real_t         *bulk, *a, *q, *r, *eig;
    uint16_t        i;
    bool            shifted = true;

    if(matrix->header.matrixRows == matrix->header.matrixColumns) {
      if((bulk = allocWp43(size * size * REAL_SIZE_IN_BYTES * 2 * 4))) {
        a   = bulk;
        q   = bulk + size * size * 2;
        r   = bulk + size * size * 2 * 2;
        eig = bulk + size * size * 2 * 3;

        // Convert real34 to real
        for(i = 0; i < size * size; i++) {
          real34ToReal(VARIABLE_REAL34_DATA(&matrix->matrixElements[i]), a + i * 2    );
          real34ToReal(VARIABLE_IMAG34_DATA(&matrix->matrixElements[i]), a + i * 2 + 1);
          realZero(a + i * 2 + 1);
        }

        // Calculate eigenvalues
        calculateEigenvalues(a, q, r, eig, size, shifted, false, &ctxtReal75);
        shifted = false;
        calculateEigenvectors((any34Matrix_t *)matrix, true, a, q, r, eig, &ctxtReal75);

        // Write back
        if(matrix == res || complexMatrixInit(res, size, size)) {
          for(i = 0; i < size * size; i++) {
            realToReal34(r + i * 2,     VARIABLE_REAL34_DATA(&res->matrixElements[i]));
            realToReal34(r + i * 2 + 1, VARIABLE_IMAG34_DATA(&res->matrixElements[i]));
          }
        }
        else {
          displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        }

        freeWp43(bulk, size * size * REAL_SIZE_IN_BYTES * 2 * 4);
      }
      else {
        displayCalcErrorMessage(ERROR_RAM_FULL, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      }
    }
  #endif // !TESTSUITE_BUILD
}


#if !defined(TESTSUITE_BUILD)
  static void elementwiseRemaGetResult(bool *complex, real34Matrix_t *x, complex34Matrix_t *xc, int i) {
    if((!(*complex)) && (getRegisterDataType(REGISTER_X) != dtComplex34)) {
      if(getRegisterDataType(REGISTER_X) == dtLongInteger) {
        convertLongIntegerRegisterToReal34Register(REGISTER_X, REGISTER_X);
      }
      if(getRegisterDataType(REGISTER_X) == dtReal34) {
        real34Copy(REGISTER_REAL34_DATA(REGISTER_X), &x->matrixElements[i]);
      }
    }
    else {
      if(!(*complex)) {
        convertReal34MatrixToComplex34Matrix(x, xc);
        if(lastErrorCode != ERROR_RAM_FULL) {
          realMatrixFree(x);
          *complex = true;
        }
      }
      if(getRegisterDataType(REGISTER_X) == dtComplex34) {
        complex34Copy(REGISTER_COMPLEX34_DATA(REGISTER_X), &xc->matrixElements[i]);
      }
      else {
        if(getRegisterDataType(REGISTER_X) == dtLongInteger) {
          convertLongIntegerRegisterToReal34Register(REGISTER_X, REGISTER_X);
        }
        real34Copy(REGISTER_REAL34_DATA(REGISTER_X), VARIABLE_REAL34_DATA(&xc->matrixElements[i]));
        real34Copy(const34_0,                        VARIABLE_IMAG34_DATA(&xc->matrixElements[i]));
      }
    }
  }
#endif // !TESTSUITE_BUILD


/* Elementwise function call */
void elementwiseRema(void (*f)(void)) {
  #if !defined(TESTSUITE_BUILD)
    real34Matrix_t    x;
    complex34Matrix_t xc;
    bool              complex = false;

    convertReal34MatrixRegisterToReal34Matrix(REGISTER_X, &x);
    const int numOfElements = x.header.matrixRows * x.header.matrixColumns;

    for(int i = 0; i < numOfElements; ++i) {
      reallocateRegister(REGISTER_X, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
      if(complex) {
        real34Copy(VARIABLE_REAL34_DATA(&xc.matrixElements[i]), REGISTER_REAL34_DATA(REGISTER_X));
      }
      else {
        real34Copy(&x.matrixElements[i], REGISTER_REAL34_DATA(REGISTER_X));
      }
      f();
      elementwiseRemaGetResult(&complex, &x, &xc, i);
    }

    if(complex) {
      convertComplex34MatrixToComplex34MatrixRegister(&xc, REGISTER_X);
      complexMatrixFree(&xc);
    }
    else {
      convertReal34MatrixToReal34MatrixRegister(&x, REGISTER_X);
      realMatrixFree(&x);
    }
  #endif // !TESTSUITE_BUILD
}


void elementwiseRema_UInt16(void (*f)(uint16_t), uint16_t param) {
  #if !defined(TESTSUITE_BUILD)
    real34Matrix_t    x;
    complex34Matrix_t xc;
    bool              complex = false;

    convertReal34MatrixRegisterToReal34Matrix(REGISTER_X, &x);
    const int numOfElements = x.header.matrixRows * x.header.matrixColumns;

    for(int i = 0; i < numOfElements; ++i) {
      reallocateRegister(REGISTER_X, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
      if(complex) {
        real34Copy(VARIABLE_REAL34_DATA(&xc.matrixElements[i]), REGISTER_REAL34_DATA(REGISTER_X));
      }
      else {
        real34Copy(&x.matrixElements[i], REGISTER_REAL34_DATA(REGISTER_X));
      }
      f(param);
      elementwiseRemaGetResult(&complex, &x, &xc, i);
    }

    if(complex) {
      convertComplex34MatrixToComplex34MatrixRegister(&xc, REGISTER_X);
      complexMatrixFree(&xc);
    }
    else {
      convertReal34MatrixToReal34MatrixRegister(&x, REGISTER_X);
      realMatrixFree(&x);
    }
  #endif // !TESTSUITE_BUILD
}


void elementwiseRemaLonI(void (*f)(void)) {
  #if !defined(TESTSUITE_BUILD)
    real34Matrix_t    y;
    complex34Matrix_t yc;
    longInteger_t     x;
    bool              complex = false;

    convertReal34MatrixRegisterToReal34Matrix(REGISTER_Y, &y);
    convertLongIntegerRegisterToLongInteger(REGISTER_X, x);
    const int numOfElements = y.header.matrixRows * y.header.matrixColumns;

    for(int i = 0; i < numOfElements; ++i) {
      reallocateRegister(REGISTER_Y, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
      if(complex) {
        real34Copy(VARIABLE_REAL34_DATA(&yc.matrixElements[i]), REGISTER_REAL34_DATA(REGISTER_Y));
      }
      else {
        real34Copy(&y.matrixElements[i], REGISTER_REAL34_DATA(REGISTER_Y));
      }
      convertLongIntegerToLongIntegerRegister(x, REGISTER_X);
      f();
      elementwiseRemaGetResult(&complex, &y, &yc, i);
    }

    if(complex) {
      convertComplex34MatrixToComplex34MatrixRegister(&yc, REGISTER_X);
      complexMatrixFree(&yc);
    }
    else {
      convertReal34MatrixToReal34MatrixRegister(&y, REGISTER_X);
      realMatrixFree(&y);
    }
    longIntegerFree(x);
  #endif // !TESTSUITE_BUILD
}


void elementwiseRemaReal(void (*f)(void)) {
  #if !defined(TESTSUITE_BUILD)
    real34Matrix_t    y;
    complex34Matrix_t yc;
    real34_t          x;
    bool              complex = false;

    convertReal34MatrixRegisterToReal34Matrix(REGISTER_Y, &y);
    real34Copy(REGISTER_REAL34_DATA(REGISTER_X), &x);
    const int numOfElements = y.header.matrixRows * y.header.matrixColumns;

    for(int i = 0; i < numOfElements; ++i) {
      reallocateRegister(REGISTER_Y, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
      reallocateRegister(REGISTER_X, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
      if(complex) {
        real34Copy(VARIABLE_REAL34_DATA(&yc.matrixElements[i]), REGISTER_REAL34_DATA(REGISTER_Y));
      }
      else {
        real34Copy(&y.matrixElements[i], REGISTER_REAL34_DATA(REGISTER_Y));
      }
      real34Copy(&x, REGISTER_REAL34_DATA(REGISTER_X));
      f();
      elementwiseRemaGetResult(&complex, &y, &yc, i);
    }

    if(complex) {
      convertComplex34MatrixToComplex34MatrixRegister(&yc, REGISTER_X);
      complexMatrixFree(&yc);
    }
    else {
      convertReal34MatrixToReal34MatrixRegister(&y, REGISTER_X);
      realMatrixFree(&y);
    }
  #endif // !TESTSUITE_BUILD
}


void elementwiseRemaShoI(void (*f)(void)) {
  #if !defined(TESTSUITE_BUILD)
    real34Matrix_t    y;
    complex34Matrix_t yc;
    uint64_t          x;
    uint32_t          base;
    int16_t           sign;
    bool              complex = false;

    convertReal34MatrixRegisterToReal34Matrix(REGISTER_Y, &y);
    convertShortIntegerRegisterToUInt64(REGISTER_X, &sign, &x);
    base = getRegisterShortIntegerBase(REGISTER_X);
    const int numOfElements = y.header.matrixRows * y.header.matrixColumns;

    for(int i = 0; i < numOfElements; ++i) {
      reallocateRegister(REGISTER_Y, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
      if(complex) {
        real34Copy(VARIABLE_REAL34_DATA(&yc.matrixElements[i]), REGISTER_REAL34_DATA(REGISTER_Y));
      }
      else {
        real34Copy(&y.matrixElements[i], REGISTER_REAL34_DATA(REGISTER_Y));
      }
      convertUInt64ToShortIntegerRegister(sign, x, base, REGISTER_X);
      f();
      elementwiseRemaGetResult(&complex, &y, &yc, i);
    }

    if(complex) {
      convertComplex34MatrixToComplex34MatrixRegister(&yc, REGISTER_X);
      complexMatrixFree(&yc);
    }
    else {
      convertReal34MatrixToReal34MatrixRegister(&y, REGISTER_X);
      realMatrixFree(&y);
    }
  #endif // !TESTSUITE_BUILD
}


void elementwiseRealRema(void (*f)(void)) {
  #if !defined(TESTSUITE_BUILD)
    real34Matrix_t    x;
    complex34Matrix_t xc;
    real34_t          y;
    bool              complex = false;

    convertReal34MatrixRegisterToReal34Matrix(REGISTER_X, &x);
    real34Copy(REGISTER_REAL34_DATA(REGISTER_Y), &y);
    const int numOfElements = x.header.matrixRows * x.header.matrixColumns;

    for(int i = 0; i < numOfElements; ++i) {
      reallocateRegister(REGISTER_Y, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
      reallocateRegister(REGISTER_X, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
      real34Copy(&y, REGISTER_REAL34_DATA(REGISTER_Y));
      if(complex) {
        real34Copy(VARIABLE_REAL34_DATA(&xc.matrixElements[i]), REGISTER_REAL34_DATA(REGISTER_X));
      }
      else {
        real34Copy(&x.matrixElements[i], REGISTER_REAL34_DATA(REGISTER_X));
      }
      f();
      elementwiseRemaGetResult(&complex, &x, &xc, i);
    }

    if(complex) {
      convertComplex34MatrixToComplex34MatrixRegister(&xc, REGISTER_X);
      complexMatrixFree(&xc);
    }
    else {
      convertReal34MatrixToReal34MatrixRegister(&x, REGISTER_X);
      realMatrixFree(&x);
    }
  #endif // !TESTSUITE_BUILD
}


#if !defined(TESTSUITE_BUILD)
  static void elementwiseCxmaGetResult(complex34Matrix_t *x, int i) {
    if(getRegisterDataType(REGISTER_X) == dtComplex34) {
      complex34Copy(REGISTER_COMPLEX34_DATA(REGISTER_X), &x->matrixElements[i]);
    }
    else {
      if(getRegisterDataType(REGISTER_X) == dtLongInteger) {
        convertLongIntegerRegisterToReal34Register(REGISTER_X, REGISTER_X);
      }
      if(getRegisterDataType(REGISTER_X) == dtReal34) {
        real34Copy(REGISTER_REAL34_DATA(REGISTER_X), VARIABLE_REAL34_DATA(&x->matrixElements[i]));
        real34Copy(const34_0,                        VARIABLE_IMAG34_DATA(&x->matrixElements[i]));
      }
    }
  }
#endif // !TESTSUITE_BUILD


void elementwiseCxma(void (*f)(void)) {
  #if !defined(TESTSUITE_BUILD)
    complex34Matrix_t x;

    convertComplex34MatrixRegisterToComplex34Matrix(REGISTER_X, &x);

    for(int i = 0; i < x.header.matrixRows * x.header.matrixColumns; ++i) {
      reallocateRegister(REGISTER_X, dtComplex34, TO_BLOCKS(COMPLEX34_SIZE_IN_BYTES), amNone);
      complex34Copy(&x.matrixElements[i], REGISTER_COMPLEX34_DATA(REGISTER_X));
      f();
      elementwiseCxmaGetResult(&x, i);
    }

    convertComplex34MatrixToComplex34MatrixRegister(&x, REGISTER_X);

    complexMatrixFree(&x);
  #endif // !TESTSUITE_BUILD
}


void elementwiseCxma_UInt16(void (*f)(uint16_t), uint16_t param) {
  #if !defined(TESTSUITE_BUILD)
    complex34Matrix_t x;

    convertComplex34MatrixRegisterToComplex34Matrix(REGISTER_X, &x);

    for(int i = 0; i < x.header.matrixRows * x.header.matrixColumns; ++i) {
      reallocateRegister(REGISTER_X, dtComplex34, TO_BLOCKS(COMPLEX34_SIZE_IN_BYTES), amNone);
      complex34Copy(&x.matrixElements[i], REGISTER_COMPLEX34_DATA(REGISTER_X));
      f(param);
      elementwiseCxmaGetResult(&x, i);
    }

    convertComplex34MatrixToComplex34MatrixRegister(&x, REGISTER_X);

    complexMatrixFree(&x);
  #endif // !TESTSUITE_BUILD
}


void elementwiseCxmaLonI(void (*f)(void)) {
  #if !defined(TESTSUITE_BUILD)
    complex34Matrix_t y;
    longInteger_t x;

    convertComplex34MatrixRegisterToComplex34Matrix(REGISTER_Y, &y);
    convertLongIntegerRegisterToLongInteger(REGISTER_X, x);

    for(int i = 0; i < y.header.matrixRows * y.header.matrixColumns; ++i) {
      reallocateRegister(REGISTER_Y, dtComplex34, TO_BLOCKS(COMPLEX34_SIZE_IN_BYTES), amNone);
      complex34Copy(&y.matrixElements[i], REGISTER_COMPLEX34_DATA(REGISTER_Y));
      convertLongIntegerToLongIntegerRegister(x, REGISTER_X);
      f();
      elementwiseCxmaGetResult(&y, i);
    }

    convertComplex34MatrixToComplex34MatrixRegister(&y, REGISTER_X);

    longIntegerFree(x);
    complexMatrixFree(&y);
  #endif // !TESTSUITE_BUILD
}


void elementwiseCxmaReal(void (*f)(void)) {
  #if !defined(TESTSUITE_BUILD)
    complex34Matrix_t y;
    real34_t x;

    convertComplex34MatrixRegisterToComplex34Matrix(REGISTER_Y, &y);
    real34Copy(REGISTER_REAL34_DATA(REGISTER_X), &x);

    for(int i = 0; i < y.header.matrixRows * y.header.matrixColumns; ++i) {
      reallocateRegister(REGISTER_Y, dtComplex34, TO_BLOCKS(COMPLEX34_SIZE_IN_BYTES), amNone);
      reallocateRegister(REGISTER_X, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
      complex34Copy(&y.matrixElements[i], REGISTER_COMPLEX34_DATA(REGISTER_Y));
      real34Copy(&x, REGISTER_REAL34_DATA(REGISTER_X));
      f();
      elementwiseCxmaGetResult(&y, i);
    }

    convertComplex34MatrixToComplex34MatrixRegister(&y, REGISTER_X);

    complexMatrixFree(&y);
  #endif // !TESTSUITE_BUILD
}


void elementwiseCxmaShoI(void (*f)(void)) {
  #if !defined(TESTSUITE_BUILD)
    complex34Matrix_t y;
    uint64_t x; uint32_t base; int16_t sign;

    convertComplex34MatrixRegisterToComplex34Matrix(REGISTER_Y, &y);
    convertShortIntegerRegisterToUInt64(REGISTER_X, &sign, &x);
    base = getRegisterShortIntegerBase(REGISTER_X);

    for(int i = 0; i < y.header.matrixRows * y.header.matrixColumns; ++i) {
      reallocateRegister(REGISTER_Y, dtComplex34, TO_BLOCKS(COMPLEX34_SIZE_IN_BYTES), amNone);
      complex34Copy(&y.matrixElements[i], REGISTER_COMPLEX34_DATA(REGISTER_Y));
      convertUInt64ToShortIntegerRegister(sign, x, base, REGISTER_X);
      f();
      elementwiseCxmaGetResult(&y, i);
    }

    convertComplex34MatrixToComplex34MatrixRegister(&y, REGISTER_X);

    complexMatrixFree(&y);
  #endif // !TESTSUITE_BUILD
}


void elementwiseCxmaCplx(void (*f)(void)) {
  #if !defined(TESTSUITE_BUILD)
    complex34Matrix_t y;
    complex34_t x;

    convertComplex34MatrixRegisterToComplex34Matrix(REGISTER_Y, &y);
    complex34Copy(REGISTER_COMPLEX34_DATA(REGISTER_X), &x);

    for(int i = 0; i < y.header.matrixRows * y.header.matrixColumns; ++i) {
      reallocateRegister(REGISTER_Y, dtComplex34, TO_BLOCKS(COMPLEX34_SIZE_IN_BYTES), amNone);
      reallocateRegister(REGISTER_X, dtComplex34, TO_BLOCKS(COMPLEX34_SIZE_IN_BYTES), amNone);
      complex34Copy(&y.matrixElements[i], REGISTER_COMPLEX34_DATA(REGISTER_Y));
      complex34Copy(&x, REGISTER_COMPLEX34_DATA(REGISTER_X));
      f();
      elementwiseCxmaGetResult(&y, i);
    }

    convertComplex34MatrixToComplex34MatrixRegister(&y, REGISTER_X);

    complexMatrixFree(&y);
  #endif // !TESTSUITE_BUILD
}


void elementwiseRealCxma(void (*f)(void)) {
  #if !defined(TESTSUITE_BUILD)
    complex34Matrix_t x;
    real34_t y;

    convertComplex34MatrixRegisterToComplex34Matrix(REGISTER_X, &x);
    real34Copy(REGISTER_REAL34_DATA(REGISTER_Y), &y);

    for(int i = 0; i < x.header.matrixRows * x.header.matrixColumns; ++i) {
      reallocateRegister(REGISTER_Y, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
      reallocateRegister(REGISTER_X, dtComplex34, TO_BLOCKS(COMPLEX34_SIZE_IN_BYTES), amNone);
      real34Copy(&y, REGISTER_REAL34_DATA(REGISTER_Y));
      complex34Copy(&x.matrixElements[i], REGISTER_COMPLEX34_DATA(REGISTER_X));
      f();
      elementwiseCxmaGetResult(&x, i);
    }

    convertComplex34MatrixToComplex34MatrixRegister(&x, REGISTER_X);

    complexMatrixFree(&x);
  #endif // !TESTSUITE_BUILD
}


#if !defined(TESTSUITE_BUILD)
  void callByIndexedMatrix(bool (*real_f)(real34Matrix_t *), bool (*complex_f)(complex34Matrix_t *)) {
    const int16_t i = getIRegisterAsInt(true);
    const int16_t j = getJRegisterAsInt(true);

    if(matrixIndex == INVALID_VARIABLE || !regInRange(matrixIndex)) {
      displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("Cannot execute, destination register is out of range: %d", matrixIndex);
    }
    else if(getRegisterDataType(matrixIndex) == dtReal34Matrix) {
      real34Matrix_t mat;
      convertReal34MatrixRegisterToReal34Matrix(matrixIndex, &mat);
      if(i < 0 || i >= mat.header.matrixRows || j < 0 || j >= mat.header.matrixColumns) {
        displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
        errorMoreInfo("Cannot execute: element (%" PRId16 ", %" PRId16 ") out of range", (int16_t)(i + 1), (int16_t)(j + 1));
      }
      else {
        if(real_f(&mat)) {
          convertReal34MatrixToReal34MatrixRegister(&mat, matrixIndex);
        }
      }
      realMatrixFree(&mat);
    }
    else if(getRegisterDataType(matrixIndex) == dtComplex34Matrix) {
      complex34Matrix_t mat;
      convertComplex34MatrixRegisterToComplex34Matrix(matrixIndex, &mat);
      if(i < 0 || i >= mat.header.matrixRows || j < 0 || j >= mat.header.matrixColumns) {
        displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
        errorMoreInfo("Cannot execute: element (%" PRId16 ", %" PRId16 ") out of range", (int16_t)(i + 1), (int16_t)(j + 1));
      }
      else {
        if(complex_f(&mat)) {
          convertComplex34MatrixToComplex34MatrixRegister(&mat, matrixIndex);
        }
      }
      complexMatrixFree(&mat);
    }
    else {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("Cannot execute: something other than a matrix is indexed %s", getRegisterDataTypeName(REGISTER_X, true, false));
    }
  }
#endif // !TESTSUITE_BUILD
