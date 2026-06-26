#include "CryptMsgGetParam.h"

#include <thunk/_common.h>

#include <windows.h>

// after <windows.h>
#include <wincrypt.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(crypt32,
                 20,
                 BOOL,
                 WINAPI,
                 CryptMsgGetParam,
                 _In_ HCRYPTMSG hCryptMsg,
                 _In_ DWORD dwParamType,
                 _In_ DWORD dwIndex,
                 _Out_ void *pvData,
                 _Inout_ DWORD *pcbData)
  {
    __DISPATCH_THUNK_2(CryptMsgGetParam,
                       const auto pfn = try_get_CryptMsgGetParam(),
                       pfn,
                       &f::unimplemented_CryptMsgGetParam);

    return dllimport_CryptMsgGetParam(hCryptMsg,
                                      dwParamType,
                                      dwIndex,
                                      pvData,
                                      pcbData);
  }

  namespace f
  {
    BOOL __stdcall unimplemented_CryptMsgGetParam(
        _In_ HCRYPTMSG hCryptMsg,
        _In_ DWORD dwParamType,
        _In_ DWORD dwIndex,
        _Out_ void *pvData,
        _Inout_ DWORD *pcbData)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return FALSE;
    }
  } // namespace f
} // namespace mingw_thunk
