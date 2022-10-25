// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "config.h"

#include "apps/bugScreen.h"
#include "apps/flagBrowser.h"
#include "apps/fontBrowser.h"
#include "apps/registerBrowser.h"
#include "apps/timerApp.h"
#include "calcMode.h"
#include "charString.h"
#include "constantPointers.h"
#include "core/memory.h"
#include "core/timer.h"
#include "debug.h"
#include "display.h"
#include "error.h"
#include "fonts.h"
#include "flags.h"
#include "hal/debug.h"
#include "hal/io.h"
#include "items.h"
#include "matrix.h"
#include "plotstat.h"
#include "programming/flash.h"
#include "programming/manage.h"
#include "programming/programmableMenu.h"
#include "recall.h"
#include "registers.h"
#include "registerValueConversions.h"
#include "solver/equation.h"
#include "stack.h"
#include "stats.h"
#include "ui/bufferize.h"
#include "ui/keyboard.h"
#include "ui/screen.h"
#include "ui/tam.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "wp43.h"

void fnConfigChina(uint16_t unusedButMandatoryParameter) {
  setSystemFlag(FLAG_DECIMP);
  groupingGap = 4;
  setSystemFlag(FLAG_TDM24); // time format = 24H
  clearSystemFlag(FLAG_DMY); // date format
  clearSystemFlag(FLAG_MDY); // date format
  setSystemFlag(FLAG_YMD);   // date format
  firstGregorianDay = 2433191 /* 1 Oct 1949 */;  // JDN of the first day in the Gregorian calendar
}



void fnConfigEurope(uint16_t unusedButMandatoryParameter) {
  clearSystemFlag(FLAG_DECIMP);
  groupingGap = 3;
  setSystemFlag(FLAG_TDM24); // time format = 24H
  clearSystemFlag(FLAG_MDY); // date format
  clearSystemFlag(FLAG_YMD); // date format
  setSystemFlag(FLAG_DMY);   // date format
  firstGregorianDay = 2299161 /* 15 Oct 1582 */; // JDN of the first day in the Gregorian calendar
}



void fnConfigIndia(uint16_t unusedButMandatoryParameter) {
  setSystemFlag(FLAG_DECIMP);
  groupingGap = 3;
  setSystemFlag(FLAG_TDM24); // time format = 24H
  clearSystemFlag(FLAG_MDY); // date format
  clearSystemFlag(FLAG_YMD); // date format
  setSystemFlag(FLAG_DMY);   // date format
  firstGregorianDay = 2361222 /* 14 Sept 1752 */; // JDN of the first day in the Gregorian calendar
}



void fnConfigJapan(uint16_t unusedButMandatoryParameter) {
  setSystemFlag(FLAG_DECIMP);
  groupingGap = 3;
  setSystemFlag(FLAG_TDM24); // time format = 24H
  clearSystemFlag(FLAG_MDY); // date format
  clearSystemFlag(FLAG_DMY); // date format
  setSystemFlag(FLAG_YMD);   // date format
  firstGregorianDay = 2405160 /* 1 Jan 1873 */; // JDN of the first day in the Gregorian calendar
}



void fnConfigUk(uint16_t unusedButMandatoryParameter) {
  setSystemFlag(FLAG_DECIMP);
  groupingGap = 3;
  clearSystemFlag(FLAG_TDM24); // time format = 12H
  clearSystemFlag(FLAG_MDY);   // date format
  clearSystemFlag(FLAG_YMD);   // date format
  setSystemFlag(FLAG_DMY);     // date format
  firstGregorianDay = 2361222 /* 14 Sept 1752 */; // JDN of the first day in the Gregorian calendar
}



void fnConfigUsa(uint16_t unusedButMandatoryParameter) {
  setSystemFlag(FLAG_DECIMP);
  groupingGap = 3;
  clearSystemFlag(FLAG_TDM24); // time format = 12H
  clearSystemFlag(FLAG_YMD);   // date format
  clearSystemFlag(FLAG_DMY);   // date format
  setSystemFlag(FLAG_MDY);     // date format
  firstGregorianDay = 2361222 /* 14 Sept 1752 */; // JDN of the first day in the Gregorian calendar
}



void fnIntegerMode(uint16_t mode) {
  shortIntegerMode = mode;
}



void fnWho(uint16_t unusedButMandatoryParameter) {
  temporaryInformation = TI_WHO;
 }



void fnVersion(uint16_t unusedButMandatoryParameter) {
  temporaryInformation = TI_VERSION;
}



