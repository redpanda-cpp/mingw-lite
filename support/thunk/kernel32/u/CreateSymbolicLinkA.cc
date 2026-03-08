#include <thunk/_common.h>
#include <thunk/string.h>

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
    if (!lpSymlinkFileName || !lpTargetFileName) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    d::w_str w_symlink;
    if (!w_symlink.from_u(lpSymlinkFileName)) {
      SetLastError(ERROR_OUTOFMEMORY);
      return FALSE;
    }

    d::w_str w_target;
    if (!w_target.from_u(lpTargetFileName)) {
      SetLastError(ERROR_OUTOFMEMORY);
      return FALSE;
    }

    return CreateSymbolicLinkW(w_symlink.c_str(), w_target.c_str(), dwFlags);
  }
} // namespace mingw_thunk
