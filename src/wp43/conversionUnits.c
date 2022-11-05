// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "conversionUnits.h"

#include "constantPointers.h"
#include "debug.h"
#include "error.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

static void unitConversion(const real_t * const coefficient, uint16_t multiplyDivide) {
  real_t reX;

  if(!saveLastX()) {
    return;
  }

  if(getRegisterDataType(REGISTER_X) == dtReal34) {
    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &reX);
  }
  else if(getRegisterDataType(REGISTER_X) == dtLongInteger) {
    convertLongIntegerRegisterToReal(REGISTER_X, &reX, &ctxtReal39);
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("%s cannot be converted!", getRegisterDataTypeName(REGISTER_X, true, false));
    return;
  }

  if(multiplyDivide == multiply) {
    realMultiply(&reX, coefficient, &reX, &ctxtReal39);
  }
  else {
    realDivide(&reX, coefficient, &reX, &ctxtReal39);
  }

  convertRealToReal34ResultRegister(&reX, REGISTER_X);

  adjustResult(REGISTER_X, false, false, -1, -1, -1);
}



void fnCvtCToF(uint16_t unusedButMandatoryParameter) {
  real_t reX;

  if(!saveLastX()) {
    return;
  }

  if(getRegisterDataType(REGISTER_X) == dtReal34) {
    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &reX);
  }
  else if(getRegisterDataType(REGISTER_X) == dtLongInteger) {
    convertLongIntegerRegisterToReal(REGISTER_X, &reX, &ctxtReal39);
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("%s cannot be converted!", getRegisterDataTypeName(REGISTER_X, true, false));
    return;
  }

  realFMA(&reX, const_9on5, const_32, &reX, &ctxtReal39);

  convertRealToReal34ResultRegister(&reX, REGISTER_X);

  adjustResult(REGISTER_X, false, false, -1, -1, -1);
}



void fnCvtFToC(uint16_t unusedButMandatoryParameter) {
  real_t reX;

  if(!saveLastX()) {
    return;
  }

  if(getRegisterDataType(REGISTER_X) == dtReal34) {
    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &reX);
  }
  else if(getRegisterDataType(REGISTER_X) == dtLongInteger) {
    convertLongIntegerRegisterToReal(REGISTER_X, &reX, &ctxtReal39);
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("%s cannot be converted!", getRegisterDataTypeName(REGISTER_X, true, false));
    return;
  }

  realSubtract(&reX, const_32, &reX, &ctxtReal39);
  realDivide(&reX, const_9on5, &reX, &ctxtReal39);

  convertRealToReal34ResultRegister(&reX, REGISTER_X);

  adjustResult(REGISTER_X, false, false, -1, -1, -1);
}


void fnCvtYearS(uint16_t multiplyDivide) {
  unitConversion(const_YearToS, multiplyDivide);
}


void fnCvtCalJ(uint16_t multiplyDivide) {
  unitConversion(const_CalToJ, multiplyDivide);
}


void fnCvtBtuJ(uint16_t multiplyDivide) {
  unitConversion(const_BtuToJ, multiplyDivide);
}


void fnCvtWhJ(uint16_t multiplyDivide) {
  unitConversion(const_WhToJ, multiplyDivide);
}


void fnCvtHpeW(uint16_t multiplyDivide) {
  unitConversion(const_HpeToW, multiplyDivide);
}


void fnCvtHpmW(uint16_t multiplyDivide) {
  unitConversion(const_HpmToW, multiplyDivide);
}


void fnCvtHpukW(uint16_t multiplyDivide) {
  unitConversion(const_HpukToW, multiplyDivide);
}


void fnCvtLbfN(uint16_t multiplyDivide) {
  unitConversion(const_LbfToN, multiplyDivide);
}


void fnCvtBarPa(uint16_t multiplyDivide) {
  unitConversion(const_BarToPa, multiplyDivide);
}


void fnCvtPsiPa(uint16_t multiplyDivide) {
  unitConversion(const_PsiToPa, multiplyDivide);
}


