#include "SetFilePointerEx.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL WINAPI
    fallback_SetFilePointerEx(_In_ HANDLE hFile,
                              _In_ LARGE_INTEGER liDistanceToMove,
                              _Out_opt_ PLARGE_INTEGER lpNewFilePointer,
                              _In_ DWORD dwMoveMethod);
  }

  __DEFINE_THUNK(kernel32,
                 20,
                 BOOL,
                 WINAPI,
                 SetFilePointerEx,
                 _In_ HANDLE hFile,
                 _In_ LARGE_INTEGER liDistanceToMove,
                 _Out_opt_ PLARGE_INTEGER lpNewFilePointer,
                 _In_ DWORD dwMoveMethod)
  {
    __DISPATCH_THUNK_2(SetFilePointerEx,
                       const auto pfn = try_get_SetFilePointerEx(),
                       pfn,
                       &f::fallback_SetFilePointerEx);

    return dllimport_SetFilePointerEx(
        hFile, liDistanceToMove, lpNewFilePointer, dwMoveMethod);
  }

  namespace f
  {
    BOOL WINAPI
    fallback_SetFilePointerEx(_In_ HANDLE hFile,
                              _In_ LARGE_INTEGER liDistanceToMove,
                              _Out_opt_ PLARGE_INTEGER lpNewFilePointer,
                              _In_ DWORD dwMoveMethod)
    {
      LONG high = liDistanceToMove.HighPart;
      DWORD low =
          SetFilePointer(hFile, liDistanceToMove.LowPart, &high, dwMoveMethod);

      if (low == INVALID_SET_FILE_POINTER && GetLastError())
        return FALSE;

      if (lpNewFilePointer) {
        lpNewFilePointer->LowPart = low;
        lpNewFilePointer->HighPart = high;
      }

      return TRUE;
    }
  } // namespace f
} // namespace mingw_thunk
