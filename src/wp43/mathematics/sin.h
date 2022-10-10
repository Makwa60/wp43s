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

/**
 * \file mathematics/sin.h
 */
#if !defined(SIN_H)
  #define SIN_H

  #include "defines.h"
  #include "realType.h"
  #include <stdint.h>

  void fnSin   (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void sinError(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR != 1)
    #define sinError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void sinLonI (void);
  void sinRema (void);
  void sinCxma (void);
  void sinReal (void);
  void sinCplx (void);
  void sinComplex(const real_t *real, const real_t *imag, real_t *resReal, real_t *resImag, realContext_t *realContext);
#endif // !SIN_H
