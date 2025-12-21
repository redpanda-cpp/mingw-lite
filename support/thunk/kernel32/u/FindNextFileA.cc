#include <thunk/_common.h>
#include <thunk/string.h>
#include <thunk/win32_find_data.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 BOOL,
                 WINAPI,
                 FindNextFileA,
                 _In_ HANDLE hFindFile,
                 _Out_ LPWIN32_FIND_DATAA lpFindFileData)
  {
    WIN32_FIND_DATAW w_find_data;
    BOOL result = FindNextFileW(hFindFile, &w_find_data);
    if (!result)
      return FALSE;

    *lpFindFileData = internal::w2u(w_find_data);
    return result;
  }
} // namespace mingw_thunk
