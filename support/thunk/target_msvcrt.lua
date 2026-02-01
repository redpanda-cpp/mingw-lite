target('alias-short-msvcrt-os')
  enable_if_x86_32()
  on_build(build_short_import_library('def/lib32/msvcrt.def'))
  set_kind('static')

function until_mingw(version)
  return function(target_version)
    return target_version < version
  end
end

function msvcrt_thunk_core_4_0()
  return {
    {'_beginthreadex'},
  }
end

function msvcrt_thunk_core_5_0()
  return {
    {'_fstat64', v = until_mingw(13)},
    {'_wstat64', v = until_mingw(13)},
  }
end

function msvcrt_thunk_core_5_1()
  return {
    {'_aligned_free', v = until_mingw(13)},
    {'_aligned_malloc', v = until_mingw(13)},
  }
end

function msvcrt_thunk_toolchain_4_0()
  return {
    {'_beginthreadex'},
  }
end

function msvcrt_thunk_toolchain_5_0()
  return {
    {'_fstat64', v = until_mingw(13)},
    {'_futime64'},
    {'_gmtime64', v = until_mingw(14)},
    {'_localtime64'},
    {'_time64', v = until_mingw(13)},
    {'_wfindfirst64'},
    {'_wfindnext64'},
    {'_wstat64', v = until_mingw(13)},
    {'_wutime64'},
  }
end

function msvcrt_thunk_toolchain_5_1()
  return {
    {'_aligned_free', v = until_mingw(13)},
    {'_aligned_malloc', v = until_mingw(13)},
  }
end

function msvcrt_thunk_toolchain_6_0()
  return {
    {'_wputenv_s'},
    {'wcscat_s'},
    {'wcscpy_s'},
    {'wcsncat_s'},
    {'wcsncpy_s'},
    {'wcstok_s', v = until_mingw(13)},
  }
end

function msvcrt_thunk_toolchain_a_5_0()
  return {
    {'_ctime64'},
    {'_findfirst64'},
    {'_findnext64'},
    {'_stat64', v = until_mingw(13)},
    {'_utime64'},
  }
end

function add_msvcrt_sources(functions, prefix)
  local target_version = tonumber(get_config('mingw-version'))
  for _, entry in ipairs(functions) do
    local func_name = entry[1]
    local condition = entry.v

    if condition == nil or condition(target_version) then
      add_files(prefix .. '/' .. func_name .. '.cc')
    end
  end
end

target('overlay-msvcrt-os')
  add_defines('__MSVCRT_VERSION__=0x0600')
  enable_thunk_options()

  if profile_core() then
    if ntddi_version() < ntddi_win4() then
      add_msvcrt_sources(msvcrt_thunk_core_4_0(), 'msvcrt/4.0')
      add_files(
        'msvcrt/w/_wchdir.cc',
        'msvcrt/w/_wchmod.cc',
        'msvcrt/w/_wfindfirst.cc',
        'msvcrt/w/_wfindfirst32.cc',
        'msvcrt/w/_wfindfirst32i64.cc',
        'msvcrt/w/_wfindfirsti64.cc',
        'msvcrt/w/_wfindnext.cc',
        'msvcrt/w/_wfindnext32.cc',
        'msvcrt/w/_wfindnext32i64.cc',
        'msvcrt/w/_wfindnexti64.cc',
        'msvcrt/w/_wfopen.cc',
        'msvcrt/w/_wfullpath.cc',
        'msvcrt/w/_wgetcwd.cc',
        'msvcrt/w/_wmkdir.cc',
        'msvcrt/w/_wopen.cc',
        'msvcrt/w/_wstat32.cc',
        'msvcrt/w/_wstat32i64.cc',
        'msvcrt/w/_wstati64.cc',
        'msvcrt/w/_wutime.cc',
        'msvcrt/w/_wutime32.cc')
    end
    if ntddi_version() < ntddi_win2k() then
      add_msvcrt_sources(msvcrt_thunk_core_5_0(), 'msvcrt/5.0')
    end
    if ntddi_version() < ntddi_winxp() then
      add_msvcrt_sources(msvcrt_thunk_core_5_1(), 'msvcrt/5.1')
    end
  end

  if profile_toolchain_or_utf8() then
    if ntddi_version() < ntddi_win4() then
      add_msvcrt_sources(msvcrt_thunk_toolchain_4_0(), 'msvcrt/4.0')
      add_files(
        'msvcrt/w/_wchdir.cc',
        'msvcrt/w/_wchmod.cc',
        'msvcrt/w/_wfindfirst.cc',
        'msvcrt/w/_wfindfirst32.cc',
        'msvcrt/w/_wfindfirst32i64.cc',
        'msvcrt/w/_wfindfirsti64.cc',
        'msvcrt/w/_wfindnext.cc',
        'msvcrt/w/_wfindnext32.cc',
        'msvcrt/w/_wfindnext32i64.cc',
        'msvcrt/w/_wfindnexti64.cc',
        'msvcrt/w/_wfopen.cc',
        'msvcrt/w/_wfullpath.cc',
        'msvcrt/w/_wgetcwd.cc',
        'msvcrt/w/_wmkdir.cc',
        'msvcrt/w/_wopen.cc',
        'msvcrt/w/_wstat32.cc',
        'msvcrt/w/_wstat32i64.cc',
        'msvcrt/w/_wstati64.cc',
        'msvcrt/w/_wutime.cc',
        'msvcrt/w/_wutime32.cc')
    end
    if ntddi_version() < ntddi_win2k() then
      add_msvcrt_sources(msvcrt_thunk_toolchain_5_0(), 'msvcrt/5.0')
    end
    if ntddi_version() < ntddi_winxp() then
      add_msvcrt_sources(msvcrt_thunk_toolchain_5_1(), 'msvcrt/5.1')
    end
    if ntddi_version() < ntddi_vista() then
      add_msvcrt_sources(msvcrt_thunk_toolchain_6_0(), 'msvcrt/6.0')
    end
  end

  if profile_toolchain() then
    if ntddi_version() < ntddi_win2k() then
      add_msvcrt_sources(msvcrt_thunk_toolchain_a_5_0(), 'msvcrt/a/5.0')
    end
  end

