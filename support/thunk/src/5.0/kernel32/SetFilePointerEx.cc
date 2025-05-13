#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
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
    if (const auto pfn = try_get_SetFilePointerEx())
      return pfn(hFile, liDistanceToMove, lpNewFilePointer, dwMoveMethod);

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
} // namespace mingw_thunk
