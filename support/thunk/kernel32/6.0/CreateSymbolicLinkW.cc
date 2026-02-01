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
    if (const auto pfn = try_get_CreateSymbolicLinkW())
      return pfn(lpSymlinkFileName, lpTargetFileName, dwFlags);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
  }
} // namespace mingw_thunk
