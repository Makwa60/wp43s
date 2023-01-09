# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: Copyright The WP43 Authors

project('wp43', 'c', 'cpp',
  meson_version   : '>=0.56.0',
  license         : 'GPL-3.0-only',
  default_options : ['warning_level=2',
                     'b_ndebug=if-release']
)

# Simulator and generate tools arguments
add_project_arguments('-DPC_BUILD', native : true, language : ['c', 'cpp'])
if build_machine.system() == 'windows'
  add_project_arguments('-DWIN32',  native : true, language : ['c', 'cpp'])
elif build_machine.system() == 'darwin'
  add_project_arguments('-DOSX',    native : true, language : ['c', 'cpp'])
elif build_machine.system() == 'linux'
  add_project_arguments('-DLINUX',  native : true, language : ['c', 'cpp'])
else
  error('Unsupported platform')
endif

if build_machine.cpu_family() in ['aarch64', 'x86_64']
  add_project_arguments('-DOS64BIT', native : true, language : ['c', 'cpp'])
elif build_machine.cpu_family() in ['arm', 'x86']
  add_project_arguments('-DOS32BIT', native : true, language : ['c', 'cpp'])
else
  error('Unsupported platform')
endif

if get_option('RASPBERRY')
  add_project_arguments('-DRASPBERRY', native : true, language : ['c', 'cpp'])
endif

# DMCP arguments
add_project_arguments('-DDMCP_BUILD', native : false, language : ['c', 'cpp'])
add_project_arguments('-DOS32BIT',    native : false, language : ['c', 'cpp'])
if get_option('DEBUG_POWER')
  add_project_arguments('-DDEBUG_POWER', native : false, language : ['c', 'cpp'])
endif

git = find_program('git')
run_command(git, 'submodule', 'update', '--init', '--recursive', meson.project_source_root(), check: true)

cc = meson.get_compiler('c')
ft_dep  = dependency('freetype2', native : true)
gtk_dep = dependency('gtk+-3.0',  native : true)
gmp_dep = dependency('gmp',       native : true)
m_dep   = cc.find_library('m', required : false)

if (not meson.is_cross_build()) and (host_machine.system() == 'linux' or host_machine.system() == 'darwin')
  pulse_dep = dependency('libpulse-simple', native : true, required : false)
  if pulse_dep.found()
    add_project_arguments('-DWITH_PULSEAUDIO', native : true, language : ['c', 'cpp'])
  else
    message('libpulse-simple not found so BEEP and TONE are disabled')
  endif
else
  pulse_dep = dependency('', required : false)
endif

conf_data = configuration_data()
commit_tag = get_option('CI_COMMIT_TAG')
today = run_command('date', '+%Y-%m-%d', check: true)
conf_data.set('TODAY', today.stdout().strip())
if commit_tag != ''
  conf_data.set('CI_USE_TAG',    true)
  conf_data.set('CI_COMMIT_TAG', commit_tag)
else
  conf_data.set('CI_COMMIT_TAG', '')
endif

if not meson.is_cross_build()
  gtest_proj     = subproject('gtest')
  gtest_main_dep = gtest_proj.get_variable('gtest_main_dep')
else
  gtest_main_dep = dependency('', required : false)
endif

subdir('docs/code')
subdir('dep')
subdir('src/wp43')
subdir('src/ttf2RasterFonts')
subdir('src/generateConstants')
subdir('src/generateCatalogs')
subdir('src/generateTestPgms')
subdir('src/tests')
subdir('src/testSuite')
subdir('src/wp43-gtk')

# Only attempt to include the DMCP target if we have set-up the cross compiler
if meson.is_cross_build()
  add_pgm_chsum = files('tools/add_pgm_chsum')[0]
  modify_crc    = files('tools/modify_crc')[0]
  gen_qspi_crc  = files('tools/gen_qspi_crc')[0]
  arm_objcopy   = find_program('arm-none-eabi-objcopy')
  bash          = find_program('bash')
  subproject('gmp-6.2.1')
  arm_gmp_dep   = dependency('arm-gmp-6.2.1', native : false)
  subdir('src/wp43-dmcp')
endif
