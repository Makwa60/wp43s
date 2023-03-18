// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "stringFuncs.h"

#include "charString.h"
#include "constantPointers.h"
#include "debug.h"
#include "display.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/matrix.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"
#include "stack.h"
#include <stdbool.h>

#include "wp43.h"

void fnAlphaLeng(uint16_t regist) {
  longInteger_t stringSize;

  if(getRegisterDataType(regist) != dtString) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot get the " STD_alpha "LENG? from %s", getRegisterDataTypeName(regist, true, false));
    return;
  }

  longIntegerInit(stringSize);
  uIntToLongInteger(stringGlyphLength(REGISTER_STRING_DATA(regist)), stringSize);

  liftStack();

  convertLongIntegerToLongIntegerRegister(stringSize, REGISTER_X);
  longIntegerFree(stringSize);
}



void fnAlphaToX(uint16_t regist) {
  unsigned char char1, char2;
  longInteger_t lgInt;
  liftStack();
 
  if(getRegisterDataType(regist) != dtString) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot use " STD_alpha STD_RIGHT_ARROW "x on %s", getRegisterDataTypeName(regist, true, false));
    return;
  }

//  if(stringByteLength(REGISTER_STRING_DATA(regist)) == 0) {  ***DL***
//    displayCalcErrorMessage(ERROR_EMPTY_STRING, ERR_REGISTER_LINE, REGISTER_X);  ***DL***
//    errorMoreInfo("cannot use " STD_alpha STD_RIGHT_ARROW "x on an empty string");  ***DL***
//    return;  ***DL***
//  }  ***DL*** 

  liftStack();

  longIntegerInit(lgInt);
  intToLongInteger(0, lgInt);

  if(stringByteLength(REGISTER_STRING_DATA(regist)) != 0) {
    char1 = *(REGISTER_STRING_DATA(regist));
    if(char1 & 0x80) {
      char2 = *(REGISTER_STRING_DATA(regist) + 1);
    }
    intToLongInteger(char1 & 0x7f, lgInt);
    if(char1 & 0x80) {
      longIntegerMultiplyUInt(lgInt, 256, lgInt);
      longIntegerAddUInt(lgInt, char2, lgInt);
    }
	convertLongIntegerToShortIntegerRegister(lgInt, 16, REGISTER_X);
  } 
  else {
    convertLongIntegerToLongIntegerRegister(lgInt, REGISTER_X);
  }

  longIntegerFree(lgInt);

  if(!getSystemFlag(FLAG_ASLIFT) || regist != getStackTop()) {
    if(REGISTER_X <= regist && regist < getStackTop()) {
      regist++;
    }
    xcopy(REGISTER_STRING_DATA(regist), REGISTER_STRING_DATA(regist) + (char1 & 0x80 ? 2 : 1), stringByteLength(REGISTER_STRING_DATA(regist) + (char1 & 0x80 ? 2 : 1)) + 1);
  }
}



static void _readDestinationRegister(uint16_t regist) {
  switch(getRegisterDataType(regist)) {
    case dtLongInteger: {
      longIntegerRegisterToDisplayString(regist, tmpString, TMP_STR_LENGTH, SCREEN_WIDTH, 50, STD_SPACE_PUNCTUATION);
      break;
    }

    case dtTime: {
      timeToDisplayString(regist, tmpString, false);
      break;
    }

    case dtDate: {
      dateToDisplayString(regist, tmpString);
      break;
    }

    case dtString: {
      xcopy(tmpString, REGISTER_STRING_DATA(regist), stringByteLength(REGISTER_STRING_DATA(regist)) + 1);
      break;
    }

    case dtReal34Matrix: {
      real34MatrixToDisplayString(regist, tmpString);
      break;
    }

    case dtComplex34Matrix: {
      complex34MatrixToDisplayString(regist, tmpString);
      break;
    }

    case dtShortInteger: {
      shortIntegerToDisplayString(regist, tmpString, false);
      break;
    }

    case dtReal34: {
      real34ToDisplayString(REGISTER_REAL34_DATA(regist), getRegisterAngularMode(regist), tmpString, &standardFont, SCREEN_WIDTH, NUMBER_OF_DISPLAY_DIGITS, false, STD_SPACE_PUNCTUATION, true);
      break;
    }

    case dtComplex34: {
      complex34ToDisplayString(REGISTER_COMPLEX34_DATA(regist), tmpString, &numericFont, SCREEN_WIDTH, NUMBER_OF_DISPLAY_DIGITS, false, STD_SPACE_PUNCTUATION, true);
      break;
    }

    default: {
      tmpString[0] = 0;
      break;
    }
  }
}

