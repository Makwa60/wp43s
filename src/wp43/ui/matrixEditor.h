// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file ui/matrixEditor.h
 */
#if !defined(MATRIXEDITOR_H)
  #define MATRIXEDITOR_H

  #include "typeDefinitions.h"

  #if !defined(TESTSUITE_BUILD)
    #define MATRIX_LINE_WIDTH            380
    //#define MATRIX_LINE_WIDTH_LARGE      120
    //#define MATRIX_LINE_WIDTH_SMALL      93
    //#define MATRIX_CHAR_LEN              30
    #define MATRIX_MAX_ROWS              5
    #define MATRIX_MAX_COLUMNS           11
  #endif // TESTSUITE_BUILD

  /**
   * Opens the Matrix Editor.
   *
   * \param[in] regist
   */
  void       fnEditMatrix                   (uint16_t regist);

  /**
   * Recalls old element in the Matrix Editor.
   *
   * \param[in] unusedParamButMandatory
   */
  void       fnOldMatrix                    (uint16_t unusedParamButMandatory);

  /**
   * Go to an element in the Matrix Editor.
   *
   * \param[in] unusedParamButMandatory
   */
  void       fnGoToElement                  (uint16_t unusedParamButMandatory);

  void       fnGoToRow                      (uint16_t row);
  void       fnGoToColumn                   (uint16_t col);

  /**
   * Set grow mode.
   *
   * \param[in] growFlag
   */
  void       fnSetGrowMode                  (uint16_t growFlag);

  /**
   * Increment or decrement of register I as row pointer.
   *
   * \param[in] mode
   */
  void       fnIncDecI                      (uint16_t mode);

  /**
   * Increment or decrement of register J as column pointer.
   *
   * \param[in] mode
   */
  void       fnIncDecJ                      (uint16_t mode);

  /**
   * Insert a row.
   *
   * \param[in] unusedParamButMandatory
   */
  void       fnInsRow                       (uint16_t unusedParamButMandatory);

  /**
   * Delete a row.
   *
   * \param[in] unusedParamButMandatory
   */
  void       fnDelRow                       (uint16_t unusedParamButMandatory);

  #if !defined(TESTSUITE_BUILD)
    int16_t  getIRegisterAsInt              (bool asArrayPointer);
    int16_t  getJRegisterAsInt              (bool asArrayPointer);
    void     setIRegisterAsInt              (bool asArrayPointer, int16_t toStore);
    void     setJRegisterAsInt              (bool asArrayPointer, int16_t toStore);

    bool     wrapIJ                         (uint16_t rows, uint16_t cols);
  #endif // TESTSUITE_BUILD

#endif // !MATRIXEDITOR_H
