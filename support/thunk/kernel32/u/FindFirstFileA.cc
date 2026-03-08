#include <thunk/_common.h>
#include <thunk/string.h>
#include <thunk/win32_find_data.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 HANDLE,
                 WINAPI,
                 FindFirstFileA,
                 _In_ LPCSTR lpFileName,
                 _Out_ LPWIN32_FIND_DATAA lpFindFileData)
  {
    if (!lpFileName || !lpFindFileData) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return INVALID_HANDLE_VALUE;
    }

    d::w_str w_name;
    if (!w_name.from_u(lpFileName)) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return INVALID_HANDLE_VALUE;
    }

    WIN32_FIND_DATAW w_find_data;
    HANDLE h = FindFirstFileW(w_name.c_str(), &w_find_data);
    if (h == INVALID_HANDLE_VALUE)
      return h;

    *lpFindFileData = internal::w2u(w_find_data);
    return h;
  }
} // namespace mingw_thunk
