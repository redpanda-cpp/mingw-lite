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
                 PathCchCanonicalize,
                 _Out_writes_(cchPathOut) PWSTR pszPathOut,
                 _In_ size_t cchPathOut,
                 _In_ PCWSTR pszPathIn)
  {
    return PathCchCanonicalizeEx(pszPathOut, cchPathOut, pszPathIn, 0);
  }
} // namespace mingw_thunk
