#include "SearchPathW.h"

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
    __DISPATCH_THUNK_2(
        SearchPathW, i::is_nt(), &__ms_SearchPathW, &f::win9x_SearchPathW);

    return dllimport_SearchPathW(
        lpPath, lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);
  }

  namespace f
  {
    DWORD __stdcall win9x_SearchPathW(_In_opt_ LPCWSTR lpPath,
                                      _In_ LPCWSTR lpFileName,
                                      _In_opt_ LPCWSTR lpExtension,
                                      _In_ DWORD nBufferLength,
                                      _Out_ LPWSTR lpBuffer,
                                      _Out_opt_ LPWSTR *lpFilePart)
    {
      if (!lpFileName) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
      }

      // dry run for buffer size
      if (nBufferLength && !lpBuffer) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
      }

      d::a_str a_path;
      if (lpPath && !a_path.from_w(lpPath)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return 0;
      }

      d::a_str a_name;
      if (!a_name.from_w(lpFileName)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return 0;
      }

      d::a_str a_extension;
      if (lpExtension && !a_extension.from_w(lpExtension)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return 0;
      }

      d::a_str a_buf{d::max_path_tag{}};
      char *a_file_part = nullptr;

      DWORD ret = __ms_SearchPathA(lpPath ? a_path.c_str() : nullptr,
                                   a_name.c_str(),
                                   lpExtension ? a_extension.c_str() : nullptr,
                                   MAX_PATH,
                                   a_buf.data(),
                                   &a_file_part);
      if (ret == 0)
        return FALSE;

      while (ret > a_buf.size()) {
        if (!a_buf.resize(ret)) {
          SetLastError(ERROR_OUTOFMEMORY);
          return 0;
        }
        ret = __ms_SearchPathA(lpPath ? a_path.c_str() : nullptr,
                               a_name.c_str(),
                               lpExtension ? a_extension.c_str() : nullptr,
                               ret,
                               a_buf.data(),
                               &a_file_part);
        if (ret == 0)
          return 0;
      }

      size_t file_part_idx = a_file_part - a_buf.data();

      d::w_str w_buffer;
      if (!w_buffer.from_a(a_buf.c_str(), ret)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return 0;
      }

      size_t w_size = w_buffer.size();
      if (w_size + 1 > nBufferLength) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return w_size + 1;
      }

      c::wmemcpy(lpBuffer, w_buffer.c_str(), w_size);
      lpBuffer[w_size] = 0;
      if (lpFilePart) {
        int w_prefix = d::w_str::size_from_a(a_buf.c_str(), file_part_idx);
        *lpFilePart = lpBuffer + w_prefix;
      }
      return w_size;
    }
  } // namespace f
} // namespace mingw_thunk
