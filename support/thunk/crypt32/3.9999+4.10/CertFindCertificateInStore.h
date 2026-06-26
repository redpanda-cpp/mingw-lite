#pragma once

#include <windows.h>

// after <windows.h>
#include <wincrypt.h>

namespace mingw_thunk
{
  namespace f
  {
    PCCERT_CONTEXT __stdcall unimplemented_CertFindCertificateInStore(
        _In_ HCERTSTORE hCertStore,
        _In_ DWORD dwCertEncodingType,
        _In_ DWORD dwFindFlags,
        _In_ DWORD dwFindType,
        _In_ const void *pvFindPara,
        _In_ PCCERT_CONTEXT pPrevCertContext);
  }
} // namespace mingw_thunk
