#include <thunk/_common.h>

#include <nocrt/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 BOOL,
                 WINAPI,
                 GetCPInfoExA,
                 _In_ UINT CodePage,
                 _In_ DWORD dwFlags,
                 _Out_ LPCPINFOEXA lpCPInfoEx)
  {
    if (const auto pfn = try_get_GetCPInfoExA())
      return pfn(CodePage, dwFlags, lpCPInfoEx);

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

    lpCPInfoEx->UnicodeDefaultChar = L'?';
    lpCPInfoEx->CodePage = CodePage;

    if (CodePage == CP_UTF8)
      libc::strcpy(lpCPInfoEx->CodePageName, "UTF-8");
    else {
      char *name = lpCPInfoEx->CodePageName;
      *name++ = 'C';
      *name++ = 'P';
      if (CodePage >= 10000)
        *name++ = '0' + CodePage / 10000;
      if (CodePage >= 1000)
        *name++ = '0' + CodePage / 1000 % 10;
      *name++ = '0' + CodePage / 100 % 10;
      *name++ = '0' + CodePage / 10 % 10;
      *name++ = '0' + CodePage % 10;
      *name++ = '\0';
    }

    return TRUE;
  }
} // namespace mingw_thunk
