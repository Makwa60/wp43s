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

#include "wp43.h"

#include "defines.h"

#if defined(PC_BUILD) || defined(TESTSUITE_BUILD)
  bool_t              debugMemAllocation;
#endif // PC_BUILD || TESTSUITE_BUILD

const font_t          *fontForShortInteger;
const font_t          *cursorFont;
TO_QSPI const char     digits[17] = "0123456789ABCDEF";
real51_t               const *gammaLanczosCoefficients;
real51_t               const *angle180;
real51_t               const *angle90;
real51_t               const *angle45;
void                   (*confirmedFunction)(uint16_t);

// Variables stored in RAM
bool_t                 funcOK;
bool_t                 keyActionProcessed;
bool_t                 fnKeyInCatalog;
bool_t                 hourGlassIconEnabled;
bool_t                 watchIconEnabled;
bool_t                 printerIconEnabled;
bool_t                 shiftF;
bool_t                 shiftG;
bool_t                 rbr1stDigit;
bool_t                 updateDisplayValueX;
bool_t                 thereIsSomethingToUndo;
bool_t                 lastProgramListEnd;
bool_t                 programListEnd;
bool_t                 serialIOIconEnabled;
bool_t                 pemCursorIsZerothStep;

realContext_t          ctxtReal4;    //   limited digits: used for higher speed internal real calcs
realContext_t          ctxtReal34;   //   34 digits
realContext_t          ctxtReal39;   //   39 digits: used for 34 digits intermediate calculations
realContext_t          ctxtReal51;   //   51 digits: used for 34 digits intermediate calculations
realContext_t          ctxtReal75;   //   75 digits: used in SLVQ
realContext_t          ctxtReal1071; // 1071 digits: used in radian angle reduction
//realContext_t          ctxtReal2139; // 2139 digits: used for really big modulo

registerHeader_t       globalRegister[NUMBER_OF_GLOBAL_REGISTERS];
registerHeader_t       savedStackRegister[NUMBER_OF_SAVED_STACK_REGISTERS + NUMBER_OF_TEMP_REGISTERS];
registerHeader_t      *currentLocalRegisters;

dataBlock_t            allSubroutineLevels;
dataBlock_t           *statisticalSumsPointer;
dataBlock_t           *savedStatisticalSumsPointer;
dataBlock_t           *ram = NULL;
dataBlock_t           *currentLocalFlags;
dataBlock_t           *currentSubroutineLevelData;

namedVariableHeader_t *allNamedVariables;
softmenuStack_t        softmenuStack[SOFTMENU_STACK_SIZE];
userMenuItem_t         userMenuItems[18];
userMenuItem_t         userAlphaItems[18];
userMenu_t            *userMenus;
programmableMenu_t     programmableMenu;
calcKey_t              kbd_usr[37];
glyph_t                glyphNotFound = {.charCode = 0x0000, .colsBeforeGlyph = 0, .colsGlyph = 13, .colsAfterGlyph = 0, .rowsGlyph = 19, .data = NULL};
freeMemoryRegion_t     freeMemoryRegions[MAX_FREE_REGION];
pcg32_random_t         pcg32_global = PCG32_INITIALIZER;
labelList_t           *labelList = NULL;
labelList_t           *flashLabelList = NULL;
programList_t         *programList = NULL;
programList_t         *flashProgramList = NULL;
angularMode_t          currentAngularMode;
formulaHeader_t       *allFormulae;

pgmPtr_t               beginOfCurrentProgram;
pgmPtr_t               endOfCurrentProgram;
pgmPtr_t               firstDisplayedStep;
pgmPtr_t               currentStep;

char                  *tmpString = NULL;
char                  *tmpStringLabelOrVariableName = NULL;
char                  *aimBuffer; // aimBuffer is also used for NIM
char                  *nimBufferDisplay;
char                  *tamBuffer;
char                  *userKeyLabel;
char                   asmBuffer[5];
char                   oldTime[8];
char                   dateTimeString[12];
char                   displayValueX[DISPLAY_VALUE_LEN];

