#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
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
      return __ms_FindNextFileW(hFindFile, lpFindFileData);

    WIN32_FIND_DATAA a_find_data;
    BOOL result = __ms_FindNextFileA(hFindFile, &a_find_data);
    if (!result)
      return FALSE;

    *lpFindFileData = internal::a2w(a_find_data);
    return result;
  }
} // namespace mingw_thunk
