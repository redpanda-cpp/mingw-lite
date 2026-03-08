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
    if (internal::is_nt())
      return __ms_DeleteFileW(lpFileName);

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
} // namespace mingw_thunk
