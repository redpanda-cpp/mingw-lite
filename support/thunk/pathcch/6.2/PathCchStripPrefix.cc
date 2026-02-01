#include <thunk/_common.h>
#include <thunk/yy/api-ms-win-core-path.h>
#include <thunk/yy/strsafe.h>

#include <pathcch.h>
#include <windows.h>

namespace mingw_thunk
{
  // YY-Thunks 1.1.6
  // Minimum supported client	Windows 8 [desktop apps | UWP apps]
  // Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
  __DEFINE_THUNK(api_ms_win_core_path_l1_1_0,
                 8,
                 HRESULT,
                 WINAPI,
                 PathCchStripPrefix,
                 _Inout_updates_(cchPath) PWSTR pszPath,
                 _In_ size_t cchPath)
  {
    if (pszPath == nullptr || cchPath == 0 || cchPath > PATHCCH_MAX_CCH)
      return E_INVALIDARG;

    if (!internal::IsExtendedLengthDosDevicePath(pszPath))
      return S_FALSE;

    if (cchPath < 4)
      return E_INVALIDARG;

    if ((pszPath[4] == L'U' || pszPath[4] == L'u') &&
        (pszPath[5] == L'N' || pszPath[5] == L'n') &&
        (pszPath[6] == L'C' || pszPath[6] == L'c') && pszPath[7] == L'\\') {
      if (cchPath < 8)
        return E_INVALIDARG;

      return internal::StringCchCopyW(pszPath + 2, cchPath - 2, pszPath + 8);
    } else if ((L'A' <= pszPath[4] && pszPath[4] <= L'Z' ||
                L'a' <= pszPath[4] && pszPath[4] <= L'z') &&
               pszPath[5] == L':') {
      return internal::StringCchCopyW(pszPath, cchPath, pszPath + 4);
    } else {
      return S_FALSE;
    }
  }
} // namespace mingw_thunk
