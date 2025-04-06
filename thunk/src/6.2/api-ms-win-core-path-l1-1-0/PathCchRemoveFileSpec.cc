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
                 PathCchRemoveFileSpec,
                 _Inout_updates_(_Inexpressible_(cchPath)) PWSTR pszPath,
                 _In_ size_t cchPath)
  {
    if (pszPath == nullptr || cchPath == 0 || cchPath > PATHCCH_MAX_CCH)
      return E_INVALIDARG;

    PWSTR pszRootEnd;

    if (FAILED(PathCchSkipRoot(pszPath, (PCWSTR *)&pszRootEnd))) {
      pszRootEnd = pszPath;
    }

    const auto pszPathBufferEnd = pszPath + cchPath;

    if (pszRootEnd >= pszPathBufferEnd)
      return E_INVALIDARG;

    // 搜索最后一个 ‘\\’

    LPWSTR szLast = pszRootEnd;

    for (; *pszRootEnd; ++pszRootEnd) {
      if (pszRootEnd >= pszPathBufferEnd)
        return E_INVALIDARG;

      if (*pszRootEnd == L'\\')
        szLast = pszRootEnd;
    }

    if (*szLast) {
      *szLast = L'\0';
      PathCchRemoveBackslash(pszPath, cchPath);

      return S_OK;
    } else {
      return PathCchRemoveBackslash(pszPath, cchPath);
    }
  }
} // namespace mingw_thunk
