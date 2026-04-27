set_plat('mingw')
add_rules('mode.debug', 'mode.release', 'mode.minsizerel')
set_languages('c11', 'c++17')

includes('dep/catch2')

target('iconv')
  add_cxxflags(
    '-fno-exceptions',
    '-fno-threadsafe-statics',
    '-nostdinc++')
  add_defines(
    'NOMINMAX',
    'WIN32_LEAN_AND_MEAN',
    'WINVER=0x0600',
    '_WIN32_WINNT=0x0600')
  add_files('src/*.cc')
  add_includedirs('include', {public = true})
  add_headerfiles('include/iconv.h')
  set_exceptions('none')
  set_kind('static')

target('test')
  add_deps('iconv', 'catch2')
  add_files('test/*.cc')
  add_ldflags('-static')
  add_tests('default')
  set_kind('binary')
