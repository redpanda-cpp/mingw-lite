add_cxxflags(
  '-fno-exceptions',
  '-fno-threadsafe-statics',
  '-nostdinc++')
add_rules('mode.release')
add_shflags('-nostdlib')
set_arch('i386')
set_languages('c11', 'c++17')

target('atomic')
  add_files('atomic.cc')
  set_kind('static')

target('atomic-1')
  add_files('atomic.cc')
  set_kind('shared')
