set_plat('mingw')
add_rules('mode.debug', 'mode.release', 'mode.minsizerel')
set_languages('c11', 'cxx14')

target('intl')
  set_kind('static')
  add_cxxflags(
    '-fno-exceptions',
    '-fno-threadsafe-statics')
  add_includedirs('include', {public = true})
  set_exceptions('none')
  add_defines('NS_NOSTL=intl::stl')
  add_files('src/*.cc')
  add_headerfiles('include/libintl.h')

target('test')
  set_default(false)
  set_kind('binary')
  add_deps('intl')
  add_files('test/*.c')
  add_ldflags('-static')

  add_installfiles('test/zh_CN/*.mo',
    {prefixdir = 'share/locale/zh_CN/LC_MESSAGES'})
