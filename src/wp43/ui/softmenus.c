// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "ui/softmenus.h"

#include "apps/bugScreen.h"
#include "assign.h"
#include "calcMode.h"
#include "charString.h"
#include "core/memory.h"
#include "error.h"
#include "flags.h"
#include "hal/lcd.h"
#include "items.h"
#include "programming/manage.h"
#include "programming/nextStep.h"
#include "registers.h"
#include "solver/differentiate.h"
#include "solver/equation.h"
#include "sort.h"
#include "ui/cursor.h"
#include "ui/screen.h"
#include "ui/tam.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "wp43.h"

smStackMode_t smStackMode[SOFTMENU_STACKMODE_SIZE];  // Softmenu stack mode stack - depth should be minimum 3 (normal -> pem -> aim)

/* The numbers refer to the index of items in items.c
 *         item <     0  ==>  sub menu
 *     0 < item <  9999  ==>  item with top and bottom line
 * 10000 < item < 19999  ==>  item without top line
 * 20000 < item < 29999  ==>  item without bottom line
 * 30000 < item < 39999  ==>  item without top and bottom line
 */

/*      Menu name                           <----------------------------------------------------------------------------- 6 functions ---------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 f shifted functions ------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 g shifted functions ------------------------------------------------------------------------->  */
TO_QSPI const int16_t menu_ADV[]         = { ITM_SOLVE,                     ITM_SLVQ,                   ITM_FQX,                  ITM_PIn,               ITM_SIGMAn,                  -MNU_Sfdx,
                                             ITM_PGMSLV,                    ITM_NULL,                   ITM_FDQX,                 ITM_NULL,              ITM_NULL,                    ITM_PGMINT                    };

TO_QSPI const int16_t menu_Sfdx[]        = { ITM_NULL,                      ITM_DRAW,                   VAR_ACC,                  VAR_LLIM,              VAR_ULIM,                    ITM_INTEGRAL                  };

TO_QSPI const int16_t menu_BITS[]        = { ITM_LOGICALAND,                ITM_LOGICALOR,              ITM_LOGICALXOR,           ITM_LOGICALNOT,        ITM_MASKL,                   ITM_MASKR,
                                             ITM_LOGICALNAND,               ITM_LOGICALNOR,             ITM_LOGICALXNOR,          ITM_MIRROR,            ITM_NULL,                    ITM_NULL,
                                             ITM_SB,                        ITM_BS,                     ITM_NUMB,                 ITM_FB,                ITM_BC,                      ITM_CB,
                                             ITM_A,                         ITM_B,                      ITM_C,                    ITM_D,                 ITM_E,                       ITM_F,
                                             ITM_SL,                        ITM_RL,                     ITM_RLC,                  ITM_RRC,               ITM_RR,                      ITM_SR,
                                             ITM_LJ,                        ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_RJ,                      ITM_ASR,
                                             ITM_1COMPL,                    ITM_2COMPL,                 ITM_UNSIGN,               ITM_SIGNMT,            ITM_NULL,                    ITM_WSIZE                     };

TO_QSPI const int16_t menu_CLK[]         = { ITM_DATE,                      ITM_toDATE,                 ITM_DATEto,               ITM_WDAY,              ITM_TIME,                    ITM_XtoDATE,
                                             ITM_JtoD,                      ITM_DtoJ,                   ITM_NULL,                 ITM_DAY,               ITM_MONTH,                   ITM_YEAR,
                                             ITM_NULL,                      ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL,
#if DMCP_BUILD
                                             ITM_SETTIM,                    ITM_TDISP,                  ITM_SETDAT,               ITM_DMY,               ITM_YMD,                     ITM_MDY,
#else // !DMCP_BUILD
                                             ITM_NULL,                      ITM_TDISP,                  ITM_NULL,                 ITM_DMY,               ITM_YMD,                     ITM_MDY,
#endif // DMCP_BUILD
                                             ITM_NULL,                      ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_JUL_GREG                  };

TO_QSPI const int16_t menu_CLR[]         = { ITM_CLSIGMA,                   ITM_CLP,                    ITM_CF,                   ITM_CLMENU,            ITM_CLCVAR,                  ITM_CLX,
                                             ITM_CLREGS,                    ITM_CLPALL,                 ITM_CLFALL,               ITM_NULL,              ITM_CLLCD,                   ITM_CLSTK,
                                             ITM_CLALL,                     ITM_CLBKUP,                 ITM_NULL,                 ITM_DELITM,            ITM_NULL,                    ITM_RESET                     };

/*      Menu name                           <----------------------------------------------------------------------------- 6 functions ---------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 f shifted functions ------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 g shifted functions ------------------------------------------------------------------------->  */
TO_QSPI const int16_t menu_CPX[]         = { ITM_DOT_PROD,                  ITM_CROSS_PROD,             ITM_UNITV,                ITM_RE,                ITM_CONJ,                    ITM_REexIM,
                                             ITM_CXtoRE,                    ITM_REtoCX,                 ITM_NULL,                 ITM_IM,                ITM_NULL,                    ITM_NULL                      };

TO_QSPI const int16_t menu_DISP[]        = { ITM_FIX,                       ITM_SCI,                    ITM_ENG,                  ITM_ALL,               ITM_ROUNDI,                  ITM_ROUND,
                                             ITM_SDL,                       ITM_SDR,                    ITM_NULL,                 ITM_NULL,              ITM_RDP,                     ITM_RSD,
                                             ITM_NULL,                      ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL,
                                             ITM_SETCHN,                    ITM_SETEUR,                 ITM_SETIND,               ITM_SETJPN,            ITM_SETUK,                   ITM_SETUSA,
                                             ITM_GAP,                       ITM_HIDE,                   ITM_GETHIDE,              ITM_RANGE,             ITM_GETRANGE,                ITM_DSTACK                    };

TO_QSPI const int16_t menu_EQN[]         = { ITM_EQ_NEW,                    ITM_EQ_EDI,                 -MNU_2NDDERIV,            -MNU_1STDERIV,         -MNU_Sf,                     -MNU_Solver,
                                             ITM_EQ_DEL,                    ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL                      };

TO_QSPI const int16_t menu_EXP[]         = { ITM_CUBE,                      ITM_YX,                     ITM_LOGXY,                ITM_LOG2,              ITM_2X,                      ITM_SQUARE,
                                             ITM_CUBEROOT,                  ITM_XTHROOT,                ITM_NULL,                 ITM_LN1X,              ITM_EX1,                     ITM_SQRT1PX2,
                                             ITM_sinh,                      ITM_arsinh,                 ITM_cosh,                 ITM_arcosh,            ITM_tanh,                    ITM_artanh                    };

TO_QSPI const int16_t menu_TRI[]         = { ITM_sin,                       ITM_arcsin,                 ITM_cos,                  ITM_arccos,            ITM_tan,                     ITM_arctan,
                                             ITM_sinc,                      ITM_sincpi,                 ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_atan2,
                                             ITM_sinh,                      ITM_arsinh,                 ITM_cosh,                 ITM_arcosh,            ITM_tanh,                    ITM_artanh                    };

TO_QSPI const int16_t menu_FIN[]         = { ITM_PC,                        ITM_PCMRR,                  ITM_PCT,                  ITM_PCSIGMA,           ITM_PCPMG,                   -MNU_TVM                      };

/*      Menu name                           <----------------------------------------------------------------------------- 6 functions ---------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 f shifted functions ------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 g shifted functions ------------------------------------------------------------------------->  */
TO_QSPI const int16_t menu_TVM[]         = { VAR_NPER,                      VAR_IPonA,                  VAR_PERonA,               VAR_PV,                VAR_PMT,                     VAR_FV,
                                             ITM_RCL_NPER,                  ITM_RCL_IPonA,              ITM_RCL_PERonA,           ITM_RCL_PV,            ITM_RCL_PMT,                 ITM_RCL_FV,
                                             ITM_BEGINP,                    ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_ENDP                      };

TO_QSPI const int16_t menu_FLAGS[]       = { ITM_SF,                        ITM_FS,                     ITM_FF,                   ITM_STATUS,            ITM_FC,                      ITM_CF,
                                             ITM_FSS,                       ITM_FSC,                    ITM_FSF,                  ITM_FCF,               ITM_FCS,                     ITM_FCC,
                                             ITM_NULL,                      ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_CLFALL                    };

TO_QSPI const int16_t menu_INFO[]        = { ITM_SSIZE,                     ITM_MEM,                    ITM_RMQ,                  ITM_ISM,               ITM_WSIZEQ,                  ITM_KTYP,
#if DMCP_BUILD
                                             ITM_LocRQ,                     ITM_DISK,                   ITM_ULP,                  ITM_NEIGHB,            ITM_SDIGS,                   ITM_BATT,
#else // !DMCP_BUILD
                                             ITM_LocRQ,                     ITM_NULL,                   ITM_ULP,                  ITM_NEIGHB,            ITM_SDIGS,                   ITM_BATT,
#endif // DMCP_BUILD
                                             ITM_WHO,                       ITM_VERS,                   ITM_M_DIMQ,               ITM_PMINFINITY,        ITM_ALPHAPOS,                ITM_ALPHALENG,
                                             ITM_GETRANGE,                  ITM_GETHIDE,                ITM_GET_JUL_GREG,         ITM_VOLQ,              ITM_NULL,                    ITM_BESTFQ                    };

TO_QSPI const int16_t menu_INTS[]        = { ITM_A,                         ITM_B,                      ITM_C,                    ITM_D,                 ITM_E,                       ITM_F,
                                             ITM_IDIV,                      ITM_RMD,                    ITM_MOD,                  ITM_XMOD,              ITM_FLOOR,                   ITM_LCM,
                                             ITM_DBLDIV,                    ITM_DBLR,                   ITM_DBLMULT,              ITM_PMOD,              ITM_CEIL,                    ITM_GCD,
                                             ITM_1COMPL,                    ITM_2COMPL,                 ITM_UNSIGN,               ITM_SIGNMT,            ITM_NULL,                    ITM_WSIZE,
                                             ITM_NULL,                      ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL,
                                             ITM_NULL,                      ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL,
                                             ITM_toBIN,                     ITM_toOCT,                  ITM_NULL,                 ITM_NULL,              ITM_toDEC,                   ITM_toHEX                     };

TO_QSPI const int16_t menu_LOOP[]        = { ITM_DSE,                       ITM_DSZ,                    ITM_DSL,                  ITM_ISE,               ITM_ISZ,                     ITM_ISG,
                                             ITM_DEC,                       ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_INC                       };

/*      Menu name                           <----------------------------------------------------------------------------- 6 functions ---------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 f shifted functions ------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 g shifted functions ------------------------------------------------------------------------->  */
TO_QSPI const int16_t menu_MATX[]        = { ITM_M_NEW,                     ITM_M_INV,                  ITM_M_DET,                ITM_M_TRANSP,          ITM_SIM_EQ,                  ITM_M_EDI,
                                             ITM_DOT_PROD,                  ITM_CROSS_PROD,             ITM_UNITV,                ITM_M_DIM,             ITM_INDEX,                   ITM_M_EDIN,
                                             ITM_ENORM,                     ITM_VANGLE,                 ITM_STOEL,                ITM_RCLEL,             ITM_M_PUT,                   ITM_M_GET,
                                             ITM_IPLUS,                     ITM_IMINUS,                 ITM_STOIJ,                ITM_RCLIJ,             ITM_JMINUS,                  ITM_JPLUS,
                                             ITM_RNORM,                     ITM_RSUM,                   ITM_M_LU,                 ITM_M_DIMQ,            ITM_NULL,                    ITM_M_RR,
                                             ITM_EIGVAL,                    ITM_NULL,                   ITM_M_QR,                 ITM_NULL,              ITM_NULL,                    ITM_EIGVEC                    };

TO_QSPI const int16_t menu_M_SIM_Q[]     = { VAR_MATA,                      VAR_MATB,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_MATX                      }; // Should VAR_MATA and VAR_MATB be reclaced by ITM_MATA (to be created) and ITM_MATB (to be created) here?

TO_QSPI const int16_t menu_M_EDIT[]      = { ITM_LEFT_ARROW,                ITM_UP_ARROW,               ITM_M_OLD,                ITM_M_GOTO,            ITM_DOWN_ARROW,              ITM_RIGHT_ARROW,
                                             ITM_M_INSR,                    ITM_NULL,                   ITM_M_DELR,               ITM_NULL,              ITM_M_WRAP,                  ITM_M_GROW                    };

TO_QSPI const int16_t menu_MODE[]        = { ITM_SF,                        ITM_DEG,                    ITM_RAD,                  ITM_MIL,               ITM_MULPI,                   ITM_CF,
                                             ITM_NULL,                      ITM_GRAD,                   ITM_RM,                   ITM_NULL,              ITM_SETSIG,                  ITM_DENMAX,
#if DMCP_BUILD
                                             ITM_SYSTEM,                    ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL,
#endif // DMCP_BUILD
                                                                                                                                                                                                            };

TO_QSPI const int16_t menu_PARTS[]       = { ITM_IP,                        ITM_FP,                     ITM_MANT,                 ITM_EXPT,              ITM_SIGN,                    ITM_DECOMP,
                                             ITM_NULL,                      ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_RE,                      ITM_IM                        };

/*      Menu name                           <----------------------------------------------------------------------------- 6 functions ---------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 f shifted functions ------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 g shifted functions ------------------------------------------------------------------------->  */
TO_QSPI const int16_t menu_PROB[]        = { -MNU_NORML,                    -MNU_T,                     ITM_COMB,                 ITM_PERM,              -MNU_F,                      -MNU_CHI2,
                                             -MNU_LGNRM,                    -MNU_CAUCH,                 ITM_NULL,                 -MNU_EXPON,            -MNU_LOGIS,                  -MNU_WEIBL,
                                             ITM_NULL,                      -MNU_NBIN,                  -MNU_GEOM,                -MNU_HYPER,            -MNU_BINOM,                  -MNU_POISS,
                                             ITM_RAN,                       ITM_SEED,                   ITM_RANI,                 ITM_NULL,              ITM_LNGAMMA,                 ITM_GAMMAX                    };

