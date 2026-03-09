#include "GetFullPathNameW.h"

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
                 16,
                 DWORD,
                 WINAPI,
                 GetFullPathNameW,
                 _In_ LPCWSTR lpFileName,
                 _In_ DWORD nBufferLength,
                 _Out_ LPWSTR lpBuffer,
                 _Out_opt_ LPWSTR *lpFilePart)
  {
    __DISPATCH_THUNK_2(GetFullPathNameW,
                       i::is_nt(),
                       &__ms_GetFullPathNameW,
                       &f::win9x_GetFullPathNameW);

    return dllimport_GetFullPathNameW(
        lpFileName, nBufferLength, lpBuffer, lpFilePart);
  }

  namespace f
  {
    DWORD __stdcall win9x_GetFullPathNameW(_In_ LPCWSTR lpFileName,
                                           _In_ DWORD nBufferLength,
                                           _Out_ LPWSTR lpBuffer,
                                           _Out_opt_ LPWSTR *lpFilePart)
    {
      // dry run for buffer size
      if (nBufferLength && !lpBuffer) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
      }

      d::a_str a_name;
      if (!a_name.from_w(lpFileName))
        return 0;

      d::a_str a_buf{d::max_path_tag{}};
      char *a_file_part = nullptr;
      DWORD ret = __ms_GetFullPathNameA(
          a_name.c_str(), MAX_PATH, a_buf.data(), &a_file_part);
      if (ret == 0)
        return 0;

      while (ret > a_buf.size()) {
        if (!a_buf.resize(ret)) {
          SetLastError(ERROR_OUTOFMEMORY);
          return 0;
        }

        ret = __ms_GetFullPathNameA(
            a_name.c_str(), ret, a_buf.data(), &a_file_part);
        if (ret == 0)
          return 0;
      }

      size_t a_prefix_len = a_file_part - a_buf.data();

      d::w_str w_res;
      if (!w_res.from_a(a_buf.c_str(), ret)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return 0;
      }

      size_t w_size = w_res.size();
      if (w_size + 1 > nBufferLength) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return w_size + 1;
      }

      c::wmemcpy(lpBuffer, w_res.c_str(), w_size);
      lpBuffer[w_size] = 0;
      if (lpFilePart) {
        int w_prefix = d::w_str::size_from_a(a_buf.c_str(), a_prefix_len);
        *lpFilePart = lpBuffer + w_prefix;
      }
      return w_size;
    }
  } // namespace f
} // namespace mingw_thunk
