#include "GetFileAttributesExA.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 BOOL,
                 WINAPI,
                 GetFileAttributesExA,
                 _In_ LPCSTR lpFileName,
                 _In_ GET_FILEEX_INFO_LEVELS fInfoLevelId,
                 _Out_ LPVOID lpFileInformation)
  {
    __DISPATCH_THUNK_2(GetFileAttributesExA,
                       const auto pfn = try_get_GetFileAttributesExA(),
                       pfn,
                       &f::win9x_GetFileAttributesExA);

    return dllimport_GetFileAttributesExA(
        lpFileName, fInfoLevelId, lpFileInformation);
  }

  namespace f
  {
    BOOL __stdcall
    win9x_GetFileAttributesExA(_In_ LPCSTR lpFileName,
                               _In_ GET_FILEEX_INFO_LEVELS fInfoLevelId,
                               _Out_ LPVOID lpFileInformation)
    {
      if (!lpFileName) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
      }

      d::w_str w_file_name;
      if (!w_file_name.from_a(lpFileName)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return FALSE;
      }

      return GetFileAttributesExW(
          w_file_name.c_str(), fInfoLevelId, lpFileInformation);
    }
  } // namespace f
} // namespace mingw_thunk
