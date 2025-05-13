#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  // YY-Thunks 1.1.6
  // Minimum supported client	Windows Vista [desktop apps | UWP apps]
  // Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
  __DEFINE_THUNK(kernel32,
                 36,
                 int,
                 WINAPI,
                 LCMapStringEx,
                 _In_opt_ LPCWSTR lpLocaleName,
                 _In_ DWORD dwMapFlags,
                 _In_reads_(cchSrc) LPCWSTR lpSrcStr,
                 _In_ int cchSrc,
                 _Out_writes_opt_(cchDest) LPWSTR lpDestStr,
                 _In_ int cchDest,
                 _In_opt_ LPNLSVERSIONINFO lpVersionInformation,
                 _In_opt_ LPVOID lpReserved,
                 _In_opt_ LPARAM sortHandle)
  {
    if (const auto pLCMapStringEx = try_get_LCMapStringEx()) {
      return pLCMapStringEx(lpLocaleName,
                            dwMapFlags,
                            lpSrcStr,
                            cchSrc,
                            lpDestStr,
                            cchDest,
                            lpVersionInformation,
                            lpReserved,
                            sortHandle);
    }

    auto Locale = LocaleNameToLCID(lpLocaleName, 0);

    if (Locale == 0) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return 0;
    }

    return LCMapStringW(
        Locale, dwMapFlags, lpSrcStr, cchSrc, lpDestStr, cchDest);
  }
} // namespace mingw_thunk