TO_QSPI const int16_t menu_t[]           = { ITM_TPX,                       ITM_NULL,                   ITM_TX,                   ITM_TUX,               ITM_NULL,                    ITM_TM1P                      };
TO_QSPI const int16_t menu_F[]           = { ITM_FPX,                       ITM_NULL,                   ITM_FX,                   ITM_FUX,               ITM_NULL,                    ITM_FM1P                      };
TO_QSPI const int16_t menu_chi2[]        = { ITM_chi2Px,                    ITM_NULL,                   ITM_chi2x,                ITM_chi2ux,            ITM_NULL,                    ITM_chi2M1                    };
TO_QSPI const int16_t menu_Norml[]       = { ITM_NORMLP,                    ITM_NULL,                   ITM_NORML,                ITM_NORMLU,            ITM_NULL,                    ITM_NORMLM1                   };
TO_QSPI const int16_t menu_LgNrm[]       = { ITM_LGNRMP,                    ITM_NULL,                   ITM_LGNRM,                ITM_LGNRMU,            ITM_NULL,                    ITM_LGNRMM1                   };
TO_QSPI const int16_t menu_Cauch[]       = { ITM_CAUCHP,                    ITM_NULL,                   ITM_CAUCH,                ITM_CAUCHU,            ITM_NULL,                    ITM_CAUCHM1                   };
TO_QSPI const int16_t menu_Expon[]       = { ITM_EXPONP,                    ITM_NULL,                   ITM_EXPON,                ITM_EXPONU,            ITM_NULL,                    ITM_EXPONM1                   };
TO_QSPI const int16_t menu_Logis[]       = { ITM_LOGISP,                    ITM_NULL,                   ITM_LOGIS,                ITM_LOGISU,            ITM_NULL,                    ITM_LOGISM1                   };
TO_QSPI const int16_t menu_Weibl[]       = { ITM_WEIBLP,                    ITM_NULL,                   ITM_WEIBL,                ITM_WEIBLU,            ITM_NULL,                    ITM_WEIBLM1                   };
TO_QSPI const int16_t menu_Binom[]       = { ITM_BINOMP,                    ITM_NULL,                   ITM_BINOM,                ITM_BINOMU,            ITM_NULL,                    ITM_BINOMM1                   };
TO_QSPI const int16_t menu_Geom[]        = { ITM_GEOMP,                     ITM_NULL,                   ITM_GEOM,                 ITM_GEOMU,             ITM_NULL,                    ITM_GEOMM1                    };
TO_QSPI const int16_t menu_Hyper[]       = { ITM_HYPERP,                    ITM_NULL,                   ITM_HYPER,                ITM_HYPERU,            ITM_NULL,                    ITM_HYPERM1                   };
TO_QSPI const int16_t menu_Nbin[]        = { ITM_NBINP,                     ITM_NULL,                   ITM_NBIN,                 ITM_NBINU,             ITM_NULL,                    ITM_NBINM1                    };
TO_QSPI const int16_t menu_Poiss[]       = { ITM_POISSP,                    ITM_NULL,                   ITM_POISS,                ITM_POISSU,            ITM_NULL,                    ITM_POISSM1                   };

/*      Menu name                           <----------------------------------------------------------------------------- 6 functions ---------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 f shifted functions ------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 g shifted functions ------------------------------------------------------------------------->  */
TO_QSPI const int16_t menu_PFN[]         = { ITM_INPUT,                     ITM_MVAR,                   ITM_VARMNU,               ITM_END,               ITM_PAUSE,                  -MNU_PFN2,
                                             ITM_R_CLR,                     ITM_R_COPY,                 ITM_R_SORT,               ITM_R_SWAP,            ITM_LocR,                    ITM_POPLR,
                                             ITM_PUTK,                      ITM_ERR,                    ITM_MSG,                  ITM_CNST,              ITM_TICKS,                   ITM_NULL                      };

TO_QSPI const int16_t menu_PFN2[]        = { ITM_MENU,                      ITM_KEYG,                   ITM_KEYX,                 ITM_CLMENU,            ITM_EXITALL,                 ITM_RTNP1,
                                             ITM_BACK,                      ITM_CASE,                   ITM_SKIP,                 ITM_NOP,               ITM_SDL,                     ITM_SDR,
                                             ITM_AGRAPH,                    ITM_PIXEL,                  ITM_POINT,                ITM_NULL,              ITM_NULL,                    ITM_NULL                      };

TO_QSPI const int16_t menu_STAT[]        = { ITM_SIGMAPLUS,                 ITM_XBAR,                   ITM_STDDEVWEIGHTED,       ITM_STDDEV,            ITM_SM,                      ITM_SUM,
                                             ITM_SIGMAMINUS,                ITM_XW,                     ITM_SW,                   ITM_STDDEVPOP,         ITM_SMW,                     ITM_NULL,
                                             ITM_CLSIGMA,                   ITM_XG,                     ITM_SCATTFACT,            ITM_SCATTFACTp,        ITM_SCATTFACTm,              ITM_NULL,
                                             ITM_LR,                        ITM_CORR,                   ITM_SXY,                  ITM_COV,               ITM_XCIRC,                   ITM_YCIRC,
                                             ITM_SA,                        ITM_XH,                     ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL,
                                             ITM_PLOT_LR,                   ITM_XRMS,                   ITM_XMAX,                 ITM_XMIN,             -MNU_HIST,                    ITM_PLOT,

                                             ITM_LINF,                      ITM_EXPF,                   ITM_LOGF,                 ITM_POWERF,            ITM_NULL,                    ITM_ORTHOF,
                                             ITM_PARABF,                    ITM_HYPF,                   ITM_ROOTF,                ITM_NULL,              ITM_NULL,                    ITM_NULL,
                                             ITM_GAUSSF,                    ITM_CAUCHF,                 ITM_NULL,                 ITM_BESTF,             ITM_NULL,                    ITM_NULL                      };

TO_QSPI const int16_t menu_SUMS[]        = { ITM_NSIGMA,                    ITM_SIGMAx,                 ITM_SIGMAx2,              ITM_SIGMAxy,           ITM_SIGMAy2,                 ITM_SIGMAy,
                                             ITM_NULL,                      ITM_SIGMAlnx,               ITM_SIGMAln2x,            ITM_SIGMAlnxy,         ITM_SIGMAln2y,               ITM_SIGMAlny,
                                             ITM_SIGMAx2y,                  ITM_SIGMAxlny,              ITM_SIGMAx2lny,           ITM_SIGMAlnyonx,       ITM_NULL,                    ITM_SIGMAylnx,
                                             ITM_SIGMAx2ony,                ITM_SIGMA1onx,              ITM_SIGMA1onx2,           ITM_SIGMAxony,         ITM_SIGMA1ony2,              ITM_SIGMA1ony,
                                             ITM_SIGMAx3,                   ITM_SIGMAx4,                ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL                      };

TO_QSPI const int16_t menu_GRAPH[]   = {
                                             ITM_REPLT,                     ITM_PLOTZOOM,               ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL                      };

TO_QSPI const int16_t menu_PLOT_STAT[]   = {
                                             ITM_PLOT_CENTRL,               ITM_SMI,                    ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL,
                                             ITM_NULL,                      ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL,
                                             ITM_NULL,                      ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL                      };

TO_QSPI const int16_t menu_PLOT_LR[]   = {
                                             ITM_PLOT_NXT,                  ITM_PLOTZOOM,               ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL,
                                             ITM_NULL,                      ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL,
                                             ITM_NULL,                      ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL                      };

TO_QSPI const int16_t menu_HIST[]   = {
                                             ITM_HISTOX,                    ITM_HISTOY,                 ITM_LOBIN,                ITM_nBINS,             ITM_HIBIN,                   ITM_HPLOT,
                                             ITM_NULL,                      ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL,
                                             ITM_NULL,                      ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL                      };

TO_QSPI const int16_t menu_HPLOT[]   = {
                                             ITM_HNORM,                     ITM_PLOTZOOM,               ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL,
                                             ITM_NULL,                      ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL,
                                             ITM_NULL,                      ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL                      };

TO_QSPI const int16_t menu_STK[]         = { ITM_Xex,                       ITM_Yex,                    ITM_Zex,                  ITM_Tex,               ITM_SHUFFLE,                 ITM_DROPY                     };

/*      Menu name                           <----------------------------------------------------------------------------- 6 functions ---------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 f shifted functions ------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 g shifted functions ------------------------------------------------------------------------->  */
TO_QSPI const int16_t menu_TEST[]        = { ITM_XLT,                       ITM_XLE,                    ITM_XEQU,                 ITM_XNE,               ITM_XGE,                     ITM_XGT,
                                             ITM_INT,                       ITM_EVEN,                   ITM_ODD,                  ITM_PRIME,             ITM_LEAP,                    ITM_FPQ,
                                             ITM_ENTRY,                     ITM_KEYQ,                   ITM_LBLQ,                 ITM_STRI,              ITM_CONVG,                   ITM_TOP,
                                             ITM_XEQUP0,                    ITM_XEQUM0,                 ITM_XAEQU,                ITM_MATR,              ITM_CPX,                     ITM_REAL,
                                             ITM_SPEC,                      ITM_NAN,                    ITM_NULL,                 ITM_M_SQR,             ITM_NULL,                    ITM_NULL                      };

TO_QSPI const int16_t menu_XFN[]         = { ITM_AGM,                       ITM_BN,                     ITM_BNS,                  ITM_ERF,              -MNU_ELLIPT,                 -MNU_ORTHOG,
                                             ITM_ERFC,                      ITM_FIB,                    ITM_GD,                   ITM_GDM1,              ITM_IXYZ,                    ITM_IGAMMAP,
                                             ITM_IGAMMAQ,                   ITM_JYX,                    ITM_LNBETA,               ITM_LNGAMMA,           ITM_MAX,                     ITM_MIN,
                                             ITM_NEXTP,                     ITM_WM,                     ITM_WP,                   ITM_WM1,               ITM_YYX,                     ITM_BETAXY,
                                             ITM_gammaXY,                   ITM_GAMMAXY,                ITM_zetaX,                ITM_M1X,               ITM_NULL,                    ITM_PARALLEL                  };

TO_QSPI const int16_t menu_Orthog[]      = { ITM_HN,                        ITM_Lm,                     ITM_LmALPHA,              ITM_Pn,                ITM_Tn,                      ITM_Un,
                                             ITM_HNP,                       ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL                      };

TO_QSPI const int16_t menu_Ellipt[]      = { ITM_sn,                        ITM_cn,                     ITM_dn,                   ITM_Kk,                ITM_Ek,                      ITM_PInk,
                                             ITM_am,                        ITM_NULL,                   ITM_NULL,                 ITM_Fphik,             ITM_Ephik,                   ITM_ZETAphik                  };

/*      Menu name                           <----------------------------------------------------------------------------- 6 functions ---------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 f shifted functions ------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 g shifted functions ------------------------------------------------------------------------->  */
TO_QSPI const int16_t menu_CATALOG[]     = { -MNU_FCNS,                     ITM_NULL,                  -MNU_CHARS,               -MNU_PROGS,            -MNU_VARS,                   -MNU_MENUS                    };

TO_QSPI const int16_t menu_CHARS[]       = { -MNU_ALPHAINTL,               -MNU_ALPHA_OMEGA,            ITM_NULL,                -MNU_ALPHAMATH,        -MNU_MyAlpha,                -MNU_ALPHADOT                 };

TO_QSPI const int16_t menu_VARS[]        = { -MNU_LINTS,                   -MNU_SINTS,                 -MNU_REALS,               -MNU_CPXS,             -MNU_ALLVAR,                 -MNU_MATRS,
                                             -MNU_DATES,                   -MNU_TIMES,                 -MNU_ANGLES,              -MNU_CONFIG,            ITM_NULL,                   -MNU_STRINGS                  };

TO_QSPI const int16_t menu_DELITM[]      = {  ITM_NULL,                     ITM_NULL,                   ITM_NULL,                -MNU_PROGS,            -MNU_VARS,                   -MNU_MENUS                    };

TO_QSPI const int16_t menu_CAT_AIM[]     = {  ITM_NULL,                     ITM_NULL,                  -MNU_CHARS,                ITM_NULL,              ITM_NULL,                   -MNU_MENUS_AIM                };

TO_QSPI const int16_t menu_MENUS_AIM[]   = { -MNU_ALPHAINTL,               -MNU_ALPHAMATH,             -MNU_ALPHADOT,            -MNU_ALPHA_OMEGA,       ITM_NULL,                    ITM_NULL                     };

/*      Menu name                           <----------------------------------------------------------------------------- 6 functions ---------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 f shifted functions ------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 g shifted functions ------------------------------------------------------------------------->  */
// Following menu is UPPER CASE for lower case: +36
TO_QSPI const int16_t menu_ALPHA_OMEGA[] = { ITM_ALPHA,                     ITM_BETA,                   ITM_GAMMA,                ITM_DELTA,             ITM_EPSILON,                 ITM_ZETA,
                                             ITM_ETA,                       ITM_THETA,                  ITM_IOTA,                 ITM_KAPPA,             ITM_LAMBDA,                  ITM_MU,
                                             ITM_NU,                        ITM_XI,                     ITM_OMICRON,              ITM_PI,                ITM_RHO,                     ITM_SIGMA,
                                             ITM_NULL,                      ITM_TAU,                    ITM_UPSILON,              ITM_PHI,               ITM_CHI,                     ITM_PSI,
                                             ITM_OMEGA,                     ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL,
                                             ITM_IOTA_DIALYTIKA,            ITM_NULL,                   ITM_NULL,                 ITM_UPSILON_DIALYTIKA, ITM_NULL,                    ITM_NULL                      };

TO_QSPI const int16_t menu_alpha_omega[] = { ITM_alpha,                     ITM_beta,                   ITM_gamma,                ITM_delta,             ITM_epsilon,                 ITM_zeta,
                                             ITM_eta,                       ITM_theta,                  ITM_iota,                 ITM_kappa,             ITM_lambda,                  ITM_mu,
                                             ITM_nu,                        ITM_xi,                     ITM_omicron,              ITM_pi,                ITM_rho,                     ITM_sigma,
                                             ITM_sigma_end,                 ITM_tau,                    ITM_upsilon,              ITM_phi,               ITM_chi,                     ITM_psi,
                                             ITM_omega,                     ITM_alpha_TONOS,            ITM_epsilon_TONOS,        ITM_eta_TONOS,         ITM_iotaTON,                 ITM_iota_DIALYTIKA_TONOS,
                                             ITM_iota_DIALYTIKA,            ITM_omicron_TONOS,          ITM_upsilon_TONOS,        ITM_upsilon_DIALYTIKA, ITM_upsilon_DIALYTIKA_TONOS, ITM_omega_TONOS               };

