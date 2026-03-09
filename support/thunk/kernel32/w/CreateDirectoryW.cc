#include "CreateDirectoryW.h"

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
                 8,
                 BOOL,
                 WINAPI,
                 CreateDirectoryW,
                 _In_ LPCWSTR lpPathName,
                 _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes)
  {
    __DISPATCH_THUNK_2(CreateDirectoryW,
                       i::is_nt(),
                       &__ms_CreateDirectoryW,
                       &f::win9x_CreateDirectoryW);

    return dllimport_CreateDirectoryW(lpPathName, lpSecurityAttributes);
  }

  namespace f
  {
    BOOL __stdcall
    win9x_CreateDirectoryW(_In_ LPCWSTR lpPathName,
                           _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes)
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

      return __ms_CreateDirectoryA(a_path_name.c_str(), lpSecurityAttributes);
    }
  } // namespace f
} // namespace mingw_thunk