void fnFreeMemory(uint16_t unusedButMandatoryParameter) {
  longInteger_t mem;

  liftStack();

  longIntegerInit(mem);
  uIntToLongInteger(getFreeRamMemory(), mem);
  convertLongIntegerToLongIntegerRegister(mem, REGISTER_X);
  longIntegerFree(mem);
}



void fnGetRoundingMode(uint16_t unusedButMandatoryParameter) {
  longInteger_t rounding;

  liftStack();

  longIntegerInit(rounding);
  uIntToLongInteger(roundingMode, rounding);
  convertLongIntegerToLongIntegerRegister(rounding, REGISTER_X);
  longIntegerFree(rounding);
}



void fnSetRoundingMode(uint16_t RM) {
  roundingMode = RM;
}

// "realRoundingMode_t" does not match with the specification of WP43 rounding mode.
// So you need roundingModeTable[roundingMode] rather than roundingMode
// to specify rounding mode in the real number functions.
TO_QSPI const realRoundingMode_t roundingModeTable[7] = {
  DEC_ROUND_HALF_EVEN, DEC_ROUND_HALF_UP, DEC_ROUND_HALF_DOWN,
  DEC_ROUND_UP, DEC_ROUND_DOWN, DEC_ROUND_CEILING, DEC_ROUND_FLOOR
};



void fnGetIntegerSignMode(uint16_t unusedButMandatoryParameter) {
  fnRecall(RESERVED_VARIABLE_ISM);
}



void fnGetWordSize(uint16_t unusedButMandatoryParameter) {
  longInteger_t wordSize;

  liftStack();

  longIntegerInit(wordSize);
  uIntToLongInteger(shortIntegerWordSize, wordSize);
  convertLongIntegerToLongIntegerRegister(wordSize, REGISTER_X);
  longIntegerFree(wordSize);
}



void fnSetWordSize(uint16_t WS) {
  bool reduceWordSize;
  if(WS == 0) {
    WS = 64;
  }

  reduceWordSize = (WS < shortIntegerWordSize);

  shortIntegerWordSize = WS;

  if(shortIntegerWordSize == 64) {
    shortIntegerMask    = -1;
  }
  else {
    shortIntegerMask    = ((uint64_t)1 << shortIntegerWordSize) - 1;
  }

  shortIntegerSignBit = (uint64_t)1 << (shortIntegerWordSize - 1);
  //printf("shortIntegerMask  =   %08x-%08x\n", (unsigned int)(shortIntegerMask>>32), (unsigned int)(shortIntegerMask&0xffffffff));
  //printf("shortIntegerSignBit = %08x-%08x\n", (unsigned int)(shortIntegerSignBit>>32), (unsigned int)(shortIntegerSignBit&0xffffffff));

  if(reduceWordSize) {
    // reduce the word size of integers on the stack
    for(calcRegister_t regist=REGISTER_X; regist<=getStackTop(); regist++) {
      if(getRegisterDataType(regist) == dtShortInteger) {
        *(REGISTER_SHORT_INTEGER_DATA(regist)) &= shortIntegerMask;
      }
    }

    // reduce the word size of integers in the L register
    if(getRegisterDataType(REGISTER_L) == dtShortInteger) {
      *(REGISTER_SHORT_INTEGER_DATA(REGISTER_L)) &= shortIntegerMask;
    }
  }
}



void fnFreeFlashMemory(uint16_t unusedButMandatoryParameter) {
  longInteger_t flashMem;

  liftStack();

  longIntegerInit(flashMem);
  uIntToLongInteger(getFreeFlash(), flashMem);
  convertLongIntegerToLongIntegerRegister(flashMem, REGISTER_X);
  longIntegerFree(flashMem);
}



void fnBatteryVoltage(uint16_t unusedButMandatoryParameter) {
  real_t value;

  liftStack();

  #if defined(PC_BUILD)
    int32ToReal(3100, &value);
  #endif // PC_BUILD

  #if defined(DMCP_BUILD)
    int32ToReal(get_vbat(), &value);
  #endif // DMCP_BUILD

  realDivide(&value, const_1000, &value, &ctxtReal39);
  convertRealToReal34ResultRegister(&value, REGISTER_X);
}



uint32_t getFreeFlash(void) {
  return FLASH_PGM_PAGE_SIZE * FLASH_PGM_NUMBER_OF_PAGES - sizeOfFlashPgmLibrary - 2;
}



