set_plat('mingw')
add_rules('mode.release')
set_languages('c11', 'cxx17')

add_requires("catch2")

target('thunk')
  set_kind('static')
  add_cxxflags(
    '-nostdinc++',
    '-fno-exceptions',
    '-fno-threadsafe-statics')
  add_includedirs('include')
  add_defines(
    '__USE_MINGW_ANSI_STDIO=0',
    '_WIN32_WINNT=0x0f00')
  set_exceptions('none')
  if is_arch('i386') then
    add_files(
      'src/5.0/**.cc|**.test.cc',
      'src/5.1/**.cc|**.test.cc',
      'src/5.2/**.cc|**.test.cc')
  end
  if is_arch('i386') or is_arch('x86_64') then
    add_files('src/6.0/**.cc|**.test.cc')
    add_files('src/6.1/**.cc|**.test.cc')
    add_files('src/6.2/**.cc|**.test.cc')
  end

target('test')
  add_packages('catch2')
  add_includedirs('include')
  add_defines(
    'ENABLE_TEST_OVERRIDE',
    '__USE_MINGW_ANSI_STDIO=0',
    '_WIN32_WINNT=0x0f00')
  set_exceptions('none')
  if is_arch('i386') then
    add_files(
      'src/5.0/**.cc',
      'src/5.1/**.cc',
      'src/5.2/**.cc')
  end
  if is_arch('i386') or is_arch('x86_64') then
    add_files('src/6.0/**.cc')
    add_files('src/6.1/**.cc')
    add_files('src/6.2/**.cc')
  end
  add_ldflags('-static')
  add_links('ws2_32')
  add_tests('default')
