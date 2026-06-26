#include "CertCloseStore.h"

#include <thunk/_common.h>

#include <windows.h>

// after <windows.h>
#include <wincrypt.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(crypt32,
                 8,
                 BOOL,
                 WINAPI,
                 CertCloseStore,
                 _In_ HCERTSTORE hCertStore,
                 _In_ DWORD dwFlags)
  {
    __DISPATCH_THUNK_2(CertCloseStore,
                       const auto pfn = try_get_CertCloseStore(),
                       pfn,
                       &f::unimplemented_CertCloseStore);

    return dllimport_CertCloseStore(hCertStore, dwFlags);
  }

  namespace f
  {
    BOOL __stdcall unimplemented_CertCloseStore(_In_ HCERTSTORE hCertStore,
                                                _In_ DWORD dwFlags)
    {
      // don't fail on close
      return TRUE;
    }
  } // namespace f
} // namespace mingw_thunk
