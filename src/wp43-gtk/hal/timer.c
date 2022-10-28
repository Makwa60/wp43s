// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "hal/timer.h"

#include "wp43-gtk.h"
#include <assert.h>
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdint.h>

#include "wp43.h"

typedef struct {
  guint           gtkTimer;
  timerCallback_t func;
  uint16_t        param;
  bool            running;
} timerState_t;

static timerState_t _timer[MAX_TIMER_ID];
static bool _timersInitialized = false;

gboolean cbGtkTimer(gpointer data) {
  timerState_t *timer = (timerState_t *)data;
  timer->running = false;
  timer->func(timer->param);
  if(screenChange) {
    gtk_widget_queue_draw(screen);
  }
  return G_SOURCE_REMOVE;
}



void timerReset(void) {
  for(int i = 0; i < MAX_TIMER_ID; i++) {
    if(!_timersInitialized) {
      _timer[i].running = false;
    }
    else if(_timer[i].running) {
      timerStop(i);
    }
  }
  _timersInitialized = true;
}



void timerConfig(timerId_t nr, timerCallback_t func) {
  assert(nr < MAX_TIMER_ID);

  _timer[nr].func  = func;
}



void timerStart(timerId_t nr, uint16_t param, uint32_t time) {
  assert(nr < MAX_TIMER_ID);
  assert(time < (UINT32_MAX / 2));

  timerStop(nr);
  _timer[nr].param    = param;
  _timer[nr].running  = true;
  _timer[nr].gtkTimer = g_timeout_add(time, cbGtkTimer, &_timer[nr]);
}



void timerStop(timerId_t nr) {
  assert(nr < MAX_TIMER_ID);

  if(_timer[nr].running) {
    _timer[nr].running = false;
    g_source_remove(_timer[nr].gtkTimer);
  }
}



bool timerIsRunning(timerId_t nr) {
  assert(nr < MAX_TIMER_ID);

  return _timer[nr].running;
}
