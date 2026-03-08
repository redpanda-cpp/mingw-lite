#include <thunk/_common.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 DWORD,
                 WINAPI,
                 GetLongPathNameW,
                 _In_ LPCWSTR lpszShortPath,
                 _Out_ LPWSTR lpszLongPath,
                 _In_ DWORD cchBuffer)
  {
    if (const auto pfn = try_get_GetLongPathNameW()) {
      return pfn(lpszShortPath, lpszLongPath, cchBuffer);
    }

    // TODO: special handling Windows 9x

    if (!lpszShortPath) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return 0;
    }

    // dry run for buffer size
    if (cchBuffer && !lpszLongPath) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return 0;
    }

    size_t len = c::wcslen(lpszShortPath);
    if (len >= cchBuffer)
      return len + 1;

    c::wmemcpy(lpszLongPath, lpszShortPath, len);
    lpszLongPath[len] = 0;
    return len;
  }
} // namespace mingw_thunk
