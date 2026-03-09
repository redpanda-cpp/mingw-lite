#include "MoveFileW.h"

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
                 MoveFileW,
                 _In_ LPCWSTR lpExistingFileName,
                 _In_ LPCWSTR lpNewFileName)
  {
    __DISPATCH_THUNK_2(
        MoveFileW, i::is_nt(), &__ms_MoveFileW, &f::win9x_MoveFileW);

    return dllimport_MoveFileW(lpExistingFileName, lpNewFileName);
  }

  namespace f
  {
    BOOL __stdcall win9x_MoveFileW(_In_ LPCWSTR lpExistingFileName,
                                   _In_ LPCWSTR lpNewFileName)
    {
      if (!lpExistingFileName || !lpNewFileName) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
      }

      d::a_str a_old;
      if (!a_old.from_w(lpExistingFileName)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return FALSE;
      }

      d::a_str a_new;
      if (!a_new.from_w(lpNewFileName)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return FALSE;
      }

      return __ms_MoveFileA(a_old.c_str(), a_new.c_str());
    }
  } // namespace f
} // namespace mingw_thunk
