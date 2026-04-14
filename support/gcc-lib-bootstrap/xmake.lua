add_cxxflags(
  '-fno-exceptions',
  '-fno-threadsafe-statics',
  '-nostdinc++')
add_rules('mode.release')
add_shflags('-nostdlib')
set_languages('c11', 'c++17')

option('exception')
  set_showmenu(true)
  set_values('seh', 'dwarf')

  if is_arch('x86_64', 'aarch64') then
    set_default('seh')
  elseif is_arch('i[34567]86') then
    set_default('dwarf')
  end

target('gcc')
  add_files('chkstk.S')
  set_kind('static')

target('gcc_s')
  add_files('udivmoddi4.cc')
  set_kind('shared')

  if get_config('exception') == 'seh' then
    set_basename('gcc_s_seh-1')
  else
    set_basename('gcc_s_dw2-1')
  end

target('pthread')
  add_files('placeholder.c')
  set_kind('static')
