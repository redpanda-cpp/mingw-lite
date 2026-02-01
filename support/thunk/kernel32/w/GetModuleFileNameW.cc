#include "GetModuleFileNameW.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <errhandlingapi.h>
#include <libloaderapi.h>

#include <nocrt/__wchar/wmemcpy.h>

namespace mingw_thunk
{
  // Windows 95 (stub)
  // Windows NT 4.0
  __DEFINE_THUNK(kernel32,
                 12,
                 DWORD,
                 WINAPI,
                 GetModuleFileNameW,
                 _In_opt_ HMODULE hModule,
                 _Out_ LPWSTR lpFilename,
                 _In_ DWORD nSize)
  {
    if (internal::is_nt())
      return __ms_GetModuleFileNameW(hModule, lpFilename, nSize);

    return impl::win9x_GetModuleFileNameW(hModule, lpFilename, nSize);
  }

  namespace impl
  {
    DWORD
    win9x_GetModuleFileNameW(_In_opt_ HMODULE hModule,
                             _Out_ LPWSTR lpFilename,
                             _In_ DWORD nSize)
    {
      char a_filename[MAX_PATH];
      DWORD a_size = __ms_GetModuleFileNameA(hModule, a_filename, MAX_PATH);
      if (a_size == 0)
        return 0;
      stl::wstring w_filename = internal::a2w(a_filename);
      if (nSize >= w_filename.size() + 1) {
        libc::wmemcpy(lpFilename, w_filename.c_str(), w_filename.size());
        lpFilename[w_filename.size()] = L'\0';
        return w_filename.size();
      } else {
        libc::wmemcpy(lpFilename, w_filename.c_str(), nSize - 1);
        lpFilename[nSize - 1] = L'\0';
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return nSize;
      }
    }
  } // namespace impl
} // namespace mingw_thunk
