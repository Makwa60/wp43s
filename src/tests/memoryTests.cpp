// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

extern "C" {
  #include "core/freeList.h"
  #include "core/memory.h"
  #include "defines.h"
  #include "programming/manage.h"
  #include "typeDefinitions.h"
  #include "ui/tam.h"
  #include "wp43.h"
}
#include <gtest/gtest.h>
#include <cstdint>
#include <cstdlib>

TEST(MemoryTest, AllocWp43ReturnsAlignedPointersForLargeStructures) {
  ram = (dataBlock_t *)calloc(1, TO_BYTES(RAM_SIZE_IN_BLOCKS));
  ASSERT_NE(ram, nullptr);

  numberOfFreeMemoryRegions = 1;
  freeMemoryRegions[0].address = 1;
  freeMemoryRegions[0].sizeInBlocks = RAM_SIZE_IN_BLOCKS;
  wp43MemInBlocks = 0;

  void *first = allocWp43(TO_BYTES(1));
  ASSERT_NE(first, nullptr);
  EXPECT_EQ(reinterpret_cast<uintptr_t>(first) % alignof(void *), 0u);

  void *second = allocWp43(sizeof(programList_t));
  ASSERT_NE(second, nullptr);
  EXPECT_EQ(reinterpret_cast<uintptr_t>(second) % alignof(void *), 0u);

  freeWp43(second, sizeof(programList_t));
  freeWp43(first, TO_BYTES(1));
  free(ram);
  ram = nullptr;
}

TEST(MemoryTest, AllocWp43HandlesExactFitUnalignedRegions) {
  ram = (dataBlock_t *)calloc(1, TO_BYTES(RAM_SIZE_IN_BLOCKS));
  ASSERT_NE(ram, nullptr);

  numberOfFreeMemoryRegions = 1;
  freeMemoryRegions[0].address = 1;
  freeMemoryRegions[0].sizeInBlocks = TO_BLOCKS(REAL34_SIZE_IN_BYTES) + 1;
  wp43MemInBlocks = 0;

  void *allocation = allocWp43(REAL34_SIZE_IN_BYTES);
  ASSERT_NE(allocation, nullptr);
  EXPECT_EQ(reinterpret_cast<uintptr_t>(allocation) % alignof(void *), 0u);

  freeWp43(allocation, REAL34_SIZE_IN_BYTES);
  free(ram);
  ram = nullptr;
}

TEST(MemoryTest, AllocWp43KeepsRemainderRegionSizeCorrectWhenPaddingIsNeeded) {
  ram = (dataBlock_t *)calloc(1, TO_BYTES(RAM_SIZE_IN_BLOCKS));
  ASSERT_NE(ram, nullptr);

  numberOfFreeMemoryRegions = 1;
  freeMemoryRegions[0].address = 1;
  freeMemoryRegions[0].sizeInBlocks = 6;
  freeMemoryRegions[1].address = 0;
  freeMemoryRegions[1].sizeInBlocks = 0;
  wp43MemInBlocks = 0;

  void *allocation = allocWp43(REAL34_SIZE_IN_BYTES);
  ASSERT_NE(allocation, nullptr);
  EXPECT_EQ(reinterpret_cast<uintptr_t>(allocation) % alignof(void *), 0u);
  EXPECT_EQ(freeMemoryRegions[1].address, 2u + TO_BLOCKS(REAL34_SIZE_IN_BYTES));
  EXPECT_EQ(freeMemoryRegions[1].sizeInBlocks, 1u);

  freeWp43(allocation, REAL34_SIZE_IN_BYTES);
  free(ram);
  ram = nullptr;
}

TEST(MemoryTest, FreeListFreeDetectsOverlapWhenFreeingSameBlockTwice) {
  ram = (dataBlock_t *)calloc(1, TO_BYTES(RAM_SIZE_IN_BLOCKS));
  ASSERT_NE(ram, nullptr);

  numberOfFreeMemoryRegions = 1;
  freeMemoryRegions[0].address = 1;
  freeMemoryRegions[0].sizeInBlocks = RAM_SIZE_IN_BLOCKS;
  wp43MemInBlocks = 0;

  void *block = allocWp43(TO_BYTES(2));
  ASSERT_NE(block, nullptr);

  freeListFree(block, TO_BLOCKS(TO_BYTES(2)));
  freeListFree(block, TO_BLOCKS(TO_BYTES(2)));

  EXPECT_EQ(numberOfFreeMemoryRegions, 1u);

  free(ram);
  ram = nullptr;
}

TEST(MemoryTest, AllocWp43DoesNotCreateZeroSizeRemainderRegionsWhenPaddingConsumesTheRegion) {
  ram = (dataBlock_t *)calloc(1, TO_BYTES(RAM_SIZE_IN_BLOCKS));
  ASSERT_NE(ram, nullptr);

  numberOfFreeMemoryRegions = 1;
  freeMemoryRegions[0].address = 1;
  freeMemoryRegions[0].sizeInBlocks = 3;
  wp43MemInBlocks = 0;

  void *allocation = allocWp43(TO_BYTES(2));
  ASSERT_NE(allocation, nullptr);

  EXPECT_EQ(numberOfFreeMemoryRegions, 1);
  EXPECT_EQ(freeMemoryRegions[0].address, 1u);
  EXPECT_EQ(freeMemoryRegions[0].sizeInBlocks, 1u);
  EXPECT_EQ(reinterpret_cast<uintptr_t>(allocation) % alignof(void *), 0u);

  freeWp43(allocation, TO_BYTES(2));
  free(ram);
  ram = nullptr;
}

