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
                 16,
                 HRESULT,
                 WINAPI,
                 PathCchAppendEx,
                 _Inout_updates_(cchPath) PWSTR pszPath,
                 _In_ size_t cchPath,
                 _In_opt_ PCWSTR pszMore,
                 _In_ ULONG dwFlags)
  {
    if (pszMore && PathIsUNCEx(pszMore, nullptr) == false &&
        internal::IsExtendedLengthDosDevicePath(pszMore) == false) {
      // 跳过开头的所有 '\\'
      for (; *pszMore == L'\\'; ++pszMore)
        ;
    }

    return PathCchCombineEx(pszPath, cchPath, pszPath, pszMore, dwFlags);
  }
} // namespace mingw_thunk
