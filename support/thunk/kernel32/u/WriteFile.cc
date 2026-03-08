#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 20,
                 BOOL,
                 WINAPI,
                 WriteFile,
                 _In_ HANDLE hFile,
                 _In_ LPCVOID lpBuffer,
                 _In_ DWORD nNumberOfBytesToWrite,
                 _Out_opt_ LPDWORD lpNumberOfBytesWritten,
                 _Inout_opt_ LPOVERLAPPED lpOverlapped)
  {
    DWORD mode;
    if (!GetConsoleMode(hFile, &mode))
      return __ms_WriteFile(hFile,
                            lpBuffer,
                            nNumberOfBytesToWrite,
                            lpNumberOfBytesWritten,
                            lpOverlapped);

    if (!lpBuffer) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    d::w_str w_buffer;
    if (!w_buffer.from_a(static_cast<const char *>(lpBuffer),
                         nNumberOfBytesToWrite)) {
      SetLastError(ERROR_OUTOFMEMORY);
      return FALSE;
    }

    DWORD w_written = 0;
    int ok = WriteConsoleW(
        hFile, w_buffer.c_str(), w_buffer.size(), &w_written, nullptr);
    if (lpNumberOfBytesWritten) {
      if (w_written == w_buffer.size())
        // fast path
        *lpNumberOfBytesWritten = nNumberOfBytesToWrite;
      else {
        d::u_str u_written;
        if (!u_written.from_w(w_buffer.c_str())) {
          SetLastError(ERROR_OUTOFMEMORY);
          return FALSE;
        }
        *lpNumberOfBytesWritten = u_written.size();
      }
    }
    return ok;
  }
} // namespace mingw_thunk
