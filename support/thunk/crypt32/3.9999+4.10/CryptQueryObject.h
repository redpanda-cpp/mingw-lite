#pragma once

#include <windows.h>

// after <windows.h>
#include <wincrypt.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall unimplemented_CryptQueryObject(
        _In_ DWORD dwObjectType,
        _In_ const void *pvObject,
        _In_ DWORD dwExpectedContentTypeFlags,
        _In_ DWORD dwExpectedFormatTypeFlags,
        _In_ DWORD dwFlags,
        _Out_ DWORD *pdwMsgAndCertEncodingType,
        _Out_ DWORD *pdwContentType,
        _Out_ DWORD *pdwFormatType,
        _Out_ HCERTSTORE *phCertStore,
        _Out_ HCRYPTMSG *phMsg,
        _Out_ const void **ppvContext);
  }
} // namespace mingw_thunk
