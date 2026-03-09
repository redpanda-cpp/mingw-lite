#include "LCMapStringEx.h"

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
    __DISPATCH_THUNK_2(LCMapStringEx,
                       const auto pfn = try_get_LCMapStringEx(),
                       pfn,
                       &f::fallback_LCMapStringEx);

    return dllimport_LCMapStringEx(lpLocaleName,
                                   dwMapFlags,
                                   lpSrcStr,
                                   cchSrc,
                                   lpDestStr,
                                   cchDest,
                                   lpVersionInformation,
                                   lpReserved,
                                   sortHandle);
  }

  namespace f
  {
    int __stdcall
    fallback_LCMapStringEx(_In_opt_ LPCWSTR lpLocaleName,
                           _In_ DWORD dwMapFlags,
                           _In_reads_(cchSrc) LPCWSTR lpSrcStr,
                           _In_ int cchSrc,
                           _Out_writes_opt_(cchDest) LPWSTR lpDestStr,
                           _In_ int cchDest,
                           _In_opt_ LPNLSVERSIONINFO lpVersionInformation,
                           _In_opt_ LPVOID lpReserved,
                           _In_opt_ LPARAM sortHandle)
    {
      auto Locale = LocaleNameToLCID(lpLocaleName, 0);

      if (Locale == 0) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
      }

      return LCMapStringW(
          Locale, dwMapFlags, lpSrcStr, cchSrc, lpDestStr, cchDest);
    }
  } // namespace f
} // namespace mingw_thunk