void fnCvtInhgPa(uint16_t multiplyDivide) {
  unitConversion(const_InhgToPa, multiplyDivide);
}


void fnCvtMmhgPa(uint16_t multiplyDivide) {
  unitConversion(const_MmhgToPa, multiplyDivide);
}


void fnCvtTorrPa(uint16_t multiplyDivide) {
  unitConversion(const_TorrToPa, multiplyDivide);
}


void fnCvtAtmPa(uint16_t multiplyDivide) {
  unitConversion(const_AtmToPa, multiplyDivide);
}


void fnCvtLbKg(uint16_t multiplyDivide) {
  unitConversion(const_LbToKg, multiplyDivide);
}


void fnCvtCwtKg(uint16_t multiplyDivide) {
  unitConversion(const_CwtToKg, multiplyDivide);
}


void fnCvtOzG(uint16_t multiplyDivide) {
  unitConversion(const_OzToG, multiplyDivide);
}


void fnCvtStoneKg(uint16_t multiplyDivide) {
  unitConversion(const_StoneToKg, multiplyDivide);
}


void fnCvtShortcwtKg(uint16_t multiplyDivide) {
  unitConversion(const_ShortcwtToKg, multiplyDivide);
}


void fnCvtTrozG(uint16_t multiplyDivide) {
  unitConversion(const_TrozToG, multiplyDivide);
}


void fnCvtTonKg(uint16_t multiplyDivide) {
  unitConversion(const_TonToKg, multiplyDivide);
}


void fnCvtShorttonKg(uint16_t multiplyDivide) {
  unitConversion(const_ShorttonToKg, multiplyDivide);
}


void fnCvtCaratG(uint16_t multiplyDivide) {
  unitConversion(const_CaratToG, multiplyDivide);
}


void fnCvtLiangKg(uint16_t multiplyDivide) {
  unitConversion(const_20, multiplyDivide);
}


void fnCvtJinKg(uint16_t multiplyDivide) {
  unitConversion(const_2, multiplyDivide);
}


void fnCvtAuM(uint16_t multiplyDivide) {
  unitConversion(const_AuToM, multiplyDivide);
}


void fnCvtMiKm(uint16_t multiplyDivide) {
  unitConversion(const_MiToKm, multiplyDivide);
}


void fnCvtLyM(uint16_t multiplyDivide) {
  unitConversion(const_LyToM, multiplyDivide);
}


void fnCvtNmiKm(uint16_t multiplyDivide) {
  unitConversion(const_NmiToKm, multiplyDivide);
}


void fnCvtFtM(uint16_t multiplyDivide) {
  unitConversion(const_FtToM, multiplyDivide);
}


void fnCvtPcM(uint16_t multiplyDivide) {
  unitConversion(const_PcToM, multiplyDivide);
}


void fnCvtInchMm(uint16_t multiplyDivide) {
  unitConversion(const_InchToMm, multiplyDivide);
}


void fnCvtSfeetM(uint16_t multiplyDivide) {
  unitConversion(const_SfeetToM, multiplyDivide);
}


void fnCvtYardM(uint16_t multiplyDivide) {
  unitConversion(const_YardToM, multiplyDivide);
}


void fnCvtPointMm(uint16_t multiplyDivide) {
  unitConversion(const_PointToMm, multiplyDivide);
}


void fnCvtFathomM(uint16_t multiplyDivide) {
  unitConversion(const_FathomToM, multiplyDivide);
}


void fnCvtLiM(uint16_t multiplyDivide) {
  unitConversion(const_LiToM, multiplyDivide);
}


void fnCvtChiM(uint16_t multiplyDivide) {
  unitConversion(const_3, multiplyDivide);
}


void fnCvtYinM(uint16_t multiplyDivide) {
  unitConversion(const_YinToM, multiplyDivide);
}


void fnCvtCunM(uint16_t multiplyDivide) {
  unitConversion(const_CunToM, multiplyDivide);
}


