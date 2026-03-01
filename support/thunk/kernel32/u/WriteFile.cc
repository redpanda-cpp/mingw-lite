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

    stl::wstring w_buffer =
        internal::u2w((const char *)lpBuffer, nNumberOfBytesToWrite);
    DWORD w_written = 0;
    int ok = WriteConsoleW(
        hFile, w_buffer.c_str(), w_buffer.size(), &w_written, NULL);
    if (lpNumberOfBytesWritten) {
      if (w_written == w_buffer.size())
        // fast path
        *lpNumberOfBytesWritten = nNumberOfBytesToWrite;
      else {
        stl::string a_written = internal::w2u(w_buffer.c_str(), w_written);
        *lpNumberOfBytesWritten = a_written.size();
      }
    }
    return ok;
  }
} // namespace mingw_thunk