static void _doXToAlpha(uint16_t regist) {
  longInteger_t lgInt;
  unsigned char char1, char2;

  longIntegerInit(lgInt);
  switch(getRegisterDataType(REGISTER_X)) {
    case dtLongInteger: {
      convertLongIntegerRegisterToLongInteger(REGISTER_X, lgInt);
      break;
    }

    case dtReal34: {
      if(real34CompareAbsGreaterThan(REGISTER_REAL34_DATA(REGISTER_X), const34_1e6)) {
        uIntToLongInteger(1000000, lgInt);
      }
      else {
        convertReal34ToLongInteger(REGISTER_REAL34_DATA(REGISTER_X), lgInt, DEC_ROUND_DOWN);
      }
      break;
    }

    case dtShortInteger: {
      convertShortIntegerRegisterToLongInteger(REGISTER_X, lgInt);
      break;
    }

    case dtReal34Matrix: {
      convertShortIntegerRegisterToLongInteger(REGISTER_X, lgInt);
      break;
    }

    default: {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("cannot x" STD_RIGHT_ARROW STD_alpha " when X is %s", getRegisterDataTypeName(REGISTER_X, true, false));
      return;
    }
  }

  longIntegerSetPositiveSign(lgInt);
  //if(longIntegerCompareUInt(lgInt, standardFont.glyphs[standardFont.numberOfGlyphs - 1].charCode & 0x7fff) > 0) {
  if(longIntegerCompareUInt(lgInt, 0x8000) >= 0) {
    displayCalcErrorMessage(ERROR_OUT_OF_RANGE, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("for x" STD_RIGHT_ARROW STD_alpha ", X must be < 32768. Here X = %" PRIu32, (uint32_t)lgInt->_mp_d[0]); // OK for 32 and 64 bit limbs
    return;
  }

  if(longIntegerIsZero(lgInt)) {
    char1 = 0;
    char2 = 0;
  }
  else if(lgInt->_mp_d[0] < 0x0080) {      // OK for 32 and 64 bit limbs
    char1 = lgInt->_mp_d[0];               // OK for 32 and 64 bit limbs
    char2 = 0;
  }
  else {
    char1 = (lgInt->_mp_d[0] >> 8) | 0x80; // OK for 32 and 64 bit limbs
    char2 = lgInt->_mp_d[0] & 0x00ff;      // OK for 32 and 64 bit limbs
  }

  longIntegerFree(lgInt);

  _readDestinationRegister(regist);

  if(stringGlyphLength(tmpString) >= MAX_NUMBER_OF_GLYPHS_IN_STRING) {
    displayCalcErrorMessage(ERROR_STRING_WOULD_BE_TOO_LONG, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("the resulting string would be %d characters long. Maximum is %d",
        stringGlyphLength(tmpString) + 1, MAX_NUMBER_OF_GLYPHS_IN_STRING);
  }
  else {
    int l = stringByteLength(tmpString);
    tmpString[l]       = char1;
    tmpString[l + 1]   = char2;
    if(char2) {
      tmpString[l + 2] = 0;
      ++l;
    }
    ++l;

    reallocateRegister(regist, dtString, l + 1, amNone);
    xcopy(REGISTER_STRING_DATA(regist), tmpString, l + 1);
  }

}

void fnXToAlpha(uint16_t regist) {
  if(!saveLastX()) {
    return;
  }

  switch(getRegisterDataType(REGISTER_X)) {
    case dtLongInteger:
    case dtReal34:
    case dtShortInteger: {
      _doXToAlpha(regist);
      fnDrop(0);
      return;
    }

    case dtString: {
      _readDestinationRegister(regist);
      if(stringGlyphLength(tmpString) + stringGlyphLength(REGISTER_STRING_DATA(REGISTER_X)) > MAX_NUMBER_OF_GLYPHS_IN_STRING) {
        displayCalcErrorMessage(ERROR_STRING_WOULD_BE_TOO_LONG, ERR_REGISTER_LINE, REGISTER_X);
        errorMoreInfo("the resulting string would be %d (%d + %d) characters long. Maximum is %d",
            stringGlyphLength(tmpString) + stringGlyphLength(REGISTER_STRING_DATA(REGISTER_X)),
            stringGlyphLength(tmpString),
            stringGlyphLength(REGISTER_STRING_DATA(REGISTER_X)), MAX_NUMBER_OF_GLYPHS_IN_STRING);
      }
      else {
        int l = stringByteLength(tmpString);
        xcopy(tmpString + l, REGISTER_STRING_DATA(REGISTER_X), stringByteLength(REGISTER_STRING_DATA(REGISTER_X)) + 1);
        l = stringByteLength(tmpString);
        reallocateRegister(regist, dtString, l + 1, amNone);
        xcopy(REGISTER_STRING_DATA(regist), tmpString, l + 1);
        fnDrop(0);
      }
      return;
    }

    case dtReal34Matrix: {
      elementwiseRema_UInt16(_doXToAlpha, regist);
      fnDrop(0);
      return;
    }

    default: {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("cannot x" STD_RIGHT_ARROW STD_alpha " when X is %s", getRegisterDataTypeName(REGISTER_X, true, false));
      return;
    }
  }

}



void fnAlphaPos(uint16_t regist) {
  longInteger_t  lgInt;
  char          *ptrTarget, *ptrRegist;
  int16_t        lgTarget, lgRegist, i, j;
  bool           found;

  if(getRegisterDataType(regist) != dtString) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot use " STD_alpha "POS? on %s (reg %" PRIu16 ")", getRegisterDataTypeName(regist, true, false), regist);
    return;
  }

  if(getRegisterDataType(REGISTER_X) != dtString) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot use " STD_alpha "POS? on %s (reg X)", getRegisterDataTypeName(regist, true, false));
    return;
  }

//  if(stringGlyphLength(REGISTER_STRING_DATA(regist)) == 0 || stringGlyphLength(REGISTER_STRING_DATA(REGISTER_X)) == 0) {  ***DL***
//    displayCalcErrorMessage(ERROR_EMPTY_STRING, ERR_REGISTER_LINE, REGISTER_X);  ***DL***
//    errorMoreInfo("cannot use " STD_alpha "POS? on or with an empty string");  ***DL***
//    return;  ***DL***
//  }  ***DL***

  if(!saveLastX()) {
    return;
  }

  longIntegerInit(lgInt);
  intToLongInteger(-1, lgInt);
  
  if(stringGlyphLength(REGISTER_STRING_DATA(regist)) != 0 && stringGlyphLength(REGISTER_STRING_DATA(REGISTER_X)) != 0) {
    ptrTarget = REGISTER_STRING_DATA(REGISTER_X);
    ptrRegist = REGISTER_STRING_DATA(regist);
    lgTarget = stringByteLength(ptrTarget);
    lgRegist = stringByteLength(ptrRegist);

    for(i=0; i<=lgRegist-lgTarget; i++) {
      found = true;
      for(j=0; j<lgTarget; j++) {
        if(*(ptrRegist+i+j) != *(ptrTarget+j)) {
          found = false;
          break;
        }
      }
      if(found) {
        intToLongInteger(i, lgInt);
        break;
      }
	}
  }

  liftStack();
  convertLongIntegerToLongIntegerRegister(lgInt, REGISTER_X);
  longIntegerFree(lgInt);
}



