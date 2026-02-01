#include "MoveFileExW.h"

#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>
#include <winerror.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 BOOL,
                 WINAPI,
                 MoveFileExW,
                 _In_ LPCWSTR lpExistingFileName,
                 _In_opt_ LPCWSTR lpNewFileName,
                 _In_ DWORD dwFlags)
  {
    if (internal::is_nt())
      return get_MoveFileExW()(lpExistingFileName, lpNewFileName, dwFlags);

    // Windows 9x: `MoveFileExW(-A)` is a stub
    return impl::win9x_MoveFileExW(lpExistingFileName, lpNewFileName, dwFlags);
  }

  namespace impl
  {
    BOOL win9x_MoveFileExW(_In_ LPCWSTR lpExistingFileName,
                           _In_opt_ LPCWSTR lpNewFileName,
                           _In_ DWORD dwFlags)
    {
      if (dwFlags & MOVEFILE_DELAY_UNTIL_REBOOT) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
      }

      int ok = MoveFileW(lpExistingFileName, lpNewFileName);
      if (!ok &&
          (dwFlags & (MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING))) {
        bool replace = dwFlags & MOVEFILE_REPLACE_EXISTING;
        ok = CopyFileW(lpExistingFileName, lpNewFileName, !replace);
        if (ok)
          DeleteFileW(lpExistingFileName);
      }
      return ok;
    }
  } // namespace impl

} // namespace mingw_thunk
