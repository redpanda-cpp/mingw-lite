#include "CertFindCertificateInStore.h"

#include <thunk/_common.h>

#include <windows.h>

// after <windows.h>
#include <wincrypt.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(crypt32,
                 24,
                 PCCERT_CONTEXT,
                 WINAPI,
                 CertFindCertificateInStore,
                 _In_ HCERTSTORE hCertStore,
                 _In_ DWORD dwCertEncodingType,
                 _In_ DWORD dwFindFlags,
                 _In_ DWORD dwFindType,
                 _In_ const void *pvFindPara,
                 _In_ PCCERT_CONTEXT pPrevCertContext)
  {
    __DISPATCH_THUNK_2(CertFindCertificateInStore,
                       const auto pfn = try_get_CertFindCertificateInStore(),
                       pfn,
                       &f::unimplemented_CertFindCertificateInStore);

    return dllimport_CertFindCertificateInStore(hCertStore,
                                                dwCertEncodingType,
                                                dwFindFlags,
                                                dwFindType,
                                                pvFindPara,
                                                pPrevCertContext);
  }

  namespace f
  {
    PCCERT_CONTEXT __stdcall unimplemented_CertFindCertificateInStore(
        _In_ HCERTSTORE hCertStore,
        _In_ DWORD dwCertEncodingType,
        _In_ DWORD dwFindFlags,
        _In_ DWORD dwFindType,
        _In_ const void *pvFindPara,
        _In_ PCCERT_CONTEXT pPrevCertContext)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return nullptr;
    }
  } // namespace f
} // namespace mingw_thunk
