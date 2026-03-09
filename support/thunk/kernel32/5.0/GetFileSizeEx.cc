#include "GetFileSizeEx.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL WINAPI fallback_GetFileSizeEx(_In_ HANDLE hFile,
                                       _Out_ PLARGE_INTEGER lpFileSize);
  }

  __DEFINE_THUNK(kernel32,
                 8,
                 BOOL,
                 WINAPI,
                 GetFileSizeEx,
                 _In_ HANDLE hFile,
                 _Out_ PLARGE_INTEGER lpFileSize)
  {
    __DISPATCH_THUNK_2(GetFileSizeEx,
                       const auto pfn = try_get_GetFileSizeEx(),
                       pfn,
                       &f::fallback_GetFileSizeEx);

    return dllimport_GetFileSizeEx(hFile, lpFileSize);
  }

  namespace f
  {
    BOOL WINAPI fallback_GetFileSizeEx(_In_ HANDLE hFile,
                                       _Out_ PLARGE_INTEGER lpFileSize)
    {
      DWORD low, high;
      low = GetFileSize(hFile, &high);

      if (low == INVALID_FILE_SIZE && GetLastError())
        return FALSE;

      lpFileSize->LowPart = low;
      lpFileSize->HighPart = high;

      return TRUE;
    }
  } // namespace f
} // namespace mingw_thunk
