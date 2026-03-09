#include "RemoveDirectoryW.h"

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
      kernel32, 4, BOOL, WINAPI, RemoveDirectoryW, _In_ LPCWSTR lpPathName)
  {
    __DISPATCH_THUNK_2(RemoveDirectoryW,
                       i::is_nt(),
                       &__ms_RemoveDirectoryW,
                       &f::win9x_RemoveDirectoryW);

    return dllimport_RemoveDirectoryW(lpPathName);
  }

  namespace f
  {
    BOOL __stdcall win9x_RemoveDirectoryW(_In_ LPCWSTR lpPathName)
    {
      if (!lpPathName) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
      }

      d::a_str aname;
      if (!aname.from_w(lpPathName)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return FALSE;
      }

      return __ms_RemoveDirectoryA(aname.c_str());
    }
  } // namespace f
} // namespace mingw_thunk
