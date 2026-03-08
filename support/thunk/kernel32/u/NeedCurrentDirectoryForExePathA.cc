#include <thunk/_common.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 4,
                 BOOL,
                 WINAPI,
                 NeedCurrentDirectoryForExePathA,
                 _In_ LPCSTR ExeName)
  {
    if (!ExeName) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    d::w_str w_exe_name;
    if (!w_exe_name.from_u(ExeName)) {
      SetLastError(ERROR_OUTOFMEMORY);
      return FALSE;
    }

    return NeedCurrentDirectoryForExePathW(w_exe_name.c_str());
  }
} // namespace mingw_thunk
