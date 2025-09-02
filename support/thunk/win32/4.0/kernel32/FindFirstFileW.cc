#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>
#include <thunk/win32_find_data.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 HANDLE,
                 WINAPI,
                 FindFirstFileW,
                 _In_ LPCWSTR lpFileName,
                 _Out_ LPWIN32_FIND_DATAW lpFindFileData)
  {
    if (internal::is_nt())
      return get_FindFirstFileW()(lpFileName, lpFindFileData);

    auto a_name = internal::narrow(lpFileName);
    WIN32_FIND_DATAA a_find_data;
    HANDLE h = FindFirstFileA(a_name.c_str(), &a_find_data);
    if (h == INVALID_HANDLE_VALUE)
      return h;

    *lpFindFileData = internal::widen(a_find_data);
    return h;
  }
} // namespace mingw_thunk
