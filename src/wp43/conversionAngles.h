// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file conversionAngles.h
 */
#if !defined(CONVERSIONANGLES_H)
  #define CONVERSIONANGLES_H

  #include "realType.h"
  #include "typeDefinitions.h"
  #include <stdint.h>

  void     fnCvtToCurrentAngularMode   (uint16_t fromAngularMode);
  void     fnCvtFromCurrentAngularMode (uint16_t toAngularMode);
  void     fnCvtDmsToCurrentAngularMode(uint16_t unusedButMandatoryParameter);
  void     fnCvtDegToRad               (uint16_t unusedButMandatoryParameter);
  void     fnCvtDegToDms               (uint16_t unusedButMandatoryParameter);
  void     fnCvtDmsToDeg               (uint16_t unusedButMandatoryParameter);
  void     fnCvtRadToDeg               (uint16_t unusedButMandatoryParameter);
  void     fnCvtRadToMultPi            (uint16_t unusedButMandatoryParameter);
  void     fnCvtMultPiToRad            (uint16_t unusedButMandatoryParameter);
  void     convertAngle34FromTo        (real34_t *angle34, angularMode_t fromAngularMode, angularMode_t toAngularMode);
  void     convertAngleFromTo          (real_t *angle, angularMode_t fromAngularMode, angularMode_t toAngularMode, realContext_t *realContext);
  void     checkDms34                  (real34_t *angle34Dms);
  uint32_t getInfiniteComplexAngle     (real_t *x, real_t *y);
  void     setInfiniteComplexAngle     (uint32_t angle, real_t *x, real_t *y);

  /**
   * Converts a real34 from DMS to DEG
   *
   * \param[in]  angleDms Real to be conveted to DEG, the format of the real must be ddd.mm.ssfffffff e.g. 12.345678 = 12°34'56.78"
   * \param[out] angleDec Converted real, from the example above the result is 12.58243888888888888888888888888889°
   */
  void     real34FromDmsToDeg          (const real34_t *angleDms, real34_t *angleDec);

  /**
   * Converts a real34 from DEG to DMS
   *
   * \param[in]  angleDec Real to be conveted to DMS, 23.456789°
   * \param[out] angleDms Converted real, from the example above the result 23.27244404 = 23°27'24.4404"
   */
  void     real34FromDegToDms          (const real34_t *angleDec, real34_t *angleDms);

#endif // !CONVERSIONANGLES_H
