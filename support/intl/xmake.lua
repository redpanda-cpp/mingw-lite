set_plat('mingw')
add_rules('mode.release')
set_languages('c11', 'cxx17')

target('intl')
  set_kind('static')
  add_cxxflags(
    '-fno-exceptions',
    '-fno-threadsafe-statics')
  add_includedirs('include')
  set_exceptions('none')

  add_files('src/*.cc')

  add_headerfiles('include/libintl.h')
