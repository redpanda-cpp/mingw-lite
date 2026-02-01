#include <thunk/_common.h>
#include <thunk/string.h>

#include <nostl/iterator.h>

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

    CPINFOEXW w_info;
    if (!GetCPInfoExW(CodePage, dwFlags, &w_info))
      return FALSE;

    libc::memcpy(lpCPInfoEx, &w_info, sizeof(CPINFO));
    lpCPInfoEx->UnicodeDefaultChar = w_info.UnicodeDefaultChar;
    lpCPInfoEx->CodePage = w_info.CodePage;
    WideCharToMultiByte(CP_ACP,
                        0,
                        w_info.CodePageName,
                        -1,
                        lpCPInfoEx->CodePageName,
                        stl::size(lpCPInfoEx->CodePageName),
                        NULL,
                        NULL);

    return TRUE;
  }
} // namespace mingw_thunk
