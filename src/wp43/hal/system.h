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
 * \file hal/system.h
 */
#if !defined(SYSTEM_H)
  #define SYSTEM_H

  /**
   * Take a screenshot.
   * Creates an image file with an appropriate name that is a bitmap
   * of the contents of the LCD.
   */
  void systemScreenshot(void);
#endif // !SYSTEM_H
