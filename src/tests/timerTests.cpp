// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

extern "C" {
  #include "core/timer.h"
}
#include "hal/time.hpp"
#include <gtest/gtest.h>

using ::testing::Return;
using ::testing::Invoke;

class MockCallback;
static MockCallback *_callback;

class MockCallback {
public:
  MOCK_METHOD(void, callback1, (uint16_t));
  MOCK_METHOD(void, callback2, (uint16_t));
  MockCallback() {
    _callback = this;
  }
};

const static timerId_t timer1 = tidRefreshLcd;
const static timerId_t timer2 = tidAutoRepeat;

void c_callback1(uint16_t param) {
  _callback->callback1(param);
}

void c_callback2(uint16_t param) {
  _callback->callback2(param);
}

TEST(TimerTest, TimerDoesFireIfExpired) {
  MockTime time;
  MockCallback cb;
  EXPECT_CALL(time, uptimeMs).WillOnce(Return(0));
  timerReset();
  timerConfig(timer1, &c_callback1);
  timerStart(timer1, 0, 1);
  EXPECT_CALL(time, uptimeMs).WillOnce(Return(1));
  EXPECT_CALL(cb, callback1).Times(1);
  timerRun();
}

TEST(TimerTest, TimerDoesNotFireIfNotExpired) {
  MockTime time;
  MockCallback cb;
  EXPECT_CALL(time, uptimeMs).WillOnce(Return(0));
  timerReset();
  timerConfig(timer1, &c_callback1);
  timerStart(timer1, 0, 1);
  EXPECT_CALL(time, uptimeMs).WillOnce(Return(0));
  EXPECT_CALL(cb, callback1).Times(0);
  timerRun();
}

TEST(TimerTest, OnlyExpiredTimerFires) {
  MockTime time;
  MockCallback cb;
  EXPECT_CALL(time, uptimeMs)
    .WillOnce(Return(0))
    .WillOnce(Return(0));
  timerReset();
  timerConfig(timer1, &c_callback1);
  timerConfig(timer2, &c_callback2);
  timerStart(timer1, 0, 1);
  timerStart(timer2, 0, 2);
  EXPECT_CALL(time, uptimeMs).WillOnce(Return(1));
  EXPECT_CALL(cb, callback1).Times(1);
  EXPECT_CALL(cb, callback2).Times(0);
  timerRun();
}

TEST(TimerTest, StatusOfTimerReportedCorrectly) {
  MockTime time;
  MockCallback cb;
  EXPECT_CALL(time, uptimeMs).WillOnce(Return(0));
  timerReset();
  EXPECT_EQ(timerGetStatus(timer1), tsUnused);
  timerConfig(timer1, &c_callback1);
  EXPECT_EQ(timerGetStatus(timer1), tsStopped);
  timerStart(timer1, 0, 1);
  EXPECT_EQ(timerGetStatus(timer1), tsRunning);
  EXPECT_CALL(time, uptimeMs).WillOnce(Return(1));
  EXPECT_CALL(cb, callback1).Times(1);
  timerRun();
  EXPECT_EQ(timerGetStatus(timer1), tsCompleted);
}

TEST(TimerTest, ParameterPassedToCallback) {
  MockTime time;
  MockCallback cb;
  EXPECT_CALL(time, uptimeMs).WillOnce(Return(0));
  timerReset();
  timerConfig(timer1, &c_callback1);
  timerStart(timer1, 1023, 1);
  EXPECT_CALL(time, uptimeMs).WillOnce(Return(1));
  EXPECT_CALL(cb, callback1(1023)).Times(1);
  timerRun();
}

TEST(TimerTest, ZeroTimeoutWillFireOnNextTimerRun) {
  MockTime time;
  MockCallback cb;
  EXPECT_CALL(time, uptimeMs).WillOnce(Return(0));
  timerReset();
  timerConfig(timer1, &c_callback1);
  timerStart(timer1, 0, 0);
  EXPECT_CALL(time, uptimeMs).WillOnce(Return(0));
  EXPECT_CALL(cb, callback1).Times(1);
  timerRun();
}

TEST(TimerTest, UsingTimerRunReturnValueCausesATimerToFire) {
  MockTime time;
  MockCallback cb;
  EXPECT_CALL(time, uptimeMs)
    .WillOnce(Return(100))
    .WillOnce(Return(100));
  timerReset();
  timerConfig(timer1, &c_callback1);
  timerStart(timer1, 0, 100);
  uint32_t waitTime = timerRun();
  EXPECT_CALL(time, uptimeMs).WillOnce(Return(100 + waitTime));
  EXPECT_CALL(cb, callback1).Times(1);
  timerRun();
}

TEST(TimerTest, CallbackCanRestartTimer) {
  MockTime time;
  MockCallback cb;
  EXPECT_CALL(time, uptimeMs)
    .WillOnce(Return(0))
    .WillOnce(Return(0))
    .WillOnce(Return(0));
  timerReset();
  timerConfig(timer1, &c_callback1);
  timerStart(timer1, 0, 0);
  EXPECT_CALL(cb, callback1)
    .WillOnce(Invoke([] (uint16_t) {
      timerStart(timer1, 0, 100);
    }));
  timerRun();
  EXPECT_EQ(timerGetStatus(timer1), tsRunning);
  EXPECT_CALL(time, uptimeMs).WillOnce(Return(100));
  EXPECT_CALL(cb, callback1).Times(1);
  timerRun();
}

TEST(TimerTest, WrapAroundOfUptimeWorksAsExpected) {
  MockTime time;
  MockCallback cb;
  EXPECT_CALL(time, uptimeMs).WillOnce(Return(UINT32_MAX - 99));
  timerReset();
  timerConfig(timer1, &c_callback1);
  timerStart(timer1, 0, 100);
  EXPECT_CALL(time, uptimeMs).WillOnce(Return(UINT32_MAX));
  timerRun();
  EXPECT_CALL(time, uptimeMs).WillOnce(Return(0));
  EXPECT_CALL(cb, callback1).Times(1);
  timerRun();
}
