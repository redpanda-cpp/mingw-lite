#include <thunk/_common.h>
#include <thunk/string.h>

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

    if (!lpCPInfoEx) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    CPINFOEXW w_info;
    if (!GetCPInfoExW(CodePage, dwFlags, &w_info))
      return FALSE;

    c::memcpy(lpCPInfoEx, &w_info, sizeof(CPINFO));
    lpCPInfoEx->UnicodeDefaultChar = w_info.UnicodeDefaultChar;
    lpCPInfoEx->CodePage = w_info.CodePage;
    d::a_str::best_effort_from_w(
        lpCPInfoEx->CodePageName, MAX_PATH, w_info.CodePageName);

    return TRUE;
  }
} // namespace mingw_thunk
