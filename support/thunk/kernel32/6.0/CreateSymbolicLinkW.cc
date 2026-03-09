#include "CreateSymbolicLinkW.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 BOOLEAN,
                 APIENTRY,
                 CreateSymbolicLinkW,
                 _In_ LPCWSTR lpSymlinkFileName,
                 _In_ LPCWSTR lpTargetFileName,
                 _In_ DWORD dwFlags)
  {
    __DISPATCH_THUNK_2(CreateSymbolicLinkW,
                       const auto pfn = try_get_CreateSymbolicLinkW(),
                       pfn,
                       &f::fallback_CreateSymbolicLinkW);

    return dllimport_CreateSymbolicLinkW(
        lpSymlinkFileName, lpTargetFileName, dwFlags);
  }

  namespace f
  {
    BOOLEAN __stdcall
    fallback_CreateSymbolicLinkW(_In_ LPCWSTR lpSymlinkFileName,
                                 _In_ LPCWSTR lpTargetFileName,
                                 _In_ DWORD dwFlags)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return FALSE;
    }
  } // namespace f
} // namespace mingw_thunk
