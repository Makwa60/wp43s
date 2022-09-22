wp43_gtk_src = files(
  'wp43-gtk.c',
  'gtkGui.c',
  'gtkScreen.c',
  'hal/audio.c',
  'hal/gui.c',
  'hal/lcd.c')
sim = executable(
  'wp43',
  sources             : decNumber_src + wp43_src + wp43_gtk_src +
                        [rasterFontsData_c, softmenuCatalogs_h,
                          constantPointers_ch, vcs_h, version_h],
  c_pch               : 'pch/wp43-gtk_pch.h',
  include_directories : [decNumber_inc, wp43_inc],
  dependencies        : [gtk_dep, gmp_dep, m_dep, pulse_dep],
  native              : true,
  override_options    : ['b_lto=true'],
  win_subsystem       : 'windows')
alias_target('sim', sim)
