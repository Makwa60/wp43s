// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "hal/time.hpp"

extern "C" {
  #include "hal/time.h"
}
#include <assert.h>

static Time *_mockTime = nullptr;

Time::Time() {
  assert(_mockTime == nullptr);
  _mockTime = this;
}



Time::~Time() {
  _mockTime = nullptr;
}



void timeGetTimeInfo(timeInfo_t *) {
}



void timeGetDateInfo(dateInfo_t *) {
}



uint32_t timeCurrentMs(void) {
  return 0;
}



uint32_t timeUptimeMs(void) {
  if(_mockTime != nullptr) {
    return _mockTime->uptimeMs();
  }
  return 0;
}
