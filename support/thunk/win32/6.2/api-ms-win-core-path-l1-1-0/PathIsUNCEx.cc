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
                 _Success_(return) BOOL,
                 WINAPI,
                 PathIsUNCEx,
                 _In_ PCWSTR pszPath,
                 _Outptr_opt_ PCWSTR *ppszServer)
  {
    if (ppszServer)
      *ppszServer = nullptr;

    if (pszPath[0] == L'\\' && pszPath[1] == L'\\') {
      if (pszPath[2] == L'?') {
        if (pszPath[3] == L'\\' && (pszPath[4] == L'U' || pszPath[4] == L'u') &&
            (pszPath[5] == L'N' || pszPath[5] == L'n') &&
            (pszPath[6] == L'C' || pszPath[6] == L'c') && pszPath[7] == L'\\') {
          pszPath += 8;
        } else {
          return FALSE;
        }
      } else {
        // PathIsVolumeGUIDWorker 判断是多余的，因为 "\\\\?\\Volume{GUID}"
        // 这样的形式走不到这个分支

        pszPath += 2;
      }

      if (ppszServer)
        *ppszServer = pszPath;

      return TRUE;
    }

    return FALSE;
  }
} // namespace mingw_thunk
