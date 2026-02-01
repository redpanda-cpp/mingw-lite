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
                 12,
                 HRESULT,
                 WINAPI,
                 PathCchAddExtension,
                 _Inout_updates_(cchPath) PWSTR pszPath,
                 _In_ size_t cchPath,
                 _In_ PCWSTR pszExt)
  {
    if (pszPath == nullptr || cchPath == 0 || cchPath > PATHCCH_MAX_CCH ||
        internal::IsValidExtensionWorker(pszExt) == false) {
      // loc_1009895F
      return E_INVALIDARG;
    }

    // 检测Unicode Path，如果不是，那么最多允许 260
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

    if (*pszPathExt != L'\0') {
      // 已经存在后缀，所以我们什么也不做。
      return S_FALSE;
    }

    if (*pszExt == L'.')
      ++pszExt;

    // 后缀是 ""
    if (*pszExt == L'\0') {
      return S_OK;
    }

    do {
      const auto szPathBufferEnd = pszPath + cchPath;
      auto szDest = pszPathExt;

      if (szDest >= szPathBufferEnd)
        break;

      *szDest = L'.';
      ++szDest;

      for (; szDest < szPathBufferEnd; ++szDest, ++pszExt) {
        if ((*szDest = *pszExt) == L'\0') {
          return S_OK;
        }
      }

    } while (false);

    *pszPathExt = L'\0';

    if (bExtendedLengthDosDevicePath) {
      if (cchPath != PATHCCH_MAX_CCH) {
        return __HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
      }
    } else {
      if (cchPath != MAX_PATH) {
        return __HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
      }
    }

    return __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
  }
} // namespace mingw_thunk
