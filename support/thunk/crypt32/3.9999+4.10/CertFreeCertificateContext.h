#pragma once

#include <windows.h>

// after <windows.h>
#include <wincrypt.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall unimplemented_CertFreeCertificateContext(
        _In_ PCCERT_CONTEXT pCertContext);
  }
} // namespace mingw_thunk
