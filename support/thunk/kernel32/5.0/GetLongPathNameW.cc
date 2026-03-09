#include "GetLongPathNameW.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>
#include <winerror.h>

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
    if (i::is_nt()) {
      __DISPATCH_THUNK_2(GetLongPathNameW,
                         const auto pfn = try_get_GetLongPathNameW(),
                         pfn,
                         &f::noop_GetLongPathNameW);
    } else {
      __DISPATCH_THUNK_2(GetLongPathNameW,
                         const auto pfn = kernel32_GetLongPathNameA(),
                         &f::win98_GetLongPathNameW,
                         &f::noop_GetLongPathNameW);
    }

    return dllimport_GetLongPathNameW(lpszShortPath, lpszLongPath, cchBuffer);
  }

  namespace f
  {
    DWORD WINAPI win98_GetLongPathNameW(_In_ LPCWSTR lpszShortPath,
                                        _Out_ LPWSTR lpszLongPath,
                                        _In_ DWORD cchBuffer)
    {
      auto pGetLongPathNameA = kernel32_GetLongPathNameA();

      if (!lpszShortPath) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
      }

      // dry run for buffer size
      if (cchBuffer && !lpszLongPath) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
      }

      d::a_str a_short{d::max_path_tag{}};
      if (!a_short.from_w(lpszShortPath)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return 0;
      }

      d::a_str a_long{d::max_path_tag{}};
      DWORD ret = pGetLongPathNameA(a_short.c_str(), a_long.data(), MAX_PATH);
      if (ret == 0)
        return 0;

      while (ret > a_long.size()) {
        if (!a_long.resize(ret)) {
          SetLastError(ERROR_OUTOFMEMORY);
          return 0;
        }

        ret = pGetLongPathNameA(a_short.c_str(), a_long.data(), a_long.size());
        if (ret == 0)
          return 0;
      }

      d::w_str w_long;
      if (!w_long.from_a(a_long.c_str())) {
        SetLastError(ERROR_OUTOFMEMORY);
        return 0;
      }

      int w_size = w_long.size();
      if (w_size + 1 > cchBuffer)
        return w_size + 1;

      c::wmemcpy(lpszLongPath, w_long.c_str(), w_size);
      lpszLongPath[w_size] = 0;
      return w_size;
    }

    DWORD WINAPI noop_GetLongPathNameW(_In_ LPCWSTR lpszShortPath,
                                       _Out_ LPWSTR lpszLongPath,
                                       _In_ DWORD cchBuffer)
    {
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
      if (len + 1 > cchBuffer)
        return len + 1;

      c::wmemcpy(lpszLongPath, lpszShortPath, len);
      lpszLongPath[len] = 0;
      return len;
    }
  } // namespace f
} // namespace mingw_thunk
