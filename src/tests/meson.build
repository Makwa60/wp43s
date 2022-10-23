# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: Copyright The WP43 Authors

tests_src = files(
  'catalogTests.cpp',
  '../testSuite/hal/audio.c',
  '../testSuite/hal/gui.c',
  '../testSuite/hal/io.c',
  '../testSuite/hal/lcd.c',
  '../testSuite/hal/system.c',
  '../testSuite/hal/time.c')
tests = executable(
  'tests',
  build_by_default    : false,
  sources             : decNumber_src + wp43_src + tests_src +
                        [rasterFontsData_c, softmenuCatalogs_h,
                          constantPointers_ch, vcs_h, version_h],
  c_args              : '-DTESTSUITE_BUILD',
  include_directories : [decNumber_inc, wp43_inc],
  dependencies        : [gtest_main_dep, gtk_dep, gmp_dep, m_dep],
  native              : true,
  override_options    : ['cpp_std=c++17'])
test(
  'tests',
  tests,
  protocol : 'gtest',
  timeout : 120)
