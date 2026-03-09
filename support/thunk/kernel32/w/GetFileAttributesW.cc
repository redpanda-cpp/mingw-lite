#include "GetFileAttributesW.h"

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
      kernel32, 4, DWORD, WINAPI, GetFileAttributesW, _In_ LPCWSTR lpFileName)
  {
    __DISPATCH_THUNK_2(GetFileAttributesW,
                       i::is_nt(),
                       &__ms_GetFileAttributesW,
                       &f::win9x_GetFileAttributesW);

    return dllimport_GetFileAttributesW(lpFileName);
  }

  namespace f
  {
    DWORD __stdcall win9x_GetFileAttributesW(_In_ LPCWSTR lpFileName)
    {
      if (!lpFileName) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return INVALID_FILE_ATTRIBUTES;
      }

      d::a_str aname;
      if (!aname.from_w(lpFileName)) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return INVALID_FILE_ATTRIBUTES;
      }

      return __ms_GetFileAttributesA(aname.c_str());
    }
  } // namespace f
} // namespace mingw_thunk