void fnGetSignificantDigits(uint16_t unusedButMandatoryParameter) {
  longInteger_t sigDigits;

  liftStack();

  longIntegerInit(sigDigits);
  uIntToLongInteger(significantDigits == 0 ? 34 : significantDigits, sigDigits);
  convertLongIntegerToLongIntegerRegister(sigDigits, REGISTER_X);
  longIntegerFree(sigDigits);
}



void fnSetSignificantDigits(uint16_t unusedButMandatoryParameter) {
  longInteger_t sigDigits;

  if(getRegisterDataType(REGISTER_X) == dtLongInteger) {
    convertLongIntegerRegisterToLongInteger(REGISTER_X, sigDigits);
    if((longIntegerCompareInt(sigDigits, 0) >= 0) && (longIntegerCompareInt(sigDigits, 34) <= 0)) {
      significantDigits = longIntegerToUInt(sigDigits);
      if(significantDigits == 0) {
        significantDigits = 34;
      }
    }
    else {
      displayCalcErrorMessage(ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
      #if defined(PC_BUILD)
        longIntegerToAllocatedString(sigDigits, errorMessage, sizeof(errorMessage));
        moreInfoOnError("In function fnSetSignificantDigits:", errorMessage, "is out of range.", "");
      #endif // PC_BUILD
    }
    longIntegerFree(sigDigits);
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
    #if defined(PC_BUILD)
      sprintf(errorMessage, "DataType %" PRIu32, getRegisterDataType(REGISTER_X));
      moreInfoOnError("In function fnSetSignificantDigits:", errorMessage, "is not a long integer.", "");
    #endif // PC_BUILD
  }
}



void fnRoundingMode(uint16_t RM) {
  roundingMode = RM;

  switch(RM) {
    case 0: {
      ctxtReal34.round = DEC_ROUND_HALF_EVEN;
      break;
    }

    case 1: {
      ctxtReal34.round = DEC_ROUND_HALF_UP;
      break;
    }

    case 2: {
      ctxtReal34.round = DEC_ROUND_HALF_DOWN;
      break;
    }

    case 3: {
      ctxtReal34.round = DEC_ROUND_UP;
      break;
    }

    case 4: {
      ctxtReal34.round = DEC_ROUND_DOWN;
      break;
    }

    case 5: {
      ctxtReal34.round = DEC_ROUND_CEILING;
      break;
    }

    case 6: {
      ctxtReal34.round = DEC_ROUND_FLOOR;
      break;
    }

    default: {
      sprintf(errorMessage, "In function fnRoundingMode: %d is an unexpected value for RM! Must be from 0 to 6", RM);
      bugScreen(errorMessage);
      break;
    }
  }
}



void fnAngularMode(uint16_t am) {
  currentAngularMode = am;
}



void fnFractionType(uint16_t unusedButMandatoryParameter) {
  if(getSystemFlag(FLAG_FRACT)) {
    flipSystemFlag(FLAG_PROPFR);
  }
  else {
    setSystemFlag(FLAG_FRACT);
    setSystemFlag(FLAG_PROPFR);
  }
}



void setConfirmationMode(void (*func)(uint16_t)) {
  previousCalcMode = calcMode;
  cursorEnabled = false;
  calcMode = cmConfirmation;
  clearSystemFlag(FLAG_ALPHA);
  confirmedFunction = func;
  temporaryInformation = TI_ARE_YOU_SURE;
}



void fnRange(uint16_t unusedButMandatoryParameter) {
  longInteger_t longInt;

  if(getRegisterDataType(REGISTER_X) == dtLongInteger) {
    convertLongIntegerRegisterToLongInteger(REGISTER_X, longInt);
  }
  else if(getRegisterDataType(REGISTER_X) == dtReal34) {
    convertReal34ToLongInteger(REGISTER_REAL34_DATA(REGISTER_X), longInt, DEC_ROUND_DOWN);
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "cannot use %s for setting RANGE", getRegisterDataTypeName(REGISTER_X, true, true));
      moreInfoOnError("In function fnRange:", errorMessage, NULL, NULL);
    #endif //  (EXTRA_INFO_ON_CALC_ERROR == 1)
    return;
  }

  longIntegerSetPositiveSign(longInt);

  if(longIntegerCompareInt(longInt, 6145) > 0) {
    exponentLimit = 6145;
  }
  else if(longIntegerCompareInt(longInt, 99) < 0) {
    exponentLimit = 99;
  }
  else {
    exponentLimit = (int16_t)(longInt->_mp_d[0]); // OK for 32 and 64 bit limbs
  }

  longIntegerFree(longInt);
}



