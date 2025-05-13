#include <thunk/_common.h>
#include <thunk/try_get.h>
#include <thunk/yy/yy.h>

#include <ntstatus.h>
#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 16,
                 DWORD,
                 WINAPI,
                 GetFinalPathNameByHandleW,
                 _In_ HANDLE hFile,
                 _Out_writes_(cchFilePath) LPWSTR lpszFilePath,
                 _In_ DWORD cchFilePath,
                 _In_ DWORD dwFlags)
  {
    if (const auto pfn = try_get_GetFinalPathNameByHandleW())
      return pfn(hFile, lpszFilePath, cchFilePath, dwFlags);

    // This function is widely used to implement 'realpath'.
    // Simply fail, let the caller try fallback methods.
    SetLastError(ERROR_NOT_SUPPORTED);
    return 0;
  }
} // namespace mingw_thunk
