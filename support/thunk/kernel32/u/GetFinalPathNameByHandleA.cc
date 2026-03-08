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
    // dry run for buffer size
    if (cchFilePath && !lpszFilePath) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return 0;
    }

    d::w_str w_buffer{d::max_path_tag{}};

    DWORD ret =
        GetFinalPathNameByHandleW(hFile, w_buffer.data(), MAX_PATH, dwFlags);
    if (ret == 0)
      return 0;

    if (ret > MAX_PATH) {
      if (!w_buffer.resize(ret - 1)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return 0;
      }
      GetFinalPathNameByHandleW(hFile, w_buffer.data(), ret, dwFlags);
    } else {
      // Windows Server 2008 and Windows Vista: For the ANSI version of this
      // function, GetFinalPathNameByHandleA, the return value includes the size
      // of the terminating null character.
      if (w_buffer[ret - 1] == 0)
        ret = ret - 1;
      w_buffer.resize(ret);
    }

    d::u_str u_buffer;
    if (!u_buffer.from_w(w_buffer.data(), w_buffer.size())) {
      SetLastError(ERROR_OUTOFMEMORY);
      return 0;
    }

    if (cchFilePath >= u_buffer.size() + 1) {
      c::memcpy(lpszFilePath, u_buffer.data(), u_buffer.size());
      lpszFilePath[u_buffer.size()] = 0;
      return u_buffer.size();
    } else {
      return u_buffer.size() + 1;
    }
  }
} // namespace mingw_thunk
