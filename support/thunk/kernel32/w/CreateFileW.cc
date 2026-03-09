#include "CreateFileW.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  // Windows 95 (stub)
  // Windows NT 4.0
  __DEFINE_THUNK(kernel32,
                 28,
                 HANDLE,
                 WINAPI,
                 CreateFileW,
                 _In_ LPCWSTR lpFileName,
                 _In_ DWORD dwDesiredAccess,
                 _In_ DWORD dwShareMode,
                 _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                 _In_ DWORD dwCreationDisposition,
                 _In_ DWORD dwFlagsAndAttributes,
                 _In_opt_ HANDLE hTemplateFile)
  {
    __DISPATCH_THUNK_2(
        CreateFileW, i::is_nt(), &__ms_CreateFileW, &f::win9x_CreateFileW);

    return dllimport_CreateFileW(lpFileName,
                                 dwDesiredAccess,
                                 dwShareMode,
                                 lpSecurityAttributes,
                                 dwCreationDisposition,
                                 dwFlagsAndAttributes,
                                 hTemplateFile);
  }

  namespace f
  {
    HANDLE __stdcall
    win9x_CreateFileW(_In_ LPCWSTR lpFileName,
                      _In_ DWORD dwDesiredAccess,
                      _In_ DWORD dwShareMode,
                      _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                      _In_ DWORD dwCreationDisposition,
                      _In_ DWORD dwFlagsAndAttributes,
                      _In_opt_ HANDLE hTemplateFile)
    {
      if (!lpFileName) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return INVALID_HANDLE_VALUE;
      }

      // Windows 9x: flags not supported
      DWORD access = dwDesiredAccess & ~FILE_READ_ATTRIBUTES;
      DWORD mode = dwShareMode & ~FILE_SHARE_DELETE;

      d::a_str a_name;
      if (!a_name.from_w(lpFileName)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return INVALID_HANDLE_VALUE;
      }

      return __ms_CreateFileA(a_name.c_str(),
                              access,
                              mode,
                              lpSecurityAttributes,
                              dwCreationDisposition,
                              dwFlagsAndAttributes,
                              hTemplateFile);
    }
  } // namespace f
} // namespace mingw_thunk
