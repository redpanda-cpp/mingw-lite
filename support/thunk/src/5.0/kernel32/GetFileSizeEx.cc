#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 BOOL,
                 WINAPI,
                 GetFileSizeEx,
                 _In_ HANDLE hFile,
                 _Out_ PLARGE_INTEGER lpFileSize)
  {
    if (const auto pfn = try_get_GetFileSizeEx())
      return pfn(hFile, lpFileSize);

    DWORD low, high;
    low = GetFileSize(hFile, &high);

    if (low == INVALID_FILE_SIZE && GetLastError())
      return FALSE;

    lpFileSize->LowPart = low;
    lpFileSize->HighPart = high;

    return TRUE;
  }
} // namespace mingw_thunk
