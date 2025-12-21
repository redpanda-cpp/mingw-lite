#include "SearchPathW.h"

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
                 24,
                 DWORD,
                 WINAPI,
                 SearchPathW,
                 _In_opt_ LPCWSTR lpPath,
                 _In_ LPCWSTR lpFileName,
                 _In_opt_ LPCWSTR lpExtension,
                 _In_ DWORD nBufferLength,
                 _Out_ LPWSTR lpBuffer,
                 _Out_opt_ LPWSTR *lpFilePart)
  {
    if (internal::is_nt())
      return __ms_SearchPathW(
          lpPath, lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);

    return impl::win9x_SearchPathW(
        lpPath, lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);
  }

  namespace impl
  {
    DWORD
    win9x_SearchPathW(_In_opt_ LPCWSTR lpPath,
                      _In_ LPCWSTR lpFileName,
                      _In_opt_ LPCWSTR lpExtension,
                      _In_ DWORD nBufferLength,
                      _Out_ LPWSTR lpBuffer,
                      _Out_opt_ LPWSTR *lpFilePart)
    {
      stl::string a_path;
      if (lpPath)
        a_path = internal::w2a(lpPath);

      stl::string a_name = internal::w2a(lpFileName);

      stl::string a_extension;
      if (lpExtension)
        a_extension = internal::w2a(lpExtension);

      stl::string a_buf(MAX_PATH, 0);
      char *a_file_part = nullptr;

      DWORD ret = __ms_GetFullPathNameA(
          a_name.c_str(), MAX_PATH, a_buf.data(), &a_file_part);
      if (ret == 0) {
        return FALSE;
      }
      size_t file_part_idx = a_file_part - a_buf.data();

      // Windows 9x: path cannot be longer than MAX_PATH
      auto w_buffer = internal::a2w(a_buf.data(), ret);
      if (nBufferLength >= w_buffer.size() + 1) {
        libc::memcpy(lpBuffer, w_buffer.data(), w_buffer.size());
        lpBuffer[w_buffer.size()] = 0;
        if (lpFilePart) {
          stl::wstring w_prefix = internal::a2w(a_buf.data(), file_part_idx);
          *lpFilePart = lpBuffer + w_prefix.length();
        }
        return w_buffer.size();
      } else {
        return w_buffer.size() + 1;
      }
    }
  } // namespace impl
} // namespace mingw_thunk
