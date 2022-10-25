// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#if !defined(TIME_HPP)
  #define TIME_HPP

  #include <stdint.h>
  #include <gmock/gmock.h>

  class Time {
  public:
    virtual ~Time();
    virtual uint32_t uptimeMs() = 0;

  protected:
    Time();
  };

  class MockTime : public Time {
  public:
    MOCK_METHOD(uint32_t, uptimeMs, ());
  };

#endif // !TIME_HPP
