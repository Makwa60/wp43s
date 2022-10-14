// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "logicalOps/setClearFlipBits.h"

#include "debug.h"
#include "error.h"
#include "registers.h"

#include "wp43.h"

void fnCb(uint16_t bit) { // bit from 0=LSB to shortIntegerWordSize-1=MSB
  if(getRegisterDataType(REGISTER_X) == dtShortInteger) {
    if(!saveLastX()) {
      return;
    }

    *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) &= ~((uint64_t)1 << bit);
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "cannot CB %s", getRegisterDataTypeName(REGISTER_X, true, false));
      moreInfoOnError("In function fnCb:", errorMessage, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
  }
}



void fnSb(uint16_t bit) { // bit from 0=LSB to shortIntegerWordSize-1=MSB
  if(getRegisterDataType(REGISTER_X) == dtShortInteger) {
    if(!saveLastX()) {
      return;
    }

    *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) |= (uint64_t)1 << bit;
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "cannot SB %s", getRegisterDataTypeName(REGISTER_X, true, false));
      moreInfoOnError("In function fnSb:", errorMessage, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
  }
}



void fnFb(uint16_t bit) { // bit from 0=LSB to shortIntegerWordSize-1=MSB
  if(getRegisterDataType(REGISTER_X) == dtShortInteger) {
    if(!saveLastX()) {
      return;
    }

    *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) ^= (uint64_t)1 << bit;
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "cannot FB %s", getRegisterDataTypeName(REGISTER_X, true, false));
      moreInfoOnError("In function fnFb:", errorMessage, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
  }
}



void fnBc(uint16_t bit) { // bit from 0=LSB to shortIntegerWordSize-1=MSB
  if(getRegisterDataType(REGISTER_X) == dtShortInteger) {
    thereIsSomethingToUndo = false;
    temporaryInformation = ((*(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) & ((uint64_t)1 << bit)) ? TI_FALSE : TI_TRUE);
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "cannot BC %s", getRegisterDataTypeName(REGISTER_X, true, false));
      moreInfoOnError("In function fnBc:", errorMessage, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
  }
}



void fnBs(uint16_t bit) { // bit from 0=LSB to shortIntegerWordSize-1=MSB
  if(getRegisterDataType(REGISTER_X) == dtShortInteger) {
    thereIsSomethingToUndo = false;
    temporaryInformation = ((*(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) & ((uint64_t)1 << bit)) ? TI_TRUE : TI_FALSE);
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "cannot BS %s", getRegisterDataTypeName(REGISTER_X, true, false));
      moreInfoOnError("In function fnBs:", errorMessage, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
  }
}
