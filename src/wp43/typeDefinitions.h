// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file typeDefinitions.h
 */
#if !defined(TYPEDEFINITIONS_H)
  #define TYPEDEFINITIONS_H

  #include "realType.h"
  #include <stdbool.h>
  #include <stdint.h>

  /**
   * \union multiplyDivide_t
   * Used for unit conversions.
   */
  typedef enum {
    multiply,
    divide
  } multiplyDivide_t;



  /**
   * \struct calcKey_t
   * Structure keeping the informations for one key.
   */
  typedef struct {
    int16_t keyId;       ///< ID of the key
    int16_t primary;     ///< ID of the primary function of the key
    int16_t fShifted;    ///< ID of the f shifted function of the key
    int16_t gShifted;    ///< ID of the g shifted function of the key
    int16_t keyLblAim;   ///< ID of the main label of the key
    int16_t primaryAim;  ///< ID of the primary AIM function: latin letters
    int16_t fShiftedAim; ///< ID of the f shifted AIM function:
    int16_t gShiftedAim; ///< ID of the g shifted AIM function: greek letters
    int16_t primaryTam;  ///< ID of the primary TAM function of the key
  } calcKey_t;



  /**
   * \struct glyph_t
   * Structure keeping the informations for one glyph.
   */
  typedef struct {
    uint16_t charCode;         ///< Unicode code point
    uint8_t  colsBeforeGlyph;  ///< Number of empty columns before the glyph
    uint8_t  colsGlyph;        ///< Number of columns of the glyph
    uint8_t  colsAfterGlyph;   ///< Number of empty columns afer the glyph
    uint8_t  rowsAboveGlyph;   ///< Number of empty rows above the glyph
    uint8_t  rowsGlyph;        ///< Number of rows of the glyph
    uint8_t  rowsBelowGlyph;   ///< Number of empty rows below the glypg
    int16_t  rank1;            ///< Rank of the replacement glyph
    int16_t  rank2;            ///< Rank of the glyph
    char     *data;            ///< Hexadecimal data representing the glyph.
                               ///< There are rowsGlyph x (colsGlyph rounded up to 8 bit boundary) bytes
  } glyph_t;



  /**
   * \struct font_t
   * Font description.
   */
  typedef struct {
    int8_t  id;              ///< 0=numeric 1=standard
    uint16_t numberOfGlyphs; ///< Number of glyphs in the font
    glyph_t glyphs[];        ///< Pointer to the glyph description structure
  } font_t;



  /**
   * \struct freeMemoryRegion_t
   * Keeps track of free memory regions.
   */
  typedef struct {
    uint16_t address;      ///< Address of the free memory region
    uint16_t sizeInBlocks; ///< Size in blocks of the free memory region
  } freeMemoryRegion_t;



  /**
   * \enum dataType_t
   * Different data types.
   */
  typedef enum {
    dtLongInteger     =  0,  ///< Z arbitrary precision integer
    dtReal34          =  1,  ///< R double precision real (128 bits)
    dtComplex34       =  2,  ///< C double precision complex (2x 128 bits)
    dtTime            =  3,  ///< Time
    dtDate            =  4,  ///< Date in various formats
    dtString          =  5,  ///< Alphanumeric string
    dtReal34Matrix    =  6,  ///< Double precision vector or matrix
    dtComplex34Matrix =  7,  ///< Double precision complex vector or matrix
    dtShortInteger    =  8,  ///< Short integer (64 bit)
    dtConfig          =  9,  ///< Configuration
    //dtLabel           = 10,  ///< Label
    //dtSystemInteger   = 11,  ///< System integer (64 bits)
    //dtFlags           = 12,  ///< Flags
    //dtPgmStep         = 13,  ///< Program step
    //dtDirectory       = 14,  ///< Program
  } dataType_t; // 4 bits (NOT 5 BITS)



  /**
   * \enum angularMode_t
   * Angular units.
   */
  typedef enum {
    amRadian =  0, // radian must be 0  | This is because of the tables angle45,
    amGrad   =  1, // grad   must be 1  | angle90, and angle180 for angle reduction
    amDegree =  2, // degree must be 2  | before Taylor trig computation.
    amDMS    =  3,
    amMultPi =  4,
    amNone   =  5,
    amSecond =  6  // not an angular but a time unit: for the routine unified with the real type
  } angularMode_t;



  /**
   * \struct dtConfigDescriptor_t
   * Configuration for STOCFG and RCLCFG.
   */
  typedef struct {
    uint8_t       shortIntegerMode;
    uint8_t       shortIntegerWordSize;
    uint8_t       displayFormat;
    uint8_t       displayFormatDigits;
    uint8_t       groupingGap;
    uint8_t       displayStack;
    uint8_t       roundingMode;
    uint8_t       timeDisplayFormatDigits;
    uint8_t       reservedForPossibleFutureUse[3];
    angularMode_t currentAngularMode;
    uint16_t      lrSelection;
    uint16_t      lrChosen;
    uint32_t      denMax;
    uint32_t      firstGregorianDay;
    uint64_t      systemFlags;
    calcKey_t     kbd_usr[37];
  } dtConfigDescriptor_t;



  /**
   * \union registerHeader_t
   * 32 bits describing the register.
   */
  typedef union {
    uint32_t descriptor;
    struct {
      uint32_t pointerToRegisterData : 16; ///< Memory block number
      uint32_t dataType              :  4; ///< dtLongInteger, dtReal34, ...
      uint32_t tag                   :  5; ///< Short integer base, real34 angular mode, or long integer sign
      uint32_t readOnly              :  1; ///< The register or variable is readOnly if this field is 1 (used for system named variables)
      uint32_t notUsed               :  6; ///< 6 bits free
    };
  } registerHeader_t;



  /**
   * \union dataBlock_t
   * Header for datatypes: string, long integer, and matrix.
   */
  typedef union {
    uint32_t data;
    uint32_t localFlags;

    struct {
      uint16_t dataMaxLength;             ///< String max length (includind terminating \0) in blocks or Long integer max length in blocks
      uint16_t dummy;                     ///< Dummy
    };

    struct {
      uint16_t variableNameLen;           ///< Size of the name in blocs: 1 to 4, up to 15 bytes = 7 double byte glyphs + trailing 0
      uint16_t ptrToVariableName;         ///< Pointer to the name of the variable
    };

    struct {
      uint16_t matrixRows;                ///< Number of rows in the matrix
      uint16_t matrixColumns;             ///< Number of columns in the matrix
    };

    struct {
      uint16_t numberOfSubroutineLevels;  ///< Number of subroutine levels
      uint16_t ptrToSubroutineLevel0Data; ///< Pointer to subroutine level 0 data
    };

    struct {
      uint16_t numberOfNamedVariables;    ///< Number of named variables
      uint16_t ptrToNamedVariablesList;   ///< Pointer to the named variable list
    };

    struct {
      int16_t  returnProgramNumber;       ///< return program number >0 if in RAM and <0 if in FLASH
      uint16_t returnLocalStep;           ///< Return local step number in program number
    };

    struct {
      uint8_t  numberOfLocalFlags;        ///< Number of allocated local flags
      uint8_t  numberOfLocalRegisters;    ///< Number of allocated local registers
      uint16_t subroutineLevel;           ///< Subroutine level
    };

    struct {
      uint16_t ptrToNextLevel;            ///< Pointer to next level of subroutine data
      uint16_t ptrToPreviousLevel;        ///< Pointer to previous level of subroutine data
    };
  } dataBlock_t;



  /**
   * \struct namedVariableHeader_t
   * Header for named variables.
   */
  typedef struct {
    registerHeader_t header;  ///< Header
    uint8_t variableName[16]; ///< Variable name
  } namedVariableHeader_t;



  /**
   * \struct reservedVariableHeader_t
   * Header for system named variables.
   */
  typedef struct {
    registerHeader_t header;         ///< Header
    uint8_t reservedVariableName[8]; ///< Variable name
  } reservedVariableHeader_t;



  /**
   * \struct formulaHeader_t
   * Header for EQN formulae.
   */
  typedef struct {
    uint16_t pointerToFormulaData; ///< Memory block number
    uint8_t  sizeInBlocks;         ///< Size of allocated memory block
    uint8_t  unused;               ///< Padding
  } formulaHeader_t;



  /**
   * \enum videoMode_t
   * Video mode: normal video or reverse video.
   */
  typedef enum {
    vmNormal,  ///< Normal mode: black on white background
    vmReverse  ///< Reverse mode: white on black background
  } videoMode_t; // 1 bit



  /**
   * \struct softmenu_t
   * Structure keeping the informations for one softmenu.
   */
  typedef struct {
    int16_t menuItem;           ///< ID of the menu. The item is always negative and -item must be in the indexOfItems area
    int16_t numItems;           ///< Number of items in the softmenu (must be a multiple of 6 for now)
    const int16_t *softkeyItem; ///< Pointer to the first item of the menu
  } softmenu_t;



  /**
   * \struct dynamicSoftmenu_t
   * Structure keeping the informations for one variable softmenu.
   */
  typedef struct {
    int16_t menuItem;           ///< ID of the menu. The item is always negative and -item must be in the indexOfItems area
    int16_t numItems;           ///< Number of items in the dynamic softmenu (must be a multiple of 6 for now)
    uint8_t *menuContent;       ///< Pointer to the menu content
  } dynamicSoftmenu_t;



  /**
   * \struct softmenuStack_t
   * Stack of softmenus.
   */
  typedef struct {
    int16_t softmenuId; ///< Softmenu ID = rank in dynamicSoftmenu or softmenu
    int16_t firstItem;  ///< Current first item on the screen (unshifted F1 = bottom left)
  } softmenuStack_t;



  /**
   * \typedef calcRegister_t
   * A type for calculator registers.
   */
  typedef int16_t calcRegister_t;



  /**
   * \struct real34Matrix_t
   * A type for real34Matrix.
   */
  typedef struct {
     dataBlock_t header;
     real34_t    *matrixElements;
  } real34Matrix_t;



  /**
   * \struct complex34Matrix_t
   * A type for complex34Matrix.
   */
  typedef struct {
     dataBlock_t header;
     complex34_t *matrixElements;
  } complex34Matrix_t;



  /**
   * \union any34Matrix_t
   * Stores either real34Matrix_t or complex34Matrix_t.
   */
  typedef union {
     dataBlock_t       header;
     real34Matrix_t    realMatrix;
     complex34Matrix_t complexMatrix;
  } any34Matrix_t;



  /**
   * \struct item_t
   * Structure keeping the information for one item.
   */
  typedef struct {
    void     (*func)(uint16_t); ///< Function called to execute the item
    uint16_t param;             ///< 1st parameter to the above function
    char     *itemCatalogName;  ///< Name of the item in the catalogs and in programs
    char     *itemSoftmenuName; ///< Representation of the item in the menus and on the keyboard
    uint16_t tamMinMax;         ///< Minimal value (2 bits) and maximal value (14 bits) for TAM argument
    //uint16_t tamMin;            ///< Minimal value for TAM argument
    //uint16_t tamMax;            ///< Maximal value for TAM argument
    uint16_t status;            ///< Catalog, stack lift status and undo status
    //char     catalog;           ///< Catalog in which the item is located: see #define CAT_* in defines.h
    //uint8_t  stackLiftStatus;   ///< Stack lift status after item execution.
    //uint8_t  undoStatus;        ///< Undo status after item execution.
  } item_t;



  /**
   * \struct userMenuItem_t
   * Structure keeping the information for one item of MyMenu and MyAlpha.
   */
  typedef struct {
    int16_t  item;               ///< Item ID
    int16_t  unused;             ///< Padding
    char     argumentName[16];   ///< Name of variable or label
  } userMenuItem_t;



  /**
   * \struct userMenu_t
   * Structure keeping the information for a user-defined menu.
   */
  typedef struct {
    char           menuName[16];  ///< Name of menu
    userMenuItem_t menuItem[18];  ///< Items
  } userMenu_t;



  /**
   * \struct programmableMenu_t
   * Structure keeping the information for programmable menu.
   */
  typedef struct {
    char           itemName[18][16]; ///< Name of items
    uint16_t       itemParam[21];    ///< Parameter, XEQ if MSB set, GTO if MSB clear
    uint16_t       unused;           ///< Padding
  } programmableMenu_t;



  /**
   * \union pgmPtr_t
   * Stores a pointer to program step in RAM or flash
   */
  typedef union {
    uint8_t  *any;
    uint8_t  *ram;
    uintptr_t flash;
  } pgmPtr_t;



  /**
   * \struct labelList_t
   * Structure keeping the information for a program label.
   */
  typedef struct {
    int16_t  program;             ///< Program id: <0 for FLASH and >0 for RAM
    int32_t  step;                ///< Step number of the label: <0 for a local label and >0 for a global label
    pgmPtr_t labelPointer;        ///< Pointer to the byte after the 0x01 op code (LBL)
    pgmPtr_t instructionPointer;  ///< Pointer to the instruction following the label
  } labelList_t;



  /**
   * \struct programList_t
   * Structure keeping the information for a program.
   */
  typedef struct {
    int32_t  step;                ///< (Step number + 1) of the program begin: <0 for a FLASH program and >0 for a RAM program
    pgmPtr_t instructionPointer;  ///< Pointer to the program begin
  } programList_t;



  // Temporary information
  typedef enum {
    TI_NO_INFO             =  0,
    TI_RADIUS_THETA        =  1,
    TI_THETA_RADIUS        =  2,
    TI_X_Y                 =  3,
    TI_RE_IM               =  4,
    TI_STATISTIC_SUMS      =  5,
    TI_RESET               =  6,
    TI_ARE_YOU_SURE        =  7,
    TI_VERSION             =  8,
    TI_WHO                 =  9,
    TI_FALSE               = 10,
    TI_TRUE                = 11,
    TI_SHOW_REGISTER       = 12,
    TI_VIEW_REGISTER       = 13,
    TI_SUMX_SUMY           = 14,
    TI_MEANX_MEANY         = 15,
    TI_GEOMMEANX_GEOMMEANY = 16,
    TI_WEIGHTEDMEANX       = 17,
    TI_HARMMEANX_HARMMEANY = 18,
    TI_RMSMEANX_RMSMEANY   = 19,
    TI_WEIGHTEDSAMPLSTDDEV = 20,
    TI_WEIGHTEDPOPLSTDDEV  = 21,
    TI_WEIGHTEDSTDERR      = 22,
    TI_SAMPLSTDDEV         = 23,
    TI_POPLSTDDEV          = 24,
    TI_STDERR              = 25,
    TI_GEOMSAMPLSTDDEV     = 26,
    TI_GEOMPOPLSTDDEV      = 27,
    TI_GEOMSTDERR          = 28,
    TI_SAVED               = 29,
    TI_BACKUP_RESTORED     = 30,
    TI_XMIN_YMIN           = 31,
    TI_XMAX_YMAX           = 32,
    TI_DAY_OF_WEEK         = 33,
    TI_SXY                 = 34,
    TI_COV                 = 35,
    TI_CORR                = 36,
    TI_SMI                 = 37,
    TI_LR                  = 38,
    TI_CALCX               = 39,
    TI_CALCY               = 40,
    TI_CALCX2              = 41,
    TI_STATISTIC_LR        = 42,
    TI_STATISTIC_HISTO     = 43,
    TI_SA                  = 44,
    TI_INACCURATE          = 45,
    TI_UNDO_DISABLED       = 46,
    TI_SOLVER_VARIABLE     = 47,
    TI_SOLVER_FAILED       = 48,
    TI_ACC                 = 49,
    TI_ULIM                = 50,
    TI_LLIM                = 51,
    TI_INTEGRAL            = 52,
    TI_1ST_DERIVATIVE      = 53,
    TI_2ND_DERIVATIVE      = 54
  } temporaryInformation_t;



  // Rounding mode 3 bits
  typedef enum {
    rmHalfEven  = 0,
    rmHalfUp    = 1,
    rmHalfDown  = 2,
    rmUp        = 3,
    rmDown      = 4,
    rmCeil      = 5,
    rmFloor     = 6
  } roundingMode_t;



  typedef enum {
    dfAll = 0,
    dfFix = 1,
    dfSci = 2,
    dfEng = 3
  } displayFormat_t;

#endif // !TYPEDEFINITIONS_H
