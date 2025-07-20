// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "saveRestoreCalcState.h"

#include "apps/apps.h"
#include "apps/bugScreen.h"
#include "apps/flagBrowser.h"
#include "apps/fontBrowser.h"
#include "apps/registerBrowser.h"
#include "apps/timerApp.h"
#include "assign.h"
#include "calcMode.h"
#include "charString.h"
#include "config.h"
#include "core/memory.h"
#include "display.h"
#include "error.h"
#include "flags.h"
#include "hal/gui.h"
#include "hal/io.h"
#include "items.h"
#include "mathematics/matrix.h"
#include "plotstat.h"
#include "programming/decode.h"
#include "programming/lblGtoXeq.h"
#include "programming/manage.h"
#include "programming/nextStep.h"
#include "registers.h"
#include "registerValueConversions.h"
#include "solver/equation.h"
#include "solver/graph.h"
#include "sort.h"
#include "stats.h"
#include "ui/cursor.h"
#include "ui/screen.h"
#include "ui/softmenus.h"
#include "ui/tam.h"
#if defined(DMCP_BUILD)
  #include <dmcp.h>
  #include <wp43-dmcp.h>
#endif // DMCP_BUILD
#include <stdbool.h>
#include <string.h>

#include "wp43.h"

// Backup versions:
// 92 Add CONFIG and ALLVAR catalogs
// 93 Change softmenu stack depth from 8 to 4
// 94 Add different softmenu stacks for RUM, AIM & PEM, add MyPFN user menu
// 95 Updated softmenuStack_t structure for user menu id
// 96 Changed 'MSG r' to 'MSG n'
// 97 Added cursor arrows to Mya
// 98 Added FNCS_EIM catalog
//
#define BACKUP_VERSION                     98  // Added FNCS_EIM catalog
#define OLDEST_COMPATIBLE_BACKUP_VERSION   87  // save running app
#define START_REGISTER_VALUE             1000  // was 1522, why?

#define configFileVersion       1

#if !defined(TESTSUITE_BUILD)
  static char *tmpRegisterString = NULL;

  static void save(const void *buffer, uint32_t size) {
    ioFileWrite(buffer, size);
  }
#endif //TESTSUITE_BUILD

static uint32_t restore(void *buffer, uint32_t size) {
  return ioFileRead(buffer, size);
}



