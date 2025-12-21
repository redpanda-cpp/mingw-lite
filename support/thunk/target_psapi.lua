target('overlay-psapi')
  enable_thunk_options()

  if profile_toolchain_or_utf8() then
    if ntddi_version() < ntddi_winxp() then
      add_files('psapi/5.1/GetModuleFileNameExW.cc')
    end
  end

  if profile_toolchain_utf8() then
    add_files('psapi/u/GetModuleFileNameExA.cc')
  end

target('thunk-psapi')
  add_files('psapi/5.1/GetModuleFileNameExW.cc')
  enable_if_x86_32()
  enable_thunk_options()
  merge_win32_alias()
  skip_install()

target('thunk-psapi-u')
  add_files('psapi/u/GetModuleFileNameExA.cc')
  enable_thunk_options()
  merge_win32_alias()
  skip_install()
