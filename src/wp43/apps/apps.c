// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "apps/apps.h"

#include "calcMode.h"
#include "flags.h"
#include "items.h"
#include "ui/screen.h"
#include <assert.h>

#include "wp43.h"

static guiLayout_t      _appsLayout;
static appsKeyHandler_t _appsKeyHandler;
static appsDraw_t       _appsDraw;

static bool _appsHandleKeyDefault(int16_t item) {
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
  bool processed = _appsKeyHandler(item);
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

  hourGlassIconEnabled = false;
  clearSystemFlag(FLAG_ALPHA);

  calcModeEnter(cmApp);
}
