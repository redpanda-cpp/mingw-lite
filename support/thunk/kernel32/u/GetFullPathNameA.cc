#include <thunk/_common.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 16,
                 DWORD,
                 WINAPI,
                 GetFullPathNameA,
                 _In_ LPCSTR lpFileName,
                 _In_ DWORD nBufferLength,
                 _Out_ LPSTR lpBuffer,
                 _Out_opt_ LPSTR *lpFilePart)
  {
    // dry run for buffer size
    if (nBufferLength && !lpBuffer) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return 0;
    }

    d::w_str w_name;
    if (!w_name.from_u(lpFileName)) {
      SetLastError(ERROR_OUTOFMEMORY);
      return 0;
    }

    d::w_str w_buffer{d::max_path_tag{}};

    wchar_t *w_file_part = nullptr;

    DWORD ret = GetFullPathNameW(
        w_name.c_str(), MAX_PATH, w_buffer.data(), &w_file_part);
    if (ret == 0)
      return 0;

    while (ret > w_buffer.size()) {
      if (!w_buffer.resize(ret)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return 0;
      }

      ret =
          GetFullPathNameW(w_name.c_str(), ret, w_buffer.data(), &w_file_part);
      if (ret == 0)
        return 0;
    }

    int file_part_idx = w_file_part - w_buffer.data();

    d::u_str u_buffer;
    if (!u_buffer.from_w(w_buffer.data())) {
      SetLastError(ERROR_OUTOFMEMORY);
      return 0;
    }

    size_t u_size = u_buffer.size();
    if (u_size + 1 > nBufferLength) {
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      return u_size + 1;
    }

    c::memcpy(lpBuffer, u_buffer.data(), u_size);
    lpBuffer[u_size] = 0;
    if (lpFilePart) {
      int u_prefix = d::u_str::size_from_w(w_buffer.data(), file_part_idx);
      *lpFilePart = lpBuffer + u_prefix;
    }
    return u_size;
  }
} // namespace mingw_thunk
