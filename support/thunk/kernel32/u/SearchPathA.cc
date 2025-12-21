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
    stl::wstring w_path;
    if (lpPath)
      w_path = internal::u2w(lpPath);

    stl::wstring w_name = internal::u2w(lpFileName);

    stl::wstring w_extension;
    if (lpExtension)
      w_extension = internal::u2w(lpExtension);

    stl::wstring w_buffer(MAX_PATH, 0);
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
      w_buffer.resize(ret - 1);
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

    stl::string u_buffer = internal::w2u(w_buffer.data(), w_buffer.size());
    if (nBufferLength >= u_buffer.size() + 1) {
      libc::memcpy(lpBuffer, u_buffer.data(), u_buffer.size());
      lpBuffer[u_buffer.size()] = 0;
      if (lpFilePart) {
        stl::string u_prefix = internal::w2u(w_buffer.data(), file_part_idx);
        *lpFilePart = lpBuffer + u_prefix.size();
      }
      return u_buffer.size();
    } else {
      return u_buffer.size() + 1;
    }
  }
} // namespace mingw_thunk
