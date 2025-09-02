#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>
#include <thunk/win32_find_data.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 BOOL,
                 WINAPI,
                 FindNextFileW,
                 _In_ HANDLE hFindFile,
                 _Out_ LPWIN32_FIND_DATAW lpFindFileData)
  {
    if (internal::is_nt())
      return get_FindNextFileW()(hFindFile, lpFindFileData);

    WIN32_FIND_DATAA a_find_data;
    BOOL result = FindNextFileA(hFindFile, &a_find_data);
    if (!result)
      return FALSE;

    *lpFindFileData = internal::widen(a_find_data);
    return result;
  }
} // namespace mingw_thunk
