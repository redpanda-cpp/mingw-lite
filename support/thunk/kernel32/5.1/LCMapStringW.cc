#include "LCMapStringW.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>

#include <nocrt/wchar.h>

#include <windows.h>

namespace mingw_thunk
{
  // Windows 95 (stub)
  // Windows NT 4.0 (w/o LCMAP_INVARIANT)
  // Windows XP
  __DEFINE_THUNK(kernel32,
                 24,
                 int,
                 WINAPI,
                 LCMapStringW,
                 _In_ LCID Locale,
                 _In_ DWORD dwMapFlags,
                 _In_ LPCWSTR lpSrcStr,
                 _In_ int cchSrc,
                 _Out_opt_ LPWSTR lpDestStr,
                 _In_ int cchDest)
  {
    if (internal::os_geq(5, 1))
      return __ms_LCMapStringW(
          Locale, dwMapFlags, lpSrcStr, cchSrc, lpDestStr, cchDest);

    if (internal::is_nt())
      return impl::winnt_LCMapStringW(
          Locale, dwMapFlags, lpSrcStr, cchSrc, lpDestStr, cchDest);

    return impl::win9x_LCMapStringW(
        Locale, dwMapFlags, lpSrcStr, cchSrc, lpDestStr, cchDest);
  }

  namespace impl
  {
    int winnt_LCMapStringW(_In_ LCID Locale,
                           _In_ DWORD dwMapFlags,
                           _In_ LPCWSTR lpSrcStr,
                           _In_ int cchSrc,
                           _Out_opt_ LPWSTR lpDestStr,
                           _In_ int cchDest)
    {
      // fallback to en-US for invariant locale
      LCID lcid = (Locale == LOCALE_INVARIANT) ? 0x0409 : Locale;
      return __ms_LCMapStringW(
          lcid, dwMapFlags, lpSrcStr, cchSrc, lpDestStr, cchDest);
    }

    int win9x_LCMapStringW(_In_ LCID Locale,
                           _In_ DWORD dwMapFlags,
                           _In_ LPCWSTR lpSrcStr,
                           _In_ int cchSrc,
                           _Out_opt_ LPWSTR lpDestStr,
                           _In_ int cchDest)
    {
      if (!(dwMapFlags == LCMAP_UPPERCASE || dwMapFlags == LCMAP_LOWERCASE)) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
      }
      if (!(Locale == LOCALE_SYSTEM_DEFAULT || Locale == LOCALE_USER_DEFAULT ||
            Locale == LOCALE_INVARIANT || Locale == 0x0409)) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
      }

      if (cchSrc == -1)
        cchSrc = libc::wcslen(lpSrcStr) + 1;
      if (cchDest == 0)
        return cchSrc;
      if (lpDestStr == nullptr) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
      }
      if (cchDest < cchSrc) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return 0;
      }

      if (dwMapFlags == LCMAP_UPPERCASE) {
        for (int i = 0; i < cchSrc; ++i) {
          if (lpSrcStr[i] >= L'a' && lpSrcStr[i] <= L'z')
            lpDestStr[i] = lpSrcStr[i] - L'a' + L'A';
          else
            lpDestStr[i] = lpSrcStr[i];
        }
      } else {
        for (int i = 0; i < cchSrc; ++i) {
          if (lpSrcStr[i] >= L'A' && lpSrcStr[i] <= L'Z')
            lpDestStr[i] = lpSrcStr[i] - L'A' + L'a';
          else
            lpDestStr[i] = lpSrcStr[i];
        }
      }
      return cchSrc;
    }
  } // namespace impl
} // namespace mingw_thunk
