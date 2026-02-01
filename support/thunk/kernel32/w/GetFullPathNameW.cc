#include "GetFullPathNameW.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <nostl/string.h>

#include <nocrt/string.h>

#include <windows.h>

namespace mingw_thunk
{
  // Windows 95 (stub)
  // Windows NT 4.0
  __DEFINE_THUNK(kernel32,
                 16,
                 DWORD,
                 WINAPI,
                 GetFullPathNameW,
                 _In_ LPCWSTR lpFileName,
                 _In_ DWORD nBufferLength,
                 _Out_ LPWSTR lpBuffer,
                 _Out_opt_ LPWSTR *lpFilePart)
  {
    if (internal::is_nt())
      return __ms_GetFullPathNameW(
          lpFileName, nBufferLength, lpBuffer, lpFilePart);

    return impl::win9x_GetFullPathNameW(
        lpFileName, nBufferLength, lpBuffer, lpFilePart);
  }

  namespace impl
  {
    DWORD
    win9x_GetFullPathNameW(_In_ LPCWSTR lpFileName,
                           _In_ DWORD nBufferLength,
                           _Out_ LPWSTR lpBuffer,
                           _Out_opt_ LPWSTR *lpFilePart)
    {
      auto a_name = internal::w2a(lpFileName);
      size_t a_buf_len = nBufferLength > MAX_PATH ? nBufferLength : MAX_PATH;
      stl::string a_buf(a_buf_len, 0);
      char *a_file_part = nullptr;
      DWORD alen = __ms_GetFullPathNameA(
          a_name.c_str(), a_buf_len, a_buf.data(), &a_file_part);
      if (alen == 0) {
        return FALSE;
      }
      if (alen > a_buf_len) {
        a_buf.resize(alen);
        alen = __ms_GetFullPathNameA(
            a_name.c_str(), a_buf_len, a_buf.data(), &a_file_part);
      }
      size_t a_prefix_len = a_file_part - a_buf.data();
      a_buf.resize(alen); // shrink

      auto w_res = internal::a2w(a_buf.data(), alen);

      size_t len = w_res.length() + 1;
      if (len > nBufferLength)
        return len;

      if (lpFilePart) {
        auto w_prefix = internal::a2w(a_buf.data(), a_prefix_len);
        *lpFilePart = lpBuffer + w_prefix.length();
      }

      libc::wmemcpy(lpBuffer, w_res.data(), len);
      return w_res.length();
    }
  } // namespace impl
} // namespace mingw_thunk
