#include <thunk/_common.h>
#include <thunk/yy/api-ms-win-core-path.h>

#include <pathcch.h>

namespace mingw_thunk
{
  // YY-Thunk 1.1.6
  // Minimum supported client	Windows 8 [desktop apps | UWP apps]
  // Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
  __DEFINE_THUNK(api_ms_win_core_path_l1_1_0,
                 4,
                 BOOL,
                 WINAPI,
                 PathCchIsRoot,
                 _In_opt_ PCWSTR pszPath)
  {
    // NULL or ""
    if (pszPath == nullptr || pszPath[0] == L'\0')
      return FALSE;

    // "C:\\" 形式
    if (((L'A' <= pszPath[0] && pszPath[0] <= L'Z') ||
         (L'a' <= pszPath[0] && pszPath[0] <= L'z')) // iswalpha(pszPath[0])
        && pszPath[1] == L':' && pszPath[2] == L'\\' && pszPath[3] == L'\0') {
      return TRUE;
    }

    // "\\" 形式
    if (pszPath[0] == L'\\' && pszPath[1] == L'\0') {
      return TRUE;
    }

    /*
     * "\\\path1\\path2"
     * "\\\\path1"
     * "\\\\?\\UNC\\path1\\path2"
     * "\\\\?\\UNC\\path1"
     */
    PCWSTR pszServer;
    if (PathIsUNCEx(pszPath, &pszServer)) {
      DWORD nSeparatorsCount = 0;

      for (; *pszServer; ++pszServer) {
        if (*pszServer == L'\\') {
          ++nSeparatorsCount;

          if (nSeparatorsCount > 1 || pszServer[1] == L'\0')
            return FALSE;
        }
      }

      return TRUE;
    }

    /*
     * "\\\\?\\C:\\"
     * "\\\\?\\Volume{guid}\\"
     */

    if (pszPath[0] == L'\\' && pszPath[1] == L'\\' && pszPath[2] == L'?' &&
        pszPath[3] == L'\\') {
      // "\\\\?\\C:\\"
      if (((L'A' <= pszPath[4] && pszPath[4] <= L'Z') ||
           (L'a' <= pszPath[4] && pszPath[4] <= L'z')) // iswalpha(pszPath[4])
          && pszPath[5] == L':' && pszPath[6] == L'\\' && pszPath[7] == L'\0') {
        return TRUE;
      }

      // "\\\\?\\Volume{guid}\\"
      if (internal::PathIsVolumeGUIDWorker(pszPath) && pszPath[48] == L'\\' &&
          pszPath[49] == L'\0') {
        return TRUE;
      }
    }

    return FALSE;
  }
} // namespace mingw_thunk
