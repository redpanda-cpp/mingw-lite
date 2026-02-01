#include "GetTempPathW.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <nostl/string.h>

#include <windows.h>

namespace mingw_thunk
{
  // Windows 95 (stub)
  // Windows NT 4.0
  __DEFINE_THUNK(kernel32,
                 8,
                 DWORD,
                 WINAPI,
                 GetTempPathW,
                 _In_ DWORD nBufferLength,
                 _Out_ LPWSTR lpBuffer)
  {
    if (internal::is_nt())
      return __ms_GetTempPathW(nBufferLength, lpBuffer);

    return impl::win9x_GetTempPathW(nBufferLength, lpBuffer);
  }

  namespace impl
  {
    DWORD
    win9x_GetTempPathW(_In_ DWORD nBufferLength, _Out_ LPWSTR lpBuffer)
    {
      // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-gettemppathw
      // The maximum possible return value is MAX_PATH+1 (261).
      char a_buffer[MAX_PATH + 1];
      DWORD size = __ms_GetTempPathA(MAX_PATH + 1, a_buffer);
      if (size == 0)
        return 0;

      stl::wstring w_str = internal::a2w(a_buffer, size);
      if (nBufferLength >= w_str.size() + 1) {
        libc::wmemcpy(lpBuffer, w_str.c_str(), w_str.size());
        lpBuffer[w_str.size()] = L'\0';
        return w_str.size();
      } else {
        return w_str.size() + 1;
      }
    }
  } // namespace impl
} // namespace mingw_thunk
