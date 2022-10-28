# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: Copyright The WP43 Authors

testSuite_src = files(
  'reporter.c',
  'testSuite.c',
  'hal/audio.c',
  'hal/gui.c',
  'hal/io.c',
  'hal/lcd.c',
  'hal/system.c',
  'hal/time.c',
  'hal/timer.c')
testSuite = executable(
  'testSuite',
  build_by_default    : false,
  sources             : decNumber_src + wp43_src + testSuite_src +
                        [rasterFontsData_c, softmenuCatalogs_h,
                          constantPointers_ch],
  c_args              : '-DTESTSUITE_BUILD',
  include_directories : [decNumber_inc, wp43_inc],
  dependencies        : [gmp_dep, m_dep],
  native              : true)
test(
  'testSuite',
  testSuite,
  args    : [files('tests/testSuiteList.txt')],
  workdir : meson.project_source_root(),
  timeout : 120)
