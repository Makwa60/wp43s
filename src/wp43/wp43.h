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
 * \file wp43.h
 */

#if !defined(WP43_H)
  #define WP43_H

  #pragma GCC diagnostic ignored "-Wunused-parameter"


  #if defined(LINUX)
    #include <math.h>
  #endif // LINUX

  #if defined(OSX)
    // needed by chdir
    #include <unistd.h>
  #endif // OSX

  #if defined(PC_BUILD)
    #include <glib.h>
    #include <gtk/gtk.h>
    #include <gdk/gdk.h>
  #endif // PC_BUILD

  #if defined(WIN32)
    #include <locale.h>
  #endif // WIN32

  #if defined(DMCP_BUILD)
    #define DBG_PRINT

    #if defined(DBG_PRINT)
      #include <stdio.h>
    #else
      #define printf(...)
    #endif

    #include <dmcp.h>
  #endif // DMCP_BUILD

  #include "mathematics/pcg_basic.h"
  #include "realType.h"
  #include "typeDefinitions.h"
  #include <stdbool.h>

  // Variables for the simulator
  #if defined(PC_BUILD) || defined(TESTSUITE_BUILD)
    extern bool                 debugMemAllocation;
  #endif // PC_BUILD || TESTSUITE_BUILD

  #if defined(PC_BUILD)
    extern bool                 calcLandscape;
    extern bool                 calcAutoLandscapePortrait;
    extern GtkWidget           *screen;
    extern GtkWidget           *frmCalc;
    extern int16_t              screenStride;
    extern int16_t              debugWindow;
    extern uint32_t            *screenData;
    extern bool                 screenChange;
    extern char                 debugString[10000];
    #if (DEBUG_REGISTER_L == 1)
      extern GtkWidget         *lblRegisterL1;
      extern GtkWidget         *lblRegisterL2;
    #endif // (DEBUG_REGISTER_L == 1)
    #if (SHOW_MEMORY_STATUS == 1)
      extern GtkWidget         *lblMemoryStatus;
    #endif // (SHOW_MEMORY_STATUS == 1)
  #endif //PC_BUILD

  // Variables stored in FLASH
  extern const item_t                    indexOfItems[];
  extern const reservedVariableHeader_t  allReservedVariables[];
  extern const calcKey_t                 kbd_std[37];
  extern const font_t                    standardFont, numericFont;
  extern const font_t                   *fontForShortInteger;
  extern const font_t                   *cursorFont;
  extern const char                      digits[17];
  extern any34Matrix_t                   openMatrixMIMPointer;
  extern uint16_t                        matrixIndex;
  extern void                            (* const addition[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS][NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void);
  extern void                            (* const subtraction[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS][NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void);
  extern void                            (* const multiplication[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS][NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void);
  extern void                            (* const division[NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS][NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS])(void);
  extern void                            (*confirmedFunction)(uint16_t);
  extern const softmenu_t                softmenu[];
  extern real51_t                 const *gammaLanczosCoefficients;
  extern real51_t                 const *angle180;
  extern real51_t                 const *angle90;
  extern real51_t                 const *angle45;

  // Variables stored in RAM
  extern bool                   funcOK;
  extern bool                   keyActionProcessed;
  extern bool                   fnKeyInCatalog;
  extern bool                   hourGlassIconEnabled;
  extern bool                   watchIconEnabled;
  extern bool                   printerIconEnabled;
  extern bool                   shiftF;
  extern bool                   shiftG;
  extern bool                   rbr1stDigit;
  extern bool                   updateDisplayValueX;
  extern bool                   thereIsSomethingToUndo;
  extern bool                   lastProgramListEnd;
  extern bool                   programListEnd;
  extern bool                   serialIOIconEnabled;
  extern bool                   pemCursorIsZerothStep;

  extern registerHeader_t       globalRegister[NUMBER_OF_GLOBAL_REGISTERS];
  extern registerHeader_t       savedStackRegister[NUMBER_OF_SAVED_STACK_REGISTERS + NUMBER_OF_TEMP_REGISTERS];
  extern registerHeader_t      *currentLocalRegisters;
  extern dynamicSoftmenu_t      dynamicSoftmenu[NUMBER_OF_DYNAMIC_SOFTMENUS];

  extern dataBlock_t            allSubroutineLevels;
  extern dataBlock_t           *statisticalSumsPointer;
  extern dataBlock_t           *savedStatisticalSumsPointer;
  extern dataBlock_t           *ram;
  extern dataBlock_t           *currentLocalFlags;
  extern dataBlock_t           *currentSubroutineLevelData;

  extern namedVariableHeader_t *allNamedVariables;
  extern softmenuStack_t        softmenuStack[SOFTMENU_STACK_SIZE];
  extern userMenuItem_t         userMenuItems[18];
  extern userMenuItem_t         userAlphaItems[18];
  extern userMenu_t            *userMenus;
  extern programmableMenu_t     programmableMenu;
  extern calcKey_t              kbd_usr[37];
  extern glyph_t                glyphNotFound;
  extern freeMemoryRegion_t     freeMemoryRegions[MAX_FREE_REGION];
  extern pcg32_random_t         pcg32_global;
  extern labelList_t           *labelList;
  extern labelList_t           *flashLabelList;
  extern programList_t         *programList;
  extern programList_t         *flashProgramList;
  extern angularMode_t          currentAngularMode;
  extern formulaHeader_t       *allFormulae;

  extern pgmPtr_t               beginOfCurrentProgram;
  extern pgmPtr_t               endOfCurrentProgram;
  extern pgmPtr_t               firstDisplayedStep;
  extern pgmPtr_t               currentStep;

  extern char                  *tmpString;
  extern char                  *tmpStringLabelOrVariableName;
  extern char                  *aimBuffer; // aimBuffer is also used for NIM
  extern char                  *nimBufferDisplay;

  /**
   * Buffer for output of TAM current state. After calling ::tamProcessInput this
   * buffer is updated to the latest TAM state and should be redrawn to the relevant
   * part of the screen.
   */
  extern char                  *tamBuffer;
  extern char                  *userKeyLabel;
  extern char                   asmBuffer[5];
  extern char                   oldTime[8];
  extern char                   dateTimeString[12];
  extern char                   displayValueX[DISPLAY_VALUE_LEN];

  extern uint8_t                numScreensStandardFont;
  extern uint8_t                displayFormat;
  extern uint8_t                displayFormatDigits;
  extern uint8_t                timeDisplayFormatDigits;
  extern uint8_t                shortIntegerWordSize;
  extern uint8_t                significantDigits;
  extern uint8_t                shortIntegerMode;
  extern uint8_t                previousCalcMode;
  extern uint8_t                groupingGap;
  extern roundingMode_t         roundingMode;
  extern uint8_t                nextChar;
  extern uint8_t                displayStack;
  extern uint8_t                cachedDisplayStack;
  extern uint8_t                alphaCase;
  extern uint8_t                cursorEnabled;
  extern uint8_t                nimNumberPart;
  extern uint8_t                hexDigits;
  extern temporaryInformation_t temporaryInformation;
  extern uint8_t                numScreensNumericFont;
  extern uint8_t                programRunStop;
  extern uint8_t                lastKeyCode;
  extern uint8_t                entryStatus; // 0x01 for the entry flag, backed up to 0x02 for undo
  extern uint8_t                screenUpdatingMode;
  extern uint8_t               *beginOfProgramMemory;
  extern uint8_t               *firstFreeProgramByte;

  /**
   * Instance of the internal state for TAM.
   */
  extern tamState_t             tam;
  extern int16_t                lineTWidth;
  extern int16_t                rbrRegister;
  extern int16_t                catalog;
  extern int16_t                lastCatalogPosition[NUMBER_OF_CATALOGS];
  extern int16_t                showFunctionNameItem;
  extern int16_t                exponentSignLocation;
  extern int16_t                denominatorLocation;
  extern int16_t                imaginaryExponentSignLocation;
  extern int16_t                imaginaryMantissaSignLocation;
  extern int16_t                exponentLimit;
  extern int16_t                exponentHideLimit;
  extern int16_t                showFunctionNameCounter;
  extern int16_t                dynamicMenuItem;
  extern int16_t               *menu_RAM;
  extern int16_t                numberOfTamMenusToPop;
  extern int16_t                itemToBeAssigned;
  extern int16_t                cachedDynamicMenu;

  extern uint16_t               globalFlags[7];
  extern uint16_t               freeProgramBytes;
  extern uint16_t               firstDisplayedLocalStepNumber;
  extern uint16_t               numberOfLabels;
  extern uint16_t               numberOfLabelsInFlash;
  extern uint16_t               numberOfPrograms;
  extern uint16_t               numberOfProgramsInFlash;
  extern uint16_t               numberOfNamedVariables;
  extern uint16_t               currentLocalStepNumber;
  extern uint16_t               currentProgramNumber;
  extern uint16_t               lrSelection;
  extern uint16_t               lrSelectionUndo;
  extern uint16_t               lrChosen;
  extern uint16_t               lrChosenUndo;
  extern uint16_t               lastPlotMode;
  extern uint16_t               plotSelection;
  extern uint16_t               currentViewRegister;
  extern uint16_t               currentSolverStatus;
  extern uint16_t               currentSolverProgram;
  extern uint16_t               currentSolverVariable;
  extern uint16_t               currentSolverNestingDepth;
  extern uint16_t               numberOfFormulae;
  extern uint16_t               currentFormula;
  extern uint16_t               numberOfUserMenus;
  extern uint16_t               currentUserMenu;
  extern uint16_t               userKeyLabelSize;
  extern uint16_t               currentInputVariable;
  extern uint16_t               currentMvarLabel;
  #if (REAL34_WIDTH_TEST == 1)
    extern uint16_t               largeur;
  #endif // (REAL34_WIDTH_TEST == 1)

  extern int32_t                numberOfFreeMemoryRegions;
  extern int32_t                lgCatalogSelection;
  extern int32_t                graphVariable;

  extern uint32_t               firstGregorianDay;
  extern uint32_t               denMax;
  extern uint32_t               lastIntegerBase;
  extern uint32_t               alphaSelectionTimer;
  extern uint32_t               xCursor;
  extern uint32_t               yCursor;
  extern uint32_t               tamOverPemYPos;
  extern uint32_t               pointerOfFlashPgmLibrary;
  extern uint32_t               sizeOfFlashPgmLibrary;

  extern uint64_t               shortIntegerMask;
  extern uint64_t               shortIntegerSignBit;
  extern uint64_t               systemFlags;
  extern uint64_t               savedSystemFlags;

  extern size_t                 gmpMemInBytes;
  extern size_t                 wp43MemInBlocks;

  extern real_t                 SAVED_SIGMA_LASTX;
  extern real_t                 SAVED_SIGMA_LASTY;
  extern int32_t                SAVED_SIGMA_LAct;

  extern uint16_t               lrSelectionHistobackup;
  extern uint16_t               lrChosenHistobackup;
  extern int16_t                histElementXorY;
  extern real34_t               loBinR;
  extern real34_t               nBins ;
  extern real34_t               hiBinR;
  extern char                   statMx[8];
  extern char                   plotStatMx[8];

  #if defined(DMCP_BUILD)
    extern bool                 backToDMCP;
    //extern int                  keyAutoRepeat; // Key repetition
    //extern int16_t              previousItem;
    extern uint32_t             nextTimerRefresh;

    int                         convertKeyCode(int key);
  #endif // DMCP_BUILD
#endif // !WP43_H