void fnGetRange(uint16_t unusedButMandatoryParameter) {
  longInteger_t range;

  liftStack();

  longIntegerInit(range);
  uIntToLongInteger(exponentLimit, range);
  convertLongIntegerToLongIntegerRegister(range, REGISTER_X);
  longIntegerFree(range);
}



void fnHide(uint16_t unusedButMandatoryParameter) {
  longInteger_t longInt;

  if(getRegisterDataType(REGISTER_X) == dtLongInteger) {
    convertLongIntegerRegisterToLongInteger(REGISTER_X, longInt);
  }
  else if(getRegisterDataType(REGISTER_X) == dtReal34) {
    convertReal34ToLongInteger(REGISTER_REAL34_DATA(REGISTER_X), longInt, DEC_ROUND_DOWN);
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "cannot use %s for setting HIDE", getRegisterDataTypeName(REGISTER_X, true, true));
      moreInfoOnError("In function fnHide:", errorMessage, NULL, NULL);
    #endif //  (EXTRA_INFO_ON_CALC_ERROR == 1)
    return;
  }

  longIntegerSetPositiveSign(longInt);

  if(longIntegerIsZero(longInt)) {
    exponentHideLimit = 0;
  }
  else if(longIntegerCompareInt(longInt, 99) > 0) {
    exponentHideLimit = 99;
  }
  else if(longIntegerCompareInt(longInt, 12) < 0) {
    exponentHideLimit = 12;
  }
  else {
    exponentHideLimit = (int16_t)(longInt->_mp_d[0]); // OK for 32 and 64 bit limbs
  }

  longIntegerFree(longInt);
}



void fnGetHide(uint16_t unusedButMandatoryParameter) {
  longInteger_t range;

  liftStack();

  longIntegerInit(range);
  uIntToLongInteger(exponentHideLimit, range);
  convertLongIntegerToLongIntegerRegister(range, REGISTER_X);
  longIntegerFree(range);
}



void fnClAll(uint16_t confirmation) {
  if(confirmation == NOT_CONFIRMED) {
    setConfirmationMode(fnClAll);
  }
  else {
    calcRegister_t regist;

    fnClPAll(CONFIRMED);  // Clears all the programs
    fnClSigma(CONFIRMED); // Clears and releases the memory of all statistical sums
    if(savedStatisticalSumsPointer != NULL) {
      freeWp43(savedStatisticalSumsPointer, NUMBER_OF_STATISTICAL_SUMS * REAL_SIZE);
    }

    // Clear local registers
    allocateLocalRegisters(0);

    // Clear registers including stack, I, J, K and L
    for(regist=0; regist<FIRST_LOCAL_REGISTER; regist++) {
      clearRegister(regist);
    }

    // Clear saved stack registers
    for(regist=FIRST_SAVED_STACK_REGISTER; regist<=LAST_TEMP_REGISTER; regist++) {
      clearRegister(regist);
    }
    thereIsSomethingToUndo = false;

    // TODO: clear (or delete) named variables

    // Clear global flags
    fnClFAll(CONFIRMED);

    temporaryInformation = TI_NO_INFO;
    if(programRunStop == PGM_WAITING) {
      programRunStop = PGM_STOPPED;
    }
  }
}