void fnAlphaRR(uint16_t regist) {
  longInteger_t lgInt;
  real_t real, mod;
  int16_t stringGlyphLen, steps, glyphPointer;
  char *ptr;

  if(getRegisterDataType(regist) != dtString) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot use " STD_alpha "RR on %s", getRegisterDataTypeName(regist, true, false));
    return;
  }

  stringGlyphLen = stringGlyphLength(REGISTER_STRING_DATA(regist));
  if(stringGlyphLen == 0) {
    //displayCalcErrorMessage(ERROR_EMPTY_STRING, ERR_REGISTER_LINE, REGISTER_X);  ***DL***
    //errorMoreInfo("cannot use " STD_alpha "RR on an empty string");  ***DL***
    return;
  }

  longIntegerInit(lgInt);
  switch(getRegisterDataType(REGISTER_X)) {
    case dtLongInteger: {
      convertLongIntegerRegisterToLongInteger(REGISTER_X, lgInt);
      break;
    }

    case dtReal34: {
      if(stringGlyphLen == 0) {
        lgInt->_mp_size = 0; // lgInt = 0
      }
      else {
        real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &real);
        realSetPositiveSign(&real);
        int32ToReal(stringGlyphLen, &mod);
        WP34S_Mod(&real, &mod, &real, &ctxtReal39);
        convertRealToLongInteger(&real, lgInt, DEC_ROUND_DOWN);
      }
      break;
    }

    case dtShortInteger: {
      convertShortIntegerRegisterToLongInteger(REGISTER_X, lgInt);
      break;
    }

    default: {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("cannot " STD_alpha "RR when X is %s", getRegisterDataTypeName(REGISTER_X, true, false));
      return;
    }
  }

  longIntegerSetPositiveSign(lgInt);
  steps = longIntegerModuloUInt(lgInt, stringGlyphLen);
  longIntegerFree(lgInt);

  if(!saveLastX()) {
    return;
  }

  if(steps > 0) {
    for(glyphPointer=0, steps=stringGlyphLen-steps; steps > 0; steps--) {
      glyphPointer = stringNextGlyph(REGISTER_STRING_DATA(regist), glyphPointer);
    }

    ptr = REGISTER_STRING_DATA(regist) + glyphPointer;
    steps = stringByteLength(ptr) + 1;
    xcopy(tmpString, ptr, steps);
    *(ptr) = 0;
    xcopy(tmpString + stringByteLength(tmpString), REGISTER_STRING_DATA(regist), stringByteLength(REGISTER_STRING_DATA(regist)) + 1);
    xcopy(REGISTER_STRING_DATA(regist), tmpString, stringByteLength(tmpString) + 1);
  }
}



