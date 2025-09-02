#include <thunk/_common.h>

#include <pathcch.h>

namespace mingw_thunk
{
  // YY-Thunk 1.1.6
  // Minimum supported client	Windows 8 [desktop apps | UWP apps]
  // Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
  __DEFINE_THUNK(api_ms_win_core_path_l1_1_0,
                 8,
                 HRESULT,
                 WINAPI,
                 PathCchRemoveBackslash,
                 _Inout_updates_(cchPath) PWSTR pszPath,
                 _In_ size_t cchPath)
  {
    return PathCchRemoveBackslashEx(pszPath, cchPath, nullptr, nullptr);
  }
} // namespace mingw_thunk
