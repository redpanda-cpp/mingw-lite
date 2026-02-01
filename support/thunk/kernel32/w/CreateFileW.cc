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
    if (internal::is_nt())
      return __ms_CreateFileW(lpFileName,
                              dwDesiredAccess,
                              dwShareMode,
                              lpSecurityAttributes,
                              dwCreationDisposition,
                              dwFlagsAndAttributes,
                              hTemplateFile);

    return impl::win9x_CreateFileW(lpFileName,
                                   dwDesiredAccess,
                                   dwShareMode,
                                   lpSecurityAttributes,
                                   dwCreationDisposition,
                                   dwFlagsAndAttributes,
                                   hTemplateFile);
  }

  namespace impl
  {
    HANDLE
    win9x_CreateFileW(_In_ LPCWSTR lpFileName,
                      _In_ DWORD dwDesiredAccess,
                      _In_ DWORD dwShareMode,
                      _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                      _In_ DWORD dwCreationDisposition,
                      _In_ DWORD dwFlagsAndAttributes,
                      _In_opt_ HANDLE hTemplateFile)
    {
      // Windows 9x: flags not supported
      DWORD access = dwDesiredAccess & ~FILE_READ_ATTRIBUTES;
      DWORD mode = dwShareMode & ~FILE_SHARE_DELETE;

      auto a_name = internal::w2a(lpFileName);
      return __ms_CreateFileA(a_name.c_str(),
                              access,
                              mode,
                              lpSecurityAttributes,
                              dwCreationDisposition,
                              dwFlagsAndAttributes,
                              hTemplateFile);
    }
  } // namespace impl
} // namespace mingw_thunk