TO_QSPI const int16_t menu_AngleConv[]   = { ITM_toDEG,                     ITM_toRAD,                  ITM_toMULpi,              ITM_toMIL,             ITM_toGRAD,                  ITM_toDMS,
                                             ITM_DEGto,                     ITM_RADto,                  ITM_MULPIto,              ITM_MILto,             ITM_GRADto,                  ITM_DMSto,
                                             ITM_DtoR,                      ITM_RtoD,                   ITM_MPItoR,               ITM_RtoMPI,            ITM_DtoDMS,                  ITM_DMStoD,
                                             ITM_DtoMIL,                    ITM_MILtoD,                 ITM_MILtoR,               ITM_RtoMIL,            ITM_NULL,                    ITM_NULL                      };

/*      Menu name                           <----------------------------------------------------------------------------- 6 functions ---------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 f shifted functions ------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 g shifted functions ------------------------------------------------------------------------->  */
TO_QSPI const int16_t menu_UnitConv[]    = { -MNU_CONVE,                    -MNU_CONVP,                 ITM_YEARtoS,              -MNU_CONVFP,           -MNU_CONVM,                  -MNU_CONVX,
                                             ITM_CtoF,                      ITM_FtoC,                   ITM_StoYEAR,              ITM_NULL,              -MNU_CONVV,                  -MNU_CONVA,
                                             ITM_NULL,                      ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL,
                                             10000+ITM_PRtoDBc,             10000+ITM_DBtoPRc,          10000+ITM_NMtoLBFFTb,     10000+ITM_LBFFTtoNMb,  10000+ITM_FRtoDBc,           10000+ITM_DBtoFRc,
                                             30000+ITM_PRtoDBb,             30000+ITM_DBtoPRb,          20000+ITM_NMtoLBFFT,      20000+ITM_LBFFTtoNM,   30000+ITM_FRtoDBb,           30000+ITM_DBtoFRb,
                                             20000+ITM_PRtoDB,              20000+ITM_DBtoPR,           ITM_NULL,                 ITM_NULL,              20000+ITM_FRtoDB,            20000+ITM_DBtoFR              };

TO_QSPI const int16_t menu_ConvE[]       = { ITM_CALtoJ,                    ITM_JtoCAL,                 ITM_BTUtoJ,               ITM_JtoBTU,            ITM_WHtoJ,                   ITM_JtoWH                     };

TO_QSPI const int16_t menu_ConvP[]       = { ITM_HPEtoW,                    ITM_WtoHPE,                 ITM_HPUKtoW,              ITM_WtoHPUK,           ITM_HPMtoW,                  ITM_WtoHPM                    };

TO_QSPI const int16_t menu_ConvFP[]      = { ITM_LBFtoN,                    ITM_NtoLBF,                 ITM_PAtoBAR,              ITM_BARtoPA,           ITM_PSItoPA,                 ITM_PAtoPSI,
                                             10000+ITM_INCHHGtoPAb,         10000+ITM_PAtoINCHHGb,      10000+ITM_TORtoPAb,       10000+ITM_PAtoTORb,    10000+ITM_ATMtoPA,           10000+ITM_PAtoATM,
                                             20000+ITM_INCHHGtoPA,          20000+ITM_PAtoINCHHG,       20000+ITM_TORtoPA,        20000+ITM_PAtoTOR,     20000+ITM_ATMtoPAb,          20000+ITM_PAtoATMb,
                                             ITM_NULL,                      ITM_NULL,                   10000+ITM_MMHGtoPAb,      10000+ITM_PAtoMMHGb,   ITM_NULL,                    ITM_NULL,
                                             ITM_NULL,                      ITM_NULL,                   20000+ITM_MMHGtoPA,       20000+ITM_PAtoMMHG,    ITM_NULL,                    ITM_NULL                      };

TO_QSPI const int16_t menu_ConvM[]       = { ITM_LBStoKG,                   ITM_KGtoLBS,                ITM_CWTtoKG,              ITM_KGtoCWT,           ITM_OZtoG,                   ITM_GtoOZ,
                                             10000+ITM_STOtoKGb,            10000+ITM_KGtoSTOb,         10000+ITM_SCWtoKGb,       10000+ITM_KGtoSCWb,    10000+ITM_TRZtoGb,           10000+ITM_GtoTRZb,
                                             20000+ITM_STOtoKG,             20000+ITM_KGtoSTO,          20000+ITM_SCWtoKG,        20000+ITM_KGtoSCW,     20000+ITM_TRZtoG,            20000+ITM_GtoTRZ,
                                             ITM_TONtoKG,                   ITM_KGtoTON,                10000+ITM_STtoKGc,        10000+ITM_KGtoSTc,     10000+ITM_CARATtoGb,         10000+ITM_GtoCARATb,
                                             10000+ITM_LIANGtoKGb,          10000+ITM_KGtoLIANGb,       30000+ITM_STtoKGb,        30000+ITM_KGtoSTb,     20000+ITM_CARATtoG,          20000+ITM_GtoCARAT,
                                             20000+ITM_LIANGtoKG,           20000+ITM_KGtoLIANG,        20000+ITM_STtoKG,         20000+ITM_KGtoST,      ITM_JINtoKG,                 ITM_KGtoJIN                   };

TO_QSPI const int16_t menu_ConvX[]       = { ITM_AUtoM,                     ITM_MtoAU,                  ITM_LYtoM,                ITM_MtoLY,             ITM_PCtoM,                   ITM_MtoPC,
                                             ITM_MILEtoM,                   ITM_MtoMILE,                ITM_NMItoM,               ITM_MtoNMI,            ITM_FTtoM,                   ITM_MtoFT,
                                             ITM_INCHtoMM,                  ITM_MMtoINCH,               ITM_NULL,                 ITM_NULL,              ITM_YDtoM,                   ITM_MtoYD,
                                             ITM_LItoM,                     ITM_MtoLI,                  ITM_YINtoM,               ITM_MtoYIN,            10000+ITM_ZHANGtoMb,         10000+ITM_MtoZHANGb,
                                             ITM_NULL,                      ITM_NULL,                   ITM_NULL,                 ITM_NULL,              20000+ITM_ZHANGtoM,          20000+ITM_MtoZHANG,
                                             ITM_CHItoM,                    ITM_MtoCHI,                 ITM_CUNtoM,               ITM_MtoCUN,            ITM_FENtoM,                  ITM_MtoFEN,
                                             10000+ITM_FATHOMtoMb,          10000+ITM_MtoFATHOMb,       10000+ITM_POINTtoMMb,     10000+ITM_MMtoPOINTb,  10000+ITM_FTUStoMc,          10000+ITM_MtoFTUSc,
                                             20000+ITM_FATHOMtoM,           20000+ITM_MtoFATHOM,        20000+ITM_POINTtoMM,      20000+ITM_MMtoPOINT,   30000+ITM_FTUStoMb,          30000+ITM_MtoFTUSb,
                                             ITM_MItoKM,                    ITM_KMtoMI,                 ITM_NMItoKM,              ITM_KMtoNMI,           20000+ITM_FTUStoM,           20000+ITM_MtoFTUS             };

TO_QSPI const int16_t menu_ConvA[]       = { 10000+ITM_ACtoHAb,             10000+ITM_HAtoACb,          ITM_HECTAREtoM2,          ITM_M2toHECTARE,       10000+ITM_ACUStoHAb,         10000+ITM_HAtoACUSb,
                                             20000+ITM_ACtoHA,              20000+ITM_HAtoAC,           ITM_NULL,                 ITM_NULL,              20000+ITM_ACUStoHA,          20000+ITM_HAtoACUS,
                                             ITM_MUtoM2,                    ITM_M2toMU,                 ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL                      };

/*      Menu name                           <----------------------------------------------------------------------------- 6 functions ---------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 f shifted functions ------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 g shifted functions ------------------------------------------------------------------------->  */
TO_QSPI const int16_t menu_ConvV[]       = { ITM_GLUKtoL,                   ITM_LtoGLUK,                ITM_QTtoL,                ITM_LtoQT,             ITM_GLUStoL,                 ITM_LtoGLUS,
                                             10000+ITM_FZUKtoMLb,           10000+ITM_MLtoFZUKb,        10000+ITM_BARRELtoM3b,    10000+ITM_M3toBARRELb, 10000+ITM_FZUStoMLb,         10000+ITM_MLtoFZUSb,
                                             20000+ITM_FZUKtoML,            20000+ITM_MLtoFZUK,         20000+ITM_BARRELtoM3,     20000+ITM_M3toBARREL,  20000+ITM_FZUStoML,          20000+ITM_MLtoFZUS            };

TO_QSPI const int16_t menu_alphaFN[]     = { ITM_XtoALPHA,                  ITM_ALPHARL,                ITM_ALPHARR,              ITM_ALPHASL,           ITM_ALPHASR,                 ITM_ALPHAtoX,
                                             ITM_NULL,                      ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_ALPHALENG,               ITM_ALPHAPOS,
                                             ITM_FBR,                       ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL                      };

/*      Menu name                           <----------------------------------------------------------------------------- 6 functions ---------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 f shifted functions ------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 g shifted functions ------------------------------------------------------------------------->  */
TO_QSPI const int16_t menu_alphaMATH[]   = { ITM_LESS_THAN,                 ITM_LESS_EQUAL,             ITM_EQUAL,                ITM_ALMOST_EQUAL,      ITM_GREATER_EQUAL,           ITM_GREATER_THAN,
                                             ITM_LEFT_CURLY_BRACKET,        ITM_LEFT_SQUARE_BRACKET,    ITM_LEFT_PARENTHESIS,     ITM_RIGHT_PARENTHESIS, ITM_RIGHT_SQUARE_BRACKET,    ITM_RIGHT_CURLY_BRACKET,
                                             ITM_PROD_SIGN,                 ITM_OBELUS,                 ITM_INTEGRAL_SIGN,        ITM_SUB_INFINITY,      ITM_SUP_INFINITY,            ITM_INFINITY,
                                             ITM_NOT,                       ITM_AND,                    ITM_OR,                   ITM_NOT_EQUAL,         ITM_PIPE,                    ITM_AMPERSAND,
                                             ITM_MEASURED_ANGLE,            ITM_RIGHT_ANGLE,            ITM_PERPENDICULAR,        ITM_CUBE_ROOT,         ITM_SQUARE_ROOT,             ITM_xTH_ROOT,
                                             ITM_x_BAR,                     ITM_y_BAR,                  ITM_x_CIRC,               ITM_y_CIRC,            ITM_x_UNDER_ROOT,            ITM_y_UNDER_ROOT,
                                             ITM_COLON_EQUALS,              ITM_ESTIMATES,              ITM_CORRESPONDS_TO,       ITM_SUB_E_OUTLINE,     ITM_COMPLEX_C,               ITM_REAL_R,
                                             ITM_SUN,                       ITM_SUB_SUN,                ITM_SUB_EARTH,            ITM_NULL,              ITM_NULL,                    ITM_NULL,
                                             ITM_PLUS_MINUS,                ITM_CIRCUMFLEX,             ITM_SUP_T,                ITM_SUP_MINUS_1,       ITM_PLANCK_2PI,              ITM_NULL                      };

/*      Menu name                           <----------------------------------------------------------------------------- 6 functions ---------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 f shifted functions ------------------------------------------------------------------------->  */
/*                                          <---------------------------------------------------------------------- 6 g shifted functions ------------------------------------------------------------------------->  */
TO_QSPI const int16_t menu_alphaDot[]    = { ITM_EXCLAMATION_MARK,          ITM_SEMICOLON,              ITM_COLON,                ITM_QUOTE,             ITM_DOUBLE_QUOTE,            ITM_CHECK_MARK,
                                             ITM_INVERTED_EXCLAMATION_MARK, ITM_INVERTED_QUESTION_MARK, ITM_SECTION,              ITM_EMPTY_SET,         ITM_TILDE,                   ITM_BACK_SLASH,
                                             ITM_DOLLAR,                    ITM_EURO,                   ITM_PERCENT,              ITM_AMPERSAND,         ITM_POUND,                   ITM_YEN,
                                             ITM_LEFT_ARROW,                ITM_UP_ARROW,               ITM_SERIAL_IO,            ITM_LEFT_RIGHT_ARROWS, ITM_DOWN_ARROW,              ITM_RIGHT_ARROW,
                                             ITM_LEFT_DOUBLE_ANGLE,         ITM_RIGHT_DOUBLE_ANGLE,     ITM_NULL,                 ITM_WATCH,             ITM_TIMER_SYMBOL,            ITM_ASTERISK,
                                             ITM_HOURGLASS,                 ITM_NEG_EXCLAMATION_MARK,   ITM_USER_MODE,            ITM_BATTERY,           ITM_AT,                      ITM_SUP_ASTERISK,
                                             ITM_DOUBLE_LOW_QUOTE,          ITM_RIGHT_DOUBLE_QUOTE,     ITM_ELLIPSIS,             ITM_UNDERSCORE,        ITM_BULLET,                  ITM_NULL                      };

TO_QSPI const int16_t menu_1stDeriv[]    = { ITM_NULL,                      ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL                      };
TO_QSPI const int16_t menu_2ndDeriv[]    = { ITM_NULL,                      ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL                      };
TO_QSPI const int16_t menu_Sf[]          = { ITM_NULL,                      ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL                      };
TO_QSPI const int16_t menu_Solver[]      = { ITM_NULL,                      ITM_NULL,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL                      };
TO_QSPI const int16_t menu_IO[]          = { ITM_LOAD,                      ITM_LOADP,                  ITM_LOADR,                ITM_DUMP,              ITM_SAVEST,                  ITM_LOADST,
                                             ITM_LOADSS,                    ITM_LOADV,                  ITM_LOADSIGMA,            ITM_NULL,              ITM_WRITEP,                  ITM_READP,
#if DMCP_BUILD
                                             ITM_BEEP,                      ITM_TONE,                   ITM_BUZZ,                 ITM_PLAY,              ITM_VOLMINUS,                ITM_VOLPLUS                   };
#else // !DMCP_BUILD
                                             ITM_BEEP,                      ITM_TONE,                   ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL                      };
#endif // DMCP_BUILD
TO_QSPI const int16_t menu_PRINT[]       = { ITM_PRINTERX,                  ITM_PRINTERR,               ITM_PRINTERSIGMA,         ITM_PRINTERADV,        ITM_PRINTERLCD,              ITM_PRINTERPROG,
                                             ITM_PRINTERSTK,                ITM_PRINTERREGS,            ITM_PRINTERUSER,          ITM_PRINTERTAB,        ITM_PRINTERHASH,             ITM_PRINTERCHAR,
                                             ITM_NULL,                      ITM_NULL,                   ITM_NULL,                 ITM_PRINTERWIDTH,      ITM_PRINTERDLAY,             ITM_PRINTERMODE               };
