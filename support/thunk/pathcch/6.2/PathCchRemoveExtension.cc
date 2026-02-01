#include <thunk/_common.h>
#include <thunk/yy/api-ms-win-core-path.h>

#include <nostl/__algorithm/min.h>

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
                 PathCchRemoveExtension,
                 _Inout_updates_(_Inexpressible_(cchPath)) PWSTR pszPath,
                 _In_ size_t cchPath)
  {
    if (pszPath == nullptr || cchPath == 0 || cchPath > PATHCCH_MAX_CCH)
      return E_INVALIDARG;

    const auto bExtendedLengthDosDevicePath =
        internal::IsExtendedLengthDosDevicePath(pszPath);
    if (!bExtendedLengthDosDevicePath) {
      cchPath = stl::min<size_t>(MAX_PATH, cchPath);
    }

    PWSTR pszPathExt;

    auto hr = PathCchFindExtension(pszPath, cchPath, (LPCWSTR *)&pszPathExt);

    if (FAILED(hr)) {
      return hr;
    }

    if (*pszPathExt) {
      *pszPathExt = L'\0';
      return S_OK;
    } else {
      return S_FALSE;
    }
  }
} // namespace mingw_thunk
