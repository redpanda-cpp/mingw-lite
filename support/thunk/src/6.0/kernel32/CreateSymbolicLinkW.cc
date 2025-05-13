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
                 CreateSymbolicLinkW,
                 _In_ LPCWSTR lpSymlinkFileName,
                 _In_ LPCWSTR lpTargetFileName,
                 _In_ DWORD dwFlags)
  {
    if (const auto pCreateSymbolicLinkW = try_get_CreateSymbolicLinkW()) {
      return pCreateSymbolicLinkW(lpSymlinkFileName, lpTargetFileName, dwFlags);
    }

    SetLastError(ERROR_INVALID_FUNCTION);

    return FALSE;
  }
} // namespace mingw_thunk
