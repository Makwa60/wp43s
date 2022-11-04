// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "logicalOps/rotateBits.h"

#include "debug.h"
#include "error.h"
#include "flags.h"
#include "registers.h"
#include "registerValueConversions.h"
#include "stack.h"

#include "wp43.h"

void fnAsr(uint16_t numberOfShifts) {
  int32_t i;
  uint64_t sign;

  if(getRegisterDataType(REGISTER_X) == dtShortInteger) {
    if(!saveLastX()) {
      return;
    }

    sign = *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) & shortIntegerSignBit;
    for(i=1; i<=numberOfShifts; i++) {
      if(i == numberOfShifts) {
        if(*(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) & 1) {
          setSystemFlag(FLAG_CARRY);
        }
        else {
          clearSystemFlag(FLAG_CARRY);
        }
      }

      *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) = (*(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) >> 1) | sign;
    }
    *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) &= shortIntegerMask;
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot ASR %s", getRegisterDataTypeName(REGISTER_X, true, false));
  }
}



void fnSl(uint16_t numberOfShifts) {
  int32_t i;

  if(getRegisterDataType(REGISTER_X) == dtShortInteger) {
    if(!saveLastX()) {
      return;
    }

    for(i=1; i<=numberOfShifts; i++) {
      if(i == numberOfShifts) {
        if(*(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) & shortIntegerSignBit) {
          setSystemFlag(FLAG_CARRY);
        }
        else {
          clearSystemFlag(FLAG_CARRY);
        }
      }

      *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) <<= 1;
    }
    *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) &= shortIntegerMask;
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot SL %s", getRegisterDataTypeName(REGISTER_X, true, false));
  }
}



void fnSr(uint16_t numberOfShifts) {
  int32_t i;

  if(getRegisterDataType(REGISTER_X) == dtShortInteger) {
    if(!saveLastX()) {
      return;
    }

    for(i=1; i<=numberOfShifts; i++) {
      if(i == numberOfShifts) {
        if(*(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) & 1) {
          setSystemFlag(FLAG_CARRY);
        }
        else {
          clearSystemFlag(FLAG_CARRY);
        }
      }

      *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) >>= 1;
    }
    *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) &= shortIntegerMask;
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot SR %s", getRegisterDataTypeName(REGISTER_X, true, false));
  }
}



void fnRl(uint16_t numberOfShifts) {
  int32_t i;
  uint64_t sign;

  if(getRegisterDataType(REGISTER_X) == dtShortInteger) {
    if(!saveLastX()) {
      return;
    }

    for(i=1; i<=numberOfShifts; i++) {
      sign = (*(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) & shortIntegerSignBit) >> (shortIntegerWordSize - 1);
      if(i == numberOfShifts) {
        if(sign) {
          setSystemFlag(FLAG_CARRY);
        }
        else {
          clearSystemFlag(FLAG_CARRY);
        }
      }

      *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) = (*(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) << 1) | sign;
    }
    *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) &= shortIntegerMask;
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot RL %s", getRegisterDataTypeName(REGISTER_X, true, false));
  }
}



void fnRr(uint16_t numberOfShifts) {
  int32_t i;

  if(getRegisterDataType(REGISTER_X) == dtShortInteger) {
    if(!saveLastX()) {
      return;
    }

    for(i=1; i<=numberOfShifts; i++) {
      if(i == numberOfShifts) {
        if(*(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) & 1) {
          setSystemFlag(FLAG_CARRY);
        }
        else {
          clearSystemFlag(FLAG_CARRY);
        }
      }

      *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) = (*(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) >> 1) | ((*(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) & 1) << (shortIntegerWordSize - 1));
    }
    *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) &= shortIntegerMask;
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot RR %s", getRegisterDataTypeName(REGISTER_X, true, false));
  }
}



void fnRlc(uint16_t numberOfShifts) {
  int32_t i;
  uint64_t sign, carry;

  if(getRegisterDataType(REGISTER_X) == dtShortInteger) {
    if(!saveLastX()) {
      return;
    }

    carry = getSystemFlag(FLAG_CARRY);
    for(i=1; i<=numberOfShifts; i++) {
      sign = (*(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) & shortIntegerSignBit) >> (shortIntegerWordSize - 1);
      *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) = (*(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) << 1) | carry;
      carry = sign;
    }

    if(carry) {
      setSystemFlag(FLAG_CARRY);
    }
    else {
      clearSystemFlag(FLAG_CARRY);
    }
    *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) &= shortIntegerMask;
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot RLC %s", getRegisterDataTypeName(REGISTER_X, true, false));
  }
}



void fnRrc(uint16_t numberOfShifts) {
  int32_t i;
  uint64_t lsb, carry;

  if(getRegisterDataType(REGISTER_X) == dtShortInteger) {
    if(!saveLastX()) {
      return;
    }

    carry = getSystemFlag(FLAG_CARRY);
    for(i=1; i<=numberOfShifts; i++) {
      lsb = *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) & 1;
      *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) = (*(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) >> 1) | (carry << (shortIntegerWordSize - 1));
      carry = lsb;
    }

    if(carry) {
      setSystemFlag(FLAG_CARRY);
    }
    else {
      clearSystemFlag(FLAG_CARRY);
    }
    *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) &= shortIntegerMask;
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot RRC %s", getRegisterDataTypeName(REGISTER_X, true, false));
  }
}



void fnLj(uint16_t numberOfShifts) {
  int32_t count;
  longInteger_t regX;

  if(getRegisterDataType(REGISTER_X) == dtShortInteger) {
    if(!saveLastX()) {
      return;
    }

    count = 0;
    while((*(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) & shortIntegerSignBit) == 0) {
      count++;
      *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) <<= 1;
    }

    longIntegerInit(regX);
    intToLongInteger(count, regX);

    setSystemFlag(FLAG_ASLIFT);
    liftStack();
    convertLongIntegerToLongIntegerRegister(regX, REGISTER_X);
    longIntegerFree(regX);
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot LJ %s", getRegisterDataTypeName(REGISTER_X, true, false));
  }
}



void fnRj(uint16_t numberOfShifts) {
  int32_t count;
  longInteger_t regX;

  if(getRegisterDataType(REGISTER_X) == dtShortInteger) {
    if(!saveLastX()) {
      return;
    }

    count = 0;
    while((*(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) & 1) == 0) {
      count++;
      *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) >>= 1;
    }

    longIntegerInit(regX);
    intToLongInteger(count, regX);

    setSystemFlag(FLAG_ASLIFT);
    liftStack();
    convertLongIntegerToLongIntegerRegister(regX, REGISTER_X);
    longIntegerFree(regX);
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot RJ %s", getRegisterDataTypeName(REGISTER_X, true, false));
  }
}



void fnMirror(uint16_t unusedButMandatoryParameter) {
  if(getRegisterDataType(REGISTER_X) == dtShortInteger) {
    uint64_t x, r=0;

    if(!saveLastX()) {
      return;
    }

    x = *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X));
    if(x != 0) {
      for(uint32_t i=0; i<shortIntegerWordSize; i++) {
        if(x & (1LL << i)) {
          r |= 1LL << (shortIntegerWordSize-i-1);
        }
      }
    }

    *(REGISTER_SHORT_INTEGER_DATA(REGISTER_X)) = r;
  }

  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("the input type %s is not allowed for MIRROR!", getDataTypeName(getRegisterDataType(REGISTER_X), false, false));
  }
}
