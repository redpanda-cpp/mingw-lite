set_plat('mingw')
add_rules('mode.release')
set_languages('c11', 'cxx17')

includes('dep/catch2')

target('thunk')
  set_kind('static')
  add_cxxflags(
    '-nostdinc++',
    '-fno-exceptions',
    '-fno-threadsafe-statics')
  add_includedirs('include')
  add_defines(
    'NOSTL_NOCRT',
    'NS_NOSTL=mingw_thunk::stl',
    'NS_NOCRT=mingw_thunk::libc',
    '__USE_MINGW_ANSI_STDIO=0',
    '_WIN32_WINNT=0x0f00')
  add_defines()
  set_exceptions('none')
  if is_arch('i386') then
    add_files(
      'win32/3.9999+4.10/**.cc|**.test.cc',
      'win32/4.0/**.cc|**.test.cc',
      'win32/5.0/**.cc|**.test.cc',
      'win32/5.1/**.cc|**.test.cc',
      'win32/5.2/**.cc|**.test.cc')
    add_files(
      'msvcrt_/4.0/*.cc|*.test.cc',
      'msvcrt_/5.0/*.cc|*.test.cc',
      'msvcrt_/5.1/*.cc|*.test.cc')
  end
  if is_arch('i386') or is_arch('x86_64') then
    add_files(
      'win32/6.0/**.cc|**.test.cc',
      'win32/6.1/**.cc|**.test.cc',
      'win32/6.2/**.cc|**.test.cc')
    add_files('msvcrt_/6.0/*.cc|*.test.cc')
  end

target('test')
  add_deps('catch2')
  add_includedirs('include')
  add_defines(
    'ENABLE_TEST_OVERRIDE',
    'NOSTL_NOCRT',
    'NS_NOSTL=mingw_thunk::stl',
    'NS_NOCRT=mingw_thunk::libc',
    '__USE_MINGW_ANSI_STDIO=0',
    '_WIN32_WINNT=0x0f00')
  set_exceptions('none')
  if is_arch('i386') then
    add_files(
      'win32/3.9999+4.10/**.cc',
      'win32/4.0/**.cc',
      'win32/5.0/**.cc',
      'win32/5.1/**.cc',
      'win32/5.2/**.cc')
    add_files(
      'msvcrt_/4.0/*.cc',
      'msvcrt_/5.0/*.cc',
      'msvcrt_/5.1/*.cc')
  end
  if is_arch('i386') or is_arch('x86_64') then
    add_files(
      'win32/6.0/**.cc',
      'win32/6.1/**.cc',
      'win32/6.2/**.cc')
    add_files('msvcrt_/6.0/*.cc')
  end
  add_ldflags('-static')
  add_links('ws2_32')
  add_tests('default')