TO_QSPI const int16_t menu_Tam[]         = { ITM_INDIRECTION,               -MNU_VAR,                   ITM_REG_X,                ITM_REG_Y,             ITM_REG_Z,                   ITM_REG_T                     };
TO_QSPI const int16_t menu_TamCmp[]      = { ITM_INDIRECTION,               -MNU_VAR,                   ITM_REG_X,                ITM_REG_Y,             ITM_REG_Z,                   ITM_REG_T,
                                             ITM_0P,                        ITM_1P,                     ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL                      };
TO_QSPI const int16_t menu_TamFlag[]     = { ITM_INDIRECTION,               -MNU_SYSFL,                 ITM_REG_X,                ITM_REG_Y,             ITM_REG_Z,                   ITM_REG_T                     };
TO_QSPI const int16_t menu_TamStoRcl[]   = { ITM_INDIRECTION,               -MNU_VAR,                   ITM_REG_X,                ITM_REG_Y,             ITM_REG_Z,                   ITM_REG_T,
                                             ITM_Config,                    ITM_Stack,                  ITM_NULL,                 ITM_NULL,              ITM_Max,                     ITM_Min,
                                             ITM_dddEL,                     ITM_dddIJ,                  ITM_NULL,                 ITM_NULL,              ITM_NULL,                    ITM_NULL                      };
TO_QSPI const int16_t menu_TamShuffle[]  = { ITM_REG_X,                     ITM_REG_Y,                  ITM_REG_Z,                ITM_REG_T,             ITM_NULL,                    ITM_NULL,                     };
TO_QSPI const int16_t menu_TamLabel[]    = { ITM_INDIRECTION,               -MNU_PROG,                  ITM_REG_X,                ITM_REG_Y,             ITM_REG_Z,                   ITM_REG_T                     };

TO_QSPI const int16_t menu_Eim[]         = { ITM_EQ_LEFT,                   ITM_PAIR_OF_PARENTHESES,    ITM_CIRCUMFLEX,           ITM_COLON,             ITM_EQUAL,                   ITM_EQ_RIGHT                  };

TO_QSPI const int16_t menu_Timer[]       = { ITM_TIMER_ADD,                 ITM_NULL,                   ITM_NULL,                 ITM_TIMER_0_1S,        ITM_NULL,                    ITM_TIMER_RESET               };

TO_QSPI const int16_t baseMenu[]         = { smMyMenu, smMyAlpha, smMyPFN, smEqEdit };

#include "softmenuCatalogs.h"

TO_QSPI const softmenu_t softmenu[] = {
/*   0 */  {.menuItem = -MNU_MyMenu,      .numItems = 0,                                        .softkeyItem = NULL             }, // MyMenu must be the 1st
/*   1 */  {.menuItem = -MNU_MyAlpha,     .numItems = 0,                                        .softkeyItem = NULL             }, // MyAlpha must be the 2nd
/*   2 */  {.menuItem = -MNU_MyPFN,       .numItems = 0,                                        .softkeyItem = NULL             }, // MyPFN must be the 3rd
/*   3 */  {.menuItem = -MNU_PROGS,       .numItems = 0,                                        .softkeyItem = NULL             },
/*   4 */  {.menuItem = -MNU_VAR,         .numItems = 0,                                        .softkeyItem = NULL             }, // The 16 first menus are
/*   5 */  {.menuItem = -MNU_PROG,        .numItems = 0,                                        .softkeyItem = NULL             }, // variable softmenus and
/*   6 */  {.menuItem = -MNU_MATRS,       .numItems = 0,                                        .softkeyItem = NULL             }, // MUST be in the same
/*   7 */  {.menuItem = -MNU_STRINGS,     .numItems = 0,                                        .softkeyItem = NULL             }, // order as the
/*   8 */  {.menuItem = -MNU_DATES,       .numItems = 0,                                        .softkeyItem = NULL             }, // dynamicSoftmenu area.
/*   9 */  {.menuItem = -MNU_TIMES,       .numItems = 0,                                        .softkeyItem = NULL             }, //
/*  10 */  {.menuItem = -MNU_ANGLES,      .numItems = 0,                                        .softkeyItem = NULL             }, // If you add or remove one:
/*  11 */  {.menuItem = -MNU_SINTS,       .numItems = 0,                                        .softkeyItem = NULL             }, // don't forget to adjust
/*  12 */  {.menuItem = -MNU_LINTS,       .numItems = 0,                                        .softkeyItem = NULL             }, // NUMBER_OF_DYNAMIC_SOFTMENUS
/*  13 */  {.menuItem = -MNU_REALS,       .numItems = 0,                                        .softkeyItem = NULL             }, // in defines.h
/*  14 */  {.menuItem = -MNU_CPXS,        .numItems = 0,                                        .softkeyItem = NULL             },
/*  15 */  {.menuItem = -MNU_CONFIG,      .numItems = 0,                                        .softkeyItem = NULL             },
/*  16 */  {.menuItem = -MNU_ALLVAR,      .numItems = 0,                                        .softkeyItem = NULL             },
/*  17 */  {.menuItem = -MNU_MVAR,        .numItems = 0,                                        .softkeyItem = NULL             },
/*  18 */  {.menuItem = -MNU_MENUS,       .numItems = 0,                                        .softkeyItem = NULL             },
/*  19 */  {.menuItem = -MNU_DYNAMIC,     .numItems = 0,                                        .softkeyItem = NULL             },
/*  20 */  {.menuItem = -ITM_MENU,        .numItems = 0,                                        .softkeyItem = NULL             },
/*  21 */  {.menuItem = -MNU_TAMFLAG,     .numItems = sizeof(menu_TamFlag    )/sizeof(int16_t), .softkeyItem = menu_TamFlag     },
/*  22 */  {.menuItem = -MNU_SYSFL,       .numItems = sizeof(menu_SYSFL      )/sizeof(int16_t), .softkeyItem = menu_SYSFL       },
/*  23 */  {.menuItem = -MNU_ALPHAINTL,   .numItems = sizeof(menu_alpha_INTL )/sizeof(int16_t), .softkeyItem = menu_alpha_INTL  },
/*  24 */  {.menuItem = -MNU_ALPHAintl,   .numItems = sizeof(menu_alpha_intl )/sizeof(int16_t), .softkeyItem = menu_alpha_intl  },
/*  25 */  {.menuItem = -MNU_ADV,         .numItems = sizeof(menu_ADV        )/sizeof(int16_t), .softkeyItem = menu_ADV         },
/*  26 */  {.menuItem = -MNU_Sfdx,        .numItems = sizeof(menu_Sfdx       )/sizeof(int16_t), .softkeyItem = menu_Sfdx        },
/*  27 */  {.menuItem = -MNU_BITS,        .numItems = sizeof(menu_BITS       )/sizeof(int16_t), .softkeyItem = menu_BITS        },
/*  28 */  {.menuItem = -MNU_CLK,         .numItems = sizeof(menu_CLK        )/sizeof(int16_t), .softkeyItem = menu_CLK         },
/*  29 */  {.menuItem = -MNU_CLR,         .numItems = sizeof(menu_CLR        )/sizeof(int16_t), .softkeyItem = menu_CLR         },
/*  30 */  {.menuItem = -MNU_CPX,         .numItems = sizeof(menu_CPX        )/sizeof(int16_t), .softkeyItem = menu_CPX         },
/*  31 */  {.menuItem = -MNU_DISP,        .numItems = sizeof(menu_DISP       )/sizeof(int16_t), .softkeyItem = menu_DISP        },
/*  32 */  {.menuItem = -MNU_EQN,         .numItems = sizeof(menu_EQN        )/sizeof(int16_t), .softkeyItem = menu_EQN         },
/*  33 */  {.menuItem = -MNU_1STDERIV,    .numItems = sizeof(menu_1stDeriv   )/sizeof(int16_t), .softkeyItem = menu_1stDeriv    },
/*  34 */  {.menuItem = -MNU_2NDDERIV,    .numItems = sizeof(menu_2ndDeriv   )/sizeof(int16_t), .softkeyItem = menu_2ndDeriv    },
/*  35 */  {.menuItem = -MNU_Sf,          .numItems = sizeof(menu_Sf         )/sizeof(int16_t), .softkeyItem = menu_Sf          },
/*  36 */  {.menuItem = -MNU_Solver,      .numItems = sizeof(menu_Solver     )/sizeof(int16_t), .softkeyItem = menu_Solver      },
/*  37 */  {.menuItem = -MNU_EXP,         .numItems = sizeof(menu_EXP        )/sizeof(int16_t), .softkeyItem = menu_EXP         },
/*  38 */  {.menuItem = -MNU_TRI,         .numItems = sizeof(menu_TRI        )/sizeof(int16_t), .softkeyItem = menu_TRI         },
/*  39 */  {.menuItem = -MNU_FIN,         .numItems = sizeof(menu_FIN        )/sizeof(int16_t), .softkeyItem = menu_FIN         },
/*  40 */  {.menuItem = -MNU_TVM,         .numItems = sizeof(menu_TVM        )/sizeof(int16_t), .softkeyItem = menu_TVM         },
/*  41 */  {.menuItem = -MNU_FLAGS,       .numItems = sizeof(menu_FLAGS      )/sizeof(int16_t), .softkeyItem = menu_FLAGS       },
/*  42 */  {.menuItem = -MNU_INFO,        .numItems = sizeof(menu_INFO       )/sizeof(int16_t), .softkeyItem = menu_INFO        },
/*  43 */  {.menuItem = -MNU_INTS,        .numItems = sizeof(menu_INTS       )/sizeof(int16_t), .softkeyItem = menu_INTS        },
/*  44 */  {.menuItem = -MNU_LOOP,        .numItems = sizeof(menu_LOOP       )/sizeof(int16_t), .softkeyItem = menu_LOOP        },
/*  45 */  {.menuItem = -MNU_MATX,        .numItems = sizeof(menu_MATX       )/sizeof(int16_t), .softkeyItem = menu_MATX        },
/*  46 */  {.menuItem = -MNU_SIMQ,        .numItems = sizeof(menu_M_SIM_Q    )/sizeof(int16_t), .softkeyItem = menu_M_SIM_Q     },
/*  47 */  {.menuItem = -MNU_M_EDIT,      .numItems = sizeof(menu_M_EDIT     )/sizeof(int16_t), .softkeyItem = menu_M_EDIT      },
/*  48 */  {.menuItem = -MNU_MODE,        .numItems = sizeof(menu_MODE       )/sizeof(int16_t), .softkeyItem = menu_MODE        },
/*  49 */  {.menuItem = -MNU_PARTS,       .numItems = sizeof(menu_PARTS      )/sizeof(int16_t), .softkeyItem = menu_PARTS       },
/*  50 */  {.menuItem = -MNU_PROB,        .numItems = sizeof(menu_PROB       )/sizeof(int16_t), .softkeyItem = menu_PROB        },
/*  51 */  {.menuItem = -MNU_T,           .numItems = sizeof(menu_t          )/sizeof(int16_t), .softkeyItem = menu_t           },
/*  52 */  {.menuItem = -MNU_F,           .numItems = sizeof(menu_F          )/sizeof(int16_t), .softkeyItem = menu_F           },
/*  53 */  {.menuItem = -MNU_CHI2,        .numItems = sizeof(menu_chi2       )/sizeof(int16_t), .softkeyItem = menu_chi2        },
/*  54 */  {.menuItem = -MNU_NORML,       .numItems = sizeof(menu_Norml      )/sizeof(int16_t), .softkeyItem = menu_Norml       },
/*  55 */  {.menuItem = -MNU_LGNRM,       .numItems = sizeof(menu_LgNrm      )/sizeof(int16_t), .softkeyItem = menu_LgNrm       },
/*  56 */  {.menuItem = -MNU_CAUCH,       .numItems = sizeof(menu_Cauch      )/sizeof(int16_t), .softkeyItem = menu_Cauch       },
/*  57 */  {.menuItem = -MNU_EXPON,       .numItems = sizeof(menu_Expon      )/sizeof(int16_t), .softkeyItem = menu_Expon       },
/*  58 */  {.menuItem = -MNU_LOGIS,       .numItems = sizeof(menu_Logis      )/sizeof(int16_t), .softkeyItem = menu_Logis       },
/*  59 */  {.menuItem = -MNU_WEIBL,       .numItems = sizeof(menu_Weibl      )/sizeof(int16_t), .softkeyItem = menu_Weibl       },
/*  60 */  {.menuItem = -MNU_BINOM,       .numItems = sizeof(menu_Binom      )/sizeof(int16_t), .softkeyItem = menu_Binom       },
/*  61 */  {.menuItem = -MNU_GEOM,        .numItems = sizeof(menu_Geom       )/sizeof(int16_t), .softkeyItem = menu_Geom        },
/*  62 */  {.menuItem = -MNU_HYPER,       .numItems = sizeof(menu_Hyper      )/sizeof(int16_t), .softkeyItem = menu_Hyper       },
/*  63 */  {.menuItem = -MNU_NBIN,        .numItems = sizeof(menu_Nbin       )/sizeof(int16_t), .softkeyItem = menu_Nbin        },
/*  64 */  {.menuItem = -MNU_POISS,       .numItems = sizeof(menu_Poiss      )/sizeof(int16_t), .softkeyItem = menu_Poiss       },
/*  65 */  {.menuItem = -MNU_PFN,         .numItems = sizeof(menu_PFN        )/sizeof(int16_t), .softkeyItem = menu_PFN         },
/*  66 */  {.menuItem = -MNU_PFN2,        .numItems = sizeof(menu_PFN2       )/sizeof(int16_t), .softkeyItem = menu_PFN2        },
/*  67 */  {.menuItem = -MNU_STAT,        .numItems = sizeof(menu_STAT       )/sizeof(int16_t), .softkeyItem = menu_STAT        },
/*  68 */  {.menuItem = -MNU_SUMS,        .numItems = sizeof(menu_SUMS       )/sizeof(int16_t), .softkeyItem = menu_SUMS        },
/*  69 */  {.menuItem = -MNU_GRAPH,       .numItems = sizeof(menu_GRAPH      )/sizeof(int16_t), .softkeyItem = menu_GRAPH       },
/*  70 */  {.menuItem = -MNU_PLOT_STAT,   .numItems = sizeof(menu_PLOT_STAT  )/sizeof(int16_t), .softkeyItem = menu_PLOT_STAT   },
/*  71 */  {.menuItem = -MNU_PLOT_LR,     .numItems = sizeof(menu_PLOT_LR    )/sizeof(int16_t), .softkeyItem = menu_PLOT_LR     },
/*  72 */  {.menuItem = -MNU_HPLOT,       .numItems = sizeof(menu_HPLOT      )/sizeof(int16_t), .softkeyItem = menu_HPLOT       },
/*  73 */  {.menuItem = -MNU_HIST,        .numItems = sizeof(menu_HIST       )/sizeof(int16_t), .softkeyItem = menu_HIST        },
/*  74 */  {.menuItem = -MNU_STK,         .numItems = sizeof(menu_STK        )/sizeof(int16_t), .softkeyItem = menu_STK         },
/*  75 */  {.menuItem = -MNU_TEST,        .numItems = sizeof(menu_TEST       )/sizeof(int16_t), .softkeyItem = menu_TEST        },
/*  76 */  {.menuItem = -MNU_XFN,         .numItems = sizeof(menu_XFN        )/sizeof(int16_t), .softkeyItem = menu_XFN         },
/*  77 */  {.menuItem = -MNU_ORTHOG,      .numItems = sizeof(menu_Orthog     )/sizeof(int16_t), .softkeyItem = menu_Orthog      },
/*  78 */  {.menuItem = -MNU_ELLIPT,      .numItems = sizeof(menu_Ellipt     )/sizeof(int16_t), .softkeyItem = menu_Ellipt      },
/*  79 */  {.menuItem = -MNU_CATALOG,     .numItems = sizeof(menu_CATALOG    )/sizeof(int16_t), .softkeyItem = menu_CATALOG     },
/*  80 */  {.menuItem = -MNU_CHARS,       .numItems = sizeof(menu_CHARS      )/sizeof(int16_t), .softkeyItem = menu_CHARS       },
/*  81 */  {.menuItem = -MNU_VARS,        .numItems = sizeof(menu_VARS       )/sizeof(int16_t), .softkeyItem = menu_VARS        },
/*  82 */  {.menuItem = -MNU_ALPHA_OMEGA, .numItems = sizeof(menu_ALPHA_OMEGA)/sizeof(int16_t), .softkeyItem = menu_ALPHA_OMEGA },
/*  83 */  {.menuItem = -MNU_alpha_omega, .numItems = sizeof(menu_alpha_omega)/sizeof(int16_t), .softkeyItem = menu_alpha_omega },
/*  84 */  {.menuItem = -MNU_FCNS,        .numItems = sizeof(menu_FCNS       )/sizeof(int16_t), .softkeyItem = menu_FCNS        },
/*  85 */  {.menuItem = -MNU_ALPHAMATH,   .numItems = sizeof(menu_alphaMATH  )/sizeof(int16_t), .softkeyItem = menu_alphaMATH   },
/*  86 */  {.menuItem = -MNU_ALPHADOT,    .numItems = sizeof(menu_alphaDot   )/sizeof(int16_t), .softkeyItem = menu_alphaDot    },
/*  87 */  {.menuItem = -MNU_ALPHAFN,     .numItems = sizeof(menu_alphaFN    )/sizeof(int16_t), .softkeyItem = menu_alphaFN     },
/*  88 */  {.menuItem = -MNU_ANGLECONV,   .numItems = sizeof(menu_AngleConv  )/sizeof(int16_t), .softkeyItem = menu_AngleConv   },
/*  89 */  {.menuItem = -MNU_UNITCONV,    .numItems = sizeof(menu_UnitConv   )/sizeof(int16_t), .softkeyItem = menu_UnitConv    },
/*  90 */  {.menuItem = -MNU_CONVE,       .numItems = sizeof(menu_ConvE      )/sizeof(int16_t), .softkeyItem = menu_ConvE       },
/*  91 */  {.menuItem = -MNU_CONVP,       .numItems = sizeof(menu_ConvP      )/sizeof(int16_t), .softkeyItem = menu_ConvP       },
/*  92 */  {.menuItem = -MNU_CONVFP,      .numItems = sizeof(menu_ConvFP     )/sizeof(int16_t), .softkeyItem = menu_ConvFP      },
/*  93 */  {.menuItem = -MNU_CONVM,       .numItems = sizeof(menu_ConvM      )/sizeof(int16_t), .softkeyItem = menu_ConvM       },
/*  94 */  {.menuItem = -MNU_CONVX,       .numItems = sizeof(menu_ConvX      )/sizeof(int16_t), .softkeyItem = menu_ConvX       },
/*  95 */  {.menuItem = -MNU_CONVV,       .numItems = sizeof(menu_ConvV      )/sizeof(int16_t), .softkeyItem = menu_ConvV       },
/*  96 */  {.menuItem = -MNU_CONVA,       .numItems = sizeof(menu_ConvA      )/sizeof(int16_t), .softkeyItem = menu_ConvA       },
/*  97 */  {.menuItem = -MNU_CONST,       .numItems = sizeof(menu_CONST      )/sizeof(int16_t), .softkeyItem = menu_CONST       },
/*  98 */  {.menuItem = -MNU_IO,          .numItems = sizeof(menu_IO         )/sizeof(int16_t), .softkeyItem = menu_IO          },
/*  99 */  {.menuItem = -MNU_PRINT,       .numItems = sizeof(menu_PRINT      )/sizeof(int16_t), .softkeyItem = menu_PRINT       },
/* 100 */  {.menuItem = -MNU_TAM,         .numItems = sizeof(menu_Tam        )/sizeof(int16_t), .softkeyItem = menu_Tam         },
/* 101 */  {.menuItem = -MNU_TAMCMP,      .numItems = sizeof(menu_TamCmp     )/sizeof(int16_t), .softkeyItem = menu_TamCmp      },
/* 102 */  {.menuItem = -MNU_TAMSTORCL,   .numItems = sizeof(menu_TamStoRcl  )/sizeof(int16_t), .softkeyItem = menu_TamStoRcl   },
/* 103 */  {.menuItem = -MNU_TAMSHUFFLE,  .numItems = sizeof(menu_TamShuffle )/sizeof(int16_t), .softkeyItem = menu_TamShuffle  },
/* 104 */  {.menuItem = -MNU_TAMLABEL,    .numItems = sizeof(menu_TamLabel   )/sizeof(int16_t), .softkeyItem = menu_TamLabel    },
/* 105 */  {.menuItem = -MNU_EQ_EDIT,     .numItems = sizeof(menu_Eim        )/sizeof(int16_t), .softkeyItem = menu_Eim         },
/* 106 */  {.menuItem = -MNU_TIMERF,      .numItems = sizeof(menu_Timer      )/sizeof(int16_t), .softkeyItem = menu_Timer       },
/* 107 */  {.menuItem = -ITM_DELITM,      .numItems = sizeof(menu_DELITM     )/sizeof(int16_t), .softkeyItem = menu_DELITM      },
/* 108 */  {.menuItem = -MNU_CAT_AIM,     .numItems = sizeof(menu_CAT_AIM    )/sizeof(int16_t), .softkeyItem = menu_CAT_AIM     },
/* 109 */  {.menuItem = -MNU_MENUS_AIM,   .numItems = sizeof(menu_MENUS_AIM  )/sizeof(int16_t), .softkeyItem = menu_MENUS_AIM   },
/* 110 */  {.menuItem =  0,               .numItems = 0,                                        .softkeyItem = NULL             }
};

