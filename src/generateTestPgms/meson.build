# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: Copyright The WP43 Authors

generateTestPgms = executable(
  'generateTestPgms',
  build_by_default    : false,
  sources             : files('generateTestPgms.c'),
  include_directories : [decNumber_inc, wp43_inc],
  dependencies        : gtk_dep,
  native              : true,
  override_options    : ['b_coverage=false'])
testPgms_bin = custom_target(
  'testPgms.bin',
  build_by_default : false,
  output           : 'testPgms.bin',
  command          : [generateTestPgms, '@OUTPUT@'])
alias_target('testPgms', testPgms_bin)
