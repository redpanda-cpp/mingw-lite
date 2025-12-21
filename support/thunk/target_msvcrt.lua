target('alias-short-msvcrt-os')
  set_kind('static')

  if is_arch('i386', 'i686') then
    on_build(build_short_import_library('def/lib32/msvcrt.def'))
  else
    on_build(build_short_import_library('def/msvcrt.def'))
  end

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
        'msvcrt/w/_wremove.cc',
        'msvcrt/w/_wrename.cc',
        'msvcrt/w/_wstat32.cc',
        'msvcrt/w/_wstat32i64.cc',
        'msvcrt/w/_wstati64.cc',
        'msvcrt/w/_wunlink.cc',
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

  if profile_toolchain_utf8() then
    add_files(
      'msvcrt/u/environment/__initenv.cc',
      'msvcrt/u/environment/__p__environ.cc',
      'msvcrt/u/environment/_putenv.cc',
      'msvcrt/u/environment/getenv.cc',
      'msvcrt/u/environment/putenv.cc',
      'msvcrt/u/filesystem/_chmod.cc',
      'msvcrt/u/filesystem/_findfirst.cc',
      'msvcrt/u/filesystem/_findfirst32.cc',
      'msvcrt/u/filesystem/_findfirst32i64.cc',
      'msvcrt/u/filesystem/_findfirst64.cc',
      'msvcrt/u/filesystem/_findfirst64i32.cc',
      'msvcrt/u/filesystem/_findfirsti64.cc',
      'msvcrt/u/filesystem/_findnext.cc',
      'msvcrt/u/filesystem/_findnext32.cc',
      'msvcrt/u/filesystem/_findnext32i64.cc',
      'msvcrt/u/filesystem/_findnext64.cc',
      'msvcrt/u/filesystem/_findnext64i32.cc',
      'msvcrt/u/filesystem/_findnexti64.cc',
      'msvcrt/u/filesystem/_fullpath.cc',
      'msvcrt/u/filesystem/_stat.cc',
      'msvcrt/u/filesystem/_stat32.cc',
      'msvcrt/u/filesystem/_stat32i64.cc',
      'msvcrt/u/filesystem/_stat64.cc',
      'msvcrt/u/filesystem/_stat64i32.cc',
      'msvcrt/u/filesystem/_stati64.cc',
      'msvcrt/u/filesystem/_unlink.cc',
      'msvcrt/u/filesystem/remove.cc',
      'msvcrt/u/filesystem/rename.cc',
      'msvcrt/u/filesystem/unlink.cc',
      'msvcrt/u/runtime/__getmainargs.cc',
      'msvcrt/u/runtime/__p___argv.cc',
      'msvcrt/u/stdio/@console_buffer.cc',
      'msvcrt/u/stdio/_getcwd.cc',
      'msvcrt/u/stdio/_open.cc',
      'msvcrt/u/stdio/fflush.cc',
      'msvcrt/u/stdio/fopen.cc',
      'msvcrt/u/stdio/fputc.cc',
      'msvcrt/u/stdio/fputs.cc',
      'msvcrt/u/stdio/getcwd.cc',
      'msvcrt/u/stdio/open.cc',
      'msvcrt/u/stdio/putc.cc',
      'msvcrt/u/stdio/puts.cc',
      'msvcrt/u/time/_ctime64.cc')
  end

  if profile_toolchain() then
    if ntddi_version() < ntddi_win2k() then
      add_msvcrt_sources(msvcrt_thunk_toolchain_a_5_0(), 'msvcrt/a/5.0')
    end
  end

target('alias-long-msvcrt')
  set_kind('static')
  skip_install()

  if is_arch('i386', 'i686') then
    on_build(build_long_import_library('def/lib32/msvcrt.def'))
  else
    on_build(build_long_import_library('def/msvcrt.def'))
  end

target('thunk-msvcrt')
  add_defines('__MSVCRT_VERSION__=0x0600')
  add_deps('alias-long-msvcrt')
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
      'msvcrt/w/_wunlink.cc',
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

