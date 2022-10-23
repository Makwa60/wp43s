// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "testSuite.h"

#include "charString.h"
#include "config.h"
#include "constants.h"
#include "conversionAngles.h"
#include "conversionUnits.h"
#include "core/memory.h"
#include "dateTime.h"
#include "debug.h"
#include "display.h"
#include "distributions/distributions.h"
#include "error.h"
#include "flags.h"
#include "fractions.h"
#include "items.h"
#include "logicalOps/logicalOps.h"
#include "mathematics/mathematics.h"
#include "recall.h"
#include "registers.h"
#include "registerValueConversions.h"
#include "reporter.h"
#include "sort.h"
#include "stack.h"
#include "stats.h"
#include "store.h"
#include <stdbool.h>
#include <string.h>
#include <libgen.h>

#include "wp43.h"

#define NUMBER_OF_CORRECT_SIGNIFICANT_DIGITS_EXPECTED 34


extern const int16_t menu_FCNS[];
extern const int16_t menu_CONST[];
extern const int16_t menu_MENUS[];
extern const int16_t menu_SYSFL[];
extern const int16_t menu_alpha_INTL[];
extern const int16_t menu_alpha_intl[];
extern const int16_t menu_REGIST[];
extern const softmenu_t softmenu[];
char msgString[200], line[100000], lastInParameters[10000], fileName[1000], *filePath, filePathName[2000], registerExpectedAndValue[1000], realString[1000];
int32_t lineNumber, numTestsFile, numTestsTotal, failedTests;
int32_t functionIndex, funcType, correctSignificantDigits;
void (*funcNoParam)(uint16_t);
void (*funcCvt)(uint16_t);
bool testStarted;
bool testPassed;
uint32_t testInFile;

const funcTest_t funcTestNoParam[] = {
  {"fn10Pow",                fn10Pow               },
  {"fn2Pow",                 fn2Pow                },
  {"fnAdd",                  fnAdd                 },
  {"fnAim",                  fnAim                 },
  {"fnAgm",                  fnAgm                 },
  {"fnArccos",               fnArccos              },
  {"fnArccosh",              fnArccosh             },
  {"fnArcsin",               fnArcsin              },
  {"fnArcsinh",              fnArcsinh             },
  {"fnArctan",               fnArctan              },
  {"fnArctanh",              fnArctanh             },
  {"fnArg",                  fnArg                 },
  {"fnAsr",                  fnAsr                 },
  {"fnBatteryVoltage",       fnBatteryVoltage      },
  {"fnBesselJ",              fnBesselJ             },
  {"fnBesselY",              fnBesselY             },
  {"fnBinomialI",            fnBinomialI           },
  {"fnBinomialL",            fnBinomialL           },
  {"fnBinomialP",            fnBinomialP           },
  {"fnBinomialR",            fnBinomialR           },
  {"fnBn",                   fnBn                  },
  {"fnBnStar",               fnBnStar              },
  {"fnCauchyI",              fnCauchyI             },
  {"fnCauchyL",              fnCauchyL             },
  {"fnCauchyP",              fnCauchyP             },
  {"fnCauchyR",              fnCauchyR             },
  {"fnCb",                   fnCb                  },
  {"fnCeil",                 fnCeil                },
  {"fnChangeSign",           fnChangeSign          },
  {"fnChebyshevT",           fnChebyshevT          },
  {"fnChebyshevU",           fnChebyshevU          },
  {"fnChi2I",                fnChi2I               },
  {"fnChi2L",                fnChi2L               },
  {"fnChi2P",                fnChi2P               },
  {"fnChi2R",                fnChi2R               },
  {"fnClearRegisters",       fnClearRegisters      },
  {"fnClearStack",           fnClearStack          },
  {"fnClFAll",               fnClFAll              },
  {"fnClSigma",              fnClSigma             },
  {"fnClX",                  fnClX                 },
  {"fnConfigChina",          fnConfigChina         },
  {"fnConfigEurope",         fnConfigEurope        },
  {"fnConfigIndia",          fnConfigIndia         },
  {"fnConfigJapan",          fnConfigJapan         },
  {"fnConfigUk",             fnConfigUk            },
  {"fnConfigUsa",            fnConfigUsa           },
  {"fnConjugate",            fnConjugate           },
  {"fnCos",                  fnCos                 },
  {"fnCosh",                 fnCosh                },
  {"fnCountBits",            fnCountBits           },
  {"fnCross",                fnCross               },
  {"fnCube",                 fnCube                },
  {"fnCubeRoot",             fnCubeRoot            },
  {"fnCxToRe",               fnCxToRe              },
  {"fnCvtCToF",              fnCvtCToF             },
  {"fnCvtDegToDms",          fnCvtDegToDms         },
  {"fnCvtDegToRad",          fnCvtDegToRad         },
  {"fnCvtDmsToDeg",          fnCvtDmsToDeg         },
  {"fnCvtFToC",              fnCvtFToC             },
  {"fnCvtRadToDeg",          fnCvtRadToDeg         },
  {"fnCyx",                  fnCyx                 },
  {"fnDateTo",               fnDateTo              },
  {"fnDateToJulian",         fnDateToJulian        },
  {"fnDay",                  fnDay                 },
  {"fnDblDivide",            fnDblDivide           },
  {"fnDblDivideRemainder",   fnDblDivideRemainder  },
  {"fnDblMultiply",          fnDblMultiply         },
  {"fnDec",                  fnDec                 },
  {"fnDecomp",               fnDecomp              },
  {"fnDeltaPercent",         fnDeltaPercent        },
  {"fnDenMax",               fnDenMax              },
  {"fnDivide",               fnDivide              },
  {"fnDot",                  fnDot                 },
  {"fnDrop",                 fnDrop                },
  {"fnDropY",                fnDropY               },
  {"fnEllipticE",            fnEllipticE           },
  {"fnEllipticEphi",         fnEllipticEphi        },
  {"fnEllipticFphi",         fnEllipticFphi        },
  {"fnEllipticK",            fnEllipticK           },
  {"fnEllipticPi",           fnEllipticPi          },
  {"fnErf",                  fnErf                 },
  {"fnErfc",                 fnErfc                },
  {"fnExp",                  fnExp                 },
  {"fnExpM1",                fnExpM1               },
  {"fnExpMod",               fnExpMod              },
  {"fnExponentialI",         fnExponentialI        },
  {"fnExponentialL",         fnExponentialL        },
  {"fnExponentialP",         fnExponentialP        },
  {"fnExponentialR",         fnExponentialR        },
  {"fnExpt",                 fnExpt                },
  {"fnFactorial",            fnFactorial           },
  {"fnFib",                  fnFib                 },
  {"fnFillStack",            fnFillStack           },
  {"fnFloor",                fnFloor               },
  {"fnFp",                   fnFp                  },
  {"fnFreeFlashMemory",      fnFreeFlashMemory     },
  {"fnFreeMemory",           fnFreeMemory          },
  {"fnF_I",                  fnF_I                 },
  {"fnF_L",                  fnF_L                 },
  {"fnF_P",                  fnF_P                 },
  {"fnF_R",                  fnF_R                 },
  {"fnGamma",                fnGamma               },
  {"fnGammaP",               fnGammaP              },
  {"fnGammaQ",               fnGammaQ              },
  {"fnGammaXyLower",         fnGammaXyLower        },
  {"fnGammaXyUpper",         fnGammaXyUpper        },
  {"fnGcd",                  fnGcd                 },
  {"fnGd",                   fnGd                  },
  {"fnGeometricI",           fnGeometricI          },
  {"fnGeometricL",           fnGeometricL          },
  {"fnGeometricP",           fnGeometricP          },
  {"fnGeometricR",           fnGeometricR          },
  {"fnGetIntegerSignMode",   fnGetIntegerSignMode  },
  {"fnGetLocR",              fnGetLocR             },
  {"fnGetRoundingMode",      fnGetRoundingMode     },
  {"fnGetSignificantDigits", fnGetSignificantDigits},
  {"fnGetStackSize",         fnGetStackSize        },
  {"fnGetWordSize",          fnGetWordSize         },
  {"fnHermite",              fnHermite             },
  {"fnHermiteP",             fnHermiteP            },
  {"fnHypergeometricI",      fnHypergeometricI     },
  {"fnHypergeometricL",      fnHypergeometricL     },
  {"fnHypergeometricP",      fnHypergeometricP     },
  {"fnHypergeometricR",      fnHypergeometricR     },
  {"fnIDiv",                 fnIDiv                },
  {"fnIDivR",                fnIDivR               },
  {"fnImaginaryPart",        fnImaginaryPart       },
  {"fnInvert",               fnInvert              },
  {"fnInvGd",                fnInvGd               },
  {"fnIp",                   fnIp                  },
  {"fnIsPrime",              fnIsPrime             },
  {"fnIxyz",                 fnIxyz                },
  {"fnJacobiAmplitude",      fnJacobiAmplitude     },
  {"fnJacobiCn",             fnJacobiCn            },
  {"fnJacobiDn",             fnJacobiDn            },
  {"fnJacobiSn",             fnJacobiSn            },
  {"fnJacobiZeta",           fnJacobiZeta          },
  {"fnJulianToDate",         fnJulianToDate        },
  {"fnLaguerre",             fnLaguerre            },
  {"fnLaguerreAlpha",        fnLaguerreAlpha       },
  {"fnLcm",                  fnLcm                 },
  {"fnLegendre",             fnLegendre            },
  {"fnLn",                   fnLn                  },
  {"fnLnP1",                 fnLnP1                },
  {"fnLnGamma",              fnLnGamma             },
  {"fnLog10",                fnLog10               },
  {"fnLog2",                 fnLog2                },
  {"fnLogisticI",            fnLogisticI           },
  {"fnLogisticL",            fnLogisticL           },
  {"fnLogisticP",            fnLogisticP           },
  {"fnLogisticR",            fnLogisticR           },
  {"fnLogNormalI",           fnLogNormalI          },
  {"fnLogNormalL",           fnLogNormalL          },
  {"fnLogNormalP",           fnLogNormalP          },
  {"fnLogNormalR",           fnLogNormalR          },
  {"fnLogXY",                fnLogXY               },
  {"fnLnBeta",               fnLnBeta              },
  {"fnBeta",                 fnBeta                },
  {"fnLogicalAnd",           fnLogicalAnd          },
  {"fnLogicalNand",          fnLogicalNand         },
  {"fnLogicalNor",           fnLogicalNor          },
  {"fnLogicalNot",           fnLogicalNot          },
  {"fnLogicalOr",            fnLogicalOr           },
  {"fnLogicalXnor",          fnLogicalXnor         },
  {"fnLogicalXor",           fnLogicalXor          },
  {"fnM1Pow",                fnM1Pow               },
  {"fnMagnitude",            fnMagnitude           },
  {"fnMaskl",                fnMaskl               },
  {"fnMaskr",                fnMaskr               },
  {"fnMin",                  fnMin                 },
  {"fnMax",                  fnMax                 },
  {"fnMant",                 fnMant                },
  {"fnMirror",               fnMirror              },
  {"fnMod",                  fnMod                 },
  {"fnMonth",                fnMonth               },
  {"fnMulMod",               fnMulMod              },
  {"fnMultiply",             fnMultiply            },
  {"fnNegBinomialI",         fnNegBinomialI        },
  {"fnNegBinomialL",         fnNegBinomialL        },
  {"fnNegBinomialP",         fnNegBinomialP        },
  {"fnNegBinomialR",         fnNegBinomialR        },
  {"fnNeighb",               fnNeighb              },
  {"fnNop",                  fnNop                 },
  {"fnNormalI",              fnNormalI             },
  {"fnNormalL",              fnNormalL             },
  {"fnNormalP",              fnNormalP             },
  {"fnNormalR",              fnNormalR             },
  {"fnParallel",             fnParallel            },
  {"fnPi",                   fnPi                  },
  {"fnPercent",              fnPercent             },
  {"fnPercentMRR",           fnPercentMRR          },
  {"fnPercentT",             fnPercentT            },
  {"fnPercentPlusMG",        fnPercentPlusMG       },
  {"fnPercentSigma",         fnPercentSigma        },
  {"fnPoissonI",             fnPoissonI            },
  {"fnPoissonL",             fnPoissonL            },
  {"fnPoissonP",             fnPoissonP            },
  {"fnPoissonR",             fnPoissonR            },
  {"fnPower",                fnPower               },
  {"fnPyx",                  fnPyx                 },
  {"fnRealPart",             fnRealPart            },
  {"fnRecallIJ",             fnRecallIJ            },
  {"fnReToCx",               fnReToCx              },
  {"fnRmd",                  fnRmd                 },
  {"fnRollDown",             fnRollDown            },
  {"fnRollUp",               fnRollUp              },
  {"fnRound",                fnRound               },
  {"fnRoundi",               fnRoundi              },
  {"fnSign",                 fnSign                },
  {"fnSin",                  fnSin                 },
  {"fnSinc",                 fnSinc                },
  {"fnSincpi",               fnSincpi              },
  {"fnSinh",                 fnSinh                },
  {"fnSlvq",                 fnSlvq                },
  {"fnSquare",               fnSquare              },
  {"fnStoreIJ",              fnStoreIJ             },
  {"fnSqrt1Px2",             fnSqrt1Px2            },
  {"fnSquareRoot",           fnSquareRoot          },
  {"fnSubtract",             fnSubtract            },
  {"fnSumXY",                fnSumXY               },
  {"fnSwapRealImaginary",    fnSwapRealImaginary   },
  {"fnSwapXY",               fnSwapXY              },
  {"fnTan",                  fnTan                 },
  {"fnTanh",                 fnTanh                },
  {"fnToDate",               fnToDate              },
  {"fnToHms",                fnToHms               },
  {"fnToPolar",              fnToPolar             },
  {"fnToReal",               fnToReal              },
  {"fnToRect",               fnToRect              },
  {"fnT_I",                  fnT_I                 },
  {"fnT_L",                  fnT_L                 },
  {"fnT_P",                  fnT_P                 },
  {"fnT_R",                  fnT_R                 },
  {"fnUlp",                  fnUlp                 },
  {"fnUnitVector",           fnUnitVector          },
  {"fnWday",                 fnWday                },
  {"fnWeibullI",             fnWeibullI            },
  {"fnWeibullL",             fnWeibullL            },
  {"fnWeibullP",             fnWeibullP            },
  {"fnWeibullR",             fnWeibullR            },
  {"fnWinverse",             fnWinverse            },
  {"fnWnegative",            fnWnegative           },
  {"fnWpositive",            fnWpositive           },
  {"fnXthRoot",              fnXthRoot             },
  {"fnXToDate",              fnXToDate             },
  {"fnYear",                 fnYear                },
  {"fnZeta",                 fnZeta                },
  {"",                       NULL                  }
};

