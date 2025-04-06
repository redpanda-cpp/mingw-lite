#include <thunk/_common.h>

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
                 PathCchStripToRoot,
                 _Inout_updates_(_Inexpressible_(cchPath)) PWSTR pszPath,
                 _In_ size_t cchPath)
  {
    if (pszPath == nullptr || cchPath == 0 || cchPath > PATHCCH_MAX_CCH)
      return E_INVALIDARG;

    PWSTR szRootEnd;
    auto hr = PathCchSkipRoot(pszPath, (PCWSTR *)&szRootEnd);

    do {
      if (FAILED(hr))
        break;

      if (szRootEnd >= pszPath + cchPath) {
        hr = E_INVALIDARG;
        break;
      }

      if (*szRootEnd) {
        *szRootEnd = L'\0';
        PathCchRemoveBackslash(pszPath, cchPath);
        hr = S_OK;
      } else {
        hr = PathCchRemoveBackslash(pszPath, cchPath);
        if (FAILED(hr))
          break;
      }

      return hr;

    } while (false);

    *pszPath = L'\0';

    return hr;
  }
} // namespace mingw_thunk