#if defined(PC_BUILD)
  void saveCalc(void) {
    uint32_t backupVersion = BACKUP_VERSION;
    uint32_t ramSize       = RAM_SIZE_IN_BLOCKS;
    uint32_t ramPtr;
    int ret;

    ret = ioFileOpen(ioPathBackup, ioModeWrite);

    if(ret != FILE_OK ) {
      if(ret == FILE_CANCEL ) {
        return;
      } else {
        printf("Cannot save calc's memory in file backup.bin!\n");
        exit(0);
      }
    }

    if(calcMode == cmConfirmation) {
      calcMode = previousCalcMode;
      refreshScreen();
    }

    printf("Begin of calc's backup\n");

    save(&backupVersion,                      sizeof(backupVersion));
    save(&ramSize,                            sizeof(ramSize));
    save(ram,                                 TO_BYTES(RAM_SIZE_IN_BLOCKS));
    save(freeMemoryRegions,                   sizeof(freeMemoryRegions));
    save(&numberOfFreeMemoryRegions,          sizeof(numberOfFreeMemoryRegions));
    save(globalFlags,                         sizeof(globalFlags));
    save(errorMessage,                        ERROR_MESSAGE_LENGTH);
    save(aimBuffer,                           AIM_BUFFER_LENGTH);
    save(nimBufferDisplay,                    NIM_BUFFER_LENGTH);
    save(tamBuffer,                           TAM_BUFFER_LENGTH);
    save(asmBuffer,                           sizeof(asmBuffer));
    save(oldTime,                             sizeof(oldTime));
    save(dateTimeString,                      sizeof(dateTimeString));
    save(softmenuStacks,                      sizeof(softmenuStacks));
    save(smStackMode,                         sizeof(smStackMode));
    save(globalRegister,                      sizeof(globalRegister));
    save(savedStackRegister,                  sizeof(savedStackRegister));
    save(kbd_usr,                             sizeof(kbd_usr));
    save(userMenuItems,                       sizeof(userMenuItems));
    save(userAlphaItems,                      sizeof(userAlphaItems));
    save(userPfnItems,                        sizeof(userPfnItems));
    save(&tam.mode,                           sizeof(tam.mode));
    save(&tam.function,                       sizeof(tam.function));
    save(&tam.alpha,                          sizeof(tam.alpha));
    save(&tam.currentOperation,               sizeof(tam.currentOperation));
    save(&tam.dot,                            sizeof(tam.dot));
    save(&tam.indirect,                       sizeof(tam.indirect));
    save(&tam.digitsSoFar,                    sizeof(tam.digitsSoFar));
    save(&tam.value,                          sizeof(tam.value));
    save(&tam.min,                            sizeof(tam.min));
    save(&tam.max,                            sizeof(tam.max));
    save(&rbrRegister,                        sizeof(rbrRegister));
    save(&numberOfNamedVariables,             sizeof(numberOfNamedVariables));
    ramPtr = TO_WP43MEMPTR(allNamedVariables);
    save(&ramPtr,                             sizeof(ramPtr));
    ramPtr = TO_WP43MEMPTR(allFormulae);
    save(&ramPtr,                             sizeof(ramPtr));
    ramPtr = TO_WP43MEMPTR(userMenus);
    save(&ramPtr,                             sizeof(ramPtr));
    ramPtr = TO_WP43MEMPTR(userKeyLabel);
    save(&ramPtr,                             sizeof(ramPtr));
    ramPtr = TO_WP43MEMPTR(statisticalSumsPointer);
    save(&ramPtr,                             sizeof(ramPtr));
    ramPtr = TO_WP43MEMPTR(savedStatisticalSumsPointer);
    save(&ramPtr,                             sizeof(ramPtr));
    ramPtr = TO_WP43MEMPTR(labelList);
    save(&ramPtr,                             sizeof(ramPtr));
    ramPtr = TO_WP43MEMPTR(programList);
    save(&ramPtr,                             sizeof(ramPtr));
    ramPtr = TO_WP43MEMPTR(currentSubroutineLevelData);
    save(&ramPtr,                             sizeof(ramPtr));
    save(&xCursor,                            sizeof(xCursor));
    save(&yCursor,                            sizeof(yCursor));
    save(&firstGregorianDay,                  sizeof(firstGregorianDay));
    save(&denMax,                             sizeof(denMax));
    save(&lastDenominator,                    sizeof(lastDenominator));
    save(&currentRegisterBrowserScreen,       sizeof(currentRegisterBrowserScreen));
    save(&currentFntScr,                      sizeof(currentFntScr));
    save(&flagScreen,                         sizeof(flagScreen));
    save(&displayFormat,                      sizeof(displayFormat));
    save(&displayFormatDigits,                sizeof(displayFormatDigits));
    save(&timeDisplayFormatDigits,            sizeof(timeDisplayFormatDigits));
    save(&shortIntegerWordSize,               sizeof(shortIntegerWordSize));
    save(&significantDigits,                  sizeof(significantDigits));
    save(&shortIntegerMode,                   sizeof(shortIntegerMode));
    save(&currentAngularMode,                 sizeof(currentAngularMode));
    save(&groupingGap,                        sizeof(groupingGap));
    save(&roundingMode,                       sizeof(roundingMode));
    save(&calcMode,                           sizeof(calcMode));
    save(&currentApp,                         sizeof(currentApp));
    save(&nextChar,                           sizeof(nextChar));
    save(&alphaCase,                          sizeof(alphaCase));
    save(&hourGlassIconEnabled,               sizeof(hourGlassIconEnabled));
    save(&watchIconEnabled,                   sizeof(watchIconEnabled));
    save(&serialIOIconEnabled,                sizeof(serialIOIconEnabled));
    save(&printerIconEnabled,                 sizeof(printerIconEnabled));
    save(&programRunStop,                     sizeof(programRunStop));
    save(&entryStatus,                        sizeof(entryStatus));
    save(&cursorEnabled,                      sizeof(cursorEnabled));
    save(&cursorFont,                         sizeof(cursorFont));
    save(&rbr1stDigit,                        sizeof(rbr1stDigit));
    save(&shiftF,                             sizeof(shiftF));
    save(&shiftG,                             sizeof(shiftG));
    save(&rbrMode,                            sizeof(rbrMode));
    save(&showContent,                        sizeof(showContent));
    save(&numScreensNumericFont,              sizeof(numScreensNumericFont));
    save(&numScreensStandardFont,             sizeof(numScreensStandardFont));
    save(&previousCalcMode,                   sizeof(previousCalcMode));
    save(&lastErrorCode,                      sizeof(lastErrorCode));
    save(&nimNumberPart,                      sizeof(nimNumberPart));
    save(&displayStack,                       sizeof(displayStack));
    save(&hexDigits,                          sizeof(hexDigits));
    save(&errorMessageRegisterLine,           sizeof(errorMessageRegisterLine));
    save(&shortIntegerMask,                   sizeof(shortIntegerMask));
    save(&shortIntegerSignBit,                sizeof(shortIntegerSignBit));
    save(&temporaryInformation,               sizeof(temporaryInformation));
    save(&glyphNotFound,                      sizeof(glyphNotFound));
    save(&funcOK,                             sizeof(funcOK));
    save(&screenChange,                       sizeof(screenChange));
    save(&exponentSignLocation,               sizeof(exponentSignLocation));
    save(&denominatorLocation,                sizeof(denominatorLocation));
    save(&imaginaryExponentSignLocation,      sizeof(imaginaryExponentSignLocation));
    save(&imaginaryMantissaSignLocation,      sizeof(imaginaryMantissaSignLocation));
    save(&lineTWidth,                         sizeof(lineTWidth));
    save(&lastIntegerBase,                    sizeof(lastIntegerBase));
    save(&wp43MemInBlocks,                    sizeof(wp43MemInBlocks));
    save(&gmpMemInBytes,                      sizeof(gmpMemInBytes));
    save(&catalog,                            sizeof(catalog));
    save(&lastCatalogPosition,                sizeof(lastCatalogPosition));
    save(&lgCatalogSelection,                 sizeof(lgCatalogSelection));
    save(displayValueX,                       sizeof(displayValueX));
    save(&pcg32_global,                       sizeof(pcg32_global));
    save(&exponentLimit,                      sizeof(exponentLimit));
    save(&exponentHideLimit,                  sizeof(exponentHideLimit));
    save(&keyActionProcessed,                 sizeof(keyActionProcessed));
    save(&systemFlags,                        sizeof(systemFlags));
    save(&savedSystemFlags,                   sizeof(savedSystemFlags));
    save(&thereIsSomethingToUndo,             sizeof(thereIsSomethingToUndo));
    ramPtr = TO_WP43MEMPTR(beginOfProgramMemory);
    save(&ramPtr,                             sizeof(ramPtr)); // beginOfProgramMemory pointer to block
    ramPtr = (uint32_t)((void *)beginOfProgramMemory -        TO_PCMEMPTR(TO_WP43MEMPTR(beginOfProgramMemory)));
    save(&ramPtr,                             sizeof(ramPtr)); // beginOfProgramMemory offset within block
    ramPtr = TO_WP43MEMPTR(firstFreeProgramByte);
    save(&ramPtr,                             sizeof(ramPtr)); // firstFreeProgramByte pointer to block
    ramPtr = (uint32_t)((void *)firstFreeProgramByte - TO_PCMEMPTR(TO_WP43MEMPTR(firstFreeProgramByte)));
    save(&ramPtr,                             sizeof(ramPtr)); // firstFreeProgramByte offset within block
    ramPtr = TO_WP43MEMPTR(firstDisplayedStep);
    save(&ramPtr,                             sizeof(ramPtr)); // firstDisplayedStep pointer to block
    ramPtr = (uint32_t)((void *)firstDisplayedStep - TO_PCMEMPTR(TO_WP43MEMPTR(firstDisplayedStep)));
    save(&ramPtr,                             sizeof(ramPtr)); // firstDisplayedStep offset within block
    ramPtr = TO_WP43MEMPTR(currentStep);
    save(&ramPtr,                             sizeof(ramPtr)); // currentStep pointer to block
    ramPtr = (uint32_t)((void *)currentStep - TO_PCMEMPTR(TO_WP43MEMPTR(currentStep)));
    save(&ramPtr,                             sizeof(ramPtr)); // currentStep offset within block
    save(&freeProgramBytes,                   sizeof(freeProgramBytes));
    save(&firstDisplayedLocalStepNumber,      sizeof(firstDisplayedLocalStepNumber));
    save(&numberOfLabels,                     sizeof(numberOfLabels));
    save(&numberOfPrograms,                   sizeof(numberOfPrograms));
    save(&currentLocalStepNumber,             sizeof(currentLocalStepNumber));
    save(&currentProgramNumber,               sizeof(currentProgramNumber));
    save(&lastProgramListEnd,                 sizeof(lastProgramListEnd));
    save(&programListEnd,                     sizeof(programListEnd));
    save(&allSubroutineLevels,                sizeof(allSubroutineLevels));
    save(&pemCursorIsZerothStep,              sizeof(pemCursorIsZerothStep));
    save(&numberOfTamMenusToPop,              sizeof(numberOfTamMenusToPop));
    save(&lrSelection,                        sizeof(lrSelection));
    save(&lrSelectionUndo,                    sizeof(lrSelectionUndo));
    save(&lrChosen,                           sizeof(lrChosen));
    save(&lrChosenUndo,                       sizeof(lrChosenUndo));
    save(&lastPlotMode,                       sizeof(lastPlotMode));
    save(&plotSelection,                      sizeof(plotSelection));

    save(&graph_dx,                           sizeof(graph_dx));
    save(&graph_dy,                           sizeof(graph_dy));
    save(&roundedTicks,                       sizeof(roundedTicks));
    save(&extentx,                            sizeof(extentx));
    save(&extenty,                            sizeof(extenty));
    save(&PLOT_VECT,                          sizeof(PLOT_VECT));
    save(&PLOT_NVECT,                         sizeof(PLOT_NVECT));
    save(&PLOT_SCALE,                         sizeof(PLOT_SCALE));
    save(&Aspect_Square,                      sizeof(Aspect_Square));
    save(&PLOT_LINE,                          sizeof(PLOT_LINE));
    save(&PLOT_CROSS,                         sizeof(PLOT_CROSS));
    save(&PLOT_BOX,                           sizeof(PLOT_BOX));
    save(&PLOT_INTG,                          sizeof(PLOT_INTG));
    save(&PLOT_DIFF,                          sizeof(PLOT_DIFF));
    save(&PLOT_RMS,                           sizeof(PLOT_RMS));
    save(&PLOT_SHADE,                         sizeof(PLOT_SHADE));
    save(&PLOT_AXIS,                          sizeof(PLOT_AXIS));
    save(&PLOT_ZMX,                           sizeof(PLOT_ZMX));
    save(&PLOT_ZMY,                           sizeof(PLOT_ZMY));
    save(&PLOT_ZOOM,                          sizeof(PLOT_ZOOM));
    save(&plotmode,                           sizeof(plotmode));
    save(&tick_int_x,                         sizeof(tick_int_x));
    save(&tick_int_y,                         sizeof(tick_int_y));
    save(&x_min,                              sizeof(x_min));
    save(&x_max,                              sizeof(x_max));
    save(&y_min,                              sizeof(y_min));
    save(&y_max,                              sizeof(y_max));
    save(&xzero,                              sizeof(xzero));
    save(&yzero,                              sizeof(yzero));
    save(&matrixIndex,                        sizeof(matrixIndex));
    save(&currentViewRegister,                sizeof(currentViewRegister));
    save(&currentSolverStatus,                sizeof(currentSolverStatus));
    save(&currentSolverProgram,               sizeof(currentSolverProgram));
    save(&currentSolverVariable,              sizeof(currentSolverVariable));
    save(&numberOfFormulae,                   sizeof(numberOfFormulae));
    save(&currentFormula,                     sizeof(currentFormula));
    save(&equationEditorCursor,               sizeof(equationEditorCursor));
    save(&equationEditorScrollPosition,       sizeof(equationEditorScrollPosition));
    save(&alphaCursor,                        sizeof(alphaCursor));
    save(&numberOfUserMenus,                  sizeof(numberOfUserMenus));
    save(&currentUserMenu,                    sizeof(currentUserMenu));
    save(&userKeyLabelSize,                   sizeof(userKeyLabelSize));
    save(&timerAppState,                      sizeof(timerAppState));
    save(&currentInputVariable,               sizeof(currentInputVariable));
    save(&SAVED_SIGMA_LASTX,                  sizeof(SAVED_SIGMA_LASTX));
    save(&SAVED_SIGMA_LASTY,                  sizeof(SAVED_SIGMA_LASTY));
    save(&SAVED_SIGMA_LAct,                   sizeof(SAVED_SIGMA_LAct));
    save(&currentMvarLabel,                   sizeof(currentMvarLabel));
    save(&graphVariable,                      sizeof(graphVariable));
    save(&plotStatMx,                         sizeof(plotStatMx));
    save(&drawHistogram,                      sizeof(drawHistogram));
    save(&statMx,                             sizeof(statMx));
    save(&lrSelectionHistobackup,             sizeof(lrSelectionHistobackup));
    save(&lrChosenHistobackup,                sizeof(lrChosenHistobackup));
    save(&loBinR,                             sizeof(loBinR));
    save(&nBins ,                             sizeof(nBins ));
    save(&hiBinR,                             sizeof(hiBinR));
    save(&histElementXorY,                    sizeof(histElementXorY));

    save(&screenUpdatingMode,                 sizeof(screenUpdatingMode));
    for(int y = 0; y < SCREEN_HEIGHT; ++y) {
      uint8_t bmpdata = 0;
      for(int x = 0; x < SCREEN_WIDTH; ++x) {
        bmpdata <<= 1;
        if(*(screenData + y*screenStride + x) == ON_PIXEL) {
          bmpdata |= 1;
        }
        if((x % 8) == 7) {
          save(&bmpdata,                      sizeof(bmpdata));
          bmpdata = 0;
        }
      }
    }

    ioFileClose();
    printf("End of calc's backup\n");
  }



  void restoreCalc(void) {
    uint32_t backupVersion, ramSize, ramPtr;
    uint8_t *loadedScreen = malloc(SCREEN_WIDTH * SCREEN_HEIGHT / 8);
    int ret;

    fnReset(CONFIRMED);
    ret = ioFileOpen(ioPathBackup, ioModeRead);

    if(ret != FILE_OK ) {
      if(ret == FILE_CANCEL ) {
        return;
      } else {
        printf("Cannot restore calc's memory from file backup.bin! Performing RESET\n");
        refreshScreen();
        return;
      }
    }

    restore(&backupVersion,                      sizeof(backupVersion));
    restore(&ramSize,                            sizeof(ramSize));
    if(backupVersion > BACKUP_VERSION || backupVersion < OLDEST_COMPATIBLE_BACKUP_VERSION || ramSize != RAM_SIZE_IN_BLOCKS) {
      ioFileClose();
      refreshScreen();

      printf("Cannot restore calc's memory from file backup.bin! File backup.bin is from incompatible backup version.\n");
      printf("               Backup file      Program\n");
      printf("backupVersion  %6u           %6d\n", backupVersion, BACKUP_VERSION);
      printf("ramSize blocks %6u           %6d\n", ramSize, RAM_SIZE_IN_BLOCKS);
      printf("ramSize bytes  %6u           %6d\n", TO_BYTES(ramSize), TO_BYTES(RAM_SIZE_IN_BLOCKS));
      return;
    }
    else {
      printf("Begin of calc's restoration\n");
      printf("*** backupVersion %d\n",backupVersion);fflush(stdout);

      restore(ram,                                 TO_BYTES(RAM_SIZE_IN_BLOCKS));
      restore(freeMemoryRegions,                   sizeof(freeMemoryRegions));
      restore(&numberOfFreeMemoryRegions,          sizeof(numberOfFreeMemoryRegions));
      restore(globalFlags,                         sizeof(globalFlags));
      restore(errorMessage,                        ERROR_MESSAGE_LENGTH);
      restore(aimBuffer,                           AIM_BUFFER_LENGTH);
      restore(nimBufferDisplay,                    NIM_BUFFER_LENGTH);
      restore(tamBuffer,                           TAM_BUFFER_LENGTH);
      restore(asmBuffer,                           sizeof(asmBuffer));
      restore(oldTime,                             sizeof(oldTime));
      restore(dateTimeString,                      sizeof(dateTimeString));
      if (backupVersion < 93) {
        restore(softmenuStacks,                    sizeof(softmenuStacks)/2);   // single softmenuStack , depth 8 levels,
        softmenuStacksInit();                                                   // reinitialize the softmenuStack to a clean state
      } else if (backupVersion == 93) {
        restore(softmenuStacks,                    sizeof(softmenuStacks)/4);   // single softmenuStack , depth 4 levels,
        softmenuStacksInit();                                                   // reinitialize the softmenuStack to a clean state
      } else if (backupVersion == 94) {
        restore(softmenuStacks,                    4*4*4);                      // Four softmenu Stacks, depth 4 levels each, 4 bytes per level (2xint16)
        softmenuStacksInit();                                                   // reinitialize the softmenuStack to a clean state
        restore(smStackMode,                       sizeof(smStackMode));
      } else {
        restore(&softmenuStacks,                   sizeof(softmenuStacks));     // Four softmenu Stacks, depth 4 levels each, 6 bytes per level (3xint16)
        restore(smStackMode,                       sizeof(smStackMode));
      }
      restore(globalRegister,                      sizeof(globalRegister));
      restore(savedStackRegister,                  sizeof(savedStackRegister));
      restore(kbd_usr,                             sizeof(kbd_usr));
      restore(userMenuItems,                       sizeof(userMenuItems));
      restore(userAlphaItems,                      sizeof(userAlphaItems));
      if (backupVersion > 93) {
        restore(userPfnItems,                      sizeof(userPfnItems));       // MyPFN menu is available from backup version 94
      }
      restore(&tam.mode,                           sizeof(tam.mode));
      restore(&tam.function,                       sizeof(tam.function));
      restore(&tam.alpha,                          sizeof(tam.alpha));
      restore(&tam.currentOperation,               sizeof(tam.currentOperation));
      restore(&tam.dot,                            sizeof(tam.dot));
      restore(&tam.indirect,                       sizeof(tam.indirect));
      restore(&tam.digitsSoFar,                    sizeof(tam.digitsSoFar));
      restore(&tam.value,                          sizeof(tam.value));
      restore(&tam.min,                            sizeof(tam.min));
      restore(&tam.max,                            sizeof(tam.max));
      restore(&rbrRegister,                        sizeof(rbrRegister));
      restore(&numberOfNamedVariables,             sizeof(numberOfNamedVariables));
      restore(&ramPtr,                             sizeof(ramPtr));
      allNamedVariables = TO_PCMEMPTR(ramPtr);
      restore(&ramPtr,                             sizeof(ramPtr));
      allFormulae = TO_PCMEMPTR(ramPtr);
      restore(&ramPtr,                             sizeof(ramPtr));
      userMenus = TO_PCMEMPTR(ramPtr);
      restore(&ramPtr,                             sizeof(ramPtr));
      userKeyLabel = TO_PCMEMPTR(ramPtr);
      restore(&ramPtr,                             sizeof(ramPtr));
      statisticalSumsPointer = TO_PCMEMPTR(ramPtr);
      restore(&ramPtr,                             sizeof(ramPtr));
      savedStatisticalSumsPointer = TO_PCMEMPTR(ramPtr);
      restore(&ramPtr,                             sizeof(ramPtr));
      labelList = TO_PCMEMPTR(ramPtr);
      if(backupVersion < 91) { // flashLabelList
        restore(&ramPtr,                           sizeof(ramPtr));
      }
      restore(&ramPtr,                             sizeof(ramPtr));
      programList = TO_PCMEMPTR(ramPtr);
      if(backupVersion < 91) { // flashProgramList
        restore(&ramPtr,                           sizeof(ramPtr));
      }
      restore(&ramPtr,                             sizeof(ramPtr));
      currentSubroutineLevelData = TO_PCMEMPTR(ramPtr);
      restore(&xCursor,                            sizeof(xCursor));
      restore(&yCursor,                            sizeof(yCursor));
      restore(&firstGregorianDay,                  sizeof(firstGregorianDay));
      restore(&denMax,                             sizeof(denMax));
      if(backupVersion >= 88) {
        restore(&lastDenominator,                  sizeof(lastDenominator));
      }
      restore(&currentRegisterBrowserScreen,       sizeof(currentRegisterBrowserScreen));
      restore(&currentFntScr,                      sizeof(currentFntScr));
      restore(&flagScreen,                         sizeof(flagScreen));
      restore(&displayFormat,                      sizeof(displayFormat));
      restore(&displayFormatDigits,                sizeof(displayFormatDigits));
      restore(&timeDisplayFormatDigits,            sizeof(timeDisplayFormatDigits));
      restore(&shortIntegerWordSize,               sizeof(shortIntegerWordSize));
      restore(&significantDigits,                  sizeof(significantDigits));
      restore(&shortIntegerMode,                   sizeof(shortIntegerMode));
      restore(&currentAngularMode,                 sizeof(currentAngularMode));
      restore(&groupingGap,                        sizeof(groupingGap));
      restore(&roundingMode,                       sizeof(roundingMode));
      restore(&calcMode,                           sizeof(calcMode));
      restore(&currentApp,                         sizeof(currentApp));
      restore(&nextChar,                           sizeof(nextChar));
      restore(&alphaCase,                          sizeof(alphaCase));
      restore(&hourGlassIconEnabled,               sizeof(hourGlassIconEnabled));
      restore(&watchIconEnabled,                   sizeof(watchIconEnabled));
      restore(&serialIOIconEnabled,                sizeof(serialIOIconEnabled));
      restore(&printerIconEnabled,                 sizeof(printerIconEnabled));
      restore(&programRunStop,                     sizeof(programRunStop));
      restore(&entryStatus,                        sizeof(entryStatus));
      restore(&cursorEnabled,                      sizeof(cursorEnabled));
      restore(&cursorFont,                         sizeof(cursorFont));
      restore(&rbr1stDigit,                        sizeof(rbr1stDigit));
      restore(&shiftF,                             sizeof(shiftF));
      restore(&shiftG,                             sizeof(shiftG));
      restore(&rbrMode,                            sizeof(rbrMode));
      restore(&showContent,                        sizeof(showContent));
      restore(&numScreensNumericFont,              sizeof(numScreensNumericFont));
      restore(&numScreensStandardFont,             sizeof(numScreensStandardFont));
      restore(&previousCalcMode,                   sizeof(previousCalcMode));
      restore(&lastErrorCode,                      sizeof(lastErrorCode));
      restore(&nimNumberPart,                      sizeof(nimNumberPart));
      restore(&displayStack,                       sizeof(displayStack));
      restore(&hexDigits,                          sizeof(hexDigits));
      restore(&errorMessageRegisterLine,           sizeof(errorMessageRegisterLine));
      restore(&shortIntegerMask,                   sizeof(shortIntegerMask));
      restore(&shortIntegerSignBit,                sizeof(shortIntegerSignBit));
      restore(&temporaryInformation,               sizeof(temporaryInformation));

      restore(&glyphNotFound,                      sizeof(glyphNotFound));
      glyphNotFound.data   = malloc(38);
      xcopy(glyphNotFound.data, "\xff\xf8\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\x80\x08\xff\xf8", 38);

      restore(&funcOK,                             sizeof(funcOK));
      restore(&screenChange,                       sizeof(screenChange));
      restore(&exponentSignLocation,               sizeof(exponentSignLocation));
      restore(&denominatorLocation,                sizeof(denominatorLocation));
      restore(&imaginaryExponentSignLocation,      sizeof(imaginaryExponentSignLocation));
      restore(&imaginaryMantissaSignLocation,      sizeof(imaginaryMantissaSignLocation));
      restore(&lineTWidth,                         sizeof(lineTWidth));
      restore(&lastIntegerBase,                    sizeof(lastIntegerBase));
      restore(&wp43MemInBlocks,                    sizeof(wp43MemInBlocks));
      restore(&gmpMemInBytes,                      sizeof(gmpMemInBytes));
      restore(&catalog,                            sizeof(catalog));
      if(backupVersion < 92) { // add catalogs
        restore(&lastCatalogPosition,              38);
        for(int i = 19 /* ITM_MENU */; i >= 16 /* MNU_MVAR */; --i) {
          lastCatalogPosition[i] = lastCatalogPosition[i - 2];
        }
        lastCatalogPosition[15 /* MNU_ALLVAR */] = lastCatalogPosition[14 /* MNU_MVAR */] = 0;
      }
      else if(backupVersion < 98) { // add FNCS_EIM catalog
        restore(&lastCatalogPosition,              44);
        lastCatalogPosition[22 /* MNU_FNCS_EIM */]  = 0;
      }
      else {
        restore(&lastCatalogPosition,              sizeof(lastCatalogPosition));
      }
      restore(&lgCatalogSelection,                 sizeof(lgCatalogSelection));
      restore(displayValueX,                       sizeof(displayValueX));
      restore(&pcg32_global,                       sizeof(pcg32_global));
      restore(&exponentLimit,                      sizeof(exponentLimit));
      restore(&exponentHideLimit,                  sizeof(exponentHideLimit));
      restore(&keyActionProcessed,                 sizeof(keyActionProcessed));
      restore(&systemFlags,                        sizeof(systemFlags));
      restore(&savedSystemFlags,                   sizeof(savedSystemFlags));
      restore(&thereIsSomethingToUndo,             sizeof(thereIsSomethingToUndo));
      restore(&ramPtr,                             sizeof(ramPtr)); // beginOfProgramMemory pointer to block
      beginOfProgramMemory = TO_PCMEMPTR(ramPtr);
      restore(&ramPtr,                             sizeof(ramPtr)); // beginOfProgramMemory offset within block
      beginOfProgramMemory += ramPtr;
      restore(&ramPtr,                             sizeof(ramPtr)); // firstFreeProgramByte pointer to block
      firstFreeProgramByte = TO_PCMEMPTR(ramPtr);
      restore(&ramPtr,                             sizeof(ramPtr)); // firstFreeProgramByte offset within block
      firstFreeProgramByte += ramPtr;
      restore(&ramPtr,                             sizeof(ramPtr)); // firstDisplayedStep pointer to block
      firstDisplayedStep = TO_PCMEMPTR(ramPtr);
      restore(&ramPtr,                             sizeof(ramPtr)); // firstDisplayedStep offset within block
      firstDisplayedStep += ramPtr;
      restore(&ramPtr,                             sizeof(ramPtr)); // currentStep pointer to block
      currentStep = TO_PCMEMPTR(ramPtr);
      restore(&ramPtr,                             sizeof(ramPtr)); // currentStep offset within block
      currentStep += ramPtr;
      restore(&freeProgramBytes,                   sizeof(freeProgramBytes));
      restore(&firstDisplayedLocalStepNumber,      sizeof(firstDisplayedLocalStepNumber));
      restore(&numberOfLabels,                     sizeof(numberOfLabels));
      if(backupVersion < 91) { // numberOfLabelsInFlash
        restore(&numberOfPrograms,                 sizeof(numberOfPrograms));
      }
      restore(&numberOfPrograms,                   sizeof(numberOfPrograms));
      if(backupVersion < 91) { // numberOfProgramsInFlash
        restore(&currentLocalStepNumber,           sizeof(currentLocalStepNumber));
      }
      restore(&currentLocalStepNumber,             sizeof(currentLocalStepNumber));
      restore(&currentProgramNumber,               sizeof(currentProgramNumber));
      restore(&lastProgramListEnd,                 sizeof(lastProgramListEnd));
      restore(&programListEnd,                     sizeof(programListEnd));
      restore(&allSubroutineLevels,                sizeof(allSubroutineLevels));
      restore(&pemCursorIsZerothStep,              sizeof(pemCursorIsZerothStep));
      restore(&numberOfTamMenusToPop,              sizeof(numberOfTamMenusToPop));
      restore(&lrSelection,                        sizeof(lrSelection));
      restore(&lrSelectionUndo,                    sizeof(lrSelectionUndo));
      restore(&lrChosen,                           sizeof(lrChosen));
      restore(&lrChosenUndo,                       sizeof(lrChosenUndo));
      restore(&lastPlotMode,                       sizeof(lastPlotMode));
      restore(&plotSelection,                      sizeof(plotSelection));

      restore(&graph_dx,                           sizeof(graph_dx));
      restore(&graph_dy,                           sizeof(graph_dy));
      restore(&roundedTicks,                       sizeof(roundedTicks));
      restore(&extentx,                            sizeof(extentx));
      restore(&extenty,                            sizeof(extenty));
      restore(&PLOT_VECT,                          sizeof(PLOT_VECT));
      restore(&PLOT_NVECT,                         sizeof(PLOT_NVECT));
      restore(&PLOT_SCALE,                         sizeof(PLOT_SCALE));
      restore(&Aspect_Square,                      sizeof(Aspect_Square));
      restore(&PLOT_LINE,                          sizeof(PLOT_LINE));
      restore(&PLOT_CROSS,                         sizeof(PLOT_CROSS));
      restore(&PLOT_BOX,                           sizeof(PLOT_BOX));
      restore(&PLOT_INTG,                          sizeof(PLOT_INTG));
      restore(&PLOT_DIFF,                          sizeof(PLOT_DIFF));
      restore(&PLOT_RMS,                           sizeof(PLOT_RMS));
      restore(&PLOT_SHADE,                         sizeof(PLOT_SHADE));
      restore(&PLOT_AXIS,                          sizeof(PLOT_AXIS));
      restore(&PLOT_ZMX,                           sizeof(PLOT_ZMX));
      restore(&PLOT_ZMY,                           sizeof(PLOT_ZMY));
      restore(&PLOT_ZOOM,                          sizeof(PLOT_ZOOM));
      restore(&plotmode,                           sizeof(plotmode));
      restore(&tick_int_x,                         sizeof(tick_int_x));
      restore(&tick_int_y,                         sizeof(tick_int_y));
      restore(&x_min,                              sizeof(x_min));
      restore(&x_max,                              sizeof(x_max));
      restore(&y_min,                              sizeof(y_min));
      restore(&y_max,                              sizeof(y_max));
      restore(&xzero,                              sizeof(xzero));
      restore(&yzero,                              sizeof(yzero));
      restore(&matrixIndex,                        sizeof(matrixIndex));
      restore(&currentViewRegister,                sizeof(currentViewRegister));
      restore(&currentSolverStatus,                sizeof(currentSolverStatus));
      restore(&currentSolverProgram,               sizeof(currentSolverProgram));
      restore(&currentSolverVariable,              sizeof(currentSolverVariable));
      restore(&numberOfFormulae,                   sizeof(numberOfFormulae));
      restore(&currentFormula,                     sizeof(currentFormula));
      restore(&equationEditorCursor,               sizeof(equationEditorCursor));
      restore(&equationEditorScrollPosition,       sizeof(equationEditorScrollPosition));
      if(backupVersion >= 97) {  // cursor arrows
        restore(&alphaCursor,                      sizeof(alphaCursor));
      }
      restore(&numberOfUserMenus,                  sizeof(numberOfUserMenus));
      restore(&currentUserMenu,                    sizeof(currentUserMenu));
      restore(&userKeyLabelSize,                   sizeof(userKeyLabelSize));
      restore(&timerAppState,                      sizeof(timerAppState));
      restore(&currentInputVariable,               sizeof(currentInputVariable));
      restore(&SAVED_SIGMA_LASTX,                  sizeof(SAVED_SIGMA_LASTX));
      restore(&SAVED_SIGMA_LASTY,                  sizeof(SAVED_SIGMA_LASTY));
      restore(&SAVED_SIGMA_LAct,                   sizeof(SAVED_SIGMA_LAct));
      restore(&currentMvarLabel,                   sizeof(currentMvarLabel));
      restore(&graphVariable,                      sizeof(graphVariable));
      restore(&plotStatMx,                         sizeof(plotStatMx));
      restore(&drawHistogram,                      sizeof(drawHistogram));
      restore(&statMx,                             sizeof(statMx));
      restore(&lrSelectionHistobackup,             sizeof(lrSelectionHistobackup));
      restore(&lrChosenHistobackup,                sizeof(lrChosenHistobackup));
      restore(&loBinR,                             sizeof(loBinR));
      restore(&nBins ,                             sizeof(nBins ));
      restore(&hiBinR,                             sizeof(hiBinR));
      restore(&histElementXorY,                    sizeof(histElementXorY));

      restore(&screenUpdatingMode,                 sizeof(screenUpdatingMode));
      restore(loadedScreen,                        SCREEN_WIDTH * SCREEN_HEIGHT / 8);

      ioFileClose();
      printf("End of calc's restoration\n");fflush(stdout);

      if(calcMode == cmApp) {
        calcModeLeave();
        switch(currentApp) {
          case appRegisterBrowser: {
            fnRegisterBrowser(NOPARAM);
            break;
          }
          case appFlagBrowser: {
            fnFlagBrowser(NOPARAM);
            break;
          }
          case appFontBrowser: {
            fnFontBrowser(NOPARAM);
            break;
          }
          case appBugScreen: {
            bugScreen("In function restoreCalc: Error message that was here was not saved!");
            break;
          }
          default: {
            bugScreen("In function restoreCalc: unrecognized app ID!");
            break;
          }
        }
      }

      if(backupVersion <= 88) { // Program incompatibility for x→α
        int globalStep = 1;
        uint8_t *step = beginOfProgramMemory;
        printf("****Program binary incompatibility****\n");
        printf("x→α now followed by a destination register\n");
        printf("Loaded x→α in RAM will be replaced by NOP\n");
        printf("CAVEAT: x→α in Flash will not be replaced so it may cause crash\n");
        printf("Also SAVE data will not be reinterpreted so LOAD or LOADP may crash\n");
        while(!isAtEndOfPrograms(step)) { // .END.
          if(checkOpCodeOfStep(step, ITM_XtoALPHA)) { // x->alpha
            step[0] = (ITM_NOP >> 8) | 0x80;
            step[1] =  ITM_NOP       & 0xff;
            printf("x→α found at global step %d\n", globalStep);
          }
          ++globalStep;
          step = findNextStep(step);
        }
      }

      if(backupVersion <= 89) { // Angle incompatibility
        printf("****Angle code incompatibility****\n");
        printf("Angle code has been changed since Mil has been introduced\n");
        printf("real34 values now will be converted\n");
        printf("CAVEAT: local registers which are out of scope will not be converted\n");
        if(currentAngularMode >= 3) {
          ++currentAngularMode;
        }
        for(int16_t regist = 0; regist <= LAST_GLOBAL_REGISTER; ++regist) {
          if(getRegisterDataType(regist) == dtReal34 && getRegisterAngularMode(regist) >= 3) {
            setRegisterAngularMode(regist, getRegisterAngularMode(regist) + 1);
          }
        }
        for(int16_t regist = FIRST_LOCAL_REGISTER; regist < FIRST_LOCAL_REGISTER + currentNumberOfLocalRegisters; ++regist) {
          if(getRegisterDataType(regist) == dtReal34 && getRegisterAngularMode(regist) >= 3) {
            setRegisterAngularMode(regist, getRegisterAngularMode(regist) + 1);
          }
        }
        for(int16_t regist = FIRST_NAMED_VARIABLE; regist < FIRST_NAMED_VARIABLE + numberOfNamedVariables; ++regist) {
          if(getRegisterDataType(regist) == dtReal34 && getRegisterAngularMode(regist) >= 3) {
            setRegisterAngularMode(regist, getRegisterAngularMode(regist) + 1);
          }
        }
        for(int16_t regist = FIRST_RESERVED_VARIABLE + 12; regist <= LAST_RESERVED_VARIABLE; ++regist) {
          if(getRegisterDataType(regist) == dtReal34 && getRegisterAngularMode(regist) >= 3) {
            setRegisterAngularMode(regist, getRegisterAngularMode(regist) + 1);
          }
        }
      }

      if(backupVersion <= 95) { // Program incompatibility for MSG
        printf("****Program binary incompatibility****\n");
        printf("MSG now followed by a value\n");
        printf("Loaded MSG in programs will be replaced by NOP\n");fflush(stdout);
        replaceInstruction(beginOfProgramMemory, 1, ITM_MSG, ITM_NOP_REGISTER);
      }

      if(backupVersion <= 96) { // Cursor arrows added to Mya
        printf("**** Old version of Myalpha ****\n");
        printf("Myalpha includes now cursor arrows\n");fflush(stdout);
        sprintf(tmpString,"**** Old version of Myalpha ****\n"
                          "Myalpha includes now cursor "
                          "arrows. Use CLMyα to get the\n"
                          "new default menu.\n");
        #if !defined(TESTSUITE_BUILD)
          show_warning(tmpString);
        #endif // TESTSUITE_BUILD
      }

      scanLabelsAndPrograms();
      defineCurrentProgramFromGlobalStepNumber(currentLocalStepNumber + abs(programList[currentProgramNumber - 1].step) - 1);
      defineCurrentStep();
      defineFirstDisplayedStep();
      defineCurrentProgramFromCurrentStep();

      //defineCurrentLocalRegisters();

      if(temporaryInformation==TI_SHOW_REGISTER) {
        temporaryInformation = TI_NO_INFO;
      }

      #if (DEBUG_REGISTER_L == 1)
        refreshRegisterLine(REGISTER_X); // to show L register
      #endif // (DEBUG_REGISTER_L == 1)

      for(int y = 0; y < SCREEN_HEIGHT; ++y) {
        for(int x = 0; x < SCREEN_WIDTH; x += 8) {
          uint8_t bmpdata = *(loadedScreen + (y * SCREEN_WIDTH + x) / 8);
          for(int bit = 7; bit >= 0; --bit) {
            *(screenData + y * screenStride + x + (7 - bit)) = (bmpdata & (1 << bit)) ? ON_PIXEL : OFF_PIXEL;
          }
        }
      }
      free(loadedScreen);

      if(calcMode == cmNim || calcMode == cmAim) {
        cursorEnabled = true;
      }
      if(calcMode == cmMim) {
        mimRestore();
      }
      if(catalog) {
        clearSystemFlag(FLAG_ALPHA);
      }

      if(cursorEnabled) {
        cursorShow(cursorFont == &standardFont, xCursor, yCursor);
      }
      calcModeUpdateGui();
      updateMatrixHeightCache();
      cachedDynamicMenu = 0;
      refreshScreen();
    }
  }
