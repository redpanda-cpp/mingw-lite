#pragma once

#include <windows.h>

// after <windows.h>
#include <wincrypt.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall unimplemented_CertCloseStore(_In_ HCERTSTORE hCertStore,
                                                _In_ DWORD dwFlags);
  }
} // namespace mingw_thunk