void fnAlphaRL(uint16_t regist) {
  longInteger_t lgInt;
  real_t real, mod;
  int16_t stringGlyphLen, steps, glyphPointer;
  char *ptr;

  if(getRegisterDataType(regist) != dtString) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot use " STD_alpha "RL on %s", getRegisterDataTypeName(regist, true, false));
    return;
  }

  stringGlyphLen = stringGlyphLength(REGISTER_STRING_DATA(regist));
  if(stringGlyphLen == 0) {
    //displayCalcErrorMessage(ERROR_EMPTY_STRING, ERR_REGISTER_LINE, REGISTER_X);  ***DL***
    //errorMoreInfo("cannot use " STD_alpha "RL on an empty string");  ***DL***
    return;
  }

  longIntegerInit(lgInt);
  switch(getRegisterDataType(REGISTER_X)) {
    case dtLongInteger: {
      convertLongIntegerRegisterToLongInteger(REGISTER_X, lgInt);
      break;
    }

    case dtReal34: {
      if(stringGlyphLen == 0) {
        lgInt->_mp_size = 0; // lgInt = 0
      }
      else {
        real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &real);
        realSetPositiveSign(&real);
        int32ToReal(stringGlyphLen, &mod);
        WP34S_Mod(&real, &mod, &real, &ctxtReal39);
        convertRealToLongInteger(&real, lgInt, DEC_ROUND_DOWN);
      }
      break;
    }

    case dtShortInteger: {
      convertShortIntegerRegisterToLongInteger(REGISTER_X, lgInt);
      break;
    }

    default: {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("cannot " STD_alpha "RL when X is %s", getRegisterDataTypeName(REGISTER_X, true, false));
      return;
    }
  }

  longIntegerSetPositiveSign(lgInt);
  steps = longIntegerModuloUInt(lgInt, stringGlyphLen);
  longIntegerFree(lgInt);

  if(!saveLastX()) {
    return;
  }

  if(steps > 0) {
    for(glyphPointer=0; steps > 0; steps--) {
      glyphPointer = stringNextGlyph(REGISTER_STRING_DATA(regist), glyphPointer);
    }

    ptr = REGISTER_STRING_DATA(regist) + glyphPointer;
    steps = stringByteLength(ptr) + 1;
    xcopy(tmpString, ptr, steps);
    *(ptr) = 0;
    xcopy(tmpString + stringByteLength(tmpString), REGISTER_STRING_DATA(regist), stringByteLength(REGISTER_STRING_DATA(regist)) + 1);
    xcopy(REGISTER_STRING_DATA(regist), tmpString, stringByteLength(tmpString) + 1);
  }
}



