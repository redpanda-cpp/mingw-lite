#pragma once

#include <windows.h>

// after <windows.h>
#include <wincrypt.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall unimplemented_CryptMsgGetParam(
        _In_ HCRYPTMSG hCryptMsg,
        _In_ DWORD dwParamType,
        _In_ DWORD dwIndex,
        _Out_ void *pvData,
        _Inout_ DWORD *pcbData);
  }
} // namespace mingw_thunk
