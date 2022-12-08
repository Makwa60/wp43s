# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: Copyright The WP43 Authors

dmcp_cargs = [
  '-D__weak="__attribute__((weak))"',
  '-D__packed="__attribute__((__packed__))"',
  '-Wno-unused-parameter']
dmcp_linkargs = [
  '--specs=nosys.specs',
  '-Wl,--gc-sections',
  '-Wl,--wrap=_malloc_r',
  '-Wl,-Map=' + meson.current_build_dir() + '/WP43.map,--cref',
  '-T' + meson.current_source_dir() + '/stm32l4.ld']

wp43_dmcp_src = files(
  'hal/audio.c',
  'hal/io.c',
  'hal/system.c',
  'hal/time.c',
  'hal/timer.c',
  'wp43-dmcp.c')
wp43_lib = static_library(
  'libwp43',
  build_by_default    : false,
  sources             : decNumber_src + wp43_src + wp43_dmcp_src +
                        [rasterFontsData_c, softmenuCatalogs_h,
                          constantPointers_ch, vcs_h, version_h],
  c_args              : dmcp_cargs,
  include_directories : [decNumber_inc, wp43_inc, dmcp_inc],
  dependencies        : arm_gmp_dep,
  pic                 : false,
  override_options    : ['debug=false', 'optimization=s'])

# First pass building the ELF to get the size of the QSPI
wp43_pre_elf = executable(
  'WP43_pre.elf',
  build_by_default    : false,
  sources             : dmcp_src + [vcs_h, version_h],
  c_args              : dmcp_cargs,
  link_args           : dmcp_linkargs,
  link_with           : wp43_lib,
  include_directories : [wp43_inc, dmcp_inc],
  pie                 : false,
  override_options    : ['debug=false', 'optimization=s'])
wp43_pre_qspi_incorrect_crc = custom_target(
  'WP43_pre_qspi_incorrect_crc.bin',
  build_by_default : false,
  input            : [wp43_pre_elf],
  output           : 'WP43_pre_qspi_incorrect_crc.bin',
  command          : [arm_objcopy, '--only-section', '.qspi', '-O', 'binary', '@INPUT@', '@OUTPUT@'])
wp43_pre_qspi = custom_target(
  'WP43_pre_qspi.bin',
  build_by_default : false,
  input            : [wp43_pre_qspi_incorrect_crc],
  output           : 'WP43_pre_qspi.bin',
  command          : [bash, modify_crc, forcecrc32, '@INPUT@', '@OUTPUT@'])
generated_qspi_crc_h = custom_target(
  'generated_qspi_crc.h',
  build_by_default : false,
  input            : [wp43_pre_qspi],
  output           : 'generated_qspi_crc.h',
  command          : [bash, gen_qspi_crc, '@INPUT@', '@OUTPUT@'])

# Second pass building the ELF including the correct qspi_crc.h
wp43_elf = executable(
  'WP43.elf',
  build_by_default    : false,
  sources             : dmcp_src + [vcs_h, version_h, generated_qspi_crc_h],
  c_args              : dmcp_cargs + ['-DUSE_GEN_QSPI_CRC'],
  link_args           : dmcp_linkargs,
  link_with           : wp43_lib,
  include_directories : [wp43_inc, dmcp_inc],
  pie                 : false,
  override_options    : ['debug=false', 'optimization=s'])
wp43_flash = custom_target(
  'WP43_flash.bin',
  build_by_default : false,
  input            : [wp43_elf],
  output           : 'WP43_flash.bin',
  command          : [arm_objcopy, '--remove-section', '.qspi', '-O', 'binary', '@INPUT@', '@OUTPUT@'])
wp43_pgm = custom_target(
  'WP43.pgm',
  build_by_default : false,
  input            : [wp43_flash],
  output           : 'WP43.pgm',
  command          : [bash, add_pgm_chsum, '@INPUT@', '@OUTPUT@'])
wp43_qspi_incorrect_crc = custom_target(
  'WP43_qspi_incorrect_crc.bin',
  build_by_default : false,
  input            : [wp43_elf],
  output           : 'WP43_qspi_incorrect_crc.bin',
  command          : [arm_objcopy, '--only-section', '.qspi', '-O', 'binary', '@INPUT@', '@OUTPUT@'])
wp43_qspi = custom_target(
  'WP43_qspi.bin',
  build_by_default : false,
  input            : [wp43_qspi_incorrect_crc],
  output           : 'WP43_qspi.bin',
  command          : [bash, modify_crc, forcecrc32, '@INPUT@', '@OUTPUT@'])

alias_target('dmcp', wp43_pgm, wp43_qspi)

# Build keymap.bin for DMCP
keymap = custom_target(
  'keymap.bin',
  build_by_default : false,
  input            : files('keymap.txt'),
  output           : 'keymap.bin',
  command          : [bash, mk_keymap, 'WP43', '@INPUT@'])

alias_target('keymap', keymap)