#endif // PC_BUILD


char aimBuffer1[400];             //The concurrent use of the global aimBuffer
                                  //does not work. See tmpString.
                                  //Temporary solution is to use a local variable of sufficient length for the target.

#if !defined(TESTSUITE_BUILD)
  static void UI64toString(uint64_t value, char * tmpRegisterString);
  static void registerToSaveString(calcRegister_t regist) {
    longInteger_t lgInt;
    int16_t sign;
    uint64_t value;
    uint32_t base;
    char *str;
    uint8_t *cfg;

  tmpRegisterString = tmpString + START_REGISTER_VALUE;

    switch(getRegisterDataType(regist)) {
      case dtLongInteger: {
        convertLongIntegerRegisterToLongInteger(regist, lgInt);
        longIntegerToAllocatedString(lgInt, tmpRegisterString, TMP_STR_LENGTH - START_REGISTER_VALUE - 1);
        longIntegerFree(lgInt);
        strcpy(aimBuffer1, "LonI");
        break;
      }

      case dtString: {
        stringToUtf8(REGISTER_STRING_DATA(regist), (uint8_t *)(tmpRegisterString));
        strcpy(aimBuffer1, "Stri");
        break;
      }

      case dtShortInteger: {
        convertShortIntegerRegisterToUInt64(regist, &sign, &value);
        base = getRegisterShortIntegerBase(regist);

        char yy[25];
        UI64toString(value, yy);
        sprintf(tmpRegisterString, "%c%s %" PRIu32, sign ? '-' : '+', yy, base);
        strcpy(aimBuffer1, "ShoI");
        break;
      }

      case dtReal34: {
        real34ToString(REGISTER_REAL34_DATA(regist), tmpRegisterString);
        switch(getRegisterAngularMode(regist)) {
          case amDegree: {
            strcpy(aimBuffer1, "Real:DEG");
            break;
          }

          case amDMS: {
            strcpy(aimBuffer1, "Real:DMS");
            break;
          }

          case amRadian: {
            strcpy(aimBuffer1, "Real:RAD");
            break;
          }

          case amMultPi: {
            strcpy(aimBuffer1, "Real:MULTPI");
            break;
          }

          case amGrad: {
            strcpy(aimBuffer1, "Real:GRAD");
            break;
          }

          case amMil: {
            strcpy(aimBuffer1, "Real:MIL");
            break;
          }

          case amNone: {
            strcpy(aimBuffer1, "Real");
            break;
          }

          default: {
            strcpy(aimBuffer1, "Real:???");
            break;
          }
        }
        break;
      }

      case dtComplex34: {
        real34ToString(REGISTER_REAL34_DATA(regist), tmpRegisterString);
        strcat(tmpRegisterString, " ");
        real34ToString(REGISTER_IMAG34_DATA(regist), tmpRegisterString + strlen(tmpRegisterString));
        strcpy(aimBuffer1, "Cplx");
        break;
      }

      case dtTime: {
        real34ToString(REGISTER_REAL34_DATA(regist), tmpRegisterString);
        strcpy(aimBuffer1, "Time");
        break;
      }

      case dtDate: {
        real34ToString(REGISTER_REAL34_DATA(regist), tmpRegisterString);
        strcpy(aimBuffer1, "Date");
        break;
      }

      case dtReal34Matrix: {
        sprintf(tmpRegisterString, "%" PRIu16 " %" PRIu16, REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixRows, REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixColumns);
        strcpy(aimBuffer1, "Rema");
        break;
      }

      case dtComplex34Matrix: {
        sprintf(tmpRegisterString, "%" PRIu16 " %" PRIu16, REGISTER_COMPLEX34_MATRIX_DBLOCK(regist)->matrixRows, REGISTER_COMPLEX34_MATRIX_DBLOCK(regist)->matrixColumns);
        strcpy(aimBuffer1, "Cxma");
        break;
      }

      case dtConfig: {
        for(str=tmpRegisterString, cfg=(uint8_t *)REGISTER_CONFIG_DATA(regist), value=0; value<sizeof(dtConfigDescriptor_t); value++, cfg++, str+=2) {
          sprintf(str, "%02X", *cfg);
        }
        strcpy(aimBuffer1, "Conf");
        break;
        break;
      }

      default: {
        strcpy(tmpRegisterString, "???");
        strcpy(aimBuffer1, "????");
      }
    }
  }