void fnCvtZhangM(uint16_t multiplyDivide) {
  unitConversion(const_ZhangToM, multiplyDivide);
}


void fnCvtFenM(uint16_t multiplyDivide) {
  unitConversion(const_FenToM, multiplyDivide);
}


void fnCvtMileM(uint16_t multiplyDivide) {
  unitConversion(const_MiToM, multiplyDivide);
}


void fnCvtNMiM(uint16_t multiplyDivide) {
  unitConversion(const_NmiToM, multiplyDivide);
}


void fnCvtGalukL(uint16_t multiplyDivide) {
  unitConversion(const_GalukToL, multiplyDivide);
}


void fnCvtGalusL(uint16_t multiplyDivide) {
  unitConversion(const_GalusToL, multiplyDivide);
}


void fnCvtFlozukMl(uint16_t multiplyDivide) {
  unitConversion(const_FlozukToMl, multiplyDivide);
}


void fnCvtFlozusMl(uint16_t multiplyDivide) {
  unitConversion(const_FlozusToMl, multiplyDivide);
}


void fnCvtBarrelM3(uint16_t multiplyDivide) {
  unitConversion(const_BarrelToM3, multiplyDivide);
}


void fnCvtQuartL(uint16_t multiplyDivide) {
  unitConversion(const_QuartToL, multiplyDivide);
}


void fnCvtAcreHa(uint16_t multiplyDivide) {
  unitConversion(const_AccreToHa, multiplyDivide);
}


void fnCvtAcreusHa(uint16_t multiplyDivide) {
  unitConversion(const_AccreusToHa, multiplyDivide);
}


void fnCvtHectareM2(uint16_t multiplyDivide) {
  unitConversion(const_10000, multiplyDivide);
}


void fnCvtMuM2(uint16_t multiplyDivide) {
  unitConversion(const_MuToM2, multiplyDivide);
}


void fnCvtLbfftNm(uint16_t multiplyDivide) {
  unitConversion(const_LbfftToNm, multiplyDivide);
}



void fnCvtRatioDb(uint16_t tenOrTwenty) { // ten: power ratio   twenty: field ratio
  real_t reX;

  if(!saveLastX()) {
    return;
  }

  if(getRegisterDataType(REGISTER_X) == dtReal34) {
    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &reX);
  }
  else if(getRegisterDataType(REGISTER_X) == dtLongInteger) {
    convertLongIntegerRegisterToReal(REGISTER_X, &reX, &ctxtReal39);
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("%s cannot be converted!", getRegisterDataTypeName(REGISTER_X, true, false));
    return;
  }

  WP34S_Log10(&reX, &reX, &ctxtReal39);
  realMultiply(&reX, (tenOrTwenty == 10 ? const_10 : const_20), &reX, &ctxtReal39);

  convertRealToReal34ResultRegister(&reX, REGISTER_X);

  adjustResult(REGISTER_X, false, false, -1, -1, -1);
}



void fnCvtDbRatio(uint16_t tenOrTwenty) { // ten: power ratio   twenty: field ratio
  real_t reX;

  if(!saveLastX()) {
    return;
  }

  if(getRegisterDataType(REGISTER_X) == dtReal34) {
    real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &reX);
  }
  else if(getRegisterDataType(REGISTER_X) == dtLongInteger) {
    convertLongIntegerRegisterToReal(REGISTER_X, &reX, &ctxtReal39);
    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE_IN_BYTES, amNone);
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    errorMoreInfo("%s cannot be converted!", getRegisterDataTypeName(REGISTER_X, true, false));
    return;
  }

  realDivide(&reX, (tenOrTwenty == 10 ? const_10 : const_20), &reX, &ctxtReal39);
  realPower(const_10, &reX, &reX, &ctxtReal39);

  convertRealToReal34ResultRegister(&reX, REGISTER_X);

  adjustResult(REGISTER_X, false, false, -1, -1, -1);
}