void addTestPrograms(void) {
  uint32_t numberOfBytesUsed, numberOfBytesForTheTestPrograms = TO_BYTES(TO_BLOCKS(11509));

  resizeProgramMemory(TO_BLOCKS(numberOfBytesForTheTestPrograms));
  firstDisplayedStep.ram        = beginOfProgramMemory;
  currentStep.ram               = beginOfProgramMemory;
  currentLocalStepNumber        = 1;
  firstDisplayedLocalStepNumber = 0;

  if(!ioFileOpen(ioPathTestPgms, ioModeRead)) {
    #if !defined(DMCP_BUILD)
      printf("Cannot open file res/dmcp/testPgms.bin\n");
    #endif // !DMCP_BUILD
    *(beginOfProgramMemory)     = 255; // .END.
    *(beginOfProgramMemory + 1) = 255; // .END.
    firstFreeProgramByte = beginOfProgramMemory;
    freeProgramBytes = numberOfBytesForTheTestPrograms - 2;
  }
  else {
    #if defined(DMCP_BUILD)
      ioFileRead(&numberOfBytesUsed, sizeof(numberOfBytesUsed));
      ioFileRead(beginOfProgramMemory, numberOfBytesUsed);
    #else
      ignore_result(ioFileRead(&numberOfBytesUsed, sizeof(numberOfBytesUsed)));
      printf("%u bytes\n", numberOfBytesUsed);
      if(numberOfBytesUsed > numberOfBytesForTheTestPrograms) {
        printf("Increase allocated memory for programs! File config.c 1st line of function addTestPrograms\n");
        ioFileClose();
        exit(0);
      }
      ignore_result(ioFileRead(beginOfProgramMemory, numberOfBytesUsed));
    #endif // DMCP_BUILD !DMCP_BUILD
    ioFileClose();
    firstFreeProgramByte = beginOfProgramMemory + (numberOfBytesUsed - 2);
    freeProgramBytes = numberOfBytesForTheTestPrograms - numberOfBytesUsed;
  }

  #if !defined(DMCP_BUILD)
    printf("freeProgramBytes = %u\n", freeProgramBytes);
  #endif // !DMCP_BUILD

  scanLabelsAndPrograms();
  #if !defined(DMCP_BUILD)
    #if !defined(TESTSUITE_BUILD)
      leavePem();
    #endif // !TESTSUITE_BUILD
    printf("freeProgramBytes = %u\n", freeProgramBytes);
    //listPrograms();
    //listLabelsAndPrograms();
  #endif // !DMCP_BUILD
}



void restoreStats(void){
  if(lrChosen !=65535) {
    lrChosen = lrChosenHistobackup;
  }
  if(lrSelection !=65535) {
    lrSelection = lrSelectionHistobackup;
  }
  strcpy(statMx,"STATS");
  lrSelectionHistobackup = 65535;
  lrChosenHistobackup = 65535;
  calcSigma(0);
}



