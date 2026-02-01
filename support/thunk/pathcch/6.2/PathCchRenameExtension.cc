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
                 PathCchRenameExtension,
                 _Inout_updates_(cchPath) PWSTR pszPath,
                 _In_ size_t cchPath,
                 _In_ PCWSTR pszExt)
  {
    if (pszPath == nullptr || cchPath == 0 || cchPath > PATHCCH_MAX_CCH ||
        internal::IsValidExtensionWorker(pszExt) == false)
      return E_INVALIDARG;

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

    if (*pszExt == L'.')
      ++pszExt;

    // 如果调用者提供的 后缀为空，那么删除现有的，然后立即返回 S_OK
    if (*pszExt == L'\0') {
      *pszPathExt = L'\0';
      return S_OK;
    }

    // 故意不用微软的方法，微软的方法在缓存区不足时会破坏原始值。
    // 覆盖现有文件后缀
    const auto szPathBufferEnd = pszPath + cchPath;
    const auto cchExt = wcslen(pszExt);

    // . ext < szPathBufferEnd
    if (pszPathExt + 1 + cchExt + 1 <= szPathBufferEnd) {
      *pszPathExt = L'.';
      ++pszPathExt;

      memcpy(pszPathExt, pszExt, (cchExt + 1) * sizeof(pszExt[0]));

      return S_OK;
    }

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
