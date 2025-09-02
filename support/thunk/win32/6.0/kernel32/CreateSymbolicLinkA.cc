#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  // YY-Thunks 1.1.6
  // Windows Vista [desktop apps only]
  // Windows Server 2008[desktop apps only]
  __DEFINE_THUNK(kernel32,
                 12,
                 BOOLEAN,
                 APIENTRY,
                 CreateSymbolicLinkA,
                 _In_ LPCSTR lpSymlinkFileName,
                 _In_ LPCSTR lpTargetFileName,
                 _In_ DWORD dwFlags)
  {
    if (const auto pCreateSymbolicLinkA = try_get_CreateSymbolicLinkA()) {
      return pCreateSymbolicLinkA(lpSymlinkFileName, lpTargetFileName, dwFlags);
    }

    SetLastError(ERROR_INVALID_FUNCTION);

    return FALSE;
  }
} // namespace mingw_thunk
