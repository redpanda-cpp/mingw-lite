#include <thunk/_common.h>

#include <windows.h>

#include <nocrt/wchar.h>

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

    size_t len = libc::wcslen(lpszShortPath);
    if (len >= cchBuffer)
      return len + 1;

    libc::wmemcpy(lpszLongPath, lpszShortPath, len);
    lpszLongPath[len] = 0;
    return len;
  }
} // namespace mingw_thunk
