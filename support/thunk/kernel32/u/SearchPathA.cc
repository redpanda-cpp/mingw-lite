#include <thunk/_common.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 24,
                 DWORD,
                 WINAPI,
                 SearchPathA,
                 _In_opt_ LPCSTR lpPath,
                 _In_ LPCSTR lpFileName,
                 _In_opt_ LPCSTR lpExtension,
                 _In_ DWORD nBufferLength,
                 _Out_ LPSTR lpBuffer,
                 _Out_opt_ LPSTR *lpFilePart)

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

    d::w_str w_path;
    if (lpPath && !w_path.from_u(lpPath)) {
      SetLastError(ERROR_OUTOFMEMORY);
      return 0;
    }

    d::w_str w_name;
    if (!w_name.from_u(lpFileName)) {
      SetLastError(ERROR_OUTOFMEMORY);
      return 0;
    }

    d::w_str w_extension;
    if (lpExtension && !w_extension.from_u(lpExtension)) {
      SetLastError(ERROR_OUTOFMEMORY);
      return 0;
    }

    d::w_str w_buffer{d::max_path_tag{}};
    wchar_t *w_file_part;

    DWORD ret = SearchPathW(lpPath ? w_path.c_str() : nullptr,
                            w_name.c_str(),
                            lpExtension ? w_extension.c_str() : nullptr,
                            MAX_PATH,
                            w_buffer.data(),
                            &w_file_part);
    if (ret == 0)
      return 0;
    size_t file_part_idx = w_file_part - w_buffer.data();

    if (ret > MAX_PATH) {
      if (!w_buffer.resize(ret - 1)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return 0;
      }
      SearchPathW(lpPath ? w_path.c_str() : nullptr,
                  w_name.c_str(),
                  lpExtension ? w_extension.c_str() : nullptr,
                  ret,
                  w_buffer.data(),
                  &w_file_part);
      file_part_idx = w_file_part - w_buffer.data();
    } else {
      w_buffer.resize(ret);
    }

    d::u_str u_buffer;
    if (!u_buffer.from_w(w_buffer.data(), w_buffer.size())) {
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
