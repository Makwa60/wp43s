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

#include "reporter.h"

#include <stdio.h>
#include <string.h>

static uint32_t _passingTests = 0;
static uint32_t _failingTests = 0;
static char     _testName[100];
static uint32_t _testsInSuite = 0;

void reporterStartTestSuites(void) {
  _testName[99] = '\0';
}

void reporterStartTestSuite(const char *testSuiteName) {
  printf("\nChecking: %s", testSuiteName);
  _testsInSuite = 0;
}

void reporterStartTest(const char *testName) {
  strncpy(_testName, testName, 99);
  _testsInSuite++;
  if((_testsInSuite % 10) == 0) {
    printf(".");
  }
}

void reporterTestError(const char *errorMsg) {
  printf("\nTest [%s]: %s", _testName, errorMsg);
}

void reporterEndTest(bool_t passed, const char *errorMsg) {
  if(passed) {
    _passingTests++;
  } else {
    printf("\nTest [%s] failed: %s", _testName, errorMsg);
    _failingTests++;
  }
}

void reporterEndTestSuite(void) {
}

bool_t reporterEndTestSuites(void) {
  printf("\n\n************************************\n");
  printf("* %6d TESTS PASSED SUCCESSFULLY *\n", _passingTests);
  printf("* %6d TEST%c FAILED              *\n", _failingTests, _failingTests == 1 ? ' ' : 'S');
  printf("************************************\n");
  return (_failingTests == 0);
}
