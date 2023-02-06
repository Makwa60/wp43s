# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: Copyright The WP43 Authors

generateLookupTables = executable(
  'generateLookupTables',
  build_by_default    : false,
  sources             : files('generateLookupTables.c') + decNumber_src,
  include_directories : [decNumber_inc, wp43_inc],
  native              : true,
  override_options    : ['b_coverage=false'])
lookupTables_ch = custom_target(
  'lookupTables.ch',
  build_by_default : false,
  output           : ['lookupTables.c', 'lookupTables.h'],
  command          : [generateLookupTables, '@OUTPUT@'])
