#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 20,
                 BOOL,
                 WINAPI,
                 WriteConsoleW,
                 _In_ HANDLE hConsoleOutput,
                 _In_ const VOID *lpBuffer,
                 _In_ DWORD nNumberOfCharsToWrite,
                 _Out_opt_ LPDWORD lpNumberOfCharsWritten,
                 _Reserved_ LPVOID lpReserved)
  {
    if (internal::is_nt())
      return get_WriteConsoleW()(hConsoleOutput,
                                 lpBuffer,
                                 nNumberOfCharsToWrite,
                                 lpNumberOfCharsWritten,
                                 lpReserved);

    auto abuf =
        internal::narrow((const wchar_t *)lpBuffer, nNumberOfCharsToWrite);
    DWORD awritten;
    int ok = WriteConsoleA(
        hConsoleOutput, abuf.c_str(), abuf.length(), &awritten, lpReserved);
    if (lpNumberOfCharsWritten) {
      if (awritten == abuf.length())
        // fast path
        *lpNumberOfCharsWritten = nNumberOfCharsToWrite;
      else {
        auto wwritten = internal::widen(abuf.c_str(), awritten);
        *lpNumberOfCharsWritten = wwritten.length();
      }
    }
    return ok;
  }
} // namespace mingw_thunk
