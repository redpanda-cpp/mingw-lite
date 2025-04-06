#include <thunk/_common.h>

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
                 PathCchAppend,
                 _Inout_updates_(cchPath) PWSTR pszPath,
                 _In_ size_t cchPath,
                 _In_opt_ PCWSTR pszMore)
  {
    return PathCchAppendEx(pszPath, cchPath, pszMore, 0);
  }
} // namespace mingw_thunk