void fnAlphaSR(uint16_t regist) {
  longInteger_t lgInt;
  int16_t stringGlyphLen, steps, glyphPointer;

  if(getRegisterDataType(regist) != dtString) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot use " STD_alpha "SR on %s", getRegisterDataTypeName(regist, true, false));
    return;
  }

  stringGlyphLen = stringGlyphLength(REGISTER_STRING_DATA(regist));
  if(stringGlyphLen == 0) {
    //displayCalcErrorMessage(ERROR_EMPTY_STRING, ERR_REGISTER_LINE, REGISTER_X);  ***DL***
    //errorMoreInfo("cannot use " STD_alpha "SR on an empty string");  ***DL***
    return;
  }

  longIntegerInit(lgInt);
  switch(getRegisterDataType(REGISTER_X)) {
    case dtLongInteger: {
      convertLongIntegerRegisterToLongInteger(REGISTER_X, lgInt);
      break;
    }

    case dtReal34: {
      if(real34CompareAbsGreaterThan(REGISTER_REAL34_DATA(REGISTER_X), const34_1e6)) {
        uIntToLongInteger(1000000, lgInt);
      }
      else {
        convertReal34ToLongInteger(REGISTER_REAL34_DATA(REGISTER_X), lgInt, DEC_ROUND_DOWN);
      }
      break;
    }

    case dtShortInteger: {
      convertShortIntegerRegisterToLongInteger(REGISTER_X, lgInt);
      break;
    }

    default: {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("cannot " STD_alpha "SR when X is %s", getRegisterDataTypeName(REGISTER_X, true, false));
      return;
    }
  }

  longIntegerSetPositiveSign(lgInt);
  if(longIntegerCompareInt(lgInt, stringGlyphLen) >= 0) {
    intToLongInteger(stringGlyphLen, lgInt);
  }

  steps = longIntegerToInt(lgInt);
  longIntegerFree(lgInt);

  if(!saveLastX()) {
    return;
  }

  if(steps > 0) {
    for(glyphPointer=0, steps=stringGlyphLen-steps; steps > 0; steps--) {
      glyphPointer = stringNextGlyph(REGISTER_STRING_DATA(regist), glyphPointer);
    }

    *(REGISTER_STRING_DATA(regist) + glyphPointer) = 0;
  }
}



void fnAlphaSL(uint16_t regist) {
  longInteger_t lgInt;
  int16_t stringGlyphLen, steps, glyphPointer;
  char *ptr;

  if(getRegisterDataType(regist) != dtString) {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("cannot use " STD_alpha "SL on %s", getRegisterDataTypeName(regist, true, false));
    return;
  }

  ptr = REGISTER_STRING_DATA(regist);
  stringGlyphLen = stringGlyphLength(ptr);
  if(stringGlyphLen == 0) {
    //displayCalcErrorMessage(ERROR_EMPTY_STRING, ERR_REGISTER_LINE, REGISTER_X);  ***DL***
    //errorMoreInfo("cannot use " STD_alpha "SL on an empty string");  ***DL***
    return;
  }

  longIntegerInit(lgInt);
  switch(getRegisterDataType(REGISTER_X)) {
    case dtLongInteger: {
      convertLongIntegerRegisterToLongInteger(REGISTER_X, lgInt);
      break;
    }

    case dtReal34: {
      if(real34CompareAbsGreaterThan(REGISTER_REAL34_DATA(REGISTER_X), const34_1e6)) {
        uIntToLongInteger(1000000, lgInt);
      }
      else {
        convertReal34ToLongInteger(REGISTER_REAL34_DATA(REGISTER_X), lgInt, DEC_ROUND_DOWN);
      }
      break;
    }

    case dtShortInteger: {
      convertShortIntegerRegisterToLongInteger(REGISTER_X, lgInt);
      break;
    }

    default: {
      displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("cannot " STD_alpha "SL when X is %s", getRegisterDataTypeName(REGISTER_X, true, false));
      return;
    }
  }

  longIntegerSetPositiveSign(lgInt);
  if(longIntegerCompareInt(lgInt, stringGlyphLen) >= 0) {
    intToLongInteger(stringGlyphLen, lgInt);
  }

  steps = longIntegerToInt(lgInt);
  longIntegerFree(lgInt);

  if(!saveLastX()) {
    return;
  }

  if(steps > 0) {
    for(glyphPointer=0; steps > 0; steps--) {
      glyphPointer = stringNextGlyph(ptr, glyphPointer);
    }

   xcopy(ptr, ptr + glyphPointer, stringByteLength(ptr + glyphPointer) + 1);
  }
}



void fnIsString(uint16_t unusedButMandatoryParameter) {
  if(getRegisterDataType(REGISTER_X) == dtString) {
    temporaryInformation = TI_TRUE;
  }
  else {
    temporaryInformation = TI_FALSE;
  }
}
