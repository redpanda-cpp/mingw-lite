#include <thunk/_common.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 16,
                 DWORD,
                 WINAPI,
                 GetFinalPathNameByHandleA,
                 _In_ HANDLE hFile,
                 _Out_ LPSTR lpszFilePath,
                 _In_ DWORD cchFilePath,
                 _In_ DWORD dwFlags)
  {
    stl::wstring w_buffer(MAX_PATH, 0);

    DWORD ret =
        GetFinalPathNameByHandleW(hFile, w_buffer.data(), MAX_PATH, dwFlags);
    if (ret == 0)
      return 0;

    if (ret > MAX_PATH) {
      w_buffer.resize(ret - 1);
      GetFinalPathNameByHandleW(hFile, w_buffer.data(), ret, dwFlags);
    } else {
      w_buffer.resize(ret);
    }

    stl::string u_buffer = internal::w2u(w_buffer.data(), w_buffer.size());
    if (cchFilePath >= u_buffer.size() + 1) {
      libc::memcpy(lpszFilePath, u_buffer.data(), u_buffer.size());
      lpszFilePath[u_buffer.size()] = 0;
      return u_buffer.size();
    } else {
      return u_buffer.size() + 1;
    }
  }
} // namespace mingw_thunk