uint8_t                numScreensStandardFont;
uint8_t                displayFormat;
uint8_t                displayFormatDigits;
uint8_t                timeDisplayFormatDigits;
uint8_t                shortIntegerWordSize;
uint8_t                significantDigits;
uint8_t                shortIntegerMode;
uint8_t                previousCalcMode;
uint8_t                groupingGap;
uint8_t                roundingMode;
uint8_t                nextChar;
uint8_t                displayStack;
uint8_t                cachedDisplayStack;
uint8_t                alphaCase;
uint8_t                cursorEnabled;
uint8_t                nimNumberPart;
uint8_t                hexDigits;
temporaryInformation_t temporaryInformation;
uint8_t                numScreensNumericFont;
uint8_t                timerCraAndDeciseconds = 128u;
uint8_t                programRunStop;
uint8_t                lastKeyCode;
uint8_t                entryStatus;
uint8_t                screenUpdatingMode;
uint8_t               *beginOfProgramMemory;
uint8_t               *firstFreeProgramByte;

tamState_t             tam;
int16_t                lineTWidth;
int16_t                rbrRegister;
int16_t                catalog;
int16_t                lastCatalogPosition[NUMBER_OF_CATALOGS];
int16_t                showFunctionNameItem;
int16_t                exponentSignLocation;
int16_t                denominatorLocation;
int16_t                imaginaryExponentSignLocation;
int16_t                imaginaryMantissaSignLocation;
int16_t                exponentLimit;
int16_t                exponentHideLimit;
int16_t                showFunctionNameCounter;
int16_t                dynamicMenuItem;
int16_t               *menu_RAM;
int16_t                numberOfTamMenusToPop;
int16_t                itemToBeAssigned;
int16_t                cachedDynamicMenu;

uint16_t               globalFlags[7];
uint16_t               freeProgramBytes;
uint16_t               firstDisplayedLocalStepNumber;
uint16_t               numberOfLabels;
uint16_t               numberOfLabelsInFlash;
uint16_t               numberOfPrograms;
uint16_t               numberOfProgramsInFlash;
uint16_t               numberOfNamedVariables;
uint16_t               currentLocalStepNumber;
uint16_t               currentProgramNumber;
uint16_t               lrSelection;
uint16_t               lrSelectionUndo;
uint16_t               lrChosen;
uint16_t               lrChosenUndo;
uint16_t               lastPlotMode;
uint16_t               plotSelection;
uint16_t               currentViewRegister;
uint16_t               currentSolverStatus;
uint16_t               currentSolverProgram;
uint16_t               currentSolverVariable;
uint16_t               currentSolverNestingDepth;
uint16_t               numberOfFormulae;
uint16_t               currentFormula;
uint16_t               numberOfUserMenus;
uint16_t               currentUserMenu;
uint16_t               userKeyLabelSize;
uint16_t               currentInputVariable = INVALID_VARIABLE;
uint16_t               currentMvarLabel = INVALID_VARIABLE;
#if (REAL34_WIDTH_TEST == 1)
  uint16_t               largeur=200;
#endif // (REAL34_WIDTH_TEST == 1)

int32_t                numberOfFreeMemoryRegions;
int32_t                lgCatalogSelection;
int32_t                graphVariable;

uint32_t               firstGregorianDay;
uint32_t               denMax;
uint32_t               lastIntegerBase;
uint32_t               alphaSelectionTimer;
uint32_t               xCursor;
uint32_t               yCursor;
uint32_t               tamOverPemYPos;
uint32_t               timerValue;
uint32_t               timerStartTime = TIMER_APP_STOPPED;
uint32_t               timerTotalTime;
uint32_t               pointerOfFlashPgmLibrary;
uint32_t               sizeOfFlashPgmLibrary;

uint64_t               shortIntegerMask;
uint64_t               shortIntegerSignBit;
uint64_t               systemFlags;
uint64_t               savedSystemFlags;

size_t                 gmpMemInBytes;
size_t                 wp43MemInBlocks;

real_t                 SAVED_SIGMA_LASTX;
real_t                 SAVED_SIGMA_LASTY;
int32_t                SAVED_SIGMA_LAct;

uint16_t               lrSelectionHistobackup;
uint16_t               lrChosenHistobackup;
int16_t                histElementXorY;
real34_t               loBinR;
real34_t               nBins ;
real34_t               hiBinR;
char                   statMx[8];
char                   plotStatMx[8];