TEST(MemoryTest, LabelInsertCreatesProgramStepWithLabelName) {
  ram = (dataBlock_t *)calloc(1, TO_BYTES(RAM_SIZE_IN_BLOCKS));
  ASSERT_NE(ram, nullptr);

  numberOfFreeMemoryRegions = 1;
  freeMemoryRegions[0].address = 1;
  freeMemoryRegions[0].sizeInBlocks = RAM_SIZE_IN_BLOCKS;
  wp43MemInBlocks = 0;

  tmpString = (char *)calloc(1, 4096);
  ASSERT_NE(tmpString, nullptr);
  aimBuffer = (char *)calloc(1, 2048);
  ASSERT_NE(aimBuffer, nullptr);

  uint8_t *programBuffer = (uint8_t *)ram + 64;
  memset(programBuffer, 0, 256);
  beginOfProgramMemory = programBuffer + 32;
  firstFreeProgramByte = beginOfProgramMemory + 64;
  beginOfCurrentProgram = beginOfProgramMemory;
  endOfCurrentProgram = beginOfProgramMemory;
  firstDisplayedStep = beginOfProgramMemory;
  currentStep = beginOfProgramMemory;
  currentProgramNumber = 1;
  currentLocalStepNumber = 1;
  firstDisplayedLocalStepNumber = 0;
  numberOfPrograms = 1;
  numberOfLabels = 0;
  freeProgramBytes = 64;
  pemCursorIsZerothStep = true;
  programListEnd = false;
  lastProgramListEnd = false;
  aimBuffer[0] = 'T';
  aimBuffer[1] = 0;
  nimNumberPart = NP_EMPTY;
  tam.alpha = true;
  tam.function = ITM_LBL;
  tam.mode = tmLabel;
  tam.indirect = false;
  tam.value = 0;

  programList = (programList_t *)allocWp43(sizeof(programList_t));
  ASSERT_NE(programList, nullptr);
  programList[0].instructionPointer = beginOfProgramMemory;
  programList[0].step = 1;
  beginOfProgramMemory[0] = 0xff;
  beginOfProgramMemory[1] = 0xff;

  addStepInProgram(ITM_LBL);

  EXPECT_EQ(beginOfProgramMemory[0], ITM_LBL);
  EXPECT_EQ(beginOfProgramMemory[1], static_cast<uint8_t>(STRING_LABEL_VARIABLE));
  EXPECT_EQ(beginOfProgramMemory[2], 1);
  EXPECT_EQ(beginOfProgramMemory[3], 'T');

  freeWp43(programList, sizeof(programList_t));
  free(tmpString);
  free(aimBuffer);
  free(ram);
  tmpString = nullptr;
  aimBuffer = nullptr;
  ram = nullptr;
  programList = nullptr;
}

TEST(MemoryTest, ProgramRecordingKeepsAllInsertedSteps) {
  ram = (dataBlock_t *)calloc(1, TO_BYTES(RAM_SIZE_IN_BLOCKS));
  ASSERT_NE(ram, nullptr);

  numberOfFreeMemoryRegions = 1;
  freeMemoryRegions[0].address = 1;
  freeMemoryRegions[0].sizeInBlocks = RAM_SIZE_IN_BLOCKS;
  wp43MemInBlocks = 0;

  tmpString = (char *)calloc(1, 4096);
  ASSERT_NE(tmpString, nullptr);
  aimBuffer = (char *)calloc(1, 2048);
  ASSERT_NE(aimBuffer, nullptr);

  uint8_t *programBuffer = (uint8_t *)ram + 64;
  memset(programBuffer, 0, 256);
  beginOfProgramMemory = programBuffer + 32;
  firstFreeProgramByte = beginOfProgramMemory + 64;
  beginOfCurrentProgram = beginOfProgramMemory;
  endOfCurrentProgram = beginOfProgramMemory;
  firstDisplayedStep = beginOfProgramMemory;
  currentStep = beginOfProgramMemory;
  currentProgramNumber = 1;
  currentLocalStepNumber = 1;
  firstDisplayedLocalStepNumber = 0;
  numberOfPrograms = 1;
  numberOfLabels = 0;
  freeProgramBytes = 64;
  pemCursorIsZerothStep = true;
  programListEnd = false;
  lastProgramListEnd = false;
  aimBuffer[0] = 0;

  programList = (programList_t *)allocWp43(sizeof(programList_t));
  ASSERT_NE(programList, nullptr);
  programList[0].instructionPointer = beginOfProgramMemory;
  programList[0].step = 1;
  beginOfProgramMemory[0] = 0xff;
  beginOfProgramMemory[1] = 0xff;

  addStepInProgram(ITM_STOP);
  addStepInProgram(ITM_STOP);
  addStepInProgram(ITM_STOP);

  EXPECT_EQ(currentLocalStepNumber, 3u);
  EXPECT_EQ(currentStep[0], ITM_STOP);

  freeWp43(programList, sizeof(programList_t));
  free(tmpString);
  free(aimBuffer);
  free(ram);
  tmpString = nullptr;
  aimBuffer = nullptr;
  ram = nullptr;
  programList = nullptr;
}
