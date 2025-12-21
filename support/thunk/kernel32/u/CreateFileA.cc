#include <thunk/_common.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 28,
                 HANDLE,
                 WINAPI,
                 CreateFileA,
                 _In_ LPCSTR lpFileName,
                 _In_ DWORD dwDesiredAccess,
                 _In_ DWORD dwShareMode,
                 _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                 _In_ DWORD dwCreationDisposition,
                 _In_ DWORD dwFlagsAndAttributes,
                 _In_opt_ HANDLE hTemplateFile)
  {
    stl::wstring w_name = internal::u2w(lpFileName);

    return CreateFileW(w_name.c_str(),
                       dwDesiredAccess,
                       dwShareMode,
                       lpSecurityAttributes,
                       dwCreationDisposition,
                       dwFlagsAndAttributes,
                       hTemplateFile);
  }
} // namespace mingw_thunk