dynamicSoftmenu_t dynamicSoftmenu[NUMBER_OF_DYNAMIC_SOFTMENUS] = {
/*   0 */  {.menuItem = -MNU_MyMenu,  .numItems = 0, .menuContent = NULL},
/*   1 */  {.menuItem = -MNU_MyAlpha, .numItems = 0, .menuContent = NULL},
/*   2 */  {.menuItem = -MNU_MyPFN,   .numItems = 0, .menuContent = NULL},
/*   3 */  {.menuItem = -MNU_PROGS,   .numItems = 0, .menuContent = NULL},
/*   4 */  {.menuItem = -MNU_VAR,     .numItems = 0, .menuContent = NULL},
/*   5 */  {.menuItem = -MNU_PROG,    .numItems = 0, .menuContent = NULL},
/*   6 */  {.menuItem = -MNU_MATRS,   .numItems = 0, .menuContent = NULL},
/*   7 */  {.menuItem = -MNU_STRINGS, .numItems = 0, .menuContent = NULL},
/*   8 */  {.menuItem = -MNU_DATES,   .numItems = 0, .menuContent = NULL},
/*   9 */  {.menuItem = -MNU_TIMES,   .numItems = 0, .menuContent = NULL},
/*  10 */  {.menuItem = -MNU_ANGLES,  .numItems = 0, .menuContent = NULL},
/*  11 */  {.menuItem = -MNU_SINTS,   .numItems = 0, .menuContent = NULL},
/*  12 */  {.menuItem = -MNU_LINTS,   .numItems = 0, .menuContent = NULL},
/*  13 */  {.menuItem = -MNU_REALS,   .numItems = 0, .menuContent = NULL},
/*  14 */  {.menuItem = -MNU_CPXS,    .numItems = 0, .menuContent = NULL},
/*  15 */  {.menuItem = -MNU_CONFIG,  .numItems = 0, .menuContent = NULL},
/*  16 */  {.menuItem = -MNU_ALLVAR,  .numItems = 0, .menuContent = NULL},
/*  17 */  {.menuItem = -MNU_MVAR,    .numItems = 0, .menuContent = NULL},
/*  18 */  {.menuItem = -MNU_MENUS,   .numItems = 0, .menuContent = NULL},
/*  19 */  {.menuItem = -MNU_DYNAMIC, .numItems = 0, .menuContent = NULL},
/*  20 */  {.menuItem = -ITM_MENU   , .numItems = 0, .menuContent = NULL},
};

#if defined(PC_BUILD)
  TO_QSPI const char *softMenuNames[] = {
  /*   0 */    "MyMenu",
  /*   1 */    "MyAlpha",
  /*   2 */    "MyPFN",
  /*   3 */    "PROGS",
  /*   4 */    "VAR",
  /*   5 */    "PROG",
  /*   6 */    "MATRS",
  /*   7 */    "STRINGS",
  /*   8 */    "DATES",
  /*   9 */    "TIMES",
  /*  10 */    "ANGLES",
  /*  11 */    "SINTS",
  /*  12 */    "LINTS",
  /*  13 */    "REALS",
  /*  14 */    "CPXS",
  /*  15 */    "CONFIG",
  /*  16 */    "ALLVAR",
  /*  17 */    "MVAR",
  /*  18 */    "MENUS",
  /*  19 */    "DYNAMIC",
  /*  20 */    "ITM_MENU",
  /*  21 */    "TAMFLAG",
  /*  22 */    "SYSFL",
  /*  23 */    "ALPHAINTL",
  /*  24 */    "ALPHAintl",
  /*  25 */    "ADV",
  /*  26 */    "Sfdx",
  /*  27 */    "BITS",
  /*  28 */    "CLK",
  /*  29 */    "CLR",
  /*  30 */    "CPX",
  /*  31 */    "DISP",
  /*  32 */    "EQN",
  /*  33 */    "1STDERIV",
  /*  34 */    "2NDDERIV",
  /*  35 */    "Sf",
  /*  36 */    "Solver",
  /*  37 */    "EXP",
  /*  38 */    "TRI",
  /*  39 */    "FIN",
  /*  40 */    "TVM",
  /*  41 */    "FLAGS",
  /*  42 */    "INFO",
  /*  43 */    "INTS",
  /*  44 */    "LOOP",
  /*  45 */    "MATX",
  /*  46 */    "SIMQ",
  /*  47 */    "M_EDIT",
  /*  48 */    "MODE",
  /*  49 */    "PARTS",
  /*  50 */    "PROB",
  /*  51 */    "T",
  /*  52 */    "F",
  /*  53 */    "CHI2",
  /*  54 */    "NORML",
  /*  55 */    "LGNRM",
  /*  56 */    "CAUCH",
  /*  57 */    "EXPON",
  /*  58 */    "LOGIS",
  /*  59 */    "WEIBL",
  /*  60 */    "BINOM",
  /*  61 */    "GEOM",
  /*  62 */    "HYPER",
  /*  63 */    "NBIN",
  /*  64 */    "POISS",
  /*  65 */    "PFN",
  /*  66 */    "PFN2",
  /*  67 */    "STAT",
  /*  68 */    "SUMS",
  /*  69 */    "GRAPH",
  /*  70 */    "PLOT_STAT",
  /*  71 */    "PLOT_LR",
  /*  72 */    "HPLOT",
  /*  73 */    "HIST",
  /*  74 */    "STK",
  /*  75 */    "TEST",
  /*  76 */    "XFN",
  /*  77 */    "ORTHOG",
  /*  78 */    "ELLIPT",
  /*  79 */    "CATALOG",
  /*  80 */    "CHARS",
  /*  81 */    "VARS",
  /*  82 */    "ALPHA_OMEGA",
  /*  83 */    "alpha_omega",
  /*  84 */    "FCNS",
  /*  85 */    "ALPHAMATH",
  /*  86 */    "ALPHADOT",
  /*  87 */    "ALPHAFN",
  /*  88 */    "ANGLECONV",
  /*  89 */    "UNITCONV",
  /*  90 */    "CONVE",
  /*  91 */    "CONVP",
  /*  92 */    "CONVFP",
  /*  93 */    "CONVM",
  /*  94 */    "CONVX",
  /*  95 */    "CONVV",
  /*  96 */    "CONVA",
  /*  97 */    "CONST",
  /*  98 */    "IO",
  /*  99 */    "PRINT",
  /* 100 */    "TAM",
  /* 101 */    "TAMCMP",
  /* 102 */    "TAMSTORCL",
  /* 103 */    "TAMSHUFFLE",
  /* 104 */    "TAMLABEL",
  /* 105 */    "EQ_EDIT",
  /* 106 */    "TIMERF",
  /* 107 */    "ITM_DELITM",
  /* 108 */    "CAT_AIM",
  /* 109 */    "MENUS_AIM"
  };
#endif // PC_BUILD

uint8_t *getNthString(uint8_t *ptr, int16_t n) { // Starting with string 0 (the 1ast string is returned for n=0)
  while(n) {
    ptr += stringByteLength((char *)ptr) + 1;
    n--;
  }

  return ptr;
}

