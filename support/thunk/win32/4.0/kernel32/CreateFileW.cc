#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
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
      return get_CreateFileW()(lpFileName,
                               dwDesiredAccess,
                               dwShareMode,
                               lpSecurityAttributes,
                               dwCreationDisposition,
                               dwFlagsAndAttributes,
                               hTemplateFile);

    // Windows 9x: flags not supported
    DWORD access = dwDesiredAccess & ~FILE_READ_ATTRIBUTES;
    DWORD mode = dwShareMode & ~FILE_SHARE_DELETE;

    auto aname = internal::narrow(lpFileName);
    return CreateFileA(aname.c_str(),
                       access,
                       mode,
                       lpSecurityAttributes,
                       dwCreationDisposition,
                       dwFlagsAndAttributes,
                       hTemplateFile);
  }
} // namespace mingw_thunk
