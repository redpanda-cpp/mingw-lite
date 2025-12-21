target('overlay-advapi32')
  enable_thunk_options()

  if profile_toolchain_or_utf8() then
    if ntddi_version() < ntddi_win2k() then
      add_files('advapi32/5.0/ConvertStringSecurityDescriptorToSecurityDescriptorW.cc')
    end
    if ntddi_version() < ntddi_ws03() then
      add_files('advapi32/5.2/RegDeleteKeyExW.cc')
    end
  end

  if profile_toolchain_utf8() then
    add_files('advapi32/u/RegDeleteKeyExA.cc')
  end

  if profile_toolchain() then
    if ntddi_version() < ntddi_ws03() then
      add_files('advapi32/a/5.2/RegDeleteKeyExA.cc')
    end
  end

target('thunk-advapi32')
  add_files(
    'advapi32/5.0/ConvertStringSecurityDescriptorToSecurityDescriptorW.cc',
    'advapi32/5.2/RegDeleteKeyExW.cc')
  enable_if_x86_32()
  enable_thunk_options()
  merge_win32_alias()
  skip_install()

target('thunk-advapi32-a')
  add_files('advapi32/a/5.2/RegDeleteKeyExA.cc')
  enable_if_x86_32()
  enable_thunk_options()
  merge_win32_alias()
  skip_install()

target('thunk-advapi32-u')
  add_files('advapi32/u/RegDeleteKeyExA.cc')
  enable_thunk_options()
  merge_win32_alias()
  skip_install()
