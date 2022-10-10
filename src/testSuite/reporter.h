// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file reporter.h
 */
#if !defined(REPORTER_H)
  #define REPORTER_H

  #include <stdbool.h>

  void reporterStartTestSuites(void);
  void reporterStartTestSuite(const char *testSuiteName);
  void reporterStartTest(const char *testName);
  void reporterTestError(const char *errorMsg);
  void reporterEndTest(bool passed, const char *errorMsg);
  void reporterEndTestSuite(void);
  bool reporterEndTestSuites(void);

#endif // !REPORTER_H
