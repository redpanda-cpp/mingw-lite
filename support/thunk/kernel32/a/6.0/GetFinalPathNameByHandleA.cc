#include <thunk/_common.h>

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
    if (const auto pfn = try_get_GetFinalPathNameByHandleA())
      return pfn(hFile, lpszFilePath, cchFilePath, dwFlags);

    // This function is widely used to implement 'realpath'.
    // Simply fail, let the caller try fallback methods.
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
  }
} // namespace mingw_thunk