const funcTest_t funcTestCvt[] = {
  {"fnCvtAcreHa",     fnCvtAcreHa    },
  {"fnCvtAcreusHa",   fnCvtAcreusHa  },
  {"fnCvtAtmPa",      fnCvtAtmPa     },
  {"fnCvtAuM",        fnCvtAuM       },
  {"fnCvtBarPa",      fnCvtBarPa     },
  {"fnCvtBtuJ",       fnCvtBtuJ      },
  {"fnCvtCalJ",       fnCvtCalJ      },
  {"fnCvtCwtKg",      fnCvtCwtKg     },
  {"fnCvtFlozukMl",   fnCvtFlozukMl  },
  {"fnCvtFlozusMl",   fnCvtFlozusMl  },
  {"fnCvtFtM",        fnCvtFtM       },
  {"fnCvtGalukL",     fnCvtGalukL    },
  {"fnCvtGalusL",     fnCvtGalusL    },
  {"fnCvtHpeW",       fnCvtHpeW      },
  {"fnCvtHpmW",       fnCvtHpmW      },
  {"fnCvtHpukW",      fnCvtHpukW     },
  {"fnCvtInchMm",     fnCvtInchMm    },
  {"fnCvtInhgPa",     fnCvtInhgPa    },
  {"fnCvtLbfN",       fnCvtLbfN      },
  {"fnCvtLbfftNm",    fnCvtLbfftNm   },
  {"fnCvtLbKg",       fnCvtLbKg      },
  {"fnCvtLyM",        fnCvtLyM       },
  {"fnCvtMiKm",       fnCvtMiKm      },
  {"fnCvtNmiKm",      fnCvtNmiKm     },
  {"fnCvtOzG",        fnCvtOzG       },
  {"fnCvtPcM",        fnCvtPcM       },
  {"fnCvtPointMm",    fnCvtPointMm   },
  {"fnCvtPsiPa",      fnCvtPsiPa     },
  {"fnCvtQuartL",     fnCvtQuartL    },
  {"fnCvtSfeetM",     fnCvtSfeetM    },
  {"fnCvtShortcwtKg", fnCvtShortcwtKg},
  {"fnCvtShorttonKg", fnCvtShorttonKg},
  {"fnCvtStoneKg",    fnCvtStoneKg   },
  {"fnCvtTonKg",      fnCvtTonKg     },
  {"fnCvtTorrPa",     fnCvtTorrPa    },
  {"fnCvtTrozG",      fnCvtTrozG     },
  {"fnCvtWhJ",        fnCvtWhJ       },
  {"fnCvtYardM",      fnCvtYardM     },
  {"fnCvtYearS",      fnCvtYearS     },
  {"",                NULL           }
};



char *endOfString(char *string) { // string must point on the 1st "
  string++;
  while(*string != '"' && *string != 0) {
    if(*string == '\\' && *(string + 1) == 'x') {
      string += 3;
    }
    else if(*string == '\\') {
      string++;
    }

    string++;
  }

  if(*string == '"') {
    string++;
  }
  else {
    processError("unterminated string");
  }

  return string; // pointer to the 1st char after the ending "
}



