#include "CopyFileW.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  // Windows 95 (stub)
  // Windows NT 4.0
  __DEFINE_THUNK(kernel32,
                 12,
                 BOOL,
                 WINAPI,
                 CopyFileW,
                 _In_ LPCWSTR lpExistingFileName,
                 _In_ LPCWSTR lpNewFileName,
                 _In_ BOOL bFailIfExists)
  {
    __DISPATCH_THUNK_2(
        CopyFileW, i::is_nt(), &__ms_CopyFileW, &f::win9x_CopyFileW);

    return dllimport_CopyFileW(
        lpExistingFileName, lpNewFileName, bFailIfExists);
  }

  namespace f
  {
    BOOL __stdcall win9x_CopyFileW(_In_ LPCWSTR lpExistingFileName,
                                   _In_ LPCWSTR lpNewFileName,
                                   _In_ BOOL bFailIfExists)
    {
      if (!lpExistingFileName || !lpNewFileName) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
      }

      d::a_str a_old_file;
      if (!a_old_file.from_w(lpExistingFileName)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return FALSE;
      }

      d::a_str a_new_file;
      if (!a_new_file.from_w(lpNewFileName)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return FALSE;
      }

      return __ms_CopyFileA(
          a_old_file.c_str(), a_new_file.c_str(), bFailIfExists);
    }
  } // namespace f
} // namespace mingw_thunk
