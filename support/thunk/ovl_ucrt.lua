function ucrt_utf8_files()
  return {
    'ucrt/environment/__p__environ.cc',
    'ucrt/environment/_putenv.cc',
    'ucrt/environment/_wputenv.cc',
    'ucrt/environment/getenv.cc',
    'ucrt/environment/putenv.cc',
    'ucrt/filesystem/_chmod.cc',
    'ucrt/filesystem/_findfirst.cc',
    'ucrt/filesystem/_findfirst32.cc',
    'ucrt/filesystem/_findfirst32i64.cc',
    'ucrt/filesystem/_findfirst64.cc',
    'ucrt/filesystem/_findfirst64i32.cc',
    'ucrt/filesystem/_findfirsti64.cc',
    'ucrt/filesystem/_findnext.cc',
    'ucrt/filesystem/_findnext32.cc',
    'ucrt/filesystem/_findnext32i64.cc',
    'ucrt/filesystem/_findnext64.cc',
    'ucrt/filesystem/_findnext64i32.cc',
    'ucrt/filesystem/_findnexti64.cc',
    'ucrt/filesystem/_fullpath.cc',
    'ucrt/filesystem/_stat.cc',
    'ucrt/filesystem/_stat32.cc',
    'ucrt/filesystem/_stat32i64.cc',
    'ucrt/filesystem/_stat64.cc',
    'ucrt/filesystem/_stat64i32.cc',
    'ucrt/filesystem/_stati64.cc',
    'ucrt/filesystem/_unlink.cc',
    'ucrt/filesystem/remove.cc',
    'ucrt/filesystem/rename.cc',
    'ucrt/filesystem/stat.cc',
    'ucrt/filesystem/stat32.cc',
    'ucrt/filesystem/stat32i64.cc',
    'ucrt/filesystem/stat64.cc',
    'ucrt/filesystem/stat64i32.cc',
    'ucrt/filesystem/unlink.cc',
    'ucrt/runtime/__p___argv.cc',
    'ucrt/stdio/__stdio_common_vfprintf.cc',
    'ucrt/stdio/__stdio_common_vfscanf.cc',
    'ucrt/stdio/_getcwd.cc',
    'ucrt/stdio/_open.cc',
    'ucrt/stdio/_read.cc',
    'ucrt/stdio/_write.cc',
    'ucrt/stdio/fflush.cc',
    'ucrt/stdio/fgetc.cc',
    'ucrt/stdio/fgets.cc',
    'ucrt/stdio/fopen.cc',
    'ucrt/stdio/fputc.cc',
    'ucrt/stdio/fputs.cc',
    'ucrt/stdio/fread.cc',
    'ucrt/stdio/fwrite.cc',
    'ucrt/stdio/getc.cc',
    'ucrt/stdio/getchar.cc',
    'ucrt/stdio/getcwd.cc',
    'ucrt/stdio/open.cc',
    'ucrt/stdio/putc.cc',
    'ucrt/stdio/puts.cc',
    'ucrt/stdio/read.cc',
    'ucrt/stdio/ungetc.cc',
    'ucrt/stdio/write.cc',
  }
end

function ucrt_utf8_startup_deps()
  -- symbols that are referenced by startup object
  -- they should be explicitly added to test target to be chosen
  -- also they depend on some internal declarations not available in old versions
  return {
    'ucrt/environment/__p__environ.cc',
    'ucrt/runtime/__p___argv.cc',
    'ucrt/runtime/_configure_narrow_argv.cc',
    'ucrt/runtime/_initialize_narrow_environment.cc',
  }
end

function ucrt_def_files()
  local result = {
    'def/api-ms-win-crt-environment-l1-1-0.def',
    'def/api-ms-win-crt-runtime-l1-1-0.def',
    'def/api-ms-win-crt-stdio-l1-1-0.def',
  }

  if is_arch('i386', 'i686') then
    table.insert(result,
      'def/lib32/api-ms-win-crt-filesystem-l1-1-0.def')
  else
    table.insert(result,
      'def/api-ms-win-crt-filesystem-l1-1-0.def')
  end

  return result
end

function add_ucrt_test_links(thunk)
  add_linkgroups(
    'catch2', 'stdc++', 'pthread',
    'mingw32', 'gcc', 'mingwex',
    thunk, 'ucrt')
end

target('alias-short-ucrt')
  on_build(build_short_import_library(ucrt_def_files()))
  set_enabled(has_config('short-alias'))
  set_kind('static')

target('overlay-ucrt')
  add_defines('_UCRT')
  enable_thunk_options()

  if profile_toolchain_utf8() then
    add_deps('utf8-musl.a')
    add_files(table.unpack(ucrt_utf8_files()))
    add_files(table.unpack(ucrt_utf8_startup_deps()))
    set_policy('build.merge_archive', true)
  end

target('alias-short-utf8-ucrt')
  on_build(build_short_import_library(ucrt_def_files()))
  set_enabled(has_config('short-alias') and has_config('u8crt'))
  set_kind('static')

target('overlay-utf8-ucrt')
  add_defines('_UCRT')
  add_files(table.unpack(ucrt_utf8_files()))
  add_files(table.unpack(ucrt_utf8_startup_deps()))
  enable_thunk_options()
  set_enabled(has_config('u8crt'))

target('alias-long-ucrt')
  on_build(build_long_import_library(ucrt_def_files()))
  set_kind('static')
  skip_install()

target('thunk-ucrt-u')
  add_defines('_UCRT')
  add_deps('alias-long-ucrt', 'utf8-musl.a')
  add_files(table.unpack(ucrt_utf8_files()))
  enable_thunk_options()
  merge_win32_alias()
  skip_install()

target('test-ucrt-u')
  add_deps('thunk-ucrt-u')
  add_files(
    'ucrt/stdio/_open.test.cc',
    'ucrt/stdio/fopen.test.cc')
  add_tests('default')
  add_ucrt_test_links('thunk-ucrt-u')
  enable_test_options()
  skip_install()

target('console-ucrt')
  add_cxflags('-fno-builtin')
  add_defines('_UCRT')
  add_deps('thunk-ucrt-u')
  add_files('test/console.c')
  add_ucrt_test_links('thunk-ucrt-u')
  enable_test_options()
  skip_install()

target('argv-ucrt')
  add_cxxflags('-nostdinc++')
  add_defines('_UCRT')
  add_deps('thunk-ucrt-u')
  add_files('test/argv.c')
  add_files(table.unpack(ucrt_utf8_startup_deps()))
  add_tests('default', {
    runargs = {"你好", "世界"},
    pass_outputs = "argv[1] = 你好\nargv[2] = 世界\n",
    plain = true,
  })
  add_ucrt_test_links('thunk-ucrt-u')
  enable_test_options()
  skip_install()
