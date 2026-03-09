#include "FindNextFileW.h"

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
    __DISPATCH_THUNK_2(FindNextFileW,
                       i::is_nt(),
                       &__ms_FindNextFileW,
                       &f::win9x_FindNextFileW);

    return dllimport_FindNextFileW(hFindFile, lpFindFileData);
  }

  namespace f
  {
    BOOL __stdcall win9x_FindNextFileW(_In_ HANDLE hFindFile,
                                       _Out_ LPWIN32_FIND_DATAW lpFindFileData)
    {
      WIN32_FIND_DATAA a_find_data;
      BOOL result = __ms_FindNextFileA(hFindFile, &a_find_data);
      if (!result)
        return FALSE;

      *lpFindFileData = internal::a2w(a_find_data);
      return result;
    }
  } // namespace f
} // namespace mingw_thunk