target('overlay-bcrypt')
  enable_thunk_options()

  if profile_toolchain_or_utf8() then
    if ntddi_version() < ntddi_vista() then
      add_files('bcrypt/6.0/BCryptGenRandom.cc')
    end
  end

target('thunk-bcrypt')
  add_files('bcrypt/6.0/BCryptGenRandom.cc')
  enable_if_x86()
  enable_thunk_options()
  merge_win32_alias()
  skip_install()
