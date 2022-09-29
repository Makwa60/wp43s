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
 * \file apps/apps.h
 */
#if !defined(APPS_H)
  #define APPS_H

  #include "hal/gui.h"
  #include "typeDefinitions.h"
  #include <stdint.h>

  // Interface for other modules

  void        appsHandleKey(int16_t item);
  void        appsDraw     (void);
  guiLayout_t appsGetLayout(void);

  // Interface for apps

  typedef bool_t (*appsKeyHandler_t)(int16_t item);
  typedef void   (*appsDraw_t)      (void);

  void appsEnter(guiLayout_t layout, appsKeyHandler_t keyHandler, appsDraw_t draw);

#endif // !APPS_H
