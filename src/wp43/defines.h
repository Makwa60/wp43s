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

/********************************************//**
 * \file defines.h
 ***********************************************/
#if !defined(DEFINES_H)
  #define DEFINES_H


  //*********************************
  //* General configuration defines *
  //*********************************
  #define DEBUG_INSTEAD_STATUS_BAR         0 // Debug data instead of the status bar
  #define EXTRA_INFO_ON_CALC_ERROR         1 // Print extra information on the console about an error
  #define DEBUG_PANEL                !NDEBUG // Showing registers, local registers, saved stack registers, flags, statistical sums, ... in a debug panel
  #define DEBUG_REGISTER_L           !NDEBUG // Showing register L content on the PC GUI
  #define SHOW_MEMORY_STATUS         !NDEBUG // Showing the memory status on the PC GUI
  #define MMHG_PA_133_3224                 1 // mmHg to Pa conversion coefficient is 133.3224 an not 133.322387415
  #define FN_KEY_TIMEOUT_TO_NOP            0 // Set to 1 if you want the 6 function keys to timeout
  #define MAX_LONG_INTEGER_SIZE_IN_BITS 3328 // 1001 decimal digits: 3328 ≃ log2(10^1001)
  #define SHORT_INTEGER_SIZE               2 // 2 blocks = 8 bytes = 64 bits

  #define DECNUMDIGITS                    75 // Default number of digits used in the decNumber library

  #define SCREEN_800X480                   1 // Set to 0 if you want a keyboard in addition to the screen on Raspberry pi
  #if !defined(RASPBERRY)
    #undef SCREEN_800X480
    #define SCREEN_800x480 0
  #endif // !RASPBERRY


  #if defined(LINUX)
    #define _XOPEN_SOURCE                700 // see: https://stackoverflow.com/questions/5378778/what-does-d-xopen-source-do-mean
  #endif // LINUX


  #define DEBUG_STAT                       0 // PLOT & STATS verbose level can be 0, 1 or 2 (more)
  #if (DEBUG_STAT == 0)
    #undef STATDEBUG
    #undef STATDEBUG_VERBOSE
  #endif // DEBUG_STAT == 0
  #if (DEBUG_STAT == 1)
    #define STATDEBUG
    #undef STATDEBUG_VERBOSE
  #endif // DEBUG_STAT == 1
  #if (DEBUG_STAT == 2)
    #define STATDEBUG
    #define STATDEBUG_VERBOSE
  #endif // DEBUG_STAT == 2


  #if defined(PC_BUILD)
    //#define DEBUGUNDO
    #undef DEBUGUNDO
  #else // !PC_BUILD
    #undef DEBUGUNDO
  #endif // PC_BUILD


  #define REAL34_WIDTH_TEST 0 // For debugging real34 ALL 0 formating. Use UP/DOWN to shrink or enlarge the available space. The Z register holds the available width.


  //*************************
  //* Other defines         *
  //*************************
  #define CHARS_PER_LINE                            80 // Used in the flag browser application

  #define NUMERIC_FONT_HEIGHT                       36 // In pixel. Used in the font browser application
  #define STANDARD_FONT_HEIGHT                      22 // In Pixel. Used in the font browser application

  #define AIM_BUFFER_LENGTH                        400 // 199 double byte glyphs + trailing 0 + 1 byte to round up to a 4 byte boundary
  #define TAM_BUFFER_LENGTH                         32 // TODO: find the exact maximum needed
  #define NIM_BUFFER_LENGTH                        200 // TODO: find the exact maximum needed

  #define DEBUG_LINES                               68 // Used in for the debug panel

  // List of errors
  #define ERROR_NONE                                 0
  #define ERROR_ARG_EXCEEDS_FUNCTION_DOMAIN          1
  #define ERROR_BAD_TIME_OR_DATE_INPUT               2
  #define ERROR_UNDEFINED_OPCODE                     3
  #define ERROR_OVERFLOW_PLUS_INF                    4
  #define ERROR_OVERFLOW_MINUS_INF                   5
  #define ERROR_LABEL_NOT_FOUND                      6
  #define ERROR_FUNCTION_NOT_FOUND                   7
  #define ERROR_OUT_OF_RANGE                         8
  #define ERROR_INVALID_INTEGER_INPUT                9
  #define ERROR_INPUT_TOO_LONG                      10
  #define ERROR_RAM_FULL                            11
  #define ERROR_STACK_CLASH                         12
  #define ERROR_OPERATION_UNDEFINED                 13
  #define ERROR_WORD_SIZE_TOO_SMALL                 14
  #define ERROR_TOO_FEW_DATA                        15
  #define ERROR_INVALID_DISTRIBUTION_PARAM          16
  #define ERROR_IO                                  17
  #define ERROR_INVALID_CORRUPTED_DATA              18
  #define ERROR_FLASH_MEMORY_WRITE_PROTECTED        19
  #define ERROR_NO_ROOT_FOUND                       20
  #define ERROR_MATRIX_MISMATCH                     21
  #define ERROR_SINGULAR_MATRIX                     22
  #define ERROR_FLASH_MEMORY_FULL                   23
  #define ERROR_INVALID_DATA_TYPE_FOR_OP            24
  #define ERROR_WP34S_COMPAT                        25
  #define ERROR_ENTER_NEW_NAME                      26
  #define ERROR_CANNOT_DELETE_PREDEF_ITEM           27
  #define ERROR_NO_SUMMATION_DATA                   28
  #define ERROR_ITEM_TO_BE_CODED                    29
  #define ERROR_FUNCTION_TO_BE_CODED                30
  #define ERROR_INPUT_DATA_TYPE_NOT_MATCHING        31
  #define ERROR_WRITE_PROTECTED_SYSTEM_FLAG         32
  #define ERROR_STRING_WOULD_BE_TOO_LONG            33
  #define ERROR_EMPTY_STRING                        34
  #define ERROR_NO_BACKUP_DATA                      35
  #define ERROR_UNDEF_SOURCE_VAR                    36
  #define ERROR_WRITE_PROTECTED_VAR                 37
  #define ERROR_NO_MATRIX_INDEXED                   38
  #define ERROR_NOT_ENOUGH_MEMORY_FOR_NEW_MATRIX    39
  #define ERROR_NO_ERRORS_CALCULABLE                40
  #define ERROR_LARGE_DELTA_AND_OPPOSITE_SIGN       41
  #define ERROR_SOLVER_REACHED_LOCAL_EXTREMUM       42
  #define ERROR_INITIAL_GUESS_OUT_OF_DOMAIN         43
  #define ERROR_FUNCTION_VALUES_LOOK_CONSTANT       44
  #define ERROR_SYNTAX_ERROR_IN_EQUATION            45
  #define ERROR_EQUATION_TOO_COMPLEX                46
  #define ERROR_CANNOT_ASSIGN_HERE                  47
  #define ERROR_INVALID_NAME                        48
  #define ERROR_TOO_MANY_VARIABLES                  49 // unlikely
  #define ERROR_NON_PROGRAMMABLE_COMMAND            50
  #define ERROR_NO_GLOBAL_LABEL                     51
  #define ERROR_BAD_INPUT                           52 // This error is not in ReM and cannot occur (theoretically).

  #define NUMBER_OF_ERROR_CODES                     53

  #define NUMBER_OF_GLOBAL_FLAGS                   112
  #define FIRST_LOCAL_FLAG                         112 // There are 112 global flag from 0 to 111
  #define NUMBER_OF_LOCAL_FLAGS                     32
  #define LAST_LOCAL_FLAG                          143

  // Global flags
  #define FLAG_X                                   100
  #define FLAG_Y                                   101
  #define FLAG_Z                                   102
  #define FLAG_T                                   103
  #define FLAG_A                                   104
  #define FLAG_B                                   105
  #define FLAG_C                                   106
  #define FLAG_D                                   107
  #define FLAG_L                                   108
  #define FLAG_I                                   109
  #define FLAG_J                                   110
  #define FLAG_K                                   111

  // System flags
  // Bit 15 (MSB) is always set for a system flag
  // If bit 14 is set the system flag is read only for the user
  #define FLAG_TDM24                            0x8000 // The system flags
  #define FLAG_YMD                              0xc001 // MUST be in the same
  #define FLAG_DMY                              0xc002 // order as the items
  #define FLAG_MDY                              0xc003 // in items.c and items.h
  #define FLAG_CPXRES                           0x8004
  #define FLAG_CPXj                             0x8005 // And TDM24 MUST be
  #define FLAG_POLAR                            0x8006 // the first.
  #define FLAG_FRACT                            0x8007
  #define FLAG_PROPFR                           0x8008
  #define FLAG_DENANY                           0x8009
  #define FLAG_DENFIX                           0x800a
  #define FLAG_CARRY                            0x800b
  #define FLAG_OVERFLOW                         0x800c
  #define FLAG_LEAD0                            0x800d
  #define FLAG_ALPHA                            0x800e
  #define FLAG_alphaCAP                         0xc00f
  #define FLAG_RUNTIM                           0xc010
  #define FLAG_RUNIO                            0xc011
  #define FLAG_PRINTS                           0xc012
  #define FLAG_TRACE                            0x8013
  #define FLAG_USER                             0x8014
  #define FLAG_LOWBAT                           0xc015
  #define FLAG_SLOW                             0x8016
  #define FLAG_SPCRES                           0x8017
  #define FLAG_SSIZE8                           0x8018
  #define FLAG_QUIET                            0x8019
  #define FLAG_DECIMP                           0x801a
  #define FLAG_MULTx                            0x801b
  #define FLAG_ALLENG                           0x801c
  #define FLAG_GROW                             0x801d
  #define FLAG_AUTOFF                           0x801e
  #define FLAG_AUTXEQ                           0x801f
  #define FLAG_PRTACT                           0x8020
  #define FLAG_NUMIN                            0xc021
  #define FLAG_ALPIN                            0xc022
  #define FLAG_ASLIFT                           0xc023
  #define FLAG_IGN1ER                           0x8024
  #define FLAG_INTING                           0xc025
  #define FLAG_SOLVING                          0xc026
  #define FLAG_VMDISP                           0xc027
  #define FLAG_USB                              0xc028
  #define FLAG_ENDPMT                           0xc029
  #define FLAG_FRCSRN                           0x802a
  #define NUMBER_OF_SYSTEM_FLAGS                    43

  typedef enum {
    LI_ZERO     = 0, // Long integer sign 0
    LI_NEGATIVE = 1, // Long integer sign -
    LI_POSITIVE = 2  // Long integer sign +
  } longIntegerSign_t;


  // PC GUI
  #define CSSFILE                      "res/wp43_pre.css"

  #define TAM_MAX_BITS                              14
  #define TAM_MAX_MASK                          0x3fff

  // Stack Lift Status (2 bits)
  #define SLS_STATUS                            0x0003
  #define SLS_ENABLED                        ( 0 << 0) // Stack lift enabled after item execution
  #define SLS_DISABLED                       ( 1 << 0) // Stack lift disabled after item execution
  #define SLS_UNCHANGED                      ( 2 << 0) // Stack lift unchanged after item execution

  // Undo Status (2 bits)
  #define US_STATUS                             0x000c
  #define US_ENABLED                         ( 0 << 2) // The command saves the stack, the statistical sums, and the system flags for later UNDO
  #define US_CANCEL                          ( 1 << 2) // The command cancels the last UNDO data
  #define US_UNCHANGED                       ( 2 << 2) // The command leaves the existing UNDO data as is
  #define US_ENABL_XEQ                       ( 3 << 2) // Like US_STATUS, but if there is not enough memory for UNDO, deletes UNDO data then continue

  // Item category (4 bits)
  #define CAT_STATUS                            0x00f0
  #define CAT_NONE                           ( 0 << 4) // None of the others
  #define CAT_FNCT                           ( 1 << 4) // Function
  #define CAT_MENU                           ( 2 << 4) // Menu
  #define CAT_CNST                           ( 3 << 4) // Constant
  #define CAT_FREE                           ( 4 << 4) // To identify and find the free items
  #define CAT_REGS                           ( 5 << 4) // Registers
  #define CAT_RVAR                           ( 6 << 4) // Reserved variable
  #define CAT_DUPL                           ( 7 << 4) // Duplicate of another item e.g. acus->m^2
  #define CAT_SYFL                           ( 8 << 4) // System flags
  #define CAT_AINT                           ( 9 << 4) // Upper case alpha_INTL
  #define CAT_aint                           (10 << 4) // Lower case alpha_intl

  // EIM (Equation Input Mode) status (1 bit)
  #define EIM_STATUS                            0x0100
  #define EIM_DISABLED                        (0 << 8) // Function disabled in EIM
  #define EIM_ENABLED                         (1 << 8) // Function enabled in EIM

  // Parameter Type in Program status (4 bit)
  #define PTP_STATUS                            0x1e00
  #define PTP_NONE                           ( 0 << 9) // No parameters
  #define PTP_DECLARE_LABEL                  ( 1 << 9) // These
  #define PTP_LABEL                          ( 2 << 9) //   parameter
  #define PTP_REGISTER                       ( 3 << 9) //   types
  #define PTP_FLAG                           ( 4 << 9) //   must
  #define PTP_NUMBER_8                       ( 5 << 9) //   match
  #define PTP_NUMBER_16                      ( 6 << 9) //   with
  #define PTP_COMPARE                        ( 7 << 9) //   PARAM_*
  #define PTP_KEYG_KEYX                      ( 8 << 9) //   defined
  #define PTP_SKIP_BACK                      ( 9 << 9) //   below.
  #define PTP_SHUFFLE                        (10 << 9) //
  #define PTP_LITERAL                        (11 << 9) // Literal
  #define PTP_DISABLED                       (12 << 9) // Not programmable


  #define INC_FLAG                                   0
  #define DEC_FLAG                                   1

  ///////////////////////////////////////////////////////
  // Register numbering:
  //    0 to  111 global resisters
  //  112 to  210 local registers (from .00 to .98) this are 99 local registers
  //  212 to  219 saved stack registers (UNDO feature)
  //  220 to  220 temporary registers
  //  221 to 1999 named variables
  // 2000 to 2029 reserved variables
  #define REGISTER_X                               100
  #define REGISTER_Y                               101
  #define REGISTER_Z                               102
  #define REGISTER_T                               103
  #define REGISTER_A                               104
  #define REGISTER_B                               105
  #define REGISTER_C                               106
  #define REGISTER_D                               107
  #define REGISTER_L                               108
  #define REGISTER_I                               109
  #define REGISTER_J                               110
  #define REGISTER_K                               111
  #define LAST_GLOBAL_REGISTER                     111
  #define NUMBER_OF_GLOBAL_REGISTERS               112 // There are 112 global registers from 0 to 111
  #define FIRST_LOCAL_REGISTER                     112 // There are 112 global registers from 0 to 111
  #define LAST_LOCAL_REGISTER                      210 // There are maximum 99 local registers from 112 to 210 (.00 to .98)
  #define NUMBER_OF_SAVED_STACK_REGISTERS            9 // 211 to 219
  #define FIRST_SAVED_STACK_REGISTER               211
  #define SAVED_REGISTER_X                         211
  #define SAVED_REGISTER_Y                         212
  #define SAVED_REGISTER_Z                         213
  #define SAVED_REGISTER_T                         214
  #define SAVED_REGISTER_A                         215
  #define SAVED_REGISTER_B                         216
  #define SAVED_REGISTER_C                         217
  #define SAVED_REGISTER_D                         218
  #define SAVED_REGISTER_L                         219
  #define LAST_SAVED_STACK_REGISTER                219
  #define NUMBER_OF_TEMP_REGISTERS                   2 // 220, 221
  #define FIRST_TEMP_REGISTER                      220
  #define TEMP_REGISTER_1                          220
  #define TEMP_REGISTER_2_SAVED_STATS              221
  #define LAST_TEMP_REGISTER                       221
  #define FIRST_NAMED_VARIABLE                     222
  #define LAST_NAMED_VARIABLE                     1999
  #define FIRST_RESERVED_VARIABLE                 2000
  #define RESERVED_VARIABLE_X                     2000
  #define RESERVED_VARIABLE_Y                     2001
  #define RESERVED_VARIABLE_Z                     2002
  #define RESERVED_VARIABLE_T                     2003
  #define RESERVED_VARIABLE_A                     2004
  #define RESERVED_VARIABLE_B                     2005
  #define RESERVED_VARIABLE_C                     2006
  #define RESERVED_VARIABLE_D                     2007
  #define RESERVED_VARIABLE_L                     2008
  #define RESERVED_VARIABLE_I                     2009
  #define RESERVED_VARIABLE_J                     2010
  #define RESERVED_VARIABLE_K                     2011
  #define RESERVED_VARIABLE_ADM                   2012
  #define RESERVED_VARIABLE_DENMAX                2013
  #define RESERVED_VARIABLE_ISM                   2014
  #define RESERVED_VARIABLE_REALDF                2015
  #define RESERVED_VARIABLE_NDEC                  2016
  #define RESERVED_VARIABLE_ACC                   2017
  #define RESERVED_VARIABLE_ULIM                  2018
  #define RESERVED_VARIABLE_LLIM                  2019
  #define RESERVED_VARIABLE_FV                    2020
  #define RESERVED_VARIABLE_IPONA                 2021
  #define RESERVED_VARIABLE_NPER                  2022
  #define RESERVED_VARIABLE_PERONA                2023
  #define RESERVED_VARIABLE_PMT                   2024
  #define RESERVED_VARIABLE_PV                    2025
  #define RESERVED_VARIABLE_GRAMOD                2026
  #define LAST_RESERVED_VARIABLE                  2026
  #define INVALID_VARIABLE                        2027
  #define FIRST_LABEL                             2028
  #define LAST_LABEL                              6999

  #define NUMBER_OF_RESERVED_VARIABLES        (LAST_RESERVED_VARIABLE - FIRST_RESERVED_VARIABLE + 1)


  // If one of the 4 next defines is changed: change also Y_POSITION_OF_???_LINE below
  #define AIM_REGISTER_LINE                 REGISTER_X
  #define TAM_REGISTER_LINE                 REGISTER_T
  #define NIM_REGISTER_LINE                 REGISTER_X // MUST be REGISTER_X
  #define ERR_REGISTER_LINE                 REGISTER_Z
  #define TRUE_FALSE_REGISTER_LINE          REGISTER_Z

  // If one of the 4 next defines is changed: change also ???_REGISTER_LINE above
  #define Y_POSITION_OF_AIM_LINE        Y_POSITION_OF_REGISTER_X_LINE
  #define Y_POSITION_OF_TAM_LINE        Y_POSITION_OF_REGISTER_T_LINE
  #define Y_POSITION_OF_NIM_LINE        Y_POSITION_OF_REGISTER_X_LINE
  #define Y_POSITION_OF_ERR_LINE        Y_POSITION_OF_REGISTER_Z_LINE
  #define Y_POSITION_OF_TRUE_FALSE_LINE Y_POSITION_OF_REGISTER_Z_LINE



  #define SCREEN_WIDTH                             400 // Width of the screen
  #define SCREEN_HEIGHT                            240 // Height of the screen
  #define ON_PIXEL                            0x303030 // blue red green
  #define OFF_PIXEL                           0xe0e0e0 // blue red green
  #define SOFTMENU_STACK_SIZE                        8
  #define TEMPORARY_INFO_OFFSET                     10 // Vertical offset for temporary informations. I find 4 looks better
  #define REGISTER_LINE_HEIGHT                      36

  #define Y_POSITION_OF_REGISTER_T_LINE             24 // 135 - REGISTER_LINE_HEIGHT*(registerNumber - REGISTER_X)
  #define Y_POSITION_OF_REGISTER_Z_LINE             60
  #define Y_POSITION_OF_REGISTER_Y_LINE             96
  #define Y_POSITION_OF_REGISTER_X_LINE            132

  #define NUMBER_OF_DYNAMIC_SOFTMENUS               19
  #define SOFTMENU_HEIGHT                           23

  // Horizontal offsets in the status bar
  #define X_DATE                                     1
  #define X_REAL_COMPLEX                           133
  #define X_COMPLEX_MODE                           143
  #define X_ANGULAR_MODE                           157
  #define X_FRAC_MODE                              185
  #define X_INTEGER_MODE                           260
  #define X_OVERFLOW_CARRY                         292
  #define X_ALPHA_MODE                             300
  #define X_HOURGLASS                              311
  #define X_PROGRAM_BEGIN                          324
  #define X_WATCH                                  336
  #define X_SERIAL_IO                              351
  #define X_PRINTER                                361
  #define X_USER_MODE                              375
  #define X_BATTERY                                389

  #define TIMER_IDX_REFRESH_SLEEP                    0 // use timer 0 to wake up for screen refresh
  //#define TIMER_IDX_AUTO_REPEAT                    1 // use timer 1 to wake up for key auto-repeat

  #define TMR_NUMBER                                 4

  // timer
  #define TO_AUTO_REPEAT                             0
  #define TO_TIMER_APP                               1
  #define TO_KB_ACTV                                 2
  #define TO_SHOW_NOP                                3


  #if defined(PC_BUILD)
    #if defined(LINUX)
      #define LINEBREAK                           "\n"
    #elif defined(WIN32)
      #define LINEBREAK                         "\n\r"
    #elif defined(OSX)
      #define LINEBREAK                         "\r\n"
    #else // Unsupported OS
      #error Only Linux, MacOS, and Windows MINGW64 are supported for now
    #endif // OS
  #endif // PC_BUILD

  #define NUMBER_OF_DISPLAY_DIGITS                  16
  #define NUMBER_OF_DATA_TYPES_FOR_CALCULATIONS     10

  // Number of constants
  #define NUMBER_OF_CONSTANTS_39                   198
  #define NUMBER_OF_CONSTANTS_51                    39
  #define NUMBER_OF_CONSTANTS_1071                   1
  #define NUMBER_OF_CONSTANTS_34                    44

  #define MAX_FREE_REGION                           50 // Maximum number of free memory regions

  // On/Off 1 bit
  #define OFF                                        0
  #define ON                                         1

  // Short integer mode 2 bits
  #define SIM_UNSIGN                                 0
  #define SIM_1COMPL                                 1
  #define SIM_2COMPL                                 2
  #define SIM_SIGNMT                                 3

  // Display format 2 bits
  #define DF_ALL                                     0
  #define DF_FIX                                     1
  #define DF_SCI                                     2
  #define DF_ENG                                     3

  // Date format 2 bits
  #define DF_DMY                                     0
  #define DF_YMD                                     1
  #define DF_MDY                                     2

  // Curve fitting 10 bits
  #define CF_LINEAR_FITTING                          1
  #define CF_EXPONENTIAL_FITTING                     2
  #define CF_LOGARITHMIC_FITTING                     4
  #define CF_POWER_FITTING                           8
  #define CF_ROOT_FITTING                           16
  #define CF_HYPERBOLIC_FITTING                     32
  #define CF_PARABOLIC_FITTING                      64
  #define CF_CAUCHY_FITTING                        128
  #define CF_GAUSS_FITTING                         256
  #define CF_ORTHOGONAL_FITTING                    512

  // Curve fitting excluding all other curve fitting bits, 10 bits
  #define CF_LINEAR_FITTING_EX                     (~CF_LINEAR_FITTING) & 0x01FF
  #define CF_EXPONENTIAL_FITTING_EX                (~CF_EXPONENTIAL_FITTING) & 0x01FF
  #define CF_LOGARITHMIC_FITTING_EX                (~CF_LOGARITHMIC_FITTING) & 0x01FF
  #define CF_POWER_FITTING_EX                      (~CF_POWER_FITTING) & 0x03FF
  #define CF_ROOT_FITTING_EX                       (~CF_ROOT_FITTING) & 0x01FF
  #define CF_HYPERBOLIC_FITTING_EX                 (~CF_HYPERBOLIC_FITTING) & 0x01FF
  #define CF_PARABOLIC_FITTING_EX                  (~CF_PARABOLIC_FITTING) & 0x01FF
  #define CF_CAUCHY_FITTING_EX                     (~CF_CAUCHY_FITTING) & 0x01FF
  #define CF_GAUSS_FITTING_EX                      (~CF_GAUSS_FITTING) & 0x01FF
  #define CF_ORTHOGONAL_FITTING_EX                 (~CF_ORTHOGONAL_FITTING) & 0x01FF

  // Plot curve fitting 4 bits
  #define PLOT_ORTHOF                                0
  #define PLOT_NXT                                   1
  #define PLOT_REV                                   2
  #define PLOT_LR                                    3
  #define PLOT_START                                 4
  #define PLOT_NOTHING                               5
  #define PLOT_GRAPH                                 6
  #define H_PLOT                                     7
  #define H_NORM                                     8

  // Rounding mode 3 bits
  #define RM_HALF_EVEN                               0
  #define RM_HALF_UP                                 1
  #define RM_HALF_DOWN                               2
  #define RM_UP                                      3
  #define RM_DOWN                                    4
  #define RM_CEIL                                    5
  #define RM_FLOOR                                   6

  // Next character in AIM 2 bits
  #define NC_NORMAL                                  0
  #define NC_SUBSCRIPT                               1
  #define NC_SUPERSCRIPT                             2

  // Alpha case 1 bit
  #define AC_UPPER                                   0
  #define AC_LOWER                                   1

  // TAM mode
  #define TM_VALUE                               10001 // TM_VALUE must be the 1st in this list
  #define TM_VALUE_CHB                           10002 // same as TM_VALUE but for ->INT (#) change base
  #define TM_REGISTER                            10003
  #define TM_FLAGR                               10004
  #define TM_FLAGW                               10005
  #define TM_STORCL                              10006
  #define TM_M_DIM                               10007
  #define TM_SHUFFLE                             10008
  #define TM_LABEL                               10009
  #define TM_SOLVE                               10010
  #define TM_NEWMENU                             10011
  #define TM_KEY                                 10012
  #define TM_INTEGRATE                           10013
  #define TM_CMP                                 10014 // TM_CMP must be the last in this list

  // NIM number part
  #define NP_EMPTY                                   0
  #define NP_INT_10                                  1 // Integer base 10
  #define NP_INT_16                                  2 // Integer base > 10
  #define NP_INT_BASE                                3 // Integer: the base
  #define NP_REAL_FLOAT_PART                         4 // Decimal part of the real
  #define NP_REAL_EXPONENT                           5 // Ten exponent of the real
  #define NP_FRACTION_DENOMINATOR                    6 // Denominator of the fraction
  #define NP_COMPLEX_INT_PART                        7 // Integer part of the complex imaginary part
  #define NP_COMPLEX_FLOAT_PART                      8 // Decimal part of the complex imaginary part
  #define NP_COMPLEX_EXPONENT                        9 // Ten exponent of the complex imaginary part

  // Temporary information
  #define TI_NO_INFO                                 0
  #define TI_RADIUS_THETA                            1
  #define TI_THETA_RADIUS                            2
  #define TI_X_Y                                     3
  #define TI_RE_IM                                   4
  #define TI_STATISTIC_SUMS                          5
  #define TI_RESET                                   6
  #define TI_ARE_YOU_SURE                            7
  #define TI_VERSION                                 8
  #define TI_WHO                                     9
  #define TI_FALSE                                  10
  #define TI_TRUE                                   11
  #define TI_SHOW_REGISTER                          12
  #define TI_VIEW_REGISTER                          13
  #define TI_SUMX_SUMY                              14
  #define TI_MEANX_MEANY                            15
  #define TI_GEOMMEANX_GEOMMEANY                    16
  #define TI_WEIGHTEDMEANX                          17
  #define TI_HARMMEANX_HARMMEANY                    18
  #define TI_RMSMEANX_RMSMEANY                      19
  #define TI_WEIGHTEDSAMPLSTDDEV                    20
  #define TI_WEIGHTEDPOPLSTDDEV                     21
  #define TI_WEIGHTEDSTDERR                         22
  #define TI_SAMPLSTDDEV                            23
  #define TI_POPLSTDDEV                             24
  #define TI_STDERR                                 25
  #define TI_GEOMSAMPLSTDDEV                        26
  #define TI_GEOMPOPLSTDDEV                         27
  #define TI_GEOMSTDERR                             28
  #define TI_SAVED                                  29
  #define TI_BACKUP_RESTORED                        30
  #define TI_XMIN_YMIN                              31
  #define TI_XMAX_YMAX                              32
  #define TI_DAY_OF_WEEK                            33
  #define TI_SXY                                    34
  #define TI_COV                                    35
  #define TI_CORR                                   36
  #define TI_SMI                                    37
  #define TI_LR                                     38
  #define TI_CALCX                                  39
  #define TI_CALCY                                  40
  #define TI_CALCX2                                 41
  #define TI_STATISTIC_LR                           42
  #define TI_STATISTIC_HISTO                        43
  #define TI_SA                                     44
  #define TI_INACCURATE                             45
  #define TI_UNDO_DISABLED                          46
  #define TI_SOLVER_VARIABLE                        47
  #define TI_SOLVER_FAILED                          48
  #define TI_ACC                                    49
  #define TI_ULIM                                   50
  #define TI_LLIM                                   51
  #define TI_INTEGRAL                               52
  #define TI_1ST_DERIVATIVE                         53
  #define TI_2ND_DERIVATIVE                         54

  // Register browser mode
  #define RBR_GLOBAL                                 0 // Global registers are browsed
  #define RBR_LOCAL                                  1 // Local registers are browsed
  #define RBR_NAMED                                  2 // Named variables are browsed

  // Debug window
  #define DBG_BIT_FIELDS                             0
  #define DBG_FLAGS                                  1
  #define DBG_REGISTERS                              2
  #define DBG_LOCAL_REGISTERS                        3
  #define DBG_STATISTICAL_SUMS                       4
  #define DBG_NAMED_VARIABLES                        5
  #define DBG_TMP_SAVED_STACK_REGISTERS              6

  // alpha selection menus
  #define CATALOG_NONE                               0 // CATALOG_NONE must be 0
  #define CATALOG_CNST                               1
  #define CATALOG_FCNS                               2
  #define CATALOG_MENU                               3
  #define CATALOG_SYFL                               4
  #define CATALOG_AINT                               5
  #define CATALOG_aint                               6
  #define CATALOG_PROG                               7
  #define CATALOG_VAR                                8
  #define CATALOG_MATRS                              9
  #define CATALOG_STRINGS                           10
  #define CATALOG_DATES                             11
  #define CATALOG_TIMES                             12
  #define CATALOG_ANGLES                            13
  #define CATALOG_SINTS                             14
  #define CATALOG_LINTS                             15
  #define CATALOG_REALS                             16
  #define CATALOG_CPXS                              17
  #define CATALOG_MVAR                              18
  #define NUMBER_OF_CATALOGS                        19

  // String comparison type
  #define CMP_BINARY                                 0
  #define CMP_CLEANED_STRING_ONLY                    1
  #define CMP_EXTENSIVE                              2
  #define CMP_NAME                                   3

  // Indirect parameter mode
  #define INDPM_PARAM                                0
  #define INDPM_REGISTER                             1
  #define INDPM_FLAG                                 2

  // Combination / permutation
  #define CP_PERMUTATION                             0
  #define CP_COMBINATION                             1

  // Gudermannian
  #define GD_DIRECT_FUNCTION                         0
  #define GD_INVERSE_FUNCTION                        1

  // Program running mode
  #define PGM_STOPPED                                0
  #define PGM_RUNNING                                1
  #define PGM_WAITING                                2
  #define PGM_PAUSED                                 3
  #define PGM_KEY_PRESSED_WHILE_PAUSED               4
  #define PGM_RESUMING                               5

  // Load mode
  #define LM_ALL                                     0
  #define LM_PROGRAMS                                1
  #define LM_REGISTERS                               2
  #define LM_NAMED_VARIABLES                         3
  #define LM_SUMS                                    4
  #define LM_SYSTEM_STATE                            5
  #define LM_REGISTERS_PARTIAL                       6

  // Screen updating mode
  #define SCRUPD_AUTO                             0x00
  #define SCRUPD_MANUAL_STATUSBAR                 0x01
  #define SCRUPD_MANUAL_STACK                     0x02
  #define SCRUPD_MANUAL_MENU                      0x04
  #define SCRUPD_MANUAL_SHIFT_STATUS              0x08
  //#define SCRUPD_SKIP_STATUSBAR_ONE_TIME          0x10
  #define SCRUPD_SKIP_STACK_ONE_TIME              0x20
  #define SCRUPD_SKIP_MENU_ONE_TIME               0x40
  //#define SCRUPD_SHIFT_STATUS                     0x80
  #define SCRUPD_ONE_TIME_FLAGS                   0xf0

  // Statistical sums TODO: optimize size of SIGMA_N, _X, _Y, _XMIN, _XMAX, _YMIN, and _YMAX. Thus, saving 2×(7×60 - 4 - 6×16) = 640 bytes
  #define SUM_X                                      1
  #define SUM_Y                                      2
  #define SUM_X2                                     3
  #define SUM_X2Y                                    4
  #define SUM_Y2                                     5
  #define SUM_XY                                     6
  #define SUM_lnXlnY                                 7
  #define SUM_lnX                                    8
  #define SUM_ln2X                                   9
  #define SUM_YlnX                                  10
  #define SUM_lnY                                   11
  #define SUM_ln2Y                                  12
  #define SUM_XlnY                                  13
  #define SUM_X2lnY                                 14
  #define SUM_lnYonX                                15
  #define SUM_X2onY                                 16
  #define SUM_1onX                                  17
  #define SUM_1onX2                                 18
  #define SUM_XonY                                  19
  #define SUM_1onY                                  20
  #define SUM_1onY2                                 21
  #define SUM_X3                                    22
  #define SUM_X4                                    23
  #define SUM_XMIN                                  24
  #define SUM_XMAX                                  25
  #define SUM_YMIN                                  26
  #define SUM_YMAX                                  27

  #define NUMBER_OF_STATISTICAL_SUMS                28
  #define SIGMA_N      ((real_t *)(statisticalSumsPointer)) // could be a 32 bit unsigned integer
  #define SIGMA_X      ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_X     )) // could be a real34
  #define SIGMA_Y      ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_Y     )) // could be a real34
  #define SIGMA_X2     ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_X2    ))
  #define SIGMA_X2Y    ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_X2Y   ))
  #define SIGMA_Y2     ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_Y2    ))
  #define SIGMA_XY     ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_XY    ))
  #define SIGMA_lnXlnY ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_lnXlnY))
  #define SIGMA_lnX    ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_lnX   ))
  #define SIGMA_ln2X   ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_ln2X  ))
  #define SIGMA_YlnX   ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_YlnX  ))
  #define SIGMA_lnY    ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_lnY   ))
  #define SIGMA_ln2Y   ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_ln2Y  ))
  #define SIGMA_XlnY   ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_XlnY  ))
  #define SIGMA_X2lnY  ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_X2lnY ))
  #define SIGMA_lnYonX ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_lnYonX))
  #define SIGMA_X2onY  ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_X2onY ))
  #define SIGMA_1onX   ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_1onX  ))
  #define SIGMA_1onX2  ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_1onX2 ))
  #define SIGMA_XonY   ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_XonY  ))
  #define SIGMA_1onY   ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_1onY  ))
  #define SIGMA_1onY2  ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_1onY2 ))
  #define SIGMA_X3     ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_X3    ))
  #define SIGMA_X4     ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_X4    ))
  #define SIGMA_XMIN   ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_XMIN  )) // could be a real34
  #define SIGMA_XMAX   ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_XMAX  )) // could be a real34
  #define SIGMA_YMIN   ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_YMIN  )) // could be a real34
  #define SIGMA_YMAX   ((real_t *)(statisticalSumsPointer + REAL_SIZE * SUM_YMAX  )) // could be a real34

  #define MAX_NUMBER_OF_GLYPHS_IN_STRING           196

  #define MAX_DENMAX                              9999 // Biggest denominator in fraction display mode

  #define FAST_SCREEN_REFRESH_PERIOD               100 // in milliseconds
  #define SCREEN_REFRESH_PERIOD                    500 // in milliseconds
  #define KEY_AUTOREPEAT_FIRST_PERIOD              400 // in milliseconds
  #define KEY_AUTOREPEAT_PERIOD                    200 // in milliseconds
  #define TIMER_APP_PERIOD                         100 // in milliseconds
  #define RAM_SIZE                               16384 // 16384 blocks = 65536 bytes  MUST be a multiple of 4 and MUST be <= 262140 (not 262144)
  //#define RAM_SIZE                                3072 // 16384 blocks = 65536 bytes  MUST be a multiple of 4 and MUST be <= 262140 (not 262144)

  #define CONFIG_SIZE            TO_BLOCKS(sizeof(dtConfigDescriptor_t))

  #define FLASH_PGM_PAGE_SIZE                      512
  #define FLASH_PGM_NUMBER_OF_PAGES                 64

  // Type of constant stored in a program
  #define BINARY_SHORT_INTEGER                       1
  #define BINARY_LONG_INTEGER                        2
  #define BINARY_REAL34                              3
  #define BINARY_COMPLEX34                           4
  #define BINARY_DATE                                5
  #define BINARY_TIME                                6
  #define STRING_SHORT_INTEGER                       7
  #define STRING_LONG_INTEGER                        8
  #define STRING_REAL34                              9
  #define STRING_COMPLEX34                          10
  #define STRING_TIME                               11
  #define STRING_DATE                               12
  //#define BINARY_ANGLE_RADIAN                       13
  //#define BINARY_ANGLE_GRAD                         14
  //#define BINARY_ANGLE_DEGREE                       15
  //#define BINARY_ANGLE_DMS                          16
  //#define BINARY_ANGLE_MULTPI                       17
  //#define STRING_ANGLE_RADIAN                       18
  //#define STRING_ANGLE_GRAD                         19
  //#define STRING_ANGLE_DEGREE                       20
  #define STRING_ANGLE_DMS                          21
  //#define STRING_ANGLE_MULTPI                       22

  // OP parameter special values
  #define SYSTEM_FLAG_NUMBER                       250
  #define VALUE_0                                  251
  #define VALUE_1                                  252
  #define STRING_LABEL_VARIABLE                    253
  #define INDIRECT_REGISTER                        254
  #define INDIRECT_VARIABLE                        255

  // OP parameter type
  #define PARAM_DECLARE_LABEL                        1
  #define PARAM_LABEL                                2
  #define PARAM_REGISTER                             3
  #define PARAM_FLAG                                 4
  #define PARAM_NUMBER_8                             5
  #define PARAM_NUMBER_16                            6
  #define PARAM_COMPARE                              7
  #define PARAM_KEYG_KEYX                            8
  #define PARAM_SKIP_BACK                            9
  #define PARAM_SHUFFLE                             10

  #define CHECK_INTEGER                              0
  #define CHECK_INTEGER_EVEN                         1
  #define CHECK_INTEGER_ODD                          2
  #define CHECK_INTEGER_FP                           3

  #define CHECK_VALUE_COMPLEX                        0
  #define CHECK_VALUE_REAL                           1
  #define CHECK_VALUE_POSITIVE_ZERO                  2
  #define CHECK_VALUE_NEGATIVE_ZERO                  3
  #define CHECK_VALUE_SPECIAL                        4
  #define CHECK_VALUE_NAN                            5
  #define CHECK_VALUE_INFINITY                       6
  #define CHECK_VALUE_MATRIX                         7
  #define CHECK_VALUE_MATRIX_SQUARE                  8

  #define OPMOD_MULTIPLY                             0
  #define OPMOD_POWER                                1

  #define ORTHOPOLY_HERMITE_H                        0
  #define ORTHOPOLY_HERMITE_HE                       1
  #define ORTHOPOLY_LAGUERRE_L                       2
  #define ORTHOPOLY_LAGUERRE_L_ALPHA                 3
  #define ORTHOPOLY_LEGENDRE_P                       4
  #define ORTHOPOLY_CHEBYSHEV_T                      5
  #define ORTHOPOLY_CHEBYSHEV_U                      6

  #define QF_NEWTON_F                                0
  #define QF_NEWTON_POISSON                          1
  #define QF_NEWTON_BINOMIAL                         2
  #define QF_NEWTON_GEOMETRIC                        3
  #define QF_NEWTON_NEGBINOM                         4
  #define QF_NEWTON_HYPERGEOMETRIC                   5

  #define QF_DISCRETE_CDF_POISSON                    0
  #define QF_DISCRETE_CDF_BINOMIAL                   1
  #define QF_DISCRETE_CDF_GEOMETRIC                  2
  #define QF_DISCRETE_CDF_NEGBINOM                   3
  #define QF_DISCRETE_CDF_HYPERGEOMETRIC             4

  #define SOLVER_STATUS_READY_TO_EXECUTE             0x0001
  #define SOLVER_STATUS_INTERACTIVE                  0x0002
  #define SOLVER_STATUS_EQUATION_MODE                0x000c
  #define SOLVER_STATUS_EQUATION_SOLVER              0x0000
  #define SOLVER_STATUS_EQUATION_INTEGRATE           0x0004
  #define SOLVER_STATUS_EQUATION_1ST_DERIVATIVE      0x0008
  #define SOLVER_STATUS_EQUATION_2ND_DERIVATIVE      0x000C
  #define SOLVER_STATUS_USES_FORMULA                 0x0100
  #define SOLVER_STATUS_MVAR_BEING_OPENED            0x0200
  #define SOLVER_STATUS_TVM_APPLICATION              0x1000

  #define SOLVER_RESULT_NORMAL                       0
  #define SOLVER_RESULT_SIGN_REVERSAL                1
  #define SOLVER_RESULT_EXTREMUM                     2
  #define SOLVER_RESULT_BAD_GUESS                    3
  #define SOLVER_RESULT_CONSTANT                     4
  #define SOLVER_RESULT_OTHER_FAILURE                5

  #define ASSIGN_NAMED_VARIABLES                 10000
  #define ASSIGN_LABELS                          12000
  #define ASSIGN_RESERVED_VARIABLES                  (ASSIGN_NAMED_VARIABLES + FIRST_RESERVED_VARIABLE - FIRST_NAMED_VARIABLE)
  #define ASSIGN_USER_MENU                     (-10000)
  #define ASSIGN_CLEAR                         (-32768)

  #define TIMER_APP_STOPPED                          0xFFFFFFFFu

  #if !defined(DMCP_BUILD)
    #define TO_QSPI
  #else // DMCP_BUILD
    #define beep(frequence, length)            {while(get_beep_volume() < 11) beep_volume_up(); start_buzzer_freq(frequence * 1000); sys_delay(length); stop_buzzer();}
    #define TO_QSPI                            __attribute__ ((section(".qspi")))
  #endif // !DMCP_BUILD


  //******************************
  //* Macros replacing functions *
  //******************************
  #if (EXTRA_INFO_ON_CALC_ERROR == 0) || defined(TESTSUITE_BUILD) || defined(DMCP_BUILD)
    #define EXTRA_INFO_MESSAGE(function, msg)
  #else // EXTRA_INFO_ON_CALC_ERROR != 0 && !TESTSUITE_BUILD && !DMCP_BUILD
    #define EXTRA_INFO_MESSAGE(function, msg)  {sprintf(errorMessage, msg); moreInfoOnError("In function ", function, errorMessage, NULL);}
  #endif // EXTRA_INFO_ON_CALC_ERROR == 0 || TESTSUITE_BUILD || DMCP_BUILD

  #define isSystemFlagWriteProtected(sf)       ((sf & 0x4000) != 0)
  #define getSystemFlag(sf)                    ((systemFlags &   ((uint64_t)1 << (sf & 0x3fff))) != 0)
  #define setSystemFlag(sf)                    { systemFlags |=  ((uint64_t)1 << (sf & 0x3fff)); systemFlagAction(sf, 1); }
  #define clearSystemFlag(sf)                  { systemFlags &= ~((uint64_t)1 << (sf & 0x3fff)); systemFlagAction(sf, 0); }
  #define flipSystemFlag(sf)                   { systemFlags ^=  ((uint64_t)1 << (sf & 0x3fff)); systemFlagAction(sf, 2); }
  #define shortIntegerIsZero(op)               (((*(uint64_t *)(op)) == 0) || (shortIntegerMode == SIM_SIGNMT && (((*(uint64_t *)(op)) == 1u<<((uint64_t)shortIntegerWordSize-1)))))
  #define getStackTop()                        (getSystemFlag(FLAG_SSIZE8) ? REGISTER_D : REGISTER_T)
  #define freeRegisterData(regist)             freeWp43((void *)getRegisterDataPointer(regist), getRegisterFullSize(regist))
  #define storeToDtConfigDescriptor(config)    (configToStore->config = config)
  #define recallFromDtConfigDescriptor(config) (config = configToRecall->config)
  #define getRecalledSystemFlag(sf)            ((configToRecall->systemFlags &   ((uint64_t)1 << (sf & 0x3fff))) != 0)
  #define TO_BLOCKS(n)                         (((n) + 3) >> 2)
  #define TO_BYTES(n)                          ((n) << 2)
  #define WP43_NULL                            65535 // NULL pointer
  #define TO_PCMEMPTR(p)                       ((void *)((p) == WP43_NULL ? NULL : ram + (p)))
  #define TO_WP43MEMPTR(p)                     ((p) == NULL ? WP43_NULL : (uint16_t)((dataBlock_t *)(p) - ram))
  #define min(a,b)                             ((a)<(b)?(a):(b))
  #define max(a,b)                             ((a)>(b)?(a):(b))
  #define rmd(n, d)                            ((n)%(d))                                                       // rmd(n,d) = n - d*idiv(n,d)   where idiv is the division with decimal part truncature
  #define mod(n, d)                            (((n)%(d) + (d)) % (d))                                         // mod(n,d) = n - d*floor(n/d)  where floor(a) is the biggest integer <= a
  //#define modulo(n, d)                         ((n)%(d)<0 ? ((d)<0 ? (n)%(d) - (d) : (n)%(d) + (d)) : (n)%(d)) // modulo(n,d) = rmd(n,d) (+ |d| if rmd(n,d)<0)  thus the result is always >= 0
  #define modulo(n, d)                         ((n)%(d)<0 ? (n)%(d)+(d) : (n)%(d))                             // This version works only if d > 0
  #define COMPLEX_UNIT                         (getSystemFlag(FLAG_CPXj)   ? STD_j     : STD_i)
  #define RADIX34_MARK_CHAR                    (getSystemFlag(FLAG_DECIMP) ? '.'       : ',')
  #define RADIX34_MARK_STRING                  (getSystemFlag(FLAG_DECIMP) ? "."       : ",")
  #define PRODUCT_SIGN                         (getSystemFlag(FLAG_MULTx)  ? STD_CROSS : STD_DOT)
  #define currentReturnProgramNumber           (currentSubroutineLevelData[0].returnProgramNumber)
  #define currentReturnLocalStep               (currentSubroutineLevelData[0].returnLocalStep)
  #define currentNumberOfLocalFlags            (currentSubroutineLevelData[1].numberOfLocalFlags)
  #define currentNumberOfLocalRegisters        (currentSubroutineLevelData[1].numberOfLocalRegisters)
  #define currentSubroutineLevel               (currentSubroutineLevelData[1].subroutineLevel)
  #define currentPtrToNextLevel                (currentSubroutineLevelData[2].ptrToNextLevel)
  #define currentPtrToPreviousLevel            (currentSubroutineLevelData[2].ptrToPreviousLevel)

  #if !defined(PC_BUILD) && !defined(DMCP_BUILD)
    #error One of PC_BUILD and DMCP_BUILD must be defined
  #endif // !PC_BUILD && !DMCP_BUILD

  #if defined(PC_BUILD) && defined(DMCP_BUILD)
    #error Only one of PC_BUILD and DMCP_BUILD must be defined
  #endif // PC_BUILD && DMCP_BUILD

  #if !defined(OS32BIT) && !defined(OS64BIT)
    #error One of OS32BIT and OS64BIT must be defined
  #endif // !OS32BIT && !OS64BIT

  #if defined(OS32BIT) && defined(OS64BIT)
    #error Only one of OS32BIT and OS64BIT must be defined
  #endif // OS32BIT && OS64BIT

  #if defined(PC_BUILD) && defined(RASPBERRY)
    // No DEBUG_PANEL mode for Raspberry Pi
    #undef  DEBUG_PANEL
    #define DEBUG_PANEL 0
  #endif // PC_BUILD && RASPBERRY

  #if defined(DMCP_BUILD) || (SCREEN_800X480 == 1)
    #undef  DEBUG_PANEL
    #define DEBUG_PANEL 0
    #undef  DEBUG_REGISTER_L
    #define DEBUG_REGISTER_L 0
    #undef  SHOW_MEMORY_STATUS
    #define SHOW_MEMORY_STATUS 0
    #undef  EXTRA_INFO_ON_CALC_ERROR
    #define EXTRA_INFO_ON_CALC_ERROR 0
  #endif // DMCP_BUILD || SCREEN_800X480 == 1

  #if defined(TESTSUITE_BUILD) && !defined(GENERATE_CATALOGS)
    #undef  PC_BUILD
    #undef  DMCP_BUILD
    #undef  DEBUG_PANEL
    #define DEBUG_PANEL 0
    #undef  DEBUG_REGISTER_L
    #define DEBUG_REGISTER_L 0
    #undef  SHOW_MEMORY_STATUS
    #define SHOW_MEMORY_STATUS 0
    #undef  EXTRA_INFO_ON_CALC_ERROR
    #define EXTRA_INFO_ON_CALC_ERROR 0
    #define addItemToBuffer fnNop
    #define fnOff           fnNop
    #define fnAim           fnNop
    #define refreshRegisterLine(a)  {}
    #define displayBugScreen(a)     { printf("\n-----------------------------------------------------------------------\n"); printf("%s\n", a); printf("\n-----------------------------------------------------------------------\n");}
    #define showHideHourGlass()     {}
    #define refreshScreen()         {}
  #endif // TESTSUITE_BUILD && !GENERATE_CATALOGS

  /* Turn off -Wunused-result for a specific function call */
  #if defined(OS32BIT)
    #define ignore_result(M) if(1==((uint32_t)M)){;}
  #else // !OS32BIT
    #define ignore_result(M) if(1==((uint64_t)M)){;}
  #endif // OS32BIT

  #if defined(DMCP_BUILD)
    #define TMP_STR_LENGTH       AUX_BUF_SIZE
  #else // !DMCP_BUILD
    #define TMP_STR_LENGTH       2560
  #endif // DMCP_BUILD
  #define WRITE_BUFFER_LEN       4096
  #define ERROR_MESSAGE_LENGTH    512
  #define DISPLAY_VALUE_LEN        80

  //************************
  //* Macros for debugging *
  //************************
  #define TEST_REG(r, comment) { \
                                 if(globalRegister[r].dataPointer >= 500) { \
                                   uint32_t a, b; \
                                   a = 1; \
                                   b = 0; \
                                   printf("\n=====> BAD  REGISTER %d DATA POINTER: %u <===== %s\n", r, globalRegister[r].dataPointer, comment); \
                                   globalRegister[r].dataType = a/b; \
                                 } \
                                 else { \
                                   printf("\n=====> good register %d data pointer: %u <===== %s\n", r, globalRegister[r].dataPointer, comment); \
                                 } \
                               }

  #define PRINT_LI(lint, comment) { \
                                    int i; \
                                    printf("\n%s", comment); \
                                    if((lint)->_mp_size == 0) printf(" lint=0"); \
                                    else if((lint)->_mp_size < 0) printf(" lint=-"); \
                                    else printf(" lint=+"); \
                                    for(i=0; i<abs((lint)->_mp_size); i++) { \
                                      printf("%lu ", (unsigned long)((lint)->_mp_d[i])); \
                                    } \
                                    printf("  _mp_alloc=%dlimbs=", (lint)->_mp_alloc); \
                                    printf("%lubytes", LIMB_SIZE * (lint)->_mp_alloc); \
                                    printf(" _mp_size=%dlimbs=", abs((lint)->_mp_size)); \
                                    printf("%lubytes", LIMB_SIZE * abs((lint)->_mp_size)); \
                                    printf(" PCaddress=%p", (lint)->_mp_d); \
                                    printf(" 43address=%d", TO_WP43MEMPTR((lint)->_mp_d)); \
                                    printf("\n"); \
                                  }


  #define PRINT_LI_REG(reg, comment) { \
                                       int i; \
                                       mp_limb_t *p; \
                                       printf("\n%s", comment); \
                                       if(getRegisterLongIntegerSign(reg) == LI_ZERO) printf("lint=0"); \
                                       else if(getRegisterLongIntegerSign(reg) == LI_NEGATIVE) printf("lint=-"); \
                                       else printf("lint=+"); \
                                       for(i=*REGISTER_DATA_MAX_LEN(reg)/LIMB_SIZE, p=REGISTER_LONG_INTEGER_DATA(reg); i>0; i--, p++) { \
                                         printf("%lu ", *p); \
                                       } \
                                       printf(" maxLen=%dbytes=", *REGISTER_DATA_MAX_LEN(reg)); \
                                       printf("%lulimbs", *REGISTER_DATA_MAX_LEN(reg) / LIMB_SIZE); \
                                       printf("\n"); \
                                      }
#endif // !DEFINES_H
