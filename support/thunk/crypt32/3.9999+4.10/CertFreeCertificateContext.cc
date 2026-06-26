#include "CertFreeCertificateContext.h"

#include <thunk/_common.h>

#include <windows.h>

// after <windows.h>
#include <wincrypt.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(crypt32,
                 4,
                 BOOL,
                 WINAPI,
                 CertFreeCertificateContext,
                 _In_ PCCERT_CONTEXT pCertContext)
  {
    __DISPATCH_THUNK_2(CertFreeCertificateContext,
                       const auto pfn = try_get_CertFreeCertificateContext(),
                       pfn,
                       &f::unimplemented_CertFreeCertificateContext);

    return dllimport_CertFreeCertificateContext(pCertContext);
  }

  namespace f
  {
    BOOL __stdcall unimplemented_CertFreeCertificateContext(
        _In_ PCCERT_CONTEXT pCertContext)
    {
      // don't fail on free
      return TRUE;
    }
  } // namespace f
} // namespace mingw_thunk
