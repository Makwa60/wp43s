// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file conversionUnits.h
 */
#if !defined(CONVERSIONUNITS_H)
  #define CONVERSIONUNITS_H

  #include <stdint.h>

  // Temperature

  /**
   * Converts °Celcius to °Fahrenheit: (°Celcius * 1,8) + 32.
   * Refreshes the stack. This is the exact formula.
   *
   * \param[in] unusedButMandatoryParameter
   * \return void
   */
  void fnCvtCToF      (uint16_t unusedButMandatoryParameter);

  /**
   * Converts °Fahrenheit to °Celcius: (°Fahrenheit - 32) / 1,8.
   * Refreshes the stack. This is the exact formula.
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnCvtFToC      (uint16_t unusedButMandatoryParameter);

  // Duration
  void fnCvtYearS     (uint16_t multiplyDivide);

  // Energy
  void fnCvtCalJ      (uint16_t multiplyDivide);
  void fnCvtBtuJ      (uint16_t multiplyDivide);
  void fnCvtWhJ       (uint16_t multiplyDivide);

  // Power
  void fnCvtHpeW      (uint16_t multiplyDivide);
  void fnCvtHpukW     (uint16_t multiplyDivide);
  void fnCvtHpmW      (uint16_t multiplyDivide);

  // Force
  void fnCvtLbfN      (uint16_t multiplyDivide);

  // Pressure
  void fnCvtBarPa     (uint16_t multiplyDivide);
  void fnCvtPsiPa     (uint16_t multiplyDivide);
  void fnCvtInhgPa    (uint16_t multiplyDivide);
  void fnCvtMmhgPa    (uint16_t multiplyDivide);
  void fnCvtTorrPa    (uint16_t multiplyDivide);
  void fnCvtAtmPa     (uint16_t multiplyDivide);

  // Mass
  void fnCvtLbKg      (uint16_t multiplyDivide);
  void fnCvtCwtKg     (uint16_t multiplyDivide);
  void fnCvtOzG       (uint16_t multiplyDivide);
  void fnCvtStoneKg   (uint16_t multiplyDivide);
  void fnCvtShortcwtKg(uint16_t multiplyDivide);
  void fnCvtTrozG     (uint16_t multiplyDivide);
  void fnCvtTonKg     (uint16_t multiplyDivide);
  void fnCvtShorttonKg(uint16_t multiplyDivide);
  void fnCvtCaratG    (uint16_t multiplyDivide);
  void fnCvtLiangKg   (uint16_t multiplyDivide);
  void fnCvtJinKg     (uint16_t multiplyDivide);

  // Length
  void fnCvtAuM       (uint16_t multiplyDivide);
  void fnCvtMiKm       (uint16_t multiplyDivide);
  void fnCvtLyM       (uint16_t multiplyDivide);
  void fnCvtNmiKm     (uint16_t multiplyDivide);
  void fnCvtFtM       (uint16_t multiplyDivide);
  void fnCvtPcM       (uint16_t multiplyDivide);
  void fnCvtInchMm    (uint16_t multiplyDivide);
  void fnCvtSfeetM    (uint16_t multiplyDivide);
  void fnCvtYardM     (uint16_t multiplyDivide);
  void fnCvtPointMm   (uint16_t multiplyDivide);
  void fnCvtFathomM   (uint16_t multiplyDivide);
  void fnCvtLiM       (uint16_t multiplyDivide);
  void fnCvtChiM      (uint16_t multiplyDivide);
  void fnCvtYinM      (uint16_t multiplyDivide);
  void fnCvtCunM      (uint16_t multiplyDivide);
  void fnCvtZhangM    (uint16_t multiplyDivide);
  void fnCvtFenM      (uint16_t multiplyDivide);
  void fnCvtMileM     (uint16_t multiplyDivide);
  void fnCvtNMiM      (uint16_t multiplyDivide);

  // Volume
  void fnCvtGalukL    (uint16_t multiplyDivide);
  void fnCvtGalusL    (uint16_t multiplyDivide);
  void fnCvtFlozukMl  (uint16_t multiplyDivide);
  void fnCvtFlozusMl  (uint16_t multiplyDivide);
  void fnCvtQuartL    (uint16_t multiplyDivide);
  void fnCvtBarrelM3  (uint16_t multiplyDivide);

  // Area
  void fnCvtAcreHa    (uint16_t multiplyDivide);
  void fnCvtAcreusHa  (uint16_t multiplyDivide);
  void fnCvtHectareM2 (uint16_t multiplyDivide);
  void fnCvtMuM2      (uint16_t multiplyDivide);

  // ...

  /**
   * Converts power or field ratio to dB
   * dB = (10 or 20) * log10((power or field) ratio) this is the exact formula
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnCvtRatioDb   (uint16_t tenOrTwenty);

  /**
   * Converts dB to power or field ratio
   * (power or field) ratio = 10^(dB / 10 or 20) this is the exact formula
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnCvtDbRatio   (uint16_t tenOrTwenty);

  void fnCvtLbfftNm   (uint16_t multiplyDivide);

#endif // !CONVERSIONUNITS_H
