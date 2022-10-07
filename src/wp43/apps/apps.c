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

#include "apps/apps.h"

#include "calcMode.h"
#include "items.h"
#include "ui/screen.h"
#include <assert.h>

#include "wp43.h"

static guiLayout_t      _appsLayout;
static appsKeyHandler_t _appsKeyHandler;
static appsDraw_t       _appsDraw;

static bool_t _appsHandleKeyDefault(int16_t item) {
  switch(item) {
    case ITM_BACKSPACE:
      calcModeLeave();
      break;
    case ITM_EXIT:
      // TODO: This should probably be set in init functions instead
      rbr1stDigit = true;
      calcModeLeave();
      break;
    case ITM_SHIFTf:
      fnScreenDump(NOPARAM);
      break;
    default:
      return false;
  }
  return true;
}

void appsHandleKey(int16_t item) {
  assert(calcMode == cmApp);
  bool_t processed = _appsKeyHandler(item);
  if(!processed) {
    processed = _appsHandleKeyDefault(item);
  }
  if(processed) {
    refreshScreen();
  }
}

void appsDraw(void) {
  assert(calcMode == cmApp);
  _appsDraw();
}

guiLayout_t appsGetLayout(void) {
  assert(calcMode == cmApp);
  return _appsLayout;
}

void appsEnter(guiLayout_t layout, appsKeyHandler_t keyHandler, appsDraw_t draw) {
  assert(layout < MAX_GUI_LAYOUTS);
  assert(keyHandler != NULL);
  assert(draw != NULL);
  _appsLayout     = layout;
  _appsKeyHandler = keyHandler;
  _appsDraw       = draw;
  calcModeEnter(cmApp);
}