target('alias-long-msvcrt')
  enable_if_x86_32()
  on_build(build_long_import_library('def/lib32/msvcrt.def'))
  set_kind('static')
  skip_install()

target('thunk-msvcrt')
  add_defines('__MSVCRT_VERSION__=0x0600')
  add_files(
    'msvcrt/6.0/_wputenv_s.cc',
    'msvcrt/6.0/wcscat_s.cc',
    'msvcrt/6.0/wcscpy_s.cc',
    'msvcrt/6.0/wcsncat_s.cc',
    'msvcrt/6.0/wcsncpy_s.cc',
    'msvcrt/6.0/wcstok_s.cc')
  enable_if_x86()
  enable_thunk_options()
  merge_win32_alias()
  skip_install()

  if is_arch('i386', 'i686') then
    add_deps('alias-long-msvcrt')
    add_files(
      'msvcrt/4.0/_beginthreadex.cc',
      'msvcrt/5.0/_fstat64.cc',
      'msvcrt/5.0/_futime64.cc',
      'msvcrt/5.0/_gmtime64.cc',
      'msvcrt/5.0/_localtime64.cc',
      'msvcrt/5.0/_time64.cc',
      'msvcrt/5.0/_wfindfirst64.cc',
      'msvcrt/5.0/_wfindnext64.cc',
      'msvcrt/5.0/_wstat64.cc',
      'msvcrt/5.0/_wutime64.cc',
      'msvcrt/5.0/fstat64.cc',
      'msvcrt/5.1/_aligned_free.cc',
      'msvcrt/5.1/_aligned_malloc.cc',
      'msvcrt/w/_wchdir.cc',
      'msvcrt/w/_wchmod.cc',
      'msvcrt/w/_wfindfirst.cc',
      'msvcrt/w/_wfindfirst32.cc',
      'msvcrt/w/_wfindfirst32i64.cc',
      'msvcrt/w/_wfindfirsti64.cc',
      'msvcrt/w/_wfindnext.cc',
      'msvcrt/w/_wfindnext32.cc',
      'msvcrt/w/_wfindnext32i64.cc',
      'msvcrt/w/_wfindnexti64.cc',
      'msvcrt/w/_wfopen.cc',
      'msvcrt/w/_wfullpath.cc',
      'msvcrt/w/_wgetcwd.cc',
      'msvcrt/w/_wmkdir.cc',
      'msvcrt/w/_wopen.cc',
      'msvcrt/w/_wstat32.cc',
      'msvcrt/w/_wstat32i64.cc',
      'msvcrt/w/_wstati64.cc',
      'msvcrt/w/_wutime.cc',
      'msvcrt/w/_wutime32.cc')
  end

target('test-msvcrt')
  add_tests('default')
  add_deps('thunk-msvcrt')
  add_files(
    'msvcrt/6.0/_wputenv_s.test.cc',
    'msvcrt/6.0/wcscat_s.test.cc',
    'msvcrt/6.0/wcscpy_s.test.cc',
    'msvcrt/6.0/wcsncat_s.test.cc',
    'msvcrt/6.0/wcsncpy_s.test.cc')
  add_linkorders('thunk-msvcrt', 'msvcrt-os')
  add_links('msvcrt-os')
  enable_if_x86()
  enable_test_options()
  skip_install()

  if is_arch('i386', 'i686') then
    add_files(
      'msvcrt/5.0/_futime64.test.cc',
      'msvcrt/5.0/_gmtime64.test.cc',
      'msvcrt/5.0/_localtime64.test.cc',
      'msvcrt/5.0/_wfindfirst64.test.cc',
      'msvcrt/5.0/_wutime64.test.cc',
      'msvcrt/5.1/_aligned_malloc.test.cc',
      'msvcrt/w/_wfopen.test.cc')
  end

target('thunk-msvcrt-a')
  add_defines('__MSVCRT_VERSION__=0x0600')
  add_deps('alias-long-msvcrt')
  add_files(
    'msvcrt/a/5.0/_ctime64.cc',
    'msvcrt/a/5.0/_findfirst64.cc',
    'msvcrt/a/5.0/_findnext64.cc',
    'msvcrt/a/5.0/_stat64.cc',
    'msvcrt/a/5.0/_utime64.cc')
  enable_if_x86_32()
  enable_thunk_options()
  merge_win32_alias()
  skip_install()

target('test-msvcrt-a')
  add_tests('default')
  add_deps('thunk-msvcrt-a')
  add_files(
    'msvcrt/a/5.0/_findfirst64.test.cc',
    'msvcrt/a/5.0/_utime64.test.cc')
  add_linkorders('thunk-msvcrt-a', 'msvcrt-os')
  add_links('msvcrt-os')
  enable_if_x86_32()
  enable_test_options()
  skip_install()