void initUserMenus() {
  // initialize MyMenu
  memset(userMenuItems,  0, sizeof(userMenuItem_t) * 18);
  
  // initialize MyAlpha
  memset(userAlphaItems, 0, sizeof(userMenuItem_t) * 18);
  itemToBeAssigned = -MNU_ALPHAMATH;
  assignToMyAlpha(0);
  itemToBeAssigned = -MNU_ALPHADOT;
  assignToMyAlpha(1);
  itemToBeAssigned = -MNU_ALPHA_OMEGA;
  assignToMyAlpha(4);
  itemToBeAssigned = -MNU_ALPHAINTL;
  assignToMyAlpha(5);
  
  // initialize MyPFN
  memset(userPfnItems,   0, sizeof(userMenuItem_t) * 18);
  itemToBeAssigned = -MNU_LOOP;
  assignToMyPFN(0);
  itemToBeAssigned = -MNU_TEST;
  assignToMyPFN(1);
  itemToBeAssigned = -MNU_PFN;
  assignToMyPFN(4);
  itemToBeAssigned = -MNU_PFN2;
  assignToMyPFN(5);
  
  userMenus = NULL;
  numberOfUserMenus = 0;
  currentUserMenu = 0;
}

void fnDynamicMenu(uint16_t unusedButMandatoryParameter) {
  smStackMode_t sm = smStackMode[0];
  printf("fnDynamicMenu:\n       softmenuId = %d\n  dynamicMenuItem = %d\n", softmenuStacks[sm].item[0].softmenuId, dynamicMenuItem);
}