static void saveMatrixElements(calcRegister_t regist) {
    if(getRegisterDataType(regist) == dtReal34Matrix) {
      for(uint32_t element = 0; element < REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixRows * REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixColumns; ++element) {
        real34ToString(REGISTER_REAL34_MATRIX_M_ELEMENTS(regist) + element, tmpString);
        strcat(tmpString, "\n");
        save(tmpString, strlen(tmpString));
      }
    }
    else if(getRegisterDataType(regist) == dtComplex34Matrix) {
      for(uint32_t element = 0; element < REGISTER_COMPLEX34_MATRIX_DBLOCK(regist)->matrixRows * REGISTER_COMPLEX34_MATRIX_DBLOCK(regist)->matrixColumns; ++element) {
        real34ToString(VARIABLE_REAL34_DATA(REGISTER_COMPLEX34_MATRIX_M_ELEMENTS(regist) + element), tmpString);
        strcat(tmpString, " ");
        real34ToString(VARIABLE_IMAG34_DATA(REGISTER_COMPLEX34_MATRIX_M_ELEMENTS(regist) + element), tmpString + strlen(tmpString));
        strcat(tmpString, "\n");
        save(tmpString, strlen(tmpString));
      }
    }
}
#endif // !TESTSUITE_BUILD



