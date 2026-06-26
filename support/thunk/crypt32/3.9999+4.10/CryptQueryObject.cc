#include "CryptQueryObject.h"

#include <thunk/_common.h>

#include <windows.h>

// after <windows.h>
#include <wincrypt.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(crypt32,
                 44,
                 BOOL,
                 WINAPI,
                 CryptQueryObject,
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
                 _Out_ const void **ppvContext)
  {
    __DISPATCH_THUNK_2(CryptQueryObject,
                       const auto pfn = try_get_CryptQueryObject(),
                       pfn,
                       &f::unimplemented_CryptQueryObject);

    return dllimport_CryptQueryObject(dwObjectType,
                                      pvObject,
                                      dwExpectedContentTypeFlags,
                                      dwExpectedFormatTypeFlags,
                                      dwFlags,
                                      pdwMsgAndCertEncodingType,
                                      pdwContentType,
                                      pdwFormatType,
                                      phCertStore,
                                      phMsg,
                                      ppvContext);
  }

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
        _Out_ const void **ppvContext)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return FALSE;
    }
  } // namespace f
} // namespace mingw_thunk