#if !defined(TESTSUITE_BUILD)
  static int sortMenu(void const *a, void const *b) {
    return compareString(a, b, CMP_EXTENSIVE);
  }



  static bool _filterDataType(calcRegister_t regist, dataType_t typeFilter, bool isAngular) {
    dataType_t dt = getRegisterDataType(regist);
    if(dt != dtReal34 && dt == typeFilter) {
      return true;
    }
    if(typeFilter == dtReal34Matrix && dt == dtComplex34Matrix) {
      return true;
    }
    if(typeFilter == dtReal34 && dt == dtReal34) {
      if(isAngular) {
        return getRegisterAngularMode(regist) != amNone;
      }
      if(!isAngular) {
        return getRegisterAngularMode(regist) == amNone;
      }
    }
    return false;
  }



  static bool _registerIsWritable(calcRegister_t regist) {
    if(regist < FIRST_RESERVED_VARIABLE) {
      return true;
    }
    else if(regist <= LAST_RESERVED_VARIABLE) {
      return !(allReservedVariables[regist - FIRST_RESERVED_VARIABLE].header.readOnly);
    }
    else {
      return false;
    }
  }



  static bool _isWritingTam() {
    if(!tamIsActive()) {
      return false;
    }
    else {
      switch(tam.function) {
        case ITM_ISE:
        case ITM_ISG:
        case ITM_ISZ:
        case ITM_DSE:
        case ITM_DSL:
        case ITM_DSZ:
        case ITM_INPUT:
        case ITM_STO:
        case ITM_STOADD:
        case ITM_STOSUB:
        case ITM_STOMULT:
        case ITM_STODIV:
        case ITM_KEYQ:
        case ITM_DEC:
        case ITM_INC:
        case ITM_Xex:

        case ITM_STOMAX:
        case ITM_MVAR:
        case ITM_M_EDIN:
        case ITM_STOMIN:
        case ITM_STOCFG:
        case ITM_STOEL:
        case ITM_STOIJ:
        case ITM_STOS:
        case ITM_Tex:
        case ITM_XtoALPHA:
        case ITM_Yex:
        case ITM_Zex:
        case ITM_ALPHARL:
        case ITM_ALPHARR:
        case ITM_ALPHASL:
        case ITM_ALPHASR:
        case ITM_INTEGRAL: {
          return true;
        }
        default: {
          return false;
        }
      }
    }
  }



  static void _dynmenuConstructVars(int16_t menu, bool applyFilter, dataType_t typeFilter, bool isAngular, bool onlyWritable) {
    uint16_t numberOfBytes, numberOfVars;
    uint8_t *ptr;
    numberOfBytes = 1;
    numberOfVars = 0;
    memset(tmpString, 0, TMP_STR_LENGTH);
    for(int i=0; i<numberOfNamedVariables; i++) {
      calcRegister_t regist = i+FIRST_NAMED_VARIABLE;
      if(!applyFilter || _filterDataType(regist, typeFilter, isAngular)) {
        xcopy(tmpString + 15 * numberOfVars, allNamedVariables[i].variableName + 1, allNamedVariables[i].variableName[0]);
        numberOfVars++;
        numberOfBytes += 1 + allNamedVariables[i].variableName[0];
      }
    }
    for(int i=12; i<NUMBER_OF_RESERVED_VARIABLES; i++) {
      calcRegister_t regist = i+FIRST_RESERVED_VARIABLE;
      if((!applyFilter || _filterDataType(regist, typeFilter, isAngular)) && (!onlyWritable || _registerIsWritable(regist))) {
        xcopy(tmpString + 15 * numberOfVars, allReservedVariables[i].reservedVariableName + 1, allReservedVariables[i].reservedVariableName[0]);
        numberOfVars++;
        numberOfBytes += 1 + allReservedVariables[i].reservedVariableName[0];
      }
    }

    if(numberOfVars != 0) {
      qsort(tmpString, numberOfVars, 15, sortMenu);
    }

    ptr = malloc(numberOfBytes);
    dynamicSoftmenu[menu].menuContent = ptr;
    for(int i=0; i<numberOfVars; i++) {
      int16_t len = stringByteLength(tmpString + 15*i) + 1;
      xcopy(ptr, tmpString + 15*i, len);
      ptr += len;
    }

    dynamicSoftmenu[menu].numItems = numberOfVars;
  }



  static void _dynmenuConstructMVarsFromPgm(uint16_t label, uint16_t *numberOfBytes, uint16_t *numberOfVars) {
    uint8_t  *step;
    step = labelList[label].instructionPointer;
    while((*numberOfVars < 18) && checkOpCodeOfStep(step, ITM_MVAR) && *(step + 2) == STRING_LABEL_VARIABLE) {
      xcopy(tmpString + *numberOfBytes, step + 4, *(step + 3));
      (void)findOrAllocateNamedVariable(tmpString + *numberOfBytes);
      *numberOfBytes += *(step + 3) + 1;
      (*numberOfVars)++;
      step = findNextStep(step);
    }
  }



  static void _dynmenuConstructMVars(int16_t menu) {
    uint16_t numberOfBytes = 0;
    uint16_t numberOfVars = 0;
    memset(tmpString, 0, TMP_STR_LENGTH);

    if(currentMvarLabel != INVALID_VARIABLE) {
      _dynmenuConstructMVarsFromPgm(currentMvarLabel - FIRST_LABEL, &numberOfBytes, &numberOfVars);
    }
    else if(currentSolverStatus & SOLVER_STATUS_USES_FORMULA) {
      char *bufPtr = tmpString;
      uint8_t errorCode = lastErrorCode;
      lastErrorCode = ERROR_NONE;
      parseEquation(currentFormula, EQUATION_PARSER_MVAR, tmpString + TMP_STR_LENGTH - AIM_BUFFER_LENGTH, tmpString);
      while(*bufPtr != 0 || numberOfVars < 6) {
        numberOfVars += 1;
        numberOfBytes += stringByteLength(bufPtr) + 1;
        bufPtr += stringByteLength(bufPtr) + 1;
      }
      lastErrorCode = errorCode;
    }
    else {
      _dynmenuConstructMVarsFromPgm(currentSolverProgram, &numberOfBytes, &numberOfVars);
    }

    dynamicSoftmenu[menu].menuContent = malloc(numberOfBytes);
    xcopy(dynamicSoftmenu[menu].menuContent, tmpString, numberOfBytes);
    dynamicSoftmenu[menu].numItems = numberOfVars;
  }



  static void _dynmenuConstructUser(int16_t menu) {
    userMenuItem_t *menuData = (dynamicSoftmenu[menu].menuItem == -MNU_DYNAMIC) ? userMenus[currentUserMenu].menuItem : (dynamicSoftmenu[menu].menuItem == -MNU_MyMenu) ? userMenuItems : (dynamicSoftmenu[menu].menuItem == -MNU_MyAlpha) ? userAlphaItems : userPfnItems;
    int16_t i, numberOfBytes = 1;
    uint8_t *ptr;

    for(i = 0; i < 18; i++) {
      if(menuData[i].argumentName[0] != 0) {
        numberOfBytes += stringByteLength(menuData[i].argumentName) + 1;
      }
      else if(menuData[i].item == ITM_NOP || menuData[i].item == ITM_NULL) {
        numberOfBytes += 1;
      }
      else if(indexOfItems[abs(menuData[i].item)].itemCatalogName[0] == 0) {
        numberOfBytes += stringByteLength(indexOfItems[abs(menuData[i].item)].itemSoftmenuName) + 1;
      }
      else {
        numberOfBytes += stringByteLength(indexOfItems[abs(menuData[i].item)].itemCatalogName) + 1;
      }
    }
    ptr = malloc(numberOfBytes);
    dynamicSoftmenu[menu].menuContent = ptr;
    for(i = 0; i < 18; i++) {
      const char *lbl;
      if(menuData[i].argumentName[0] != 0) {
        lbl = menuData[i].argumentName;
      }
      else if(menuData[i].item == ITM_NULL) {
        lbl = "";
      }
      else if(indexOfItems[abs(menuData[i].item)].itemCatalogName[0] == 0) {
        lbl = indexOfItems[abs(menuData[i].item)].itemSoftmenuName;
      }
      else {
        lbl = indexOfItems[abs(menuData[i].item)].itemCatalogName;
      }
      int16_t len = stringByteLength(lbl) + 1;
      xcopy(ptr, lbl, len);
      ptr += len;
    }
    dynamicSoftmenu[menu].numItems = (numberOfBytes <= 19) ? 0 : 18;
  }



  static void initVariableSoftmenu(int16_t menu) {
    int16_t i, numberOfBytes, numberOfGlobalLabels;
    uint8_t *ptr;

    free(dynamicSoftmenu[menu].menuContent);

    switch(-dynamicSoftmenu[menu].menuItem) {
      case MNU_MyAlpha: {
        _dynmenuConstructUser(menu);
        break;
      }

      case MNU_MyMenu: {
        _dynmenuConstructUser(menu);
        break;
      }

      case MNU_MyPFN: {
        _dynmenuConstructUser(menu);
        break;
      }
      
      case MNU_VAR: {
        _dynmenuConstructVars(menu, false, 0, false, _isWritingTam());
        break;
      }

      case MNU_PROG:
      case MNU_PROGS: {
        numberOfBytes = 1;
        numberOfGlobalLabels = 0;
        memset(tmpString, 0, TMP_STR_LENGTH);
        for(i=0; i<numberOfLabels; i++) {
          if(labelList[i].step > 0) { // Global label
            xcopy(tmpString + 15 * numberOfGlobalLabels, labelList[i].labelPointer + 1, labelList[i].labelPointer[0]);
            numberOfGlobalLabels++;
            numberOfBytes += 1 + labelList[i].labelPointer[0];
          }
        }

        if(numberOfGlobalLabels != 0) {
          qsort(tmpString, numberOfGlobalLabels, 15, sortMenu);
        }

        ptr = malloc(numberOfBytes);
        dynamicSoftmenu[menu].menuContent = ptr;
        for(i=0; i<numberOfGlobalLabels; i++) {
          int16_t len = stringByteLength(tmpString + 15*i) + 1;
          xcopy(ptr, tmpString + 15*i, len);
          ptr += len;
        }

        dynamicSoftmenu[menu].numItems = numberOfGlobalLabels;
        break;
      }

      case MNU_MATRS: {
        _dynmenuConstructVars(menu, true, dtReal34Matrix, false, false);
        break;
      }

      case MNU_STRINGS: {
        _dynmenuConstructVars(menu, true, dtString, false, false);
        break;
      }

      case MNU_DATES: {
        _dynmenuConstructVars(menu, true, dtDate, false, false);
        break;
      }

      case MNU_TIMES: {
        _dynmenuConstructVars(menu, true, dtTime, false, false);
        break;
      }

      case MNU_ANGLES: {
        _dynmenuConstructVars(menu, true, dtReal34, true, false);
        break;
      }

      case MNU_SINTS: {
        _dynmenuConstructVars(menu, true, dtShortInteger, false, false);
        break;
      }

      case MNU_LINTS: {
        _dynmenuConstructVars(menu, true, dtLongInteger, false, false);
        break;
      }

      case MNU_REALS: {
        _dynmenuConstructVars(menu, true, dtReal34, false, false);
        break;
      }

      case MNU_CPXS: {
        _dynmenuConstructVars(menu, true, dtComplex34, false, false);
        break;
      }

      case MNU_CONFIG: {
        _dynmenuConstructVars(menu, true, dtConfig, false, false);
        break;
      }

      case MNU_ALLVAR: {
        _dynmenuConstructVars(menu, false, 0, false, false);
        break;
      }

      case MNU_MVAR: {
        _dynmenuConstructMVars(menu);
        break;
      }

      case MNU_MENUS: {
        numberOfBytes = 1;
        numberOfGlobalLabels = 0;
        memset(tmpString, 0, TMP_STR_LENGTH);
        for(i=0; i<LAST_ITEM; i++) {
          if((indexOfItems[i].status & CAT_STATUS) == CAT_MENU && indexOfItems[i].itemCatalogName[0] != 0 && i != MNU_CATALOG && i != MNU_MENUS) {
            int16_t len = stringByteLength(indexOfItems[i].itemCatalogName);
            xcopy(tmpString + 15 * numberOfGlobalLabels, indexOfItems[i].itemCatalogName, len);
            numberOfGlobalLabels++;
            numberOfBytes += 1 + len;
          }
        }
        for(i=0; i<numberOfUserMenus; i++) {
          int16_t len = stringByteLength(userMenus[i].menuName);
          xcopy(tmpString + 15 * numberOfGlobalLabels, userMenus[i].menuName, len);
          numberOfGlobalLabels++;
          numberOfBytes += 1 + len;
        }

        if(numberOfGlobalLabels != 0) {
          qsort(tmpString, numberOfGlobalLabels, 15, sortMenu);
        }

        ptr = malloc(numberOfBytes);
        dynamicSoftmenu[menu].menuContent = ptr;
        for(i=0; i<numberOfGlobalLabels; i++) {
          int16_t len = stringByteLength(tmpString + 15*i) + 1;
          xcopy(ptr, tmpString + 15*i, len);
          ptr += len;
        }

        dynamicSoftmenu[menu].numItems = numberOfGlobalLabels;
        break;
      }

      case MNU_DYNAMIC: {
        _dynmenuConstructUser(menu);
        break;
      }

      case ITM_MENU: {
        numberOfBytes = 0;
        numberOfGlobalLabels = 0;
        memset(tmpString, 0, TMP_STR_LENGTH);
        for(i=0; i<18; i++) {
          xcopy(tmpString + numberOfBytes, programmableMenu.itemName[i], stringByteLength(programmableMenu.itemName[i]) + 1);
          numberOfBytes += stringByteLength(programmableMenu.itemName[i]) + 1;
        }

        ptr = malloc(numberOfBytes);
        dynamicSoftmenu[menu].menuContent = ptr;
        xcopy(ptr, tmpString, numberOfBytes);

        dynamicSoftmenu[menu].numItems = 18;
        break;
      }

      default: {
        sprintf(errorMessage, "In function initVariableSoftmenu: unexpected variable softmenu %" PRId16 "!", (int16_t)(-dynamicSoftmenu[menu].menuItem));
        bugScreen(errorMessage);
      }
    }
  }



  void showSoftkey(const char *label, int16_t xSoftkey, int16_t ySoftKey, videoMode_t videoMode, bool topLine, bool bottomLine) {
    int16_t x1, y1, x2, y2;
    int16_t w;
    char l[15];

    if((calcMode == cmPlotStat || calcMode == cmGraph) && xSoftkey >= 2) {           //prevent softkeys columns 3-6 from displaying over the graph
        return;
    }

    if(0 <= xSoftkey && xSoftkey <= 5) {
      x1 = 67 * xSoftkey - 1;
      x2 = x1 + 67;
    }
    else {
      sprintf(errorMessage, "In function showSoftkey: xSoftkey=%" PRId16 " must be from 0 to 5" , xSoftkey);
      bugScreen(errorMessage);
      return;
    }

    if(0 <= ySoftKey && ySoftKey <= 2) {
      y1 = 217 - SOFTMENU_HEIGHT * ySoftKey;
      y2 = y1 + SOFTMENU_HEIGHT;
    }
    else {
      sprintf(errorMessage, "In function showSoftkey: ySoftKey=%" PRId16 " but must be from 0 to 2!" , ySoftKey);
      bugScreen(errorMessage);
      return;
    }

    // Draw the frame
    //   Top line
    if(topLine) {
      lcd_fill_rect(max(0, x1), y1, min(x2, SCREEN_WIDTH) - max(0, x1), 1, (videoMode == vmNormal ? LCD_EMPTY_VALUE : LCD_SET_VALUE));
    }

    //   Bottom line
    if(y1 + SOFTMENU_HEIGHT <= min(y2, 239) && bottomLine) {
      lcd_fill_rect(max(0, x1), y1 + SOFTMENU_HEIGHT, min(x2, SCREEN_WIDTH) - max(0, x1), 1, (videoMode == vmNormal ? LCD_EMPTY_VALUE : LCD_SET_VALUE));
    }

    //   Left line
    if(x1 >= 0) {
      lcd_fill_rect(x1, y1, 1, min(y2, SCREEN_HEIGHT - 1) + 1 - y1, (videoMode == vmNormal ? LCD_EMPTY_VALUE : LCD_SET_VALUE));
    }

    //   Right line
    if(x2 < SCREEN_WIDTH) {
      lcd_fill_rect(x2, y1, 1, min(y2, SCREEN_HEIGHT - 1) + 1 - y1, (videoMode == vmNormal ? LCD_EMPTY_VALUE : LCD_SET_VALUE));
    }

    // Clear inside the frame
    lcd_fill_rect(x1 + 1, y1 + 1, min(x2, SCREEN_WIDTH) - x1 - 1, min(y2, SCREEN_HEIGHT) - y1 - 1, (videoMode == vmNormal ? LCD_SET_VALUE : LCD_EMPTY_VALUE));

    xcopy(l, label, stringByteLength(label) + 1);
    char *lw = stringAfterPixels(l, &standardFont, (xSoftkey == 5 ? 65 : 66), false, false);
    *lw = 0;
    w = stringWidth(l, &standardFont, false, false);

    showString(l, &standardFont, x1 + (xSoftkey == 5 ? 33 : 34) - w/2, y1 + 2, videoMode, false, false);
  }


  void setCurrentUserMenu(int16_t item, char* funcParam) {
    if (item == -MNU_DYNAMIC) {
      for(uint32_t i = 0; i < numberOfUserMenus; ++i) {
        if(compareString(funcParam, userMenus[i].menuName, CMP_NAME) == 0) {
            currentUserMenu = i;
        }
      }
    }
  }


  void showSoftmenuCurrentPart(void) {
    smStackMode_t sm = smStackMode[0];
    int16_t x, y, yDotted=0, currentFirstItem, item, numberOfItems, m = softmenuStacks[sm].item[0].softmenuId;
    bool    dottedTopLine;

    if(tamIsWaitingKey()) {
      for(y=0; y<=2; y++) {
        for(x=0; x<6; x++) {
          showSoftkey("", x, y, vmReverse, true, true);
        }
      }
      return;
    }

    if(m < NUMBER_OF_DYNAMIC_SOFTMENUS) { // Dynamic softmenu
      if(softmenu[m].menuItem != cachedDynamicMenu || softmenu[m].menuItem == -MNU_DYNAMIC) {
        initVariableSoftmenu(m);
        cachedDynamicMenu = softmenu[m].menuItem;
      }
      numberOfItems = dynamicSoftmenu[m].numItems;
    }
    else if(softmenu[m].menuItem == -MNU_EQN && numberOfFormulae == 0) {
      numberOfItems = 1;
    }
    else { // Static softmenu
      numberOfItems = softmenu[m].numItems;
    }
    currentFirstItem = softmenuStacks[sm].item[0].firstItem;
    
    if(numberOfItems <= 18) {
      dottedTopLine = false;
      if(catalog != CATALOG_NONE) {
        currentFirstItem = softmenuStacks[sm].item[0].firstItem = 0;
        setCatalogLastPos();
      }
    }
    else {
      dottedTopLine = true;
      yDotted = min(3, (numberOfItems + modulo(currentFirstItem - numberOfItems, 6))/6 - currentFirstItem/6) - 1;

      if(m >= NUMBER_OF_DYNAMIC_SOFTMENUS) { // Static softmenu
        item = 6 * (currentFirstItem / 6 + yDotted);
        if(                softmenu[m].softkeyItem[item]==0 && softmenu[m].softkeyItem[item+1]==0 && softmenu[m].softkeyItem[item+2]==0 && softmenu[m].softkeyItem[item+3]==0 && softmenu[m].softkeyItem[item+4]==0 && softmenu[m].softkeyItem[item+5]==0) {
          yDotted--;
        }

        item = 6 * (currentFirstItem / 6 + yDotted);
        if(yDotted >= 0 && softmenu[m].softkeyItem[item]==0 && softmenu[m].softkeyItem[item+1]==0 && softmenu[m].softkeyItem[item+2]==0 && softmenu[m].softkeyItem[item+3]==0 && softmenu[m].softkeyItem[item+4]==0 && softmenu[m].softkeyItem[item+5]==0) {
          yDotted--;
        }

        item = 6 * (currentFirstItem / 6 + yDotted);
        if(yDotted >= 0 && softmenu[m].softkeyItem[item]==0 && softmenu[m].softkeyItem[item+1]==0 && softmenu[m].softkeyItem[item+2]==0 && softmenu[m].softkeyItem[item+3]==0 && softmenu[m].softkeyItem[item+4]==0 && softmenu[m].softkeyItem[item+5]==0) {
          yDotted--;
        }
      }
    }

    if(m < NUMBER_OF_DYNAMIC_SOFTMENUS) { // Dynamic softmenu
      if(numberOfItems == 0) {
        for(x=0; x<6; x++) {
          showSoftkey("", x, 0, vmNormal, true, true);
        }
      }
      else {
        uint8_t *ptr = getNthString(dynamicSoftmenu[m].menuContent, currentFirstItem);
        for(y=0; y<3; y++) {
          for(x=0; x<6; x++) {
            if(x + 6*y + currentFirstItem < numberOfItems) {
              if(*ptr != 0) {
                videoMode_t vm = vmNormal;
                switch(-softmenu[m].menuItem) {
                  case MNU_MENUS: {
                    vm = vmReverse;
                    break;
                  }
                  case MNU_MyMenu: {
                    vm = (userMenuItems[x + 6*y].item < 0) ? vmReverse : vmNormal;
                    break;
                  }
                  case MNU_MyAlpha: {
                    vm = (userAlphaItems[x + 6*y].item < 0) ? vmReverse : vmNormal;
                    break;
                  }
                  case MNU_MyPFN: {
                    vm = (userPfnItems[x + 6*y].item < 0) ? vmReverse : vmNormal;
                    break;
                  }
                  case MNU_DYNAMIC: {
                    vm = (userMenus[currentUserMenu].menuItem[x + 6*y].item < 0) ? vmReverse : vmNormal;
                    break;
                  }
                  case MNU_1STDERIV:
                  case MNU_2NDDERIV:
                  case MNU_MVAR: {
                    if(!compareString((char *)getNthString(dynamicSoftmenu[m].menuContent, x+6*y), indexOfItems[ITM_DRAW].itemSoftmenuName, CMP_NAME)) {
                      vm = vmReverse;
                    }
                    break;
                  }
                  default: {
                    vm = vmNormal;
                    break;
                  }
                }
                showSoftkey((char *)ptr, x, y, vm, true, true);
              }
              ptr += stringByteLength((char *)ptr) + 1;
            }
          }
        }
      }
      if(softmenu[m].menuItem == -MNU_MVAR && (currentSolverStatus & SOLVER_STATUS_USES_FORMULA) && (currentSolverStatus & SOLVER_STATUS_INTERACTIVE)) {
        showEquation(currentFormula, 0, EQUATION_NO_CURSOR, false, NULL, NULL);
      }
    }
    else {
      const int16_t *softkeyItem = softmenu[m].softkeyItem + currentFirstItem;
      for(y=currentFirstItem/6; y<=min(currentFirstItem/6+2, numberOfItems/6); y++, softkeyItem+=6) {
        for(x=0; x<6; x++) {
          if(softkeyItem + x >= softmenu[m].softkeyItem + numberOfItems) {
            item = ITM_NULL;
          }
          else {
            item = softkeyItem[x];
          }
          if(item < 0) { // softmenu
            int16_t menu = 0;
            while(softmenu[menu].menuItem != 0) {
              if(softmenu[menu].menuItem == item) {
                break;
              }
              menu++;
            }

            if(softmenu[menu].menuItem == 0) {
              sprintf(errorMessage, "In function showSoftmenuCurrentPart: softmenu ID %" PRId16 " not found!", item);
              bugScreen(errorMessage);
            }
            else {
              showSoftkey(indexOfItems[-softmenu[menu].menuItem].itemSoftmenuName, x, y-currentFirstItem/6, vmReverse, true, true);
            }
          }
          else if(item == 9999) {
            showSoftkey(indexOfItems[getSystemFlag(FLAG_MULTx) ? ITM_DOT : ITM_CROSS].itemSoftmenuName, x, y-currentFirstItem/6, vmNormal, true, true);
          }
          else if(item > 0 && indexOfItems[item%10000].itemSoftmenuName[0] != 0) { // softkey
            // item : +10000 -> no top line
            //        +20000 -> no bottom line
            //        +30000 -> neither top nor bottom line
            if(softmenu[m].menuItem == -MNU_FCNS) {
              showSoftkey(indexOfItems[item%10000].itemCatalogName,  x, y-currentFirstItem/6, vmNormal, (item/10000)==0 || (item/10000)==2, (item/10000)==0 || (item/10000)==1);
            }
            else if(item == ITM_PLOT || item == ITM_PLOT_LR || item == ITM_HPLOT  || item == ITM_DRAW) {       //colour PLOT in reverse font to appear to be menus
              showSoftkey(indexOfItems[item%10000].itemSoftmenuName, x, y-currentFirstItem/6, vmReverse, (item/10000)==0 || (item/10000)==2, (item/10000)==0 || (item/10000)==1);
            }
            else {
              showSoftkey(indexOfItems[item%10000].itemSoftmenuName, x, y-currentFirstItem/6, vmNormal, (item/10000)==0 || (item/10000)==2, (item/10000)==0 || (item/10000)==1);
            }
            if(indexOfItems[item%10000].func == itemToBeCoded) {
              // Strike out non coded functions
              int16_t yStroke = SCREEN_HEIGHT - (y-currentFirstItem/6)*23 - 3;
              for(int16_t xStroke=x*67 + 10; xStroke<x*67 + 57; xStroke++) {
                if(xStroke%3 == 0) {
                  yStroke--;
                }
                setBlackPixel(xStroke, yStroke);
              }
            }
          }
        }
      }
      if(softmenu[m].menuItem == -MNU_EQN) {
        showEquation(currentFormula, 0, EQUATION_NO_CURSOR, false, NULL, NULL);
        dottedTopLine = (numberOfFormulae >= 2);
        yDotted = 2;
      }
      if(softmenu[m].menuItem == -MNU_EQ_EDIT) {
        bool cursorShown;
        bool rightEllipsis;
        while(1) {
          showEquation(EQUATION_AIM_BUFFER, equationEditorScrollPosition, equationEditorCursor, true, &cursorShown, &rightEllipsis);
          if(cursorShown) {
            break;
          }
          if(equationEditorScrollPosition > equationEditorCursor) {
            --equationEditorScrollPosition;
          }
          else {
            ++equationEditorScrollPosition;
          }
        }
        if(!rightEllipsis && equationEditorScrollPosition > 0) {
          do {
            --equationEditorScrollPosition;
            showEquation(EQUATION_AIM_BUFFER, equationEditorScrollPosition, equationEditorCursor, true, &cursorShown, &rightEllipsis);
            if((!cursorShown) || rightEllipsis) {
              ++equationEditorScrollPosition;
              break;
            }
          } while(equationEditorScrollPosition > 0);
        }
        showEquation(EQUATION_AIM_BUFFER, equationEditorScrollPosition, equationEditorCursor, false, NULL, NULL);
      }
    }

    if(0 <= yDotted && yDotted <= 2) {
      yDotted = 217 - SOFTMENU_HEIGHT * yDotted;

      if(dottedTopLine) {
        for(x=0; x<SCREEN_WIDTH; x++) {
          if(x%8 < 4) {
            setBlackPixel(x, yDotted);
          }
          else {
            setWhitePixel(x, yDotted);
          }
        }
      }
    }
  }


  /* Softmenu Stack management
   *
   * All functions for soft,enus stacks management are here
   *
   */

 static void fillSoftmenuStack(smStackMode_t stackMode, int16_t softmenuId) {  
    for(int i=0; i<SOFTMENU_STACK_SIZE; i++) {
      softmenuStacks[stackMode].item[i].softmenuId = softmenuId;
      softmenuStacks[stackMode].item[i].firstItem  = 0;
    }
  }


  void softmenuStacksInit(void) {
    fillSoftmenuStack(smNormal, smMyMenu);                // Initialize RUM softmenu stack  (Normal mode)
    fillSoftmenuStack(smAim,    smMyAlpha);               // Initialize AIM softmenu stack
    fillSoftmenuStack(smPem,    smMyPFN);                 // Initialize PEM softmenu stack
    memset(&smStackMode, smNormal, sizeof(smStackMode));  // Initialize stack mode   stack
  }


  int16_t getSoftmenuId(int index) {
    smStackMode_t sm = smStackMode[0];
    
    return softmenuStacks[sm].item[index].softmenuId;
  }


  void setSoftmenuId(int index, int16_t menuId) {
    smStackMode_t sm = smStackMode[0];
    
    #if defined(PC_BUILD)
      printf("*** Set softmenu ID %3d %s\n",menuId, softMenuNames[baseMenu[sm]]);
    #endif // PC_BUILD
    
    softmenuStacks[sm].item[index].softmenuId = menuId;
  }


  int16_t getSoftmenuFirstItem(void) {
    smStackMode_t sm = smStackMode[0];
    
    return softmenuStacks[sm].item[0].firstItem;
  }


  void setSoftmenuFirstItem(int16_t item) {
    smStackMode_t sm = smStackMode[0];
    
    softmenuStacks[sm].item[0].firstItem = item;
  }


  smStackMode_t getSmStackMode(void) {
      return smStackMode[0];
  }


  void pushSmStackMode(smStackMode_t stackMode) {
    int i;
    
    for(i=SOFTMENU_STACKMODE_SIZE-1; i>0; i--) {
      smStackMode[i] = smStackMode[i-1];  
    }    
    smStackMode[0] = stackMode;
    fillSoftmenuStack(stackMode, baseMenu[stackMode]);  // Reset content of the new softmenu stack

    #if defined(PC_BUILD)
      printf("*** Push stack mode %d - baseMenu %s - calcMode %d\n",stackMode,softMenuNames[baseMenu[stackMode]],calcMode);
    #endif // PC_BUILD
  }


  void popSmStackMode(void) {
    int i;
    
    #if defined(PC_BUILD)
      printf("*** Pop  stack mode %d - baseMenu %s - calcMode %d\n",smStackMode[0],softMenuNames[baseMenu[smStackMode[0]]],calcMode);
    #endif // PC_BUILD
    
    for(i=0; i<SOFTMENU_STACKMODE_SIZE-1; i++) {
      smStackMode[i] = smStackMode[i+1];  
    }    
    smStackMode[SOFTMENU_STACKMODE_SIZE-1] = smNormal;    
  }


  /* Pushes a new softmenu on the softmenu stack.
   *
   * \param[in] softmenuId Softmenu ID
   */
  static void pushSoftmenu(int16_t softmenuId) {
    int i;
    int16_t userMenuId;
    smStackMode_t sm = smStackMode[0];

    if(softmenu[softmenuId].menuItem == -MNU_DYNAMIC) {       // Get currentUserMenu Id
      userMenuId = currentUserMenu;
      printf("***[DL]** pushSoftmenu userMenuId %d\n",userMenuId);
    } else {
      userMenuId = 0;
    }

    if((softmenuStacks[sm].item[0].softmenuId == softmenuId) && (softmenuStacks[sm].item[0].userMenuId == userMenuId)) { // The menu to push on the stack is already displayed
      return;
    }

    for(i=0; i<SOFTMENU_STACK_SIZE; i++) { // Searching the stack for the menu to push on the stack
      if((softmenuStacks[sm].item[i].softmenuId == softmenuId) && (softmenuStacks[sm].item[i].userMenuId == userMenuId)) { // if found, remove it
        xcopy(softmenuStacks[sm].item + 1, softmenuStacks[sm].item, i * sizeof(softmenuStackItem_t));
        break;
      }
    }

    if(i == SOFTMENU_STACK_SIZE) { // The menu to push was not found on the stack
      xcopy(softmenuStacks[sm].item + 1, softmenuStacks[sm].item, (SOFTMENU_STACK_SIZE - 1) * sizeof(softmenuStackItem_t)); // shifting the entire stack
    }

    #if defined(PC_BUILD)
      printf("*** Push softmenuId %16s - stack %d - calcMode %d\n", softMenuNames[softmenuId], sm, calcMode);
    #endif // PC_BUILD
    
    softmenuStacks[sm].item[0].softmenuId = softmenuId;
    softmenuStacks[sm].item[0].firstItem = lastCatalogPosition[catalog];
    softmenuStacks[sm].item[0].userMenuId = userMenuId;
  }



  void popSoftmenu(void) {
    smStackMode_t sm = smStackMode[0];
    
    #if defined(PC_BUILD)
      printf("*** Pop  softmenuId %16s - stack %d - calcMode %d - tam.mode %d - tam.alpha %d\n",softMenuNames[softmenuStacks[sm].item[0].softmenuId],sm,calcMode,tam.mode,tam.alpha);
    #endif // PC_BUILD
    
    screenUpdatingMode &= ~SCRUPD_MANUAL_MENU;

    xcopy(softmenuStacks[sm].item, softmenuStacks[sm].item + 1, (SOFTMENU_STACK_SIZE - 1) * sizeof(softmenuStackItem_t)); // shifting the entire stack
    softmenuStacks[sm].item[3].softmenuId = baseMenu[sm];  // Put default menu in the last stack element
    softmenuStacks[sm].item[0].firstItem = 0;
   
    // failsafe return to Normal stack
    if (calcMode == cmNormal) {
      while (sm != smNormal) {
        popSmStackMode();
        sm = smStackMode[0];
      }
    }

    enterAsmModeIfMenuIsACatalog(softmenu[softmenuStacks[sm].item[0].softmenuId].menuItem);

    if(softmenu[softmenuStacks[sm].item[0].softmenuId].menuItem == -MNU_MVAR) {
      setSystemFlag(FLAG_VMDISP);
    }
    else {
      clearSystemFlag(FLAG_VMDISP);
    }

    if(softmenu[softmenuStacks[sm].item[0].softmenuId].menuItem == -MNU_DYNAMIC) {
      currentUserMenu = softmenuStacks[sm].item[0].userMenuId;
    }
  }



  void showSoftmenu(int16_t id) {
    int16_t m;
    smStackMode_t sm = smStackMode[0];

    if(softmenu[softmenuStacks[sm].item[0].softmenuId].menuItem == -MNU_MENUS) {
      leaveAsmMode();
    }
    enterAsmModeIfMenuIsACatalog(id);

    if(id == 0) {
      bugScreen("In function showSoftmenu: id must not be 0!");
      return;
    }

    screenUpdatingMode &= ~SCRUPD_MANUAL_MENU;

    if(id == -MNU_ALPHAINTL && alphaCase == AC_LOWER) { // alphaINTL
      id = -MNU_ALPHAintl;
    }
    else if(id == -MNU_ALPHA_OMEGA && alphaCase == AC_LOWER) { // alpha...omega
      id = -MNU_alpha_omega;
    }
    else if(id == -MNU_Solver || id == -MNU_Sf || id == -MNU_1STDERIV || id == -MNU_2NDDERIV) {
      int32_t numberOfVars = -1;
      currentSolverStatus = SOLVER_STATUS_USES_FORMULA | SOLVER_STATUS_INTERACTIVE;
      currentMvarLabel = INVALID_VARIABLE;
      switch(-id) {
        case MNU_Solver: {
          currentSolverStatus |= SOLVER_STATUS_EQUATION_SOLVER;
          break;
        }
        case MNU_Sf: {
          currentSolverStatus |= SOLVER_STATUS_EQUATION_INTEGRATE;
          break;
        }
        case MNU_1STDERIV: {
          currentSolverStatus |= SOLVER_STATUS_EQUATION_1ST_DERIVATIVE;
          break;
        }
        case MNU_2NDDERIV: {
          currentSolverStatus |= SOLVER_STATUS_EQUATION_2ND_DERIVATIVE;
          break;
        }
      }
      cachedDynamicMenu = 0;
      parseEquation(currentFormula, EQUATION_PARSER_MVAR, aimBuffer, tmpString);
      id = -MNU_MVAR;
      while((getNthString((uint8_t *)tmpString, ++numberOfVars))[0] != 0) {
      }
      if(numberOfVars > 12) {
        displayCalcErrorMessage(ERROR_EQUATION_TOO_COMPLEX, ERR_REGISTER_LINE, NIM_REGISTER_LINE);
        errorMoreInfo("there are more than 11 variables in this equation!");
      }
      else if((currentSolverStatus & SOLVER_STATUS_EQUATION_MODE) == SOLVER_STATUS_EQUATION_1ST_DERIVATIVE || (currentSolverStatus & SOLVER_STATUS_EQUATION_MODE) == SOLVER_STATUS_EQUATION_2ND_DERIVATIVE) {
        if((getNthString((uint8_t *)tmpString, 1))[0] == 0) {
          currentSolverVariable = findOrAllocateNamedVariable((char *)getNthString((uint8_t *)tmpString, 0));
          reallyRunFunction(ITM_STO, currentSolverVariable);
          if((currentSolverStatus & SOLVER_STATUS_EQUATION_MODE) == SOLVER_STATUS_EQUATION_1ST_DERIVATIVE) {
            fn1stDerivEq(NOPARAM);
          }
          else {
            fn2ndDerivEq(NOPARAM);
          }
        }
      }
    }
    else if(id == -MNU_ADV || id == -MNU_EQN) {
      currentSolverStatus &= ~SOLVER_STATUS_INTERACTIVE;
      for(int i=0; i<SOFTMENU_STACK_SIZE; i++) { // Searching the stack for MNU_MVAR
        if(softmenu[softmenuStacks[sm].item[i].softmenuId].menuItem == -MNU_MVAR) { // if found, remove it
          xcopy(softmenuStacks[sm].item, softmenuStacks[sm].item +i + 1, (SOFTMENU_STACK_SIZE - i - 1) * sizeof(softmenuStackItem_t)); // shifting the entire stack
          memset(softmenuStacks[sm].item + SOFTMENU_STACK_SIZE - 1, 0, sizeof(softmenuStackItem_t)); // Put MyMenu in the last stack element
          --i; // redo
        }
      }
    }

    m = 0;
    while(softmenu[m].menuItem != 0) {
      if(softmenu[m].menuItem == id) {
       break;
      }
      m++;
    }

    if(softmenu[m].menuItem == 0) {
      sprintf(errorMessage, "In function showSoftmenu: softmenu %" PRId16 " not found!", id);
      bugScreen(errorMessage);
    }
    else {
      //if(tamIsActive() || (calcMode == cmAssign && tam.alpha)) {
      if(tamIsActive() && !tam.alpha) {           // Don't count menus in aim as they will be trashed with the aim menu stack
         numberOfTamMenusToPop++;
      }
      pushSoftmenu(m);
      if(id == -MNU_MVAR) {
        setSystemFlag(FLAG_VMDISP);
      }
      else {
        clearSystemFlag(FLAG_VMDISP);
      }
    }
  }



  void setCatalogLastPos(void) {
    smStackMode_t sm = smStackMode[0];
  
    lastCatalogPosition[catalog] = (catalog ? softmenuStacks[sm].item[0].firstItem : 0);

    if(catalog == CATALOG_AINT) {
      lastCatalogPosition[CATALOG_aint] = softmenuStacks[sm].item[0].firstItem;
    }
    else if(catalog == CATALOG_aint) {
      lastCatalogPosition[CATALOG_AINT] = softmenuStacks[sm].item[0].firstItem;
    }
  }



  bool currentSoftmenuScrolls(void) {
    smStackMode_t sm = smStackMode[0];
    int16_t menuId = softmenuStacks[sm].item[0].softmenuId;
    return (menuId > 1 &&
      (   (menuId <  NUMBER_OF_DYNAMIC_SOFTMENUS && dynamicSoftmenu[menuId].numItems > 18)
       || (menuId >= NUMBER_OF_DYNAMIC_SOFTMENUS &&        softmenu[menuId].numItems > 18)));
  }



  bool isAlphabeticSoftmenu(void) {
    smStackMode_t sm = smStackMode[0];
    int16_t menuItem = softmenu[softmenuStacks[sm].item[0].softmenuId].menuItem;
    switch(menuItem) {
      case -MNU_ALPHAINTL:
      case -MNU_ALPHAintl:
      case -MNU_ALPHA_OMEGA:
      case -MNU_alpha_omega:
      case -MNU_ALPHAMATH:
      case -MNU_MyAlpha:
      case -MNU_ALPHADOT: {
        return true;
      }
      default: {
        return false;
      }
    }
  }