void fnSave(uint16_t saveMode) {
#if !defined(TESTSUITE_BUILD)
  uint32_t backupVersion = BACKUP_VERSION;
  ioFilePath_t path;
  char tmpString[3000];           //The concurrent use of the global tmpString
                                  //as target does not work while the source is at
                                  //tmpRegisterString = tmpString + START_REGISTER_VALUE;
                                  //Temporary solution is to use a local variable of sufficient length for the target.
  int ret;

#if defined(DMCP_BUILD)
  // Don't pass through if the power is insufficient
  if ( power_check_screen() ) return;
#endif

  calcRegister_t regist;
  uint32_t i;
  char yy1[35], yy2[35];

#if defined(MERGED_SAVE_SAVEST)
  if(saveMode >> 8 == FILE_NONE) {
    saveLoadMode = saveMode;
    temporaryInformation = TI_SAVE_TO;
    setFileType(fnSave);
    return;
  }

  if (saveMode >> 8 == FILE_BACKUP) {
    saveMode = SM_BACKUP;
  }
  else {
    saveMode = SM_STATE_FILE;
  }
#endif // MERGED_SAVE_SAVEST

  if (saveMode == SM_BACKUP) {
    path = ioPathSaveFile;
  }
  else {
    path = ioPathSaveStateFile;
  }

  ret = ioFileOpen(path, ioModeWrite);

  if(ret != FILE_OK ) {
    if(ret == FILE_CANCEL ) {
      return;
    } else {
      #if !defined(DMCP_BUILD)
        printf("Cannot SAVE in file wp43.sav!\n");
      #endif
      displayCalcErrorMessage(ERROR_CANNOT_WRITE_FILE, ERR_REGISTER_LINE, REGISTER_X);
      return;
    }
  }

  // SAVE file version
  sprintf(tmpString, "SAVE_FILE_REVISION\n%" PRIu8 "\n", (uint8_t)0);
  save(tmpString, strlen(tmpString));
  sprintf(tmpString, "WP43_save_file_00\n%" PRIu32 "\n", (uint32_t)backupVersion);
  save(tmpString, strlen(tmpString));

  // Global registers
  sprintf(tmpString, "GLOBAL_REGISTERS\n%" PRIu16 "\n", (uint16_t)(FIRST_LOCAL_REGISTER));
  save(tmpString, strlen(tmpString));
  for(regist=0; regist<FIRST_LOCAL_REGISTER; regist++) {
    registerToSaveString(regist);
    sprintf(tmpString, "R%03" PRId16 "\n%s\n%s\n", regist, aimBuffer1, tmpRegisterString);
    save(tmpString, strlen(tmpString));
    saveMatrixElements(regist);
  }

  // Global flags
  strcpy(tmpString, "GLOBAL_FLAGS\n");
  save(tmpString, strlen(tmpString));
  sprintf(tmpString, "%" PRIu16 " %" PRIu16 " %" PRIu16 " %" PRIu16 " %" PRIu16 " %" PRIu16 " %" PRIu16 "\n",
                       globalFlags[0],
                                   globalFlags[1],
                                               globalFlags[2],
                                                           globalFlags[3],
                                                                       globalFlags[4],
                                                                                   globalFlags[5],
                                                                                               globalFlags[6]);
  save(tmpString, strlen(tmpString));

  // Local registers
  sprintf(tmpString, "LOCAL_REGISTERS\n%" PRIu8 "\n", currentNumberOfLocalRegisters);
  save(tmpString, strlen(tmpString));
  for(i=0; i<currentNumberOfLocalRegisters; i++) {
    registerToSaveString(FIRST_LOCAL_REGISTER + i);
    sprintf(tmpString, "R.%02" PRIu32 "\n%s\n%s\n", i, aimBuffer1, tmpRegisterString);
    save(tmpString, strlen(tmpString));
    saveMatrixElements(FIRST_LOCAL_REGISTER + i);
  }

  // Local flags
  if(currentLocalRegisters) {
    sprintf(tmpString, "LOCAL_FLAGS\n%" PRIu32 "\n", currentLocalFlags->localFlags);
    save(tmpString, strlen(tmpString));
  }

  // Named variables
  sprintf(tmpString, "NAMED_VARIABLES\n%" PRIu16 "\n", numberOfNamedVariables);
  save(tmpString, strlen(tmpString));
  for(i=0; i<numberOfNamedVariables; i++) {
    registerToSaveString(FIRST_NAMED_VARIABLE + i);
    stringToUtf8((char *)allNamedVariables[i].variableName + 1, (uint8_t *)tmpString);
    sprintf(tmpString + strlen(tmpString), "\n%s\n%s\n", aimBuffer1, tmpRegisterString);
    save(tmpString, strlen(tmpString));
    saveMatrixElements(FIRST_NAMED_VARIABLE + i);
  }

  // Statistical sums
  sprintf(tmpString, "STATISTICAL_SUMS\n%" PRIu16 "\n", (uint16_t)(statisticalSumsPointer ? NUMBER_OF_STATISTICAL_SUMS : 0));
  save(tmpString, strlen(tmpString));
  for(i=0; i<(statisticalSumsPointer ? NUMBER_OF_STATISTICAL_SUMS : 0); i++) {
    realToString((real_t *)(statisticalSumsPointer + TO_BLOCKS(REAL_SIZE_IN_BYTES) * i), tmpRegisterString);
    sprintf(tmpString, "%s\n", tmpRegisterString);
    save(tmpString, strlen(tmpString));
  }

  // System flags
  UI64toString(systemFlags, yy1);
  sprintf(tmpString, "SYSTEM_FLAGS\n%s\n", yy1);
  save(tmpString, strlen(tmpString));

  // Keyboard assignments
  sprintf(tmpString, "KEYBOARD_ASSIGNMENTS\n37\n");
  save(tmpString, strlen(tmpString));
  for(i=0; i<37; i++) {
    sprintf(tmpString, "%" PRId16 " %" PRId16 " %" PRId16 " %" PRId16 " %" PRId16 " %" PRId16 " %" PRId16 " %" PRId16 " %" PRId16 "\n",
                         kbd_usr[i].keyId,
                                     kbd_usr[i].primary,
                                                 kbd_usr[i].fShifted,
                                                             kbd_usr[i].gShifted,
                                                                         kbd_usr[i].keyLblAim,
                                                                                     kbd_usr[i].primaryAim,
                                                                                                 kbd_usr[i].fShiftedAim,
                                                                                                             kbd_usr[i].gShiftedAim,
                                                                                                                         kbd_usr[i].primaryTam);
  save(tmpString, strlen(tmpString));
  }

  // Keyboard arguments
  sprintf(tmpString, "KEYBOARD_ARGUMENTS\n");
  save(tmpString, strlen(tmpString));

  uint32_t num = 0;
  for(i = 0; i < 37 * 6; ++i) {
    if(*(getNthString((uint8_t *)userKeyLabel, i)) != 0) {
      ++num;
    }
  }
  sprintf(tmpString, "%" PRIu32 "\n", num);
  save(tmpString, strlen(tmpString));

  for(i = 0; i < 37 * 6; ++i) {
    if(*(getNthString((uint8_t *)userKeyLabel, i)) != 0) {
      sprintf(tmpString, "%" PRIu32 " ", i);
      stringToUtf8((char *)getNthString((uint8_t *)userKeyLabel, i), (uint8_t *)tmpString + strlen(tmpString));
      strcat(tmpString, "\n");
      save(tmpString, strlen(tmpString));
    }
  }

  // MyMenu
  sprintf(tmpString, "MYMENU\n18\n");
  save(tmpString, strlen(tmpString));
  for(i=0; i<18; i++) {
    sprintf(tmpString, "%" PRId16, userMenuItems[i].item);
    if(userMenuItems[i].argumentName[0] != 0) {
      strcat(tmpString, " ");
      stringToUtf8(userMenuItems[i].argumentName, (uint8_t *)tmpString + strlen(tmpString));
    }
    strcat(tmpString, "\n");
    save(tmpString, strlen(tmpString));
  }

  // MyAlpha
  sprintf(tmpString, "MYALPHA\n18\n");
  save(tmpString, strlen(tmpString));
  for(i=0; i<18; i++) {
    sprintf(tmpString, "%" PRId16, userAlphaItems[i].item);
    if(userAlphaItems[i].argumentName[0] != 0) {
      strcat(tmpString, " ");
      stringToUtf8(userAlphaItems[i].argumentName, (uint8_t *)tmpString + strlen(tmpString));
    }
    strcat(tmpString, "\n");
    save(tmpString, strlen(tmpString));
  }

  // MyPFN
  sprintf(tmpString, "MYPFN\n18\n");
  save(tmpString, strlen(tmpString));
  for(i=0; i<18; i++) {
    sprintf(tmpString, "%" PRId16, userPfnItems[i].item);
    if(userPfnItems[i].argumentName[0] != 0) {
      strcat(tmpString, " ");
      stringToUtf8(userPfnItems[i].argumentName, (uint8_t *)tmpString + strlen(tmpString));
    }
    strcat(tmpString, "\n");
    save(tmpString, strlen(tmpString));
  }

  // User menus
  sprintf(tmpString, "USER_MENUS\n");
  save(tmpString, strlen(tmpString));
  sprintf(tmpString, "%" PRIu16 "\n", numberOfUserMenus);
  save(tmpString, strlen(tmpString));
  for(uint32_t j = 0; j < numberOfUserMenus; ++j) {
    stringToUtf8(userMenus[j].menuName, (uint8_t *)tmpString);
    strcat(tmpString, "\n18\n");
    save(tmpString, strlen(tmpString));
    for(i=0; i<18; i++) {
      sprintf(tmpString, "%" PRId16, userMenus[j].menuItem[i].item);
      if(userMenus[j].menuItem[i].argumentName[0] != 0) {
        strcat(tmpString, " ");
        stringToUtf8(userMenus[j].menuItem[i].argumentName, (uint8_t *)tmpString + strlen(tmpString));
      }
      strcat(tmpString, "\n");
      save(tmpString, strlen(tmpString));
    }
  }

  // Programs
  size_t currentSizeInBytes = TO_BYTES(RAM_SIZE_IN_BLOCKS - TO_WP43MEMPTR(beginOfProgramMemory));
  sprintf(tmpString, "PROGRAMS\n%" PRIu32 "\n", (uint32_t)currentSizeInBytes);
  save(tmpString, strlen(tmpString));

  sprintf(tmpString, "%" PRIu32 "\n%" PRIu32 "\n", (uint32_t)TO_WP43MEMPTR(currentStep), (uint32_t)((void *)currentStep - TO_PCMEMPTR(TO_WP43MEMPTR(currentStep)))); // currentStep block pointer + offset within block
  save(tmpString, strlen(tmpString));

  sprintf(tmpString, "%" PRIu32 "\n%" PRIu32 "\n", (uint32_t)TO_WP43MEMPTR(firstFreeProgramByte), (uint32_t)((void *)firstFreeProgramByte - TO_PCMEMPTR(TO_WP43MEMPTR(firstFreeProgramByte)))); // firstFreeProgramByte block pointer + offset within block
  save(tmpString, strlen(tmpString));

  sprintf(tmpString, "%" PRIu16 "\n", freeProgramBytes);
  save(tmpString, strlen(tmpString));

  for(i=0; i<currentSizeInBytes; i++) {
    sprintf(tmpString, "%" PRIu8 "\n", beginOfProgramMemory[i]);
    save(tmpString, strlen(tmpString));
  }

  // Equations
  sprintf(tmpString, "EQUATIONS\n%" PRIu16 "\n", numberOfFormulae);
  save(tmpString, strlen(tmpString));

  for(i=0; i<numberOfFormulae; i++) {
    stringToUtf8(TO_PCMEMPTR(allFormulae[i].pointerToFormulaData), (uint8_t *)tmpString);
    strcat(tmpString, "\n");
    save(tmpString, strlen(tmpString));
  }

  // Other configuration stuff
  sprintf(tmpString, "OTHER_CONFIGURATION_STUFF\n16\n");
  save(tmpString, strlen(tmpString));
  sprintf(tmpString, "firstGregorianDay\n%" PRIu32 "\n", firstGregorianDay);
  save(tmpString, strlen(tmpString));
  sprintf(tmpString, "denMax\n%" PRIu32 "\n", denMax);
  save(tmpString, strlen(tmpString));
  sprintf(tmpString, "lastDenominator\n%" PRIu32 "\n", lastDenominator);
  save(tmpString, strlen(tmpString));
  sprintf(tmpString, "displayFormat\n%" PRIu8 "\n", (uint8_t)displayFormat);
  save(tmpString, strlen(tmpString));
  sprintf(tmpString, "displayFormatDigits\n%" PRIu8 "\n", displayFormatDigits);
  save(tmpString, strlen(tmpString));
  sprintf(tmpString, "timeDisplayFormatDigits\n%" PRIu8 "\n", timeDisplayFormatDigits);
  save(tmpString, strlen(tmpString));
  sprintf(tmpString, "shortIntegerWordSize\n%" PRIu8 "\n", shortIntegerWordSize);
  save(tmpString, strlen(tmpString));
  sprintf(tmpString, "shortIntegerMode\n%" PRIu8 "\n", shortIntegerMode);
  save(tmpString, strlen(tmpString));
  sprintf(tmpString, "significantDigits\n%" PRIu8 "\n", significantDigits);
  save(tmpString, strlen(tmpString));
  sprintf(tmpString, "currentAngularMode\n%" PRIu8 "\n", (uint8_t)currentAngularMode);
  save(tmpString, strlen(tmpString));
  sprintf(tmpString, "groupingGap\n%" PRIu8 "\n", groupingGap);
  save(tmpString, strlen(tmpString));
  sprintf(tmpString, "roundingMode\n%" PRIu8 "\n", (uint8_t)roundingMode);
  save(tmpString, strlen(tmpString));
  sprintf(tmpString, "displayStack\n%" PRIu8 "\n", displayStack);
  save(tmpString, strlen(tmpString));
  UI64toString(pcg32_global.state, yy1);
  UI64toString(pcg32_global.inc, yy2);
  sprintf(tmpString, "rngState\n%s %s\n", yy1, yy2);
  save(tmpString, strlen(tmpString));
  sprintf(tmpString, "exponentLimit\n%" PRId16 "\n", exponentLimit);
  save(tmpString, strlen(tmpString));
  sprintf(tmpString, "exponentHideLimit\n%" PRId16 "\n", exponentHideLimit);
  save(tmpString, strlen(tmpString));
  sprintf(tmpString, "notBestF\n%" PRIu16 "\n", lrSelection);
  save(tmpString, strlen(tmpString));

  ioFileClose();

  temporaryInformation = TI_SAVED;
#endif // !TESTSUITE_BUILD
}



void readLine(char *line) {
  if(!ioEof()) {
    restore(line, 1);
    while((*line == '\n' || *line == '\r') && !ioEof()) {
      restore(line, 1);
    }

    while(*line != '\n' && *line != '\r' && !ioEof()) {
      restore(++line, 1);
    }
  }

  *line = 0;
}

void read2Lines(char *line1, char *line2) {  // Needed to capture empty lines due to empty strings saved from registers
  char eol1,eol2;

  if(!ioEof()) {
    restore(line1, 1);
    while((*line1 == '\n' || *line1 == '\r') && !ioEof()) {
      restore(line1, 1);
    }

    while(*line1 != '\n' && *line1 != '\r' && !ioEof()) {
      restore(++line1, 1);
    }
  }
  eol1 = *line1;
  *line1 = 0;

  if(!ioEof()) {
    restore(line2, 1);
    eol2 = *line2;
    if ((((eol1 == '\n') && (eol2 ==  '\n')) || ((eol1 == '\r') && (eol2 ==  '\r'))) && !ioEof()) {   // empty string between two CR or two LF
      *line2 = 0;
      return;
    }
    if ((((eol1 == '\r') && (eol2 ==  '\n')) || ((eol1 == '\n') && (eol2 ==  '\r'))) && !ioEof()) {   // end line is CRLF or LFCR
      restore(line2, 1);
      if (((*line2 == '\n') || (*line2 == '\r')) && !ioEof()) {     // empty string after CRLF or LFCR
        *line2 = 0;
        return;
      }
    }

    while(*line2 != '\n' && *line2 != '\r' && !ioEof()) {
      restore(++line2, 1);
    }
  }

  *line2 = 0;
}



#ifndef TESTSUITE_BUILD
static void UI64toString(uint64_t value, char * tmpRegisterString) {
  uint32_t v0,v1,v2;
  v2 =  value /      1000000000000000000;
  v1 = (value - v2 * 1000000000000000000) /      1000000000;
  v0 = (value - v2 * 1000000000000000000) - v1 * 1000000000;
  if(v2 == 0 && v1 == 0) {
    sprintf(tmpRegisterString,"%" PRIu32, v0);
  } else if (v2 == 0) {
    sprintf(tmpRegisterString,"%" PRIu32 "%" PRIu32, v1,v0);
  } else {
    sprintf(tmpRegisterString,"%" PRIu32 "%" PRIu32 "%" PRIu32, v2,v1,v0);
  }
}
#endif //TESTSUITE_BUILD



uint8_t stringToUint8(const char *str) {
  uint8_t value = 0;


  while('0' <= *str && *str <= '9') {
    value = value*10 + (*(str++) - '0');
  }

  return value;
}



uint16_t stringToUint16(const char *str) {
  uint16_t value = 0;


  while('0' <= *str && *str <= '9') {
    value = value*10 + (*(str++) - '0');
  }

  return value;
}



uint32_t stringToUint32(const char *str) {
  uint32_t value = 0;

  while('0' <= *str && *str <= '9') {
    value = value*10 + (*(str++) - '0');
  }

  return value;
}



uint64_t stringToUint64(const char *str) {
  uint64_t value = 0;

  while('0' <= *str && *str <= '9') {
    value = value*10 + (*(str++) - '0');
  }

  return value;
}



int16_t stringToInt16(const char *str) {
  int16_t value = 0;
  bool    sign = false;

  if(*str == '-') {
    str++;
    sign = true;
  }
  else if(*str == '+') {
    str++;
  }

  while('0' <= *str && *str <= '9') {
    value = value*10 + (*(str++) - '0');
  }

  if(sign) {
    value = -value;
  }
  return value;
}



int32_t stringToInt32(const char *str) {
  int32_t value = 0;
  bool    sign = false;

  if(*str == '-') {
    str++;
    sign = true;
  }
  else if(*str == '+') {
    str++;
  }

  while('0' <= *str && *str <= '9') {
    value = value*10 + (*(str++) - '0');
  }

  if(sign) {
    value = -value;
  }
  return value;
}



