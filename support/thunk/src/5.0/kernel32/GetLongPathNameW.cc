#include <thunk/_common.h>
#include <thunk/libc/string.h>
#include <thunk/libc/wchar.h>

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

    size_t len = internal::wcslen(lpszShortPath);
    if (len >= cchBuffer)
      return len + 1;

    internal::memcpy(lpszLongPath, lpszShortPath, len * sizeof(wchar_t));
    lpszLongPath[len] = 0;
    return len;
  }
} // namespace mingw_thunk
