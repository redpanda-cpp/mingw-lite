#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>
#include <thunk/win32_find_data.h>

#include <windows.h>

namespace mingw_thunk
{
  // Windows 95 (stub)
  // Windows NT 4.0
  __DEFINE_THUNK(kernel32,
                 8,
                 HANDLE,
                 WINAPI,
                 FindFirstFileW,
                 _In_ LPCWSTR lpFileName,
                 _Out_ LPWIN32_FIND_DATAW lpFindFileData)
  {
    if (internal::is_nt())
      return __ms_FindFirstFileW(lpFileName, lpFindFileData);

    auto a_name = internal::w2a(lpFileName);
    WIN32_FIND_DATAA a_find_data;
    HANDLE h = __ms_FindFirstFileA(a_name.c_str(), &a_find_data);
    if (h == INVALID_HANDLE_VALUE)
      return h;

    *lpFindFileData = internal::a2w(a_find_data);
    return h;
  }
} // namespace mingw_thunk
