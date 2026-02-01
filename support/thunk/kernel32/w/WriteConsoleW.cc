#include "WriteConsoleW.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
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
      return __ms_WriteConsoleW(hConsoleOutput,
                                lpBuffer,
                                nNumberOfCharsToWrite,
                                lpNumberOfCharsWritten,
                                lpReserved);

    return impl::win9x_WriteConsoleW(hConsoleOutput,
                                     lpBuffer,
                                     nNumberOfCharsToWrite,
                                     lpNumberOfCharsWritten,
                                     lpReserved);
  }

  namespace impl
  {
    BOOL win9x_WriteConsoleW(_In_ HANDLE hConsoleOutput,
                             _In_ const VOID *lpBuffer,
                             _In_ DWORD nNumberOfCharsToWrite,
                             _Out_opt_ LPDWORD lpNumberOfCharsWritten,
                             _Reserved_ LPVOID lpReserved)
    {
      auto abuf =
          internal::w2a((const wchar_t *)lpBuffer, nNumberOfCharsToWrite);
      DWORD awritten;
      int ok = __ms_WriteConsoleA(
          hConsoleOutput, abuf.c_str(), abuf.length(), &awritten, lpReserved);
      if (lpNumberOfCharsWritten) {
        if (awritten == abuf.length())
          // fast path
          *lpNumberOfCharsWritten = nNumberOfCharsToWrite;
        else {
          auto wwritten = internal::a2w(abuf.c_str(), awritten);
          *lpNumberOfCharsWritten = wwritten.length();
        }
      }
      return ok;
    }
  } // namespace impl
} // namespace mingw_thunk
