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

/**
 * \file error.h
 */
#if !defined(ERROR_H)
  #define ERROR_H

  #include "defines.h"
  #include "typeDefinitions.h"
  #include <stdint.h>

  typedef enum {
    ERROR_NONE                             =  0,
    ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN      =  1,
    ERROR_BAD_TIME_OR_DATE_INPUT           =  2,
    ERROR_UNDEFINED_OPCODE                 =  3,
    ERROR_OVERFLOW_PLUS_INF                =  4,
    ERROR_OVERFLOW_MINUS_INF               =  5,
    ERROR_LABEL_NOT_FOUND                  =  6,
    ERROR_FUNCTION_NOT_FOUND               =  7,
    ERROR_OUT_OF_RANGE                     =  8,
    ERROR_INVALID_INTEGER_INPUT            =  9,
    ERROR_INPUT_TOO_LONG                   = 10,
    ERROR_RAM_FULL                         = 11,
    ERROR_STACK_CLASH                      = 12,
    ERROR_OPERATION_UNDEFINED              = 13,
    ERROR_WORD_SIZE_TOO_SMALL              = 14,
    ERROR_TOO_FEW_DATA                     = 15,
    ERROR_INVALID_DISTRIBUTION_PARAM       = 16,
    ERROR_IO                               = 17,
    ERROR_INVALID_CORRUPTED_DATA           = 18,
    ERROR_FLASH_MEMORY_WRITE_PROTECTED     = 19,
    ERROR_NO_ROOT_FOUND                    = 20,
    ERROR_MATRIX_MISMATCH                  = 21,
    ERROR_SINGULAR_MATRIX                  = 22,
    ERROR_FLASH_MEMORY_FULL                = 23,
    ERROR_INVALID_DATA_TYPE_FOR_OP         = 24,
    ERROR_WP34S_COMPAT                     = 25,
    ERROR_ENTER_NEW_NAME                   = 26,
    ERROR_CANNOT_DELETE_PREDEF_ITEM        = 27,
    ERROR_NO_SUMMATION_DATA                = 28,
    ERROR_ITEM_TO_BE_CODED                 = 29,
    ERROR_FUNCTION_TO_BE_CODED             = 30,
    ERROR_INPUT_DATA_TYPE_NOT_MATCHING     = 31,
    ERROR_WRITE_PROTECTED_SYSTEM_FLAG      = 32,
    ERROR_STRING_WOULD_BE_TOO_LONG         = 33,
    ERROR_EMPTY_STRING                     = 34,
    ERROR_NO_BACKUP_DATA                   = 35,
    ERROR_UNDEF_SOURCE_VAR                 = 36,
    ERROR_WRITE_PROTECTED_VAR              = 37,
    ERROR_NO_MATRIX_INDEXED                = 38,
    ERROR_NOT_ENOUGH_MEMORY_FOR_NEW_MATRIX = 39,
    ERROR_NO_ERRORS_CALCULABLE             = 40,
    ERROR_LARGE_DELTA_AND_OPPOSITE_SIGN    = 41,
    ERROR_SOLVER_REACHED_LOCAL_EXTREMUM    = 42,
    ERROR_INITIAL_GUESS_OUT_OF_DOMAIN      = 43,
    ERROR_FUNCTION_VALUES_LOOK_CONSTANT    = 44,
    ERROR_SYNTAX_ERROR_IN_EQUATION         = 45,
    ERROR_EQUATION_TOO_COMPLEX             = 46,
    ERROR_CANNOT_ASSIGN_HERE               = 47,
    ERROR_INVALID_NAME                     = 48,
    ERROR_TOO_MANY_VARIABLES               = 49, // unlikely
    ERROR_NON_PROGRAMMABLE_COMMAND         = 50,
    ERROR_NO_GLOBAL_LABEL                  = 51,
    ERROR_BAD_INPUT                        = 52  // This error is not in ReM and cannot occur (theoretically).
  } errorCode_t;
  #define NUMBER_OF_ERROR_CODES              53

  #define ERROR_MESSAGE_LENGTH 512

  extern errorCode_t     lastErrorCode;
  extern calcRegister_t  errorMessageRegisterLine;
  extern char           *errorMessage;
  extern const char     *errorMessages[NUMBER_OF_ERROR_CODES];

  void fnRaiseError           (uint16_t errorCode);
  void fnErrorMessage         (uint16_t unusedButMandatoryParameter);

  void displayCalcErrorMessage(errorCode_t errorCode, calcRegister_t errMessageRegisterLine, calcRegister_t errRegisterLine);
  void moreInfoOnError        (const char *m1, const char *m2, const char *m3, const char *m4);
  #if (EXTRA_INFO_ON_CALC_ERROR != 1)
    void typeError            (void);
  #endif // (EXTRA_INFO_ON_CALC_ERROR != 1)
#endif // !ERROR_H
