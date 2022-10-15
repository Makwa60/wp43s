# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: Copyright The WP43 Authors

decNumber_src = files(
  'decNumberICU/decContext.c',
  'decNumberICU/decDouble.c',
  'decNumberICU/decimal128.c',
  'decNumberICU/decimal64.c',
  'decNumberICU/decNumber.c',
  'decNumberICU/decQuad.c')
decNumber_inc = include_directories('decNumberICU')

dmcp_src = files(
  'DMCP_SDK/dmcp/sys/pgm_syscalls.c',
  'DMCP_SDK/dmcp/startup_pgm.s')
dmcp_inc = include_directories('DMCP_SDK/dmcp')

mk_keymap = files('DMCP_SDK/keymap_utils/bin/mk_keymap')[0]

forcecrc32 = executable(
  'forcecrc32',
  build_by_default : false,
  sources          : files('forcecrc32.c'),
  native           : true)
