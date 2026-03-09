#include "SetCurrentDirectoryW.h"

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
      kernel32, 4, BOOL, WINAPI, SetCurrentDirectoryW, _In_ LPCWSTR lpPathName)
  {
    __DISPATCH_THUNK_2(SetCurrentDirectoryW,
                       i::is_nt(),
                       &__ms_SetCurrentDirectoryW,
                       &f::win9x_SetCurrentDirectoryW);

    return dllimport_SetCurrentDirectoryW(lpPathName);
  }

  namespace f
  {
    BOOL __stdcall win9x_SetCurrentDirectoryW(_In_ LPCWSTR lpPathName)
    {
      if (!lpPathName) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
      }

      d::a_str a_path_name;
      if (!a_path_name.from_w(lpPathName)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return FALSE;
      }

      return __ms_SetCurrentDirectoryA(a_path_name.c_str());
    }
  } // namespace f
} // namespace mingw_thunk