#endif // !TESTSUITE_BUILD



char *dynmenuGetLabel(int16_t menuitem) {
  return dynmenuGetLabelWithDup(menuitem, NULL);
}



char *dynmenuGetLabelWithDup(int16_t menuitem, int16_t *dupNum) {
  smStackMode_t sm = smStackMode[0];   
  if(dupNum) {
    *dupNum = 0;
  }
  if(menuitem < 0 || menuitem >= dynamicSoftmenu[softmenuStacks[sm].item[0].softmenuId].numItems) {
    return "";
  }
  char *labelName = (char *)dynamicSoftmenu[softmenuStacks[sm].item[0].softmenuId].menuContent;
  char *prevLabelName = labelName;
  while(menuitem > 0) {
    labelName += stringByteLength(labelName) + 1;
    menuitem--;
    if(dupNum) {
      if(compareString(labelName, prevLabelName, CMP_BINARY) == 0) {
        ++(*dupNum);
      }
      else {
        prevLabelName = labelName;
        *dupNum = 0;
      }
    }
  }
  return labelName;
}


void fnExitAllMenus(uint16_t unusedButMandatoryParameter) {
  #if !defined(TESTSUITE_BUILD)
    smStackMode_t sm = smStackMode[0];
    fillSoftmenuStack(sm, baseMenu[sm]);                // Initialize current softmenu stack with the related base menu
    catalog = CATALOG_NONE;
    if (sm == smAim) {
      setSystemFlag(FLAG_ALPHA);
    }
    else {
      clearSystemFlag(FLAG_ALPHA);
    }

    #if defined(PC_BUILD)
      printf("*** Exit all - stack %d\n",sm);
    #endif // PC_BUILD

  #endif // !TESTSUITE_BUILD
}