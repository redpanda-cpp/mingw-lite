set_plat('mingw')
add_rules('mode.debug', 'mode.release', 'mode.minsizerel')
set_languages('c11', 'c++17')

option('nested-ns')
  set_default(true)

target('intl')
  set_kind('static')
  add_cxxflags(
    '-fno-exceptions',
    '-fno-threadsafe-statics')
  add_includedirs('include', {public = true})
  set_exceptions('none')
  if has_config('nested-ns') then
    add_defines('NS_NOSTL=intl::stl')
  else
    add_defines('NS_NOSTL=intl_stl')
    add_defines('stl=intl_stl')
  end
  add_defines(
    'WINVER=0x0600',
    '_WIN32_WINNT=0x0600')
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