static void restoreRegister(calcRegister_t regist, char *type, char *value) {
  uint32_t tag = amNone;

  if(type[4] == ':') {
    if(type[5] == 'R') {
      tag = amRadian;
    }
    else if(type[5] == 'M' && type[6] == 'U') {
      tag = amMultPi;
    }
    else if(type[5] == 'G') {
      tag = amGrad;
    }
    else if(type[5] == 'D' && type[6] == 'E') {
      tag = amDegree;
    }
    else if(type[5] == 'D' && type[6] == 'M') {
      tag = amDMS;
    }
    else if(type[5] == 'M' && type[6] == 'I') {
      tag = amMil;
    }
    else {
      tag = amNone;
    }

    reallocateRegister(regist, dtReal34, REAL34_SIZE_IN_BYTES, tag);
    stringToReal34(value, REGISTER_REAL34_DATA(regist));
  }

  else if(strcmp(type, "Real") == 0) {
    reallocateRegister(regist, dtReal34, REAL34_SIZE_IN_BYTES, tag);
    stringToReal34(value, REGISTER_REAL34_DATA(regist));
  }

  else if(strcmp(type, "Time") == 0) {
    reallocateRegister(regist, dtTime, REAL34_SIZE_IN_BYTES, amNone);
    stringToReal34(value, REGISTER_REAL34_DATA(regist));
  }

  else if(strcmp(type, "Date") == 0) {
    reallocateRegister(regist, dtDate, REAL34_SIZE_IN_BYTES, amNone);
    stringToReal34(value, REGISTER_REAL34_DATA(regist));
  }

  else if(strcmp(type, "LonI") == 0) {
    longInteger_t lgInt;

    longIntegerInit(lgInt);
    stringToLongInteger(value, 10, lgInt);
    convertLongIntegerToLongIntegerRegister(lgInt, regist);
    longIntegerFree(lgInt);
  }

  else if(strcmp(type, "Stri") == 0) {
    int32_t lenInBytes;

    utf8ToString((uint8_t *)value, errorMessage);
    lenInBytes = stringByteLength(errorMessage) + 1;
    reallocateRegister(regist, dtString, lenInBytes, amNone);
    xcopy(REGISTER_STRING_DATA(regist), errorMessage, lenInBytes);
  }

  else if(strcmp(type, "ShoI") == 0) {
    uint16_t sign = (value[0] == '-' ? 1 : 0);
    uint64_t val  = stringToUint64(value + 1);

    while(*value != ' ') {
      value++;
    }
    while(*value == ' ') {
      value++;
    }
    uint32_t base = stringToUint32(value);

    convertUInt64ToShortIntegerRegister(sign, val, base, regist);
  }

  else if(strcmp(type, "Cplx") == 0) {
    char *imaginaryPart;

    reallocateRegister(regist, dtComplex34, COMPLEX34_SIZE_IN_BYTES, amNone);
    imaginaryPart = value;
    while(*imaginaryPart != ' ') {
      imaginaryPart++;
    }
    *(imaginaryPart++) = 0;
    stringToReal34(value, REGISTER_REAL34_DATA(regist));
    stringToReal34(imaginaryPart, REGISTER_IMAG34_DATA(regist));
  }

#if !defined(TESTSUITE_BUILD)
  else if(strcmp(type, "Rema") == 0) {
    char *numOfCols;
    uint16_t rows, cols;

    numOfCols = value;
    while(*numOfCols != ' ') {
      numOfCols++;
    }
    *(numOfCols++) = 0;
    rows = stringToUint16(value);
    cols = stringToUint16(numOfCols);
    reallocateRegister(regist, dtReal34Matrix, REAL34_SIZE_IN_BYTES * rows * cols, amNone);
    REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixRows = rows;
    REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixColumns = cols;
  }

  else if(strcmp(type, "Cxma") == 0) {
    char *numOfCols;
    uint16_t rows, cols;

    numOfCols = value;
    while(*numOfCols != ' ') {
      numOfCols++;
    }
    *(numOfCols++) = 0;
    rows = stringToUint16(value);
    cols = stringToUint16(numOfCols);
    reallocateRegister(regist, dtComplex34Matrix, COMPLEX34_SIZE_IN_BYTES * rows * cols, amNone);
    REGISTER_COMPLEX34_MATRIX_DBLOCK(regist)->matrixRows = rows;
    REGISTER_COMPLEX34_MATRIX_DBLOCK(regist)->matrixColumns = cols;
  }
#endif // TESTSUITE_BUILD

  else if(strcmp(type, "Conf") == 0) {
    char *cfg;

    reallocateRegister(regist, dtConfig, CONFIG_SIZE_IN_BYTES, amNone);
    for(cfg=(char *)REGISTER_CONFIG_DATA(regist), tag=0; tag<sizeof(dtConfigDescriptor_t); tag++, value+=2, cfg++) {
      *cfg = ((*value >= 'A' ? *value - 'A' + 10 : *value - '0') << 4) | (*(value + 1) >= 'A' ? *(value + 1) - 'A' + 10 : *(value + 1) - '0');
    }
  }

  else {
    sprintf(errorMessage, "In function restoreRegister: Date type %s is to be coded!", type);
    bugScreen(errorMessage);
  }
}



static void restoreMatrixData(calcRegister_t regist) {
  #if !defined(TESTSUITE_BUILD)
    uint16_t rows, cols;
    uint32_t i;

    if(getRegisterDataType(regist) == dtReal34Matrix) {
      rows = REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixRows;
      cols = REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixColumns;

      for(i = 0; i < rows * cols; ++i) {
        readLine(tmpString);
        stringToReal34(tmpString, REGISTER_REAL34_MATRIX_M_ELEMENTS(regist) + i);
      }
    }

    if(getRegisterDataType(regist) == dtComplex34Matrix) {
      rows = REGISTER_COMPLEX34_MATRIX_DBLOCK(regist)->matrixRows;
      cols = REGISTER_COMPLEX34_MATRIX_DBLOCK(regist)->matrixColumns;

      for(i = 0; i < rows * cols; ++i) {
        char *imaginaryPart;

        readLine(tmpString);
        imaginaryPart = tmpString;
        while(*imaginaryPart != ' ') {
          imaginaryPart++;
        }
        *(imaginaryPart++) = 0;
        stringToReal34(tmpString,     VARIABLE_REAL34_DATA(REGISTER_COMPLEX34_MATRIX_M_ELEMENTS(regist) + i));
        stringToReal34(imaginaryPart, VARIABLE_IMAG34_DATA(REGISTER_COMPLEX34_MATRIX_M_ELEMENTS(regist) + i));
      }
    }
  #endif // !TESTSUITE_BUILD
}



static void skipMatrixData(char *type, char *value) {
  #if !defined(TESTSUITE_BUILD)
    uint16_t rows, cols;
    uint32_t i;
    char *numOfCols;

    if(strcmp(type, "Rema") == 0 || strcmp(type, "Cxma") == 0) {
      numOfCols = value;
      while(*numOfCols != ' ') {
        numOfCols++;
      }
      *(numOfCols++) = 0;
      rows = stringToUint16(value);
      cols = stringToUint16(numOfCols);

      for(i = 0; i < rows * cols; ++i) {
        readLine(tmpString);
      }
    }
  #endif // !TESTSUITE_BUILD
}



