#include <thunk/_common.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      kernel32, 4, DWORD, WINAPI, GetFileAttributesA, _In_ LPCSTR lpFileName)
  {
    if (!lpFileName) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return INVALID_FILE_ATTRIBUTES;
    }

    d::w_str w_file_name;
    if (!w_file_name.from_u(lpFileName)) {
      SetLastError(ERROR_OUTOFMEMORY);
      return INVALID_FILE_ATTRIBUTES;
    }

    return GetFileAttributesW(w_file_name.c_str());
  }
} // namespace mingw_thunk