target('thunk-msvcrt-u')
  add_defines('__MSVCRT_VERSION__=0x0600')
  add_deps('alias-long-msvcrt')
  add_files(
    'msvcrt/u/environment/__p__environ.cc',
    'msvcrt/u/environment/_putenv.cc',
    'msvcrt/u/environment/getenv.cc',
    'msvcrt/u/environment/putenv.cc',
    'msvcrt/u/filesystem/_chmod.cc',
    'msvcrt/u/filesystem/_findfirst.cc',
    'msvcrt/u/filesystem/_findfirst32.cc',
    'msvcrt/u/filesystem/_findfirst32i64.cc',
    'msvcrt/u/filesystem/_findfirst64.cc',
    'msvcrt/u/filesystem/_findfirst64i32.cc',
    'msvcrt/u/filesystem/_findfirsti64.cc',
    'msvcrt/u/filesystem/_findnext.cc',
    'msvcrt/u/filesystem/_findnext32.cc',
    'msvcrt/u/filesystem/_findnext32i64.cc',
    'msvcrt/u/filesystem/_findnext64.cc',
    'msvcrt/u/filesystem/_findnext64i32.cc',
    'msvcrt/u/filesystem/_findnexti64.cc',
    'msvcrt/u/filesystem/_fullpath.cc',
    'msvcrt/u/filesystem/_stat.cc',
    'msvcrt/u/filesystem/_stat32.cc',
    'msvcrt/u/filesystem/_stat32i64.cc',
    'msvcrt/u/filesystem/_stat64.cc',
    'msvcrt/u/filesystem/_stat64i32.cc',
    'msvcrt/u/filesystem/_stati64.cc',
    'msvcrt/u/filesystem/_unlink.cc',
    'msvcrt/u/filesystem/remove.cc',
    'msvcrt/u/filesystem/rename.cc',
    'msvcrt/u/filesystem/unlink.cc',
    'msvcrt/u/runtime/__p___argv.cc',
    'msvcrt/u/stdio/@console_buffer.cc',
    'msvcrt/u/stdio/_getcwd.cc',
    'msvcrt/u/stdio/_open.cc',
    'msvcrt/u/stdio/fflush.cc',
    'msvcrt/u/stdio/fopen.cc',
    'msvcrt/u/stdio/fputc.cc',
    'msvcrt/u/stdio/fputs.cc',
    'msvcrt/u/stdio/getcwd.cc',
    'msvcrt/u/stdio/open.cc',
    'msvcrt/u/stdio/putc.cc',
    'msvcrt/u/stdio/puts.cc',
    'msvcrt/u/time/_ctime64.cc')
  enable_thunk_options()
  merge_win32_alias()
  skip_install()

target('test-msvcrt-u')
  add_tests('default')
  add_deps('thunk-msvcrt-u')
  add_files(
    'msvcrt/u/stdio/_open.test.cc',
    'msvcrt/u/stdio/fopen.test.cc')
  add_linkorders('thunk-msvcrt-u', 'msvcrt-os')
  add_links('msvcrt-os')
  enable_test_options()
  skip_install()

target('console-msvcrt-u')
  add_defines('__MSVCRT_VERSION__=0x0600')
  add_deps('thunk-msvcrt-u')
  add_files('test/console-u.c')
  add_linkorders('thunk-msvcrt-u', 'msvcrt-os')
  add_links('msvcrt-os')
  enable_test_options()
  skip_install()

target('argv-msvcrt')
  add_cxxflags('-nostdinc++')
  add_defines('__MSVCRT_VERSION__=0x0600')
  add_deps('thunk-msvcrt-u')
  add_files(
    'test/argv.c',
    'msvcrt/u/environment/__initenv.cc',
    'msvcrt/u/environment/__p__environ.cc',
    'msvcrt/u/runtime/__getmainargs.cc')
  add_links('msvcrt-os')
  add_tests('default', {
    runargs = {"你好", "世界"},
    pass_outputs = "argv[1] = 你好\nargv[2] = 世界\n",
    plain = true,
  })
  enable_test_options()
  skip_install()
