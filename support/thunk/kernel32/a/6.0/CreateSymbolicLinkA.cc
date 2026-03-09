#include "CreateSymbolicLinkA.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 BOOLEAN,
                 APIENTRY,
                 CreateSymbolicLinkA,
                 _In_ LPCSTR lpSymlinkFileName,
                 _In_ LPCSTR lpTargetFileName,
                 _In_ DWORD dwFlags)
  {
    __DISPATCH_THUNK_2(CreateSymbolicLinkA,
                       const auto pfn = try_get_CreateSymbolicLinkA(),
                       pfn,
                       &f::fallback_CreateSymbolicLinkA);

    return dllimport_CreateSymbolicLinkA(
        lpSymlinkFileName, lpTargetFileName, dwFlags);
  }

  namespace f
  {
    BOOLEAN __stdcall
    fallback_CreateSymbolicLinkA(_In_ LPCSTR lpSymlinkFileName,
                                 _In_ LPCSTR lpTargetFileName,
                                 _In_ DWORD dwFlags)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return FALSE;
    }
  } // namespace f
} // namespace mingw_thunk
