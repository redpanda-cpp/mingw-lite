target('alias-short-shell32')
  enable_if_x86_32()
  set_kind('static')
    on_build(build_short_import_library({'def/lib32/shell32.def'}))

target('overlay-shell32')
  enable_thunk_options()

  if profile_toolchain_or_utf8() then
    if ntddi_version() < ntddi_win4() then
      add_files(
        'shell32/w/CommandLineToArgvW.cc',
        'shell32/w/SHGetPathFromIDListW.cc')
    end
  end

target('alias-long-shell32')
  enable_if_x86_32()
  set_kind('static')
    on_build(build_long_import_library({'def/lib32/shell32.def'}))

target('thunk-shell32')
  add_deps('alias-long-shell32')
  add_files(
    'shell32/w/CommandLineToArgvW.cc',
    'shell32/w/SHGetPathFromIDListW.cc')
  enable_if_x86_32()
  enable_thunk_options()
  merge_win32_alias()
  skip_install()

target('test-shell32')
  add_tests('default')
  add_deps('thunk-shell32')
  add_files('shell32/w/CommandLineToArgvW.test.cc')
  enable_if_x86_32()
  enable_test_options()
  skip_install()
