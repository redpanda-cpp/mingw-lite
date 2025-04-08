#include <thunk/_common.h>
#include <thunk/yy/api-ms-win-core-path.h>

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
                 PathCchSkipRoot,
                 _In_ PCWSTR pszPath,
                 _Outptr_ PCWSTR *ppszRootEnd)
  {
    if (pszPath == nullptr || *pszPath == L'\0' || ppszRootEnd == nullptr)
      return E_INVALIDARG;

    *ppszRootEnd = nullptr;

    PCWSTR szRootEnd;
    if (PathIsUNCEx(pszPath, &szRootEnd)) {
      // 如果是UNC路径，则尝试跳过 2个 '\\'
      DWORD nSeparate = 0;

      for (; *szRootEnd && nSeparate < 2; ++szRootEnd) {
        if (*szRootEnd == L'\\') {
          ++nSeparate;
        }
      }
    } else if (pszPath[0] == L'\\' && pszPath[1] != L'\\') {
      // "\\XXXXXX" 一个相对路径
      szRootEnd = pszPath + 1;
    } else {
      if (internal::PathIsVolumeGUIDWorker(pszPath)) {
        szRootEnd = pszPath + 48;
      } else if (pszPath[0] == L'\\' && pszPath[1] == L'\\' &&
                 pszPath[2] == L'?' && pszPath[3] == L'\\' &&
                 ((L'A' <= pszPath[4] && pszPath[4] <= L'Z') ||
                  (L'a' <= pszPath[4] &&
                   pszPath[4] <= L'z')) // iswalpha(pszPath[4])
                 && pszPath[5] == L':') {
        // "\\\\?\\C:\\"
        szRootEnd = pszPath + 6;
      } else if (((L'A' <= pszPath[0] && pszPath[0] <= L'Z') ||
                  (L'a' <= pszPath[0] &&
                   pszPath[0] <= L'z')) // iswalpha(pszPath[0])
                 && pszPath[1] == L':') {
        // "C:\\"
        szRootEnd = pszPath + 2;
      } else {
        return E_INVALIDARG;
      }

      if (*szRootEnd == L'\\')
        ++szRootEnd;
    }

    *ppszRootEnd = szRootEnd;

    return S_OK;
  }
} // namespace mingw_thunk
