#include "DeleteFileW.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  // Windows 95 (stub)
  // Windows NT 4.0
  __DEFINE_THUNK(
      kernel32, 4, BOOL, WINAPI, DeleteFileW, _In_ LPCWSTR lpFileName)
  {
    __DISPATCH_THUNK_2(
        DeleteFileW, i::is_nt(), &__ms_DeleteFileW, &f::win9x_DeleteFileW);

    return dllimport_DeleteFileW(lpFileName);
  }

  namespace f
  {
    BOOL __stdcall win9x_DeleteFileW(_In_ LPCWSTR lpFileName)
    {
      if (!lpFileName) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
      }

      d::a_str aname;
      if (!aname.from_w(lpFileName)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return FALSE;
      }

      return __ms_DeleteFileA(aname.c_str());
    }
  } // namespace f
} // namespace mingw_thunk
