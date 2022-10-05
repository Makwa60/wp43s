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

#include "saveRestoreCalcState.h"

#include "apps/bugScreen.h"
#include "apps/flagBrowser.h"
#include "apps/fontBrowser.h"
#include "apps/registerBrowser.h"
#include "assign.h"
#include "calcMode.h"
#include "charString.h"
#include "config.h"
#include "display.h"
#include "error.h"
#include "flags.h"
#include "hal/gui.h"
#include "hal/io.h"
#include "items.h"
#include "matrix.h"
#include "memory.h"
#include "plotstat.h"
#include "programming/flash.h"
#include "programming/lblGtoXeq.h"
#include "programming/manage.h"
#include "programming/nextStep.h"
#include "registers.h"
#include "registerValueConversions.h"
#include "screen.h"
#include "softmenus.h"
#include "solver/equation.h"
#include "solver/graph.h"
#include "sort.h"
#include "stats.h"
#include <string.h>
#if defined(PC_BUILD)
#include <stdio.h>
#include <errno.h>
#endif

#include "wp43.h"

#define BACKUP_VERSION         82  // made roundingMode_t into an enum
#define START_REGISTER_VALUE 1000  // was 1522, why?

static char *tmpRegisterString = NULL;

static void save(const void *buffer, uint32_t size) {
  ioFileWrite(buffer, size);
}



static uint32_t restore(void *buffer, uint32_t size) {
  return ioFileRead(buffer, size);
}