void strToShortInteger(char *nimBuffer, calcRegister_t regist) {
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Any change in this function must be reported in the function closeNim from file bufferize.c after the line: else if(nimNumberPart == NP_INT_BASE) {
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  longInteger_t minVal, value, maxVal;
  int16_t posHash, i, lg;
  int32_t base;

  lg = strlen(nimBuffer);
  posHash = 0;
  for(i=1; i<lg; i++) {
    if(nimBuffer[i] == '#') {
      posHash = i;
      break;
    }
  }

  for(i=posHash+1; i<lg; i++) {
    if(nimBuffer[i]<'0' || nimBuffer[i]>'9') {
      processError("error while initializing a short integer: there is a non numeric character in the base of the integer!");
    }
  }

  base = atoi(nimBuffer + posHash + 1);
  if(base < 2 || base > 16) {
    processError("error while initializing a short integer: the base of the integer must be from 2 to 16!");
  }

  for(i=nimBuffer[0] == '-' ? 1 : 0; i<posHash; i++) {
    if((nimBuffer[i] > '9' ? nimBuffer[i] - 'A' + 10 : nimBuffer[i] - '0') >= base) {
      sprintf(msgString, "error while initializing a short integer: digit %c is not allowed in base %d!\n", nimBuffer[i], base);
      processError(msgString);
    }
  }

  longIntegerInit(value);
  nimBuffer[posHash] = 0;
  stringToLongInteger(nimBuffer + (nimBuffer[0] == '+' ? 1 : 0), base, value);

  // maxVal = 2^shortIntegerWordSize
  longIntegerInit(maxVal);
  if(shortIntegerWordSize >= 1 && shortIntegerWordSize <= 64) {
    longInteger2Pow(shortIntegerWordSize, maxVal);
  }
  else {
    processError("error while initializing a short integer: shortIntegerWordSize must be fom 1 to 64");
  }

  // minVal = -maxVal/2
  longIntegerInit(minVal);
  longIntegerDivideUInt(maxVal, 2, minVal); // minVal = maxVal / 2
  longIntegerSetNegativeSign(minVal); // minVal = -minVal

  if((base != 2) && (base != 4) && (base != 8) && (base != 16) && (shortIntegerMode != SIM_UNSIGN)) {
    longIntegerDivideUInt(maxVal, 2, maxVal); // maxVal /= 2
  }

  longIntegerSubtractUInt(maxVal, 1, maxVal); // maxVal--

  if(shortIntegerMode == SIM_UNSIGN) {
    longIntegerSetZero(minVal); // minVal = 0
  }

  if(shortIntegerMode == SIM_1COMPL || shortIntegerMode == SIM_SIGNMT) {
    longIntegerAddUInt(minVal, 1, minVal); // minVal++
  }

  if(longIntegerCompare(value, minVal) < 0 || longIntegerCompare(value, maxVal) > 0) {
    char strMin[22], strMax[22];
    longIntegerToAllocatedString(minVal, strMin, sizeof(strMin));
    longIntegerToAllocatedString(maxVal, strMax, sizeof(strMax));
    sprintf(msgString, "error while initializing a short integer: for a word size of %d bit%s and integer mode %s, the entered number must be from %s to %s!", shortIntegerWordSize, shortIntegerWordSize>1 ? "s" : "", getShortIntegerModeName(shortIntegerMode), strMin, strMax);
    processError(msgString);
  }

  reallocateRegister(regist, dtShortInteger, SHORT_INTEGER_SIZE, base);

  char strValue[22];
  longIntegerToAllocatedString(value, strValue, sizeof(strValue));

  uint64_t val = strtoull(strValue + (longIntegerIsNegative(value) ? 1 : 0), NULL, 10); // when value is negative: discard the minus sign

  if(shortIntegerMode == SIM_UNSIGN) {
  }
  else if(shortIntegerMode == SIM_2COMPL) {
    if(longIntegerIsNegative(value)) {
      val = (~val + 1) & shortIntegerMask;
    }
  }
  else if(shortIntegerMode == SIM_1COMPL) {
    if(longIntegerIsNegative(value)) {
      val = ~val & shortIntegerMask;
    }
  }
  else if(shortIntegerMode == SIM_SIGNMT) {
    if(longIntegerIsNegative(value)) {
      val = (val & shortIntegerMask) | shortIntegerSignBit;
    }
  }
  else {
    processError("bad integer mode while initializing a short integer");
  }

  *(REGISTER_SHORT_INTEGER_DATA(regist)) = val;

  longIntegerFree(minVal);
  longIntegerFree(value);
  longIntegerFree(maxVal);
}



char hexToChar(const char *string) {
    // the itialisation to zero prevents a 'variable used is not initialized' warning on Mac:
    char ch=0;

  if(   (('0' <= string[0] && string[0] <= '9') || ('A' <= string[0] && string[0] <= 'F') || ('a' <= string[0] && string[0] <= 'f'))
     && (('0' <= string[1] && string[1] <= '9') || ('A' <= string[1] && string[1] <= 'F') || ('a' <= string[1] && string[1] <= 'f'))) {
    if('0' <= string[0] && string[0] <= '9') {
      ch = string[0] - '0';
    }
    else if('a' <= string[0] && string[0] <= 'f') {
      ch = string[0] - 'a' + 10;
    }
    else {
      ch = string[0] - 'A' + 10;
    }

    if('0' <= string[1] && string[1] <= '9') {
      ch = ch*16 + string[1] - '0';
    }
    else if('a' <= string[1] && string[1] <= 'f') {
      ch = ch*16 + string[1] - 'a' + 10;
    }
    else {
      ch = ch*16 + string[1] - 'A' + 10;
    }
  }
  else {
    sprintf(msgString, "missformed parameter setting. The hexadecimal char \\x%c%c is erroneous", string[0], string[1]);
    processError(msgString);
  }

  return ch;
}



void getString(char *str) {
  int32_t i, j, lg;

  lg = stringByteLength(str);

  str[lg - 1] = 0; // The ending "
  lg--;

  for(i=0; i<lg; i++) {
    if(str[i] == '\\' && (str[i + 1] == '\\' || str[i + 1] == '"')) {
      for(j=i+1; j<=lg; j++) {
        str[j - 1] = str[j];
      }
      lg--;
    }

    else if(str[i] == '\\' && str[i + 1] == 'x') {
      str[i] = hexToChar(str + i + 2);
      for(j=i+4; j<=lg; j++) {
        str[j - 3] = str[j];
      }
      lg -= 3;
    }
  }
}



void setParameter(char *p) {
  char l[200], r[200], real[200], imag[200], angMod[200]; //, letter;
  int32_t i;
  angularMode_t am = amDegree;

  //printf("  setting %s\n", p);

  i = 0;
  while(p[i] != '=' && p[i] != 0) {
    i++;
  }
  if(p[i] == 0) {
    processError("missformed parameter setting. Missing equal sign, remember that no space is allowed around the equal sign.");
  }

  p[i] = 0;
  strcpy(l, p);
  strcpy(r, p + i + 1);

  if(r[0] == 0) {
    processError("missformed parameter setting. Missing value after equal sign, remember that no space is allowed around the equal sign.");
  }

  //Setting a flag
  if(!strncmp(l, "FL_", 3)) {
    if(r[0] != '0' && r[0] != '1' && r[1] != 0) {
      processError("missformed flag setting. The rvalue must be 0 or 1");
    }

    //Lettered flag
    if(l[3] >= 'A' && l[4] == 0) {
      if(strstr("XYZTABCDLIJK", l + 3) != NULL) {
        uint16_t flg;

        flg = l[3] == 'T' ? 103 :
              l[3] == 'L' ? 108 :
              l[3] <= 'D' ? l[3] + 39 :
              l[3] <= 'K' ? l[3] + 36 :
                            l[3] + 12;

        if(r[0] == '1') {
          fnSetFlag(flg);
          //printf("  Flag %c set\n", l[1]);
        }
        else {
          fnClearFlag(flg);
          //printf("  Flag %c cleared\n", l[1]);
        }
      }
      else {
        processError("missformed flag setting. After FL_ there shall be a number from 0 to 111, a lettered, or a system flag.");
      }
    }

    //Numbered flag
    else if(   (l[3] >= '0' && l[3] <= '9' && l[4] == 0)
            || (l[3] >= '0' && l[3] <= '9' && l[4] >= '0' && l[4] <= '9' && l[5] == 0)
            || (l[3] >= '0' && l[3] <= '9' && l[4] >= '0' && l[4] <= '9' && l[5] >= '0' && l[5] <= '9' && l[6] == 0)) {
      uint16_t flg = atoi(l + 3);
      if(flg <= 111) {
        if(r[0] == '1') {
          fnSetFlag(flg);
          //printf("  Flag %d set\n", flg);
        }
        else {
          fnClearFlag(flg);
          //printf("  Flag %d cleared\n", flg);
        }
      }
      else {
        processError("missformed flag setting. After FL_ there shall be a number from 0 to 111, a lettered, or a system flag.");
      }
    }

    //System flag
    else {
      if(!strcmp(l+3, "SPCRES")) {
        if(r[0] == '0') {
          clearSystemFlag(FLAG_SPCRES);
        }
        else {
          setSystemFlag(FLAG_SPCRES);
        }
      }
      else if(!strcmp(l+3, "CPXRES")) {
        if(r[0] == '0') {
          clearSystemFlag(FLAG_CPXRES);
        }
        else {
          setSystemFlag(FLAG_CPXRES);
        }
      }
      else if(!strcmp(l+3, "CARRY")) {
        if(r[0] == '0') {
          clearSystemFlag(FLAG_CARRY);
        }
        else {
          setSystemFlag(FLAG_CARRY);
        }
      }
      else if(!strcmp(l+3, "OVERFL")) {
        if(r[0] == '0') {
          clearSystemFlag(FLAG_OVERFLOW);
        }
        else {
          setSystemFlag(FLAG_OVERFLOW);
        }
      }
      else if(!strcmp(l+3, "ASLIFT")) {
        if(r[0] == '0') {
          clearSystemFlag(FLAG_ASLIFT);
        }
        else {
          setSystemFlag(FLAG_ASLIFT);
        }
      }
      else if(!strcmp(l+3, "YMD")) {
        if(r[0] == '0') {
          clearSystemFlag(FLAG_YMD);
        }
        else {
          setSystemFlag(FLAG_YMD);
        }
      }
      else if(!strcmp(l+3, "MDY")) {
        if(r[0] == '0') {
          clearSystemFlag(FLAG_MDY);}
        else {
          setSystemFlag(FLAG_MDY);
        }
      }
      else if(!strcmp(l+3, "DMY")) {
        if(r[0] == '0') {
          clearSystemFlag(FLAG_DMY);
        }
        else {
          setSystemFlag(FLAG_DMY);
        }
      }
      else {
        processError("missformed numbered flag setting. After FL_ there shall be a number from 0 to 111, a lettered, or a system flag.");
      }
    }
  }
  //Setting integer mode
  else if(strcmp(l, "IM") == 0) {
    if(strcmp(r, "1COMPL") == 0) {
      shortIntegerMode = SIM_1COMPL;
      //printf("  Set integer mode to 1COMPL\n");
    }
    else if(strcmp(r, "2COMPL") == 0) {
      shortIntegerMode = SIM_2COMPL;
      //printf("  Set integer mode to 2COMPL\n");
    }
    else if(strcmp(r, "UNSIGN") == 0) {
      shortIntegerMode = SIM_UNSIGN;
      //printf("  Set integer mode to UNSIGN\n");
    }
    else if(strcmp(r, "SIGNMT") == 0) {
      shortIntegerMode = SIM_SIGNMT;
      //printf("  Set integer mode to SIGNMT\n");
    }
    else {
      processError("missformed integer mode setting. The rvalue must be 1COMPL, 2COMPL, UNSIGN or SIGNMT.");
    }
  }

  //Setting Complex mode
  else if(strcmp(l, "CM") == 0) {
    if(strcmp(r, "RECT") == 0) {
      clearSystemFlag(FLAG_POLAR);
      //printf("  Set complex mode to RECT\n");
    }
    else if(strcmp(r, "POLAR") == 0) {
      setSystemFlag(FLAG_POLAR);
      //printf("  Set complex mode to POLAR\n");
    }
    else {
      processError("missformed complex mode setting. The rvalue must be RECT or POLAR.");
    }
  }

  //Setting angular mode
  else if(strcmp(l, "AM") == 0) {
    if(strcmp(r, "DEG") == 0) {
      currentAngularMode = amDegree;
      //printf("  Set angular mode to DEG\n");
    }
    else if(strcmp(r, "DMS") == 0) {
      currentAngularMode = amDMS;
      //printf("  Set angular mode to DMS\n");
    }
    else if(strcmp(r, "RAD") == 0) {
      currentAngularMode = amRadian;
      //printf("  Set angular mode to RAD\n");
    }
    else if(strcmp(r, "MULTPI") == 0) {
      currentAngularMode = amMultPi;
      //printf("  Set angular mode to MULTPI\n");
    }
    else if(strcmp(r, "GRAD") == 0) {
      currentAngularMode = amGrad;
      //printf("  Set angular mode to GRAD\n");
    }
    else {
      processError("missformed angular mode setting. The rvalue must be DEG, DMS, GRAD, RAD or MULTPI.");
    }
  }

  //Setting stack size
  else if(strcmp(l, "SS") == 0) {
    if(strcmp(r, "4") == 0) {
      clearSystemFlag(FLAG_SSIZE8);
      //printf("  Set stack size to 4\n");
    }
    else if(strcmp(r, "8") == 0) {
      setSystemFlag(FLAG_SSIZE8);
      //printf("  Set stack size to 8\n");
    }
    else {
      processError("missformed stack size setting. The rvalue must be 4 or 8.");
    }
  }

  //Setting word size
  else if(strcmp(l, "WS") == 0) {
    if(   (r[0] >= '0' && r[0] <= '9' && r[1] == 0)
       || (r[0] >= '0' && r[0] <= '9' && r[1] >= '0' && r[1] <= '9' && r[2] == 0)) {
      uint16_t ws = atoi(r);

      if(ws == 0) {
        ws = 64;
      }
      if(ws <= 64) {
        fnSetWordSize(ws);
        //printf("  Set word size to %d bit\n", ws);
      }
      else {
        processError("missformed word size setting. The rvalue must be from 0 to 64 (0 is the same as 64).");
      }
    }
    else {
      processError("missformed word size setting. The rvalue must be a number from 0 to 64 (0 is the same as 64).");
    }
  }

  //Setting gap
  else if(strcmp(l, "GAP") == 0) {
    if(   (r[0] >= '0' && r[0] <= '9' && r[1] == 0)
       || (r[0] >= '0' && r[0] <= '9' && r[1] >= '0' && r[1] <= '9' && r[2] == 0)) {
      uint16_t gap = atoi(r);

      if(gap <= 15) {
        fnDisplayFormatGap(gap);
        //printf("  Set grouping gap to %d\n", gap);
      }
      else {
        processError("missformed grouping gap setting. The rvalue must be from 0 to 15.");
      }
    }
    else {
      processError("missformed grouping gap setting. The rvalue must be a number from 0 to 15.");
    }
  }

  //Setting J/G
  else if(strcmp(l, "JG") == 0) {
    if(                 (r[0] >= '0' && r[0] <= '9' &&
        ((r[1] == 0) || (r[1] >= '0' && r[1] <= '9' &&
        ((r[2] == 0) || (r[2] >= '0' && r[2] <= '9' &&
        ((r[3] == 0) || (r[3] >= '0' && r[3] <= '9' &&
        ((r[4] == 0) || (r[4] >= '0' && r[4] <= '9' &&
        ((r[5] == 0) || (r[5] >= '0' && r[5] <= '9' &&
        ((r[6] == 0) || (r[6] >= '0' && r[6] <= '9' &&
        ((r[7] == 0) || (r[7] >= '0' && r[7] <= '9' &&
        ((r[8] == 0) || (r[8] >= '0' && r[8] <= '9' &&
        ((r[9] == 0) ))))))))))))))))))) {
      firstGregorianDay = atoi(r);
    }
    else {
      processError("missformed J/G setting. The rvalue must be a number.");
    }
  }

  //Setting significant digits
  else if(strcmp(l, "SD") == 0) {
    if(   (r[0] >= '0' && r[0] <= '9' && r[1] == 0)
       || (r[0] >= '0' && r[0] <= '9' && r[1] >= '0' && r[1] <= '9' && r[2] == 0)) {
      uint16_t sd = atoi(r);

      if(sd <= 34) {
        significantDigits = sd;
        //printf("  Set significant digits to %d\n", sd);
      }
      else {
        processError("missformed significant digits setting. The rvalue must be from 0 to 34 (0 is the same as 34).");
      }
    }
    else {
      processError("missformed significant digits setting. The rvalue must be a number from 0 to 34 (0 is the same as 34).");
    }
  }

  //Setting rounding mode
  else if(strcmp(l, "RM") == 0) {
    if(r[0] >= '0' && r[0] <= '9' && r[1] == 0) {
      uint16_t rm = atoi(r);

      if(rm <= 6) {
        fnRoundingMode(rm);
        //printf("  Set rounding mode to %d (%s)\n", rm, getRoundingModeName(rm));
        //printf("  Set rounding mode to %d\n", rm);
      }
      else {
        processError("missformed rounding mode setting. The rvalue must be a number from 0 to 6.");
      }
    }
    else {
      processError("missformed rounding mode setting. The rvalue must be a number from 0 to 6.");
    }
  }

  //Setting a register
  else if(l[0] == 'R') {
    calcRegister_t regist = 0;

    //Lettered register
    if(l[1] >= 'A' && l[2] == 0) {
      if(strstr("XYZTABCDLIJK", l + 1) != NULL) {
        //letter = l[1];
        regist = l[1] == 'T' ? 103 :
                 l[1] == 'L' ? 108 :
                 l[1] <= 'D' ? l[1] + 39 :
                 l[1] <= 'K' ? l[1] + 36 :
                               l[1] + 12;
      }
      else {
        processError("missformed lettered register setting. The letter after R is not a lettered register.");
      }
    }

    //Numbered register
    else if(   (l[1] >= '0' && l[1] <= '9' && l[2] == 0)
            || (l[1] >= '0' && l[1] <= '9' && l[2] >= '0' && l[2] <= '9' && l[3] == 0)
            || (l[1] >= '0' && l[1] <= '9' && l[2] >= '0' && l[2] <= '9' && l[3] >= '0' && l[3] <= '9' && l[4] == 0)) {
      regist = atoi(l + 1);
      if(regist > 111 || regist < 0) {
        processError("missformed numbered register setting. Th number after R shall be a number from 0 to 111.");
      }
      //letter = 0;
    }

    else {
      processError("missformed register setting. After R there shall be a number from 0 to 111 or a lettered register.");
    }

    // find the : separating the data type and the value
    i = 0;
    while(r[i] != ':' && r[i] != 0) {
      i++;
    }
    if(r[i] == 0) {
      processError("missformed register value. Missing colon between data type and value.");
    }

    // separating the data type and the value
    r[i] = 0;
    strcpy(l, r);
    xcopy(r, r + i + 1, strlen(r + i + 1) + 1);

    if(strcmp(l, "LONI") == 0) {
      longInteger_t lgInt;

      // remove beginning and ending " and removing leading spaces
      xcopy(r, r + 1, strlen(r));
      while(r[0] == ' ') {
        xcopy(r, r + 1, strlen(r));
      }
      r[strlen(r) - 1] = 0;

      longIntegerInit(lgInt);
      stringToLongInteger(r, 10, lgInt);
      convertLongIntegerToLongIntegerRegister(lgInt, regist);
      longIntegerFree(lgInt);
    }
    else if(strcmp(l, "REAL") == 0) {
      // find the : separating the real value from the angular mode
      i = 0;
      while(r[i] != ':' && r[i] != 0) {
        i++;
      }
      if(r[i] == 0) {
        strcat(r, ":NONE");
      }

      // separate real value and angular mode
      r[i] = 0;
      strcpy(angMod, r + i + 1);

      if(strcmp(angMod, "DEG"   ) == 0) {
        am = amDegree;
      }
      else if(strcmp(angMod, "DMS"   ) == 0) {
        am = amDMS;
      }
      else if(strcmp(angMod, "RAD"   ) == 0) {
        am = amRadian;
      }
      else if(strcmp(angMod, "MULTPI") == 0) {
        am = amMultPi;
      }
      else if(strcmp(angMod, "GRAD"  ) == 0) {
        am = amGrad;
      }
      else if(strcmp(angMod, "NONE"  ) == 0) {
        am = amNone;
      }
      else {
        sprintf(msgString, "missformed register real%d angular mode. Unknown angular mode after real value", strcmp(l, "RE16") == 0 ? 16 : 34);
        processError(msgString);
      }

      // remove beginning and ending " and removing leading spaces
      xcopy(r, r + 1, strlen(r));
      while(r[0] == ' ') {
        xcopy(r, r + 1, strlen(r));
      }
      r[strlen(r) - 1] = 0;

      // replace , with .
      for(i=0; i<(int)strlen(r); i++) {
        if(r[i] == ',') {
          r[i] = '.';
        }
      }

      reallocateRegister(regist, dtReal34, REAL34_SIZE, am);
      stringToReal34(r, REGISTER_REAL34_DATA(regist));
    }
    else if(strcmp(l, "STRI") == 0) {
      getString(r + 1);
      reallocateRegister(regist, dtString, TO_BLOCKS(stringByteLength(r + 1) + 1), amNone);
      strcpy(REGISTER_STRING_DATA(regist), r + 1);
    }
    else if(strcmp(l, "SHOI") == 0) {
      // find the # separating the value from the base
      i = 0;
      while(r[i] != '#' && r[i] != 0) {
        i++;
      }
      if(r[i] == 0) {
        processError("missformed register short integer value. Missing # between value and base.");
      }

      // remove beginning and ending " and removing leading spaces
      xcopy(r, r + 1, strlen(r));
      while(r[0] == ' ') {
        xcopy(r, r + 1, strlen(r));
      }
      r[strlen(r) - 1] = 0;

      // Convert string to upper case
      for(i=0; r[i]!=0; i++) {
        if('a' <= r[i] && r[i] <= 'z') {
          r[i] -= 32;
        }
      }

      strToShortInteger(r, regist);
    }
    else if(strcmp(l, "CPLX") == 0) {
      // remove beginning and ending " and removing leading spaces
      xcopy(r, r + 1, strlen(r));
      while(r[0] == ' ') {
        xcopy(r, r + 1, strlen(r));
      }
      r[strlen(r) - 1] = 0;

      // find the i separating the real and imagynary part
      i = 0;
      while(r[i] != 'i' && r[i] != 0) {
        i++;
      }
      if(r[i] == 0) {
        processError("missformed register complex34 value. Missing i between real and imaginary part.");
      }

      // separate real and imaginary part
      r[i] = 0;
      strcpy(real, r);
      strcpy(imag, r + i + 1);

      // remove leading spaces
      while(imag[0] == ' ') {
        xcopy(imag, imag + 1, strlen(imag));
      }

      // removing trailing spaces from real part
      while(real[strlen(real) - 1] == ' ') {
        real[strlen(real) - 1] = 0;
      }

      // removing trailing spaces from imaginary part
      while(imag[strlen(imag) - 1] == ' ') {
        imag[strlen(imag) - 1] = 0;
      }

      // replace , with . in the real part
      for(i=0; i<(int)strlen(real); i++) {
        if(real[i] == ',') {
          real[i] = '.';
        }
      }

      // replace , with . in the imaginary part
      for(i=0; i<(int)strlen(imag); i++) {
        if(imag[i] == ',') {
          imag[i] = '.';
        }
      }

      reallocateRegister(regist, dtComplex34, COMPLEX34_SIZE, amNone);
      stringToReal34(real, REGISTER_REAL34_DATA(regist));
      stringToReal34(imag, REGISTER_IMAG34_DATA(regist));
    }
    else if(strcmp(l, "TIME") == 0) {
      int32_t k = 0;
      bool isHms = false;

      // find the : separating hours and minutes
      i = 0;
      while(r[i] != ':' && r[i] != 0) {
        i++;
      }
      if(r[i] == ':') { // Input by HMS
        isHms = true;
        k = i;
        r[i] = '.';
        do {
          ++k;
          if((r[k] != ':') && (r[k] != '.') && (r[k] != ',')) {
            r[++i] = r[k];
          }
        } while(r[k] != 0);
      }
      am = amNone;

      // remove beginning and ending " and removing leading spaces
      xcopy(r, r + 1, strlen(r));
      while(r[0] == ' ') {
        xcopy(r, r + 1, strlen(r));
      }
      r[strlen(r) - 1] = 0;

      // replace , with .
      for(i=0; i<(int)strlen(r); i++) {
        if(r[i] == ',') {
          r[i] = '.';
        }
      }

      reallocateRegister(regist, dtTime, REAL34_SIZE, amNone);
      stringToReal34(r, REGISTER_REAL34_DATA(regist));
      if(isHms) {
        hmmssInRegisterToSeconds(regist);
      }
    }
    else if(strcmp(l, "DATE") == 0) {
      // remove beginning and ending " and removing leading spaces
      xcopy(r, r + 1, strlen(r));
      while(r[0] == ' ') {
        xcopy(r, r + 1, strlen(r));
      }
      r[strlen(r) - 1] = 0;

      // replace , with .
      for(i=0; i<(int)strlen(r); i++) {
        if(r[i] == ',') {
          r[i] = '.';
        }
      }

      reallocateRegister(regist, dtReal34, REAL34_SIZE, amNone);
      stringToReal34(r, REGISTER_REAL34_DATA(regist));
      convertReal34RegisterToDateRegister(regist, regist);
    }
    else {
      sprintf(msgString, "missformed register value. Unknown data type %s for register %s", l, p+1);
      processError(msgString);
    }

    //if(letter == 0) {
    //  printf("  R%d = ", regist);
    //}
    //else {
    //  printf("  R%c = ", letter);
    //}

    //printRegisterToConsole(regist, 0);
    //printf("\n");
  }

  else {
    sprintf(msgString, "unknown setting %s", l);
    processError(msgString);
  }
}



void inParameters(char *token) {
  char parameter[2000];
  int32_t lg;

  strReplace(token, "inf", "9e9999");

  while(*token == ' ') {
    token++;
  }
  while(*token != 0) {
    int32_t index = 0;
    while(*token != ' ' && *token != 0) {
      if(*token == '"') { // Inside a string
        lg = endOfString(token) - token;
        strncpy(parameter + index, token, lg--);
        index += lg;
        token += lg;
      }
      parameter[index++] = *(token++);
    }
    parameter[index] = 0;

    setParameter(parameter);

    while(*token == ' ') {
      token++;
    }
  }
}



void checkRegisterType(calcRegister_t regist, char letter, uint32_t expectedDataType, uint32_t expectedTag) {
  if(getRegisterDataType(regist) != expectedDataType) {
    if(letter == 0) {
      printf("\nRegister %d should be %s but it is %s!\n", regist, getDataTypeName(expectedDataType, true, false), getDataTypeName(getRegisterDataType(regist), true, false));
      printf("R%d = ", regist);
    }
    else {
      printf("\nRegister %c should be %s but it is %s!\n", letter, getDataTypeName(expectedDataType, true, false), getDataTypeName(getRegisterDataType(regist), true, false));
      printf("R%c = ", letter);
    }
    printRegisterToConsole(regist, "", "\n");
    processError("wrong register type");
  }

  if(getRegisterTag(regist) != expectedTag) {
    if(getRegisterDataType(regist) == dtShortInteger) {
      if(letter == 0) {
        printf("\nRegister %d is a short integer base %u but it should be base %u!\n", regist, expectedTag, getRegisterShortIntegerBase(regist));
        printf("R%d = ", regist);
      }
      else {
        printf("\nRegister %c is a short integer base %u but it should be base %u!\n", letter, expectedTag, getRegisterShortIntegerBase(regist));
        printf("R%c = ", letter);
      }
      printRegisterToConsole(regist, "", "\n");
      processError("wrong register type");
    }
    else if(getRegisterDataType(regist) == dtReal34) {
      if(letter == 0) {
        printf("\nRegister %d should be a real tagged %s but it is tagged %s!\n", regist, getAngularModeName(expectedTag), getAngularModeName(getRegisterAngularMode(regist)));
        printf("R%d = ", regist);
      }
      else {
        printf("\nRegister %c should be a real tagged %s but it is tagged %s!\n", letter, getAngularModeName(expectedTag), getAngularModeName(getRegisterAngularMode(regist)));
        printf("R%c = ", letter);
      }
      printRegisterToConsole(regist, "", "\n");
      processError("wrong register type");
    }
    else if(getRegisterDataType(regist) == dtLongInteger) {
      if(letter == 0) {
        printf("\nRegister %d should be a long integer tagged %u but it is tagged %u!\n", regist, expectedTag, getRegisterLongIntegerSign(regist));
        printf("R%d = ", regist);
      }
      else {
        printf("\nRegister %c should be a long integer tagged %u but it is tagged %u!\n", letter, expectedTag, getRegisterLongIntegerSign(regist));
        printf("R%c = ", letter);
      }
      printRegisterToConsole(regist, "", "\n");
      processError("wrong register type");
    }
  }
}



int relativeErrorReal34(real34_t *expectedValue34, real34_t *value34, char *numberPart, calcRegister_t regist, char letter) {
  real_t expectedValue, value, relativeError;

  real34ToReal(expectedValue34, &expectedValue);
  real34ToReal(value34, &value);

  realSubtract(&expectedValue, &value, &relativeError, &ctxtReal39);

  if(!realIsZero(&expectedValue)) {
    realDivide(&relativeError, &expectedValue, &relativeError, &ctxtReal39);
  }
  else {
    realCopy(&value, &relativeError);
  }
  realSetPositiveSign(&relativeError);

  correctSignificantDigits = -relativeError.exponent - relativeError.digits;
  ctxtReal39.digits = 2;
  realPlus(&relativeError, &relativeError, &ctxtReal39);
  ctxtReal39.digits = 39;
  if(correctSignificantDigits <= 34) {
    //printf("\nThere are only %d correct significant digits in the %s part of the value: %d are expected!\n", correctSignificantDigits, numberPart, NUMBER_OF_CORRECT_SIGNIFICANT_DIGITS_EXPECTED);
    realToString(&relativeError, realString);
    if(letter == 0) {
      printf("\nThere are only %d correct significant digits in the %s part of register %d! Relative error is %s\n", correctSignificantDigits, numberPart, regist, realString);
      printf("R%d = ", regist);
      printReal34ToConsole(value34, "", "\n");
    }
    else {
      printf("\nThere are only %d correct significant digits in the %s part of register %c! Relative error is %s\n", correctSignificantDigits, numberPart, letter, realString);
      printf("%c = ", letter);
      printReal34ToConsole(value34, "", "\n");
    }
    printf("%s\n", lastInParameters);
    printf("%s\n", line);
    printf("in file %s line %d\n", fileName, lineNumber);
    if(correctSignificantDigits < 32 && correctSignificantDigits < NUMBER_OF_CORRECT_SIGNIFICANT_DIGITS_EXPECTED) {
      puts(registerExpectedAndValue);
      //exit(-1);
    }
  }

  return (correctSignificantDigits < 32 && correctSignificantDigits < NUMBER_OF_CORRECT_SIGNIFICANT_DIGITS_EXPECTED) ? RE_INACCURATE : RE_ACCURATE;
}



void wrongRegisterValue(calcRegister_t regist, char letter, char *expectedValue) {
  if(letter == 0) {
    printf("\nRegister %d value should be ", regist);
  }
  else {
    printf("\nRegister %c value should be ", letter);
  }
  printf("%s\nbut it is ", expectedValue);
  printRegisterToConsole(regist, "", "\n");
  processError("wrong register value");
}



void expectedAndShouldBeValue(calcRegister_t regist, char letter, char *expectedValue, char *expectedAndValue) {
  char str[300];

  if(letter == 0) {
    sprintf(expectedAndValue, "\nRegister %d value should be ", regist);
  }
  else {
    sprintf(expectedAndValue, "\nRegister %c value should be ", letter);
  }
  strcat(expectedAndValue, expectedValue);
  strcat(expectedAndValue, "\nbut it is ");
  printRegisterToString(regist, str);
  strcat(expectedAndValue, str);
  strcat(expectedAndValue, "\n");
}



bool real34AreEqual(real34_t *a, real34_t *b) {
  if( real34IsNaN(a) &&  real34IsNaN(b)) {
    return true;
  }
  if( real34IsNaN(a) && !real34IsNaN(b)) {
    return false;
  }
  if(!real34IsNaN(a) &&  real34IsNaN(b)) {
    return false;
  }

  if( real34IsInfinite(a) && !real34IsInfinite(b)) {
    return false;
  }
  if(!real34IsInfinite(a) &&  real34IsInfinite(b)) {
    return false;
  }
  if( real34IsInfinite(a) &&  real34IsInfinite(b)) {
    if(real34IsPositive(a) && real34IsPositive(b)) {
      return true;
    }
    if(real34IsNegative(a) && real34IsNegative(b)) {
      return true;
    }
    return false;
  }

  return real34CompareEqual(a, b);
}



void checkExpectedOutParameter(char *p) {
  char l[2000], r[2000], real[200], imag[200], angMod[200], letter = 0;
  int32_t i;
  angularMode_t am = amDegree;
  real34_t expectedReal34, expectedImag34;

  //printf("  Checking %s\n", p);

  i = 0;
  while(p[i] != '=' && p[i] != 0) {
    i++;
  }
  if(p[i] == 0) {
    processError("missformed out parameter. Missing equal sign, remember that no space is allowed around the equal sign.");
  }

  p[i] = 0;
  strcpy(l, p);
  strcpy(r, p + i + 1);

  if(r[0] == 0) {
    processError("missformed out parameter. Missing value after equal sign, remember that no space is allowed around the equal sign.");
  }

  //Checking a flag
  if(!strncmp(l, "FL_", 3)) {
    if(r[0] != '0' && r[0] != '1' && r[1] != 0) {
      processError("missformed flag checking. The rvalue must be 0 or 1.");
    }

    //Lettered flag
    if(l[3] >= 'A' && l[4] == 0) {
      if(strstr("XYZTABCDLIJK", l + 3) != NULL) {
        uint16_t flg;

        flg = l[3] == 'T' ? 103 :
              l[3] == 'L' ? 108 :
              l[3] <= 'D' ? l[3] + 39 :
              l[3] <= 'K' ? l[3] + 36 :
                            l[3] + 12;

        if(r[0] == '1') {
          if(!getFlag(flg)) {
            sprintf(msgString, "flag %c should be set but it is clear!", l[1]);
            processError(msgString);
          }
        }
        else {
          if(getFlag(flg)) {
            sprintf(msgString, "flag %c should be clear but it is set!", l[1]);
            processError(msgString);
          }
        }
      }
      else {
        processError("missformed flag checking. After FL_ there shall be a number from 0 to 111, a lettered, or a system flag.");
      }
    }

    //Numbered flag
    else if(   (l[3] >= '0' && l[3] <= '9' && l[4] == 0)
            || (l[3] >= '0' && l[3] <= '9' && l[4] >= '0' && l[4] <= '9' && l[5] == 0)
            || (l[3] >= '0' && l[3] <= '9' && l[4] >= '0' && l[4] <= '9' && l[5] >= '0' && l[5] <= '9' && l[6] == 0)) {
      uint16_t flg = atoi(l + 3);
      if(flg <= 111) {
        if(r[0] == '1' && !getFlag(flg)) {
          sprintf(msgString, "flag %d should be set but it is clear!", flg);
          processError(msgString);
        }
        else if(r[0] == '0' && getFlag(flg)) {
          sprintf(msgString, "flag %d should be clear but it is set!", flg);
          processError(msgString);
        }
      }
      else {
        processError("missformed flag checking in line. After FL_ there shall be a number from 0 to 111, a lettered, or a system flag.");
      }
    }

    //System flag
    else {
      if(!strcmp(l+3, "SPCRES")) {
        if(r[0] == '1' && !getSystemFlag(FLAG_SPCRES)) {
          processError("system flag SPCRES should be set but it is clear!");
        }
        else if(r[0] == '0' && getSystemFlag(FLAG_SPCRES)) {
          processError("system flag SPCRES should be clear but it is set!");
        }
      }
      else if(!strcmp(l+3, "CPXRES")) {
        if(r[0] == '1' && !getSystemFlag(FLAG_CPXRES)) {
          processError("system flag CPXRES should be set but it is clear!");
        }
        else if(r[0] == '0' && getSystemFlag(FLAG_CPXRES)) {
          processError("system flag CPXRES should be clear but it is set!");
        }
      }
      else if(!strcmp(l+3, "CARRY")) {
        if(r[0] == '1' && !getSystemFlag(FLAG_CARRY)) {
          processError("system flag CARRY should be set but it is clear!");
        }
        else if(r[0] == '0' && getSystemFlag(FLAG_CARRY)) {
          processError("system flag CARRY should be clear but it is set!");
        }
      }
      else if(!strcmp(l+3, "OVERFL")) {
        if(r[0] == '1' && !getSystemFlag(FLAG_OVERFLOW)) {
          processError("system flag OVERFL should be set but it is clear!");
        }
        else if(r[0] == '0' && getSystemFlag(FLAG_OVERFLOW)) {
          processError("system flag OVERFL should be clear but it is set!");
        }
      }
      else if(!strcmp(l+3, "ASLIFT")) {
        if(r[0] == '1' && !getSystemFlag(FLAG_ASLIFT)) {
          processError("system flag ASLIFT should be set but it is clear!");
        }
        else if(r[0] == '0' && getSystemFlag(FLAG_ASLIFT)) {
          processError("system flag ASLIFT should be clear but it is set!");
        }
      }
      else if(!strcmp(l+3, "YMD")) {
        if(r[0] == '1' && !getSystemFlag(FLAG_YMD)) {
          processError("system flag YMD should be set but it is clear!");
        }
        else if(r[0] == '0' && getSystemFlag(FLAG_YMD)) {
          processError("system flag YMD should be clear but it is set!");
        }
      }
      else if(!strcmp(l+3, "MDY")) {
        if(r[0] == '1' && !getSystemFlag(FLAG_MDY)) {
          processError("system flag MDY should be set but it is clear!");
        }
        else if(r[0] == '0' && getSystemFlag(FLAG_MDY)) {
          processError("system flag MDY should be clear but it is set!");
        }
      }
      else if(!strcmp(l+3, "DMY")) {
        if(r[0] == '1' && !getSystemFlag(FLAG_DMY)) {
          processError("system flag DMY should be set but it is clear!");
        }
        else if(r[0] == '0' && getSystemFlag(FLAG_DMY)) {
          processError("system flag DMY should be clear but it is set!");
        }
      }
      else {
        processError("missformed numbered flag checking. After FL_ there shall be a number from 0 to 111, a lettered, or a system flag.");
      }
    }
  }

  //Checking integer mode
  else if(strcmp(l, "IM") == 0) {
    if(strcmp(r, "1COMPL") == 0) {
      if(shortIntegerMode != SIM_1COMPL) {
        processError("integer mode should be 1COMPL but it is not!");
      }
    }
    else if(strcmp(r, "2COMPL") == 0) {
      if(shortIntegerMode != SIM_2COMPL) {
        processError("integer mode should be 2COMPL but it is not!");
      }
    }
    else if(strcmp(r, "UNSIGN") == 0) {
      if(shortIntegerMode != SIM_UNSIGN) {
        processError("integer mode should be UNSIGN but it is not!");
      }
    }
    else if(strcmp(r, "SIGNMT") == 0) {
      if(shortIntegerMode != SIM_SIGNMT) {
        processError("integer mode should be SIGNMT but it is not!");
      }
    }
    else {
      processError("missformed integer mode checking. The rvalue must be 1COMPL, 2COMPL, UNSIGN or SIGNMT.");
    }
  }

  //Checking complex mode
  else if(strcmp(l, "CM") == 0) {
    if(strcmp(r, "RECT") == 0) {
      if(getSystemFlag(FLAG_POLAR)) {
        processError("Complex mode should be RECT but it is not!");
      }
    }
    else if(strcmp(r, "POLAR") == 0) {
      if(!getSystemFlag(FLAG_POLAR)) {
        processError("Complex mode should be POLAR but it is not!");
      }
    }
    else {
      processError("missformed complex mode checking. The rvalue must be RECT or POLAR.");
    }
  }

  //Checking angular mode
  else if(strcmp(l, "AM") == 0) {
    if(strcmp(r, "DEG") == 0) {
      if(currentAngularMode != amDegree) {
        processError("angular mode should be DEGREE but it is not!");
      }
    }
    else if(strcmp(r, "DMS") == 0) {
      if(currentAngularMode != amDMS) {
        processError("angular mode should be DMS but it is not!");
      }
    }
    else if(strcmp(r, "RAD") == 0) {
      if(currentAngularMode != amRadian) {
        processError("angular mode should be RAD but it is not!");
      }
    }
    else if(strcmp(r, "MULTPI") == 0) {
      if(currentAngularMode != amMultPi) {
        processError("angular mode should be MULTPI but it is not!");
      }
    }
    else if(strcmp(r, "GRAD") == 0) {
      if(currentAngularMode != amGrad) {
        processError("angular mode should be GRAD but it is not!");
      }
    }
    else {
      processError("missformed angular mode checking. The rvalue must be DEG, DMS, GRAD, RAD or MULTPI.");
    }
  }

  //Checking stack size
  else if(strcmp(l, "SS") == 0) {
    if(strcmp(r, "4") == 0) {
      if(getSystemFlag(FLAG_SSIZE8)) {
        processError("stack size should be 4 but it is not!");
      }
    }
    else if(strcmp(r, "8") == 0) {
      if(!getSystemFlag(FLAG_SSIZE8)) {
        processError("stack size should be 8 but it is not!");
      }
    }
    else {
      processError("missformed stack size checking. The rvalue must be 4 or 8.");
    }
  }

  //Checking word size
  else if(strcmp(l, "WS") == 0) {
    if(   (r[0] >= '0' && r[0] <= '9' && r[1] == 0)
       || (r[0] >= '0' && r[0] <= '9' && r[1] >= '0' && r[1] <= '9' && r[2] == 0)) {
      uint16_t ws = atoi(r);

      if(ws == 0) {
        ws = 64;
      }
      if(ws <= 64) {
        if(shortIntegerWordSize != ws) {
          sprintf(msgString, "short integer word size should be %u but it is %u!", ws, shortIntegerWordSize);
          processError(msgString);
        }
      }
      else {
        processError("missformed word size checking. The rvalue must be from 0 to 64 (0 is the same as 64).");
      }
    }
    else {
      processError("missformed word size checking. The rvalue must be a number from 0 to 64 (0 is the same as 64).");
    }
  }

  //Checking J/G
  else if(strcmp(l, "JG") == 0) {
    if(                 (r[0] >= '0' && r[0] <= '9' &&
        ((r[1] == 0) || (r[1] >= '0' && r[1] <= '9' &&
        ((r[2] == 0) || (r[2] >= '0' && r[2] <= '9' &&
        ((r[3] == 0) || (r[3] >= '0' && r[3] <= '9' &&
        ((r[4] == 0) || (r[4] >= '0' && r[4] <= '9' &&
        ((r[5] == 0) || (r[5] >= '0' && r[5] <= '9' &&
        ((r[6] == 0) || (r[6] >= '0' && r[6] <= '9' &&
        ((r[7] == 0) || (r[7] >= '0' && r[7] <= '9' &&
        ((r[8] == 0) || (r[8] >= '0' && r[8] <= '9' &&
        ((r[9] == 0) ))))))))))))))))))) {
      uint32_t jg = atoi(r);
      if(firstGregorianDay != jg) {
        sprintf(msgString, "j/G should be %u but it is %u!", jg, firstGregorianDay);
        processError(msgString);
      }
      firstGregorianDay = atoi(r);
    }
    else {
      processError("missformed J/G setting. The rvalue must be a number.");
    }
  }

  //Checking significant digits
  else if(strcmp(l, "SD") == 0) {
    if(   (r[0] >= '0' && r[0] <= '9' && r[1] == 0)
       || (r[0] >= '0' && r[0] <= '9' && r[1] >= '0' && r[1] <= '9' && r[2] == 0)) {
      uint16_t sd = atoi(r);

      if(sd <= 34) {
        if(significantDigits != sd) {
          sprintf(msgString, "number of significant digits should be %u but it is %u!", sd, significantDigits);
          processError(msgString);
        }
      }
      else {
        processError("missformed significant digits checking. The rvalue must be from 0 to 34 (0 is the same as 34).");
      }
    }
    else {
      processError("missformed significant digits checking. The rvalue must be a number from 0 to 34 (0 is the same as 34).");
    }
  }

  //Checking rounding mode
  else if(strcmp(l, "RM") == 0) {
    if(r[0] >= '0' && r[0] <= '9' && r[1] == 0) {
      uint16_t rm = atoi(r);

      if(rm <= 6) {
        if(roundingMode != rm) {
          sprintf(msgString, "rounding mode should be %u but it is %u!", rm, roundingMode);
          processError(msgString);
        }
      }
      else {
        processError("missformed rounding mode checking. The rvalue must be a number from 0 to 6.");
      }
    }
    else {
      processError("missformed rounding mode checking. The rvalue must be a number from 0 to 6.");
    }
  }

  //Checking error code
  else if(strcmp(l, "EC") == 0) {
    if(   (r[0] >= '0' && r[0] <= '9' && r[1] == 0)
       || (r[0] >= '0' && r[0] <= '9' && r[1] >= '0' && r[1] <= '9' && r[2] == 0)) {
      uint16_t ec = atoi(r);

      if(ec <= 28) {
        if(lastErrorCode != ec) {
          sprintf(msgString, "last error code should be %d (%s) but it is %d (%s)!", ec, errorMessages[ec], lastErrorCode, errorMessages[lastErrorCode]);
          processError(msgString);
        }
      }
      else {
        processError("missformed error code checking. The rvalue must be a number from 0 to 28.");
      }
    }
    else {
      processError("missformed error code checking. The rvalue must be a number from 0 to 28.");
    }
  }

  //Checking a register
  else if(l[0] == 'R') {
    calcRegister_t regist = 0;

    //Lettered register
    if(l[1] >= 'A' && l[2] == 0) {
      if(strstr("XYZTABCDLIJK", l + 1) != NULL) {
        letter = l[1];
        regist = l[1] == 'T' ? 103 :
                 l[1] == 'L' ? 108 :
                 l[1] <= 'D' ? l[1] + 39 :
                 l[1] <= 'K' ? l[1] + 36 :
                               l[1] + 12;
      }
      else {
        processError("missformed lettered register checking. The letter after R is not a lettered register.");
      }
    }

    //Numbered register
    else if(   (l[1] >= '0' && l[1] <= '9' && l[2] == 0)
            || (l[1] >= '0' && l[1] <= '9' && l[2] >= '0' && l[2] <= '9' && l[3] == 0)
            || (l[1] >= '0' && l[1] <= '9' && l[2] >= '0' && l[2] <= '9' && l[3] >= '0' && l[3] <= '9' && l[4] == 0)) {
      regist = atoi(l + 1);
      if(regist > 111 || regist < 0) {
        processError("missformed numbered register checking. The number after R shall be a number from 0 to 111.");
      }
      letter = 0;
    }

    else {
      processError("missformed register checking. After R there shall be a number from 0 to 111 or a lettered register.");
    }

    // find the : separating the data type and the value
    i = 0;
    while(r[i] != ':' && r[i] != 0) {
      i++;
    }
    if(r[i] == 0) {
      processError("missformed register value. Missing colon between data type and value.");
    }

    // separating the data type and the value
    r[i] = 0;
    strcpy(l, r);
    xcopy(r, r + i + 1, strlen(r + i + 1) + 1);

    if(strcmp(l, "LONI") == 0) {
      longInteger_t expectedLongInteger, registerLongInteger;

      // remove beginning and ending " and removing leading spaces
      xcopy(r, r + 1, strlen(r));
      while(r[0] == ' ') {
        xcopy(r, r + 1, strlen(r));
      }
      r[strlen(r) - 1] = 0;

      longIntegerInit(expectedLongInteger);
      stringToLongInteger(r, 10, expectedLongInteger);
      checkRegisterType(regist, letter, dtLongInteger, longIntegerSignTag(expectedLongInteger));
      convertLongIntegerRegisterToLongInteger(regist, registerLongInteger);
      if(longIntegerCompare(expectedLongInteger, registerLongInteger) != 0) {
        wrongRegisterValue(regist, letter, r);
      }

      longIntegerFree(expectedLongInteger);
      longIntegerFree(registerLongInteger);
    }
    else if(strcmp(l, "REAL") == 0) {
      // find the : separating the real value from the angular mode
      i = 0;
      while(r[i] != ':' && r[i] != 0) {
        i++;
      }
      if(r[i] == 0) {
        strcat(r, ":NONE");
      }

      // separate real value and angular mode
      r[i] = 0;
      strcpy(angMod, r + i + 1);

           if(strcmp(angMod, "DEG"   ) == 0) am = amDegree;
      else if(strcmp(angMod, "DMS"   ) == 0) am = amDMS;
      else if(strcmp(angMod, "RAD"   ) == 0) am = amRadian;
      else if(strcmp(angMod, "MULTPI") == 0) am = amMultPi;
      else if(strcmp(angMod, "GRAD"  ) == 0) am = amGrad;
      else if(strcmp(angMod, "NONE"  ) == 0) am = amNone;
      else {
        sprintf(msgString, "missformed register real%d angular mode. Unknown angular mode after real value.", strcmp(l, "RE16") == 0 ? 16 : 34);
        processError(msgString);
      }


      // remove beginning and ending " and removing leading spaces
      xcopy(r, r + 1, strlen(r));
      while(r[0] == ' ') {
        xcopy(r, r + 1, strlen(r));
      }
      r[strlen(r) - 1] = 0;

      // replace , with .
      for(i=0; i<(int)strlen(r); i++) {
        if(r[i] == ',') {
          r[i] = '.';
        }
      }

      checkRegisterType(regist, letter, dtReal34, am);
      stringToReal34(r, &expectedReal34);
      if(!real34AreEqual(REGISTER_REAL34_DATA(regist), &expectedReal34)) {
        expectedAndShouldBeValue(regist, letter, r, registerExpectedAndValue);
        if(relativeErrorReal34(&expectedReal34, REGISTER_REAL34_DATA(regist), "real", regist, letter) == RE_INACCURATE) {
          wrongRegisterValue(regist, letter, r);
        }
      }
    }
    else if(strcmp(l, "STRI") == 0) {
      checkRegisterType(regist, letter, dtString, amNone);
      getString(r + 1);

      char *expected, *is;
      if(stringByteLength(r + 1) != stringByteLength(REGISTER_STRING_DATA(regist))) {
        char stringUtf8[200];
        stringToUtf8(REGISTER_STRING_DATA(regist), (uint8_t *)stringUtf8);
        printf("\nThe 2 strings are not of the same size.\nRegister string: %s\n", stringUtf8);
        for(i=0, is=REGISTER_STRING_DATA(regist); i<=stringByteLength(REGISTER_STRING_DATA(regist)); i++, is++) {
          printf("%02x ", (unsigned char)*is);
        }
        stringToUtf8(r+1, (uint8_t *)stringUtf8);
        printf("\nExpected string: %s\n", stringUtf8);
        for(i=1; i<=stringByteLength(r); i++) {
          printf("%02x ", (unsigned char)r[i]);
        }
        printf("\n");
        processError("strings not the same size");
      }

      for(i=stringByteLength(r + 1), expected=r + 1, is=REGISTER_STRING_DATA(regist); i>0; i--, expected++, is++) {
        //printf("%c %02x   %c %02x\n", *expected, (unsigned char)*expected, *is, (unsigned char)*is);
        if(*expected != *is) {
          printf("\nThe 2 strings are different.\nRegister string: ");
          for(i=0, is=REGISTER_STRING_DATA(regist); i<=stringByteLength(REGISTER_STRING_DATA(regist)); i++, is++) {
            printf("%02x ", (unsigned char)*is);
          }
          printf("\nExpected string: ");
          for(i=1; i<=stringByteLength(r); i++) {
            printf("%02x ", (unsigned char)r[i]);
          }
          printf("\n");
          processError("strings different");
        }
      }
    }
    else if(strcmp(l, "SHOI") == 0) {
      // find the # separating the value from the base
      i = 0;
      while(r[i] != '#' && r[i] != 0) {
        i++;
      }
      if(r[i] == 0) {
        processError("missformed register short integer value. Missing # between value and base.");
      }

      // remove beginning and ending " and removing leading spaces
      xcopy(r, r + 1, strlen(r));
      while(r[0] == ' ') {
        xcopy(r, r + 1, strlen(r));
      }
      r[strlen(r) - 1] = 0;

      // Convert string to upper case
      for(i=0; r[i]!=0; i++) {
        if('a' <= r[i] && r[i] <= 'z') {
          r[i] -= 32;
        }
      }

      strToShortInteger(r, TEMP_REGISTER_1);
      checkRegisterType(regist, letter, dtShortInteger, getRegisterTag(TEMP_REGISTER_1));
      if(*REGISTER_SHORT_INTEGER_DATA(TEMP_REGISTER_1) != *REGISTER_SHORT_INTEGER_DATA(regist)) {
        wrongRegisterValue(regist, letter, r);
      }
    }
    else if(strcmp(l, "CPLX") == 0) {
      checkRegisterType(regist, letter, dtComplex34, amNone);

      // remove beginning and ending " and removing leading spaces
      xcopy(r, r + 1, strlen(r));
      while(r[0] == ' ') {
        xcopy(r, r + 1, strlen(r));
      }
      r[strlen(r) - 1] = 0;

      // find the i separating the real and imagynary part
      i = 0;
      while(r[i] != 'i' && r[i] != 0) {
        i++;
      }
      if(r[i] == 0) {
        processError("missformed register complex34 value. Missing i between real and imaginary part.");
      }

      // separate real and imaginary part
      r[i] = 0;
      strcpy(real, r);
      strcpy(imag, r + i + 1);

      // remove leading spaces
      while(imag[0] == ' ') {
        xcopy(imag, imag + 1, strlen(imag));
      }

      // removing trailing spaces from real part
      while(real[strlen(real) - 1] == ' ') {
        real[strlen(real) - 1] = 0;
      }

      // removing trailing spaces from imaginary part
      while(imag[strlen(imag) - 1] == ' ') {
        imag[strlen(imag) - 1] = 0;
      }

      // replace , with . in the real part
      for(i=0; i<(int)strlen(real); i++) {
        if(real[i] == ',') {
          real[i] = '.';
        }
      }

      // replace , with . in the imaginary part
      for(i=0; i<(int)strlen(imag); i++) {
        if(imag[i] == ',') {
          imag[i] = '.';
        }
      }

      stringToReal34(real, &expectedReal34);
      stringToReal34(imag, &expectedImag34);
      if(!real34AreEqual(REGISTER_REAL34_DATA(regist), &expectedReal34)) {
        strcat(r, " +ix ");
        strcat(r, imag);
        expectedAndShouldBeValue(regist, letter, r, registerExpectedAndValue);
        if(relativeErrorReal34(&expectedReal34, REGISTER_REAL34_DATA(regist), "real", regist, letter) == RE_INACCURATE) {
          wrongRegisterValue(regist, letter, r);
        }
      }
      else if(!real34AreEqual(REGISTER_IMAG34_DATA(regist), &expectedImag34)) {
        strcat(r, " +ix ");
        strcat(r, imag);
        expectedAndShouldBeValue(regist, letter, r, registerExpectedAndValue);
        if(relativeErrorReal34(&expectedImag34, REGISTER_IMAG34_DATA(regist), "imaginary", regist, letter) == RE_INACCURATE) {
          wrongRegisterValue(regist, letter, r);
        }
      }
    }
    else if(strcmp(l, "TIME") == 0) {
      int32_t k = 0;
      bool isHms = false;

      // find the : separating hours and minutes
      i = 0;
      while(r[i] != ':' && r[i] != 0) {
        i++;
      }
      if(r[i] == ':') { // Input by HMS
        isHms = true;
        k = i;
        r[i] = '.';
        do {
          ++k;
          if((r[k] != ':') && (r[k] != '.') && (r[k] != ',')) {
            r[++i] = r[k];
          }
        } while(r[k] != 0);
      }
      am = amNone;

      // remove beginning and ending " and removing leading spaces
      xcopy(r, r + 1, strlen(r));
      while(r[0] == ' ') {
        xcopy(r, r + 1, strlen(r));
      }
      r[strlen(r) - 1] = 0;

      // replace , with .
      for(i=0; i<(int)strlen(r); i++) {
        if(r[i] == ',') {
          r[i] = '.';
        }
      }

      checkRegisterType(regist, letter, dtTime, amNone);
      stringToReal34(r, &expectedReal34);
      if(isHms) {
        hmmssToSeconds(&expectedReal34, &expectedReal34);
      }
      if(!real34AreEqual(REGISTER_REAL34_DATA(regist), &expectedReal34)) {
        expectedAndShouldBeValue(regist, letter, r, registerExpectedAndValue);
        if(relativeErrorReal34(&expectedReal34, REGISTER_REAL34_DATA(regist), "time", regist, letter) == RE_INACCURATE) {
          wrongRegisterValue(regist, letter, r);
        }
      }
    }
    else if(strcmp(l, "DATE") == 0) {
      // remove beginning and ending " and removing leading spaces
      xcopy(r, r + 1, strlen(r));
      while(r[0] == ' ') {
        xcopy(r, r + 1, strlen(r));
      }
      r[strlen(r) - 1] = 0;

      // replace , with .
      for(i=0; i<(int)strlen(r); i++) {
        if(r[i] == ',') {
          r[i] = '.';
        }
      }

      checkRegisterType(regist, letter, dtDate, amNone);
      reallocateRegister(TEMP_REGISTER_1, dtReal34, REAL34_SIZE, amNone);
      stringToReal34(r, REGISTER_REAL34_DATA(TEMP_REGISTER_1));
      convertReal34RegisterToDateRegister(TEMP_REGISTER_1, TEMP_REGISTER_1);
      real34Copy(REGISTER_REAL34_DATA(TEMP_REGISTER_1), &expectedReal34)
      if(!real34AreEqual(REGISTER_REAL34_DATA(regist), &expectedReal34)) {
        expectedAndShouldBeValue(regist, letter, r, registerExpectedAndValue);
        if(relativeErrorReal34(&expectedReal34, REGISTER_REAL34_DATA(regist), "date", regist, letter) == RE_INACCURATE) {
          wrongRegisterValue(regist, letter, r);
        }
      }
    }
    else {
      sprintf(msgString, "missformed register value. Unknown data type %s for register %s", l, p+1);
      processError(msgString);
    }
  }

  else {
    sprintf(msgString, "unknown checking %s", l);
    processError(msgString);
  }
}



void outParameters(char *token) {
  char parameter[2000];
  int32_t lg;

  strReplace(token, "inf", "9e9999");

  while(*token == ' ') {
    token++;
  }
  while(*token != 0) {
    int32_t index = 0;
    while(*token != ' ' && *token != 0) {
      if(*token == '"') { // Inside a string
        lg = endOfString(token) - token;
        strncpy(parameter + index, token, lg--);
        index += lg;
        token += lg;
      }
      parameter[index++] = *(token++);
    }
    parameter[index] = 0;

    //printf("  Check %s\n", parameter);
    checkExpectedOutParameter(parameter);

    while(*token == ' ') {
      token++;
    }
  }
}



void callFunction(void) {
  lastErrorCode = 0;

  switch(funcType) {
    case FUNC_NOPARAM: {
      if((indexOfItems[functionIndex].status & US_STATUS) == US_ENABLED) {
        saveForUndo();
      }
      else if((indexOfItems[functionIndex].status & US_STATUS) == US_CANCEL) {
        thereIsSomethingToUndo = false;
      }

      funcNoParam(NOPARAM);
      break;
    }

    case FUNC_CVT: {
      funcCvt(NOPARAM);
      break;
    }

    default: {
    }
  }

  if(lastErrorCode == 0) {
    if(functionIndex < LAST_ITEM) {
      if((indexOfItems[functionIndex].status & SLS_STATUS) == SLS_DISABLED) {
        clearSystemFlag(FLAG_ASLIFT);
      }
      else if((indexOfItems[functionIndex].status & SLS_STATUS) == SLS_ENABLED) {
        setSystemFlag(FLAG_ASLIFT);
      }
    }
  }
}



void functionToCall(char *functionName) {
  int32_t function;

  function = 0;
  while(funcTestNoParam[function].name[0] != 0 && strcmp(funcTestNoParam[function].name, functionName) != 0) {
    function++;
  }
  if(funcTestNoParam[function].name[0] != 0) {
    funcNoParam = funcTestNoParam[function].func;
    funcType = FUNC_NOPARAM;

    for(functionIndex=1; functionIndex<=LAST_ITEM; functionIndex++) {
      if(indexOfItems[functionIndex].func == funcNoParam) {
        break;
      }
    }

    if(functionIndex >= LAST_ITEM) {
      sprintf(msgString, "the function %s must be somewhere in the indexOfItems array!", functionName);
      processError(msgString);
    }

    //printf("%s=%d\n", functionName, functionIndex);
    return;
  }

  else {
    function = 0;
    while(funcTestCvt[function].name[0] != 0 && strcmp(funcTestCvt[function].name, functionName) != 0) {
      function++;
    }
    if(funcTestCvt[function].name[0] != 0) {
      //ici il faut tester la présence de fromUnitToISO ou fromISOtoUnit

      funcCvt = funcTestCvt[function].func;
      funcType = FUNC_CVT;

      for(functionIndex=1; functionIndex<=LAST_ITEM; functionIndex++) {
        if(indexOfItems[functionIndex].func == funcCvt) {
          break;
        }
      }

      if(functionIndex >= LAST_ITEM) {
        sprintf(msgString, "the function %s must be somewhere in the indexOfItems array!", functionName);
        processError(msgString);
      }

      return;
    }
  }

  processError("cannot find the function to test: check spelling of the function name and remember the name is case sensitive");
}



void processError(const char *msg) {
  char testName[100];
  if(!testStarted) {
    sprintf(testName, "parsing test %d", testInFile++);
    reporterStartTest(testName);
    testStarted = true;
  }
  reporterTestError(msg);
  testPassed = false;
}



void standardizeLine(void) {
  char *location;

  // trim comments
  location = strstr(line, ";");
  if(location != NULL) {
    *location = 0;
  }

  // trim ending LF
  location = strstr(line, "\n");
  if(location != NULL) {
    *location = 0;
  }

  // trim ending CR
  location = strstr(line, "\r");
  if(location != NULL) {
    *location = 0;
  }

  // trim ending LF
  location = strstr(line, "\n");
  if(location != NULL) {
    *location = 0;
  }

  // Change tabs in spaces
  for(int i=strlen(line)-1; i>0; i--) {
    if(line[i] == '\t') {
      line[i] = ' ';
    }
  }

  // Trim ending spaces
  for(int i=strlen(line)-1; i>0; i--) {
    if(line[i] == ' ') {
      line[i] = 0;
    }
    else {
      break;
    }
  }

  // Trim beginning spaces
  while(line[0] == ' ') {
    xcopy(line, line + 1, strlen(line));
  }

  // 2 spaces ==> 1 space
  for(uint32_t i=0; i<strlen(line); i++) {
    if(line[i] == '"') {
      i = endOfString(line + i) - line;
    }
    if(line[i] == ' ' && line[i + 1] == ' ') {
      xcopy(line + i, line + i + 1, strlen(line + i) - 1);
      line[strlen(line) - 1] = 0;
      i--;
    }
  }
}



void processLine(void) {
  // convert to upper case
  int32_t lg = strlen(line);
  for(int i=0; i<lg; i++) {
    if(line[i] == '"') {
      i = endOfString(line + i) - line;
    }

    if('a' <= line[i] && line[i] <= 'z') {
      line[i] -= 32;
    }
    if(i >= 5 && strncmp(line, "FUNC: ", 6) == 0) {
      break;
    }
  }

  if(strncmp(line, "IN: ", 4) == 0) {
    //printf("%s\n", line);
    strcpy(lastInParameters, line);
    inParameters(line + 4);
  }

  else if(strncmp(line, "FUNC: ", 6) == 0) {
    //printf("%s\n", line);
    functionToCall(line + 6);
  }

  else if(strncmp(line, "OUT: ", 5) == 0) {
    if(testStarted) {
      reporterEndTest(false, "see error messages");
    }
    testStarted = true;
    testPassed = true;
    sprintf(msgString, "test number %d", testInFile++);
    reporterStartTest(msgString);
    callFunction();

    numTestsTotal++;
    outParameters(line + 5);
    reporterEndTest(testPassed, "see error messages");
    testStarted = false;
  }

  else if(line[0] != 0) {
    printf(msgString, "line %d cannot be processed [%s]", lineNumber, line);
    processError(msgString);
  }
}



void processOneFile(void) {
  FILE *testSuite;

  numTestsFile = 0;

  strcpy(fileName, line);
  strcat(fileName, ".txt");
  sprintf(filePathName, "%s/%s", filePath, fileName);

  reporterStartTestSuite(line);

  testSuite = fopen(filePathName, "rb");
  if(testSuite == NULL) {
    reporterStartTest("test file present");
    reporterEndTest(false, "no test file found");
    reporterEndTestSuite();
    return;
  }

  testInFile = 1;
  testStarted = false;

  // Default function to call
  functionIndex = ITM_NOP;
  funcNoParam = fnNop;
  funcType = FUNC_NOPARAM;

  ignore_result(fgets(line, 9999, testSuite));
  lineNumber = 1;
  while(!feof(testSuite)) {
    standardizeLine();
    while(strlen(line) >= 4 && strncmp(line + strlen(line) - 4, " ...", 4) == 0) {
      line[strlen(line) - 3] = 0;
      if(!feof(testSuite)) {
        ignore_result(fgets(line + strlen(line), 9999, testSuite));
        lineNumber++;
        standardizeLine();
      }
    }
    processLine();
    ignore_result(fgets(line, 9999, testSuite));
    lineNumber++;
  }
  if(testStarted) {
    reporterEndTest(false, "see error messages");
  }

  fclose(testSuite);
  reporterEndTestSuite();
}



int processTests(const char *listPath) {
  FILE *fileList;
  char *listPathDup = strdup(listPath);
  filePath = dirname(listPathDup);

  numTestsTotal = 0;
  failedTests = 0;

  fileList = fopen(listPath, "rb");
  if(fileList == NULL) {
    printf("Cannot open file testSuiteList.txt!\n");
    exit(-1);
  }

  reporterStartTestSuites();

  ignore_result(fgets(line, 9999, fileList));
  while(!feof(fileList)) {
    standardizeLine();
    if(line[0] != 0) {
      processOneFile();
    }
    ignore_result(fgets(line, 9999, fileList));
  }

  fclose(fileList);

  bool passed = reporterEndTestSuites();

  free(listPathDup);

  return !passed;
}

int main(int argc, char* argv[]) {
  int exitCode;

  testStarted = false;

  if(argc < 2) {
    printf("Usage: testSuite <list file>\n");
    return 1;
  }

  wp43MemInBlocks = 0;
  gmpMemInBytes = 0;
  mp_set_memory_functions(allocGmp, reallocGmp, freeGmp);

  fnReset(CONFIRMED);

  exitCode = processTests(argv[1]);
  printf("The memory owned by GMP should be 0 bytes. Else report a bug please!\n");
  debugMemory("End of testsuite");

  return exitCode;
}
