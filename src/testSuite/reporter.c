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

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define FILE_NAME            "testSuiteJunit.xml"
#define MAX_TEST_SUITES_SIZE 1000000
#define MAX_TEST_SUITE_SIZE   100000

static uint32_t _passingTests = 0;
static uint32_t _failingTests = 0;
static char     _testSuiteName[100];
static char     _testName[100];
static uint32_t _testsInSuite = 0;
static char     _testSuiteOut[MAX_TEST_SUITE_SIZE];
static uint32_t _testSuiteOutPtr = 0;
static char     _testSuitesOut[MAX_TEST_SUITES_SIZE];
static uint32_t _testSuitesOutPtr = 0;
static uint32_t _testId = 0;
static uint32_t _testSuiteId = 0;
static uint32_t _passingTestsInSuite = 0;
static uint32_t _failingTestsInSuite = 0;

static void _junitStartTestSuites(void) {
}

static void _junitStartTestSuite(void) {
}

static void _junitStartTest(void) {
}

//static void _junitTestError(const char *errorMsg) {
//}

static void _junitEndTest(bool_t passed, const char *errorMsg) {
  char tmp[1000];
  if(passed) {
    sprintf(tmp, "\t\t<testcase id=\"testcase.%d\" name=\"%s\" />\n", _testId++, _testName);
  } else {
    sprintf(tmp, "\t\t<testcase id=\"%d\" name=\"%s\">\n\t\t\t<failure message=\"%s\" />\n\t\t</testcase>\n", _testId++, _testName, errorMsg);
  }
  assert(strlen(tmp) + _testSuiteOutPtr < MAX_TEST_SUITE_SIZE);
  strcpy(_testSuiteOut + _testSuiteOutPtr, tmp);
  _testSuiteOutPtr += strlen(tmp);
}

static void _junitEndTestSuite(void) {
  assert(_testSuiteOutPtr + _testSuitesOutPtr <= MAX_TEST_SUITES_SIZE);
  char tmp[100];
  sprintf(tmp, "\t<testsuite id=\"testsuite.%d\" name=\"%s\" tests=\"%d\" failures=\"%d\">\n", _testSuiteId++, _testSuiteName, _testsInSuite, _failingTestsInSuite);
  assert(strlen(tmp) + _testSuitesOutPtr + _testSuiteOutPtr < MAX_TEST_SUITES_SIZE);
  strcpy(_testSuitesOut + _testSuitesOutPtr, tmp);
  _testSuitesOutPtr += strlen(tmp);
  strncpy(_testSuitesOut + _testSuitesOutPtr, _testSuiteOut, _testSuiteOutPtr);
  _testSuitesOutPtr += _testSuiteOutPtr;
  _testSuiteOutPtr = 0;
  sprintf(tmp, "\t</testsuite>\n");
  assert(strlen(tmp) + _testSuitesOutPtr < MAX_TEST_SUITES_SIZE);
  strcpy(_testSuitesOut + _testSuitesOutPtr, tmp);
  _testSuitesOutPtr += strlen(tmp);
}

static void _junitEndTestSuites(void) {
  FILE *junitOut = fopen(FILE_NAME, "wb");
  if(junitOut == NULL) {
    fprintf(stderr, "Cannot create file %s\n", FILE_NAME);
  }
  _testSuitesOut[_testSuitesOutPtr] = '\0';
  fprintf(junitOut, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  fprintf(junitOut, "<testsuites name=\"All tests\" tests=\"%d\" failures=\"%d\">\n", _passingTests + _failingTests, _failingTests);
  fprintf(junitOut, "%s", _testSuitesOut);
  fprintf(junitOut, "</testsuites>\n");
  fclose(junitOut);
}

static void _stdoutStartTestSuites(void) {
}

static void _stdoutStartTestSuite(const char *testSuiteName) {
  printf("\nChecking: %s", testSuiteName);
}

static void _stdoutStartTest() {
  if((_testsInSuite % 10) == 0) {
    printf(".");
  }
}

static void _stdoutTestError(const char *errorMsg) {
  printf("\nTest [%s]: %s", _testName, errorMsg);
}

static void _stdoutEndTest(bool_t passed, const char *errorMsg) {
  if(!passed) {
    printf("\nTest [%s] failed: %s", _testName, errorMsg);
  }
}

static void _stdoutEndTestSuite(void) {
}

static void _stdoutEndTestSuites(void) {
  printf("\n\n************************************\n");
  printf("* %6d TESTS PASSED SUCCESSFULLY *\n", _passingTests);
  printf("* %6d TEST%c FAILED              *\n", _failingTests, _failingTests == 1 ? ' ' : 'S');
  printf("************************************\n");
}

void reporterStartTestSuites(void) {
  _testName[99] = '\0';
  _testSuiteName[99] = '\0';
  _stdoutStartTestSuites();
  _junitStartTestSuites();
}

void reporterStartTestSuite(const char *testSuiteName) {
  _testsInSuite = 0;
  _failingTestsInSuite = 0;
  _passingTestsInSuite = 0;
  strncpy(_testSuiteName, testSuiteName, 99);
  _stdoutStartTestSuite(testSuiteName);
  _junitStartTestSuite();
}

void reporterStartTest(const char *testName) {
  strncpy(_testName, testName, 99);
  _testsInSuite++;
  _stdoutStartTest();
  _junitStartTest();
}

void reporterTestError(const char *errorMsg) {
  _stdoutTestError(errorMsg);
  //_junitTestError(errorMsg);
}

void reporterEndTest(bool_t passed, const char *errorMsg) {
  if(passed) {
    _passingTests++;
    _passingTestsInSuite++;
  } else {
    _failingTests++;
    _failingTestsInSuite++;
  }
  _stdoutEndTest(passed, errorMsg);
  _junitEndTest(passed, errorMsg);
}

void reporterEndTestSuite(void) {
  _stdoutEndTestSuite();
  _junitEndTestSuite();
}

bool_t reporterEndTestSuites(void) {
  _stdoutEndTestSuites();
  _junitEndTestSuites();
  return (_failingTests == 0);
}