#if defined(PC_BUILD)
  void saveCalc(void) {
    uint32_t backupVersion = BACKUP_VERSION;
    uint32_t ramSize       = RAM_SIZE;
    uint32_t ramPtr;

    if(!ioFileOpen(ioPathBackup, ioModeWrite)) {
      printf("Cannot save calc's memory in file backup.bin!\n");
      exit(0);
    }

    if(calcMode == cmConfirmation) {
      calcMode = previousCalcMode;
      refreshScreen();
    }

    printf("Begin of calc's backup\n");

    save(&backupVersion,                      sizeof(backupVersion));
    save(&ramSize,                            sizeof(ramSize));
    save(ram,                                 TO_BYTES(RAM_SIZE));
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
    save(softmenuStack,                       sizeof(softmenuStack));
    save(globalRegister,                      sizeof(globalRegister));
    save(savedStackRegister,                  sizeof(savedStackRegister));
    save(kbd_usr,                             sizeof(kbd_usr));
    save(userMenuItems,                       sizeof(userMenuItems));
    save(userAlphaItems,                      sizeof(userAlphaItems));
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
    ramPtr = TO_WP43MEMPTR(flashLabelList);
    save(&ramPtr,                             sizeof(ramPtr));
    ramPtr = TO_WP43MEMPTR(programList);
    save(&ramPtr,                             sizeof(ramPtr));
    ramPtr = TO_WP43MEMPTR(flashProgramList);
    save(&ramPtr,                             sizeof(ramPtr));
    ramPtr = TO_WP43MEMPTR(currentSubroutineLevelData);
    save(&ramPtr,                             sizeof(ramPtr));
    save(&xCursor,                            sizeof(xCursor));
    save(&yCursor,                            sizeof(yCursor));
    save(&firstGregorianDay,                  sizeof(firstGregorianDay));
    save(&denMax,                             sizeof(denMax));
    save(&currentRegisterBrowserScreen,       sizeof(currentRegisterBrowserScreen));
    save(&currentFntScr,                      sizeof(currentFntScr));
    save(&currentFlgScr,                      sizeof(currentFlgScr));
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
    ramPtr = TO_WP43MEMPTR(firstDisplayedStep.ram);
    save(&ramPtr,                             sizeof(ramPtr)); // firstDisplayedStep pointer to block
    ramPtr = (uint32_t)((void *)firstDisplayedStep.ram - TO_PCMEMPTR(TO_WP43MEMPTR(firstDisplayedStep.ram)));
    save(&ramPtr,                             sizeof(ramPtr)); // firstDisplayedStep offset within block
    ramPtr = TO_WP43MEMPTR(currentStep.ram);
    save(&ramPtr,                             sizeof(ramPtr)); // currentStep pointer to block
    ramPtr = (uint32_t)((void *)currentStep.ram - TO_PCMEMPTR(TO_WP43MEMPTR(currentStep.ram)));
    save(&ramPtr,                             sizeof(ramPtr)); // currentStep offset within block
    save(&freeProgramBytes,                   sizeof(freeProgramBytes));
    save(&firstDisplayedLocalStepNumber,      sizeof(firstDisplayedLocalStepNumber));
    save(&numberOfLabels,                     sizeof(numberOfLabels));
    save(&numberOfLabelsInFlash,              sizeof(numberOfLabelsInFlash));
    save(&numberOfPrograms,                   sizeof(numberOfPrograms));
    save(&numberOfProgramsInFlash,            sizeof(numberOfProgramsInFlash));
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
    save(&numberOfUserMenus,                  sizeof(numberOfUserMenus));
    save(&currentUserMenu,                    sizeof(currentUserMenu));
    save(&userKeyLabelSize,                   sizeof(userKeyLabelSize));
    save(&timerCraAndDeciseconds,             sizeof(timerCraAndDeciseconds));
    save(&timerValue,                         sizeof(timerValue));
    save(&timerTotalTime,                     sizeof(timerTotalTime));
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

    fnReset(CONFIRMED);
    if(!ioFileOpen(ioPathBackup, ioModeRead)) {
      printf("Cannot restore calc's memory from file backup.bin! Performing RESET\n");
      refreshScreen();
      return;
    }

    restore(&backupVersion,                      sizeof(backupVersion));
    restore(&ramSize,                            sizeof(ramSize));
    if(backupVersion != BACKUP_VERSION || ramSize != RAM_SIZE) {
      ioFileClose();
      refreshScreen();

      printf("Cannot restore calc's memory from file backup.bin! File backup.bin is from another backup version.\n");
      printf("               Backup file      Program\n");
      printf("backupVersion  %6u           %6d\n", backupVersion, BACKUP_VERSION);
      printf("ramSize blocks %6u           %6d\n", ramSize, RAM_SIZE);
      printf("ramSize bytes  %6u           %6d\n", TO_BYTES(ramSize), TO_BYTES(RAM_SIZE));
      return;
    }
    else {
      printf("Begin of calc's restoration\n");

      restore(ram,                                 TO_BYTES(RAM_SIZE));
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
      restore(softmenuStack,                       sizeof(softmenuStack));
      restore(globalRegister,                      sizeof(globalRegister));
      restore(savedStackRegister,                  sizeof(savedStackRegister));
      restore(kbd_usr,                             sizeof(kbd_usr));
      restore(userMenuItems,                       sizeof(userMenuItems));
      restore(userAlphaItems,                      sizeof(userAlphaItems));
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
      restore(&ramPtr,                             sizeof(ramPtr));
      flashLabelList = TO_PCMEMPTR(ramPtr);
      restore(&ramPtr,                             sizeof(ramPtr));
      programList = TO_PCMEMPTR(ramPtr);
      restore(&ramPtr,                             sizeof(ramPtr));
      flashProgramList = TO_PCMEMPTR(ramPtr);
      restore(&ramPtr,                             sizeof(ramPtr));
      currentSubroutineLevelData = TO_PCMEMPTR(ramPtr);
      restore(&xCursor,                            sizeof(xCursor));
      restore(&yCursor,                            sizeof(yCursor));
      restore(&firstGregorianDay,                  sizeof(firstGregorianDay));
      restore(&denMax,                             sizeof(denMax));
      restore(&currentRegisterBrowserScreen,       sizeof(currentRegisterBrowserScreen));
      restore(&currentFntScr,                      sizeof(currentFntScr));
      restore(&currentFlgScr,                      sizeof(currentFlgScr));
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
      restore(&lastCatalogPosition,                sizeof(lastCatalogPosition));
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
      firstDisplayedStep.ram = TO_PCMEMPTR(ramPtr);
      restore(&ramPtr,                             sizeof(ramPtr)); // firstDisplayedStep offset within block
      firstDisplayedStep.ram += ramPtr;
      restore(&ramPtr,                             sizeof(ramPtr)); // currentStep pointer to block
      currentStep.ram = TO_PCMEMPTR(ramPtr);
      restore(&ramPtr,                             sizeof(ramPtr)); // currentStep offset within block
      currentStep.ram += ramPtr;
      restore(&freeProgramBytes,                   sizeof(freeProgramBytes));
      restore(&firstDisplayedLocalStepNumber,      sizeof(firstDisplayedLocalStepNumber));
      restore(&numberOfLabels,                     sizeof(numberOfLabels));
      restore(&numberOfLabelsInFlash,              sizeof(numberOfLabelsInFlash));
      restore(&numberOfPrograms,                   sizeof(numberOfPrograms));
      restore(&numberOfProgramsInFlash,            sizeof(numberOfProgramsInFlash));
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
      restore(&numberOfUserMenus,                  sizeof(numberOfUserMenus));
      restore(&currentUserMenu,                    sizeof(currentUserMenu));
      restore(&userKeyLabelSize,                   sizeof(userKeyLabelSize));
      restore(&timerCraAndDeciseconds,             sizeof(timerCraAndDeciseconds));
      restore(&timerValue,                         sizeof(timerValue));
      restore(&timerTotalTime,                     sizeof(timerTotalTime));
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
      printf("End of calc's restoration\n");

      if(currentProgramNumber >= (numberOfPrograms - numberOfProgramsInFlash)) {
        currentStep.flash = 1;
      }
      scanFlashPgmLibrary();
      scanLabelsAndPrograms();
      defineCurrentProgramFromGlobalStepNumber((programList[currentProgramNumber - 1].step < 0 ? -1 : 1) * (currentLocalStepNumber + abs(programList[currentProgramNumber - 1].step) - 1));
      if(programList[currentProgramNumber - 1].step < 0) {
        dynamicMenuItem = -1;
        fnGotoDot(-(currentLocalStepNumber + abs(programList[currentProgramNumber - 1].step) - 1));
      }
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

      calcModeUpdateGui();
      updateMatrixHeightCache();
      refreshScreen();
    }
  }
