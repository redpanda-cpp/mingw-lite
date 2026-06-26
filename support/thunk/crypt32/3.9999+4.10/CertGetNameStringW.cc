#include "CertGetNameStringW.h"

#include <thunk/_common.h>

#include <windows.h>

// after <windows.h>
#include <wincrypt.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(crypt32,
                 24,
                 DWORD,
                 WINAPI,
                 CertGetNameStringW,
                 _In_ PCCERT_CONTEXT pCertContext,
                 _In_ DWORD dwType,
                 _In_ DWORD dwFlags,
                 _In_ void *pvTypePara,
                 _Out_ LPWSTR pszNameString,
                 _In_ DWORD cchNameString)
  {
    __DISPATCH_THUNK_2(CertGetNameStringW,
                       const auto pfn = try_get_CertGetNameStringW(),
                       pfn,
                       &f::unimplemented_CertGetNameStringW);

    return dllimport_CertGetNameStringW(pCertContext,
                                        dwType,
                                        dwFlags,
                                        pvTypePara,
                                        pszNameString,
                                        cchNameString);
  }

  namespace f
  {
    DWORD __stdcall unimplemented_CertGetNameStringW(
        _In_ PCCERT_CONTEXT pCertContext,
        _In_ DWORD dwType,
        _In_ DWORD dwFlags,
        _In_ void *pvTypePara,
        _Out_ LPWSTR pszNameString,
        _In_ DWORD cchNameString)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return 0;
    }
  } // namespace f
} // namespace mingw_thunk
