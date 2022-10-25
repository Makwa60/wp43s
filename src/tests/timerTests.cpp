// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

extern "C" {
  #include "core/timer.h"
}
#include "hal/time.hpp"
#include <gtest/gtest.h>

using ::testing::Return;

class MockCallback;
static MockCallback *_callback;

class MockCallback {
public:
  MOCK_METHOD(void, callback, (uint16_t));
  MockCallback() {
    _callback = this;
  }
};

void c_callback(uint16_t param) {
  _callback->callback(param);
}

TEST(TimerTest, TimerDoesNotFireIfNotExpired) {
  MockTime time;
  MockCallback cb;
  EXPECT_CALL(time, uptimeMs).WillOnce(Return(0));
  timerReset();
  timerConfig(tidRefreshLcd, &c_callback);
  timerStart(tidRefreshLcd, 0, 1);
  EXPECT_CALL(time, uptimeMs).WillOnce(Return(0));
  EXPECT_CALL(cb, callback).Times(0);
  timerRun();
}