#endif // PC_BUILD


static void registerToSaveString(calcRegister_t regist) {
  longInteger_t lgInt;
  int16_t sign;
  uint64_t value;
  char *str;
  uint8_t *cfg;

  tmpRegisterString = tmpString + START_REGISTER_VALUE;

  switch(getRegisterDataType(regist)) {
    case dtLongInteger: {
      convertLongIntegerRegisterToLongInteger(regist, lgInt);
      longIntegerToAllocatedString(lgInt, tmpRegisterString, TMP_STR_LENGTH - START_REGISTER_VALUE - 1);
      longIntegerFree(lgInt);
      strcpy(aimBuffer, "LonI");
      break;
    }

    case dtString: {
      stringToUtf8(REGISTER_STRING_DATA(regist), (uint8_t *)(tmpRegisterString));
      strcpy(aimBuffer, "Stri");
      break;
    }

    case dtShortInteger: {
      convertShortIntegerRegisterToUInt64(regist, &sign, &value);
      sprintf(tmpRegisterString, "%c%" PRIu64 " %" PRIu32, sign ? '-' : '+', value, getRegisterShortIntegerBase(regist));
      strcpy(aimBuffer, "ShoI");
      break;
    }

    case dtReal34: {
      real34ToString(REGISTER_REAL34_DATA(regist), tmpRegisterString);
      switch(getRegisterAngularMode(regist)) {
        case amDegree: {
          strcpy(aimBuffer, "Real:DEG");
          break;
        }

        case amDMS: {
          strcpy(aimBuffer, "Real:DMS");
          break;
        }

        case amRadian: {
          strcpy(aimBuffer, "Real:RAD");
          break;
        }

        case amMultPi: {
          strcpy(aimBuffer, "Real:MULTPI");
          break;
        }

        case amGrad: {
          strcpy(aimBuffer, "Real:GRAD");
          break;
        }

        case amNone: {
          strcpy(aimBuffer, "Real");
          break;
        }

        default: {
          strcpy(aimBuffer, "Real:???");
          break;
        }
      }
      break;
    }

    case dtComplex34: {
      real34ToString(REGISTER_REAL34_DATA(regist), tmpRegisterString);
      strcat(tmpRegisterString, " ");
      real34ToString(REGISTER_IMAG34_DATA(regist), tmpRegisterString + strlen(tmpRegisterString));
      strcpy(aimBuffer, "Cplx");
      break;
    }

    case dtTime: {
      real34ToString(REGISTER_REAL34_DATA(regist), tmpRegisterString);
      strcpy(aimBuffer, "Time");
      break;
    }

    case dtDate: {
      real34ToString(REGISTER_REAL34_DATA(regist), tmpRegisterString);
      strcpy(aimBuffer, "Date");
      break;
    }

    case dtReal34Matrix: {
      sprintf(tmpRegisterString, "%" PRIu16 " %" PRIu16, REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixRows, REGISTER_REAL34_MATRIX_DBLOCK(regist)->matrixColumns);
      strcpy(aimBuffer, "Rema");
      break;
    }

    case dtComplex34Matrix: {
      sprintf(tmpRegisterString, "%" PRIu16 " %" PRIu16, REGISTER_COMPLEX34_MATRIX_DBLOCK(regist)->matrixRows, REGISTER_COMPLEX34_MATRIX_DBLOCK(regist)->matrixColumns);
      strcpy(aimBuffer, "Cxma");
      break;
    }

    case dtConfig: {
      for(str=tmpRegisterString, cfg=(uint8_t *)REGISTER_CONFIG_DATA(regist), value=0; value<sizeof(dtConfigDescriptor_t); value++, cfg++, str+=2) {
        sprintf(str, "%02X", *cfg);
      }
      strcpy(aimBuffer, "Conf");
      break;
    }

    default: {
      strcpy(tmpRegisterString, "???");
      strcpy(aimBuffer, "????");
    }
  }
}


