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
    __DISPATCH_THUNK_2(WriteConsoleW,
                       i::is_nt(),
                       &__ms_WriteConsoleW,
                       &f::win9x_WriteConsoleW);

    return dllimport_WriteConsoleW(hConsoleOutput,
                                   lpBuffer,
                                   nNumberOfCharsToWrite,
                                   lpNumberOfCharsWritten,
                                   lpReserved);
  }

  namespace f
  {
    BOOL __stdcall win9x_WriteConsoleW(_In_ HANDLE hConsoleOutput,
                                       _In_ const VOID *lpBuffer,
                                       _In_ DWORD nNumberOfCharsToWrite,
                                       _Out_opt_ LPDWORD lpNumberOfCharsWritten,
                                       _Reserved_ LPVOID lpReserved)
    {
      if (!lpBuffer) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
      }

      d::a_str abuf;
      if (!abuf.from_w((const wchar_t *)lpBuffer, nNumberOfCharsToWrite)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return FALSE;
      }

      DWORD a_written;
      int ok = __ms_WriteConsoleA(
          hConsoleOutput, abuf.c_str(), abuf.size(), &a_written, lpReserved);
      if (lpNumberOfCharsWritten) {
        if (a_written == abuf.size())
          // fast path
          *lpNumberOfCharsWritten = nNumberOfCharsToWrite;
        else {
          int w_written = d::w_str::size_from_a(abuf.c_str(), a_written);
          *lpNumberOfCharsWritten = w_written;
        }
      }
      return ok;
    }
  } // namespace f
} // namespace mingw_thunk
