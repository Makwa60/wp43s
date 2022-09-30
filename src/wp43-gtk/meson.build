wp43_gtk_src = files(
  'wp43-gtk.c',
  'gtkGui.c',
  'gtkScreen.c',
  'hal/audio.c',
  'hal/gui.c',
  'hal/io.c',
  'hal/lcd.c',
  'hal/system.c',
  'hal/time.c')

override_opts = []
cflags        = []
if get_option('buildtype').startswith('release')
  override_opts = ['b_lto=true']
else
  cflags        = ['-D_FORTIFY_SOURCE=2', '-fstack-protector-strong']
endif

sim = executable(
  'wp43',
  sources             : decNumber_src + wp43_src + wp43_gtk_src +
                        [rasterFontsData_c, softmenuCatalogs_h,
                          constantPointers_ch, vcs_h, version_h],
  c_args              : cflags,
  c_pch               : 'pch/wp43-gtk_pch.h',
  include_directories : [decNumber_inc, wp43_inc],
  dependencies        : [gtk_dep, gmp_dep, m_dep, pulse_dep],
  native              : true,
  override_options    : override_opts,
  win_subsystem       : 'windows')
alias_target('sim', sim)