static bool restoreOneSection(uint16_t loadMode, uint16_t s, uint16_t n, uint16_t d) {
  int16_t numberOfRegs;
  calcRegister_t regist;
  char *str;

  readLine(tmpString);

  if(strcmp(tmpString, "GLOBAL_REGISTERS") == 0) {
    readLine(tmpString); // Number of global registers
    numberOfRegs = stringToInt16(tmpString);
    for(int16_t i=0; i<numberOfRegs; i++) {
      readLine(tmpString); // Register number
      regist = stringToInt16(tmpString + 1);
      read2Lines(aimBuffer,tmpString); // Register data type & Register value

      if(loadMode == LM_ALL || (loadMode == LM_REGISTERS) || (loadMode == LM_REGISTERS_PARTIAL && regist >= s && regist < (s + n))) {
        restoreRegister(loadMode == LM_REGISTERS_PARTIAL ? (regist - s + d) : regist, aimBuffer, tmpString);
        restoreMatrixData(loadMode == LM_REGISTERS_PARTIAL ? (regist - s + d) : regist);
      }
      else {
        skipMatrixData(aimBuffer, tmpString);
      }
    }
  }

  else if(strcmp(tmpString, "GLOBAL_FLAGS") == 0) {
    readLine(tmpString); // Global flags
    if(loadMode == LM_ALL || loadMode == LM_SYSTEM_STATE) {
      str = tmpString;
      globalFlags[0] = stringToInt16(str);

      while(*str != ' ') {
        str++;
      }
      while(*str == ' ') {
        str++;
      }
      globalFlags[1] = stringToInt16(str);

      while(*str != ' ') {
        str++;
      }
      while(*str == ' ') {
        str++;
      }
      globalFlags[2] = stringToInt16(str);

      while(*str != ' ') {
        str++;
      }
      while(*str == ' ') {
        str++;
      }
      globalFlags[3] = stringToInt16(str);

      while(*str != ' ') {
        str++;
      }
      while(*str == ' ') {
        str++;
      }
      globalFlags[4] = stringToInt16(str);

      while(*str != ' ') {
        str++;
      }
      while(*str == ' ') {
        str++;
      }
      globalFlags[5] = stringToInt16(str);

      while(*str != ' ') {
        str++;
      }
      while(*str == ' ') {
        str++;
      }
      globalFlags[6] = stringToInt16(str);
    }
  }

  else if(strcmp(tmpString, "LOCAL_REGISTERS") == 0) {
    readLine(tmpString); // Number of local registers
    numberOfRegs = stringToInt16(tmpString);
    if(loadMode == LM_ALL || loadMode == LM_REGISTERS) {
      if(numberOfRegs == 0) {
        popAllLocalRegistersAndFlags(NOPARAM);
      }
      else {
        allocateLocalRegisters(numberOfRegs);
      }
    }

    if((loadMode != LM_ALL && loadMode != LM_REGISTERS) || lastErrorCode == ERROR_NONE) {
      for(int16_t i=0; i<numberOfRegs; i++) {
        readLine(tmpString); // Register number
        regist = stringToInt16(tmpString + 2) + FIRST_LOCAL_REGISTER;
        read2Lines(aimBuffer,tmpString); // Register data type & Register value

        if(loadMode == LM_ALL || loadMode == LM_REGISTERS) {
          restoreRegister(regist, aimBuffer, tmpString);
          restoreMatrixData(regist);
        }
        else {
          skipMatrixData(aimBuffer, tmpString);
        }
      }
    }
  }

  else if(strcmp(tmpString, "LOCAL_FLAGS") == 0) {
    readLine(tmpString); // LOCAL_FLAGS
    if((loadMode == LM_ALL || loadMode == LM_REGISTERS) && (currentNumberOfLocalFlags > 0)) {
      currentLocalFlags->localFlags = stringToUint32(tmpString);
    }
  }

  else if(strcmp(tmpString, "NAMED_VARIABLES") == 0) {
    readLine(tmpString); // Number of named variables
    numberOfRegs = stringToInt16(tmpString);
    for(int16_t i=0; i<numberOfRegs; i++) {
      readLine(errorMessage); // Variable name
      read2Lines(aimBuffer,tmpString); // Variable data type & Variable value

      if(loadMode == LM_ALL || loadMode == LM_NAMED_VARIABLES ||
        (loadMode == LM_SUMS && ((strcmp(errorMessage, "STATS") == 0) || (strcmp(errorMessage, "HISTO") == 0)))) {
        char *varName = errorMessage + strlen(errorMessage) + 1;
        utf8ToString((uint8_t *)errorMessage, varName);
        regist = findOrAllocateNamedVariable(varName);
        if(regist != INVALID_VARIABLE) {
          restoreRegister(regist, aimBuffer, tmpString);
          restoreMatrixData(regist);
        }
        else {
          skipMatrixData(aimBuffer, tmpString);
        }
      }
      else {
        skipMatrixData(aimBuffer, tmpString);
      }
    }
  }

  else if(strcmp(tmpString, "STATISTICAL_SUMS") == 0) {
    readLine(tmpString); // Number of statistical sums
    numberOfRegs = stringToInt16(tmpString);
    if(numberOfRegs > 0 && (loadMode == LM_ALL || loadMode == LM_SUMS)) {
      initStatisticalSums();

      for(int16_t i=0; i<numberOfRegs; i++) {
        readLine(tmpString); // statistical sum
        if(statisticalSumsPointer) { // likely
          if(loadMode == LM_ALL || loadMode == LM_SUMS) {
            stringToReal(tmpString, (real_t *)(statisticalSumsPointer + TO_BLOCKS(REAL_SIZE_IN_BYTES) * i), &ctxtReal75);
          }
        }
      }
    }
  }

  else if(strcmp(tmpString, "SYSTEM_FLAGS") == 0) {
    readLine(tmpString); // Global flags
    if(loadMode == LM_ALL || loadMode == LM_SYSTEM_STATE) {
      systemFlags = stringToUint64(tmpString);
    }
  }

  else if(strcmp(tmpString, "KEYBOARD_ASSIGNMENTS") == 0) {
    readLine(tmpString); // Number of keys
    numberOfRegs = stringToInt16(tmpString);
    for(int16_t i=0; i<numberOfRegs; i++) {
      readLine(tmpString); // key
      if(loadMode == LM_ALL || loadMode == LM_SYSTEM_STATE) {
        str = tmpString;
        kbd_usr[i].keyId = stringToInt16(str);

        while(*str != ' ') {
          str++;
        }
        while(*str == ' ') {
          str++;
        }
        kbd_usr[i].primary = stringToInt16(str);

        while(*str != ' ') {
          str++;
        }
        while(*str == ' ') {
          str++;
        }
        kbd_usr[i].fShifted = stringToInt16(str);

        while(*str != ' ') {
          str++;
        }
        while(*str == ' ') {
          str++;
        }
        kbd_usr[i].gShifted = stringToInt16(str);

        while(*str != ' ') {
          str++;
        }
        while(*str == ' ') {
          str++;
        }
        kbd_usr[i].keyLblAim = stringToInt16(str);

        while(*str != ' ') {
          str++;
        }
        while(*str == ' ') {
          str++;
        }
        kbd_usr[i].primaryAim = stringToInt16(str);

        while(*str != ' ') {
          str++;
        }
        while(*str == ' ') {
          str++;
        }
        kbd_usr[i].fShiftedAim = stringToInt16(str);

        while(*str != ' ') {
          str++;
        }
        while(*str == ' ') {
          str++;
        }
        kbd_usr[i].gShiftedAim = stringToInt16(str);

        while(*str != ' ') {
          str++;
        }
        while(*str == ' ') {
          str++;
        }
        kbd_usr[i].primaryTam = stringToInt16(str);
      }
    }
  }

  else if(strcmp(tmpString, "KEYBOARD_ARGUMENTS") == 0) {
    readLine(tmpString); // Number of keys
    numberOfRegs = stringToInt16(tmpString);
    if(loadMode == LM_ALL || loadMode == LM_SYSTEM_STATE) {
      freeWp43(userKeyLabel, userKeyLabelSize);
      userKeyLabelSize = 37/*keys*/ * 6/*states*/ * 1/*byte terminator*/ + 1/*byte sentinel*/;
      userKeyLabel = allocWp43(userKeyLabelSize);
      memset(userKeyLabel,   0, TO_BYTES(TO_BLOCKS(userKeyLabelSize)));
    }
    for(int16_t i=0; i<numberOfRegs; i++) {
      readLine(tmpString); // key
      if(loadMode == LM_ALL || loadMode == LM_SYSTEM_STATE) {
        str = tmpString;
        uint16_t key = stringToUint16(str);
        userMenuItems[i].argumentName[0] = 0;

        while((*str != ' ') && (*str != '\n') && (*str != 0)) {
          str++;
        }
        if(*str == ' ') {
          while(*str == ' ') {
            str++;
          }
          if((*str != '\n') && (*str != 0)) {
            utf8ToString((uint8_t *)str, tmpString + TMP_STR_LENGTH / 2);
            setUserKeyArgument(key, tmpString + TMP_STR_LENGTH / 2);
          }
        }
      }
    }
  }

  else if(strcmp(tmpString, "MYMENU") == 0) {
    readLine(tmpString); // Number of keys
    numberOfRegs = stringToInt16(tmpString);
    for(int16_t i=0; i<numberOfRegs; i++) {
      readLine(tmpString); // key
      if(loadMode == LM_ALL || loadMode == LM_SYSTEM_STATE) {
        str = tmpString;
        userMenuItems[i].item            = stringToInt16(str);
        userMenuItems[i].argumentName[0] = 0;

        while((*str != ' ') && (*str != '\n') && (*str != 0)) {
          str++;
        }
        if(*str == ' ') {
          while(*str == ' ') {
            str++;
          }
          if((*str != '\n') && (*str != 0)) {
            utf8ToString((uint8_t *)str, userMenuItems[i].argumentName);
          }
        }
      }
    }
  }

  else if(strcmp(tmpString, "MYALPHA") == 0) {
    readLine(tmpString); // Number of keys
    numberOfRegs = stringToInt16(tmpString);
    for(int16_t i=0; i<numberOfRegs; i++) {
      readLine(tmpString); // key
      if(loadMode == LM_ALL || loadMode == LM_SYSTEM_STATE) {
        str = tmpString;
        userAlphaItems[i].item            = stringToInt16(str);
        userAlphaItems[i].argumentName[0] = 0;

        while((*str != ' ') && (*str != '\n') && (*str != 0)) {
          str++;
        }
        if(*str == ' ') {
          while(*str == ' ') {
            str++;
          }
          if((*str != '\n') && (*str != 0)) {
            utf8ToString((uint8_t *)str, userAlphaItems[i].argumentName);
          }
        }
      }
    }
  }

  else if(strcmp(tmpString, "MYPFN") == 0) {
    readLine(tmpString); // Number of keys
    numberOfRegs = stringToInt16(tmpString);
    for(int16_t i=0; i<numberOfRegs; i++) {
      readLine(tmpString); // key
      if(loadMode == LM_ALL || loadMode == LM_SYSTEM_STATE) {
        str = tmpString;
        userPfnItems[i].item            = stringToInt16(str);
        userPfnItems[i].argumentName[0] = 0;

        while((*str != ' ') && (*str != '\n') && (*str != 0)) {
          str++;
        }
        if(*str == ' ') {
          while(*str == ' ') {
            str++;
          }
          if((*str != '\n') && (*str != 0)) {
            utf8ToString((uint8_t *)str, userPfnItems[i].argumentName);
          }
        }
      }
    }
  }

  else if(strcmp(tmpString, "USER_MENUS") == 0) {
    readLine(tmpString); // Number of keys
    int16_t numberOfMenus = stringToInt16(tmpString);
    for(int32_t j=0; j<numberOfMenus; j++) {
      readLine(tmpString);
      int16_t target = -1;
      if(loadMode == LM_ALL || loadMode == LM_SYSTEM_STATE) {
        utf8ToString((uint8_t *)tmpString, tmpString + TMP_STR_LENGTH / 2);
        for(int16_t i = 0; i < numberOfUserMenus; ++i) {
          if(compareString(tmpString + TMP_STR_LENGTH / 2, userMenus[i].menuName, CMP_NAME) == 0) {
            target = i;
          }
        }
        if(target == -1) {
          createMenu(tmpString + TMP_STR_LENGTH / 2);
          target = numberOfUserMenus - 1;
        }
      }

      readLine(tmpString);
      numberOfRegs = stringToInt16(tmpString);
      for(int16_t i=0; i<numberOfRegs; i++) {
        readLine(tmpString); // key
        if(loadMode == LM_ALL || loadMode == LM_SYSTEM_STATE) {
          str = tmpString;
          userMenus[target].menuItem[i].item            = stringToInt16(str);
          userMenus[target].menuItem[i].argumentName[0] = 0;

          while((*str != ' ') && (*str != '\n') && (*str != 0)) {
            str++;
          }
          if(*str == ' ') {
            while(*str == ' ') {
              str++;
            }
            if((*str != '\n') && (*str != 0)) {
              utf8ToString((uint8_t *)str, userMenus[target].menuItem[i].argumentName);
            }
          }
        }
      }
    }
  }

  else if(strcmp(tmpString, "PROGRAMS") == 0) {
    size_t numberOfBytes;
    uint16_t oldSizeInBytes = TO_BYTES(RAM_SIZE_IN_BLOCKS - TO_WP43MEMPTR(beginOfProgramMemory));
    uint8_t *oldFirstFreeProgramByte = firstFreeProgramByte;
    uint16_t oldFreeProgramBytes = freeProgramBytes;

    readLine(tmpString); // Number of bytes
    numberOfBytes = stringToUint16(tmpString);
    if(loadMode == LM_ALL) {
      resizeProgramMemory(numberOfBytes);
    }
    else if(loadMode == LM_PROGRAMS) {
      const uint16_t availableSize = TO_BYTES(freeMemoryRegions[numberOfFreeMemoryRegions - 1].sizeInBlocks);
      if(numberOfBytes > (size_t)availableSize) {
        displayCalcErrorMessage(ERROR_NOT_ENOUGH_MEMORY_TO_LOAD_PGMS, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        errorMoreInfo("attempting to load %" PRIu64 " bytes of programs but only %" PRIu16 " bytes available", (uint64_t)numberOfBytes, availableSize);
        sprintf(tmpString + 300, "%" PRIu64, (uint64_t)numberOfBytes);
        return false; // abort
      }
      else {
        resizeProgramMemory(oldSizeInBytes + numberOfBytes);
        oldFirstFreeProgramByte = beginOfProgramMemory + oldSizeInBytes - oldFreeProgramBytes - 2;
      }
    }

    readLine(tmpString); // currentStep (pointer to block)
    if(loadMode == LM_ALL) {
      currentStep = TO_PCMEMPTR(stringToUint32(tmpString));
    }
    readLine(tmpString); // currentStep (offset in bytes within block)
    if(loadMode == LM_ALL) {
      currentStep += stringToUint32(tmpString);
    }
    else if(loadMode == LM_PROGRAMS) {
      if(programList[currentProgramNumber - 1].step > 0) {
        currentStep           -= numberOfBytes;
        firstDisplayedStep    -= numberOfBytes;
        beginOfCurrentProgram -= numberOfBytes;
        endOfCurrentProgram   -= numberOfBytes;
      }
    }

    readLine(tmpString); // firstFreeProgramByte (pointer to block)
    if(loadMode == LM_ALL || loadMode == LM_PROGRAMS) {
      firstFreeProgramByte = TO_PCMEMPTR(stringToUint32(tmpString));
    }
    readLine(tmpString); // firstFreeProgramByte (offset in bytes within block)
    if(loadMode == LM_ALL || loadMode == LM_PROGRAMS) {
      firstFreeProgramByte += stringToUint32(tmpString);
    }

    readLine(tmpString); // freeProgramBytes
    if(loadMode == LM_ALL || loadMode == LM_PROGRAMS) {
      freeProgramBytes = stringToUint16(tmpString);
    }

    if(loadMode == LM_PROGRAMS) { // .END. to END
      freeProgramBytes += oldFreeProgramBytes;
      if((oldFirstFreeProgramByte >= (beginOfProgramMemory + 2)) && isAtEndOfProgram(oldFirstFreeProgramByte - 2)) {
      }
      else {
        if(oldFreeProgramBytes + freeProgramBytes < 2) {
          uint16_t tmpFreeProgramBytes = freeProgramBytes;
          resizeProgramMemory(oldSizeInBytes + numberOfBytes + TO_BYTES(1));
          oldFirstFreeProgramByte -= 4;
          freeProgramBytes = tmpFreeProgramBytes + 4;
          if(programList[currentProgramNumber - 1].step > 0) {
            currentStep           -= 4;
            firstDisplayedStep    -= 4;
            beginOfCurrentProgram -= 4;
            endOfCurrentProgram   -= 4;
          }
        }
        *(oldFirstFreeProgramByte    ) = (ITM_END >> 8) | 0x80;
        *(oldFirstFreeProgramByte + 1) =  ITM_END       & 0xff;
        freeProgramBytes -= 2;
        oldFirstFreeProgramByte += 2;
      }
    }


    if(loadMode == LM_PROGRAMS) {
      beginOfNewlyLoadedProgramMemory = oldFirstFreeProgramByte;
    }
    else if(loadMode == LM_ALL) {
    }
    for(size_t i=0; i<numberOfBytes; i++) {
      readLine(tmpString); // One byte
      if(loadMode == LM_ALL) {
        beginOfProgramMemory[i] = stringToUint8(tmpString);
      }
      else if(loadMode == LM_PROGRAMS) {
        oldFirstFreeProgramByte[i] = stringToUint8(tmpString);
      }
    }

    scanLabelsAndPrograms();

    if(loadMode == LM_ALL) {
      currentReturnProgramNumber = currentProgramNumber;
      currentReturnLocalStep = 1;
      goToPgmStep(currentProgramNumber, currentLocalStepNumber); // to scroll PGM properly
    }
  }

  else if(strcmp(tmpString, "EQUATIONS") == 0) {
    uint16_t formulae;
    uint16_t prevNumberOfFormulae;

    prevNumberOfFormulae = numberOfFormulae;

    //Delete current equations for Load All
    if(loadMode == LM_ALL) {
      for(int16_t i = numberOfFormulae; i > 0; --i) {
        deleteEquation(i - 1);
      }
    }

    readLine(tmpString); // Number of formulae
    formulae = stringToUint16(tmpString);

    //Create headers table for Load All
    if(loadMode == LM_ALL) {
      allFormulae = allocWp43(sizeof(formulaHeader_t) * formulae);
      numberOfFormulae = formulae;
      currentFormula = 0;
      for(int16_t i = 0; i < formulae; i++) {
        allFormulae[i].pointerToFormulaData = WP43_NULL;
        allFormulae[i].sizeInBlocks = 0;
      }
    }

    for(int16_t i = 0; i < formulae; i++) {
      readLine(tmpString); // One formula
      if(loadMode == LM_ALL || loadMode == LM_EQUATIONS) {
        utf8ToString((uint8_t *)tmpString, tmpString + TMP_STR_LENGTH / 2);
        bool newEquation = true;
        if(loadMode == LM_EQUATIONS) { // Load Equations
          for(int16_t j = 0; j < prevNumberOfFormulae; j++) {
            const char *equationString = TO_PCMEMPTR(allFormulae[j].pointerToFormulaData);
            if(strcmp(tmpString, equationString) == 0) {
              newEquation = false;
              break;
            }
          }
          // if identical equation already exists in RAM don't duplicate it
          if(newEquation) {
            currentFormula = numberOfFormulae - 1;
            fnEqAdd(NOPARAM);  // Add new equation after existing ones
            setEquation(numberOfFormulae-1, tmpString + TMP_STR_LENGTH / 2);  // Copy equation string to the end of equations
          }
        }
        else { // Load All
          setEquation(i, tmpString + TMP_STR_LENGTH / 2);
        }
      }
    }
  }

  else if(strcmp(tmpString, "OTHER_CONFIGURATION_STUFF") == 0) {
    readLine(tmpString); // Number params
    numberOfRegs = stringToInt16(tmpString);
    for(int16_t i=0; i<numberOfRegs; i++) {
      readLine(aimBuffer); // param
      readLine(tmpString); // value
      if(loadMode == LM_ALL || loadMode == LM_SYSTEM_STATE) {
        if(strcmp(aimBuffer, "firstGregorianDay") == 0) {
          firstGregorianDay = stringToUint32(tmpString);
        }
        else if(strcmp(aimBuffer, "denMax") == 0) {
          denMax = stringToUint32(tmpString);
          if(denMax < 1 || denMax > MAX_DENMAX) {
            denMax = MAX_DENMAX;
          }
        }
        else if(strcmp(aimBuffer, "lastDenominator") == 0) {
          /* lastDenominator = stringToUint32(tmpString); */
          if(lastDenominator < 1 || lastDenominator > MAX_DENMAX) {
            lastDenominator = 4;
          }
        }
        else if(strcmp(aimBuffer, "displayFormat") == 0) {
          displayFormat = stringToUint8(tmpString);
        }
        else if(strcmp(aimBuffer, "displayFormatDigits") == 0) {
          displayFormatDigits = stringToUint8(tmpString);
        }
        else if(strcmp(aimBuffer, "timeDisplayFormatDigits") == 0) {
          timeDisplayFormatDigits = stringToUint8(tmpString);
        }
        else if(strcmp(aimBuffer, "shortIntegerWordSize") == 0) {
          shortIntegerWordSize = stringToUint8(tmpString);
        }
        else if(strcmp(aimBuffer, "shortIntegerMode") == 0) {
          shortIntegerMode = stringToUint8(tmpString);
        }
        else if(strcmp(aimBuffer, "significantDigits") == 0) {
          significantDigits = stringToUint8(tmpString);
        }
        else if(strcmp(aimBuffer, "currentAngularMode") == 0) {
          currentAngularMode = stringToUint8(tmpString);
        }
        else if(strcmp(aimBuffer, "groupingGap") == 0) {
          groupingGap = stringToUint8(tmpString);
        }
        else if(strcmp(aimBuffer, "roundingMode") == 0) {
          roundingMode = stringToUint8(tmpString);
        }
        else if(strcmp(aimBuffer, "displayStack") == 0) {
          displayStack = stringToUint8(tmpString);
        }
        else if(strcmp(aimBuffer, "rngState") == 0) {
          pcg32_global.state = stringToUint64(tmpString);
          str = tmpString;
          while(*str != ' ') {
            str++;
          }
          while(*str == ' ') {
            str++;
          }
          pcg32_global.inc = stringToUint64(str);
        }
        else if(strcmp(aimBuffer, "exponentLimit") == 0) {
          exponentLimit = stringToInt16(tmpString);
        }
        else if(strcmp(aimBuffer, "exponentHideLimit") == 0) {
          exponentHideLimit = stringToInt16(tmpString);
        }
        else if(strcmp(aimBuffer, "notBestF") == 0) {
          lrSelection = stringToUint16(tmpString);
        }
      }
    }
    return false; //Signal that this was the last section loaded and no more sections to follow
  }

  return true; //Signal to continue loading the next section
}



void doLoad(uint16_t loadMode, uint16_t s, uint16_t n, uint16_t d, uint16_t loadType) {
  ioFilePath_t path;
  int ret;

  if (loadType == FILE_STATE) {
    path = ioPathLoadStateFile;
  }
  else {
    path = ioPathSaveFile;
  }

  ret = ioFileOpen(path, ioModeRead);

  if(ret != FILE_OK ) {
    if(ret == FILE_CANCEL ) {
      return;
    } else {
      displayCalcErrorMessage(ERROR_CANNOT_READ_FILE, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("cannot find or read backup data file wp43.sav");
      return;
    }
  }

  if(loadMode == LM_ALL) {
    while(currentSubroutineLevel > 0) {
      fnReturn(0);
    }
    fnReturn(0); // 1 more time to clean local registers
  }

  //Check save file version
  uint32_t loadedVersion = 0;
  readLine(tmpString);
  if(strcmp(tmpString, "SAVE_FILE_REVISION") == 0) {
    readLine(aimBuffer); // internal rev number (ignore now)
    readLine(aimBuffer); // param
    readLine(tmpString); // value
    if(strcmp(aimBuffer, "WP43_save_file_00") == 0) {
      loadedVersion = stringToUint32(tmpString);
      if (loadedVersion == 1) loadedVersion = BACKUP_VERSION; //don't check old files
    }
  }


  if((loadMode == LM_PROGRAMS) || (loadMode == LM_EQUATIONS))  {
    lastErrorCode = ERROR_NONE;
  }

  //if(loadMode != LM_ALL || loadedVersion == configFileVersion) {
    while(restoreOneSection(loadMode, s, n, d)) {
    }
  //}

  if((loadMode != LM_PROGRAMS) && (loadMode != LM_EQUATIONS)) {
    lastErrorCode = ERROR_NONE;
  }

  ioFileClose();

  //-------------------------------------------------------------------------------------------------
  // This is where user is informed about versions incompatibilities and changes to loaded data occur
  // The code  below is an example of a version mismatch handling
  // The string passed to show_warning() can be the same if it fits on the HW display (7 lines of ~32
  // characters and standard ASCII characters), or two differents strings can used as shown below
  //-------------------------------------------------------------------------------------------------
  //

  if(loadedVersion <= 95) { // Check program incompatibility for MSG - MSG r replaced by MSG n
    if (loadMode == LM_ALL) {
      replaceInstruction(beginOfProgramMemory, 1, ITM_MSG, ITM_NOP_REGISTER);
    }
    else if (loadMode == LM_PROGRAMS) {
      replaceInstruction(beginOfNewlyLoadedProgramMemory, 1, ITM_MSG, ITM_NOP_REGISTER);
    }
  }

  if((loadedVersion <= 96) && (loadMode == LM_ALL)) { // Cursor arrows added to Mya
    sprintf(tmpString,"***** Old version of Myalpha *****\n"
                      "Myalpha includes now cursor arrows.\n"
                      "Use CLMya to get the new default\n"
                      "menu with the cursor arrows.\n");
    #if !defined(TESTSUITE_BUILD)
      show_warning(tmpString);
    #endif // TESTSUITE_BUILD
  }


  scanLabelsAndPrograms();


  #if !defined(TESTSUITE_BUILD)
    if(loadMode == LM_ALL) {
      if(loadType == FILE_BACKUP) {
        temporaryInformation = TI_BACKUP_RESTORED;
      } else if(loadType == FILE_STATE) {
        temporaryInformation = TI_STATEFILE_RESTORED;
      }
    } else if (loadMode == LM_PROGRAMS && lastErrorCode == ERROR_NONE) {
      temporaryInformation = TI_PROGRAMS_RESTORED;
    } else if (loadMode == LM_EQUATIONS && lastErrorCode == ERROR_NONE) {
      temporaryInformation = TI_EQUATIONS_RESTORED;
    } else if (loadMode == LM_PROGRAMS && lastErrorCode == ERROR_NOT_ENOUGH_MEMORY_TO_LOAD_PGMS) {
      temporaryInformation = TI_MEMORY_TO_LOAD_PGMS;
    } else if (loadMode == LM_EQUATIONS && lastErrorCode == ERROR_NOT_ENOUGH_MEMORY_TO_LOAD_EQUS) {
      temporaryInformation = TI_MEMORY_TO_LOAD_PGMS;
    } else if (loadMode == LM_REGISTERS) {
       temporaryInformation = TI_REGISTERS_RESTORED;
    } else if (loadMode == LM_REGISTERS) {
       temporaryInformation = TI_REGISTERS_RESTORED;
    } else if (loadMode == LM_SYSTEM_STATE) {
       temporaryInformation = TI_SETTINGS_RESTORED;
    } else if (loadMode == LM_SUMS) {
       temporaryInformation = TI_SUMS_RESTORED;
    } else if (loadMode == LM_NAMED_VARIABLES) {
       temporaryInformation = TI_VARIABLES_RESTORED;
    }
    #if defined(DMCP_BUILD)
      //Check and update current power status (USB / LOWBAT)
      dmcpCheckPowerStatus();
    #endif // DMCP_BUILD

  #endif // !TESTSUITE_BUILD
}



void fnLoad(uint16_t loadMode) {

  if(loadMode >> 8 == FILE_NONE) {
    saveLoadMode = loadMode;
    temporaryInformation = TI_LOAD_FROM;
    setFileType(fnLoad);
    return;
  }
  if (loadMode && 0xFF == LM_STATE_FILE) {
    loadMode = (loadMode & 0xFF00) + LM_ALL;
  }
  doLoad(loadMode & 0xFF, 0, 0, 0, loadMode >> 8);
}

#undef BACKUP



void fnDeleteBackup(uint16_t confirmation) {
  if(confirmation == NOT_CONFIRMED) {
    setConfirmationMode(fnDeleteBackup);
  }
  else {
    uint32_t errorNumber;
    int ret;
    ret = ioFileRemove(ioPathSaveFile, &errorNumber);
    if(ret != FILE_OK ) {
      displayCalcErrorMessage(ERROR_IO, ERR_REGISTER_LINE, REGISTER_X);
      errorMoreInfo("removing the backup failed with error code %d", errorNumber);
    }
    else if(programRunStop != PGM_RUNNING) {
      temporaryInformation = TI_CONFIRM_COMPLETED;
    }
    else {
      temporaryInformation = TI_NO_INFO;
    }
  }
}

void replaceInstruction(uint8_t *step, uint16_t programNumber, int16_t itemToReplace, int16_t itemNew) {
  ioFilePath_t path;
  int ret;
  int localStep = 1;
  char label[15];
  uint8_t labelLength;
  bool incompatibilityDetected = false;
  int16_t oldItem, newItem;

  sprintf(label,"#%d",programNumber);
  if(checkOpCodeOfStep(step, ITM_LBL)) { // LBL
    if(*(step + 1) > 109) { // Global label
      labelLength = *(step + 2);
      xcopy(label, (void *)(step + 3), labelLength);
      label[labelLength]=0;
    }
  }

  while(!isAtEndOfPrograms(step)) { // .END.
    if(isAtEndOfProgram(step)) {    // END
      programNumber++;
      localStep = 1;
      step = findNextStep(step);
      sprintf(label,"#%d",programNumber);
      if(checkOpCodeOfStep(step, ITM_LBL)) { // LBL
        if(*(step + 1) > 109) { // Global label
          labelLength = *(step + 2);
          xcopy(label, (void *)(step + 3), labelLength);
          label[labelLength]=0;
        }
      }
    }
    else {
      if((checkOpCodeOfStep(step, itemToReplace)) || ((checkOpCodeOfStep(step, ITM_REM)) && (step[2] != STRING_LABEL_VARIABLE))) {
        if(checkOpCodeOfStep(step, itemToReplace)) {
          oldItem = itemToReplace;
          newItem = itemNew;
        }
        else { // Don't replace legitimate REM instructions, only old PRCL with same item ID but not followed by a string
          oldItem = ITM_REM;
          newItem = ITM_NOP;
        }
        step[0] = (newItem >> 8) | 0x80;
        step[1] =  newItem       & 0xff;

        if(incompatibilityDetected == false) {  //First incompatible instruction detected
          incompatibilityDetected = true;
          path = ioPathLog;
          ret = ioFileOpen(path, ioModeWrite);

          if(ret != FILE_OK ) {
            if(ret == FILE_CANCEL ) {
              return;
            } else {
              #if !defined(DMCP_BUILD)
               printf("Cannot write log file!\n");
              #endif
              displayCalcErrorMessage(ERROR_CANNOT_WRITE_FILE, ERR_REGISTER_LINE, REGISTER_X);
              return;
            }
          }
          sprintf(tmpString,"****Program incompatibility detected****\n");
          ioFileWrite(tmpString, strlen(tmpString));
        }

        sprintf(tmpString,"%s found at local step %5d in program #%3d '%7s' and replaced by %s\n",
                (oldItem == ITM_REM ? "PRCL" : indexOfItems[oldItem].itemCatalogName), localStep, programNumber, label, indexOfItems[newItem].itemCatalogName);
        ioFileWrite(tmpString, strlen(tmpString));
      }
    }
    ++localStep;
    step = findNextStep(step);
  }
  if(incompatibilityDetected) {
    ioFileClose();

    sprintf(tmpString,"*Program incompatibility detected*\n"
                      "%s is replaced by %s\n"
                      "in loaded programs.\n"
                      "See log file for details\n", indexOfItems[itemToReplace].itemCatalogName, indexOfItems[itemNew].itemCatalogName);

    #if !defined(TESTSUITE_BUILD)
      show_warning(tmpString);
    #endif // TESTSUITE_BUILD
  }
}


void setFileType(void (*func)(uint16_t)) {
    previousCalcMode = calcMode;
    cursorHide();
    calcMode = cmConfirmation;
    clearSystemFlag(FLAG_ALPHA);
    confirmedFunction = func;
  #if !defined(TESTSUITE_BUILD)
    showSoftmenu(-MNU_BKUPSTF);
  #endif // !TESTSUITE_BUILD
}

void fnFileMode(uint16_t fileType) {
  #if !defined(TESTSUITE_BUILD)
    if(calcMode == cmConfirmation) {
        calcMode = previousCalcMode;
        popSoftmenu();                // Pop MNU_BKUPSTF
        temporaryInformation = TI_NO_INFO;
        confirmedFunction((fileType << 8) + saveLoadMode);
    }
  #endif // !TESTSUITE_BUILD
}