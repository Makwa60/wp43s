// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "solver/sumprod.h"

#include "constantPointers.h"
#include "defines.h"
#include "error.h"
#include "flags.h"
#include "items.h"
#include "mathematics/integerPart.h"
#include "mathematics/iteration.h"
#include "programming/lblGtoXeq.h"
#include "programming/manage.h"
#include "programming/nextStep.h"
#include "realType.h"
#include "registers.h"
#include "solver/solve.h"
#include "stack.h"
#include <stdbool.h>

#include "wp43.h"

#if !defined(TESTSUITE_BUILD)
  static void _programmableSumProd(uint16_t label, bool prod) {
    real34_t counter, result;
    bool     finished = false;

    real34Copy(prod ? const34_1 : const34_0, &result);

    ++currentSolverNestingDepth;
    setSystemFlag(FLAG_SOLVING);

    while(1) {
      fnToReal(NOPARAM);
      if(lastErrorCode != ERROR_NONE) {
        break;
      }
      real34Copy(REGISTER_REAL34_DATA(REGISTER_X), &counter);
      fnIp(NOPARAM);
      fnFillStack(NOPARAM);

      dynamicMenuItem = -1;
      execProgram(label);
      if(lastErrorCode != ERROR_NONE) {
        break;
      }

      fnToReal(NOPARAM);
      if(lastErrorCode != ERROR_NONE) {
        break;
      }
      if(prod) {
        real34Multiply(REGISTER_REAL34_DATA(REGISTER_X), &result, &result);
      }
      else {
        real34Add(REGISTER_REAL34_DATA(REGISTER_X), &result, &result);
      }
      real34Copy(&counter, REGISTER_REAL34_DATA(REGISTER_X));

      if(finished) {
        break;
      }
      fnDse(REGISTER_X);
      finished = (temporaryInformation != TI_TRUE);
    }

    if(lastErrorCode == ERROR_NONE) {
      reallocateRegister(REGISTER_X, dtReal34, TO_BLOCKS(REAL34_SIZE_IN_BYTES), amNone);
      real34Copy(&result, REGISTER_REAL34_DATA(REGISTER_X));
    }

    temporaryInformation = TI_NO_INFO;
    if(programRunStop == PGM_WAITING) {
      programRunStop = PGM_STOPPED;
    }
    adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);

    if((--currentSolverNestingDepth) == 0) {
      clearSystemFlag(FLAG_SOLVING);
    }
  }



  void _checkArgument(uint16_t label, bool prod) {
    if(label >= FIRST_LABEL && label <= LAST_LABEL) {
      _programmableSumProd(label, prod);
    }
    else if(label >= REGISTER_X && label <= REGISTER_T) {
      // Interactive mode
      char buf[4];
      switch(label) {
        case REGISTER_X: {
          buf[0] = 'X';
          break;
        }
        case REGISTER_Y: {
          buf[0] = 'Y';
          break;
        }
        case REGISTER_Z: {
          buf[0] = 'Z';
          break;
        }
        case REGISTER_T: {
          buf[0] = 'T';
          break;
        }
        default: { /* unlikely */
          buf[0] = 0;
        }
      }
      buf[1] = 0;
      label = findNamedLabel(buf);
      if(label == INVALID_VARIABLE) {
        displayCalcErrorMessage(ERROR_LABEL_NOT_FOUND, ERR_REGISTER_LINE, REGISTER_X);
        #if (EXTRA_INFO_ON_CALC_ERROR == 1)
          sprintf(errorMessage, "string '%s' is not a named label", buf);
          moreInfoOnError("In function fnPgmSlv:", errorMessage, NULL, NULL);
        #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
      }
      else {
        _programmableSumProd(label, prod);
      }
    }
    else {
      displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
      #if (EXTRA_INFO_ON_CALC_ERROR == 1)
        sprintf(errorMessage, "unexpected parameter %u", label);
        moreInfoOnError("In function fnPgmSlv:", errorMessage, NULL, NULL);
      #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    }
  }
#endif // !TESTSUITE_BUILD



void fnProgrammableSum(uint16_t label) {
  #if !defined(TESTSUITE_BUILD)
    _checkArgument(label, false);
  #endif // !TESTSUITE_BUILD
}



void fnProgrammableProduct(uint16_t label) {
  #if !defined(TESTSUITE_BUILD)
    _checkArgument(label, true);
  #endif // !TESTSUITE_BUILD
}
