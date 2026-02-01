#include "GetCPInfoExW.h"

#include <thunk/_common.h>

#include <nocrt/string.h>
#include <nocrt/wchar.h>

#include <limits.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 BOOL,
                 WINAPI,
                 GetCPInfoExW,
                 _In_ UINT CodePage,
                 _In_ DWORD dwFlags,
                 _Out_ LPCPINFOEXW lpCPInfoEx)
  {
    if (const auto pfn = try_get_GetCPInfoExW())
      return pfn(CodePage, dwFlags, lpCPInfoEx);

    return impl::fallback_GetCPInfoExW(CodePage, dwFlags, lpCPInfoEx);
  }

  namespace impl
  {
    BOOL fallback_GetCPInfoExW(_In_ UINT CodePage,
                               _In_ DWORD dwFlags,
                               _Out_ LPCPINFOEXW lpCPInfoEx)
    {
      if (CodePage >= USHRT_MAX) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
      }

      // Never use CP_THREAD_ACP! It does not mean what you think it means.
      if (CodePage == CP_MACCP || CodePage == CP_THREAD_ACP) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
      }

      if (CodePage == CP_ACP)
        CodePage = GetACP();

      if (CodePage == CP_OEMCP)
        CodePage = GetOEMCP();

      CPINFO info;
      if (!GetCPInfo(CodePage, &info))
        return FALSE;

      libc::memcpy(lpCPInfoEx, &info, sizeof(CPINFO));

      lpCPInfoEx->UnicodeDefaultChar = '?';
      lpCPInfoEx->CodePage = CodePage;

      if (CodePage == CP_UTF8) {
        libc::wcscpy(lpCPInfoEx->CodePageName, L"UTF-8");
      } else {
        wchar_t *name = lpCPInfoEx->CodePageName;
        *name++ = 'C';
        *name++ = 'P';
        if (CodePage >= 10000)
          *name++ = '0' + CodePage / 10000;
        if (CodePage >= 1000)
          *name++ = '0' + CodePage / 1000 % 10;
        *name++ = '0' + CodePage / 100 % 10;
        *name++ = '0' + CodePage / 10 % 10;
        *name++ = '0' + CodePage % 10;
        *name++ = 0;
      }

      return TRUE;
    }
  } // namespace impl
} // namespace mingw_thunk
