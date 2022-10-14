// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "logicalOps/mask.h"

#include "error.h"
#include "registers.h"
#include "stack.h"

#include "wp43.h"

void fnMaskl(uint16_t numberOfBits) {
  if(numberOfBits > shortIntegerWordSize) {
    displayCalcErrorMessage(ERROR_WORD_SIZE_TOO_SMALL, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "cannot calculate MASKL(%d) word size is %d", numberOfBits, shortIntegerWordSize);
      moreInfoOnError("In function fnMaskl:", errorMessage, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
  }
  else {
    uint64_t mask;
    liftStack();

    if(numberOfBits == 0) {
      mask = 0;
    }
    else {
      mask = (((1ULL << numberOfBits) - 1) & shortIntegerMask) << (shortIntegerWordSize - numberOfBits);
    }

    reallocateRegister(REGISTER_X, dtShortInteger, SHORT_INTEGER_SIZE, 2);
    *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) = mask;
  }
}



void fnMaskr(uint16_t numberOfBits) {
  if(numberOfBits > shortIntegerWordSize) {
    displayCalcErrorMessage(ERROR_WORD_SIZE_TOO_SMALL, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "cannot calculate MASKR(%d) word size is %d", numberOfBits, shortIntegerWordSize);
      moreInfoOnError("In function fnMaskr:", errorMessage, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
  }
  else {
    uint64_t mask;
    liftStack();

    if(numberOfBits == 0) {
      mask = 0;
    }
    else {
      mask = (1ULL << numberOfBits) - 1;
    }

    reallocateRegister(REGISTER_X, dtShortInteger, SHORT_INTEGER_SIZE, 2);
    *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) = mask;
  }
}
