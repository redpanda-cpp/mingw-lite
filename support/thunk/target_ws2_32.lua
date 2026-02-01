target('overlay-ws2_32')
  enable_thunk_options()

  if profile_toolchain_or_utf8() then
    if ntddi_version() < ntddi_winxp() then
      add_files(
        'ws2_32/5.1/freeaddrinfo.cc',
        'ws2_32/5.1/getaddrinfo.cc',
        'ws2_32/5.1/getnameinfo.cc')
    end
  end

target('thunk-ws2_32')
  add_files(
    'ws2_32/5.1/freeaddrinfo.cc',
    'ws2_32/5.1/getaddrinfo.cc',
    'ws2_32/5.1/getnameinfo.cc')
  enable_if_x86_32()
  enable_thunk_options()
  merge_win32_alias()
  skip_install()

target('test-ws2_32')
  add_tests('default')
  add_deps('thunk-ws2_32')
  add_files(
    'ws2_32/5.1/getaddrinfo.test.cc',
    'ws2_32/5.1/getnameinfo.test.cc')
  add_linkorders('thunk-ws2_32', 'ws2_32')
  add_links('ws2_32')
  enable_if_x86_32()
  enable_test_options()
  skip_install()
