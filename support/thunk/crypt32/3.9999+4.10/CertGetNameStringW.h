#pragma once

#include <windows.h>

// after <windows.h>
#include <wincrypt.h>

namespace mingw_thunk
{
  namespace f
  {
    DWORD __stdcall unimplemented_CertGetNameStringW(
        _In_ PCCERT_CONTEXT pCertContext,
        _In_ DWORD dwType,
        _In_ DWORD dwFlags,
        _In_ void *pvTypePara,
        _Out_ LPWSTR pszNameString,
        _In_ DWORD cchNameString);
  }
} // namespace mingw_thunk
