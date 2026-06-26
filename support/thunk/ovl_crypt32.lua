target('overlay-crypt32')
  enable_thunk_options()

  if profile_toolchain_or_utf8() then
    if ntddi_version() < ntddi_win98() then
      add_files(
        'crypt32/3.9999+4.10/CertCloseStore.cc',
        'crypt32/3.9999+4.10/CertFindCertificateInStore.cc',
        'crypt32/3.9999+4.10/CertFreeCertificateContext.cc',
        'crypt32/3.9999+4.10/CertGetNameStringW.cc',
        'crypt32/3.9999+4.10/CryptMsgClose.cc',
        'crypt32/3.9999+4.10/CryptMsgGetParam.cc',
        'crypt32/3.9999+4.10/CryptQueryObject.cc')
    end
  end

target('thunk-crypt32')
  add_files(
    'crypt32/3.9999+4.10/CertCloseStore.cc',
    'crypt32/3.9999+4.10/CertFindCertificateInStore.cc',
    'crypt32/3.9999+4.10/CertFreeCertificateContext.cc',
    'crypt32/3.9999+4.10/CertGetNameStringW.cc',
    'crypt32/3.9999+4.10/CryptMsgClose.cc',
    'crypt32/3.9999+4.10/CryptMsgGetParam.cc',
    'crypt32/3.9999+4.10/CryptQueryObject.cc')
  enable_if_x86_32()
  enable_thunk_options()
  merge_win32_alias()
  skip_install()