void fnReset(uint16_t confirmation) {
  if(confirmation == NOT_CONFIRMED) {
    setConfirmationMode(fnReset);
  }
  else {
    void *memPtr;

    if(ram == NULL) {
      ram = (dataBlock_t *)malloc(TO_BYTES(RAM_SIZE));
    }
    memset(ram, 0, TO_BYTES(RAM_SIZE));
    numberOfFreeMemoryRegions = 1;
    freeMemoryRegions[0].address = 40;                     // for reserved variables
    freeMemoryRegions[0].sizeInBlocks = RAM_SIZE - 40 - 1; // - 1: one block for an empty program

    if(tmpString == NULL) {
      #if defined(DMCP_BUILD)
         tmpString        = aux_buf_ptr();   // 2560 byte buffer provided by DMCP
         errorMessage     = write_buf_ptr(); // 4096 byte buffer provided by DMCP
       #else // !DMCP_BUILD
         tmpString        = (char *)malloc(TMP_STR_LENGTH);
         errorMessage     = (char *)malloc(WRITE_BUFFER_LEN);
       #endif // DMCP_BUILD

       aimBuffer        = errorMessage + ERROR_MESSAGE_LENGTH;
       nimBufferDisplay = aimBuffer + AIM_BUFFER_LENGTH;
       tamBuffer        = nimBufferDisplay + NIM_BUFFER_LENGTH;

       tmpStringLabelOrVariableName = tmpString + 1000;
    }
    memset(tmpString,        0, TMP_STR_LENGTH);
    memset(errorMessage,     0, ERROR_MESSAGE_LENGTH);
    memset(aimBuffer,        0, AIM_BUFFER_LENGTH);
    memset(nimBufferDisplay, 0, NIM_BUFFER_LENGTH);
    memset(tamBuffer,        0, TAM_BUFFER_LENGTH);

    // Empty program initialization
    beginOfProgramMemory          = (uint8_t *)(ram + freeMemoryRegions[0].sizeInBlocks);
    currentStep.ram               = beginOfProgramMemory;
    firstFreeProgramByte          = beginOfProgramMemory + 2;
    firstDisplayedStep.ram        = beginOfProgramMemory;
    firstDisplayedLocalStepNumber = 0;
    labelList                     = NULL;
    programList                   = NULL;
    flashLabelList                = NULL;
    flashProgramList              = NULL;
    *(beginOfProgramMemory + 0) = (ITM_END >> 8) | 0x80;
    *(beginOfProgramMemory + 1) =  ITM_END       & 0xff;
    *(beginOfProgramMemory + 2) = 255; // .END.
    *(beginOfProgramMemory + 3) = 255; // .END.
    freeProgramBytes            = 0;

    scanFlashPgmLibrary();
    scanLabelsAndPrograms();

    // "Not found glyph" initialization
    if(glyphNotFound.data == NULL) {
      glyphNotFound.data = malloc(38);
    }
    xcopy(glyphNotFound.data, "\xff\xf8\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\xff\xf8", 38);

    // Initialization of user key assignments
    xcopy(kbd_usr, kbd_std, sizeof(kbd_std));
    //kbd_usr[ 0].keyLblAim   = CHR_A_GRAVE;
    //kbd_usr[ 0].fShiftedAim = CHR_A_GRAVE;
    //kbd_usr[ 4].keyLblAim   = CHR_E_ACUTE;
    //kbd_usr[ 4].fShiftedAim = CHR_E_ACUTE;
    //kbd_usr[18].fShifted    = -MNU_VARS;
    //kbd_usr[18].gShifted    = CST_54;
    //kbd_usr[19].fShifted    = ITM_SW;
    //kbd_usr[19].gShifted    = ITM_SXY;
    //kbd_usr[20].gShifted    = ITM_LYtoM;

    // initialize 9 real34 reserved variables: ACC, ↑Lim, ↓Lim, FV, i%/a, NPER, PER/a, PMT, and PV
    for(int i=0; i<9; i++) {
      real34Zero(allocWp43(REAL34_SIZE));
    }

    // initialize 1 long integer reserved variables: GRAMOD
    #if defined(OS64BIT)
      memPtr = allocWp43(3);
      ((dataBlock_t *)memPtr)->dataMaxLength = 2;
    #else // !OS64BIT
      memPtr = allocWp43(2);
      ((dataBlock_t *)memPtr)->dataMaxLength = 1;
    #endif // OS64BIT

    // initialize the global registers
    memset(globalRegister, 0, sizeof(globalRegister));
    for(calcRegister_t regist=0; regist<=LAST_GLOBAL_REGISTER; regist++) {
      setRegisterDataType(regist, dtReal34, amNone);
      memPtr = allocWp43(REAL34_SIZE);
      setRegisterDataPointer(regist, memPtr);
      real34Zero(memPtr);
    }

    // initialize the NUMBER_OF_SAVED_STACK_REGISTERS + the NUMBER_OF_TEMP_REGISTERS
    memset(savedStackRegister, 0, sizeof(savedStackRegister));
    for(calcRegister_t regist=FIRST_SAVED_STACK_REGISTER; regist<=LAST_TEMP_REGISTER; regist++) {
      setRegisterDataType(regist, dtReal34, amNone);
      memPtr = allocWp43(REAL34_SIZE);
      setRegisterDataPointer(regist, memPtr);
      real34Zero(memPtr);
    }

    // Clear global flags
    memset(globalFlags, 0, sizeof(globalFlags));

    // allocate space for the local register list
    allSubroutineLevels.numberOfSubroutineLevels = 1;
    currentSubroutineLevelData = allocWp43(3);
    allSubroutineLevels.ptrToSubroutineLevel0Data = TO_WP43MEMPTR(currentSubroutineLevelData);
    currentReturnProgramNumber = 0;
    currentReturnLocalStep = 0;
    currentNumberOfLocalRegisters = 0; // No local register
    currentNumberOfLocalFlags = 0; // No local flags
    currentSubroutineLevel = 0;
    currentPtrToNextLevel = WP43_NULL;
    currentPtrToPreviousLevel = WP43_NULL;
    currentLocalFlags = NULL;
    currentLocalRegisters = NULL;

    // allocate space for the named variable list
    numberOfNamedVariables = 0;
    allNamedVariables = NULL;


    allocateNamedVariable("Mat_A", dtReal34Matrix, REAL34_SIZE + 1);
    memPtr = getRegisterDataPointer(FIRST_NAMED_VARIABLE);
    ((dataBlock_t *)memPtr)->matrixRows = 1;
    ((dataBlock_t *)memPtr)->matrixColumns = 1;
    real34Zero(memPtr + 4);

    allocateNamedVariable("Mat_B", dtReal34Matrix, REAL34_SIZE + 1);
    memPtr = getRegisterDataPointer(FIRST_NAMED_VARIABLE + 1);
    ((dataBlock_t *)memPtr)->matrixRows = 1;
    ((dataBlock_t *)memPtr)->matrixColumns = 1;
    real34Zero(memPtr + 4);

    allocateNamedVariable("Mat_X", dtReal34Matrix, REAL34_SIZE + 1);
    memPtr = getRegisterDataPointer(FIRST_NAMED_VARIABLE + 2);
    ((dataBlock_t *)memPtr)->matrixRows = 1;
    ((dataBlock_t *)memPtr)->matrixColumns = 1;
    real34Zero(memPtr + 4);

    #if !defined(TESTSUITE_BUILD)
      matrixIndex = INVALID_VARIABLE; // Unset matrix index
    #endif // !TESTSUITE_BUILD

    decContextDefault(&ctxtReal34, DEC_INIT_DECQUAD);

    decContextDefault(&ctxtReal4, DEC_INIT_DECSINGLE);
    ctxtReal4.digits = 6;
    ctxtReal4.traps  = 0;

    decContextDefault(&ctxtReal39, DEC_INIT_DECQUAD);
    ctxtReal39.digits = 39;
    ctxtReal39.emax   = 99999;
    ctxtReal39.emin   = -99999;
    ctxtReal39.traps  = 0;

    decContextDefault(&ctxtReal51, DEC_INIT_DECQUAD);
    ctxtReal51.digits = 51;
    ctxtReal51.emax   = 99999;
    ctxtReal51.emin   = -99999;
    ctxtReal51.traps  = 0;

    decContextDefault(&ctxtReal75, DEC_INIT_DECQUAD);
    ctxtReal75.digits = 75;
    ctxtReal75.emax   = 99999;
    ctxtReal75.emin   = -99999;
    ctxtReal75.traps  = 0;

    decContextDefault(&ctxtReal1071,  DEC_INIT_DECQUAD);
    ctxtReal1071.digits = 1071;
    ctxtReal1071.emax   = 99999;
    ctxtReal1071.emin   = -99999;
    ctxtReal1071.traps  = 0;

    //decContextDefault(&ctxtReal2139,  DEC_INIT_DECQUAD);
    //ctxtReal2139.digits = 2139;
    //ctxtReal2139.traps  = 0;


    statisticalSumsPointer = NULL;
    savedStatisticalSumsPointer = NULL;
    lrSelection = CF_LINEAR_FITTING;
    lrSelectionUndo = lrSelection;
    lrChosen    = 0;
    lrChosenUndo = 0;
    lastPlotMode = PLOT_NOTHING;
    plotSelection = 0;
    drawHistogram = 0;
    realZero(&SAVED_SIGMA_LASTX);
    realZero(&SAVED_SIGMA_LASTY);
    SAVED_SIGMA_LAct = 0;

    //restoreStats();
    plotStatMx[0] = 0;
    real34Zero(&loBinR);
    real34Zero(&nBins );
    real34Zero(&hiBinR);
    histElementXorY = -1;


    x_min = -10;
    x_max = 10;
    y_min = 0;
    y_max = 1;

    shortIntegerMode = SIM_2COMPL;
    fnSetWordSize(64);

    groupingGap = 3;

    systemFlags = 0;
    displayFormat = dfAll;
    displayFormatDigits = 0;
    timeDisplayFormatDigits = 0;
    currentAngularMode = amDegree;
    denMax = MAX_DENMAX;
    setSystemFlag(FLAG_DENANY);
    setSystemFlag(FLAG_MULTx);
    setSystemFlag(FLAG_DECIMP);
    setSystemFlag(FLAG_AUTOFF);
    setSystemFlag(FLAG_TDM24); // time format = 24H
    setSystemFlag(FLAG_YMD);   // date format = yyyy-mm-dd
    setSystemFlag(FLAG_ASLIFT);
    setSystemFlag(FLAG_PROPFR);
    setSystemFlag(FLAG_ENDPMT);// TVM application = END mode

    hourGlassIconEnabled = false;
    watchIconEnabled = false;
    serialIOIconEnabled = false;
    printerIconEnabled = false;
    thereIsSomethingToUndo = false;
    pemCursorIsZerothStep = true;
    fnKeyInCatalog = false;
    shiftF = false;
    shiftG = false;

    significantDigits = 0;
    roundingMode = rmHalfEven;
    ctxtReal34.round = DEC_ROUND_HALF_EVEN;
    displayStack = cachedDisplayStack = 4;

    fontBrowserInit();
    flagBrowserInit();
    registerBrowserInit();

    memset(softmenuStack, 0, sizeof(softmenuStack)); // This works because the ID of MyMenu is 0

    aimBuffer[0] = 0;
    lastErrorCode = 0;

    gammaLanczosCoefficients = (real51_t *)const_gammaC01;
    angle180 = (real51_t *)const_pi_51;
    angle90  = (real51_t *)const_piOn2_51;
    angle45  = (real51_t *)const_piOn4_51;

    #if !defined(TESTSUITE_BUILD)
      resetAlphaSelectionBuffer();
    #endif // !TESTSUITE_BUILD

    #if defined(TESTSUITE_BUILD)
      calcMode = cmNormal;
    #else // TESTSUITE_BUILD
      if(calcMode == cmMim) {
        mimFinalize();
      }
      calcModeEnter(cmNormal);
    #endif // !TESTSUITE_BUILD

    #if defined(PC_BUILD) || defined(TESTSUITE_BUILD)
      debugMemAllocation = true;
    #endif // PC_BUILD || TESTSUITE_BUILD

    // RNG initialisation
    pcg32_srandom(0x1963073019931121ULL, 0x1995062319981019ULL);

    tamReset();
    catalog = CATALOG_NONE;
    memset(lastCatalogPosition, 0, NUMBER_OF_CATALOGS * sizeof(lastCatalogPosition[0]));
    firstGregorianDay = 2361222 /* 14 Sept 1752 */;
    exponentLimit = 6145;
    exponentHideLimit = 0;
    lastIntegerBase = 0;
    temporaryInformation = TI_RESET;

    currentInputVariable = INVALID_VARIABLE;
    currentMvarLabel = INVALID_VARIABLE;
    lastKeyCode = 0;
    entryStatus = 0;

    memset(userMenuItems,  0, sizeof(userMenuItem_t) * 18);
    memset(userAlphaItems, 0, sizeof(userMenuItem_t) * 18);
    userMenus = NULL;
    numberOfUserMenus = 0;
    currentUserMenu = 0;
    userKeyLabelSize = 37/*keys*/ * 6/*states*/ * 1/*byte terminator*/ + 1/*byte sentinel*/;
    userKeyLabel = allocWp43(TO_BLOCKS(userKeyLabelSize));
    memset(userKeyLabel,   0, TO_BYTES(TO_BLOCKS(userKeyLabelSize)));

    fnClearMenu(NOPARAM);

    screenUpdatingMode = SCRUPD_AUTO;

    // The following lines are test data
    addTestPrograms();
    //fnSetFlag(  3);
    //fnSetFlag( 11);
    //fnSetFlag( 33);
    //fnSetFlag( 34);
    //fnSetFlag( 52);
    //fnSetFlag( 62);
    //fnSetFlag( 77);
    //fnSetFlag( 85);
    //setSystemFlag(FLAG_CARRY);
    //setSystemFlag(FLAG_SPCRES);

    //allocateLocalRegisters(3);
    //fnSetFlag(FIRST_LOCAL_REGISTER+0);
    //fnSetFlag(NUMBER_OF_GLOBAL_FLAGS+2);
    //reallocateRegister(FIRST_LOCAL_REGISTER+0, dtReal34, REAL34_SIZE, RT_REAL);
    //stringToReal34("5.555", REGISTER_REAL34_DATA(FIRST_LOCAL_REGISTER));

    //strcpy(tmpString, "Pure ASCII string requiring 38 bytes!");
    //reallocateRegister(FIRST_LOCAL_REGISTER+1, dtString, TO_BLOCKS(strlen(tmpString) + 1), amNone);
    //strcpy(REGISTER_STRING_DATA(FIRST_LOCAL_REGISTER + 1), tmpString);


    //allocateNamedVariable("Z" STD_a_DIARESIS "hler");
    //allocateNamedVariable(STD_omega STD_SUB_1);
    //allocateNamedVariable(STD_omega STD_SUB_2);

    // Equation formulae
    allFormulae = NULL;
    numberOfFormulae = 0;
    currentFormula = 0;

    graphVariable = 0;

    timerAppResetState();

    #if (DEBUG_PANEL == 1)
      debugInit();
    #endif // DEBUG_PANEL == 1
  }
}



void backToSystem(uint16_t unusedButMandatoryParameter) {
  #if defined(PC_BUILD)
    fnOff(NOPARAM);
  #endif // PC_BUILD

  #if defined(DMCP_BUILD)
    backToDMCP = true;
  #endif // DMCP_BUILD
}



void configSetUpTimers(void) {
  timerReset();
  timerConfig(tidAutoRepeat,               execAutoRepeat);
  timerConfig(tidTimerAppRedraw,           cbTimerAppRedraw);
  timerConfig(tidTimerAppDetectWrapAround, cbTimerAppDetectWrapAround);
  timerConfig(tidShowNop,                  cbShowNop);
}