static void saveMatrixElements(calcRegister_t regist) {
  #if !defined(TESTSUITE_BUILD)
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
  #endif // !TESTSUITE_BUILD
}



void fnSave(uint16_t unusedButMandatoryParameter) {
  calcRegister_t regist;
  uint32_t i;

  if(!ioFileOpen(ioPathSaveFile, ioModeWrite)) {
    #if !defined(DMCP_BUILD)
      printf("Cannot SAVE in file wp43.sav!\n");
    #endif
    return;
  }

  // Global registers
  sprintf(tmpString, "GLOBAL_REGISTERS\n%" PRIu16 "\n", (uint16_t)(FIRST_LOCAL_REGISTER));
  save(tmpString, strlen(tmpString));
  for(regist=0; regist<FIRST_LOCAL_REGISTER; regist++) {
    registerToSaveString(regist);
    sprintf(tmpString, "R%03" PRId16 "\n%s\n%s\n", regist, aimBuffer, tmpRegisterString);
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
    sprintf(tmpString, "R.%02" PRIu32 "\n%s\n%s\n", i, aimBuffer, tmpRegisterString);
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
    sprintf(tmpString, "%s\n%s\n%s\n", "name", aimBuffer, tmpRegisterString);
    save(tmpString, strlen(tmpString));
    saveMatrixElements(FIRST_NAMED_VARIABLE + i);
  }

  // Statistical sums
  sprintf(tmpString, "STATISTICAL_SUMS\n%" PRIu16 "\n", (uint16_t)(statisticalSumsPointer ? NUMBER_OF_STATISTICAL_SUMS : 0));
  save(tmpString, strlen(tmpString));
  for(i=0; i<(statisticalSumsPointer ? NUMBER_OF_STATISTICAL_SUMS : 0); i++) {
    realToString((real_t *)(statisticalSumsPointer + REAL_SIZE * i), tmpRegisterString);
    sprintf(tmpString, "%s\n", tmpRegisterString);
    save(tmpString, strlen(tmpString));
  }

  // System flags
  sprintf(tmpString, "SYSTEM_FLAGS\n%" PRIu64 "\n", systemFlags);
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
  uint16_t currentSizeInBlocks = RAM_SIZE - freeMemoryRegions[numberOfFreeMemoryRegions - 1].address - freeMemoryRegions[numberOfFreeMemoryRegions - 1].sizeInBlocks;
  sprintf(tmpString, "PROGRAMS\n%" PRIu16 "\n", currentSizeInBlocks);
  save(tmpString, strlen(tmpString));

  sprintf(tmpString, "%" PRIu32 "\n%" PRIu32 "\n", (uint32_t)TO_WP43MEMPTR(currentStep.ram), (uint32_t)((void *)currentStep.ram - TO_PCMEMPTR(TO_WP43MEMPTR(currentStep.ram)))); // currentStep block pointer + offset within block
  save(tmpString, strlen(tmpString));

  sprintf(tmpString, "%" PRIu32 "\n%" PRIu32 "\n", (uint32_t)TO_WP43MEMPTR(firstFreeProgramByte), (uint32_t)((void *)firstFreeProgramByte - TO_PCMEMPTR(TO_WP43MEMPTR(firstFreeProgramByte)))); // firstFreeProgramByte block pointer + offset within block
  save(tmpString, strlen(tmpString));

  sprintf(tmpString, "%" PRIu16 "\n", freeProgramBytes);
  save(tmpString, strlen(tmpString));

  for(i=0; i<currentSizeInBlocks; i++) {
    sprintf(tmpString, "%" PRIu32 "\n", *(((uint32_t *)(beginOfProgramMemory)) + i));
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
  sprintf(tmpString, "displayFormat\n%" PRIu8 "\n", displayFormat);
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
  sprintf(tmpString, "rngState\n%" PRIu64 " %" PRIu64 "\n", pcg32_global.state, pcg32_global.inc);
  save(tmpString, strlen(tmpString));
  sprintf(tmpString, "exponentLimit\n%" PRId16 "\n", exponentLimit);
  save(tmpString, strlen(tmpString));
  sprintf(tmpString, "exponentHideLimit\n%" PRId16 "\n", exponentHideLimit);
  save(tmpString, strlen(tmpString));
  sprintf(tmpString, "notBestF\n%" PRIu16 "\n", lrSelection);
  save(tmpString, strlen(tmpString));

  ioFileClose();

  temporaryInformation = TI_SAVED;
}



static void readLine(char *line) {
  restore(line, 1);
  while(*line == '\n' || *line == '\r') {
    restore(line, 1);
  }

  while(*line != '\n' && *line != '\r') {
    restore(++line, 1);
  }

  *line = 0;
}



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
  bool_t sign = false;

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
  bool_t sign = false;

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
    else if(type[5] == 'M') {
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
    else {
      tag = amNone;
    }

    reallocateRegister(regist, dtReal34, REAL34_SIZE, tag);
    stringToReal34(value, REGISTER_REAL34_DATA(regist));
  }

  else if(strcmp(type, "Real") == 0) {
    reallocateRegister(regist, dtReal34, REAL34_SIZE, tag);
    stringToReal34(value, REGISTER_REAL34_DATA(regist));
  }

  else if(strcmp(type, "Time") == 0) {
    reallocateRegister(regist, dtTime, REAL34_SIZE, amNone);
    stringToReal34(value, REGISTER_REAL34_DATA(regist));
  }

  else if(strcmp(type, "Date") == 0) {
    reallocateRegister(regist, dtDate, REAL34_SIZE, amNone);
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
    int32_t len;

    utf8ToString((uint8_t *)value, errorMessage);
    len = stringByteLength(errorMessage) + 1;
    reallocateRegister(regist, dtString, TO_BLOCKS(len), amNone);
    xcopy(REGISTER_STRING_DATA(regist), errorMessage, len);
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

    reallocateRegister(regist, dtComplex34, COMPLEX34_SIZE, amNone);
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
    reallocateRegister(regist, dtReal34Matrix, REAL34_SIZE * rows * cols, amNone);
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
    reallocateRegister(regist, dtComplex34Matrix, COMPLEX34_SIZE * rows * cols, amNone);
    REGISTER_COMPLEX34_MATRIX_DBLOCK(regist)->matrixRows = rows;
    REGISTER_COMPLEX34_MATRIX_DBLOCK(regist)->matrixColumns = cols;
  }
#endif // TESTSUITE_BUILD

  else if(strcmp(type, "Conf") == 0) {
    char *cfg;

    reallocateRegister(regist, dtConfig, CONFIG_SIZE, amNone);
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



static bool_t restoreOneSection(uint16_t loadMode, uint16_t s, uint16_t n, uint16_t d) {
  int16_t i, numberOfRegs;
  calcRegister_t regist;
  char *str;

  readLine(tmpString);

  if(strcmp(tmpString, "GLOBAL_REGISTERS") == 0) {
    readLine(tmpString); // Number of global registers
    numberOfRegs = stringToInt16(tmpString);
    for(i=0; i<numberOfRegs; i++) {
      readLine(tmpString); // Register number
      regist = stringToInt16(tmpString + 1);
      readLine(aimBuffer); // Register data type
      readLine(tmpString); // Register value

      if(loadMode == LM_ALL || (loadMode == LM_REGISTERS && regist < REGISTER_X) || (loadMode == LM_REGISTERS_PARTIAL && regist >= s && regist < (s + n))) {
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
      allocateLocalRegisters(numberOfRegs);
    }

    if((loadMode != LM_ALL && loadMode != LM_REGISTERS) || lastErrorCode == ERROR_NONE) {
      for(i=0; i<numberOfRegs; i++) {
        readLine(tmpString); // Register number
        regist = stringToInt16(tmpString + 2) + FIRST_LOCAL_REGISTER;
        readLine(aimBuffer); // Register data type
        readLine(tmpString); // Register value

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
    if(loadMode == LM_ALL || loadMode == LM_REGISTERS) {
      currentLocalFlags->localFlags = stringToUint32(tmpString);
    }
  }

  else if(strcmp(tmpString, "NAMED_VARIABLES") == 0) {
    readLine(tmpString); // Number of named variables
    numberOfRegs = stringToInt16(tmpString);
    for(i=0; i<numberOfRegs; i++) {
      readLine(errorMessage); // Variable name
      readLine(aimBuffer); // Variable data type
      readLine(tmpString); // Variable value

      if(loadMode == LM_ALL || loadMode == LM_NAMED_VARIABLES) {
        //printf("Variable %s ", errorMessage);
        //printf("%s = ", aimBuffer);
        //printf("%s\n", tmpString);
      }
      skipMatrixData(aimBuffer, tmpString);
    }
  }

  else if(strcmp(tmpString, "STATISTICAL_SUMS") == 0) {
    readLine(tmpString); // Number of statistical sums
    numberOfRegs = stringToInt16(tmpString);
    if(numberOfRegs > 0 && (loadMode == LM_ALL || loadMode == LM_SUMS)) {
      initStatisticalSums();
    }

    for(i=0; i<numberOfRegs; i++) {
      readLine(tmpString); // statistical sum
      if(statisticalSumsPointer) { // likely
        if(loadMode == LM_ALL || loadMode == LM_SUMS) {
          stringToReal(tmpString, (real_t *)(statisticalSumsPointer + REAL_SIZE * i), &ctxtReal75);
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
    for(i=0; i<numberOfRegs; i++) {
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
      freeWp43(userKeyLabel, TO_BLOCKS(userKeyLabelSize));
      userKeyLabelSize = 37/*keys*/ * 6/*states*/ * 1/*byte terminator*/ + 1/*byte sentinel*/;
      userKeyLabel = allocWp43(TO_BLOCKS(userKeyLabelSize));
      memset(userKeyLabel,   0, TO_BYTES(TO_BLOCKS(userKeyLabelSize)));
    }
    for(i=0; i<numberOfRegs; i++) {
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
    for(i=0; i<numberOfRegs; i++) {
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
    for(i=0; i<numberOfRegs; i++) {
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

  else if(strcmp(tmpString, "USER_MENUS") == 0) {
    readLine(tmpString); // Number of keys
    int16_t numberOfMenus = stringToInt16(tmpString);
    for(int32_t j=0; j<numberOfMenus; j++) {
      readLine(tmpString);
      int16_t target = -1;
      if(loadMode == LM_ALL || loadMode == LM_SYSTEM_STATE) {
        utf8ToString((uint8_t *)tmpString, tmpString + TMP_STR_LENGTH / 2);
        for(i = 0; i < numberOfUserMenus; ++i) {
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
      for(i=0; i<numberOfRegs; i++) {
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
    uint16_t numberOfBlocks;
    uint16_t oldSizeInBlocks = RAM_SIZE - freeMemoryRegions[numberOfFreeMemoryRegions - 1].address - freeMemoryRegions[numberOfFreeMemoryRegions - 1].sizeInBlocks;
    uint8_t *oldFirstFreeProgramByte = firstFreeProgramByte;
    uint16_t oldFreeProgramBytes = freeProgramBytes;

    readLine(tmpString); // Number of blocks
    numberOfBlocks = stringToUint16(tmpString);
    if(loadMode == LM_ALL) {
      resizeProgramMemory(numberOfBlocks);
    }
    else if(loadMode == LM_PROGRAMS) {
      resizeProgramMemory(oldSizeInBlocks + numberOfBlocks);
      oldFirstFreeProgramByte = beginOfProgramMemory + TO_BYTES(oldSizeInBlocks) - oldFreeProgramBytes - 2;
    }

    readLine(tmpString); // currentStep (pointer to block)
    if(loadMode == LM_ALL) {
      currentStep.ram = TO_PCMEMPTR(stringToUint32(tmpString));
    }
    readLine(tmpString); // currentStep (offset in bytes within block)
    if(loadMode == LM_ALL) {
      currentStep.ram += stringToUint32(tmpString);
    }
    else if(loadMode == LM_PROGRAMS) {
      if(programList[currentProgramNumber - 1].step > 0) {
        currentStep.ram           -= TO_BYTES(numberOfBlocks);
        firstDisplayedStep.ram    -= TO_BYTES(numberOfBlocks);
        beginOfCurrentProgram.ram -= TO_BYTES(numberOfBlocks);
        endOfCurrentProgram.ram   -= TO_BYTES(numberOfBlocks);
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
      if((oldFirstFreeProgramByte >= (beginOfProgramMemory + 2)) && (*(oldFirstFreeProgramByte - 2) == ((ITM_END >> 8) | 0x80)) && (*(oldFirstFreeProgramByte - 1) == (ITM_END & 0xff))) {
      }
      else {
        if(oldFreeProgramBytes + freeProgramBytes < 2) {
          uint16_t tmpFreeProgramBytes = freeProgramBytes;
          resizeProgramMemory(oldSizeInBlocks + numberOfBlocks + 1);
          oldFirstFreeProgramByte -= 4;
          freeProgramBytes = tmpFreeProgramBytes + 4;
          if(programList[currentProgramNumber - 1].step > 0) {
            currentStep.ram           -= 4;
            firstDisplayedStep.ram    -= 4;
            beginOfCurrentProgram.ram -= 4;
            endOfCurrentProgram.ram   -= 4;
          }
        }
        *(oldFirstFreeProgramByte    ) = (ITM_END >> 8) | 0x80;
        *(oldFirstFreeProgramByte + 1) =  ITM_END       & 0xff;
        freeProgramBytes -= 2;
        oldFirstFreeProgramByte += 2;
      }
    }

    for(i=0; i<numberOfBlocks; i++) {
      readLine(tmpString); // One block
      if(loadMode == LM_ALL) {
        *(((uint32_t *)(beginOfProgramMemory)) + i) = stringToUint32(tmpString);
      }
      else if(loadMode == LM_PROGRAMS) {
        uint32_t tmpBlock = stringToUint32(tmpString);
        xcopy(oldFirstFreeProgramByte + TO_BYTES(i), (uint8_t *)(&tmpBlock), 4);
      }
    }

    scanLabelsAndPrograms();
  }

  else if(strcmp(tmpString, "EQUATIONS") == 0) {
    uint16_t formulae;

    if(loadMode == LM_ALL || loadMode == LM_PROGRAMS) {
      for(i = numberOfFormulae; i > 0; --i) {
        deleteEquation(i - 1);
      }
    }

    readLine(tmpString); // Number of formulae
    formulae = stringToUint16(tmpString);
    if(loadMode == LM_ALL || loadMode == LM_PROGRAMS) {
      allFormulae = allocWp43(TO_BLOCKS(sizeof(formulaHeader_t)) * formulae);
      numberOfFormulae = formulae;
      currentFormula = 0;
      for(i = 0; i < formulae; i++) {
        allFormulae[i].pointerToFormulaData = WP43_NULL;
        allFormulae[i].sizeInBlocks = 0;
      }
    }

    for(i = 0; i < formulae; i++) {
      readLine(tmpString); // One formula
      if(loadMode == LM_ALL || loadMode == LM_PROGRAMS) {
        utf8ToString((uint8_t *)tmpString, tmpString + TMP_STR_LENGTH / 2);
        setEquation(i, tmpString + TMP_STR_LENGTH / 2);
      }
    }
  }

  else if(strcmp(tmpString, "OTHER_CONFIGURATION_STUFF") == 0) {
    readLine(tmpString); // Number params
    numberOfRegs = stringToInt16(tmpString);
    for(i=0; i<numberOfRegs; i++) {
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



void doLoad(uint16_t loadMode, uint16_t s, uint16_t n, uint16_t d) {
  if(!ioFileOpen(ioPathSaveFile, ioModeRead)) {
    displayCalcErrorMessage(ERROR_NO_BACKUP_DATA, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      moreInfoOnError("In function fnLoad: cannot find or read backup data file wp43.sav", NULL, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
    return;
  }

  if(loadMode == LM_ALL) {
    while(currentSubroutineLevel > 0) {
      fnReturn(0);
    }
  }

  while(restoreOneSection(loadMode, s, n, d)) {
  }

  lastErrorCode = ERROR_NONE;

  ioFileClose();

  #if !defined(TESTSUITE_BUILD)
    if(loadMode == LM_ALL) {
      temporaryInformation = TI_BACKUP_RESTORED;
    }
  #endif // !TESTSUITE_BUILD
}



void fnLoad(uint16_t loadMode) {
  doLoad(loadMode, 0, 0, 0);
}

#undef BACKUP



void fnDeleteBackup(uint16_t confirmation) {
  if(confirmation == NOT_CONFIRMED) {
    setConfirmationMode(fnDeleteBackup);
  }
  else {
    #if defined(DMCP_BUILD)
      FRESULT result;
      sys_disk_write_enable(1);
      result = f_unlink("SAVFILES\\wp43.sav");
      if(result != FR_OK && result != FR_NO_FILE && result != FR_NO_PATH) {
        displayCalcErrorMessage(ERROR_IO, ERR_REGISTER_LINE, REGISTER_X);
      }
      sys_disk_write_enable(0);
    #else // !DMCP_BUILD
      int result = remove("wp43.sav");
      if(result == -1) {
        #if !defined(TESTSUITE_BUILD)
          int e = errno;
          if(e != ENOENT) {
            displayCalcErrorMessage(ERROR_IO, ERR_REGISTER_LINE, REGISTER_X);
            #if (EXTRA_INFO_ON_CALC_ERROR == 1)
              sprintf(errorMessage, "removing the backup failed with error code %d", e);
              moreInfoOnError("In function fnDeleteBackup:", errorMessage, NULL, NULL);
            #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
          }
        #endif // !TESTSUITE_BUILD
      }
    #endif // DMCP_BUILD
  }
}
