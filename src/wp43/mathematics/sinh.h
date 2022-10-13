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
 * \file mathematics/sinh.h
 */
#if !defined(SINH_H)
  #define SINH_H

  #include "defines.h"
  #include <stdint.h>

  void fnSinh   (uint16_t unusedButMandatoryParameter);

  #if (EXTRA_INFO_ON_CALC_ERROR == 1)
    void sinhError(void);
  #else // (EXTRA_INFO_ON_CALC_ERROR != 1)
    #define sinhError typeError
  #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)

  void sinhLonI (void);
  void sinhRema (void);
  void sinhCxma (void);
  void sinhReal (void);
  void sinhCplx (void);
#endif // !SINH_H
